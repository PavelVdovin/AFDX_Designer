#ifndef XML_READER
#define XML_READER

#include "defs.h"
#include "factory.h"

#include <QtXml/QDomElement>

class XmlReader {
public:
	XmlReader(const QDomElement & element);
	~XmlReader();

	inline Network* getNetwork() const {
	    return network;
	}

	inline Partitions& getPartitions() {
        return partitions;
    }

	inline VirtualLinks& getVirtualLinks() {
	    return virtualLinks;
	}

	inline DataFlows& getDataFlows() {
	    return dataFlows;
	}

	inline bool isWellParsed() {
	    return wellParsed;
	}

private:
	bool generateElementByType(const QString& type, QDomElement& elem);
private:
	Network * network;
	Partitions partitions;
	VirtualLinks virtualLinks;
	DataFlows dataFlows;

	QDomElement rootElement;
	Factory factory;
	bool wellParsed;

};

#endif
