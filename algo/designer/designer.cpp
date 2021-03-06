#include "designer.h"
#include "verifier.h"
#include "dataFlow.h"
#include "virtualLink.h"
#include "netelement.h"
#include "partition.h"
#include "virtualLinkConfigurator.h"
#include "virtualLinksAggregator.h"
#include "operations.h"
#include "routing.h"
#include "limitedSearcher.h"
#include <stdio.h>
#include <algorithm>
#include "network.h"
#include "link.h"

Designer::~Designer() {
    delete responseTimeEstimator;

    VirtualLinks:: iterator it = designedVirtualLinks.begin();
    for ( ; it != designedVirtualLinks.end(); ++it ) {
        delete *it;
    }
}

void Designer::design() {
    if ( Verifier::verify(network, existingVirtualLinks) != "Verified successfully" ) {
        printf("Assigned virtual links violates some constraints. %s",
                "Please redesign preassigned virtual links correctly before running algorithm.\n");
        return;
    }

    designUnroutedVLs();
    checkOutgoingVirtualLinks();
    routeVirtualLinks();
    checkOutgoingVirtualLinks(false); // checking once again (for the cases when there are more then one port for end system)
    calculateAndCheckResponseTimeouts(designedVirtualLinks);
}

void assignVLToEndSystem(VirtualLink* vl, NetElement* endSystem) {
    // we can assign vl to endSystem only if it has only 1 port
    if ( endSystem->getPorts().size() == 1 ) {
        endSystem->assignOutgoingVirtualLink(vl, *endSystem->getPorts().begin());
    }
}

void Designer::setDataFlowVL(VirtualLink* vl, DataFlow* df) {
    vl->assign(df);
    designedVirtualLinks.insert(vl);
    NetElement* es = df->getFrom()->getConnected();

    Partitions& dests = df->getTo();
    for ( Partitions::iterator pit = dests.begin(); pit != dests.end(); ++pit ) {
        vl->addDestination((*pit)->getConnected());
    }

    if ( vl->getSource() != es ) { // not assigned yet
        vl->setSource(es);
        assignVLToEndSystem(vl, es);
    }
}

void Designer::designUnroutedVLs() {
    DataFlows::iterator it = dataFlows.begin();
    for ( ; it != dataFlows.end(); ++it ) {
        DataFlow* df = *it;
        if ( df->getVirtualLink() == 0 ) {
            VirtualLink* vl = VirtualLinkConfigurator::designUnroutedVL(df);
            // Check whether all constraints are satisfied

            if ( vl != 0 ) {
                setDataFlowVL(vl, df);
            }
        } else {
            printf("Data flow already has assigned virtual link.\n");
        }
    }
}

void Designer::checkOutgoingVirtualLinks(bool checkCapacity) {
    NetElements endSystems = Operations::getEndSystems(network);
    NetElements::iterator it = endSystems.begin();
    for ( ; it != endSystems.end(); ++it )
        checkOutgoingVirtualLinks(*it, checkCapacity);
}

void Designer::checkOutgoingVirtualLinks(NetElement* endSystem, bool checkCapacity) {
    Ports::iterator it = endSystem->getPorts().begin();
    for ( ; it != endSystem->getPorts().end(); ++it ) {
        Verifier::FailedConstraint failed = Verifier::verifyOutgoingVirtualLinks(*it, checkCapacity);
        while ( failed != Verifier::NONE ) {
           redesignOutgoingVirtualLinks(*it, failed);
           failed= Verifier::verifyOutgoingVirtualLinks(*it, checkCapacity);
        }
    }
}

// Prefer elements with less number of data flows
bool comparerCapacity(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getBandwidth() / vl1->getAssignments().size() > vl2->getBandwidth() / vl2->getAssignments().size();
}

bool comparerLMax(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getLMax() / vl1->getAssignments().size() > vl2->getLMax() / vl2->getAssignments().size();
}

