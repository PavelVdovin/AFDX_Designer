#ifndef VIRTUAL_LINK
#define VIRTUAL_LINK

#include "defs.h"
#include "route.h"
#include "dataFlow.h"

#include <assert.h>

#define EPS 0.00001

class VirtualLink {
    friend class Factory;
public:
	VirtualLink():
		from(0),
		bag(1),
		lMax(1000){
	    bandwidth = 1000; // in bytes/ms
	    responseTimeEstimation = 0; //microseconds
	}

	/*
	VirtualLink(const VirtualLink& other):
	    bag(other.bag),
	    lMax(other.lMax),
	    jMax(other.jMax),
	    bandwidth(other.bandwidth),
	    from(other.from),
	    to(other.to.begin(), other.to.end()),
	    responseTimeEstimation(other.responseTimeEstimation),
	    assignments(other.assignments.begin(), other.assignments.end()),
	    route(other.route.getPaths()){
	}*/

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

	inline int getLMax() const {
	    return lMax;
	}

	inline void setLMax(int lMax) {
	    this->lMax = lMax;
	    if ( bag != 0 && lMax != 0 ) {
	        float bw = (float) lMax / bag;
	        if ( (long)(bw - EPS) == (long)bw )
	            this->bandwidth = (long)bw + 1;
	        else
	            this->bandwidth = bw;
	    }
	}

	inline int getBag() const {
	    return bag;
	}

	inline void setBag(int bag) {
        this->bag = bag;
        if ( lMax != 0 && bag != 0 ) {
            float bw = (float) lMax / bag;
            if ( (long)(bw - EPS) == (long)bw )
                this->bandwidth = (long)bw + 1;
            else
                this->bandwidth = bw;
        }
    }

	inline long getJMax() const {
        return jMax;
    }

	inline void setJMax(long jMax) {
	    this->jMax = jMax;
	}

	inline long getBandwidth() {
	    return bandwidth;
	}

	inline void addPath(Path* path) {
	    route.addPath(path->getDest(), path);
	}

	inline void assign(DataFlow* df) {
	    assert(df->getVirtualLink() == 0);
	    df->assign(this);
	    assignments.insert(df);
	}

	inline void removeAssignment(DataFlow* df) {
	    assert(assignments.find(df) != assignments.end());
	    df->removeAssignment();
	    assignments.erase(df);
	}

	inline void removeAllAssignments() {
	    for ( DataFlows::iterator it = assignments.begin(); it != assignments.end(); ++it ) {
	        (*it)->removeAssignment();
	        removeAssignment(*it);
	    }
	}

	inline DataFlows& getAssignments() {
	    return assignments;
	}

	inline long getResponseTimeEstimation() {
	    return responseTimeEstimation;
	}

	inline void setResponseTimeEstimation(long estimation) {
        responseTimeEstimation = estimation;
    }

private:
	int bag;
	int lMax;
	long jMax; // value in microseconds!

	long bandwidth;

	NetElement* from;
	NetElements to;

	DataFlows assignments;
	Route route;

	// Estimation of the worst case end-to-end response time for frames in microseconds
	long responseTimeEstimation;
};

#endif
