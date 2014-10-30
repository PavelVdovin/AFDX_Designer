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

#define LIMITED_SEARCH_DEPTH 2

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
                if ( vl->getBandwidth() >= (first->getBandwidth() + second->getBandwidth()) ) {
                    printf("!!Aggregated vl capacity is more then before aggregation!\n");
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

        DataFlow* df = *(vlToDrop->getAssignments().begin());
        success = redesignVirtualLink(df, vlToDrop);
    }
}

void Designer::removeVirtualLink(Port* port, VirtualLink* vlToDrop) {
    port->getParent()->removeOutgoingVirtualLink(vlToDrop, port);
    vlToDrop->removeAllAssignments();
    designedVirtualLinks.erase(vlToDrop);
    delete vlToDrop;
}

void Designer::removeVirtualLink(VirtualLink* vl) {
    vl->removeAllAssignments();
    designedVirtualLinks.erase(vl);
    Operations::removeVirtualLink(network, vl);
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

        // trying limited search
        found = limitedSearch(*it, assigned);
        if ( found ) {
            printf("Limited search succeed\n");
            assigned.insert(*it);
            continue;
        }

        removeVirtualLink(*it);
    }
}

bool comparatorBandwidth(VirtualLink* vl1, VirtualLink* vl2) {
    // decreased by bwth
    return vl1->getBandwidth() > vl2->getBandwidth();
}

bool Designer::limitedSearch(VirtualLink* virtualLink, VirtualLinks& assigned) {
    LimitedSearcher searcher(LIMITED_SEARCH_DEPTH, network);
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

bool Designer::calculateAndCheckResponseTimeouts(VirtualLinks& vlsToCheck, bool redesignIfFail) {
    VirtualLinks virtualLinks;
    bool recheck = true;
    while ( recheck ) {
        recheck = false;
        virtualLinks = VirtualLinks(designedVirtualLinks.begin(), designedVirtualLinks.end());
        virtualLinks.insert(existingVirtualLinks.begin(), existingVirtualLinks.end());
        responseTimeEstimator->setVirtualLinks(virtualLinks);
        responseTimeEstimator->initialize();
        VirtualLinks::iterator it = vlsToCheck.begin();
        for ( ; it != vlsToCheck.end(); ++it ) {
            float estimation = responseTimeEstimator->estimateWorstCaseResponseTime(*it);
            long estimationLong = (long)(estimation - EPS) < (long)estimation ? (long)estimation : (long)estimation + 1;
            printf("Resp time: %d\n", estimationLong);
            (*it)->setResponseTimeEstimation(estimationLong);

            DataFlow* failedDataFlow = Operations::setAndCheckResponseTimes(*it);
            if ( failedDataFlow != 0 ) {
                printf("Constraints are failed for data flow.\n");
                if ( !redesignIfFail )
                    return false;

                printf("Trying to redesign vl.\n");
                bool success = redesignVirtualLink(failedDataFlow, *it);
                if ( !success ) {
                    success = limitedSearchAggregation(failedDataFlow, *it);
                }

                if ( !success ) {
                    // Removing data flow
                    (*it)->removeAssignment(failedDataFlow);

                    // Trying to redesign without failedDataFlow
                    while ( !success && (*it)->getAssignments().size() > 0 ) {
                        failedDataFlow = *((*it)->getAssignments().begin());
                        success = redesignVirtualLink(failedDataFlow, *it);
                    }

                    if ( (*it)->getAssignments().size() == 0 )
                        removeVirtualLink(*it);

                    printf("Failed to redesign virtual link.\n");
                }
                else
                    printf("Virtual link redesigned successfully! Rechecking response time.\n");
                recheck = true;
                break; // recheck again
                // removeVirtualLink(*it);
            }
        }
    }

    return true;
}

bool compareVlParams(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getBag() == vl2->getBag() && vl1->getLMax() == vl2->getLMax();
}

bool Designer::replaceVirtualLinks(VirtualLink* newVl, VirtualLink* oldVl1, VirtualLink* oldVl2) {
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
        failed = Verifier::verifyOutgoingVirtualLinks(*pit);

        if ( failed != Verifier::NONE ) {
            printf("Failed to redesign virtual link(s) as jitter/capacity constraints are not satisfied.\n");
            // Fail, redesigning without df
            removeVirtualLink(newVl);
            restoreVirtualLink(network, oldVl1, flowsFromVl1);
            if ( oldVl2 != 0 )
                restoreVirtualLink(network, oldVl2, flowsFromVl2);
            return false;
        }
    }

    // Trying to route
    bool found = Routing::findRoute(network, newVl);
    if ( !found ) {
        printf("Failed to redesign virtual link(s) as route cannot be found.\n");
        // Fail, redesigning without df
        removeVirtualLink(newVl);
        restoreVirtualLink(network, oldVl1, flowsFromVl1);
        if ( oldVl2 != 0 )
            restoreVirtualLink(network, oldVl2, flowsFromVl2);
        return false;
    }

    Operations::assignVirtualLink(network, newVl);

    // Now checking response time estimations for newly created virtual link
    VirtualLinks vls;
    vls.insert(newVl);
    bool satisfied = calculateAndCheckResponseTimeouts(vls, false);
    if ( !satisfied ) {
        printf("Failed to redesign virtual link(s) as the e2e response time constraints are not satisfied.\n");
        // Fail, redesigning without df
        removeVirtualLink(newVl);
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

bool Designer::redesignVirtualLink(DataFlow* df, VirtualLink* vl) {
    VirtualLink * newVl = VirtualLinkConfigurator::redesignVirtualLink(df, vl);
    if ( newVl == 0 ) {
        // if there are more then one data flows, removing just dataflow
        return false;
    }

    // Checking whether params has changed
    if ( compareVlParams(newVl, vl) ) {
        delete newVl;
        return false;
    }

    if ( !replaceVirtualLinks(newVl, vl) )
        return false;

    return true;
}

bool Designer::limitedSearchAggregation(DataFlow* df, VirtualLink* vl) {
    VirtualLink *first, *second;
    bool fail = false;
    VirtualLinksAggregator::DeprecatedVLPairs deprecated;

    while ( !fail ) {
       first = 0;
       second = 0;
       VirtualLinksAggregator::selectVLsForAggregation(network, vl->getRoute(), &first, &second, Verifier::TMAX, deprecated);
       if ( first != 0 && second != 0 ) {
           VirtualLink* vl = VirtualLinksAggregator::performAggregation(first, second);
           if ( vl != 0 && replaceVirtualLinks(vl, first, second) ) {
               printf("Aggregation finished\n");
               return true;
           } else {
               std::pair<VirtualLink*, VirtualLink*>vls(first, second);
               deprecated.insert(vls);
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
    Operations::assignVirtualLink(network, vl);
    designedVirtualLinks.insert(vl);
}
