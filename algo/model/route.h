#ifndef ROUTE
#define ROUTE

#include "defs.h"
#include "path.h"

#include <stdio.h>
/*
 * Route consists of the set of paths from source to destinations
 */
class Route {
public:
	Route() {
	}

	Route(const Route& other) {
        this->paths.clear();
        this->paths.insert(other.paths.begin(), other.paths.end());
    }

	Route(const Paths& paths) {
	    if ( &paths != &this->paths ) {
	        this->paths.clear();
	        this->paths.insert(paths.begin(), paths.end());
	    }
	}

	~Route() {
	    Paths::iterator it = paths.begin();
	    for ( ; it != paths.end(); ++it ) {
	        delete (it->second);
	        it->second = 0;
	    }
	}

	inline Path* getPath(NetElement* dest) {
		if ( paths.find(dest) != paths.end() )
			return paths[dest];

		return 0;
	}

	inline void addPath(NetElement* dest, Path* path) {
		paths[dest] = path;
	}

	inline void removePath(NetElement* dest) {
		paths.erase(dest);
	}

	inline Paths& getPaths() {
	    return paths;
	}
private:
	Paths paths;
};

#endif
