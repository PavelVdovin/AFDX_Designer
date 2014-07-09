#include "designer.h"
#include "verifier.h"
#include "dataFlow.h"
#include "virtualLink.h"
#include "netelement.h"
#include "partition.h"
#include "virtualLinkConfigurator.h"
#include "operations.h"
#include <stdio.h>
#include <algorithm>

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
}

void assignVLToEndSystem(VirtualLink* vl, NetElement* endSystem) {
    // we can assign vl to endSystem only if it has only 1 port
    if ( endSystem->getPorts().size() == 1 ) {
        endSystem->assignOutgoingVirtualLink(vl, *endSystem->getPorts().begin());
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
                df->assign(vl);
                designedVirtualLinks.insert(vl);
                NetElement* es = df->getFrom()->getConnected();
                vl->setSource(es);
                assignVLToEndSystem(vl, es);
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

bool comparerCapacity(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getBandwidth() < vl2->getBandwidth();
}

bool comparerLMax(VirtualLink* vl1, VirtualLink* vl2) {
    return vl1->getLMax() < vl2->getLMax();
}

void Designer::redesignOutgoingVirtualLinks(Port* port, Verifier::FailedConstraint failed) {
    assert(failed != Verifier::NONE);
    // Current implementation drops most weighted element
    VirtualLinks vls = port->getAssignedLowPriority();
    if ( failed == Verifier::CAPACITY ) {
        VirtualLink* vlToDrop = *std::max_element(vls.begin(), vls.end(), comparerCapacity);
        vls.erase(vlToDrop);
        printf("Capacity is overloaded, Vl is dropped\n");
        removeVirtualLink(port, vlToDrop);
    } else {
        assert(failed == Verifier::JMAX);
        VirtualLink* vlToDrop = *std::max_element(vls.begin(), vls.end(), comparerLMax);
        vls.erase(vlToDrop);
        printf("Jmax is overloaded, Vl is dropped\n");
        removeVirtualLink(port, vlToDrop);
    }
}

void Designer::removeVirtualLink(Port* port, VirtualLink* vlToDrop) {
    port->getParent()->removeOutgoingVirtualLink(vlToDrop, port);
    vlToDrop->removeAllAssignments();
    designedVirtualLinks.erase(vlToDrop);
    delete vlToDrop;
}
