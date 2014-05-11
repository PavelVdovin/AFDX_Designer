from PyQt4 import QtGui
from PyQt4.QtGui import QTreeWidget, QTreeWidgetItem, QDialog, QMessageBox, QIntValidator
from NetworkCanvas import State
from Resources import DataFlow
from ui.Ui_NameDialog import Ui_NameDialog
from ui.Ui_MsgSizeDialog import Ui_MsgSizeDialog
from ui.Ui_PeriodDialog import Ui_PeriodDialog
from ui.Ui_VLDialog import Ui_VLDialog

class DFNameDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_NameDialog()
        self.ui.setupUi(self)
        
    def Load(self, df):
        self.ui.id.setText(df.id)
        
    def SetResult(self, df):
        df.id = self.ui.id.text()

class MsgSizeDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_MsgSizeDialog()
        self.ui.setupUi(self)
        self.valid = QIntValidator(0, 1000000, self)
        self.ui.msgSize.setValidator(self.valid)
        
    def Load(self, df):
        self.ui.msgSize.setText(str(df.msgSize) )
        
    def SetResult(self, df):
        df.msgSize = int(self.ui.msgSize.text())
        
class PeriodDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_PeriodDialog()
        self.ui.setupUi(self)
        self.valid = QIntValidator(1, 1000000, self)
        self.ui.period.setValidator(self.valid)
        
    def Load(self, df):
        self.ui.period.setText(str(df.period) )
        
    def SetResult(self, df):
        df.period = int(self.ui.period.text())
        
class VLDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_VLDialog()
        self.ui.setupUi(self)
        
    def Load(self, df, virtualLinks):
        if len(virtualLinks) == 0 :
            self.ui.vl.setEnabled(False)
        for virtualLink in virtualLinks:
            self.ui.vl.addItem(virtualLink.id)
            if virtualLink == df.vl:
                self.ui.vl.setCurrentIndex(virtualLinks.index(virtualLink))
        
    def SetResult(self, df, virtualLinks):
        if self.ui.vl.isEnabled():
            vl = virtualLinks[self.ui.vl.currentIndex()]
            if not df.checkVL(vl):
                QMessageBox.critical(self, self.tr("Error"), self.tr("Selected virtual link cannot be chosen as some criteria are unsatisfied"))
            else:
                df.vl = vl 
        
