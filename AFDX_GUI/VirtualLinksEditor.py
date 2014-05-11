from PyQt4 import QtGui
from PyQt4.QtGui import QTreeWidget, QTreeWidgetItem, QDialog, QIntValidator
from NetworkCanvas import State
from Resources import VirtualLink
from ui.Ui_NameDialog import Ui_NameDialog
from ui.Ui_BagDialog import Ui_BagDialog
from ui.Ui_LmaxDialog import Ui_LmaxDialog
import math

class VLNameDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_NameDialog()
        self.ui.setupUi(self)
        
    def Load(self, vl):
        self.ui.id.setText(vl.id)
        
    def SetResult(self, vl):
        vl.id = self.ui.id.text()
        
class BagDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_BagDialog()
        self.ui.setupUi(self)
        
    def Load(self, vl):
        self.ui.bag.setCurrentIndex(int(math.log(vl.bag, 2)))
        
    def SetResult(self, vl):
        vl.bag = int(math.pow(2, self.ui.bag.currentIndex()))
        
class LmaxDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_LmaxDialog()
        self.ui.setupUi(self)
        self.valid = QIntValidator(64, 1518, self)
        self.ui.lmax.setValidator(self.valid)
        
    def Load(self, vl):
        self.ui.lmax.setText(str(vl.lmax))
        
    def SetResult(self, vl):
        val = int(self.ui.lmax.text())
        vl.lmax = 64 if val < 64 else val
        
