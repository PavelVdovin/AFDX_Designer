#include "routing.h"

#include "virtualLink.h"
#include "operations.h"
#include "link.h"
#include "network.h"
#include <map>

bool Routing::findRoute(Network* network, VirtualLink* vl) {
    // TODO: find route consists of several paths
    NetElements& elements = vl->getDestinations();
    NetElements::iterator it = elements.begin();
    for ( ; it != elements.end(); ++it ) {
        //TODO: different algorithms
        Path* path = 0;
        if ( algorithmMode == K_PATH ) {
            path = searchPathKShortes(network, vl->getSource(), *it, vl->getBandwidth(), &(vl->getRoute()));
        } else {
            path = searchPathDejkstra(network, vl->getSource(), *it, vl->getBandwidth(), &(vl->getRoute()));
        }

        if ( path != 0 )
            vl->addPath(path);
        else
            return false;
    }

    return true;
}

struct ElementWeight {
    ElementWeight(float weight = 0, bool parsed = false, Link* bestIncomingLink = 0)
    : weight(weight),
      parsed(parsed),
      bestIncomingLink(bestIncomingLink) {
    }
    float weight;
    bool parsed;
    Link* bestIncomingLink;
};

struct Context {
    inline void insert(NetElement* elem, float weight, bool parsed, Link* incomingLink) {
        elements[elem] = ElementWeight(weight, parsed,  incomingLink);
    }

    std::map<NetElement*, ElementWeight> elements;
    NetElements elementsToParse;
    NetElement* source;
    NetElement* dest;
    Network* network;
    Links zeroWeightLinks;
};

inline NetElement* getNeighbour(Port* port, Link* link) {
    assert(link->getPort1() == port || link->getPort2() == port);
    Port* otherPort = link->getPort1() == port ? link->getPort2() : link->getPort1();
    return link->getElementByPort(otherPort);
}

inline bool isCapacityEnough(long capacity, Link* link, Port* from) {
    return link->getFreeCapacityFromPort(from) >= capacity;
}

void parseNeighbors(Context& context, NetElement* elem, long capacity) {
    Links links = Operations::getLinksOfElement(context.network, elem);
    Links::iterator it = links.begin();
    for ( ; it != links.end(); ++it ) {
        Port* port = (*it)->getPortByParent(elem);
        if ( !isCapacityEnough(capacity, *it, port) )
            continue;

        NetElement* neighbour = getNeighbour(port, *it);
        if ( neighbour->isEndSystem() && neighbour != context.dest )
            continue; // parse only switches

        float currentWeight = context.elements[elem].weight;
        if ( context.elements.find(neighbour) == context.elements.end() ) {
            float weight = currentWeight;
            if ( context.zeroWeightLinks.find(*it) == context.zeroWeightLinks.end() )
                    weight += Routing::countWeight(*it, port, capacity);

            context.insert(neighbour, weight, false, *it);
            context.elementsToParse.insert(neighbour);
            continue;
        }

        ElementWeight& elemWeight= context.elements[neighbour];
        if ( !elemWeight.parsed ) {
            float weight = currentWeight;
            if ( context.zeroWeightLinks.find(*it) == context.zeroWeightLinks.end() )
                    weight += Routing::countWeight(*it, port, capacity);
            // float weight = currentWeight + Routing::countWeight(*it, port, capacity);
            if ( weight < elemWeight.weight ) {
                elemWeight.weight = weight;
                elemWeight.bestIncomingLink = *it;
            }
        }
    }
}

void initializePathZeroWeight(Context& context, Path* path) {
    std::list<PathNode>::iterator it = path->getPath().begin();
    NetElement* prevElement = 0;
    for ( ; it != path->getPath().end(); ++it ) {
        if ( context.elements.find(it->first) == context.elements.end() ) {
            Link* link = 0;
            if ( prevElement != 0 )
                link = Operations::getLinkByNetElements(context.network, prevElement, it->first);
            if ( link != 0 )
                context.zeroWeightLinks.insert(link);
                // context.insert(it->first, EPS, false, link);
        }
        prevElement = it->first;
    }
}

void initializeElements(Context& context, NetElement* source, long capacity, Route* existingRoute = 0) {
    if ( existingRoute == 0 || existingRoute->getPaths().size() == 0 ) {
        context.insert(source, 0, true, 0);
        parseNeighbors(context, source, capacity);
    } else {
        Paths::iterator it = existingRoute->getPaths().begin();
        for( ; it != existingRoute->getPaths().end(); ++it ) {
            initializePathZeroWeight(context, it->second);
        }

        context.insert(source, 0, true, 0);
        parseNeighbors(context, source, capacity);
        // Initialize neighbors
        // std::map<NetElement*, ElementWeight>::iterator eit = context.elements.begin();
        // for ( ; eit != context.elements.end(); ++eit ) {
            // parseNeighbors(context, eit->first, capacity);
        // }
    }
}

