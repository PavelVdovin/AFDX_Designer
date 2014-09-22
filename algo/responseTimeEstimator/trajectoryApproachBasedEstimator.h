#ifndef TRAJECTORY_APPROACH_BASED_ESTIMATOR
#define TRAJECTORY_APPROACH_BASED_ESTIMATOR

#include "responseTimeEstimator.h"
#include <map>

class TrajectoryApproachBasedEstimator: public ResponseTimeEstimator {
    friend class TrajectoryVLEstimator;
    typedef std::map<NetElement*, float> JitterAtNetElement;
    typedef std::map<VirtualLink*, JitterAtNetElement> VLJitters;
public:
    TrajectoryApproachBasedEstimator(Network* network, VirtualLinks& virtualLinks, long interFrameDelay = 0, long switchFabricDelay = 16);

    void initialize();
    float estimateWorstCaseResponseTime(VirtualLink* vl);

private:

    /*
     * Estimate and save the worst case response time (and jitter) to the specified net-element
     */
    float estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement);
    float estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement, Path* path);
    float estimateWorstCaseDelay(VirtualLink* vl, NetElement* netElement, Port* outgoingPort);

private:
    VLJitters vlJitters;
};

#endif
