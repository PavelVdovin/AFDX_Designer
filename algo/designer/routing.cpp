#include "routing.h"

#include "virtualLink.h"
#include "operations.h"
#include "link.h"
#include <map>

bool Routing::findRoute(Network* network, VirtualLink* vl) {
    // TODO: find route consists of several paths
    NetElements& elements = vl->getDestinations();
    NetElements::iterator it = elements.begin();
    for ( ; it != elements.end(); ++it ) {
        //TODO: different algorithms
        Path* path = searchPathDejkstra(network, vl->getSource(), *it, vl->getBandwidth(), &(vl->getRoute()));
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
    Links links = Operations::getLinksOfElement(elem);
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
            float weight = currentWeight + Routing::countWeight(*it, port, capacity);
            context.insert(neighbour, weight, false, *it);
            context.elementsToParse.insert(neighbour);
            continue;
        }

        ElementWeight& elemWeight= context.elements[neighbour];
        if ( !elemWeight.parsed ) {
            float weight = currentWeight + Routing::countWeight(*it, port, capacity);
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
            context.insert(it->first, 0.0, true, link);
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

        // Initialize neighbors
        std::map<NetElement*, ElementWeight>::iterator eit = context.elements.begin();
        for ( ; eit != context.elements.end(); ++eit ) {
            parseNeighbors(context, eit->first, capacity);
        }
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

static float countWeightHops(Link* link, Port* fromPort, long capacity) {
    return 1.0;
}

static float countWeightMaxRemainingBw(Link* link, Port* fromPort, long capacity) {
    return ((float)(link->getMaxCapacity() - link->getFreeCapacityFromPort(fromPort) + capacity))
            / link->getMaxCapacity();
}

void Routing::setMode(MODE mode) {
    switch(mode) {
    case Routing::DEJKSTRA_MAX_REMAINING_BW:
        countWeight = countWeightMaxRemainingBw;
    default:
        countWeight = countWeightHops;
    }
}


float (*Routing::countWeight)(Link* link, Port* fromPort, long capacity) = countWeightMaxRemainingBw;
