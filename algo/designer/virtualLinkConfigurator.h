#ifndef VIRTUAL_LINK_CONFIGURATOR
#define VIRTUAL_LINK_CONFIGURATOR

#include "defs.h"

#define FRAME_TRANSMITION_APPROXIMATION 1.0

class VirtualLinkConfigurator {
public:
    /*
     * Generate one virtual link from data flow without assigning route to it
     */
    static VirtualLink* designUnroutedVL(DataFlow* df);

    /*
     * Virtual link with message transmitted in one frame.
     */
    static VirtualLink* generateVLOneFrame(long msgSize, long period, long tMax, long jMax = 0, float frameResponseTimeEstimate = FRAME_TRANSMITION_APPROXIMATION);

    /*
     * Virtual link with message divided in more then one frame.
     */
    static VirtualLink* generateVLManyFrames(long msgSize, long period, long tMax, long jMax = 0, float frameResponseTimeEstimate = FRAME_TRANSMITION_APPROXIMATION);

    // Calculate optimal number of frames for the specified data flows
    // being aggregated in one virtual link
    static VirtualLink* generateAggregatedVirtualLink(DataFlows& dataFlows, float frameResponseTimeEstimate = FRAME_TRANSMITION_APPROXIMATION);

    // Trying to redesign virtual link of the specified data flow
    static VirtualLink* redesignVirtualLink(DataFlow* df, VirtualLink* vl);
private:
    // Use greedy algorithm to calculate data flows max frame size.
    // This is used during generation of aggregated virtual link.
    static int calculateGreedyLMax(DataFlows& dataFlows, int opt_numberOfFrames, int bag);
};

#endif
