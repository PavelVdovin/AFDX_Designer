#ifndef DESIGNER
#define DESIGNER

#include "defs.h"
#include "verifier.h"
#include "responseTimeEstimator.h"
#include "trajectoryApproachBasedEstimator.h"

class Designer {
public:
    Designer(Network* network, Partitions& partitions, DataFlows& dataFlows, VirtualLinks& existingVirtualLinks,
            int esDelay = 0, int switchDelay = 16, int ifg = 0,
            bool disableAggregationOnSource = false,
            bool disableAggregationOnResponseTime = false,
            bool disableLimitedSearch = false,
            bool disableRedesign = false,
            int limitedSearchDepth = 2):
        network(network),
        partitions(partitions),
        dataFlows(dataFlows),
        existingVirtualLinks(existingVirtualLinks),
        disableAggregationOnSource(disableAggregationOnSource),
        disableAggregationOnResponseTime(disableAggregationOnResponseTime),
        disableLimitedSearch(disableLimitedSearch),
        disableRedesign(disableRedesign),
        limitedSearchDepth(limitedSearchDepth)
         {
        responseTimeEstimator = new TrajectoryApproachBasedEstimator(network, esDelay, ifg, switchDelay);
    }

    ~Designer();

    void design();

    inline VirtualLinks& getDesignedVirtualLinks() {
        return designedVirtualLinks;
    }

private:

    void setDataFlowVL(VirtualLink* vl, DataFlow* df);

    /*
     * The first step of algorithm: generate virtual links
     * before them being routed.
     */
    void designUnroutedVLs();

    /*
     * The second step.
     * Check outgoing virtual links constraints and try to fix them
     */
    void checkOutgoingVirtualLinks(bool checkCapacity = true);
    void checkOutgoingVirtualLinks(NetElement* endSystem, bool checkCapacity = true);

    /*
     * Redesign outgoing virtual links if some constraints are failed on specified port.
     */
    void redesignOutgoingVirtualLinks(Port* port, Verifier::FailedConstraint);
    void removeVirtualLink(Port* port, VirtualLink*);
    void removeVirtualLink(VirtualLink*);

    /*
     * Remove only the most constrained data flow instead of removing the whole virtual link
     */
    void removeMostConstrainedVirtualLink(Port* port, VirtualLink* vlToDrop, Verifier::FailedConstraint failed);

    /*
     * Third step of virtual links assignment: routing
     */
    void routeVirtualLinks();

    /*
     * Limited exhaustive search for routing operation
     */
    bool limitedSearch(VirtualLink* virtualLink, VirtualLinks& assigned);

    // Calculate and check response e2e time for the specified virtual links and their data flows.
    // If the second parameter is true, then redesigning procedure is performed for failed virtual links
    bool calculateAndCheckResponseTimeouts(VirtualLinks virtualLinks, bool redesignIfFail = true);

    // Try to replace old vls with new virtual link. If operation fails, the
    // configuration is returned.
    bool replaceVirtualLinks(VirtualLink* newVl, VirtualLink* oldVl1, VirtualLink* oldVl2 = 0);

    // Try to redesign virtual link in case some constraints are not satisfied (response time out).
    bool redesignVirtualLink(DataFlow* df, VirtualLink* vl);

    // Try to aggregate some other virtual links so that constraints for data
    // flow are satisfied.
    bool limitedSearchAggregation(DataFlow* df, VirtualLink* vl);

    /*
     * Remove virtual link temporary. It can be restored but calling restoreVirtualLink method.
     */
    void cutVirtualLink(Network* network, VirtualLink* vl, DataFlows& dfs);

    /*
     * Restore virtual link cut by cutVirtualLink method.
     */
    void restoreVirtualLink(Network* network, VirtualLink* vl, DataFlows& dfs);

private:
    Network* network;
    Partitions partitions;
    DataFlows dataFlows;

    VirtualLinks existingVirtualLinks;
    VirtualLinks designedVirtualLinks;

    // Estimator of the worst case e2e response time of frames
    ResponseTimeEstimator* responseTimeEstimator;

    bool disableAggregationOnSource;
    bool disableAggregationOnResponseTime;
    bool disableLimitedSearch;
    bool disableRedesign;
    int limitedSearchDepth;
};

#endif