void filterVls(VirtualLinks& vls) {
    VirtualLinks copy(vls.begin(), vls.end());
    VirtualLinks::iterator it = copy.begin();
    for( ; it != copy.end(); ++it ) {
        if ( (*it)->getAssignments().size() == 0 )
            vls.erase(*it);
    }
}

void Designer::redesignOutgoingVirtualLinks(Port* port, Verifier::FailedConstraint failed) {
    assert(failed != Verifier::NONE);
    VirtualLinks vls = VirtualLinks(port->getAssignedLowPriority().begin(), port->getAssignedLowPriority().end());

    if ( disableAggregationOnSource ) {
       VirtualLink* vlToDrop = *std::min_element(vls.begin(), vls.end(),
            failed == Verifier::CAPACITY ? comparerCapacity : comparerLMax);

       removeMostConstrainedVirtualLink(port, vlToDrop, failed);
       return;
    }

    // do not remove already existing vls
    filterVls(vls);
    if ( failed == Verifier::CAPACITY )
        printf("Capacity is overloaded, trying to aggregate\n");
    else
        printf("Jmax is overloaded, trying to aggregate\n");

    VirtualLink *first, *second;
    bool fail = false;
    VirtualLinksAggregator::DeprecatedVLPairs deprecated;

    while ( !fail ) {
        first = 0;
        second = 0;
        VirtualLinksAggregator::selectVLsForAggregation(port->getParent(), &first, &second, failed, deprecated);
        if ( first != 0 && second != 0 ) {
            VirtualLink* vl = VirtualLinksAggregator::performAggregation(first, second);
            if ( vl != 0 ) {
                if ( vl->getBandwidth() >= (first->getBandwidth() + second->getBandwidth()) && failed == Verifier::CAPACITY ) {
                    printf("!!Aggregated vl capacity is more then before aggregation!\n");
                    std::pair<VirtualLink*, VirtualLink*>vls(first, second);
                    deprecated.insert(vls);
                    delete vl;
                    continue;
                }

                DataFlows dfs(first->getAssignments().begin(), first->getAssignments().end());
                dfs.insert(second->getAssignments().begin(), second->getAssignments().end());
                removeVirtualLink(port, first);
                removeVirtualLink(port, second);

                DataFlows::iterator it = dfs.begin();
                for ( ; it != dfs.end(); ++it ) {
                    setDataFlowVL(vl, *it);
                }
                printf("Aggregation finished\n");
                return;
            } else {
                std::pair<VirtualLink*, VirtualLink*>vls(first, second);
                deprecated.insert(vls);
            }
        } else fail = true; // failed
    }

    printf("Aggregation is failed, dropping data flow\n");
    VirtualLink* vlToDrop = *std::min_element(vls.begin(), vls.end(),
            failed == Verifier::CAPACITY ? comparerCapacity : comparerLMax);

    removeMostConstrainedVirtualLink(port, vlToDrop, failed);

}

bool compareVlParams(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getBag() == vl2->getBag() && vl1->getLMax() == vl2->getLMax();
}

void Designer::removeMostConstrainedVirtualLink(Port* port, VirtualLink* vlToDrop, Verifier::FailedConstraint failed) {
    // Trying to redesign without failedDataFlow
    bool success = false;
    while ( !success && vlToDrop->getAssignments().size() > 0 ) {
        DataFlow* toDel = 0;
        long minVal = 0;
        DataFlows::iterator it = vlToDrop->getAssignments().begin();
        for ( ; it != vlToDrop->getAssignments().end(); ++it ) {
            // removing data flows with the largest value of period/rMax
            long val = (*it)->getTMax() < (*it)->getPeriod() ? (*it)->getTMax() : (*it)->getPeriod();
            if ( toDel == 0 || val < minVal ) {
                toDel = *it;
                minVal = val;
            }
        }

        vlToDrop->removeAssignment(toDel);

        if ( vlToDrop->getAssignments().size() == 0) {
            Operations::removeVirtualLink(network, vlToDrop);
            removeVirtualLink(port, vlToDrop);
            return;
        }

        VirtualLink * newVl = VirtualLinkConfigurator::generateAggregatedVirtualLink(vlToDrop->getAssignments());
        newVl->setSource(vlToDrop->getSource());
        if ( newVl == 0 ) {
            // if there are more then one data flows, removing just dataflow
            success = false;
            continue;
        }

        // Checking whether params has changed
        if ( compareVlParams(newVl, vlToDrop) ) {
            delete newVl;
            success = false;
            continue;
        }

        if ( !replaceVirtualLinks(newVl, vlToDrop) )
            success = false;
    }
}

