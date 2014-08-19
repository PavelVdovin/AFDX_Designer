#include "xmlreader.h"
#include "network.h"
#include "verifier.h"
#include "responseTimeEstimator.h"
#include "trajectoryApproachBasedEstimator.h"
#include "designer.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    if ( argc != 3 && argc != 4 || std::string(argv[argc - 1]) != "a" && std::string(argv[argc - 1]) != "v"
            && std::string(argv[argc - 1]) != "r" )
    {
        printf("Usage: %s <input file> [<output file>] v|a|r\n", *argv);
        printf("\tv: verify and exit\n");
        printf("\ta: run designer\n");
        printf("\tr: estimate end-to-end response times\n");
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
    input.close();



    XmlReader xmlReader(document);

    if ( !xmlReader.isWellParsed() ) {
        printf("Not all parameters are specified, some elements are omitted.\n");
    }
    Network * network = xmlReader.getNetwork();
    printf("Network has %d net-elements.\n", network->getNetElements().size());
    printf("Network has %d links.\n", network->getLinks().size());
    printf("There are %d partitions.\n", xmlReader.getPartitions().size());
    printf("There are %d virtual links.\n", xmlReader.getVirtualLinks().size());
    printf("There are %d data flows.\n", xmlReader.getDataFlows().size());

    if ( std::string(argv[argc-1]) == "v" ) {
        std::string status = Verifier::verify(network, xmlReader.getVirtualLinks());
        std::cout << status << std::endl;
        return 0;
    } else if (std::string(argv[argc-1]) == "r" ) {
        ResponseTimeEstimator* estimator = new TrajectoryApproachBasedEstimator(network, xmlReader.getVirtualLinks());
        estimator->estimateWorstCaseResponseTime();
        xmlReader.updateVirtualLinks();
        QFile output(argv[argc - 2]);
        output.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outStream(&output);
        outStream << document.toString(4);
        output.close();
        delete estimator;
    } else {
        Designer designer(network, xmlReader.getPartitions(), xmlReader.getDataFlows(), xmlReader.getVirtualLinks());
        designer.design();

        VirtualLinks newVls = designer.getDesignedVirtualLinks();
        printf("Generated %d virtualLinks.\n", newVls.size());

        xmlReader.saveDesignedVirtualLinks(designer.getDesignedVirtualLinks());
        QFile output(argv[argc - 2]);
        output.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outStream(&output);
        outStream << document.toString(4);
        output.close();
    }

    return 0;
}
