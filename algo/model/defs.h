#ifndef DEFS
#define DEFS

#include <set>
#include <map>

class Port;
class NetElement;
class EndSystem;
class Switch;
class Link;
class Partition;

class Network;
class DataFlow;
class VirtualLink;

class Path;
class Route;

typedef std::pair<NetElement*, bool> PathNode;

typedef std::set<Port*> Ports;
typedef std::set<Partition*> Partitions;
typedef std::set<NetElement*> NetElements;
typedef std::set<Link*> Links;
typedef std::map<NetElement*, Path*> Paths;
typedef std::set<VirtualLink*> VirtualLinks;
typedef std::set<DataFlow*> DataFlows;
#endif
