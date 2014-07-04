#ifndef VIRTUAL_LINK
#define VIRTUAL_LINK

#include "defs.h"
#include "route.h"

#include <assert.h>

class VirtualLink {
    friend class Factory;
public:
	VirtualLink():
		from(0),
		bag(1),
		lMax(1000){
	    bandwidth = lMax/bag;
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

	inline void setLMax(int lMax) {
	    this->lMax = lMax;
	    if ( bag != 0 && lMax != 0 )
	        this->bandwidth = (float) lMax / bag;
	}

	inline void setBag(int bag) {
        this->bag = bag;
        if ( lMax != 0 && bag != 0 )
            this->bandwidth = (float) lMax / bag;
    }

	inline float getBandwidth() {
	    return bandwidth;
	}

	inline void addPath(Path* path) {
	    route.addPath(path->getDest(), path);
	}

private:
	int bag;
	int lMax;
	int jMax;

	float bandwidth;

	NetElement* from;
	NetElements to;

	Route route;
};

#endif
