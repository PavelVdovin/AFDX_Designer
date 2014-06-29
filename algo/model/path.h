#ifndef PATH_H
#define PATH_H

#include "defs.h"
#include <list>

class Path {
public:
    Path(NetElement* source = 0, NetElement* dest = 0)
        : source(source), dest(dest) {
        path.push_back(PathNode(source, false));
    }

    inline void appendVertex(NetElement* elem, bool isHighPriority = false) {
        path.push_back(PathNode(elem, isHighPriority));
    }

    inline NetElement* popVertex() {
        NetElement* last = path.back().first;
        path.pop_back();
        return last;
    }

    inline bool isCompleted() {
        return path.back().first == dest;
    }

    inline NetElement* getSource() const {
        return source;
    }

    inline NetElement* getDest() const {
        return dest;
    }

private:
    NetElement* source;
    NetElement* dest;

    // The second parameter is true if it is high-priority path in this node
    std::list<PathNode> path;
};

#endif
