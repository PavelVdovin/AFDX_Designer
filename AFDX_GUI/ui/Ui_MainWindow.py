# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MainWindow.ui'
#
# Created: Wed Oct 29 22:18:45 2014
#      by: PyQt4 UI code generator 4.11.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(662, 643)
        MainWindow.setMinimumSize(QtCore.QSize(648, 547))
        self.centralwidget = QtGui.QWidget(MainWindow)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.centralwidget.sizePolicy().hasHeightForWidth())
        self.centralwidget.setSizePolicy(sizePolicy)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.horizontalLayout = QtGui.QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.splitter = QtGui.QSplitter(self.centralwidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.splitter.sizePolicy().hasHeightForWidth())
        self.splitter.setSizePolicy(sizePolicy)
        self.splitter.setMouseTracking(False)
        self.splitter.setAcceptDrops(False)
        self.splitter.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.splitter.setOrientation(QtCore.Qt.Horizontal)
        self.splitter.setOpaqueResize(True)
        self.splitter.setHandleWidth(6)
        self.splitter.setChildrenCollapsible(False)
        self.splitter.setObjectName(_fromUtf8("splitter"))
        self.networkArea = QtGui.QScrollArea(self.splitter)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(1)
        sizePolicy.setVerticalStretch(1)
        sizePolicy.setHeightForWidth(self.networkArea.sizePolicy().hasHeightForWidth())
        self.networkArea.setSizePolicy(sizePolicy)
        self.networkArea.setMinimumSize(QtCore.QSize(388, 0))
        self.networkArea.setFocusPolicy(QtCore.Qt.NoFocus)
        self.networkArea.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.networkArea.setWidgetResizable(False)
        self.networkArea.setObjectName(_fromUtf8("networkArea"))
        self.scrollAreaWidgetContents = QtGui.QWidget()
        self.scrollAreaWidgetContents.setGeometry(QtCore.QRect(0, 0, 393, 488))
        self.scrollAreaWidgetContents.setObjectName(_fromUtf8("scrollAreaWidgetContents"))
        self.networkArea.setWidget(self.scrollAreaWidgetContents)
        self.verticalWidget = QtGui.QWidget(self.splitter)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.verticalWidget.sizePolicy().hasHeightForWidth())
        self.verticalWidget.setSizePolicy(sizePolicy)
        self.verticalWidget.setMinimumSize(QtCore.QSize(236, 0))
        self.verticalWidget.setObjectName(_fromUtf8("verticalWidget"))
        self.gridLayout_2 = QtGui.QGridLayout(self.verticalWidget)
        self.gridLayout_2.setMargin(0)
        self.gridLayout_2.setObjectName(_fromUtf8("gridLayout_2"))
        self.gridLayout_3 = QtGui.QGridLayout()
        self.gridLayout_3.setObjectName(_fromUtf8("gridLayout_3"))
        self.treeWidgetVirtualLinks = QtGui.QTreeWidget(self.verticalWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.treeWidgetVirtualLinks.sizePolicy().hasHeightForWidth())
        self.treeWidgetVirtualLinks.setSizePolicy(sizePolicy)
        self.treeWidgetVirtualLinks.setObjectName(_fromUtf8("treeWidgetVirtualLinks"))
        self.gridLayout_3.addWidget(self.treeWidgetVirtualLinks, 0, 0, 1, 1)
        self.treeWidgetDataFlows = QtGui.QTreeWidget(self.verticalWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Minimum, QtGui.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.treeWidgetDataFlows.sizePolicy().hasHeightForWidth())
        self.treeWidgetDataFlows.setSizePolicy(sizePolicy)
        self.treeWidgetDataFlows.setObjectName(_fromUtf8("treeWidgetDataFlows"))
        self.gridLayout_3.addWidget(self.treeWidgetDataFlows, 1, 0, 1, 1)
        self.gridLayout_2.addLayout(self.gridLayout_3, 0, 0, 1, 1)
        self.horizontalLayout.addWidget(self.splitter)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 662, 32))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        self.menuFile = QtGui.QMenu(self.menubar)
        self.menuFile.setObjectName(_fromUtf8("menuFile"))
        self.menuOptions = QtGui.QMenu(self.menubar)
        self.menuOptions.setObjectName(_fromUtf8("menuOptions"))
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(MainWindow)
        self.statusbar.setObjectName(_fromUtf8("statusbar"))
        MainWindow.setStatusBar(self.statusbar)
        self.toolBar = QtGui.QToolBar(MainWindow)
        self.toolBar.setObjectName(_fromUtf8("toolBar"))
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.toolBar)
        self.toolBar_2 = QtGui.QToolBar(MainWindow)
        self.toolBar_2.setObjectName(_fromUtf8("toolBar_2"))
        MainWindow.addToolBar(QtCore.Qt.TopToolBarArea, self.toolBar_2)
        MainWindow.insertToolBarBreak(self.toolBar_2)
        self.actionOpen = QtGui.QAction(MainWindow)
        icon = QtGui.QIcon()
        icon.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/folder.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionOpen.setIcon(icon)
        self.actionOpen.setObjectName(_fromUtf8("actionOpen"))
        self.actionSave = QtGui.QAction(MainWindow)
        icon1 = QtGui.QIcon()
        icon1.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/cd.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionSave.setIcon(icon1)
        self.actionSave.setObjectName(_fromUtf8("actionSave"))
        self.actionExit = QtGui.QAction(MainWindow)
        self.actionExit.setObjectName(_fromUtf8("actionExit"))
        self.actionNew = QtGui.QAction(MainWindow)
        icon2 = QtGui.QIcon()
        icon2.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/page.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionNew.setIcon(icon2)
        self.actionNew.setObjectName(_fromUtf8("actionNew"))
        self.actionEndSystem = QtGui.QAction(MainWindow)
        self.actionEndSystem.setCheckable(True)
        self.actionEndSystem.setChecked(False)
        icon3 = QtGui.QIcon()
        icon3.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/end_system_selected.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionEndSystem.setIcon(icon3)
        self.actionEndSystem.setObjectName(_fromUtf8("actionEndSystem"))
        self.actionSelect = QtGui.QAction(MainWindow)
        self.actionSelect.setCheckable(True)
        icon4 = QtGui.QIcon()
        icon4.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/select.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionSelect.setIcon(icon4)
        self.actionSelect.setObjectName(_fromUtf8("actionSelect"))
        self.actionPartition = QtGui.QAction(MainWindow)
        self.actionPartition.setCheckable(True)
        icon5 = QtGui.QIcon()
        icon5.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/computer.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionPartition.setIcon(icon5)
        self.actionPartition.setObjectName(_fromUtf8("actionPartition"))
        self.actionSwitch = QtGui.QAction(MainWindow)
        self.actionSwitch.setCheckable(True)
        icon6 = QtGui.QIcon()
        icon6.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/router.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionSwitch.setIcon(icon6)
        self.actionSwitch.setObjectName(_fromUtf8("actionSwitch"))
        self.actionLink = QtGui.QAction(MainWindow)
        self.actionLink.setCheckable(True)
        icon7 = QtGui.QIcon()
        icon7.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/edge.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionLink.setIcon(icon7)
        self.actionLink.setObjectName(_fromUtf8("actionLink"))
        self.actionVirtualLink = QtGui.QAction(MainWindow)
        icon8 = QtGui.QIcon()
        icon8.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/topology.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionVirtualLink.setIcon(icon8)
        self.actionVirtualLink.setObjectName(_fromUtf8("actionVirtualLink"))
        self.actionRemoveVirtualLink = QtGui.QAction(MainWindow)
        icon9 = QtGui.QIcon()
        icon9.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/delete.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionRemoveVirtualLink.setIcon(icon9)
        self.actionRemoveVirtualLink.setObjectName(_fromUtf8("actionRemoveVirtualLink"))
        self.actionDataFlow = QtGui.QAction(MainWindow)
        icon10 = QtGui.QIcon()
        icon10.addPixmap(QtGui.QPixmap(_fromUtf8(":/pics/pics/add.png")), QtGui.QIcon.Normal, QtGui.QIcon.Off)
        self.actionDataFlow.setIcon(icon10)
        self.actionDataFlow.setObjectName(_fromUtf8("actionDataFlow"))
        self.actionRemoveDataFlow = QtGui.QAction(MainWindow)
        self.actionRemoveDataFlow.setIcon(icon9)
        self.actionRemoveDataFlow.setObjectName(_fromUtf8("actionRemoveDataFlow"))
        self.actionVerify = QtGui.QAction(MainWindow)
        self.actionVerify.setObjectName(_fromUtf8("actionVerify"))
        self.actionDesign = QtGui.QAction(MainWindow)
        self.actionDesign.setObjectName(_fromUtf8("actionDesign"))
        self.actionSave_as = QtGui.QAction(MainWindow)
        self.actionSave_as.setObjectName(_fromUtf8("actionSave_as"))
        self.actionEstimate_Response_Times = QtGui.QAction(MainWindow)
        self.actionEstimate_Response_Times.setObjectName(_fromUtf8("actionEstimate_Response_Times"))
        self.actionOptions = QtGui.QAction(MainWindow)
        self.actionOptions.setObjectName(_fromUtf8("actionOptions"))
        self.menuFile.addAction(self.actionNew)
        self.menuFile.addAction(self.actionOpen)
        self.menuFile.addAction(self.actionSave)
        self.menuFile.addAction(self.actionSave_as)
        self.menuFile.addSeparator()
        self.menuFile.addAction(self.actionExit)
        self.menuOptions.addAction(self.actionOptions)
        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuOptions.menuAction())
        self.toolBar.addAction(self.actionNew)
        self.toolBar.addAction(self.actionOpen)
        self.toolBar.addAction(self.actionSave)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionSelect)
        self.toolBar.addAction(self.actionEndSystem)
        self.toolBar.addAction(self.actionPartition)
        self.toolBar.addAction(self.actionSwitch)
        self.toolBar.addAction(self.actionLink)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionVirtualLink)
        self.toolBar.addAction(self.actionRemoveVirtualLink)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.actionDataFlow)
        self.toolBar.addAction(self.actionRemoveDataFlow)
        self.toolBar_2.addAction(self.actionVerify)
        self.toolBar_2.addAction(self.actionDesign)
        self.toolBar_2.addAction(self.actionEstimate_Response_Times)

        self.retranslateUi(MainWindow)
        QtCore.QObject.connect(self.actionEndSystem, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleEndSystem)
        QtCore.QObject.connect(self.actionExit, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.close)
        QtCore.QObject.connect(self.actionSelect, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleSelect)
        QtCore.QObject.connect(self.actionNew, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.New)
        QtCore.QObject.connect(self.actionOpen, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.Load)
        QtCore.QObject.connect(self.actionSave, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.Save)
        QtCore.QObject.connect(self.actionPartition, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.togglePartition)
        QtCore.QObject.connect(self.actionSwitch, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleSwitch)
        QtCore.QObject.connect(self.actionVirtualLink, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleVirtualLink)
        QtCore.QObject.connect(self.actionRemoveVirtualLink, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleRemoveVirtualLink)
        QtCore.QObject.connect(self.actionDataFlow, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleDataFlow)
        QtCore.QObject.connect(self.actionRemoveDataFlow, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleRemoveDataFlow)
        QtCore.QObject.connect(self.actionLink, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.toggleLink)
        QtCore.QObject.connect(self.treeWidgetVirtualLinks, QtCore.SIGNAL(_fromUtf8("itemClicked(QTreeWidgetItem*,int)")), MainWindow.toggleVirtualLinkItem)
        QtCore.QObject.connect(self.treeWidgetVirtualLinks, QtCore.SIGNAL(_fromUtf8("itemDoubleClicked(QTreeWidgetItem*,int)")), MainWindow.toggleVirtualLinkDoubleClicked)
        QtCore.QObject.connect(self.treeWidgetDataFlows, QtCore.SIGNAL(_fromUtf8("itemClicked(QTreeWidgetItem*,int)")), MainWindow.toggleDataFlowItem)
        QtCore.QObject.connect(self.treeWidgetDataFlows, QtCore.SIGNAL(_fromUtf8("itemDoubleClicked(QTreeWidgetItem*,int)")), MainWindow.toggleDataFlowDoubleClicked)
        QtCore.QObject.connect(self.actionVerify, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.verify)
        QtCore.QObject.connect(self.actionDesign, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.design)
        QtCore.QObject.connect(self.actionSave_as, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.SaveAs)
        QtCore.QObject.connect(self.actionEstimate_Response_Times, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.estimateResponseTimes)
        QtCore.QObject.connect(self.actionOptions, QtCore.SIGNAL(_fromUtf8("triggered()")), MainWindow.Options)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow", None))
        self.treeWidgetVirtualLinks.headerItem().setText(0, _translate("MainWindow", "Virtual links", None))
        self.treeWidgetDataFlows.headerItem().setText(0, _translate("MainWindow", "Data flows", None))
        self.menuFile.setTitle(_translate("MainWindow", "File", None))
        self.menuOptions.setTitle(_translate("MainWindow", "Edit", None))
        self.toolBar.setWindowTitle(_translate("MainWindow", "toolBar", None))
        self.toolBar_2.setWindowTitle(_translate("MainWindow", "toolBar_2", None))
        self.actionOpen.setText(_translate("MainWindow", "Open", None))
        self.actionSave.setText(_translate("MainWindow", "Save", None))
        self.actionExit.setText(_translate("MainWindow", "Exit", None))
        self.actionNew.setText(_translate("MainWindow", "New", None))
        self.actionEndSystem.setText(_translate("MainWindow", "EndSystem", None))
        self.actionEndSystem.setToolTip(_translate("MainWindow", "Add end system", None))
        self.actionSelect.setText(_translate("MainWindow", "Select", None))
        self.actionSelect.setToolTip(_translate("MainWindow", "Select elements of network", None))
        self.actionPartition.setText(_translate("MainWindow", "Partition", None))
        self.actionPartition.setToolTip(_translate("MainWindow", "Add new partition", None))
        self.actionSwitch.setText(_translate("MainWindow", "Switch", None))
        self.actionSwitch.setToolTip(_translate("MainWindow", "Add AFDX switch", None))
        self.actionLink.setText(_translate("MainWindow", "link", None))
        self.actionLink.setToolTip(_translate("MainWindow", "Add link", None))
        self.actionVirtualLink.setText(_translate("MainWindow", "virtualLink", None))
        self.actionVirtualLink.setToolTip(_translate("MainWindow", "Add new virtual link", None))
        self.actionRemoveVirtualLink.setText(_translate("MainWindow", "removeVirtualLink", None))
        self.actionRemoveVirtualLink.setToolTip(_translate("MainWindow", "Remove virtual link", None))
        self.actionDataFlow.setText(_translate("MainWindow", "dataFlow", None))
        self.actionDataFlow.setToolTip(_translate("MainWindow", "Add new data flow", None))
        self.actionRemoveDataFlow.setText(_translate("MainWindow", "removeDataFlow", None))
        self.actionRemoveDataFlow.setToolTip(_translate("MainWindow", "Remove data flow", None))
        self.actionVerify.setText(_translate("MainWindow", "verify", None))
        self.actionVerify.setToolTip(_translate("MainWindow", "Verify capacity constraints", None))
        self.actionDesign.setText(_translate("MainWindow", "design", None))
        self.actionDesign.setToolTip(_translate("MainWindow", "design virtual links for data flows", None))
        self.actionSave_as.setText(_translate("MainWindow", "Save as", None))
        self.actionEstimate_Response_Times.setText(_translate("MainWindow", "estimate_response_times", None))
        self.actionOptions.setText(_translate("MainWindow", "Options", None))

import resources_rc
