#include "virtualLinksAggregator.h"
#include "netelement.h"
#include "endsystem.h"
#include "link.h"
#include "virtualLink.h"
#include "virtualLinkConfigurator.h"
#include "operations.h"

#include <vector>
#include <algorithm>

#define BW_RATION 0.5
#define costVl costPeriods

typedef std::pair<VirtualLink*, VirtualLink*> VLSPair;

bool VirtualLinksAggregator::selectVLsForAggregation(NetElement* endSystem, VirtualLink** firstVl,
        VirtualLink** secondVl, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated) {
    Partitions partitions = endSystem->toEndSystem()->getPartitions();
    Partitions::iterator it = partitions.begin();

    float maxCost = 0.0;
    for ( ; it != partitions.end(); ++it ) {
        VirtualLink *currentFirst = 0, *currentSecond = 0;
        float cost = findVLsMaxCost(*it, &currentFirst, &currentSecond, constraint, deprecated);
        if ( cost > EPS && (maxCost < EPS || cost > maxCost )) {
            *firstVl = currentFirst;
            *secondVl = currentSecond;
            maxCost = cost;
        }
    }

    return (maxCost > EPS);
}

bool VirtualLinksAggregator::selectVLsForAggregation(Network* network, Route& route, VirtualLink** firstVl,
            VirtualLink** secondVl, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated) {
    VirtualLinks vlsToParse;

    Paths& paths = route.getPaths();
    Paths::iterator it = paths.begin();
    for ( ; it != paths.end(); ++it ) {
        Path* path = it->second;
        std::list<PathNode>& nodes = path->getPath();
        std::list<PathNode>::iterator nit = nodes.begin();
        assert(nit != nodes.end());
        NetElement* prev = nit->first;
        ++nit;
        for ( ; nit != nodes.end(); ++nit ) {
            // We need to get all virtual links which has an intersection with the path of our route
            NetElement* next = nit->first;
            Link* link = Operations::getLinkByNetElements(network, prev, next);
            Port* port = link->getPort1();
            if ( prev->getPorts().find(port) == prev->getPorts().end()) {
                port = link->getPort2();
                assert(prev->getPorts().find(port) != prev->getPorts().end());
            }
            VirtualLinks& vls = link->getAssignedFromPort(port);
            vlsToParse.insert(vls.begin(), vls.end());

            prev = next;
        }
    }

    // Now, form virtualLinks with the same partition
    std::map<Partition*, VirtualLinks> vlsOfPartition;
    VirtualLinks::iterator vit = vlsToParse.begin();
    for ( ; vit != vlsToParse.end(); ++vit ) {
        assert((*vit)->getAssignments().size() > 0);

        // assuming all data flows are going from one partition
        Partition* p = (*(*vit)->getAssignments().begin())->getFrom();
        if ( vlsOfPartition.find(p) == vlsOfPartition.end() )
            vlsOfPartition[p] = VirtualLinks();
        vlsOfPartition[p].insert(*vit);
    }

    // Finding vls of max cost
    std::map<Partition*, VirtualLinks>::iterator pit = vlsOfPartition.begin();
    float maxCost = 0.0;
    for ( ; pit != vlsOfPartition.end(); ++pit ) {
        if ( (pit->second).size() > 1 ) {
            VirtualLink *currentFirst = 0, *currentSecond = 0;

            float cost = findVLsMaxCost(pit->second, &currentFirst, &currentSecond, constraint, deprecated);
            if ( cost > EPS && (maxCost < EPS || cost > maxCost )) {
                *firstVl = currentFirst;
                *secondVl = currentSecond;
                maxCost = cost;
            }
        }
    }
}

float costStandard(VirtualLink* vl) {
    if ( vl->getAssignments().size() == 0 )
        return 0.0;
    return (float)(vl->getBandwidth()) / vl->getAssignments().size();
}

float costPeriods(VirtualLink* vl) {
    DataFlows::iterator it = vl->getAssignments().begin();
    long minTMax = 0.0;
    for ( ; it != vl->getAssignments().end(); ++it ) {
        if ( minTMax == 0 || (*it)->getTMax() < minTMax )
            minTMax = (*it)->getTMax();
    }

    return (BW_RATION * (float) vl->getBandwidth() + (1.0 - BW_RATION) * (float)minTMax) / vl->getAssignments().size();
}

