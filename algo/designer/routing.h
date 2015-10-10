#ifndef ROUTING
#define ROUTING

#include "defs.h"
#include "path.h"

/*
 * Module with routing algorithms to find routes between source and destination(s).
 */
class Routing {
public:

    enum MODE {
        DEJKSTRA_HOPS = 0,
        DEJKSTRA_MAX_REMAINING_BW = 1,
        K_PATH = 2
    };
public:
    // Configuring
    static void setMode(MODE mode);

    static void setKPathDepth(unsigned int depth) {
        kPathsDepth = depth;
    }

public:
    static bool findRoute(Network* network, VirtualLink* vl);

    // Algorithms of searching route
    static Path* searchPathKShortes(Network * network, NetElement* source, NetElement* dest,
                long capacity, Route* existingRoute = 0);

    static Path* searchPathDejkstra(Network * network, NetElement* source, NetElement* dest,
            long capacity, Route* existingRoute = 0);

public:
    static float (*countWeight)(Link* link, Port* fromPort, long capacity);
    static MODE algorithmMode;

    // k shortest paths depth
    static unsigned int kPathsDepth;
};

#endif
