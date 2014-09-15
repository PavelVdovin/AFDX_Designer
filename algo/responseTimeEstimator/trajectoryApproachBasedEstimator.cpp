#include "trajectoryApproachBasedEstimator.h"
#include "virtualLink.h"
#include "link.h"
#include "operations.h"

TrajectoryApproachBasedEstimator::TrajectoryApproachBasedEstimator(Network* network, VirtualLinks& virtualLinks, long interFrameDelay, long switchFabricDelay)
    : ResponseTimeEstimator(network, virtualLinks, interFrameDelay, switchFabricDelay) {
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

        result += frameLength / (link->getMaxCapacity() + EPS);
        prevElement = node;
    }

    return result;
}

float TrajectoryApproachBasedEstimator::estimateWorstCaseResponseTime(VirtualLink* vl) {
	// Initializing data
	vlJitters[vl] = JitterAtNetElement();
	NetElement* source = vl->getSource();
	vlJitters[vl][source] = (float)vl->getJMax();

    Route& route = vl->getRoute();
    Paths& paths = route.getPaths();
    NetElements::iterator it = vl->getDestinations().begin();

    float maxJitter = 0.0;
    Path* worstPath = 0;
    for ( ; it != vl->getDestinations().end(); ++it ) {
        Path* path = route.getPath(*it);
        float jitter = estimateJitterAtNetElement(vl, *it, path);
        if ( jitter - EPS > maxJitter ) {
            maxJitter = jitter;
            worstPath = path;
        }
    }

    if (worstPath == 0 ) {
    	printf("Worst case response cannot be found for the specified virtual link\n");
    	return -1;
    }

    // e2eResponse = minimum transmission time + max jitter (jitter = sum delays in output buffers)
    float e2eResponse = worstPath->getPath().size()* ( switchFabricDelay +  countTransmissionDelay(network, (float) vl->getLMax(), worstPath) )
            - switchFabricDelay + maxJitter; // extra switch counted

    return e2eResponse;
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

Port* findOutgoingPort(VirtualLink* vl, NetElement* netElement) {
    assert(netElement->isSwitch());
    Ports::iterator it = netElement->getPorts().begin();
    for ( ; it != netElement->getPorts().end(); ++it ) {
        if ( (*it)->isAssigned(vl) )
            return *it;
    }

    // Should find!
    assert(0);
    return 0;
}

float TrajectoryApproachBasedEstimator::estimateJitterAtNetElement(VirtualLink* vl, NetElement* netElement) {
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
    if ( vlJitters[vl].find(netElement) != vlJitters[vl].end() ) {
        return vlJitters[vl][netElement];
    }

    // Searching for the previous element
    NetElement* prevElement = findPreviousNetElement(netElement, path);
    //assert(prevElement != 0);
    if ( prevElement == 0 ) {
    	printf("Virtual link's route is not fully connected\n");
    	return 0;
    }
    float jitter = estimateJitterAtNetElement(vl, prevElement, path);

    if ( netElement->isSwitch() ) {
        Port* outgoingPort = findOutgoingPort(vl, netElement);
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
    // Methods used during algorithm
    void initializeValues() {
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            NetElement* prev = findPreviousNetElement(outgoingPort->getParent(), (*it)->getRoute());
            if ( prevNetElements.find(prev) == prevNetElements.end() )
                prevNetElements[prev] = VirtualLinks();

            prevNetElements[prev].insert(*it);
            numberOfFrames[*it] = 1;

            // We need to know all jitters!
            jitters[*it] = estimator->estimateJitterAtNetElement(*it, prev);
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
            calculate = estimateNumberOfFrames(maxBPBeforeArrival, vls);
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
                } else if ( maxLowPriorityLmax < lmax )
                    maxLowPriorityLmax = lmax;

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
                float vlFramesDelay = (float)(numberOfFrames[*it] - 1) * (*it)->getBag() - jitters[*it];
                vlFramesDelay *= capacity;
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
              maxLmax = 0.0,
              maxLmaxLowPriority = 0.0;
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            float lmax = (float)(*it)->getLMax();

            // for high priority, count only max lmax from low priority
            if ( !isHighPriority || highPriority.find(*it) != highPriority.end() ) {
                bp +=  lmax * numberOfFrames[*it] + ifg;
                if ( maxLmax < lmax ) {
                    maxLmax = lmax;
                }
            }  else if ( maxLmaxLowPriority < lmax ) {
                assert (isHighPriority && highPriority.find(*it) == highPriority.end());
                maxLmaxLowPriority = lmax;
            } else
                assert(0);
        }

        bp -= (maxLmax + ifg);
        bp += maxLmaxLowPriority;
        return bp;
    }

    // Returns the fact that number is changed somewhere
    bool estimateNumberOfFrames(float bp, VirtualLinks& virtualLinks) {
        // Updating number-of-frames according to bp
        bool changed = false;
        VirtualLinks::iterator it = virtualLinks.begin();
        for ( ; it != virtualLinks.end(); ++it ) {
            int newNum  = 1 + (int)(((bp + (*it)->getLMax() + ifg) / capacity + jitters[*it]) / (float)((*it)->getBag() * 1000)) + 1;

            if ( newNum > numberOfFrames[*it] ) {
                numberOfFrames[*it] = newNum;
                changed = true;
            }
        }
        return changed;
    }
};
/*
 * The first step of the algorithm: estimation of buffers from the physical links before arrival of the current frame
float estimateBusyPeriodBeforeCurrentFrame(VirtualLink* vl, VirtualLinks& virtualLinks, bool isHighPriority,
        std::map<NetElement*, VirtualLinks> prevNetElements, std::map<VirtualLink*, unsigned>& numberOfFrames) {
    VirtualLinks& vls = virtualLinks;
    if ( isHighPriority ) {
        vls = vl->
    }
}
*/

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
    estimator.outgoingPort = outgoingPort;
    estimator.isHighPriority = (highPriority.find(vl) != highPriority.end());
    estimator.virtualLinks.insert(virtualLinks.begin(), virtualLinks.end());
    estimator.highPriority = highPriority;
    estimator.capacity = outgoingPort->getAssosiatedLink()->getMaxCapacity();
    estimator.ifg = interFrameDelay * (estimator.capacity / 1000); // counting in bytes, then get microseconds

    estimator.initializeValues();
    estimator.estimateMaxArrivalBP();
    estimator.estimateMinArrivalBP();
    estimator.estimateBP();

    return (estimator.busyPeriod - estimator.minBPBeforeArrival) / estimator.capacity;
}