void Designer::removeVirtualLink(Port* port, VirtualLink* vlToDrop) {
    port->getParent()->removeOutgoingVirtualLink(vlToDrop, port);
    vlToDrop->removeAllAssignments();
    designedVirtualLinks.erase(vlToDrop);
    delete vlToDrop;
}

void Designer::removeVirtualLink(VirtualLink* vl, bool freeMemory) {
    Port* port = *(vl->getSource()->getPorts().begin());
    port->getParent()->removeOutgoingVirtualLink(vl, port);
    vl->removeAllAssignments();
    designedVirtualLinks.erase(vl);
    Operations::removeVirtualLink(network, vl);

    if ( freeMemory )
        delete vl;
}

void Designer::routeVirtualLinks() {
    std::vector<VirtualLink*> designed(designedVirtualLinks.begin(), designedVirtualLinks.end());
    std::sort(designed.begin(), designed.end(), comparerCapacity);
    std::vector<VirtualLink*>::iterator it = designed.begin();
    VirtualLinks assigned;
    for ( ; it != designed.end(); ++it ) {
        printf("Capacity: %d\n", (*it)->getBandwidth());
        bool found = Routing::findRoute(network, *it);
        if ( found ) {
            Operations::assignVirtualLink(network, *it);
            printf("Route assigned\n");
            assigned.insert(*it);
            continue;
        }

        if ( !disableLimitedSearch ) {
           // trying limited search
           found = limitedSearch(*it, assigned);
           if ( found ) {
               printf("Limited search succeed\n");
               assigned.insert(*it);
               continue;
           }
        }

        removeVirtualLink(*it);
    }
}

bool comparatorBandwidth(VirtualLink* vl1, VirtualLink* vl2) {
    // decreased by bwth
    return vl1->getBandwidth() > vl2->getBandwidth();
}

bool Designer::limitedSearch(VirtualLink* virtualLink, VirtualLinks& assigned) {
    LimitedSearcher searcher(limitedSearchDepth, network);
    searcher.start(assigned, comparatorBandwidth);
    printf("Starting limited search\n");
    VirtualLinks vls = searcher.getNextSet();
    while(vls.size() > 0) {
        // Trying to assign the element first
        if ( Routing::findRoute(network, virtualLink) ) {
            Operations::assignVirtualLink(network, virtualLink);

            // Assigning removed nodes
            bool allAssigned = true;
            VirtualLinks::iterator it = vls.begin();
            for ( ; it != vls.end(); ++it ) {
                if ( !Routing::findRoute(network, *it) ) {
                    allAssigned = false;
                    break;
                }

                Operations::assignVirtualLink(network, *it);
            }

            if ( allAssigned ) {
                searcher.stop(true);
                return true;
            }
        }

        Operations::removeVirtualLink(network, virtualLink);
        virtualLink->getRoute().getPaths().clear();
        vls = searcher.getNextSet();
    }

    printf("Limited search failed\n");
    searcher.stop(false);
    return false;
}

