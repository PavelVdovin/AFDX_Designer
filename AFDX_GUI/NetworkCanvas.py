import math
from PyQt4 import QtGui, QtCore
from PyQt4.QtCore import QPointF, QRect
from PyQt4.QtGui import QImage, QWidget, QPainter, QPainterPath, QColor, QCursor, QDialog, QIntValidator, QTableWidgetItem, QMessageBox
from Resources import EndSystem, Partition, Switch, Link, Port, PortType, Connection, VirtualLink
from ui.Ui_NameDialog import Ui_NameDialog
from ui.Ui_LinkDialog import Ui_LinkDialog

class State:
    ''' Enum representing current editing mode '''
    Select = 0
    EndSystem = 1
    Partition = 2
    Switch = 3
    Link = 4
    VirtualLink = 5
    VirtualLinkSource = 6
    VirtualLinkDest = 7
    VirtualLinkPath = 8
    DataFlow = 9
    DataFlowSource = 10
    DataFlowDest = 11
    

class NameDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_NameDialog()
        self.ui.setupUi(self)
        
    def Load(self, v):
        self.ui.id.setText(v.id)
        
    def SetResult(self, v):
        v.id = self.ui.id.text()
        
class LinkDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_LinkDialog()
        self.ui.setupUi(self)
        self.valid = QIntValidator(0, 1000000, self)
        self.ui.capacity.setValidator(self.valid)
        
    def Load(self, l):
        self.ui.capacity.setText(str(l.capacity) )
        self.port1 = l.port1
        self.port2 = l.port2
        self.ui.port1Name.setText("Port " + self.port1.parent.id + " type:")
        self.ui.port2Name.setText("Port " + self.port2.parent.id + " type:")
        
        if not self.port1.isSwitch:
            self.ui.port1Type.setEnabled(False)
        if not self.port2.isSwitch:
            self.ui.port2Type.setEnabled(False)
            
        self.ui.port1Type.setCurrentIndex(self.port1.type)
        self.ui.port2Type.setCurrentIndex(self.port2.type)
        
    def SetResult(self, l):
        l.capacity = int(self.ui.capacity.text())
        self.port1.type =  PortType.FIFO if (self.ui.port1Type.currentText() == "FIFO") else PortType.PRIORITIZED
        self.port2.type =  PortType.FIFO if (self.ui.port2Type.currentText() == "FIFO") else PortType.PRIORITIZED 
        
