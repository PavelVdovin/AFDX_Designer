#ifndef END_SYSTEM_H
#define END_SYSTEM_H

#include "netelement.h"

class EndSystem: public NetElement {
	friend class Factory;
public:
	EndSystem() {
	    type = NetElement::END_SYSTEM;
	}

protected:
	// All connected partitions
	Partitions partitions;
};
#endif
