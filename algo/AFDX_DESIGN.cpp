#include "xmlreader.h"
#include "network.h"
#include "verifier.h"
#include "responseTimeEstimator.h"
#include "trajectoryApproachBasedEstimator.h"
#include "designer.h"
#include <string>
#include <iostream>

void printUsage(char** argv) {
    printf("Usage: %s <input file> [<output file>] v|a|r [--limit-jitter=t|f]", *argv);
    printf(" [--es-fabric-delay=num] [--switch-fabric-delay=num] [--interframe-gap=num]\n");
    printf("\tv: verify and exit\n");
    printf("\ta: run designer\n");
    printf("\tr: estimate end-to-end response times\n");
    printf("limit-jitter - jitter should be limited to 500 microseconds (true by default)\n");
    printf("es-fabric-delay - delay for sending-receiving one message in end-system only (microseconds, default 0)\n");
    printf("switch-fabric-delay - delay for parsing one frame in switch (microseconds, default 16)\n");
    printf("interframe-gap - minimum gap between two consecutive frames (microseconds, default 0)\n");
}

int main(int argc, char** argv) {
    if ( argc < 3 || argc > 8 )
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
    int esDelay = 0, switchDelay = 16, ifg = 0;

    while ( argc > 3 && std::string(argv[argc-1]).size() > 1 ) {
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
        } else if (std::string(argv[argc-1]).find("--es-fabric-delay=") != std::string::npos ) {
            std::string str = std::string(argv[argc-1]).substr(18);
            int num = atoi(str.c_str());
            if ( num < 0 ) {
                printUsage(argv);
                return 1;
            }
            esDelay = num;
            printf("Es delay = %d\n", esDelay);
        } else if (std::string(argv[argc-1]).find("--switch-fabric-delay=") != std::string::npos ) {
            std::string str = std::string(argv[argc-1]).substr(22);
            int num = atoi(str.c_str());
            if ( num < 0 ) {
                printUsage(argv);
                return 1;
            }
            switchDelay = num;
            printf("Switch delay = %d\n", switchDelay);
        } else if (std::string(argv[argc-1]).find("--interframe-gap=") != std::string::npos ) {
            std::string str = std::string(argv[argc-1]).substr(17);
            int num = atoi(str.c_str());
            if ( num < 0 ) {
                printUsage(argv);
                return 1;
            }
            ifg = num;
            printf("Ifg = %d\n", ifg);
        } else {
            printUsage(argv);
            return 1;
        }
        --argc;
    }

    Verifier::setLimitJitter(limitJitter);
    if ( std::string(argv[argc-1]) == "v" ) {
        std::string status = Verifier::verify(network, xmlReader.getVirtualLinks());
        std::cout << status << std::endl;
        return 0;
    } else if (std::string(argv[argc-1]) == "r" ) {
        ResponseTimeEstimator* estimator = new TrajectoryApproachBasedEstimator(network, esDelay, ifg, switchDelay);
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
        Designer designer(network, xmlReader.getPartitions(), xmlReader.getDataFlows(), xmlReader.getVirtualLinks(),
                esDelay, switchDelay, ifg);
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
