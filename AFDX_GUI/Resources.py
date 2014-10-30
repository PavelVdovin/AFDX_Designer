import xml.dom.minidom
from PyQt4 import QtGui
from PyQt4.QtGui import QDialog
from ui.Ui_PortTypeDialog import Ui_PortTypeDialog

class Priority:
    ''' Enum representing link priority on outgoing port'''
    LOW = 0
    HIGH = 1
    
    
class PortTypeDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_PortTypeDialog()
        self.ui.setupUi(self)
        
    def Load(self, port):
        self.ui.priority.setCurrentIndex(0)
        
    def SetResult(self, result):
        result["priority"] = Priority.LOW if (self.ui.priority.currentText() == "Low") else Priority.HIGH

class AbstractVertex:
    def __init__(self, id):
        self.id = id
        self.ports = []
        
    def addPort(self, port):
        if ( not port in self.ports ):
            self.ports.append(port)
            
    def DeletePort(self, port):
        if ( port in self.ports ):
            self.ports.remove(port)

class PortType:
    ''' Enum representing current editing mode '''
    FIFO = 0
    PRIORITIZED = 1
    
        
class EndSystem(AbstractVertex):
    def __init__(self, id):
        AbstractVertex.__init__(self, id)
        
        self.outgoingVirtualLinks = []
        self.incomingVirtualLinks = []
        
class Partition():
    def __init__(self, id):
        self.id = id
        self.connectedTo = None
        self.connection = None

class Switch(AbstractVertex):
    def __init__(self, id):
        AbstractVertex.__init__(self, id)
        
        
class Port:
    def __init__(self, parent, type = PortType.FIFO):
        self.parent = parent
        self.link = None
        parent.addPort(self)
        self.isSwitch = isinstance(parent, Switch)
        self.type = type

'''Connection- used only to connect partition and end system'''
class Connection:
    def __init__(self, e1, e2):
        self.e1 = e1
        self.e2 = e2
        
class Link:
    def __init__(self, port1, port2, capacity = 12500):
        self.e1 = port1.parent
        self.e2 = port2.parent
        port1.link = self
        port2.link = self
        self.capacity = capacity
        self.port1 = port1
        self.port2 = port2

class Path:
    def __init__(self):
        #self.virtualLink = virtualLink
        self.source = None
        self.dest = None
        self.path = [] # path is vertex, link, vertex, link ...
    
    def buildPath(self, source, dest):
        self.source = source
        self.dest = dest
        pass #TODO: dejkstra analog of building path
    
    # appending to existing path
    def appendLink(self, link, priority = None):
        firstElem = self.source
        lastElem = self.getElem(-1)
        
        if lastElem == None or firstElem == None:
            return False
            
        nextElem = None
        isPrioritized = False # identify whether outgoing port is prioritized
        if lastElem == link.e1:
            nextElem = link.e2
            isPrioritized = (link.port1.type == PortType.PRIORITIZED)
        elif lastElem == link.e2:
            nextElem = link.e1
            isPrioritized = (link.port2.type == PortType.PRIORITIZED)
        
        if nextElem != None and (isinstance(nextElem, Switch) or isinstance(nextElem, EndSystem) and nextElem == self.dest):
            result = {}
            result["elem"] = link
            if isPrioritized:
                result["priority"] = priority or Priority.LOW
                if priority == None:
                    d = PortTypeDialog()
                    d.Load(result)
                    d.exec_()
                    if d.result() == QDialog.Accepted:
                        d.SetResult(result)
            self.path.append(result)
            self.path.append({"elem": nextElem})
            return True
        
        return False
    
    # Removing last (or first) link
    def removeLink(self, link):
        if len(self.path) < 3:
            return False
        lastLink = self.getElem(-2)
        
        if lastLink == link:
            #removing two last elements
            del self.path[-1]
            del self.path[-1]
            #self.path.remove(self.path[-1])
            #self.path.remove(self.path[-1])
            return True
        return False
    
    def appendVertex(self, v, network, priority = None):
        if self.source == None:
            return False
        
        for l in network.links:
            if (l.e1 == v or l.e2 == v) and self.appendLink(l, priority):
                return True
        return False
    
    def removeVertex(self, v, network):
        if v != self.path[-1]["elem"]:
            return False
        for l in network.links:
            if (l.e1 == v or l.e2 == v) and self.removeLink(l):
                return True
        return False
    
    def getElem(self, index):
        if len(self.path) > index:
            return self.path[index]["elem"]
        return None
    
    def isLastVertex(self, v):
        return self.path[-1]["elem"] == v
    
    def isPriorityLink(self, link):
        for elem in self.path:
            if elem["elem"] == link:
                if "priority" in elem:
                    #print "Elem found, priority = " + elem["priority"] 
                    return elem["priority"] == Priority.HIGH
                return False
        return False
                
    
