#include "responseTimeEstimator.h"
#include "virtualLink.h"
#include "verifier.h"
#include <stdio.h>

void ResponseTimeEstimator::estimateWorstCaseResponseTime() {
	std::string status = Verifier::verify(network, virtualLinks);
	if ( status.find("Verification failed") != std::string::npos ) {
		printf("The configuration failed to be verified, please provide successfully verified configuration\n");
		return;
	}

	// Initializing data
	VirtualLinks::iterator it = virtualLinks.begin();
    for ( ; it != virtualLinks.end(); ++it ) {
        float estimation = estimateWorstCaseResponseTime(*it);
        long estimationLong = (long)(estimation - EPS) < (long)estimation ? (long)estimation : (long)estimation + 1;
        printf("Resp time: %d\n", estimationLong);
        (*it)->setResponseTimeEstimation(estimationLong);
    }
}
