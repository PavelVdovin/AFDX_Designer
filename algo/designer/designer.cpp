#include "designer.h"
#include "verifier.h"
#include "dataFlow.h"
#include "virtualLink.h"
#include "virtualLinkConfigurator.h"
#include <stdio.h>

Designer::~Designer() {
    VirtualLinks:: iterator it = designedVirtualLinks.begin();
    for ( ; it != designedVirtualLinks.end(); ++it ) {
        delete *it;
    }
}

void Designer::design() {
    if ( Verifier::verify(network, existingVirtualLinks) != "Verified successfully" ) {
        printf("Assigned virtual links violates some constraints. %s",
                "Please redesign preassigned virtual links correctly before running algorithm.\n");
        return;
    }

    designUnroutedVLs();
}

void Designer::designUnroutedVLs() {
    DataFlows::iterator it = dataFlows.begin();
    for ( ; it != dataFlows.end(); ++it ) {
        DataFlow* df = *it;
        if ( df->getVirtualLink() == 0 ) {
            VirtualLink* vl = VirtualLinkConfigurator::designUnroutedVL(df);
            if ( vl != 0 ) {
                df->assign(vl);
                designedVirtualLinks.insert(vl);
            }
        } else {
            printf("Data flow already has assigned virtual link.\n");
        }
    }
}
