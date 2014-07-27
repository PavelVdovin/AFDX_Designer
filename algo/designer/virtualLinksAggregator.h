#ifndef VIRTUAL_LINKS_AGGREGATOR
#define VIRTUAL_LINKS_AGGREGATOR

#include "defs.h"
#include "verifier.h"

/*
 * This class helps to perform aggregation between virtual links
 */
class VirtualLinksAggregator {
public:
    // Select two virtual links from es to perform aggregation
    static bool selectVLsForAggregation(NetElement* endSystem, VirtualLink** firstVl,
            VirtualLink** secondVl, Verifier::FailedConstraint constraint);

    // Perform virtual links aggregation, returns new virtual link (old ones are deleted)
    static VirtualLink* performAggregation(VirtualLink* firstVl, VirtualLink* secondVl);

private:
    // Select vls from partition to aggregate
    static float findVLsMaxCost(Partition* partition, VirtualLink** first,
            VirtualLink** second, Verifier::FailedConstraint constraint);
};

#endif
