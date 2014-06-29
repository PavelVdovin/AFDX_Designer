#ifndef FACTORY
#define FACTORY

#include "defs.h"
#include <stdio.h>
#include <map>
#include <set>
#include <QFile>
#include <QTextStream>
#include <QtXml/QDomElement>

class Factory {
	typedef std::map<int, NetElement*> NetElementsStorage;
	typedef std::set<Link*> LinksStorage;
	typedef std::map<int, Partition*> PartitionsStorage;
	typedef std::map<int, std::pair<VirtualLink*, QDomElement*> > VirtualLinksStorage;
	typedef std::map<DataFlow*, QDomElement*> DataFlowsStorage;
	typedef std::map<int, Port*> PortsStorage;

	typedef std::map<Path*, QDomElement*> PathsStorage;

public:

	~Factory();

	NetElement* generateNetElement(const QDomElement& element);
	Link* generateLink(const QDomElement& element);
	Partition* generatePartition(const QDomElement& element);
	VirtualLink* generateVirtualLink(QDomElement& element);
	DataFlow* generateDataFlow(QDomElement& element);


	Path* generatePath(VirtualLink* virtualLink, QDomElement& element);

private:
	NetElementsStorage netElementsStorage;
	LinksStorage linksStorage;
	PartitionsStorage partitionsStorage;
	VirtualLinksStorage virtualLinksStorage;
	DataFlowsStorage dataFlowsStorage;
	PortsStorage portsStorage;
	PathsStorage pathsStorage;
};
#endif
