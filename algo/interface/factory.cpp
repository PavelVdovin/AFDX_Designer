#include "factory.h"
#include "endsystem.h"
#include "switch.h"
#include "port.h"
#include "partition.h"
#include "link.h"
#include "dataFlow.h"
#include "virtualLink.h"
#include "path.h"
#include <stdio.h>

#include <QString>
#include <QStringList>

Factory::~Factory() {
	// Clear storages
	NetElementsStorage::iterator netIt = netElementsStorage.begin();
	for ( ; netIt != netElementsStorage.end(); ++netIt ) {
		delete netIt->second;
	}

	PortsStorage::iterator pIt = portsStorage.begin();
	for ( ; pIt != portsStorage.end(); ++pIt ) {
		delete pIt->second;
	}

	LinksStorage::iterator lit = linksStorage.begin();
	for ( ; lit != linksStorage.end(); ++lit ) {
	    delete *lit;
	}

	PartitionsStorage::iterator pit = partitionsStorage.begin();
	for( ; pit != partitionsStorage.end(); ++pit ) {
	    delete pit->second;
	}

	VirtualLinksStorage::iterator vit = virtualLinksStorage.begin();
	for ( ; vit != virtualLinksStorage.end(); ++vit ) {
	    delete vit->second.first;
	}

	PathsStorage::iterator path = pathsStorage.begin();
    for ( ; path != pathsStorage.end(); ++path ) {
        delete path->first;
    }
}

NetElement* Factory::generateNetElement(const QDomElement& element) {
	NetElement* netElement = 0;

	QString numberStr = element.attribute("number");
	if ( numberStr.length() == 0 ) {
		printf("Fail to read number property of net-element.\n");
		return 0;
	}

	int number = numberStr.toInt();
	if ( !number ) {
		printf("Fail to convert number property to int in net-element.\n");
		return 0;
	}

	if ( element.tagName() == "endSystem" ) {
		netElement = new EndSystem();
	} else if ( element.tagName() == "switch" ) {
		netElement = new Switch();
	} else {
		printf("Wrong tag during net-element generation.\n");
		return 0;
	}

	netElementsStorage[number] = netElement;
	QString ports = element.attribute("ports");
	if ( ports.length() != 0 ) {
		QStringList portsStr = ports.split(",");
		for ( int i = 0; i < portsStr.size(); ++i ) {
			int id = portsStr.at(i).toInt();
			if ( !id ) {
				printf("Fail to identify port index.\n");
				return 0;
			}

			Port* port = new Port(netElement);
			portsStorage[id] = port;
			netElement->ports.insert(port);
		}
	}

	return netElement;
}

Link* Factory::generateLink(const QDomElement& element) {
    Link* link = 0;
    QString capacityStr = element.attribute("capacity"),
            fromStr = element.attribute("from"),
            fromTypeStr = element.attribute("fromType"),
            toStr = element.attribute("to"),
            toTypeStr = element.attribute("toType");

    if ( capacityStr.length() == 0 || fromStr.length() == 0 || toStr.length() == 0 ) {
        printf("Wrong link tag format.\n");
        return 0;
    }

    int capacity = capacityStr.toInt(),
        from = fromStr.toInt(),
        to = toStr.toInt();

    if ( !capacity || !from || !to ) {
        printf("Wrong link tag format.\n");
        return 0;
    }

    if ( portsStorage.find(to) == portsStorage.end() || portsStorage.find(from) == portsStorage.end() ) {
        printf("Port not found during link generation.\n");
        return 0;
    }
    Port* port1 = portsStorage[from];
    Port* port2 = portsStorage[to];

    link = new Link(port1, port2, capacity);
    port1->assosiatedLink = link;
    port2->assosiatedLink = link;

    if ( fromTypeStr.length() > 0 ) {
        int fromType = fromTypeStr.toInt();
        port1->type = fromType ? Port::PRIORITIZED : Port::FIFO;
    }

    if ( toTypeStr.length() > 0 ) {
        int toType = toTypeStr.toInt();
        port2->type = toType ? Port::PRIORITIZED : Port::FIFO;
    }

    linksStorage.insert(link);
    return link;
}

Partition* Factory::generatePartition(const QDomElement& element) {
    Partition* partition = 0;

    QString numberStr = element.attribute("number"),
            connectedToStr = element.attribute("connectedTo");

    if ( numberStr.length() == 0 ) {
        printf("Fail to read number property of partition.\n");
        return 0;
    }

    int number = numberStr.toInt();
    if ( !number ) {
        printf("Fail to convert number property to int in partition.\n");
        return 0;
    }

    partition = new Partition();

    partitionsStorage[number] = partition;

    if ( connectedToStr.length() > 0 ) {
        int connectedTo = connectedToStr.toInt();
        if ( connectedTo && netElementsStorage.find(connectedTo) != netElementsStorage.end() ) {
            NetElement* es = netElementsStorage[connectedTo];
            if ( es->isEndSystem()) {
                partition->connectedTo = es;
                es->toEndSystem()->partitions.insert(partition);
            }
        }
    }

    return partition;
}