class VirtualLink:
    def __init__(self, id):
        self.id = id
        self.bag = 1
        self.lmax = 1518
        self.source = None
        self.dest = []
        self.route = [] # set of Path
        self.responseTime = 0
    
    def setSource(self, v):
        if ( v != self.source ):
            del self.route[:] 
            self.source = v
        
    def addOrRemoveDest(self, v):
        if self.dest.count(v) == 0:
            self.dest.append(v)
        else:
            for path in self.route:
                if path.dest == v:
                    self.route.remove(path)
            self.dest.remove(v)
        
    def addPath(self, path):
        if (self.source == None):
            return "Source not specified" # source should be set first
            
        if self.source != path.source and self.source != path.dest:
            return "Source not match"
        
        if self.source == path.dest:
            path.path.reverse()
            source = path.source
            path.source = path.dest
            path.dest = source 
        
        if self.dest.count(path.dest) == 0:
            return "Destination not match"
                         
        self.route.append(path)
        return True
    
    # Update path from source to dest:
    # check whether the path still exists or initiate path
    def updatePath(self, dest, deletedElem = None):
        #destPath = None
        for path in self.route:
            if path.dest == dest:
                if deletedElem != None and deletedElem in path.path:
                    self.route.remove(path)
                else:
                    return # don't need to update path
        
        if deletedElem == self.source:
            self.source = None
            return
        
        if deletedElem in self.dest:
            self.dest.remove(deletedElem)
            return 
        
        # create new path for specified dest
        path = Path()
        path.source = self.source
        path.dest = dest
        path.path.append({"elem":path.source})
        self.route.append(path)
        
    def isPriorityLink(self, link):
        for path in self.route:
            for elem in path.path:
                if elem["elem"] == link:
                    if "priority" in elem:
                        #print "Elem found, priority = " + elem["priority"] 
                        return elem["priority"] == Priority.HIGH
                    return False
        return False

class DataFlow:
    
    def __init__(self, id):
        self.id = id
        self.msgSize = 0
        self.period = 1
        self.tMax = 0
        self.jMax = 0
        self.source = None
        self.dest = []
        self.vl = None
        
    def setSource(self, v):
        if v != self.source:
            self.source = v
            self.vl = None
            
    def addOrRemoveDest(self, v):
        if self.dest.count(v) == 0:
            self.dest.append(v)
            if not self.checkVL(self.vl):
                self.vl = None
        else:
            self.dest.remove(v)
            
    def checkVL(self, vl):
        if vl == None:
            return True
        
        if self.source == None or self.source.connectedTo == None:
            return False
        
        if self.source.connectedTo != vl.source:
            return False
        
        for dest in self.dest:
            if dest.connectedTo == None or not dest.connectedTo in vl.dest:
                return False
        
        return True
    
    def update(self, deletedElement):
        if deletedElement != None:
            if self.source == deletedElement:
                self.setSource(None)
                return
            
            if deletedElement in self.dest:
                self.addOrRemoveDest(deletedElement)
                return
            
            if self.vl == deletedElement:
                self.vl = None
            
        if not self.checkVL(self.vl):
            self.vl = None
        
        if self.source != None:
            for dest in self.dest:
                if dest.connectedTo != None and dest.connectedTo == self.source.connectedTo:
                    self.dest.remove(dest)
        
