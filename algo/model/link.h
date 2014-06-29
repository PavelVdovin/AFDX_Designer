#ifndef LINK
#define LINK

#include "defs.h"

class Link {
public:
	Link(Port* port1, Port* port2, long capacity = 100000):
		port1(port1),
		port2(port2),
		maxCapacity(capacity),
		freeCapacityFromPort1(capacity),
		freeCapacityFromPort2(capacity) {
	}

	inline Port* getPort1() const {
		return port1;
	}

	inline Port* getPort2() const {
		return port2;
	}

	inline long getMaxCapacity() const {
		return maxCapacity;
	}

	inline long getFreeCapacityFromPort1() const {
		return freeCapacityFromPort1;
	}

	inline long getFreeCapacityFromPort2() const {
		return freeCapacityFromPort2;
	}

private:
	Port* port1;
	Port* port2;
	long maxCapacity;
	long freeCapacityFromPort1;
	long freeCapacityFromPort2;
};

#endif
