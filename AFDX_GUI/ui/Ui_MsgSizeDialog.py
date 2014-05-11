# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'MsgSizeDialog.ui'
#
# Created: Sun May 11 12:06:02 2014
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

class Ui_MsgSizeDialog(object):
    def setupUi(self, MsgSizeDialog):
        MsgSizeDialog.setObjectName(_fromUtf8("MsgSizeDialog"))
        MsgSizeDialog.resize(219, 117)
        MsgSizeDialog.setMinimumSize(QtCore.QSize(219, 117))
        MsgSizeDialog.setMaximumSize(QtCore.QSize(219, 117))
        self.layoutWidget = QtGui.QWidget(MsgSizeDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 199, 100))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.msgSizelabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.msgSizelabel.sizePolicy().hasHeightForWidth())
        self.msgSizelabel.setSizePolicy(sizePolicy)
        self.msgSizelabel.setObjectName(_fromUtf8("msgSizelabel"))
        self.verticalLayout.addWidget(self.msgSizelabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.msgSize = QtGui.QLineEdit(self.layoutWidget)
        self.msgSize.setEnabled(True)
        self.msgSize.setReadOnly(False)
        self.msgSize.setObjectName(_fromUtf8("msgSize"))
        self.horizontalLayout_2.addWidget(self.msgSize)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(self.layoutWidget)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(self.layoutWidget)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(MsgSizeDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), MsgSizeDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), MsgSizeDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(MsgSizeDialog)

    def retranslateUi(self, MsgSizeDialog):
        MsgSizeDialog.setWindowTitle(_translate("MsgSizeDialog", "Dialog", None))
        self.msgSizelabel.setText(_translate("MsgSizeDialog", "Message size:", None))
        self.OK.setText(_translate("MsgSizeDialog", "OK", None))
        self.Cancel.setText(_translate("MsgSizeDialog", "Cancel", None))