class DataFlowsEditor:
    
    def __init__(self, treeWidget, project):
        self.treeWidget = treeWidget
        self.dataFlowsCount = 0
        self.dataFlows = {}
        self.selectedItem = None
        self.project = project

    def setProject(self, project):
        self.treeWidget.clear()
        self.dataFlowsCount = 0
        self.dataFlows.clear()
        self.selectedItem = None
        self.project = project
        
        for df in self.project.dataFlows:
            self.addDataFlow(df)
        
        self.updateAllDataFlows()
        
    def addDataFlow(self, df = None):
        self.dataFlowsCount += 1
        
        id = "Data Flow " + str(self.dataFlowsCount) if df == None else df.id
        dataFlowItem = QTreeWidgetItem(self.treeWidget)
        dataFlowItem.setText(0, id)
        
        msgSize = 0 if df == None else df.msgSize
        msgSizeItem = QTreeWidgetItem(dataFlowItem)
        msgSizeItem.setText(0, "Message Size: " + str(msgSize) + " kb")
        
        period = 1 if df == None else df.period
        periodItem = QTreeWidgetItem(dataFlowItem)
        periodItem.setText(0, "Period: " + str(period) + " ms")
        
        source = None if df == None else df.source and df.source.id
        sourceItem = QTreeWidgetItem(dataFlowItem)
        sourceItem.setText(0, "Source: " + str(source))
        
        destItem = QTreeWidgetItem(dataFlowItem)
        if df == None or len(df.dest) == 0 :
            destItem.setText(0, "Destinations: None")
        else:
            text = "Destinations: "
            for i in range(len(df.dest) - 1):
                text += df.dest[i].id + ", "
            text += df.dest[-1].id
            destItem.setText(0, text)

        vl = None if df == None else df.vl and df.vl.id
        vlItem = QTreeWidgetItem(dataFlowItem)
        vlItem.setText(0, "Virtual Link: " + str(vl))
        
        df = DataFlow(id) if df == None else df
        self.dataFlows[dataFlowItem] = df
        
        if not df in self.project.dataFlows:
            self.project.dataFlows.append(df)
        
    def getState(self, item):
        self.selectedItem = item
        if (self.treeWidget.indexOfTopLevelItem(item) != -1):
            return State.DataFlow
        
        if item.text(0)[:6] == "Source":
            return State.DataFlowSource
        if item.text(0)[:12] == "Destinations":
            return State.DataFlowDest
        
        return State.DataFlow
    
    def changeParam(self, item, vls):
        if (self.treeWidget.indexOfTopLevelItem(item) != -1):
            self.changeName(item)
        elif item.text(0)[:7] == "Virtual":
            self.changeVL(item, vls)
        elif item.text(0)[:7] == "Message":
            self.changeMsgSize(item)
        elif item.text(0)[:6] == "Period":
            self.changePeriod(item)
    
    def removeSelected(self):
        if ( self.selectedItem != None) and self.selectedItem in self.dataFlows.keys():
            df = self.dataFlows[self.selectedItem]
            if df in self.project.dataFlows:
                self.project.dataFlows.remove(df)
            
            del self.dataFlows[self.selectedItem]
            self.treeWidget.takeTopLevelItem(self.treeWidget.indexOfTopLevelItem(self.selectedItem))
            del self.selectedItem
            self.selectedItem = None
            
    def getDataFlow(self, item):
        if (self.treeWidget.indexOfTopLevelItem(item) == -1):
            item = item.parent()
        
        if item in self.dataFlows.keys():
            return self.dataFlows[item]
        return None
    
    def updateDataFlow(self, dataFlow, dfItem = None, deletedElement = None):
        if dfItem == None:
            for item, df in self.dataFlows.iteritems():
                if dataFlow == df:
                    dfItem = item
                    break
        
        dataFlow.update(deletedElement)
        if dfItem != None:
            for i in range(dfItem.childCount()):
                child = dfItem.child(i)
                if child == None:
                    continue
                if child.text(0)[:6] == "Source":
                    child.setText(0, "Source: " + str(dataFlow.source and dataFlow.source.id))
                elif child.text(0)[:12] == "Destinations":
                    if len(dataFlow.dest) == 0 :
                        child.setText(0, "Destinations: None")
                    else:
                        text = "Destinations: "
                        for i in range(len(dataFlow.dest) - 1):
                            text += dataFlow.dest[i].id + ", "
                        text += dataFlow.dest[-1].id
                        child.setText(0, text)
                elif child.text(0)[:7] == "Virtual":
                    child.setText(0, "Virtual Link: " + str(dataFlow.vl and dataFlow.vl.id))
                    
    def updateAllDataFlows(self, deletedElement = None):
        for item, df in self.dataFlows.iteritems():
            self.updateDataFlow(df, item, deletedElement)

    def changeName(self, item):
        if (self.treeWidget.indexOfTopLevelItem(item) >= 0):
            d = DFNameDialog()
            d.Load(self.dataFlows[item])
            d.exec_()
            if d.result() == QDialog.Accepted:
                d.SetResult(self.dataFlows[item])
                item.setText(0, self.dataFlows[item].id)
                
    def changeMsgSize(self, item):
        parent = item.parent()
        df = self.dataFlows[parent]
        
        d = MsgSizeDialog()
        d.Load(df)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(df)
            item.setText(0, "Message Size: " + str(df.msgSize) + " kb")
            
    def changePeriod(self, item):
        parent = item.parent()
        df = self.dataFlows[parent]
        
        d = PeriodDialog()
        d.Load(df)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(df)
            item.setText(0, "Period: " + str(df.period) + " ms")
                
    def changeVL(self, item, vls):
        parent = item.parent()
        df = self.dataFlows[parent]
        
        d = VLDialog()
        d.Load(df, vls)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(df, vls)
            item.setText(0, "Virtual Link: " + str(df.vl and df.vl.id))
    
                