#ifndef TRAJECTORY_APPROACH_BASED_ESTIMATOR
#define TRAJECTORY_APPROACH_BASED_ESTIMATOR

#include "responseTimeEstimator.h"
#include <map>

class TrajectoryApproachBasedEstimator: public ResponseTimeEstimator {
    friend class TrajectoryVLEstimator;
    typedef std::map<NetElement*, float> JitterAtNetElement;
    typedef std::map<VirtualLink*, JitterAtNetElement> VLJitters;

    // This type keeps the virtual link to recalculate 
    //
    typedef std::map<VirtualLink*, std::map<NetElement*, int> > VlsNumberOfFrames;
public:
    TrajectoryApproachBasedEstimator(Network* network, long esFabricDelay = 0, long interFrameDelay = 0, long switchFabricDelay = 16);

    virtual ~TrajectoryApproachBasedEstimator() {}

    void initialize();
    float estimateWorstCaseResponseTime(VirtualLink* vl);

    inline void setRecalculate(bool recalc) {
       recalculate = recalc;
    }

    inline float getJitterAtNetElement(VirtualLink* vl, NetElement* netElement) {
      if ( vlJitters[vl].find(netElement) != vlJitters[vl].end() ) {
        return vlJitters[vl][netElement];
      }
      return -1.0;
    }

    inline void setVlsNumberOfFrames(VirtualLink* vl, NetElement* elem, int num) {
        if ( vlsNumberOfFrames.find(vl) == vlsNumberOfFrames.end() ) {
            vlsNumberOfFrames[vl] = std::map<NetElement*, int>();
        }
        vlsNumberOfFrames[vl][elem] = num;
    }

    inline int getVlsNumberOfFrames(VirtualLink* vl, NetElement* elem) {
        if ( vlsNumberOfFrames.find(vl) != vlsNumberOfFrames.end() &&
              vlsNumberOfFrames[vl].find(elem) != vlsNumberOfFrames[vl].end() ) {
           return vlsNumberOfFrames[vl][elem];
        }

        setVlsNumberOfFrames(vl, elem, 1);
        return 1;
    }


private:

    // Estimate wcrt for one virtual link implementation
   float estimateForOneVl(VirtualLink* vl);

    /*
     * Estimate and save the worst case response time (and jitter) to the specified net-element
     */
    float estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement);
    float estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement, Path* path);
    float estimateWorstCaseDelay(VirtualLink* vl, NetElement* netElement, Port* outgoingPort);

private:
    VLJitters vlJitters;

    VlsNumberOfFrames vlsNumberOfFrames;
    bool recalculate;
};

#endif
