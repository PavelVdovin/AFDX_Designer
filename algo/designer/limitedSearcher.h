#ifndef LIMITED_SEARHER_H
#define LIMITED_SEARHER_H

#include "defs.h"
#include <vector>

class LimitedSearcher {
public:
    typedef bool (*ComparatorFunc)(VirtualLink*, VirtualLink*);
    typedef std::map<VirtualLink*, Paths> Context;

    // Params:
    // Depth - the maximum power of the set to search in
    //
    LimitedSearcher(int depth, Network* network)
    : maxDepth(depth),
      network(network)
    {}

    // Start limited search for the specified virtual link
    void start(VirtualLinks& assigned, ComparatorFunc comparator);

    // Get next set (of length depth) to operate with,
    // it is automatically assigned/removed.
    VirtualLinks& getNextSet();

    // Stop limited search with the specified status
    void stop(bool success);

private:
    void removeAssignments();
    void restoreAssignments();
    bool checkLast();
    void increaseIndexes();

private:
    Network* network;
    int depth, maxDepth;

    std::vector<VirtualLink*> workingSortedSet;
    int* indexes;
    VirtualLinks removed;
    Context context;

};

#endif