bool decreasingDurationConstraints(VirtualLink* vl1, VirtualLink* vl2) {
    // decreased by bwth
    long sumDur1 = 0, sumDur2 = 0;
    for ( DataFlows::iterator it = vl1->getAssignments().begin(); it != vl1->getAssignments().end(); ++it ) {
        sumDur1 += (*it)->getTMax();
    }

    for ( DataFlows::iterator it = vl2->getAssignments().begin(); it != vl2->getAssignments().end(); ++it ) {
        sumDur2 += (*it)->getTMax();
    }

    return sumDur1 < sumDur2;
}

VirtualLinks fullyAssignedVLs;

void assertVirtualLinks(VirtualLinks& virtualLinks) {
    VirtualLinks::iterator it = virtualLinks.begin();
    for (; it != virtualLinks.end(); ++it ) {
        assert ((*it)->getBag() > 0 && (*it)->getBag() < 129 );
        assert ((*it)->getLMax() < 1519 &&  (*it)->getLMax() > 0);
        if (! ((*it)->getJMax() <= 500 && (*it)->getJMax() >= 0) ) {
            printf("JMax is %d\n", (*it)->getJMax());
            assert(0);
        }
    }
}

bool Designer::calculateAndCheckResponseTimeouts(VirtualLinks vlsToCheck, bool redesignIfFail) {
    VirtualLinks virtualLinks;
    std::vector<VirtualLink*> vlsToCheckSorted(vlsToCheck.begin(), vlsToCheck.end());
    std::sort(vlsToCheckSorted.begin(), vlsToCheckSorted.end(), decreasingDurationConstraints);
    bool recheck = true;
    while ( recheck ) {
        recheck = false;
        virtualLinks = VirtualLinks(designedVirtualLinks.begin(), designedVirtualLinks.end());
        virtualLinks.insert(existingVirtualLinks.begin(), existingVirtualLinks.end());

        assertVirtualLinks(virtualLinks);
        responseTimeEstimator->setVirtualLinks(virtualLinks);
        responseTimeEstimator->initialize();

        std::vector<VirtualLink*>::iterator it = vlsToCheckSorted.begin();
        for ( ; it != vlsToCheckSorted.end(); ++it ) {
        //VirtualLinks::iterator it = vlsToCheck.begin();
        //for ( ; it != vlsToCheck.end(); ++it ) {
            bool success = false;


            if ( designedVirtualLinks.find(*it) == designedVirtualLinks.end() )
                continue;
            float estimation = responseTimeEstimator->estimateWorstCaseResponseTime(*it);
            long estimationLong = (long)(estimation - EPS) < (long)estimation ? (long)estimation : (long)estimation + 1;
            (*it)->setResponseTimeEstimation(estimationLong);

            while ( !success && (*it)->getAssignments().size() > 0 ) {
                DataFlow* failedDataFlow = Operations::setAndCheckResponseTimes(*it);
                if ( failedDataFlow != 0 ) {
                    success = false;
                    printf("Constraints are failed for data flow.\n");
                    if ( !redesignIfFail )
                        return false;

                    if ( !disableRedesign ) {

                        printf("Trying to redesign vl.\n");
                        success = redesignVirtualLink(failedDataFlow, *it, numOfIterationsOnRedesign);
                        if ( !success && !disableAggregationOnResponseTime ) {
                            success = limitedSearchAggregation(failedDataFlow, *it);
                        }
                    }

                    if ( !success ) {
                        printf("Removing assignment for vl with %d dataFlows\n", (*it)->getAssignments().size());

                        // Removing data flow
                        if ( (*it)->getAssignments().find(failedDataFlow) != (*it)->getAssignments().end() )
                            (*it)->removeAssignment(failedDataFlow);

                    } else {
                        printf("Virtual link redesigned successfully! Rechecking response time.\n");
                        recheck = true;
                        break; // recheck again
                    }

                // removeVirtualLink(*it);
                } else {
                    success = true;
                    fullyAssignedVLs.insert(*it);
                }
            }

            if ( recheck )
                break; // recheck again as we redesigned virtual link

            if ( (*it)->getAssignments().size() == 0 && designedVirtualLinks.find(*it) != designedVirtualLinks.end() )
                removeVirtualLink(*it);
        }
    }

    return true;
}

