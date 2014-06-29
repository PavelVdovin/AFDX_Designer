#include "xmlreader.h"
#include "network.h"
#include "stdio.h"

#include <QString>
#include <QStringList>
#include <QDomNodeList>

XmlReader::XmlReader(const QDomElement & element)
: rootElement(element){
	QStringList elementTypes;
	elementTypes << "endSystem" << "switch" << "link" << "partition" << "virtualLink" << "dataFlow";

	network = new Network();

	for ( int i = 0; i < elementTypes.length(); ++i ) {
	    QString type = elementTypes.at(i);
		QDomNodeList elementsList = rootElement.elementsByTagName(type);
		for (int j = 0; j < elementsList.length(); j++) {
		    QDomElement xmlElement = elementsList.at(j).toElement();
		    if ( !generateElementByType(type, xmlElement) ) {
		        return;
		    }
		}
	}
}

XmlReader::~XmlReader() {
	delete network;
}

bool XmlReader::generateElementByType(const QString& type, QDomElement& elem) {
	if ( type == "endSystem" || type == "switch" ) {
		NetElement* element = factory.generateNetElement(elem);
		if ( element == 0 ) {
			printf ("Fail to parse xml.\n");
			return false;
		}

		network->netElements.insert(element);
	} else if ( type == "link" ) {
	    Link* link = factory.generateLink(elem);
	    if ( link == 0 ) {
	        printf ("Fail to parse xml.\n");
            return false;
	    }
	    network->links.insert(link);
	} else if ( type == "partition" ) {
	    Partition* partition = factory.generatePartition(elem);
	    if ( partition == 0 ) {
	        printf ("Fail to parse xml.\n");
            return false;
	    }
	    partitions.insert(partition);
	} else if ( type == "virtualLink" ) {
	    VirtualLink* virtualLink = factory.generateVirtualLink(elem);
	    if ( virtualLink == 0 ) {
	        printf("Fail to parse xml.\n");
	        return false;
	    }

	    virtualLinks.insert(virtualLink);
	} else if ( type == "dataFlow" ) {
	    DataFlow* dataFlow = factory.generateDataFlow(elem);
	    if ( dataFlow == 0 ) {
	        printf("Fail to parse xml.\n");
	        return false;
	    }
	    dataFlows.insert(dataFlow);
	}

	return true;
}
