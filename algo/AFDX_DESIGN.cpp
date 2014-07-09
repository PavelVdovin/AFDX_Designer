#include "xmlreader.h"
#include "network.h"
#include "verifier.h"
#include "designer.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    if ( argc != 3 || std::string(argv[2]) != "a" && std::string(argv[2]) != "v" )
    {
        printf("Usage: %s <input file> v|a\n", *argv);
        printf("\tv: verify and exit\n");
        printf("\ta: run designer\n");
        return 1;
    }

    QFile input(argv[1]);

    input.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream inputStream(&input);

    QDomDocument document("XmlDocument");

    QString errMessage;
    int errorLine, errorColumn;
    if ( !document.setContent(inputStream.readAll(), false, &errMessage, &errorLine, &errorColumn)) {
        printf("XML parsing error, reason: %s at line %d, column %d\n", errMessage.toStdString().c_str(), errorLine, errorColumn);
        return 2;
    }

    QDomElement root = document.documentElement();

    XmlReader xmlReader(root);

    if ( !xmlReader.isWellParsed() ) {
        printf("Not all parameters are specified, some elements are omitted.\n");
    }
    Network * network = xmlReader.getNetwork();
    printf("Network has %d net-elements.\n", network->getNetElements().size());
    printf("Network has %d links.\n", network->getLinks().size());
    printf("There are %d partitions.\n", xmlReader.getPartitions().size());
    printf("There are %d virtual links.\n", xmlReader.getVirtualLinks().size());
    printf("There are %d data flows.\n", xmlReader.getDataFlows().size());

    if ( std::string(argv[2]) == "v" ) {
        std::string status = Verifier::verify(network, xmlReader.getVirtualLinks());
        std::cout << status << std::endl;
        return 0;
    } else {
        Designer designer(network, xmlReader.getPartitions(), xmlReader.getDataFlows(), xmlReader.getVirtualLinks());
        designer.design();

        VirtualLinks newVls = designer.getDesignedVirtualLinks();
        printf("Generated %d virtualLinks.\n", newVls.size());
    }

    return 0;
}
