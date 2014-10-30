#ifndef RESPONSE_TIME_ESTIMATOR
#define RESPONSE_TIME_ESTIMATOR

#include "defs.h"
/**
 * This is an interface to estimate end-to-end response time of frames
 * transmitting on specified virtual links
 */
class ResponseTimeEstimator {
public:
    ResponseTimeEstimator(Network* network, long esFabricDelay = 0, long interFrameDelay = 0, long switchFabricDelay = 16):
        network(network), virtualLinks(virtualLinks), esFabricDelay(esFabricDelay),
        interFrameDelay(interFrameDelay), switchFabricDelay(switchFabricDelay)
    {}

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

protected:
    Network* network;
    VirtualLinks virtualLinks;

    // Const delays in microseconds
    const long esFabricDelay;
    const long interFrameDelay;
    const long switchFabricDelay;
};


#endif
