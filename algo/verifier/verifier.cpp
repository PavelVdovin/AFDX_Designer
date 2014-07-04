#include "verifier.h"
#include "virtualLink.h"
#include "operations.h"

std::string Verifier::verify(Network* network, VirtualLinks& virtualLinks) {
    std::string status = "Verified successfully";

    VirtualLinks::iterator it = virtualLinks.begin();
    for( ; it != virtualLinks.end(); ++it ) {
        if ( !Operations::assignVirtualLink(network, *it) )
            status = "Verification failed: cannot assign virtual link";
    }

    return status;
}
