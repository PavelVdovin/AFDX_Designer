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
#include "responseTimeEstimator.h"
#include "trajectoryApproachBasedEstimator.h"
#include <stdio.h>
#include <algorithm>

#define LIMITED_SEARCH_DEPTH 2

Designer::~Designer() {
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
    calculateAndCheckResponseTimeouts();
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

void Designer::checkOutgoingVirtualLinks() {
    NetElements endSystems = Operations::getEndSystems(network);
    NetElements::iterator it = endSystems.begin();
    for ( ; it != endSystems.end(); ++it )
        checkOutgoingVirtualLinks(*it);
}

void Designer::checkOutgoingVirtualLinks(NetElement* endSystem) {
    if ( endSystem->getPorts().size() == 1 ) {
        Port* port = *endSystem->getPorts().begin();
        Verifier::FailedConstraint failed = Verifier::verifyOutgoingVirtualLinks(port);
        while ( failed != Verifier::NONE ) {
            redesignOutgoingVirtualLinks(port, failed);
            failed= Verifier::verifyOutgoingVirtualLinks(port);
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
                    // TODO: redo aggregation, cannot do it now
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

    printf("Aggregation is failed, dropping vl\n");
    VirtualLink* vlToDrop = *std::min_element(vls.begin(), vls.end(),
            failed == Verifier::CAPACITY ? comparerCapacity : comparerLMax);
    removeVirtualLink(port, vlToDrop);
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

bool Designer::calculateAndCheckResponseTimeouts() {
    VirtualLinks virtualLinks;
    ResponseTimeEstimator* estimator = new TrajectoryApproachBasedEstimator(network, virtualLinks);
    bool recheck = true;
    while ( recheck ) {
        recheck = false;
        virtualLinks = VirtualLinks(designedVirtualLinks.begin(), designedVirtualLinks.end());
        virtualLinks.insert(existingVirtualLinks.begin(), existingVirtualLinks.end());
        estimator->setVirtualLinks(virtualLinks);
        estimator->initialize();
        VirtualLinks::iterator it = designedVirtualLinks.begin();
        for ( ; it != designedVirtualLinks.end(); ++it ) {
            float estimation = estimator->estimateWorstCaseResponseTime(*it);
            long estimationLong = (long)(estimation - EPS) < (long)estimation ? (long)estimation : (long)estimation + 1;
            printf("Resp time: %d\n", estimationLong);
            (*it)->setResponseTimeEstimation(estimationLong);

            DataFlow* failedDataFlow = Operations::setAndCheckResponseTimes(*it);
            if ( failedDataFlow != 0 ) {
                printf("Constraints are failed for data flow. Trying to redesign vl.\n");

                bool success = redesignVirtualLink(failedDataFlow, *it);
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

    delete estimator;
}

bool compareVlParams(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getBag() == vl2->getBag() && vl1->getLMax() == vl2->getLMax();
}

bool Designer::redesignVirtualLink(DataFlow* df, VirtualLink* vl) {
    DataFlows flows(vl->getAssignments().begin(), vl->getAssignments().end());
    // Removing path first
    Operations::removeVirtualLink(network, vl);
    vl->getRoute().getPaths().clear();
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

    vl->removeAllAssignments();

    DataFlows::iterator it = flows.begin();
    for ( ; it != flows.end(); ++it )
        setDataFlowVL(newVl, *it);

    Verifier::FailedConstraint failed = Verifier::NONE;
    if ( newVl->getSource()->getPorts().size() == 1 ) {
        Port* port = *(newVl->getSource()->getPorts().begin());
        failed = Verifier::verifyOutgoingVirtualLinks(port);
    }
    if ( failed != Verifier::NONE ) {
        // Fail, redesigning without df
        removeVirtualLink(newVl);
        it = flows.begin();
        for ( ; it != flows.end(); ++it )
            setDataFlowVL(vl, *it);
        return false;
    }

    // Trying to route
    bool found = Routing::findRoute(network, newVl);
    if ( !found ) {
        // Fail, redesigning without df
        removeVirtualLink(newVl);
        it = flows.begin();
        for ( ; it != flows.end(); ++it )
            setDataFlowVL(vl, *it);
        return false;
    }

    Operations::assignVirtualLink(network, newVl);
    designedVirtualLinks.erase(vl);
    delete vl;

    designedVirtualLinks.insert(newVl);

    return true;
}