class Network:

    def __init__(self):
        self.vertices = []
        self.links = []
        self.endSystemCount = 1
        self.partitionCount = 1
        self.switchCount = 1

    def AddLink(self, e):
        ''' Add link'''
        self.links.append(e)

    def AddVertex(self, v):
        self.vertices.append(v)
        v.number = len(self.vertices)

    def DeleteConnection(self, conn):
        for v in self.vertices:
            if (isinstance(v, Partition) and v.connection == conn):
                v.connection = None
                v.connectedTo = None
                
    def DeleteVertex(self, v):
        ind = self.vertices.index(v)
        new_edges = []
        for v in self.vertices[ind:]:
            v.number -= 1
        
        v = self.vertices[ind]
        for link in self.links:
            if link.e1 == v or link.e2 == v:
                self.DeleteLink(link)
                
        if (isinstance(v, Partition) and v.connection != None):
            self.DeleteConnection(v.connection)
        
        if (isinstance(v, EndSystem)):
            for vl in v.outgoingVirtualLinks:
                vl.source = None
            for vl in v.incomingVirtualLinks:
                vl.dest.remove(v)
        
        del self.vertices[ind]
        
        
    def DeleteLink(self, ed):
        ''' Delete edge'''
        ed.e1.DeletePort(ed.port1)
        ed.e2.DeletePort(ed.port2)
        del ed.port1
        del ed.port2
        new_links = []
        for e in self.links:
            if e != ed:
                new_links.append(e)
            else:
                del e
        self.links = new_links

    def Reset(self):
        self.vertices = []
        self.links = []
        self.endSystemCount = 1
        self.partitionCount = 1
        self.switchCount = 1
        
    def AppendPortsNumbers(self):
        index = 1
        for v in self.vertices:
            if ( not isinstance(v, Partition) ):
                for port in v.ports:
                    port.number = index
                    index += 1
             
    def AddXmlPorts(self, v, dom):
        portsStr = ""
        for port in v.ports:
            if len(portsStr) > 0:
                portsStr += ","
            portsStr += str(port.number)
        dom.setAttribute("ports", portsStr)
        
    def CreateXml(self, dom):
        root = dom.createElement("resources")
        # TODO: take some meaningful interval
        
        self.AppendPortsNumbers()
        for v in self.vertices:
            if isinstance(v, EndSystem):
                tag = dom.createElement("endSystem")
                self.AddXmlPorts(v, tag)
            elif isinstance(v, Partition):
                tag = dom.createElement("partition")
                if ( v.connectedTo != None):
                    tag.setAttribute("connectedTo", str(v.connectedTo.number))
                else:
                    tag.setAttribute("connectedTo", "None")
            elif isinstance(v, Switch):
                tag = dom.createElement("switch")
                self.AddXmlPorts(v, tag)
            if v.x:
                tag.setAttribute("x", str(v.x))
                tag.setAttribute("y", str(v.y))
            tag.setAttribute("number", str(v.number))
            tag.setAttribute("name", str(v.id))
            root.appendChild(tag)
        for link in self.links:
            tag = dom.createElement("link")
            tag.setAttribute("capacity", str(link.capacity))
            tag.setAttribute("from", str(link.port1.number))
            tag.setAttribute("fromType", str(link.port1.type))
            tag.setAttribute("to", str(link.port2.number))
            tag.setAttribute("toType", str(link.port2.type))
            root.appendChild(tag)
        return root
     
    def LoadFromXML(self, filename):
        ''' Load edges and vertices from XML
        
        .. warning:: Describe XML format here'''
        f = open(filename, "r")
        dom = xml.dom.minidom.parse(f)
        self.vertices = []
        #self.edges = []
        for node in dom.childNodes:
            if node.tagName == "resources":
                self.LoadFromXmlNode(node)
        f.close()
    
    def parsePorts(self, v, dom):
        result = {}
        portsStr = dom.getAttribute("ports")
        if portsStr != "":
            ports = portsStr.split(",")
            for index in ports:
                port = Port(v)
                result[int(index)] = port
        return result
        
    def LoadFromXmlNode(self, node):
        #Parse vertices
        ports = {}
        partitionsConnection = {}
        for vertex in node.childNodes:
            if isinstance(vertex, xml.dom.minidom.Text):
                continue
            if vertex.nodeName == "link":
                continue
            name = vertex.getAttribute("name")
            number = int(vertex.getAttribute("number"))
            if vertex.nodeName == "endSystem":
                #speed = int(vertex.getAttribute("speed"))
                #ram = int(vertex.getAttribute("ramcapacity")) if vertex.hasAttribute("ramcapacity") else 0
                v = EndSystem(name)
                self.endSystemCount += 1
                vertexPorts = self.parsePorts(v, vertex)
                ports = dict(ports.items() + vertexPorts.items())
            elif vertex.nodeName == "partition":
                #speed = int(vertex.getAttribute("speed"))
                #ram = int(vertex.getAttribute("ramcapacity")) if vertex.hasAttribute("ramcapacity") else 0
                v = Partition(name)
                self.partitionCount += 1
                connectedToNumber = vertex.getAttribute("connectedTo")
                if (connectedToNumber == "None"):
                    v.connectedTo = None
                    v.connection = None
                else:
                    partitionsConnection[number] = connectedToNumber
            elif vertex.nodeName == "switch":
                #speed = int(vertex.getAttribute("speed"))
                #ram = int(vertex.getAttribute("ramcapacity")) if vertex.hasAttribute("ramcapacity") else 0
                v = Switch(name)
                self.switchCount += 1
                vertexPorts = self.parsePorts(v, vertex)
                ports = dict(ports.items() + vertexPorts.items())
            x = vertex.getAttribute("x")
            y = vertex.getAttribute("y")
            if x != '':
                v.x = float(x)
            if y != '':
                v.y = float(y)
            v.number = number
            self.vertices.append(v)
        
        self.vertices.sort(key=lambda x: x.number)
        for number, connectedToNumber in partitionsConnection.iteritems():
            partition = self.vertices[number-1]
            self.vertices[number-1].connectedTo = self.vertices[int(connectedToNumber) - 1]
            self.vertices[number-1].connection == None
        
        #Parse links
        for link in node.childNodes:
            if link.nodeName == "link":
                source = int(link.getAttribute("from"))
                destination = int(link.getAttribute("to"))
                e = Link(ports[source], ports[destination], 0)
                e.capacity = link.getAttribute("capacity")
                ports[source].type = PortType.FIFO if int(link.getAttribute("fromType")) == 0 else PortType.PRIORITIZED
                ports[destination].type = PortType.FIFO if int(link.getAttribute("toType")) == 0 else PortType.PRIORITIZED
                self.links.append(e)
        