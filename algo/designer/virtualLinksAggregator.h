#ifndef VIRTUAL_LINKS_AGGREGATOR
#define VIRTUAL_LINKS_AGGREGATOR

#include "defs.h"
#include "verifier.h"

/*
 * This class helps to perform aggregation between virtual links
 */
class VirtualLinksAggregator {
public:
    typedef std::set<std::pair<VirtualLink*, VirtualLink*> > DeprecatedVLPairs;
    typedef std::map<Partition*, VirtualLinks> VLsOfPartition;

    // Select two virtual links from es to perform aggregation.
    // The last parameter contains already parsed pairs.
    static bool selectVLsForAggregation(NetElement* endSystem, VirtualLink** firstVl,
            VirtualLink** secondVl, Verifier::FailedConstraint constraint,
            DeprecatedVLPairs& deprecated);

    // Select two virtual links assigned to specified route for aggregation.
    // The last parameter contains already parsed pairs.
    static bool selectVLsForAggregation(Network* network, Route& route, VirtualLink** firstVl,
            VirtualLink** secondVl, Verifier::FailedConstraint constraint,
            DeprecatedVLPairs& deprecated);

    // Perform virtual links aggregation, returns new virtual link (old ones are deleted)
    static VirtualLink* performAggregation(VirtualLink* firstVl, VirtualLink* secondVl);

private:
    // Select vls from partition to aggregate
    static float findVLsMaxCost(Partition* partition, VirtualLink** first,
            VirtualLink** second, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated);

    // Select vls from partition to aggregate, only specified virtual links are allowed
    static float findVLsMaxCost(VirtualLinks& vls, VirtualLink** first,
            VirtualLink** second, Verifier::FailedConstraint constraint, DeprecatedVLPairs& deprecated);
};

#endif
