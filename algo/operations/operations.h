#ifndef OPERATIONS
#define OPERATIONS

#include "defs.h"

class Operations {
public:
    static Link* getLinkByNetElements(Network* network, NetElement* elem1, NetElement* elem2);
    static bool assignVirtualLink(Network* network, VirtualLink* virtualLink);
    static void removeVirtualLink(Network* network, VirtualLink* virtualLink);
};

#endif
