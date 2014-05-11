from PyQt4.QtGui import QMainWindow, QFileDialog, QMessageBox
from ui.Ui_MainWindow import Ui_MainWindow
from NetworkCanvas import NetworkCanvas, State
from Project import Project
from VirtualLinksEditor import VirtualLinksEditor
from DataFlowsEditor import DataFlowsEditor

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
    
    def SaveProjectAs(self):
        self.canvas.updatePos()
        self.projectFile = unicode(QFileDialog.getSaveFileName(directory=self.project.name + ".afdxxml", filter=self.projFilter))
        if self.projectFile != '':
            self.project.Save(self.projectFile)
        self.setWindowTitle(self.projectFile.split('/').pop().split('.')[0] + " - " + self.basename)

