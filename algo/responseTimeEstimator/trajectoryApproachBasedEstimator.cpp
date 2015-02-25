#include "trajectoryApproachBasedEstimator.h"
#include "virtualLink.h"
#include "link.h"
#include "operations.h"

TrajectoryApproachBasedEstimator::TrajectoryApproachBasedEstimator(Network* network, long esFabricDelay, long interFrameDelay, long switchFabricDelay)
    : ResponseTimeEstimator(network, esFabricDelay, interFrameDelay, switchFabricDelay) {
}

float countTransmissionDelay(Network* network, float frameLength, Path* path) {
    float result = 0.0;
    std::list<PathNode>& nodes = path->getPath();

    NetElement* prevElement = path->getSource();
    std::list<PathNode>::iterator nodeIt = nodes.begin();
    assert(nodeIt->first == prevElement);
    ++nodeIt;
    for ( ; nodeIt != nodes.end(); ++nodeIt ) {
        NetElement* node = nodeIt->first;
        assert(prevElement != 0);
        assert(node != 0);
        assert(node != prevElement);
        Link* link = Operations::getLinkByNetElements(network, prevElement, node);
        assert(link != 0);

        result += frameLength / ((float)link->getMaxCapacity() / 1000 + EPS);
        prevElement = node;
    }

    return result;
}

void TrajectoryApproachBasedEstimator::initialize() {
    // Initializing data
    VirtualLinks::iterator it = virtualLinks.begin();
    for ( ; it != virtualLinks.end(); ++it ) {
        VirtualLink* vl = *it;
        JitterAtNetElement jit;
        //vlJitters[vl] = JitterAtNetElement();
        NetElement* source = vl->getSource();
        jit[source] = (float)(vl->getJMax());
        vlJitters[vl] = jit;
    }

    recalculate = true;
}

float TrajectoryApproachBasedEstimator::estimateWorstCaseResponseTime(VirtualLink* vl) {
    while(recalculate) {
       recalculate = false;
       VirtualLinks::iterator it = virtualLinks.begin();
       for ( ; it != virtualLinks.end(); ++it ) {
           float estimation = estimateForOneVl(*it);
           long estimationLong = (long)(estimation - EPS) < (long)estimation ? (long)estimation : (long)estimation + 1;
           (*it)->setResponseTimeEstimation(estimationLong);
       }
    }

    return vl->getResponseTimeEstimation();
}
float TrajectoryApproachBasedEstimator::estimateForOneVl(VirtualLink* vl) {

    Route& route = vl->getRoute();
    Paths& paths = route.getPaths();
    NetElements::iterator it = vl->getDestinations().begin();

    float maxE2e = 0.0;
    Path* worstPath = 0;
    for ( ; it != vl->getDestinations().end(); ++it ) {
        Path* path = route.getPath(*it);
        float jitter = estimateJitterAtNetElement(vl, *it, path);

        // e2eResponse = minimum transmission time + max jitter (jitter = sum delays in output buffers)
        float e2eResponse = (path->getPath().size() - 2 )* switchFabricDelay
            +  countTransmissionDelay(network, (float) vl->getLMax(), path)
            + jitter
            + esFabricDelay; // extra switch counted

        if ( e2eResponse - EPS > maxE2e || worstPath == 0 ) {
            maxE2e = e2eResponse;
            worstPath = path;
        }
    }

    if (worstPath == 0 ) {
    	printf("Worst case response cannot be found for the specified virtual link\n");
    	return -1;
    }

    return maxE2e;
}

NetElement* findPreviousNetElement(NetElement* netElement, Path* path) {
    NetElement* prev = 0;
    std::list<PathNode>& nodes = path->getPath();
    std::list<PathNode>::iterator it = nodes.begin();
    for ( ; it != nodes.end(); ++it ) {
        if ( it->first == netElement ) {
            assert(prev != 0);
            return prev;
        }
        prev = it->first;
    }

    return 0;
}

NetElement* findPreviousNetElement(NetElement* netElement, Route& route) {
    Paths::iterator it = route.getPaths().begin();
    for ( ; it != route.getPaths().end(); ++it ) {
        NetElement* elem = findPreviousNetElement(netElement, it->second);
        if ( elem != 0 )
            return elem;
    }

    assert(0);
    return 0;
}

