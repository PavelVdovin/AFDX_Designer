#ifndef SWITCH_H
#define SWITCH_H

#include "netelement.h"

class Switch: public NetElement {
	friend class Factory;
public:
	Switch() {
	    type = NetElement::SWITCH;
	}

	virtual ~Switch() {
	}
};
#endif
