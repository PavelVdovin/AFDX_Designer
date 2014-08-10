#include "limitedSearcher.h"
#include "virtualLink.h"
#include "operations.h"
#include <algorithm>

void LimitedSearcher::start(VirtualLinks& assigned, LimitedSearcher::ComparatorFunc comparator) {
    // Reinitializing working set
    workingSortedSet = std::vector<VirtualLink*>(assigned.begin(), assigned.end());
    std::sort(workingSortedSet.begin(), workingSortedSet.end(), comparator);
    depth = maxDepth > workingSortedSet.size() ? workingSortedSet.size() : maxDepth;
    indexes = new int[depth];
    for (int i = 0; i < depth; ++i )
        indexes[i] = i;
}

VirtualLinks& LimitedSearcher::getNextSet() {
    removed.clear();

    // checking last indexes
    if ( !checkLast() )
        return removed;

    // Assign the set of virtual links from the previous step
    removeAssignments();

    for ( int i = 0; i < depth; ++i )
        removed.insert(workingSortedSet[indexes[i]]);

    // Remove assignments of virtual links from the current set
    restoreAssignments();

    // increase indexes now
    increaseIndexes();

    return removed;
}

void LimitedSearcher::stop(bool success) {
    if ( !success )
        restoreAssignments();

    delete[] indexes;
}

void LimitedSearcher::removeAssignments() {
    context.clear();
    VirtualLinks::iterator it = removed.begin();
    for ( ; it != removed.end(); ++it ) {
        // Need to remove only from network
        Operations::removeVirtualLink(network, *it);
        Paths& paths = (*it)->getRoute().getPaths();

        // Save paths
        context[*it] = Paths(paths.begin(), paths.end());
        paths.clear();
    }
}

void LimitedSearcher::restoreAssignments() {
    VirtualLinks::iterator it = removed.begin();
    for ( ; it != removed.end(); ++it ) {
        // Restore route
        (*it)->getRoute().getPaths().clear();
        (*it)->getRoute().getPaths().insert(context[*it].begin(), context[*it].end());

        // Need to restore only to network
        Operations::assignVirtualLink(network, *it);
    }
}

bool LimitedSearcher::checkLast() {
    int thres = workingSortedSet.size();
    for ( int i = depth-1; i >= 0; --i ) {
        if ( indexes[i] != thres-- )
            return false;
    }

    return true;
}

void LimitedSearcher::increaseIndexes() {
    int thres = workingSortedSet.size(),
        lastIndex = depth,
        value = 0;
    for ( int i = depth - 1; i >= 0; --i ) {    // 2 3 4, 3 1 1
        ++indexes[i];
        if ( indexes[i] < thres ) {
            value = indexes[i];
            break;
        }

        --lastIndex;
        --thres;
    }

    if ( lastIndex == 0 ) // no need to increase on the last iteration
        return;

    for ( int j = lastIndex; j < depth; ++j ) {
        indexes[j] = ++value;
    }
}