Port* findOutgoingPort(VirtualLink* vl, NetElement* netElement, Path* path) {
    assert(netElement->isSwitch());
    // Searching for the next element
    std::list<PathNode>& nodes = path->getPath();
    std::list<PathNode>::iterator nit = nodes.begin();
    bool isFound = false;
    NetElement* next = 0;
    for ( ; nit != nodes.end(); ++nit ) {
        if ( isFound ) {
            next = nit->first;
            break;
        }
        if ( nit->first == netElement ) {
            isFound = true;
        }
    }

    assert(next != 0);

    Ports::iterator it = netElement->getPorts().begin();
    for ( ; it != netElement->getPorts().end(); ++it ) {
        if ( (*it)->isAssigned(vl) 
              && (*it)->getAssosiatedLink()->getPortByParent(next) != 0 )
            return *it;
    }

    // Should find!
    assert(0);
    return 0;
}

float TrajectoryApproachBasedEstimator::estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement) {
    /*
    if ( vlJitters[vl].find(netElement) != vlJitters[vl].end() ) {
        return vlJitters[vl][netElement];
    }*/

    Paths::iterator pit = vl->getRoute().getPaths().begin();
    for ( ; pit != vl->getRoute().getPaths().end(); ++pit ) {

        std::list<PathNode>& nodes = (pit->second)->getPath();
        std::list<PathNode>::iterator it = nodes.begin();
        for ( ; it != nodes.end(); ++it ) {
            if ( it->first == netElement ) {
                return estimateJitterAtNetElement(vl, netElement, pit->second);
            }
        }
    }

    assert(0);
    return 0;
}

float TrajectoryApproachBasedEstimator::estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement, Path* path) {
    if ( netElement == vl->getSource() ) {
       assert(vlJitters[vl].find(netElement) != vlJitters[vl].end());
       return vlJitters[vl][netElement];
    }
    /*
    if ( vlJitters[vl].find(netElement) != vlJitters[vl].end() ) {
        return vlJitters[vl][netElement];
    }*/

    // Searching for the previous element
    NetElement* prevElement = findPreviousNetElement(netElement, path);
    //assert(prevElement != 0);
    if ( prevElement == 0 ) {
      	printf("Virtual link's route is not fully connected\n");
      	return 0;
    }
    float jitter = estimateJitterAtNetElement(vl, prevElement, path);

    if ( netElement->isSwitch() ) {
        Port* outgoingPort = findOutgoingPort(vl, netElement, path);
        float delayOnThis = estimateWorstCaseDelay(vl, netElement, outgoingPort);
        jitter += delayOnThis;
        vlJitters[vl][netElement] = jitter; // keeping result
    }

    return jitter;
}

// This includes context required for estimation
class TrajectoryVLEstimator {
public:
    // Estimator
    TrajectoryApproachBasedEstimator* estimator;

    Port* outgoingPort;

    // VirtualLink under consideration
    VirtualLink* vl;

    // Prev net-element of the current vl
    NetElement* prevNetElement;

    // Virtual link of outgoing port
    VirtualLinks virtualLinks;
    VirtualLinks highPriority;

    // Has this virtual link high priority
    bool isHighPriority;

    // Previous net-elements of virtual links
    std::map<NetElement*, VirtualLinks> prevNetElements;

    // current accumulated jitter
    std::map<VirtualLink*, float> jitters;

    // estimated number of frames
    std::map<VirtualLink*, unsigned> numberOfFrames;

    float capacity;

    // interframe gap in bytes
    float ifg;

public:
    // Values estimated during algorithm execution

    // Max value of busy period before arrival of the current frame
    float maxBPBeforeArrival;

    // Min value of busy period before arrival of the current frame
    float minBPBeforeArrival;

    // Busy period before starting transmission of the current frame
    float busyPeriod;

public:
    // Identify whether the virtual link has a higher priority
    inline bool isHigher(VirtualLink* vl) {
        return outgoingPort->isPrioritized()
                && outgoingPort->getAssignedLowPriority().find(this->vl) != outgoingPort->getAssignedLowPriority().end()
                && highPriority.find(vl) != highPriority.end();
    }

    // Identify whether the virtual link has a higher priority
    inline bool isLower(VirtualLink* vl) {
        return outgoingPort->isPrioritized()
                && highPriority.find(this->vl) != highPriority.end()
                && outgoingPort->getAssignedLowPriority().find(vl) != outgoingPort->getAssignedLowPriority().end();
    }