NetElement* getNextElement(Context& context) {
    NetElement* result = 0;
    float minWeight = 0;
    NetElements::iterator it = context.elementsToParse.begin();
    if ( it != context.elementsToParse.end() ) {
        assert(context.elements.find(*it) != context.elements.end());
        result = *it++;
        minWeight = context.elements[result].weight;
    }

    for ( ; it != context.elementsToParse.end(); ++it ) {
        assert(context.elements.find(*it) != context.elements.end());
        float weight = context.elements[*it].weight;
        if ( weight < minWeight ) {
            minWeight = weight;
            result = *it;
        }
    }

    if ( result != 0 ) {
        context.elements[result].parsed = true;
        context.elementsToParse.erase(result);
    }

    return result;
}

Path* retrievePath(Context& context) {
    Path* path = new Path(context.source, context.dest);

    std::list<NetElement*> reverted;
    NetElement* currentElement = context.dest;
    while ( currentElement != context.source) {
        reverted.push_back(currentElement);
        assert(context.elements.find(currentElement) != context.elements.end());
        Link* link = context.elements[currentElement].bestIncomingLink;
        Port* port = link->getPortByParent(currentElement);
        currentElement = getNeighbour(port, link);
    }

    std::list<NetElement*>::reverse_iterator it = reverted.rbegin();
    for ( ; it != reverted.rend(); ++it ) {
        path->appendVertex(*it, false); // all path elements are assumed to be low-priority
    }

    return path;
}

Path* Routing::searchPathDejkstra(Network * network, NetElement* source, NetElement* dest,
        long capacity, Route* existingRoute)
{
    if ( source == dest )
        return 0;

    // local variables
    Context context;
    context.source = source;
    context.dest = dest;
    context.network = network;

    initializeElements(context, source, capacity, existingRoute);

    NetElement* currentElement = getNextElement(context);
    while ( currentElement != dest && currentElement != 0 ) {
        parseNeighbors(context, currentElement, capacity);
        currentElement = getNextElement(context);
    }

    if ( currentElement == 0 ) {
        return 0;
    }

    return retrievePath(context);
}

// Estimate the weight of the path during selection in k shortest paths algorithm
// TODO: extra not-efficient. Make a cache of path links.
float estimateWeight(Network* network, Path* path) {
    float weight = 0.0;
    bool firstNode = true; // we can skip the first node
    NetElement* prevElement = 0;
    std::list<PathNode>& nodes = path->getPath();
    for ( std::list<PathNode>::iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt ) {
        if ( firstNode ) {
            firstNode = false;
            prevElement = nIt->first;
            continue;
        }

        Link* link = Operations::getLinkByNetElements(network, prevElement, nIt->first);
        prevElement = nIt->first;
        assert(link != 0);
        Port* fromPort = link->getPort1();
        if ( prevElement->getPorts().find(fromPort) == prevElement->getPorts().end() ) {
            fromPort = link->getPort2();
        }

        assert(prevElement->getPorts().find(fromPort) != prevElement->getPorts().end());

        weight += (float) link->getFreeCapacityFromPort(fromPort) / (float)link->getMaxCapacity();
    }
    return weight;
}

// Estimate the duration of the specified path.
// Important! This function estimates the duration, this is heuristic, not even the upper-/lower- bound of the duration.
// TODO: extra not-efficient. Make a cache of path links.
float estimateDuration(Network* network, Path* path, long capacity) {
    float weight = 0.0;
    bool firstNode = true; // we can skip the first node
    NetElement* prevElement = 0;
    std::list<PathNode>& nodes = path->getPath();
    VirtualLinks intersect; // virtual links which paths intersects with the current path
    for ( std::list<PathNode>::iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt ) {
        if ( firstNode ) {
            firstNode = false;
            prevElement = nIt->first;
            continue;
        }

        Link* link = Operations::getLinkByNetElements(network, prevElement, nIt->first);
        prevElement = nIt->first;
        assert(link != 0);
        Port* fromPort = link->getPortByParent(prevElement);
        assert(prevElement->getPorts().find(fromPort) != prevElement->getPorts().end());

        VirtualLinks& vls = link->getAssignedFromPort(fromPort);
        weight += (float)capacity / (float)link->getMaxCapacity();

        // Count the influence of each virtual link only once
        for ( VirtualLinks::iterator it = vls.begin(); it != vls.end(); ++it ) {
            if ( intersect.find(*it) != intersect.end() ) {
                weight += (float)(*it)->getLMax() / link->getMaxCapacity();
                intersect.insert(*it);
            }
        }
    }
    return weight;
}