float costLMax(VirtualLink* vl) {
    return (float) vl->getLMax() / vl->getAssignments().size();
}

VirtualLink* VirtualLinksAggregator::performAggregation(VirtualLink* firstVl, VirtualLink* secondVl) {
    DataFlows dataFlows(firstVl->getAssignments().begin(), firstVl->getAssignments().end());
    dataFlows.insert(secondVl->getAssignments().begin(), secondVl->getAssignments().end());
    VirtualLink* result = VirtualLinkConfigurator::generateAggregatedVirtualLink(dataFlows);
    return result;
}

// Get number of different destinations for the specified virtual links
inline int getNumberOfDiffDests(VirtualLink* vl1, VirtualLink* vl2) {
    NetElements& dests1 = vl1->getDestinations(), &dests2 = vl2->getDestinations();
    int numberOfEqElems = 0;
    for ( NetElements::iterator it = dests1.begin(); it != dests1.end(); ++it ) {
        if ( dests2.find(*it) != dests2.end() ) {
            ++numberOfEqElems;
        }
    }

    return dests1.size() + dests2.size() - 2 * numberOfEqElems;
}

inline float costVlPair(VirtualLink* vl1, VirtualLink* vl2) {
    return costVl(vl1) * costVl(vl2) / ( 1 + getNumberOfDiffDests(vl1, vl2));
}

inline float costLMaxPair(VirtualLink* vl1, VirtualLink* vl2) {
    return costLMax(vl1) * costLMax(vl2) / ( 1 + getNumberOfDiffDests(vl1, vl2));
}

bool sortVl(const VLSPair& pair1, const VLSPair& pair2) {
    float val1 = costVlPair(pair1.first, pair1.second),
          val2 = costVlPair(pair2.first, pair2.second);
    return val1 > val2;
}

bool sortLMax(const VLSPair& pair1, const VLSPair& pair2) {
    float val1 = costLMaxPair(pair1.first, pair1.second),
          val2 = costLMaxPair(pair2.first, pair2.second);
    return val1 > val2;
}

float VirtualLinksAggregator::findVLsMaxCost(Partition* partition, VirtualLink** first,
        VirtualLink** second, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated) {
    float firstMax = 0.0, secondMax = 0.0;
    VirtualLinks assigned = Operations::getLinksFromPartition(partition);
    if ( assigned.size() < 2 )
        return 0.0;

    return findVLsMaxCost(assigned, first, second, constraint, deprecated);
}

// Select vls from partition to aggregate, only specified virtual links are allowed
float VirtualLinksAggregator::findVLsMaxCost(VirtualLinks& vlsOfPartition, VirtualLink** first,
            VirtualLink** second, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated) {
    // vector of all virtual link pairs
    int n = vlsOfPartition.size();
    if ( n < 2 )
        return 0.0;
    std::vector<VLSPair> sortedVls;
    sortedVls.reserve( n * (n - 1) / 2 );
    VirtualLinks::iterator it = vlsOfPartition.begin();
    for ( ; it != vlsOfPartition.end(); ++it ) {
        VirtualLinks::iterator jt = it;
        for ( ++jt; jt != vlsOfPartition.end(); ++jt ) {
            sortedVls.push_back(std::pair<VirtualLink*, VirtualLink*>(*it, *jt));
        }
    }

    if ( constraint == Verifier::CAPACITY)
        std::sort(sortedVls.begin(), sortedVls.end(), sortVl);
    else
        std::sort(sortedVls.begin(), sortedVls.end(), sortLMax);

    for ( int i = 0; i < sortedVls.size(); ++i ) {
        if ( deprecated.find(sortedVls[i]) == deprecated.end() ) {
            *first = sortedVls[i].first;
            *second = sortedVls[i].second;

            // Divide by (1 + "number of different destinations") so that
            // to encourage vls with the same destinations
            if ( constraint == Verifier::CAPACITY )
                return costVlPair(sortedVls[i].first, sortedVls[i].second);
            else
                return costLMaxPair(sortedVls[i].first, sortedVls[i].second);;
        }
    }
    return 0.0;
}
