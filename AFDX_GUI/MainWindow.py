from PyQt4.QtGui import QMainWindow, QFileDialog, QMessageBox, QDialog
from ui.Ui_MainWindow import Ui_MainWindow
from NetworkCanvas import NetworkCanvas, State
from Project import Project
from VirtualLinksEditor import VirtualLinksEditor
from DataFlowsEditor import DataFlowsEditor
from PyQt4.QtCore import Qt
import os, sys
from ui.Ui_OptionsDialog import Ui_OptionsDialog


class OptionsDialog(QDialog):
    def __init__(self):
        QDialog.__init__(self)
        self.ui = Ui_OptionsDialog()
        self.ui.setupUi(self)
        
    def Load(self, mainWindow):
        self.ui.limitJitter.setCheckState( Qt.Checked if mainWindow.limitJitter else Qt.Unchecked )
        self.ui.allowMultipleLinks.setCheckState( Qt.Checked if mainWindow.canvas.allowMultipleLinks else Qt.Unchecked )
        
    def SetResult(self, mainWindow):
        mainWindow.limitJitter = self.ui.limitJitter.isChecked()
        mainWindow.canvas.allowMultipleLinks = self.ui.allowMultipleLinks.isChecked()

class MainWindow(QMainWindow):
    project = None
    projectFile = None

    def __init__(self):
        QMainWindow.__init__(self)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.project = Project()
        self.virtualLinksEditor = VirtualLinksEditor(self.ui.treeWidgetVirtualLinks, self.project)
        self.dataFlowsEditor = DataFlowsEditor(self.ui.treeWidgetDataFlows, self.project)
        self.canvas = NetworkCanvas(self.ui.networkArea, self.virtualLinksEditor, self.dataFlowsEditor)
        self.basename = self.windowTitle()
        self.ui.networkArea.setWidget(self.canvas)
        self.setWindowTitle(self.tr("Untitled") + " - " + self.basename)
        self.canvas.resources = self.project.resources
        self.projFilter = self.tr("AFDX projects (*.afdxxml)")
        self.limitJitter = True
        

    def toggleSelect(self):
        self.ui.actionSelect.setChecked(True)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = State.Select

    def toggleEndSystem(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(True)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = State.EndSystem

    def togglePartition(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(True)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = State.Partition

    def toggleSwitch(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(True)
        self.canvas.state = State.Switch
    
    def toggleLink(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(True)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = State.Link
    
    def toggleVirtualLink(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = State.VirtualLink
        self.virtualLinksEditor.addVirtualLink()
    
    def toggleVirtualLinkItem(self, item):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = self.virtualLinksEditor.getState(item)
        self.canvas.selectVirtualLink(self.virtualLinksEditor.getVirtualLink(item))
        if ( self.canvas.state == State.VirtualLinkPath ):
            self.canvas.selectVirtualLinkPath(self.virtualLinksEditor.getVirtualLinkPath(item))
        
    def toggleVirtualLinkDoubleClicked(self, item):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.virtualLinksEditor.changeParam(item)
        
    def toggleRemoveVirtualLink(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        if self.canvas.state == State.VirtualLink:
            vl = self.virtualLinksEditor.removeSelected()
            self.dataFlowsEditor.updateAllDataFlows(vl)
    
    def toggleDataFlow(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.dataFlowsEditor.addDataFlow()
        self.canvas.state = State.DataFlow
    
    def toggleDataFlowItem(self, item):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.canvas.state = self.dataFlowsEditor.getState(item)
        self.canvas.selectDataFlow(self.dataFlowsEditor.getDataFlow(item))
    
    def toggleDataFlowDoubleClicked(self, item):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        self.dataFlowsEditor.changeParam(item, self.virtualLinksEditor.virtualLinks.values() )
        
    def toggleRemoveDataFlow(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        if self.canvas.state == State.DataFlow:
            self.dataFlowsEditor.removeSelected()
    
    def resizeEvent(self, e):
        super(QMainWindow, self).resizeEvent(e)
        self.canvas.ResizeCanvas()

    def showEvent(self, e):
        super(QMainWindow, self).showEvent(e)
        self.canvas.ResizeCanvas()

    def closeEvent(self, e):
        self.canvas.updatePos()

    def New(self):
        self.project = Project()
        self.canvas.setData(self.project.resources)
        self.virtualLinksEditor.setProject(self.project)
        self.dataFlowsEditor.setProject(self.project)
        self.projectFile = None
        self.setWindowTitle(self.tr("Untitled") + " - " + self.basename)

    def Load(self):
        name = unicode(QFileDialog.getOpenFileName(filter=self.projFilter))
        if name == None or name == '':
            return
        self.OpenProjectFromFile(name)
        self.virtualLinksEditor.setProject(self.project)
        self.dataFlowsEditor.setProject(self.project)
        
    def OpenProjectFromFile(self, name):
        loadProject = Project()
        
        #try:
        loadProject.Load(name)
        #except:
            # TODO: proper exceptioning
        #    QMessageBox.critical(self, self.tr("An error occured"), self.tr("File is not a valid project file: ") + name)
        #    return
        self.project = loadProject
        self.projectFile = name
        self.canvas.setData(self.project.resources)
        self.setWindowTitle(self.project.name + " - " + self.basename)
        
    def Save(self):
        self.canvas.updatePos()
        if self.projectFile == None:
            self.SaveProjectAs()
        else:
            self.project.Save(self.projectFile)
    
    def SaveAs(self):
        self.canvas.updatePos()
        self.SaveProjectAs()
    
    def SaveProjectAs(self):
        self.canvas.updatePos()
        self.projectFile = unicode(QFileDialog.getSaveFileName(directory=self.project.name + ".afdxxml", filter=self.projFilter))
        if self.projectFile != '':
            self.project.Save(self.projectFile)
        self.setWindowTitle(self.projectFile.split('/').pop().split('.')[0] + " - " + self.basename)

    def verify(self):
        file = "tmp/tmp.afdxxml"
        self.project.Save(file)
        if sys.platform.startswith("win"):
            name = "algo/AFDX_DESIGN.exe"
        else:
            name = "algo/AFDX_DESIGN"
        if not os.path.isfile(name):
            QMessageBox.critical(self, self.tr("An error occured"), self.tr("Please build algorithm and put it into algo file"))
        else:
        #os.system(name + " \"" + os.path.relpath(file) + "\" ")
            result = os.popen(name + " \"" + os.path.relpath(file) + "\" v").read()
            print result
            results = result.split('\n')
            if "Not all parameters are specified, some elements are omitted." in results:
                QMessageBox.critical(self, "Warning", "Not all parameters are specified, some elements are omitted.")
            QMessageBox.information(self, "Verification result", results[-2])
            
    def design(self):
        projectFile = self.projectFile
        self.canvas.updatePos()
        file = "tmp/tmp.afdxxml"
        self.project.Save(file)
        if sys.platform.startswith("win"):
            name = "algo/AFDX_DESIGN.exe"
        else:
            name = "algo/AFDX_DESIGN"
        if not os.path.isfile(name):
            QMessageBox.critical(self, self.tr("An error occured"), self.tr("Please build algorithm and put it into algo file"))
        else:
            limitJitter = "t" if self.limitJitter else "f"
            result = os.popen(name + " \"" + os.path.relpath(file) + "\" a" + " --limit-jitter=" + limitJitter).read()
            print result
            results = result.split('\n')
            if "Not all parameters are specified, some elements are omitted." in results:
                QMessageBox.critical(self, "Warning", "Not all parameters are specified, some elements are omitted.")
            else:
                print "loading" 
                self.OpenProjectFromFile(os.path.relpath(file))
                self.virtualLinksEditor.setProject(self.project)
                self.dataFlowsEditor.setProject(self.project)
                self.projectFile = projectFile
                
    def estimateResponseTimes(self):
        projectFile = self.projectFile
        self.canvas.updatePos()
        file = "tmp/tmp.afdxxml"
        self.project.Save(file)
        if sys.platform.startswith("win"):
            name = "algo/AFDX_DESIGN.exe"
        else:
            name = "algo/AFDX_DESIGN"
        if not os.path.isfile(name):
            QMessageBox.critical(self, self.tr("An error occured"), self.tr("Please build algorithm and put it into algo file"))
        else:
            result = os.popen(name + " \"" + os.path.relpath(file) + "\" r").read()
            print result
            results = result.split('\n')
            if "Not all parameters are specified, some elements are omitted." in results:
                QMessageBox.critical(self, "Warning", "Not all parameters are specified, some elements are omitted.")
            else:
                print "loading" 
                self.OpenProjectFromFile(os.path.relpath(file))
                self.virtualLinksEditor.setProject(self.project)
                self.dataFlowsEditor.setProject(self.project)
                self.projectFile = projectFile
                
    def RemoveAllVirtualLinks(self):
        self.ui.actionSelect.setChecked(False)
        self.ui.actionEndSystem.setChecked(False)
        self.ui.actionPartition.setChecked(False)
        self.ui.actionLink.setChecked(False)
        self.ui.actionSwitch.setChecked(False)
        if self.canvas.state == State.VirtualLink:
            vls = self.virtualLinksEditor.removeAll()
            for vl in vls:
                self.dataFlowsEditor.updateAllDataFlows(vl)
            
    def Options(self):
        d = OptionsDialog()
        d.Load(self)
        d.exec_()
        if d.result() == QDialog.Accepted:
            d.SetResult(self)
        