bool notInPath(Network* network, Path* path, Link* linkToCheck) {
    bool firstNode = true; // we can skip the first node
    NetElement* prevElement = 0;
    std::list<PathNode>& nodes = path->getPath();
    for ( std::list<PathNode>::iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt ) {
        if ( firstNode ) {
            firstNode = false;
            prevElement = nIt->first;
            continue;
        }

        Link* link = Operations::getLinkByNetElements(network, prevElement, nIt->first);
        prevElement = nIt->first;
        if ( linkToCheck == link )
            return false;
    }

    return true;
}

Path* Routing::searchPathKShortes(Network * network, NetElement* source, NetElement* dest,
                long capacity, Route* existingRoute) {
    // Yen's algorithm
    Path* path = searchPathDejkstra(network, source, dest, capacity, existingRoute);
    if ( path == 0 ) {
        return 0;
    }

    unsigned int pathsFound = 1;
    Path* currentPath = path, *bestPath = path;
    float bestPathWeight = estimateDuration(network, path, capacity);
    bool isNewPathFound = false;
    Links removedLinks; // to restore them

    // Going through all elements of the current shortest path and remove them
    while ( pathsFound < kPathsDepth ) {
        Link* linkToRemove = 0;
        std::list<PathNode>& nodes = currentPath->getPath();
        bool firstNode = true; // we can skip the first node
        NetElement* prevElement = 0;

        Path* shortest = 0;
        float minWeight = 0.0;
        for ( std::list<PathNode>::iterator nIt = nodes.begin(); nIt != nodes.end(); ++nIt ) {
            if ( firstNode ) {
                firstNode = false;
                prevElement = nIt->first;
                continue;
            }

            Link* link = Operations::getLinkByNetElements(network, prevElement, nIt->first);
            assert(link != 0);
            prevElement =nIt->first;

            network->getLinks().erase(link);
            path = searchPathDejkstra(network, source, dest, capacity, existingRoute);

            // restore the network
            network->getLinks().insert(link);

            if ( path == 0 )
                continue;

            float weight = estimateWeight(network, path);
            if ( shortest == 0 || minWeight > weight ) {
                delete shortest;
                shortest = path;
                minWeight = weight;
                linkToRemove = link;

                assert(notInPath(network, shortest, link));
            } else {
                delete path;
            }
        }

        if ( currentPath != bestPath ) {
            delete currentPath;
        }

        // if we found the next shortest path, compare it with the previous one
        if ( shortest == 0 )
            break; // restore everything and get the answer

        pathsFound++;
        if ( pathsFound != kPathsDepth ) {
            // remove link to continue search
            removedLinks.insert(linkToRemove);
            network->getLinks().erase(linkToRemove);
        }

        currentPath = shortest;
        // Compare the current shortest path with the current best paths
        float weight = estimateDuration(network, shortest, capacity);
        if ( weight < bestPathWeight ) {
            delete bestPath;
            bestPath = shortest;
            bestPathWeight = weight;
        }
    }

    // Restoring the network and returning the best found path
    for ( Links::iterator it = removedLinks.begin(); it != removedLinks.end(); ++it ) {
        network->getLinks().insert(*it);
    }

    return bestPath;
}

static float countWeightHops(Link* link, Port* fromPort, long capacity) {
    return 1.0;
}

static float countWeightMaxRemainingBw(Link* link, Port* fromPort, long capacity) {
    return ((float)(link->getMaxCapacity() - link->getFreeCapacityFromPort(fromPort) + capacity))
            / link->getMaxCapacity();
}

void Routing::setMode(MODE mode) {
    algorithmMode = mode;
    switch(mode) {
    case Routing::DEJKSTRA_HOPS:
        countWeight = countWeightHops;
    default:
        countWeight = countWeightMaxRemainingBw;
        break;
    }
}




float (*Routing::countWeight)(Link* link, Port* fromPort, long capacity) = countWeightMaxRemainingBw;
Routing::MODE Routing::algorithmMode = Routing::K_PATH;
unsigned int Routing::kPathsDepth = 5;