class VirtualLinksEditor:
    
    def __init__(self, treeWidget, project):
        self.treeWidget = treeWidget
        self.virtualLinksCount = 0
        self.selectedItem = None
        self.virtualLinks = {}
        self.paths = {}
        self.project = project
    
    def setProject(self, project):
        self.selectedItem = None
        self.treeWidget.clear()
        self.virtualLinks.clear()
        self.virtualLinksCount = 0
        self.paths.clear()
        self.project = project
        for vl in project.virtualLinks:
            self.addVirtualLink(vl)
            
        self.updateAllVirtualLinks()
        
    # Add exising vl or generate new
    def addVirtualLink(self, vl = None):
        self.virtualLinksCount += 1
        
        id = "Virtual Link " + str(self.virtualLinksCount) if vl == None else vl.id
        vlItem = QTreeWidgetItem(self.treeWidget)
        vlItem.setText(0, id)
        
        bag = 1 if vl == None else vl.bag
        bagItem = QTreeWidgetItem(vlItem)
        bagItem.setText(0, "BAG: " + str(bag) + " ms")
        
        lmax = 1518 if vl == None else vl.lmax
        lmaxItem = QTreeWidgetItem(vlItem)
        lmaxItem.setText(0, "Lmax: " + str(lmax) + " kb")
        
        source = None if vl == None else vl.source and vl.source.id
        sourceItem = QTreeWidgetItem(vlItem)
        sourceItem.setText(0, "Source: " + str(source))
        
        destItem = QTreeWidgetItem(vlItem)
        if vl == None or len(vl.dest) == 0 :
            destItem.setText(0, "Destinations: None")
        else:
            text = "Destinations: "
            for i in range(len(vl.dest) - 1):
                text += vl.dest[i].id + ", "
            text += vl.dest[-1].id
            destItem.setText(0, text)
        
        #creating virtual link
        vl = VirtualLink(id) if vl == None else vl
        self.virtualLinks[vlItem] = vl
        self.paths[self.virtualLinks[vlItem]] = {} # we get the path by vl and destination (each destination generates one path)
        
        if not vl in self.project.virtualLinks:
            self.project.virtualLinks.append(vl)
            
        #path = QTreeWidgetItem(vl)
        #path.setText(0, "Path: None")
        

    def getState(self, item):
        self.selectedItem = item
        if (self.treeWidget.indexOfTopLevelItem(item) != -1):
            return State.VirtualLink
        
        if item.text(0)[:6] == "Source":
            return State.VirtualLinkSource
        if item.text(0)[:12] == "Destinations":
            return State.VirtualLinkDest
        if item.text(0)[:4] == "Path":
            return State.VirtualLinkPath
        
        return State.VirtualLink
    
    def getVirtualLinkPath(self, item):
        virtualLink = self.getVirtualLink(item)
        for dest in virtualLink.dest:
            if self.paths[virtualLink][dest] == item:
                for path in virtualLink.route:
                    if path.dest == dest:
                        return path
        return None
        
        
    def getVirtualLink(self, item):
        if (self.treeWidget.indexOfTopLevelItem(item) == -1):
            item = item.parent()
        
        if item in self.virtualLinks.keys():
            return self.virtualLinks[item]
        return None
    
    def changeParam(self, item):
        if (self.treeWidget.indexOfTopLevelItem(item) != -1):
            self.changeName(item)
        elif item.text(0)[:3] == "BAG":
            self.changeBag(item)
        elif item.text(0)[:4] == "Lmax":
            self.changeLmax(item)
        
    def changeName(self, item):
        if (self.treeWidget.indexOfTopLevelItem(item) >= 0):
            d = VLNameDialog()
            d.Load(self.virtualLinks[item])
            d.exec_()
            if d.result() == QDialog.Accepted:
                d.SetResult(self.virtualLinks[item])
                item.setText(0, self.virtualLinks[item].id)
                
    def changeBag(self, item):
        parent = item.parent()
        vl = self.virtualLinks[parent]
        
        d = BagDialog()
        d.Load(vl)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(vl)
            item.setText(0, "BAG: " + str(vl.bag) + " ms")
            
    def changeLmax(self, item):
        parent = item.parent()
        vl = self.virtualLinks[parent]
        
        d = LmaxDialog()
        d.Load(vl)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(vl)
            item.setText(0, "Lmax: " + str(vl.lmax) + " kb")
                
    def removeSelected(self):
        if ( self.selectedItem != None) and self.selectedItem in self.virtualLinks.keys():
            vl = self.virtualLinks[self.selectedItem]
            
            if vl in self.project.virtualLinks:
                self.project.virtualLinks.remove(vl)
            
            del self.virtualLinks[self.selectedItem]
            self.treeWidget.takeTopLevelItem(self.treeWidget.indexOfTopLevelItem(self.selectedItem))
            del self.selectedItem
            self.selectedItem = None
            return vl
            
    def updateVirtualLink(self, virtualLink, vlItem = None, deletedElem = None):
        if vlItem == None:
            for item, vl in self.virtualLinks.iteritems():
                if vl == virtualLink:
                    vlItem = item
                    break
        
        if vlItem != None:
            for i in range(vlItem.childCount()):
                child = vlItem.child(i)
                if child == None:
                    continue
                if child.text(0)[:6] == "Source":
                    child.setText(0, "Source: " + str(virtualLink.source and virtualLink.source.id))
                elif child.text(0)[:12] == "Destinations":
                    if len(virtualLink.dest) == 0 :
                        child.setText(0, "Destinations: None")
                    else:
                        text = "Destinations: "
                        for i in range(len(virtualLink.dest) - 1):
                            text += virtualLink.dest[i].id + ", "
                        text += virtualLink.dest[-1].id
                        child.setText(0, text)
        
            existingPaths = []    
            if virtualLink.source != None:
                # parsing paths
                for dest in virtualLink.dest:
                    virtualLink.updatePath(dest, deletedElem)
                    existingPaths.append(dest)
                    if not dest in self.paths[virtualLink].keys():
                        path = QTreeWidgetItem(vlItem)
                        path.setText(0, "Path from " + virtualLink.source.id + " to " + dest.id)
                        self.paths[virtualLink][dest] = path
        
            # deleting obsolete paths
            for dest in self.paths[virtualLink].keys():
                if not dest in existingPaths:
                    vlItem.removeChild(self.paths[virtualLink][dest])
                    del self.paths[virtualLink][dest]
                    
                #if item.text(0)[:4] == "Path":
                #    return State.VirtualLinkPath
             
    def updateAllVirtualLinks(self, deletedElem = None):
        for item, vl in self.virtualLinks.iteritems():
            self.updateVirtualLink(vl, item, deletedElem)
        