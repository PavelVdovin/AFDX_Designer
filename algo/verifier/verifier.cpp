#include "verifier.h"
#include "virtualLink.h"
#include "operations.h"
#include "port.h"
#include "link.h"

std::string Verifier::verify(Network* network, VirtualLinks& virtualLinks) {
    std::string status = "Verified successfully";

    VirtualLinks::iterator it = virtualLinks.begin();
    for( ; it != virtualLinks.end(); ++it ) {
        if ( !Operations::assignVirtualLink(network, *it) )
            status = "Verification failed: cannot assign virtual link";
    }

    return status;
}

Verifier::FailedConstraint Verifier::verifyOutgoingVirtualLinks(Port* port, VirtualLink* vl) {
    if ( port->getAssosiatedLink()->getFreeCapacityFromPort(port) < 0 ||
            vl != 0 && port->getAssosiatedLink()->getFreeCapacityFromPort(port) < vl->getBandwidth() ) {
        printf("Capacity is overloaded: %d\n", port->getAssosiatedLink()->getFreeCapacityFromPort(port));
        return Verifier::CAPACITY;
    }

    if ( !port->getParent()->isEndSystem() )
        return Verifier::NONE;

    long jMax = Operations::countMaxJitter(port, vl);
    if ( jMax >= 500 ) {
        printf("Jmax is overloaded: %d\n", jMax);
        return Verifier::JMAX;
    }

    return Verifier::NONE;
}
