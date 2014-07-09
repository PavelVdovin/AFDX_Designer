#ifndef DESIGNER
#define DESIGNER

#include "defs.h"

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
     * First step of algorithm: generate virtual links
     * before them being routed.
     */
    void designUnroutedVLs();

private:
    Network* network;
    Partitions partitions;
    DataFlows dataFlows;

    VirtualLinks existingVirtualLinks;
    VirtualLinks designedVirtualLinks;
};

#endif
