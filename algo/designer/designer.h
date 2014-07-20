#ifndef DESIGNER
#define DESIGNER

#include "defs.h"
#include "verifier.h"

class Designer {
public:
    Designer(Network* network, Partitions& partitions, DataFlows& dataFlows, VirtualLinks& existingVirtualLinks):
        network(network),
        partitions(partitions),
        dataFlows(dataFlows),
        existingVirtualLinks(existingVirtualLinks) {

    }

    ~Designer();

    void design();

    inline VirtualLinks& getDesignedVirtualLinks() {
        return designedVirtualLinks;
    }

private:

    /*
     * The first step of algorithm: generate virtual links
     * before them being routed.
     */
    void designUnroutedVLs();

    /*
     * The second step.
     * Check outgoing virtual links constraints and try to fix them
     */
    void checkOutgoingVirtualLinks();
    void checkOutgoingVirtualLinks(NetElement* endSystem);

    /*
     * Redesign outgoing virtual links if some constraints are failed on specified port.
     */
    void redesignOutgoingVirtualLinks(Port* port, Verifier::FailedConstraint);
    void removeVirtualLink(Port* port, VirtualLink*);
    void removeVirtualLink(VirtualLink*);

    /*
     * Third step of virtual links assignment: routing
     */
    void routeVirtualLinks();

private:
    Network* network;
    Partitions partitions;
    DataFlows dataFlows;

    VirtualLinks existingVirtualLinks;
    VirtualLinks designedVirtualLinks;
};

#endif
