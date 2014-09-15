#include "verifier.h"
#include "virtualLink.h"
#include "operations.h"
#include "port.h"
#include "link.h"

std::string Verifier::verify(Network* network, VirtualLinks& virtualLinks) {
    std::string status = "Verified successfully";

    VirtualLinks::iterator it = virtualLinks.begin();
    for( ; it != virtualLinks.end(); ++it ) {
        if ( !Operations::assignVirtualLink(network, *it) ) {
            status = "Verification failed: cannot assign virtual link";
            break;
        }

        if ( verifyOutgoingVirtualLinks(*it) == Verifier::JMAX ) {
        	status = "Verification failed: jitter is more then 500 microseconds";
        	break;
        }
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

    if ( vl != 0 ) {
    	vl->setJMax(jMax);
    }

    return Verifier::NONE;
}

Verifier::FailedConstraint Verifier::verifyOutgoingVirtualLinks(VirtualLink* vl) {
	Ports& ports = vl->getSource()->getPorts();
	assert(ports.size() == 1);
	return verifyOutgoingVirtualLinks(*(ports.begin()), vl);
}
