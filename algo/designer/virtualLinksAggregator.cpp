#include "virtualLinksAggregator.h"
#include "netelement.h"
#include "endsystem.h"
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



    std::vector<VirtualLink*>sortedVls(assigned.begin(), assigned.end());
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
