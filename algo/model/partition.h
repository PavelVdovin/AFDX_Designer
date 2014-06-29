#ifndef PARTITION
#define PARTITION

#include "defs.h"

class Partition {
    friend class Factory;
public:
	Partition(): connectedTo(0) {
	}

	NetElement* getConnected() const {
		return connectedTo;
	}

private:
	NetElement* connectedTo;
};
#endif
