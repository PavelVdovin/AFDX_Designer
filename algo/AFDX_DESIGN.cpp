#include "xmlreader.h"
#include "network.h"
#include "verifier.h"
#include "responseTimeEstimator.h"
#include "trajectoryApproachBasedEstimator.h"
#include "designer.h"
#include <string>
#include <iostream>

void printUsage(char** argv) {
    printf("Usage: %s <input file> [<output file>] v|a|r [--limit-jitter=t|f]\n", *argv);
    printf("\tv: verify and exit\n");
    printf("\ta: run designer\n");
    printf("\tr: estimate end-to-end response times\n");
    printf("limit-jitter - jitter should be limited to 500 microseconds (true by default)\n");
}

int main(int argc, char** argv) {
    if ( argc < 3 || argc > 5 )
    {
        printUsage(argv);
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

    bool limitJitter = true;
    // Checking whether there is "limit-jitter" string
    if ( std::string(argv[argc-1]).find("--limit-jitter=") != std::string::npos ) {
        std::string str = std::string(argv[argc-1]);
        if ( str.size() != 16 || (str[15] != 't' && str[15] != 'f' ) ) {
            printUsage(argv);
            return 1;
        }

        if ( str[15] == 'f' ) {
            limitJitter = false;
            printf("Not limiting jitter on end-systems\n");
        }
        --argc;
    }

    Verifier::setLimitJitter(limitJitter);
    if ( std::string(argv[argc-1]) == "v" ) {
        std::string status = Verifier::verify(network, xmlReader.getVirtualLinks());
        std::cout << status << std::endl;
        return 0;
    } else if (std::string(argv[argc-1]) == "r" ) {
        ResponseTimeEstimator* estimator = new TrajectoryApproachBasedEstimator(network);
        estimator->setVirtualLinks(xmlReader.getVirtualLinks());
        estimator->estimateWorstCaseResponseTime();
        xmlReader.updateVirtualLinks();
        QFile output(argv[argc - 2]);
        output.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream outStream(&output);
        outStream << document.toString(4);
        output.close();
        delete estimator;
    } else if (std::string(argv[argc-1]) == "a" ) {
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
    } else {
        printUsage(argv);
        return 1;
    }

    return 0;
}
