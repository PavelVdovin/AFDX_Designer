#ifndef VERIFIER
#define VERIFIER

#include "defs.h"
#include <string>

// This class verifies whether all specified virtual links
// may be assigned without links capacity failure.
//
class Verifier {
public:
    static std::string verify(Network* network, VirtualLinks& virtualLinks);

    enum FailedConstraint {
        NONE = 0,
        CAPACITY,
        JMAX,
        TMAX
    };
    // Verify that virtual links may be assigned via the port of net-element without constraints breaking
    static FailedConstraint verifyOutgoingVirtualLinks(Port* port, VirtualLink* vl);
    static FailedConstraint verifyOutgoingVirtualLinks(Port* port);
    static FailedConstraint verifyOutgoingVirtualLinks(VirtualLink* vl);

    static inline void setLimitJitter(bool limit) {
        limitJitter = limit;
    }

private:
    static bool limitJitter;
};

#endif
