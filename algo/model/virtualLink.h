#ifndef VIRTUAL_LINK
#define VIRTUAL_LINK

#include "defs.h"
#include "route.h"

class VirtualLink {
    friend class Factory;
public:
	VirtualLink():
		from(0),
		bag(1),
		lMax(1000){
	}

	inline NetElement* getSource() const {
		return from;
	}

	inline void setSource(NetElement* from) {
		this->from = from;
	}

	inline NetElements& getDestinations() {
		return to;
	}

	inline void addDestination(NetElement* dest) {
		to.insert(dest);
	}

	inline void removeDestination(NetElement* dest) {
		to.erase(dest);
	}

	inline Route& getRoute() {
		return route;
	}

private:
	int bag;
	int lMax;
	int jMax;

	NetElement* from;
	NetElements to;

	Route route;
};

#endif
