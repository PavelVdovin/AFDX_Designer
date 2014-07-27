#ifndef PARTITION
#define PARTITION

#include "defs.h"

class Partition {
    friend class Factory;
public:
	Partition(): connectedTo(0) {
	}

	inline NetElement* getConnected() const {
		return connectedTo;
	}

	inline DataFlows& getOutgoingDataFlows() {
	    return outgoingDataFlows;
	}

private:
	NetElement* connectedTo;
	DataFlows outgoingDataFlows;
};
#endif
