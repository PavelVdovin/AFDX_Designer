#ifndef NETELEMENT
#define NETELEMENT

#include "defs.h"

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

protected:
	// Outgoing ports
	Ports ports;

	TYPE type;
};

#endif