bool Designer::replaceVirtualLinks(VirtualLink* newVl, VirtualLink* oldVl1, VirtualLink* oldVl2, bool removeIfFail) {
    DataFlows flowsFromVl1, flowsFromVl2;
    cutVirtualLink(network, oldVl1, flowsFromVl1);

    if ( oldVl2 != 0 ) {
        cutVirtualLink(network, oldVl2, flowsFromVl2);
    }

    DataFlows::iterator it = flowsFromVl1.begin();
    for ( ; it != flowsFromVl1.end(); ++it )
        setDataFlowVL(newVl, *it);
    if ( oldVl2 != 0 ) {
        it = flowsFromVl2.begin();
        for ( ; it != flowsFromVl2.end(); ++it )
            setDataFlowVL(newVl, *it);
    }

    Verifier::FailedConstraint failed = Verifier::NONE;
    Ports::iterator pit = newVl->getSource()->getPorts().begin();
    for ( ; pit != newVl->getSource()->getPorts().end(); ++pit ) {
        printf("JChecking jmax\n");
        failed = Verifier::verifyOutgoingVirtualLinks(*pit, newVl);

        if ( failed != Verifier::NONE ) {
            printf("Failed to redesign virtual link(s) as jitter/capacity constraints are not satisfied.\n");
            // Fail, redesigning without df
            removeVirtualLink(newVl, removeIfFail);
            restoreVirtualLink(network, oldVl1, flowsFromVl1);
            if ( oldVl2 != 0 )
                restoreVirtualLink(network, oldVl2, flowsFromVl2);
            return false;
        }
    }

    if ( oldVl1->getRoute().getPaths().size() == 0 ) {
        designedVirtualLinks.insert(newVl);
        return true; // no need to find route
    }

    // Trying to route
    bool found = Routing::findRoute(network, newVl);
    if ( !found ) {
        printf("Failed to redesign virtual link(s) as route cannot be found.\n");
        // Fail, redesigning without df
        removeVirtualLink(newVl, removeIfFail);
        restoreVirtualLink(network, oldVl1, flowsFromVl1);
        if ( oldVl2 != 0 )
            restoreVirtualLink(network, oldVl2, flowsFromVl2);
        return false;
    }

    Operations::assignVirtualLink(network, newVl);

    // Now checking response time estimations for newly created virtual link
    VirtualLinks vls;
    vls.insert(newVl);

    printf("JMax is %d\n", newVl->getJMax());
    bool satisfied = calculateAndCheckResponseTimeouts(vls, false);
    if ( !satisfied ) {
        printf("Failed to redesign virtual link(s) as the e2e response time constraints are not satisfied.\n");
        // Fail, redesigning without df
        removeVirtualLink(newVl, removeIfFail);
        restoreVirtualLink(network, oldVl1, flowsFromVl1);
        if ( oldVl2 != 0 )
            restoreVirtualLink(network, oldVl2, flowsFromVl2);
        return false;
    }

    delete oldVl1;
    delete oldVl2;

    printf("Succeed to replace old virtual link(s) with a new one!\n");
    designedVirtualLinks.insert(newVl);

    return true;
}