    // Methods used during algorithm
    void initializeValues() {
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            NetElement* prev = findPreviousNetElement(outgoingPort->getParent(), (*it)->getRoute());
            if ( prevNetElements.find(prev) == prevNetElements.end() )
                prevNetElements[prev] = VirtualLinks();

            prevNetElements[prev].insert(*it);
            numberOfFrames[*it] = 1;

            if ( (*it) == vl ) {
                prevNetElement = prev;
            }

            // We need to know all jitters, but this may lead to infinite cycle
            // of jitter calculation. 
            // So we just take the jitter if it is already calculated,
            // otherwise we estimate the number of frames as 1 and later
            // this value is rechecked.

            if ( estimator->getJitterAtNetElement(*it, prev) >= -EPS )
               jitters[*it] = estimator->getJitterAtNetElement(*it, prev);
            else {
               estimator->setRecalculate(true);
            }
        }
    }

    void estimateMaxArrivalBP() {
        maxBPBeforeArrival = 0.0;
        bool calculate = true;
        while ( calculate ) {
            std::map<NetElement*, VirtualLinks>::iterator neIt = prevNetElements.begin();
            for ( ; neIt != prevNetElements.end(); ++neIt ) {
                VirtualLinks& vlsFromOneLink = neIt->second;
                float bp = estimateMaxBPBeforeArrival(vlsFromOneLink);
                if ( bp > maxBPBeforeArrival )
                    maxBPBeforeArrival = bp;
            }

            // Estimate number of frames for priority >= current frame
            VirtualLinks& vls = isHighPriority ? outgoingPort->getAssignedHighPriority() : virtualLinks;

            estimateMinArrivalBP();
            calculate = estimateNumberOfFrames(minBPBeforeArrival, vls);
        }
    }

    void estimateMinArrivalBP() {
        minBPBeforeArrival = 0.0;


        std::map<NetElement*, VirtualLinks>::iterator neIt = prevNetElements.begin();
        for ( ; neIt != prevNetElements.end(); ++neIt ) {
            VirtualLinks& vlsFromOneLink = neIt->second;
            float bp = estimateMinBPBeforeArrival(vlsFromOneLink);
            if ( bp > minBPBeforeArrival )
                minBPBeforeArrival = bp;
        }
    }

    void estimateBP() {
        busyPeriod = 0.0;
        float maxLowPriorityLmax = 0.0;
        bool calculate = true;
        while (calculate) {
            calculate = false;
            VirtualLinks::iterator it = virtualLinks.begin();
            for ( ; it != virtualLinks.end(); ++it ) {
                float lmax = (float)(*it)->getLMax();
                // Calculate only for the priority >= current vl

                if ( !isHighPriority || highPriority.find(*it) != highPriority.end() ) {
                    busyPeriod += numberOfFrames[*it] * lmax + ifg;
                } else {
                    // if the frame is from the same source that the virtual link, then it is already been calculated
                    if ( prevNetElements[prevNetElement].find(*it) != prevNetElements[prevNetElement].end() )
                        lmax -= (float)vl->getLMax();

                    if ( maxLowPriorityLmax < lmax )
                        maxLowPriorityLmax = lmax;
                }

            }

            busyPeriod += maxLowPriorityLmax;
            busyPeriod -= (float) vl->getLMax();

            if ( !isHighPriority && highPriority.size() > 0 ) {
                calculate = estimateNumberOfFrames(busyPeriod, outgoingPort->getAssignedHighPriority());
            }
        }
    }

