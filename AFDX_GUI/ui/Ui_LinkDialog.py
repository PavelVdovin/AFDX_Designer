# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'LinkDialog.ui'
#
# Created: Sun May  4 19:46:30 2014
#      by: PyQt4 UI code generator 4.10.4
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

class Ui_LinkDialog(object):
    def setupUi(self, LinkDialog):
        LinkDialog.setObjectName(_fromUtf8("LinkDialog"))
        LinkDialog.resize(256, 270)
        LinkDialog.setMinimumSize(QtCore.QSize(256, 270))
        LinkDialog.setMaximumSize(QtCore.QSize(256, 270))
        self.layoutWidget = QtGui.QWidget(LinkDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(0, 12, 251, 251))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.timelabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.timelabel.sizePolicy().hasHeightForWidth())
        self.timelabel.setSizePolicy(sizePolicy)
        self.timelabel.setObjectName(_fromUtf8("timelabel"))
        self.verticalLayout.addWidget(self.timelabel)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.capacity = QtGui.QLineEdit(self.layoutWidget)
        self.capacity.setEnabled(True)
        self.capacity.setReadOnly(False)
        self.capacity.setObjectName(_fromUtf8("capacity"))
        self.horizontalLayout_3.addWidget(self.capacity)
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.port1Name = QtGui.QLabel(self.layoutWidget)
        self.port1Name.setObjectName(_fromUtf8("port1Name"))
        self.verticalLayout.addWidget(self.port1Name)
        self.port1Type = QtGui.QComboBox(self.layoutWidget)
        self.port1Type.setObjectName(_fromUtf8("port1Type"))
        self.port1Type.addItem(_fromUtf8(""))
        self.port1Type.addItem(_fromUtf8(""))
        self.verticalLayout.addWidget(self.port1Type)
        self.port2Name = QtGui.QLabel(self.layoutWidget)
        self.port2Name.setObjectName(_fromUtf8("port2Name"))
        self.verticalLayout.addWidget(self.port2Name)
        self.port2Type = QtGui.QComboBox(self.layoutWidget)
        self.port2Type.setObjectName(_fromUtf8("port2Type"))
        self.port2Type.addItem(_fromUtf8(""))
        self.port2Type.addItem(_fromUtf8(""))
        self.verticalLayout.addWidget(self.port2Type)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(self.layoutWidget)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(self.layoutWidget)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(LinkDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), LinkDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), LinkDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LinkDialog)

    def retranslateUi(self, LinkDialog):
        LinkDialog.setWindowTitle(_translate("LinkDialog", "Dialog", None))
        self.timelabel.setText(_translate("LinkDialog", "Capacity:", None))
        self.port1Name.setText(_translate("LinkDialog", "Port 1 type:", None))
        self.port1Type.setItemText(0, _translate("LinkDialog", "FIFO", None))
        self.port1Type.setItemText(1, _translate("LinkDialog", "Prioritized", None))
        self.port2Name.setText(_translate("LinkDialog", "Port 2 type:", None))
        self.port2Type.setItemText(0, _translate("LinkDialog", "FIFO", None))
        self.port2Type.setItemText(1, _translate("LinkDialog", "Prioritized", None))
        self.OK.setText(_translate("LinkDialog", "OK", None))
        self.Cancel.setText(_translate("LinkDialog", "Cancel", None))

