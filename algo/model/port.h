#ifndef PORT
#define PORT

#include "defs.h"
#include <assert.h>
#include <stdio.h>

class Port {
    friend class Factory;
public:
	enum Type {
		NONE,
		FIFO,
		PRIORITIZED
	};

	Port(NetElement* parent, Type type = NONE): parent(parent), assosiatedLink(0), type(type) {
	}

	inline Link* getAssosiatedLink() const {
		return assosiatedLink;
	}

	inline NetElement* getParent() const {
		return parent;
	}

	inline void assignVirtualLink(VirtualLink* vl, bool highPriority = false) {
	    if ( !highPriority ) {
	        assert(assignedLowPriority.find(vl) == assignedLowPriority.end());
	        assignedLowPriority.insert(vl);
	        return;
	    }

	    assert(type == PRIORITIZED);
	    assert(assignedHighPriority.find(vl) == assignedHighPriority.end());
	    assignedHighPriority.insert(vl);
	}

	inline VirtualLinks& getAssignedLowPriority() {
	    return assignedLowPriority;
	}

	inline VirtualLinks& getAssignedHighPriority() {
	    assert(type == PRIORITIZED);
        return assignedHighPriority;
    }

	inline bool isAssigned(VirtualLink* vl) {
	    return assignedLowPriority.find(vl) != assignedLowPriority.end()
	            || assignedLowPriority.find(vl) != assignedLowPriority.end();
	}

	inline void removeVirtualLink(VirtualLink* vl) {
	    if ( assignedLowPriority.find(vl) != assignedLowPriority.end() ) {
	        assignedLowPriority.erase(vl);
	        return;
	    }

	    if ( assignedHighPriority.find(vl) != assignedHighPriority.end() )
            assignedHighPriority.erase(vl);
	}

private:
	Type type;

	NetElement* parent;
	Link* assosiatedLink;
	VirtualLinks assignedLowPriority;
	VirtualLinks assignedHighPriority;
};

#endif
