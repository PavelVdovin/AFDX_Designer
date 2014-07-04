# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file './PortTypeDialog.ui'
#
# Created: Thu May 29 11:19:44 2014
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

class Ui_PortTypeDialog(object):
    def setupUi(self, PortTypeDialog):
        PortTypeDialog.setObjectName(_fromUtf8("PortTypeDialog"))
        PortTypeDialog.resize(267, 122)
        self.layoutWidget = QtGui.QWidget(PortTypeDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(0, 12, 261, 101))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.outputPortName = QtGui.QLabel(self.layoutWidget)
        self.outputPortName.setObjectName(_fromUtf8("outputPortName"))
        self.verticalLayout.addWidget(self.outputPortName)
        self.priority = QtGui.QComboBox(self.layoutWidget)
        self.priority.setObjectName(_fromUtf8("priority"))
        self.priority.addItem(_fromUtf8(""))
        self.priority.addItem(_fromUtf8(""))
        self.verticalLayout.addWidget(self.priority)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(self.layoutWidget)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(self.layoutWidget)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(PortTypeDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), PortTypeDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), PortTypeDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(PortTypeDialog)

    def retranslateUi(self, PortTypeDialog):
        PortTypeDialog.setWindowTitle(_translate("PortTypeDialog", "Dialog", None))
        self.outputPortName.setText(_translate("PortTypeDialog", "Output port priority:", None))
        self.priority.setItemText(0, _translate("PortTypeDialog", "Low", None))
        self.priority.setItemText(1, _translate("PortTypeDialog", "High", None))
        self.OK.setText(_translate("PortTypeDialog", "OK", None))
        self.Cancel.setText(_translate("PortTypeDialog", "Cancel", None))

