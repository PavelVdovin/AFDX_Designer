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

bool sortVl(VirtualLink* vl1, VirtualLink* vl2) {
    return costVl(vl1) > costVl(vl2);
}

bool sortLMax(VirtualLink* vl1, VirtualLink* vl2) {
    return costLMax(vl1) > costLMax(vl2);
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
    std::vector<VirtualLink*>sortedVls(vlsOfPartition.begin(), vlsOfPartition.end());
    if ( constraint == Verifier::CAPACITY)
        std::sort(sortedVls.begin(), sortedVls.end(), sortVl);
    else
        std::sort(sortedVls.begin(), sortedVls.end(), sortLMax);

    // This is considered to be rather fast, as any but first results are returned very seldom
    for ( int indexMax = 0; indexMax < (sortedVls.size() - 1) ; ++indexMax ) {
        VirtualLink* max = sortedVls[indexMax];
        for ( int i = indexMax + 1; i < sortedVls.size(); ++i ) {
            VirtualLink* vl = sortedVls[i];;
            std::pair<VirtualLink*, VirtualLink*> vls(max, vl);
            if ( deprecated.find(vls) == deprecated.end() ) {
                *first = max;
                *second = vl;
                if ( constraint == Verifier::CAPACITY )
                    return costVl(max) * costVl(vl);
                else
                    return costLMax(max) * costLMax(vl);
            }
        }
    }
    return 0.0;
}
