#ifndef VIRTUAL_LINK_CONFIGURATOR
#define VIRTUAL_LINK_CONFIGURATOR

#include "defs.h"

class VirtualLinkConfigurator {
public:
    /*
     * Generate one virtual link from data flow without assigning route to it
     */
    static VirtualLink* designUnroutedVL(DataFlow* df);

    /*
     * Virtual link with message transmitted in one frame.
     */
    static VirtualLink* generateVLOneFrame(long msgSize, long period, long tMax);

    /*
     * Virtual link with message divided in more then one frame.
     */
    static VirtualLink* generateVLManyFrames(long msgSize, long period, long tMax);
};

#endif
