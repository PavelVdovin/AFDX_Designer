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
};

#endif