VirtualLink* Factory::generateVirtualLink(QDomElement& element) {
    VirtualLink * virtualLink = 0;

    QString numberStr = element.attribute("number"),
            bagStr = element.attribute("bag"),
            destStr = element.attribute("dest"),
            lmaxStr = element.attribute("lmax"),
            sourceStr = element.attribute("source");

    if ( numberStr.length() == 0 ) {
        printf("Fail to read number property of virtual link.\n");
        return 0;
    }

    int number = numberStr.toInt();
    if ( !number ) {
        printf("Fail to convert number property to int in partition.\n");
        return 0;
    }

    virtualLink = new VirtualLink();
    virtualLinksStorage[number] = std::pair<VirtualLink*, QDomElement*>(virtualLink, &element);

    if ( bagStr.length() > 0 ) {
        int bag = bagStr.toInt();
        if ( bag == 1 || bag == 2 || bag == 4 || bag == 8 || bag == 16 || bag == 32 || bag == 64 || bag == 128 ) {
            virtualLink->setBag(bag);
        } else
            printf("Error during bag parsing.\n");
    }

    if ( lmaxStr.length() > 0 ) {
        int lmax = lmaxStr.toInt();
        if ( lmax >= 64 && lmax <= 1518 ) {
            virtualLink->setLMax(lmax);
        }
        else
            printf("Error during lmax parsing.\n");
    }

    if ( sourceStr.length() > 0 ) {
        int source = sourceStr.toInt();
        if ( source && netElementsStorage.find(source) != netElementsStorage.end()
                && netElementsStorage[source]->isEndSystem()) {
            virtualLink->setSource(netElementsStorage[source]);
        } else {
            printf("Error during vl source parsing.\n");
            return 0;
        }
    }

    if ( destStr.length() > 0 ) {
        QStringList dests = destStr.split(",");
        for ( int i = 0; i < dests.size(); ++i ) {
            int id = dests.at(i).toInt();
            if ( !id ) {
                printf("Fail to identify dest index.\n");
                return 0;
            }

            if ( netElementsStorage.find(id) != netElementsStorage.end()
                && netElementsStorage[id]->isEndSystem())
                virtualLink->addDestination(netElementsStorage[id]);
        }
    }

    // Parsing paths
    QDomNodeList paths = element.elementsByTagName("path");
    for (int i = 0; i < paths.length(); i++) {
        QDomElement xmlElement = paths.at(i).toElement();
        Path* path = generatePath(virtualLink, xmlElement);
        if ( path == 0 ) {
            return 0;
        }

        virtualLink->route.addPath(path->getDest(), path);
    }

    return virtualLink;
}

Path* Factory::generatePath(VirtualLink* virtualLink, QDomElement& element) {
    Path* path = 0;

    QString sourceStr = element.attribute("source"),
            destStr = element.attribute("dest"),
            pathStr = element.attribute("path");

    if ( sourceStr.length() == 0 || destStr.length() == 0 || pathStr.length() == 0 ) {
        printf("Fail to parse path.\n");
        return 0;
    }

    int source = sourceStr.toInt();
    if ( !source || netElementsStorage.find(source) == netElementsStorage.end()
            || netElementsStorage[source] != virtualLink->getSource() ) {
        printf("Fail to parse path.\n");
        return 0;
    }

    int dest = destStr.toInt();
    if ( !dest || netElementsStorage.find(dest) == netElementsStorage.end() || !netElementsStorage[dest]->isEndSystem() ) {
        printf("Fail to parse path.\n");
        return 0;
    }

    path = new Path(virtualLink->getSource(), netElementsStorage[dest]);
    pathsStorage[path] = &element;

    QStringList elems = pathStr.split(",");
    if ( elems[0].toInt() != source ) {
        printf("Fail to parse path.\n");
        return 0;
    }

    for ( int i = 1; i < elems.length(); ++i ) {
        QString elem = elems[i];
        bool isHishPriority = false;
        if ( elem[elem.length()-1] == 'h' ) {
            elem.remove('h');
            isHishPriority = true;
        } else if ( elem[elem.length()-1] == 'l' ) {
            elem.remove('l');
        }

        int id = elem.toInt();
        if ( !id || netElementsStorage.find(id) == netElementsStorage.end() ) {
            printf("Fail to parse path.\n");
            return 0;
        }

        path->appendVertex(netElementsStorage[id], isHishPriority);
    }

    return path;
}

DataFlow* Factory::generateDataFlow(QDomElement& element) {
    DataFlow* dataFlow = new DataFlow();

    dataFlowsStorage[dataFlow] = &element;

    QString sourceStr = element.attribute("source"),
            destStr = element.attribute("dest"),
            msgSizeStr = element.attribute("path"),
            periodStr = element.attribute("period"),
            vlStr = element.attribute("vl");

    if ( sourceStr.length() != 0 ) {
        int number = sourceStr.toInt();
        if ( !number || partitionsStorage.find(number) == partitionsStorage.end() ) {
            printf("Fail to parse data flow.\n");
            return 0;
        }

        dataFlow->from = partitionsStorage[number];
    }

    if ( destStr.length() != 0 ) {
        QStringList dests = destStr.split(",");
        for ( int i = 0; i < dests.size(); ++i ) {
            int id = dests.at(i).toInt();
            if ( !id || partitionsStorage.find(id) == partitionsStorage.end()) {
                printf("Fail to identify dest index.\n");
                return 0;
            }

            dataFlow->to.insert(partitionsStorage[id]);
        }
    }

    if ( msgSizeStr.length() != 0 ) {
        long msgSize = msgSizeStr.toLong();
        if ( msgSize )
            dataFlow->msgSize = msgSize;
        else {
            printf("Cannot parse msg size.\n");
            return 0;
        }
    }

    if ( periodStr.length() != 0 ) {
        long period = periodStr.toLong();
        if ( period )
            dataFlow->period = period;
        else {
            printf("Cannot parse period.\n");
            return 0;
        }
    }

    if ( vlStr.length() != 0 ) {
        int vlNum = vlStr.toInt();
        if ( !vlNum || virtualLinksStorage.find(vlNum) == virtualLinksStorage.end() ) {
            printf("Cannot parse vl num.\n");
            return 0;
        }

        dataFlow->assignedTo = virtualLinksStorage[vlNum].first;
    }

    return dataFlow;
}

