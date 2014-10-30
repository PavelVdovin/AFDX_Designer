#include "verifier.h"
#include "virtualLink.h"
#include "operations.h"
#include "port.h"
#include "link.h"

bool Verifier::limitJitter = true;

std::string Verifier::verify(Network* network, VirtualLinks& virtualLinks) {
    std::string status = "Verified successfully";

    VirtualLinks::iterator it = virtualLinks.begin();
    for( ; it != virtualLinks.end(); ++it ) {
        if ( !Operations::assignVirtualLink(network, *it) ) {
            status = "Verification failed: cannot assign virtual link";
            break;
        }
    }

    // After assignment, need to count jitter and verify it
    it = virtualLinks.begin();
    for( ; it != virtualLinks.end(); ++it ) {
        if ( verifyOutgoingVirtualLinks(*it) == Verifier::JMAX ) {
            status = "Verification failed: jitter is more then 500 microseconds";
            break;
        }
    }

    return status;
}

Verifier::FailedConstraint Verifier::verifyOutgoingVirtualLinks(Port* port, bool checkCapacity) {
    VirtualLinks::iterator it = port->getAssignedLowPriority().begin();
    for ( ; it != port->getAssignedLowPriority().end(); ++it ) {
        Verifier::FailedConstraint constraint = verifyOutgoingVirtualLinks(port, *it, checkCapacity);
        if ( constraint != Verifier::NONE )
            return constraint;
    }

    if ( port->isPrioritized() ) {
        it = port->getAssignedHighPriority().begin();
        for ( ; it != port->getAssignedHighPriority().end(); ++it ) {
            Verifier::FailedConstraint constraint = verifyOutgoingVirtualLinks(port, *it, checkCapacity);
            if ( constraint != Verifier::NONE )
                return constraint;
        }
    }

    return Verifier::NONE;
}

Verifier::FailedConstraint Verifier::verifyOutgoingVirtualLinks(Port* port, VirtualLink* vl, bool checkCapacity) {
    assert(vl != 0);
    if ( checkCapacity && (port->getAssosiatedLink()->getFreeCapacityFromPort(port) < 0 ||
            vl != 0 && vl->getRoute().getPaths().size() == 0 &&
            port->getAssosiatedLink()->getFreeCapacityFromPort(port) < vl->getBandwidth()) ) {
        printf("Capacity is overloaded: %d\n", port->getAssosiatedLink()->getFreeCapacityFromPort(port));
        return Verifier::CAPACITY;
    }

    if ( !port->getParent()->isEndSystem() )
        return Verifier::NONE;

    long jMax = Operations::countMaxJitter(port, vl);
    if ( limitJitter && jMax >= 500 ) {
        printf("Jmax is overloaded: %d\n", jMax);
        return Verifier::JMAX;
    }

    vl->setJMax(jMax);


    return Verifier::NONE;
}

Verifier::FailedConstraint Verifier::verifyOutgoingVirtualLinks(VirtualLink* vl) {
	Ports& ports = vl->getSource()->getPorts();
	Ports::iterator it = ports.begin();
	for ( ; it != ports.end(); ++it ) {
	    if ( (*it)->isAssigned(vl) )
	        return verifyOutgoingVirtualLinks(*it, vl);
	}
	assert(0);
	return Verifier::NONE;
}
