#ifndef XML_READER
#define XML_READER

#include "defs.h"
#include "factory.h"

#include <QtXml/QDomElement>

class XmlReader {
public:
	XmlReader(QDomDocument& document);
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

	inline void updateVirtualLinks() {
	    factory.updateVirtualLinks();
	}

	void saveDesignedVirtualLinks(VirtualLinks& virtualLinks);

private:
	bool generateElementByType(const QString& type, QDomElement& elem);
private:
	Network * network;
	Partitions partitions;
	VirtualLinks virtualLinks;
	DataFlows dataFlows;

	QDomElement rootElement;
	QDomDocument& document;
	Factory factory;
	bool wellParsed;

};

#endif
