#ifndef NETELEMENT
#define NETELEMENT

#include "defs.h"
#include "port.h"
#include <assert.h>

/*
 * Class defines the element which takes part in communication: end system or switch.
 */
class NetElement {
	friend class Factory;

protected:
	enum TYPE {
	    NONE = 0,
	    END_SYSTEM = 1,
	    SWITCH = 2
	};

public:
	NetElement(): type(NONE) {
	}

	virtual ~NetElement() {
	}

	inline bool isEndSystem() const {
        return type == END_SYSTEM;
    }

    inline bool isSwitch() const {
        return type == SWITCH;
    }

	inline EndSystem * toEndSystem() const {
	    if ( !isEndSystem() )
	        return 0;
        return (EndSystem *)this;
    }

    inline Switch * toSwitch() const {
        if ( !isSwitch() )
            return 0;
        return (Switch *)this;
    }

    inline void assignOutgoingVirtualLink(VirtualLink* virtualLink, Port* outgoingPort, bool highPriority = false) {
        //assignedVirtualLinks[outgoingPort].insert(virtualLink);
        outgoingPort->assignVirtualLink(virtualLink, highPriority);
    }

    inline void removeOutgoingVirtualLink(VirtualLink* virtualLink, Port* outgoingPort) {
        //assignedVirtualLinks[outgoingPort].erase(outgoingPort);
        outgoingPort->removeVirtualLink(virtualLink);
    }

    inline VirtualLinks& getAssignedVirtualLinks(Port* port, bool highPriority = false) {
        if ( !highPriority )
            return port->getAssignedLowPriority();
        return port->getAssignedHighPriority();
    }

    inline Ports& getPorts() {
        return ports;
    }

protected:
	// Outgoing ports
	Ports ports;
	// AssignedVirtualLinks assignedVirtualLinks;

	TYPE type;
};

#endif
