#ifndef NETWORK
#define NETWORK

#include "defs.h"

class Network {
	friend class XmlReader;
public:
	inline Links& getLinks() {
		return links;
	}

	inline NetElements& getNetElements() {
		return netElements;
	}

private:

	NetElements netElements;
	Links links;
};

#endif
