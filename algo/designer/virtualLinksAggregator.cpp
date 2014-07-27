#include "virtualLinksAggregator.h"
#include "netelement.h"
#include "endsystem.h"
#include "virtualLink.h"
#include "virtualLinkConfigurator.h"
#include "operations.h"

#define BW_RATION 0.5
#define costVl costPeriods

bool VirtualLinksAggregator::selectVLsForAggregation(NetElement* endSystem, VirtualLink** firstVl,
        VirtualLink** secondVl, Verifier::FailedConstraint constraint) {
    Partitions partitions = endSystem->toEndSystem()->getPartitions();
    Partitions::iterator it = partitions.begin();

    float maxCost = 0.0;
    for ( ; it != partitions.end(); ++it ) {
        VirtualLink *currentFirst = 0, *currentSecond = 0;
        float cost = findVLsMaxCost(*it, &currentFirst, &currentSecond, constraint);
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

float VirtualLinksAggregator::findVLsMaxCost(Partition* partition, VirtualLink** first,
        VirtualLink** second, Verifier::FailedConstraint constraint) {
    float firstMax = 0.0, secondMax = 0.0;
    VirtualLinks assigned = Operations::getLinksFromPartition(partition);
    if ( assigned.size() < 2 )
        return 0.0;

    VirtualLinks:: iterator it = assigned.begin();
    for ( ; it != assigned.end(); ++it ) {
        VirtualLink* vl = *it;
        float cost;
        if ( constraint == Verifier::CAPACITY)
            cost = costVl(vl);
        else
            cost = costLMax(vl);

        if ( firstMax < EPS || cost > firstMax ) {
            secondMax = firstMax;
            *second = *first;
            firstMax = cost;
            *first = vl;
        } else if ( secondMax < EPS || cost > secondMax ) {
            secondMax = cost;
            *second = vl;
        }
    }
    return firstMax * secondMax;
}