bool Designer::redesignVirtualLink(DataFlow* df, VirtualLink* vl, unsigned int numberOfIterations) {
    long respTimeEstimation = vl->getResponseTimeEstimation();

    VirtualLink* currentVl = vl, *newVl = 0;
    while ( numberOfIterations-- >= 1) {
        newVl = VirtualLinkConfigurator::redesignVirtualLink(df, currentVl);
        assert(currentVl != newVl);
        if ( newVl == 0 ) {
            // if there are more then one data flows, removing just dataflow
            if ( currentVl != vl )
                delete currentVl;
            return false;
        }

        newVl->setSource(currentVl->getSource());

        // Checking whether params has changed
        if ( compareVlParams(newVl, currentVl) ) {
            printf("Params did not changed!\n");
            delete newVl;
            if ( currentVl != vl )
                delete currentVl;
            return false;
        }

        if ( replaceVirtualLinks(newVl, vl, 0, false) ) {
            //return false;
            if ( currentVl != vl )
                delete currentVl;
            assert(currentVl != newVl);
            printf("Redesign succeed!\n");
            return true;
        }


        if ( numberOfIterations > 0 ) {
            // if we can estimate the response time and it is changed - try again
            long newRespTimeEstimation = newVl->getResponseTimeEstimation();
            if ( newRespTimeEstimation == 0  ) {
                printf("Resp time is zero after redesign\n");
                break;
            }

            if ( newRespTimeEstimation == respTimeEstimation ) {
                printf("Resp time did not changed after redesign\n");
                break;
            }

            if ( newRespTimeEstimation > respTimeEstimation ) {
                printf("Resp time is longer after redesign\n");
            }

            printf("Trying to redesign again\n");
            if ( currentVl != vl )
                delete currentVl;

            currentVl = newVl;
            newVl = 0;
        }
    }

    delete newVl;
    return false;
}

// Performing limited search only for fullyAssignedVLs
bool Designer::limitedSearchAggregation(DataFlow* df, VirtualLink* vl) {
    VirtualLink *first, *second;
    bool fail = false;
    VirtualLinksAggregator::DeprecatedVLPairs deprecated;

    while ( !fail ) {
       first = 0;
       second = 0;
       VirtualLinksAggregator::selectVLsForAggregation(network, vl->getRoute(), &first, &second, Verifier::TMAX, deprecated);
       if ( first != 0 && second != 0 ) {
           // TODO: ineffective
           if ( fullyAssignedVLs.find(first) == fullyAssignedVLs.end() || fullyAssignedVLs.find(second) == fullyAssignedVLs.end() ) {
               std::pair<VirtualLink*, VirtualLink*>vls(first, second);
               deprecated.insert(vls);
               continue;
           }

           VirtualLink* newVl = VirtualLinksAggregator::performAggregation(first, second);
           if ( newVl != 0 && (newVl->getLMax() >= (first->getLMax() + second->getLMax()) || newVl->getBandwidth() >= (first->getBandwidth() + second->getBandwidth())) ) {
               printf("!!Aggregated vl LMax/bw is more then before aggregation!\n");
               std::pair<VirtualLink*, VirtualLink*>vls(first, second);
               deprecated.insert(vls);
               delete newVl;
           } else if ( newVl != 0 && replaceVirtualLinks(newVl, first, second, false) ) {
               printf("Aggregation finished\n");
               return true;
           } else {
               std::pair<VirtualLink*, VirtualLink*>vls(first, second);
               deprecated.insert(vls);
               if ( newVl != 0 ) {
                   delete newVl;
                   newVl = 0;
               }
           }
       } else fail = true; // failed
    }
    return false;
}

void Designer::cutVirtualLink(Network* network, VirtualLink* vl, DataFlows& dfs) {
    Operations::removeVirtualLink(network, vl);
    dfs = DataFlows(vl->getAssignments().begin(), vl->getAssignments().end());
    vl->removeAllAssignments();
    designedVirtualLinks.erase(vl);
}

void Designer::restoreVirtualLink(Network* network, VirtualLink* vl, DataFlows& dfs) {
    DataFlows::iterator it = dfs.begin();
    for ( ; it != dfs.end(); ++it )
        setDataFlowVL(vl, *it);
    if ( vl->getRoute().getPaths().size() == vl->getDestinations().size() )
        Operations::assignVirtualLink(network, vl);
    designedVirtualLinks.insert(vl);

}
