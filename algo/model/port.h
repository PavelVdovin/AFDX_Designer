#ifndef PORT
#define PORT

#include "defs.h"

class Port {
    friend class Factory;
public:
	enum Type {
		NONE,
		FIFO,
		PRIORITIZED
	};

	Port(NetElement* parent, Type type = NONE): parent(0), assosiatedLink(0), type(type) {
	}

	inline Link* getAssosiatedLink() const {
		return assosiatedLink;
	}

	inline NetElement* getParent() const {
		return parent;
	}

private:
	Type type;

	NetElement* parent;
	Link* assosiatedLink;
};

#endif