class NetworkCanvas(QWidget):
    resources = None
    connections = []
    vertices = {}
    selectedVertex = None
    selectedLink = None
    drawLink = False
    curLink = None
    pressed = False
    state = State.Select
    size = 25.0
    selectedVL = None
    selectedVLSource = None
    selectedVLDest = []
    selectedVLElems = []
    selectedVLAllElems = []
    selectedPath = None
    deletedElem = None # to update everything correctly
    selectedDataFlow = None
    selectedDataFlowSource = None
    selectedDataFlowDest = []
    

    colors = {
              "line": QColor(10, 34, 200),
              "selected": QColor(1, 200, 1),
              "selected_vl_source": QColor(1, 200, 1),
              "selected_vl_vertex": QColor(132, 112, 255),
              "selected_vl_link": QColor(255, 112, 255),
              "selected_vl_link_low": QColor(255, 112, 255),
              "selected_vl_link_high": QColor(150, 0, 0),
              "selected_df_source": QColor(1, 200, 1),
              "selected_df_dest": QColor(132, 112, 255),
              }

    def __init__(self, parent=None, virtualLinksEditor = None, dataFlowsEditor = None):
        QWidget.__init__(self, parent)
        self.virtualLinksEditor = virtualLinksEditor
        self.dataFlowsEditor = dataFlowsEditor
        self.setSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.endSystemIcon = QImage(":/pics/pics/end_system.png")
        self.endSystemSelectedIcon = QImage(":/pics/pics/end_system_selected.png")
        self.partitionIcon = QImage(":/pics/pics/computer.png")
        self.partitionSelectedIcon = QImage(":/pics/pics/computer_selected.png")
        self.switchIcon = QImage(":/pics/pics/router.png")
        self.switchSelectedIcon = QImage(":/pics/pics/router_selected.png")
        self.allowMultipleLinks = False

    def getConnections(self, resources):
        connections = []
        for v in resources.vertices:
            if isinstance(v, Partition):
                if (v.connectedTo != None and v.connection == None):
                    connection = Connection(v, v.connectedTo)
                    connections.append(connection);
                    v.connection = connection
        return connections
        
    def setData(self, data):
        self.resources = data
        self.connections = self.getConnections(self.resources)
        self.selectedVertex = None
        self.selectedLink = None
        self.drawLink = False
        
        self.selectedVL = None
        self.selectedVLSource = None
        self.selectedVLDest = []
        self.selectedVLElems = []
        self.selectedVLAllElems = []
        self.selectedPath = None
        self.selectedDataFlow = None
        self.selectedDataFlowSource = None
        self.selectedDataFlowDest = []
        self.Clear()
        self.Visualize(self.resources)
    
    def getLinkColor(self, link):
        if self.selectedVL == None:
            return "line"
        
        if self.selectedVL.isPriorityLink(link):
            return "selected_vl_link_high"
        else:
            return "selected_vl_link"
        
    def paintEvent(self, event):
        if not self.resources:
            return
        paint = QPainter(self)
        
        for e in (self.resources.links + self.connections):
            if (self.state == State.VirtualLink and e in self.selectedVLAllElems) or (self.state == State.VirtualLinkPath and e in self.selectedVLElems):
                paint.setPen(self.colors[self.getLinkColor(e)])
            elif self.state == State.DataFlow and self.selectedVL != None and e in self.selectedVLAllElems:
                paint.setPen(self.colors[self.getLinkColor(e)])
            elif self.state == State.DataFlow and self.selectedVL != None and self.selectedDataFlow != None and e == self.selectedDataFlow.source.connection:
                paint.setPen(self.colors["selected_df_source"])
            elif self.state == State.DataFlow and self.selectedVL != None and self.selectedDataFlow != None and (e.e1 in self.selectedDataFlow.dest or e.e2 in self.selectedDataFlow.dest):
                paint.setPen(self.colors[self.getLinkColor(e)])
            elif e == self.selectedLink:
                paint.setPen(self.colors["selected"])
            else:
                paint.setPen(self.colors["line"])
            self.drawArrow(paint, self.vertices[e.e1].x() + self.size / 2, self.vertices[e.e1].y() + self.size / 2,
                         self.vertices[e.e2].x() + self.size / 2, self.vertices[e.e2].y() + self.size / 2)
            
        for v in self.vertices.keys():
            # highlighting selected virtual link
            if (self.state == State.VirtualLink or self.state == State.VirtualLinkSource or self.state == State.VirtualLinkPath) and self.selectedVLSource == self.vertices[v]:
                paint.fillRect(self.vertices[v], self.colors["selected_vl_source"])
            elif self.state == State.VirtualLink and (self.vertices[v] in self.selectedVLDest or v in self.selectedVLAllElems):
                paint.fillRect(self.vertices[v], self.colors["selected_vl_vertex"])
            elif (self.state == State.VirtualLinkDest) and self.vertices[v] in self.selectedVLDest:
                paint.fillRect(self.vertices[v], self.colors["selected_vl_vertex"])
            elif (self.state == State.VirtualLinkPath) and v in self.selectedVLElems:
                paint.fillRect(self.vertices[v], self.colors["selected_vl_vertex"])
            
            # highlight selected data flow
            if (self.state == State.DataFlow or self.state == State.DataFlowSource ) and self.selectedDataFlowSource == self.vertices[v]:
                paint.fillRect(self.vertices[v], self.colors["selected_df_source"])
            elif (self.state == State.DataFlow or self.state == State.DataFlowDest ) and self.vertices[v] in self.selectedDataFlowDest:
                paint.fillRect(self.vertices[v], self.colors["selected_df_dest"])
            elif self.state == State.DataFlow and self.selectedVL != None and v == self.selectedVL.source:
                paint.fillRect(self.vertices[v], self.colors["selected_vl_source"])
            elif self.state == State.DataFlow and self.selectedVL != None and v in self.selectedVLAllElems:
                paint.fillRect(self.vertices[v], self.colors["selected_vl_vertex"])
                
            if isinstance(v, EndSystem):
                if self.selectedVertex != self.vertices[v]:
                    paint.drawImage(self.vertices[v], self.endSystemIcon)
                else:
                    paint.drawImage(self.vertices[v], self.endSystemSelectedIcon)
            elif isinstance(v, Partition):
                if self.selectedVertex != self.vertices[v]:
                    paint.drawImage(self.vertices[v], self.partitionIcon)
                else:
                    paint.drawImage(self.vertices[v], self.partitionSelectedIcon)
            elif isinstance(v, Switch):
                if self.selectedVertex != self.vertices[v]:
                    paint.drawImage(self.vertices[v], self.switchIcon)
                else:
                    paint.drawImage(self.vertices[v], self.switchSelectedIcon)
                    
        paint.setPen(self.colors["line"])
        if self.drawLink:
            self.drawArrow(paint, self.curLink[0].x() + self.size / 2, self.curLink[0].y() + self.size / 2,
                           QCursor.pos().x() - self.mapToGlobal(self.geometry().topLeft()).x(),
                           QCursor.pos().y() - self.mapToGlobal(self.geometry().topLeft()).y())
            
        paint.end()
        
        # Updating editors information also
        self.virtualLinksEditor.updateAllVirtualLinks(self.deletedElem)
        self.dataFlowsEditor.updateAllDataFlows(self.deletedElem)
        self.deletedElem = None

    def Visualize(self, r):
        #self.resources = r
        #self.connections = getConnections(self.resources)
        for v in r.vertices:
            rect = QtCore.QRect(v.x - self.size / 2, v.y - self.size / 2, self.size, self.size)
            self.vertices[v] = rect
        self.ResizeCanvas()
        self.repaint()

    def keyPressEvent(self, e):
        if e.key() == QtCore.Qt.Key_Delete:
            if self.selectedVertex != None:
                v = next(v for v in self.vertices.keys() if self.vertices[v] == self.selectedVertex)
                del self.vertices[v]
                if (isinstance(v, Partition) and v.connection != None):
                    connection = v.connection
                    self.connections.remove(v.connection)
                elif isinstance(v, EndSystem):
                    for connection in self.connections[:]:
                        if connection.e1 == v or connection.e2 == v:
                            self.resources.DeleteConnection(connection)
                            self.connections.remove(connection)
                self.resources.DeleteVertex(v)
                self.deletedElem = v
                self.selectedVertex = None
                self.changed = True
                self.repaint()
            elif self.selectedLink != None:
                if ( isinstance(self.selectedLink, Link)):
                    self.resources.DeleteLink(self.selectedLink)
                else:
                    self.resources.DeleteConnection(self.selectedLink)
                    self.connections.remove(self.selectedLink)
                    del self.selectedLink
                self.deletedElem = self.selectedLink
                self.selectedLink = None
                self.changed = True
                self.repaint()
        elif e.key() == QtCore.Qt.Key_Return:
            if self.selectedVertex != None:
                v = next(v for v in self.vertices.keys() if self.vertices[v] == self.selectedVertex)
                self.EditName(v)
                self.repaint()
            elif self.selectedLink != None:
                self.EditLink(self.selectedLink)
                self.repaint()

    def drawArrow(self, paint, x1, y1, x2, y2):
        m = paint.worldMatrix()
        paint.translate(x1,y1)
        pi = 3.1415926
        if abs(x2 - x1) > 0:
            alpha = math.atan(abs(y2-y1)/abs(x2-x1)) * 180 / pi
        else:
            alpha = 90
        if y2 > y1:
            if x2 > x1:
                paint.rotate(alpha)
            else:
                paint.rotate(180-alpha)
        else:
            if x2 > x1:
                paint.rotate(-alpha)
            else:
                paint.rotate(alpha-180)
        endcoord = math.sqrt((x2-x1)**2 + (y2-y1)**2)
        p1 = QPointF(endcoord , 0)
        paint.drawLine(0, 0, p1.x(), 0)
        paint.setWorldMatrix(m)
        
    def mousePressEvent(self, e):
        if self.state == State.Select:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    self.selectedVertex = self.vertices[v]
                    self.repaint()
                    self.pressed = True
                    return
                for link in (self.resources.links + self.connections):
                    a = self.vertices[link.e1].center()
                    b = self.vertices[link.e2].center()
                    c = e.pos()
                    ab = math.sqrt((a.x() - b.x())**2 + (a.y() - b.y())**2)
                    inner = QtCore.QRect(a, b)
                    if inner.contains(c):
                        bc = math.sqrt((c.x() - b.x())**2 + (c.y() - b.y())**2)
                        ac = math.sqrt((a.x() - c.x())**2 + (a.y() - c.y())**2)
                        p = (ab + bc + ac) / 2.0
                        area = math.sqrt(p * (p - ab) * (p - ac) * (p - bc))
                        if area < ab:
                            self.selectedLink = link
                            self.selectedVertex = None
                            self.repaint()
                            return
            self.selectedLink = None
            self.selectedVertex = None
            self.repaint()
            return
        elif self.state == State.EndSystem:
            rect = QtCore.QRect(e.x() - self.size / 2, e.y() - self.size / 2, self.size, self.size)
            endSystem = EndSystem("endSystem" + str(self.resources.endSystemCount))
            self.resources.endSystemCount += 1
            self.vertices[endSystem] = rect
            self.resources.AddVertex(endSystem)
            self.changed = True
            self.ResizeCanvas()
            self.repaint()
        elif self.state == State.Partition:
            rect = QtCore.QRect(e.x() - self.size / 2, e.y() - self.size / 2, self.size, self.size)
            partition = Partition("partition" + str(self.resources.partitionCount))
            self.resources.partitionCount += 1
            self.vertices[partition] = rect
            self.resources.AddVertex(partition)
            self.changed = True
            self.ResizeCanvas()
            self.repaint()
        elif self.state == State.Switch:
            rect = QtCore.QRect(e.x() - self.size / 2, e.y() - self.size / 2, self.size, self.size)
            switch = Switch("switch" + str(self.resources.switchCount))
            self.resources.switchCount += 1
            self.vertices[switch] = rect
            self.resources.AddVertex(switch)
            self.changed = True
            self.ResizeCanvas()
            self.repaint()
        elif self.state == State.Link:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    self.drawLink = True
                    self.curLink = []
                    self.curLink.append(self.vertices[v])
                    self.curLink.append(v)
                    self.repaint()
        elif self.state == State.VirtualLinkSource:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    if not isinstance(v, EndSystem):
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Only end system may be the source of virtual link"))
                    elif v in self.selectedVL.dest:
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Destination cannot be equal to source"))
                    else:    
                        self.selectedVLSource = self.vertices[v]
                        if self.selectedVL.source != None:
                            self.selectedVL.source.outgoingVirtualLinks.remove(self.selectedVL)
                        self.selectedVL.setSource(v)
                        v.outgoingVirtualLinks.append(self.selectedVL) 
                        self.virtualLinksEditor.updateVirtualLink(self.selectedVL)
                        self.repaint()
        elif self.state == State.VirtualLinkDest:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    if not isinstance(v, EndSystem):
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Only end system may be the destination of virtual link"))
                    elif v == self.selectedVL.source:
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Destination cannot be equal to source"))
                    else:
                        if self.vertices[v] in self.selectedVLDest:
                            # removing dest
                            self.selectedVL.addOrRemoveDest(v)
                            self.selectedVLDest.remove(self.vertices[v])
                            v.incomingVirtualLinks.remove(self.selectedVL)
                        else:
                            self.selectedVL.addOrRemoveDest(v)
                            self.selectedVLDest.append(self.vertices[v])
                            v.incomingVirtualLinks.append(self.selectedVL)
                        self.virtualLinksEditor.updateVirtualLink(self.selectedVL)
                        self.repaint()
        elif self.state == State.VirtualLinkPath:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    #self.selectedVertex = self.vertices[v]
                    if self.selectedPath.isLastVertex(v) and len(self.selectedPath.path) > 2:
                        link = self.selectedPath.getElem(-2)
                        removed = self.selectedPath.removeVertex(v, self.resources)
                        if removed:
                            self.selectedVLElems.remove(link)
                            self.selectedVLElems.remove(v)
                    else:
                        added = self.selectedPath.appendVertex(v, self.resources)
                        if added:
                            
                            self.selectedVLElems.append(self.selectedPath.getElem(-2))
                            self.selectedVLElems.append(v)
                        if isinstance(v, EndSystem) and not added:
                            QMessageBox.critical(self, self.tr("Error"), self.tr("Wrong destination"))
                    self.repaint()
                    return
        elif self.state == State.DataFlowSource:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    if not isinstance(v, Partition):
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Only partition may be the source of data flow"))
                    elif v in self.selectedDataFlow.dest:
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Destination cannot be equal to source"))
                    else:
                        self.selectedDataFlowSource = self.vertices[v]
                        self.selectedDataFlow.setSource(v)
                        self.dataFlowsEditor.updateDataFlow(self.selectedDataFlow)
                        self.repaint()
        elif self.state == State.DataFlowDest:
            for v in self.vertices.keys():
                if self.vertices[v].contains(e.pos()):
                    if not isinstance(v, Partition):
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Only partition may be the destination of data flow"))
                    elif v == self.selectedDataFlow.source or (self.selectedDataFlow.source != None and v.connectedTo != None and v.connectedTo == self.selectedDataFlow.source.connectedTo):
                        QMessageBox.critical(self, self.tr("Error"), self.tr("Destination cannot be equal to source"))
                    else:
                        if self.vertices[v] in self.selectedDataFlowDest:
                            # removing dest
                            self.selectedDataFlow.addOrRemoveDest(v)
                            self.selectedDataFlowDest.remove(self.vertices[v])
                        else:
                            self.selectedDataFlow.addOrRemoveDest(v)
                            self.selectedDataFlowDest.append(self.vertices[v])
                        self.dataFlowsEditor.updateDataFlow(self.selectedDataFlow)
                        self.repaint()

    def mouseMoveEvent(self, e):
        if (self.state == State.EndSystem) or (self.state == State.Partition) or (self.state == State.Switch):
            return
        elif self.state == State.Select:
            if self.pressed:
                self.selectedVertex.moveTo(e.pos().x() - self.size / 2, e.pos().y() - self.size / 2)
                self.ResizeCanvas()
                self.repaint()
        elif self.state == State.Link:
            if self.drawLink:
                self.repaint()

    def mouseReleaseEvent(self, e):
        self.pressed = False
        if self.drawLink:
            for v in self.vertices.keys():
                if ( self.vertices[v].contains(e.pos()) and self.curLink[1] != v ):
                    # partition may connect only to end system
                    partition = None
                    endSystem = None
                    if ( isinstance(v, Partition)):
                         partition = v
                         endSystem = self.curLink[1]
                    elif isinstance(self.curLink[1], Partition):
                         partition = self.curLink[1]
                         endSystem = v
                    
                    if ( partition != None ):
                        if ( isinstance(endSystem, EndSystem) and partition.connectedTo == None):
                            connection = Connection(partition, endSystem)
                            self.connections.append(connection)
                            partition.connectedTo = endSystem
                            partition.connection = connection
                        else:
                            QMessageBox.critical(self, self.tr("Error"), self.tr("Partition may be connected to one end system only"))
                    else:
                        if ( not self.allowMultipleLinks and isinstance(self.curLink[1], EndSystem) and (len(self.curLink[1].ports) > 0) ):
                            QMessageBox.critical(self, self.tr("Error"), self.tr("End System may have only one connection to AFDX network"))
                        elif ( not self.allowMultipleLinks and isinstance(v, EndSystem) and (len(v.ports) > 0) ):
                            QMessageBox.critical(self, self.tr("Error"), self.tr("End System may have only one connection to AFDX network"))
                        else:
                            port1 = Port(self.curLink[1])
                            port2 = Port(v)
                            ne = Link(port1, port2)
                            self.resources.AddLink(ne)
            self.drawLink = False
            self.curLink = None 
            self.changed = True    
            self.repaint()

    def mouseDoubleClickEvent(self, e):
        if self.selectedVertex != None:
            v = next(v for v in self.vertices.keys() if self.vertices[v] == self.selectedVertex)
            self.EditName(v)
            self.repaint()
        elif self.selectedLink != None:
            self.EditLink(self.selectedLink)
            self.repaint()


    def updatePos(self):
        for v in self.vertices.keys():
            rect = self.vertices[v]
            v.x = rect.x() + self.size/2
            v.y = rect.y() + self.size/2

    def ResizeCanvas(self):
        maxx = 0
        maxy = 0
        for r in self.vertices.values():
            if r.topRight().x() > maxx:
                maxx = r.topRight().x()
            if r.bottomRight().y() > maxy:
                maxy = r.bottomRight().y()
        self.setGeometry(0, 0, max(maxx + 10, self.parent().width()), max(maxy + 10, self.parent().height()))

    def Clear(self):
        self.vertices = {}
        self.selectedVertex = None
        self.pressed = False
        self.links = {}
        self.drawLink = False
        self.curLink = None
        self.selectedLink = None
        
    def EditName(self, v):
        if isinstance(v, Partition) or isinstance(v, EndSystem) or isinstance(v, Switch) :
            d = NameDialog()
            d.Load(v)
            d.exec_()
            if d.result() == QDialog.Accepted:
                d.SetResult(v)
                self.changed = True
                
    def EditLink(self, l):
        if isinstance(l, Link):
            d = LinkDialog()
            d.Load(l)
            d.exec_()
            if d.result() == QDialog.Accepted:
                d.SetResult(l)
                self.changed = True
                
    def selectVirtualLink(self, virtualLink):
        #selecting source
        if ( virtualLink.source != None):
            self.selectedVLSource = self.vertices[virtualLink.source]
        else:
            self.selectedVLSource = None
            
        #selecting dest
        del self.selectedVLDest[:]
        for v in virtualLink.dest:
            self.selectedVLDest.append(self.vertices[v])
            
        del self.selectedVLAllElems[:]
        for path in virtualLink.route:
            for elem in path.path[1:]:
                self.selectedVLAllElems.append(elem["elem"])
        
        self.selectedLink = None
        self.selectedVL = virtualLink
        self.repaint()
        
    def selectVirtualLinkPath(self, path):
        del self.selectedVLElems[:]
        for elem in path.path[1:]:
            self.selectedVLElems.append(elem["elem"])
        self.selectedVLElems.append(path.dest)
        self.selectedPath = path
        self.selectedLink = None
        self.repaint()
        
    def selectDataFlow(self, dataFlow):
        if dataFlow == None:
            return
        
        #selecting source
        if ( dataFlow.source != None):
            self.selectedDataFlowSource = self.vertices[dataFlow.source]
        else:
            self.selectedDataFlowSource = None
            
        #selecting dest
        del self.selectedDataFlowDest[:]
        for v in dataFlow.dest:
            self.selectedDataFlowDest.append(self.vertices[v])
        
        if dataFlow.vl != None:
            self.selectedVL = dataFlow.vl
            del self.selectedVLAllElems[:]
            for path in dataFlow.vl.route:
                for elem in path.path[1:]:
                    self.selectedVLAllElems.append(elem["elem"])
        else:
            self.selectedVL = None
            del self.selectedVLAllElems[:]
            
        
        self.selectedLink = None
        self.selectedDataFlow = dataFlow
        self.repaint()

