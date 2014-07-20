#include "xmlreader.h"
#include "network.h"
#include "stdio.h"

#include <QString>
#include <QStringList>
#include <QDomNodeList>

XmlReader::XmlReader(QDomDocument & document)
: document(document), rootElement(document.documentElement()), factory(document){
	QStringList elementTypes;
	elementTypes << "endSystem" << "switch" << "link" << "partition" << "virtualLink" << "dataFlow";

	network = new Network();

	wellParsed = true;
	for ( int i = 0; i < elementTypes.length(); ++i ) {
	    QString type = elementTypes.at(i);
		QDomNodeList elementsList = rootElement.elementsByTagName(type);
		for (int j = 0; j < elementsList.length(); j++) {
		    QDomElement xmlElement = elementsList.at(j).toElement();
		    if ( !generateElementByType(type, xmlElement) ) {
		        wellParsed = false;
		    }
		}
	}
}

XmlReader::~XmlReader() {
	delete network;
}

bool XmlReader::generateElementByType(const QString& type, QDomElement& elem) {
    bool wellParsed = true;
	if ( type == "endSystem" || type == "switch" ) {
		NetElement* element = factory.generateNetElement(elem);
		if ( element == 0 ) {
			wellParsed = false;
		} else {
		    network->netElements.insert(element);
		}
	} else if ( type == "link" ) {
	    Link* link = factory.generateLink(elem);
	    if ( link == 0 ) {
            wellParsed = false;
	    } else
	        network->links.insert(link);
	} else if ( type == "partition" ) {
	    Partition* partition = factory.generatePartition(elem);
	    if ( partition == 0 ) {
            wellParsed = false;
	    } else
	        partitions.insert(partition);
	} else if ( type == "virtualLink" ) {
	    VirtualLink* virtualLink = factory.generateVirtualLink(elem);
	    if ( virtualLink == 0 ) {
	        wellParsed = false;
	    } else
	        virtualLinks.insert(virtualLink);
	} else if ( type == "dataFlow" ) {
	    DataFlow* dataFlow = factory.generateDataFlow(elem);
	    if ( dataFlow == 0 ) {
	        wellParsed = false;
	    } else
	        dataFlows.insert(dataFlow);
	}

	return wellParsed;
}

void XmlReader::saveDesignedVirtualLinks(VirtualLinks& virtualLinks) {
    QDomNodeList elementsList = rootElement.childNodes ();
    QDomElement vls = rootElement.firstChildElement("virtualLinks");
    if ( vls.isNull() ) {
        vls = document.createElement("virtualLinks");
        rootElement.appendChild(vls);
    }

    VirtualLinks::iterator it = virtualLinks.begin();
    for ( ; it != virtualLinks.end(); ++it ) {
        factory.saveVirtualLink(network, *it, vls);
    }
}
