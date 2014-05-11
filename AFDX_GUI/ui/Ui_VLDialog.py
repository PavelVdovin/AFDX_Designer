# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'VLDialog.ui'
#
# Created: Sun May 11 02:15:48 2014
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

class Ui_VLDialog(object):
    def setupUi(self, VLDialog):
        VLDialog.setObjectName(_fromUtf8("VLDialog"))
        VLDialog.resize(255, 112)
        VLDialog.setMinimumSize(QtCore.QSize(255, 112))
        VLDialog.setMaximumSize(QtCore.QSize(255, 112))
        self.layoutWidget = QtGui.QWidget(VLDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(0, 12, 251, 100))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.vlLabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.vlLabel.sizePolicy().hasHeightForWidth())
        self.vlLabel.setSizePolicy(sizePolicy)
        self.vlLabel.setObjectName(_fromUtf8("vlLabel"))
        self.verticalLayout.addWidget(self.vlLabel)
        self.vl = QtGui.QComboBox(self.layoutWidget)
        self.vl.setObjectName(_fromUtf8("vl"))
        self.verticalLayout.addWidget(self.vl)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(self.layoutWidget)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(self.layoutWidget)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(VLDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), VLDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), VLDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(VLDialog)

    def retranslateUi(self, VLDialog):
        VLDialog.setWindowTitle(_translate("VLDialog", "Dialog", None))
        self.vlLabel.setText(_translate("VLDialog", "Select VL:", None))
        self.OK.setText(_translate("VLDialog", "OK", None))
        self.Cancel.setText(_translate("VLDialog", "Cancel", None))