private:

    // Estimate max bp of vls from one physical link
    float estimateMaxBPBeforeArrival(VirtualLinks& virtualLinks) {
        float bp = 0.0,
              maxVlFramesDelay = 0.0,
              maxLmaxLowPriority = 0.0; // it is still 0.0 if !isHighPriority
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            float lmax = (float)(*it)->getLMax();

            // for high priority, count only max lmax from low priority
            if ( !isHighPriority || highPriority.find(*it) != highPriority.end() ) {
                bp +=  lmax * numberOfFrames[*it] + ifg;

                // jitter is in microseconds, converting to milliseconds
                float jitterVal = 0.0;
                if ( jitters.find(*it) != jitters.end() )
                    jitterVal = jitters[*it] / 1000;

                float vlFramesDelay = (float)(numberOfFrames[*it] - 1) * (*it)->getBag() - jitterVal;
                vlFramesDelay *= capacity; // getting bytes
                vlFramesDelay +=  + lmax + ifg;
                if (  vlFramesDelay > maxVlFramesDelay )
                    maxVlFramesDelay = vlFramesDelay;
            } else if ( maxLmaxLowPriority < lmax ) {
                assert (isHighPriority && highPriority.find(*it) == highPriority.end());
                maxLmaxLowPriority = lmax;
            } else
                assert(0);
        }

        bp += maxLmaxLowPriority + ifg;
        if ( bp < maxVlFramesDelay )
            bp = maxVlFramesDelay;

        return bp;
    }

    // Estimate min bp of vls from one physical link
    float estimateMinBPBeforeArrival(VirtualLinks& virtualLinks) {
        float bp = 0.0,
              maxLmax = 0.0;
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            float lmax = (float)(*it)->getLMax();

            // for high priority, count only max lmax from low priority
            if ( !isHigher(*it) && !isLower(*it) ) {
                bp +=  lmax * numberOfFrames[*it] + ifg;
                if ( maxLmax < lmax ) {
                    maxLmax = lmax;
                }
            }
        }

        bp -= (maxLmax + ifg);
        return bp;
    }

    // Returns the fact that number is changed somewhere
    bool estimateNumberOfFrames(float bp, VirtualLinks& virtualLinks) {
        // Updating number-of-frames according to bp
        bool changed = false;
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            // capacity is in bytes/milliseconds,
            // bag is in milliseconds
            // jitter is in microseconds, so transmitting to milliseconds

            if ( jitters.find(*it) != jitters.end() ) {
               int newNum  = 1 + (int)(((bp + (*it)->getLMax() + ifg) / capacity + jitters[*it] / 1000) / (float)(*it)->getBag());

               if ( newNum > numberOfFrames[*it] ) {
                   numberOfFrames[*it] = newNum;
                   changed = true;
               }

               // Check that this newNum didn't change, otherwise we need to
               // recalculate all e2e estimates later
               if ( newNum > estimator->getVlsNumberOfFrames(*it, outgoingPort->getParent()) ) {
                  estimator->setRecalculate(true);
                  estimator->setVlsNumberOfFrames(*it, outgoingPort->getParent(), newNum);
               }
            } else {
               // We still didn't count jitter for the virtual link,
               // so we estimate number of frames as estimated
               int newNum = estimator->getVlsNumberOfFrames(*it, outgoingPort->getParent());
               if ( newNum > numberOfFrames[*it] ) {
                   numberOfFrames[*it] = newNum;
                   changed = true;
               }
            }
        }
        return changed;
    }
};

// The most difficult func in this whole project!!!
float TrajectoryApproachBasedEstimator::estimateWorstCaseDelay(VirtualLink* vl, NetElement* netElement, Port* outgoingPort) {
    VirtualLinks highPriority;
    if ( outgoingPort->isPrioritized() )
    	highPriority = outgoingPort->getAssignedHighPriority();
    VirtualLinks& lowPriority = outgoingPort->getAssignedLowPriority();

    VirtualLinks virtualLinks(lowPriority.begin(), lowPriority.end());
    virtualLinks.insert(highPriority.begin(), highPriority.end());

    // initializing estimator object
    TrajectoryVLEstimator estimator;
    estimator.estimator = this;
    estimator.vl = vl;
    estimator.prevNetElement = 0;
    estimator.outgoingPort = outgoingPort;
    estimator.isHighPriority = (highPriority.find(vl) != highPriority.end());
    estimator.virtualLinks.clear();
    estimator.virtualLinks.insert(virtualLinks.begin(), virtualLinks.end());
    estimator.highPriority = highPriority;
    estimator.capacity = outgoingPort->getAssosiatedLink()->getMaxCapacity();
    estimator.ifg = interFrameDelay * (estimator.capacity / 1000); // counting in bytes, then get microseconds

    estimator.initializeValues();
    assert(estimator.prevNetElement != 0);
    estimator.estimateMaxArrivalBP();
    estimator.estimateMinArrivalBP();
    estimator.estimateBP();

    return 1000 * (estimator.busyPeriod - estimator.minBPBeforeArrival) / estimator.capacity;
}
