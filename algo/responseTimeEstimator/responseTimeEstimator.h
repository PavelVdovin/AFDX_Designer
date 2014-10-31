#ifndef RESPONSE_TIME_ESTIMATOR
#define RESPONSE_TIME_ESTIMATOR

#include "defs.h"
/**
 * This is an interface to estimate end-to-end response time of frames
 * transmitting on specified virtual links
 */
class ResponseTimeEstimator {
public:
    ResponseTimeEstimator(Network* network, long esDelay = 0, long ifg = 0, long switchDelay = 16):
        network(network), virtualLinks(virtualLinks)
    {
        esFabricDelay = esDelay;
        interFrameDelay = ifg;
        switchFabricDelay = switchDelay;
    }

    virtual ~ResponseTimeEstimator() {}

    /*
     * Initialize required data.
     */
    virtual void initialize() = 0;

    /*
     * Returns estimated end-to-end response time in microseconds.
     */
    virtual float estimateWorstCaseResponseTime(VirtualLink* vl) = 0;

    /*
     * Calculate response time for all virtual links
     */
    virtual void estimateWorstCaseResponseTime();

    inline void setVirtualLinks (VirtualLinks virtualLinks) {
        this->virtualLinks = virtualLinks;
    }

    static inline long getEsFabricDelay() {
        return esFabricDelay;
    }

    static inline long getInterframeGap() {
        return interFrameDelay;
    }

    static inline long getSwitchFabricDelay() {
        return switchFabricDelay;
    }

protected:
    Network* network;
    VirtualLinks virtualLinks;

    // Const delays in microseconds
    static long esFabricDelay;
    static long interFrameDelay;
    static long switchFabricDelay;
};


#endif
