#ifndef NETWORK
#define NETWORK

#include "defs.h"

class Network {
	friend class XmlReader;
public:
	inline Links getLinks() const {
		return links;
	}

	inline NetElements getNetElements() const {
		return netElements;
	}

private:

	NetElements netElements;
	Links links;
};

#endif
