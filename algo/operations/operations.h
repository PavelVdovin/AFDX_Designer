#ifndef OPERATIONS
#define OPERATIONS

#include "defs.h"

class Operations {
public:
    // Common operations
    static Link* getLinkByNetElements(Network* network, NetElement* elem1, NetElement* elem2);
    static NetElements getEndSystems(Network* network);
    static NetElements getSwitches(Network* network);
    static Links getLinksOfElement(NetElement* element);

public:
    // Algorithm specific operations

    static bool assignVirtualLink(Network* network, VirtualLink* virtualLink);
    static void removeVirtualLink(Network* network, VirtualLink* virtualLink);

    // Count max jitter with (without if vl == 0 ) assigning vl
    static long countMaxJitter(Port* port, VirtualLink* vl = 0);
};

#endif
