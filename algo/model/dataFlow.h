#ifndef DATA_FLOW
#define DATA_FLOW

#include "defs.h"

class DataFlow {
    friend class Factory;
public:
	DataFlow(long period = 1, long msgSize = 1000, long tMax = 0, long maxJitter = 0):
		from(0),
		period(period),
		msgSize(msgSize),
		tMax(tMax),
		maxJitter(maxJitter),
		assignedTo(0),
		responseTime(0)
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

	inline long getTMax() const {
        return tMax;
    }

	inline long getMsgSize() const {
		return msgSize;
	}

	inline long getMaxJitter() const {
		return maxJitter;
	}

	inline VirtualLink* getVirtualLink() {
	    return assignedTo;
	}

	inline void assign(VirtualLink* vl) {
	    assignedTo = vl;;
	}

	inline void removeAssignment() {
	    assignedTo = 0;
	}

	inline void setResponseTime(long responseTime) {
	    this->responseTime = responseTime;
	}

	inline long getResponseTime() {
	    return responseTime;
	}

private:
	Partition* from;
	Partitions to;
	long period;
	long msgSize;
	long tMax;
	long maxJitter;

	// in microseconds
	long responseTime;

	VirtualLink* assignedTo;
};

#endif
