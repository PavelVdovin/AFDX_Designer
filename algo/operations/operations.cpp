#include "operations.h"
#include "network.h"
#include "link.h"
#include "virtualLink.h"
#include "partition.h"
#include <stdio.h>
#include <assert.h>

Link* Operations::getLinkByNetElements(Network* network, NetElement* elem1, NetElement* elem2) {
    Links::iterator it = network->getLinks().begin();
    for ( ; it != network->getLinks().end(); ++it ) {
        Link* link = *it;
        if ( link->getPort1()->getParent() == elem1 && link->getPort2()->getParent() == elem2 )
            return link;
        if ( link->getPort2()->getParent() == elem1 && link->getPort1()->getParent() == elem2 )
            return link;
    }
    return 0;
}

NetElements Operations::getEndSystems(Network* network) {
    NetElements result,
                &allElements = network->getNetElements();
    NetElements::iterator it = allElements.begin();
    for ( ; it != allElements.end(); ++it ) {
        if ( (*it)->isEndSystem() )
            result.insert(*it);
    }
    return result;
}

NetElements Operations::getSwitches(Network* network) {
    NetElements result,
                &allElements = network->getNetElements();
    NetElements::iterator it = allElements.begin();
    for ( ; it != allElements.end(); ++it ) {
        if ( (*it)->isSwitch() )
            result.insert(*it);
    }
    return result;
}

Links Operations::getLinksOfElement(NetElement* element) {
    Links result;
    Ports::iterator it = element->getPorts().begin();
    for ( ; it != element->getPorts().end(); ++it ) {
        result.insert((*it)->getAssosiatedLink());
    }
    return result;
}

VirtualLinks Operations::getLinksFromPartition(Partition* element) {
    VirtualLinks result;

    DataFlows& dfs = element->getOutgoingDataFlows();
    DataFlows::iterator it = dfs.begin();
    for ( ; it != dfs.end(); ++it ) {
        VirtualLink* vl = (*it)->getVirtualLink();
        if ( vl != 0 )
            result.insert(vl);
    }

    return result;
}

bool Operations::assignVirtualLink(Network* network, VirtualLink* virtualLink) {
    Route& route = virtualLink->getRoute();
    NetElements& elements = virtualLink->getDestinations();
    NetElements::iterator it = elements.begin();
    for ( ; it != elements.end(); ++it ) {
        Path* path = route.getPath(*it);
        std::list<PathNode>& nodes = path->getPath();

        NetElement* prevElement = path->getSource();
        bool priorityPrevElement = false;
        std::list<PathNode>::iterator nodeIt = nodes.begin();
        assert(nodeIt->first == prevElement);
        ++nodeIt;
        for ( ; nodeIt != nodes.end(); ++nodeIt ) {
            NetElement* node = nodeIt->first;
            assert(prevElement != 0);
            assert(node != 0);
            assert(node != prevElement);
            Link* link = getLinkByNetElements(network, prevElement, node);
            assert(link != 0);

            if ( !link->assignVirtualLinks(virtualLink, link->getPortByParent(prevElement), priorityPrevElement) ) {
                removeVirtualLink(network, virtualLink);
                return false;
            }
            prevElement = node;
            priorityPrevElement = nodeIt->second;
        }
    }

    return true;
}

void Operations::removeVirtualLink(Network* network, VirtualLink* virtualLink) {
	if ( virtualLink->getRoute().getPaths().size() == 0 )
		return; // No route exists
    Links& links = network->getLinks();
    Links::iterator it = links.begin();
    for ( ; it != links.end(); ++it )
        (*it)->removeVirtualLink(virtualLink);
}

long Operations::countMaxJitter(Port* port, VirtualLink* vl) {
    long maxCapacity = port->getAssosiatedLink()->getMaxCapacity();
    long size = 0;

    VirtualLinks& assigned = port->getAssignedLowPriority();
    VirtualLinks::iterator it = assigned.begin();
    for ( ; it != assigned.end(); ++it ) {
        if ( *it != vl )
            size += (*it)->getLMax();
    }

    float jMax = (float)size / ((float)maxCapacity / 1000); // multiply by 1000 to get microseconds
    return (long) jMax;
}

// Returns upper module values
inline int modMax(float val) {
    int mod = (int)(val + EPS);
    return mod > (int)(val - 2 * EPS) ? mod : mod + 1;
}

DataFlow* Operations::setAndCheckResponseTimes(VirtualLink* vl) {
    DataFlows::iterator it = vl->getAssignments().begin();

    // Calculating the whole number of frames first
    int numberOfFrames = 0;

    // The data flow with the smallest value of required response time
    DataFlow* theMostConstrainedDF = 0;
    for ( ; it != vl->getAssignments().end(); ++it ) {
        numberOfFrames += modMax((float)(*it)->getMsgSize() / vl->getLMax());
        if ( (*it)->getTMax() != 0 && (!theMostConstrainedDF || (*it)->getTMax() < theMostConstrainedDF->getTMax()) )
            theMostConstrainedDF = *it;
    }

    float e2eResponseTime = (float)vl->getResponseTimeEstimation() / 1000 + (float)(numberOfFrames - 1) * vl->getBag();
    //printf("Response time: %f\n", e2eResponseTime);
    // Starting from the most constrained - if constraints are satisfied - then it is true for every data flow

    it = vl->getAssignments().begin();
    for ( ; it != vl->getAssignments().end(); ++it ) {
        //assert((*it)->getTMax() == 0 || (float)(*it)->getTMax() >= e2eResponseTime - EPS);
        (*it)->setResponseTime((long)(e2eResponseTime * 1000 + EPS));
        printf("Response time: %d\n", (*it)->getResponseTime());
    }

    if ( theMostConstrainedDF != 0 && (float)theMostConstrainedDF->getTMax() < e2eResponseTime - EPS )
        return theMostConstrainedDF;

    return 0;
}
