from Resources import Network
import xml.dom.minidom
from Resources import Link, VirtualLink, Path, DataFlow, Priority

class Project:
    resources = None
    virtualLinks = None
    dataFlows = None
    name = "test project"

    def __init__(self):
        self.resources = Network()
        self.virtualLinks = []
        self.dataFlows = []

    def Save(self, filename):
        dom = xml.dom.minidom.Document()
        root = dom.createElement("afdxxml")
        root.setAttribute("name", self.name)
        resgraph = self.resources.CreateXml(dom)
        vlsXml = self.SaveVirtualLinks(dom)
        dfsXml = self.SaveDataFlows(dom)
        root.appendChild(resgraph)
        root.appendChild(vlsXml)
        root.appendChild(dfsXml)
        dom.appendChild(root)
        f = open(filename, "w")
        f.write(dom.toprettyxml())
        f.close()
        
    def SaveVirtualLinks(self, dom):
        root = dom.createElement("virtualLinks")
        for vl in self.virtualLinks:
            tag = dom.createElement("virtualLink")
            tag.setAttribute("id", vl.id)
            tag.setAttribute("number", str(self.virtualLinks.index(vl) + 1))
            tag.setAttribute("bag", str(vl.bag))
            tag.setAttribute("lmax", str(vl.lmax))
            tag.setAttribute("source", str(vl.source and vl.source.number))
            if len(vl.dest) == 0:
                tag.setAttribute("dest", str("None"))
            else:
                text = ""
                for i in range(len(vl.dest) - 1):
                    text += str(vl.dest[i].number) + ","
                text += str(vl.dest[-1].number)
                tag.setAttribute("dest", text)
            
            # append paths
            for path in vl.route:
                p = dom.createElement("path")
                p.setAttribute("source", str(path.source and path.source.number))
                p.setAttribute("dest", str(path.dest and path.dest.number))
                text = ""
                for i in range(len(path.path) - 1):
                    elem = path.path[i]["elem"]
                    if not isinstance(elem, Link):
                        text += str(elem.number)
                    else:
                        if "priority" in path.path[i]:
                            text += "h" if path.path[i]["priority"] == Priority.HIGH else "l"
                        text += ","
                text += str(path.path[-1]["elem"].number)
                p.setAttribute("path", text)
                tag.appendChild(p)

            root.appendChild(tag)
                        
        return root
    
    def SaveDataFlows(self, dom):
        root = dom.createElement("dataFlows")
        for df in self.dataFlows:
            tag = dom.createElement("dataFlow")
            tag.setAttribute("id", df.id)
            tag.setAttribute("msgSize", str(df.msgSize))
            tag.setAttribute("period", str(df.period))
            tag.setAttribute("source", str(df.source and df.source.number))
            if len(df.dest) == 0:
                tag.setAttribute("dest", str("None"))
            else:
                text = ""
                for i in range(len(df.dest) - 1):
                    text += str(df.dest[i].number) + ","
                text += str(df.dest[-1].number)
                tag.setAttribute("dest", text)
            
            tag.setAttribute("vl", str(df.vl and self.virtualLinks.index(df.vl) + 1))
            root.appendChild(tag)
        return root

    def Load(self, filename):
        f = open(filename, "r")
        dom = xml.dom.minidom.parse(f)
        for root in dom.childNodes:
            if root.tagName == "afdxxml":
                self.name = root.getAttribute("name")
                for node in root.childNodes:
                    if isinstance(node, xml.dom.minidom.Text):
                        continue
                    if node.tagName == "resources":
                        resources = Network()
                        resources.LoadFromXmlNode(node)
                        self.resources = resources
                    elif node.tagName == "virtualLinks":
                        self.virtualLinks = []
                        self.LoadVirtualLinks(node)
                    elif node.tagName == "dataFlows":
                        self.dataFlows = []
                        self.LoadDataFlows(node)
        f.close()
        
    def LoadVirtualLinks(self, node):
        for vlNode in node.childNodes:
            if isinstance(vlNode, xml.dom.minidom.Text):
                continue
            
            id = vlNode.getAttribute("id")
            vl = VirtualLink(id)
            vl.number = vlNode.getAttribute("number")
            vl.bag = int(vlNode.getAttribute("bag"))
            vl.lmax = int(vlNode.getAttribute("lmax"))
            
            sourceId = vlNode.getAttribute("source") 
            if sourceId == "None":
                vl.source = None
            else:
                vl.source = self.resources.vertices[int(sourceId) - 1]
            
            destStr = vlNode.getAttribute("dest")
            if destStr != "None":
                dests = destStr.split(",")
                for index in dests:
                    dest = self.resources.vertices[int(index) - 1]
                    vl.dest.append(dest)
            
            for pathNode in vlNode.childNodes:
                if isinstance(pathNode, xml.dom.minidom.Text):
                    continue
                path = Path()
                
                sourceId = pathNode.getAttribute("source")
                if sourceId == "None":
                    path.source = None
                else:
                    path.source = self.resources.vertices[int(sourceId) - 1]
                    
                destId = pathNode.getAttribute("dest")
                if destId == "None":
                    path.dest = None
                else:
                    path.dest = self.resources.vertices[int(destId) - 1]
                    
                pathStr = pathNode.getAttribute("path")
                if pathStr != "":
                    elems = pathStr.split(",")
                    elem = self.resources.vertices[int(elems[0]) - 1] # source!
                    
                    if elem != path.source:
                        raise "Assertion: source in path is not equal to source in xml"
                    
                    path.path.append({"elem": elem})
                    prevPriority = 0
                    for index in range(1, len(elems)):
                        priority = 0
                        if elems[index][-1] == 'h' or elems[index][-1] == 'l':
                            priority = Priority.HIGH if elems[index][-1] == 'h' else Priority.LOW
                            elems[index] = elems[index][:-1]
                        
                        elem = self.resources.vertices[int(elems[index]) - 1]
                        path.appendVertex(elem, self.resources, prevPriority)
                        prevPriority = priority
                    
                    vl.route.append(path)
                    
            self.virtualLinks.append(vl)
            
        self.virtualLinks.sort(key=lambda x: x.number)

    def LoadDataFlows(self, node):
        for dfNode in node.childNodes:
            if isinstance(dfNode, xml.dom.minidom.Text):
                continue
            
            id = dfNode.getAttribute("id")
            df = DataFlow(id)
            df.msgSize = int(dfNode.getAttribute("msgSize"))
            df.period = int(dfNode.getAttribute("period"))
            
            sourceId = dfNode.getAttribute("source") 
            if sourceId == "None":
                df.source = None
            else:
                df.source = self.resources.vertices[int(sourceId) - 1]
            
            destStr = dfNode.getAttribute("dest")
            if destStr != "None":
                dests = destStr.split(",")
                for index in dests:
                    dest = self.resources.vertices[int(index) - 1]
                    df.dest.append(dest)
                    
            vlId = dfNode.getAttribute("vl") 
            if vlId == "None":
                df.vl = None
            else:
                df.vl = self.virtualLinks[int(vlId) - 1]
            
            self.dataFlows.append(df)
        