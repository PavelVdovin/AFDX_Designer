#ifndef DATA_FLOW
#define DATA_FLOW

#include "defs.h"

class DataFlow {
    friend class Factory;
public:
	DataFlow(long period = 1, long msgSize = 1000, long maxJitter = 0):
		from(0),
		period(period),
		msgSize(msgSize),
		maxJitter(maxJitter),
		assignedTo(0)
	{}

	inline Partition* getFrom() const {
		return from;
	}

	inline Partitions& getTo() {
		return to;
	}

	inline long getPeriod() const {
		return period;
	}

	inline long getMsgSize() const {
		return msgSize;
	}

	inline long getMaxJitter() const {
		return maxJitter;
	}

private:
	Partition* from;
	Partitions to;
	long period;
	long msgSize;
	long maxJitter;

	VirtualLink* assignedTo;
};

#endif
