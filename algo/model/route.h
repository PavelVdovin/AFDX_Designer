#ifndef ROUTE
#define ROUTE

#include "defs.h"
#include "path.h"
/*
 * Route consists of the set of paths from source to destinations
 */
class Route {
public:
	Route() {
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
private:
	Paths paths;
};

#endif
