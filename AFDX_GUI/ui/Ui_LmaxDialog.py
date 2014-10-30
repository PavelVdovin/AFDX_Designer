# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'LmaxDialog.ui'
#
# Created: Thu Oct 30 15:30:34 2014
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

class Ui_LmaxDialog(object):
    def setupUi(self, LmaxDialog):
        LmaxDialog.setObjectName(_fromUtf8("LmaxDialog"))
        LmaxDialog.resize(219, 117)
        LmaxDialog.setMinimumSize(QtCore.QSize(219, 117))
        LmaxDialog.setMaximumSize(QtCore.QSize(219, 117))
        self.layoutWidget = QtGui.QWidget(LmaxDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 199, 100))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.lmaxlabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.lmaxlabel.sizePolicy().hasHeightForWidth())
        self.lmaxlabel.setSizePolicy(sizePolicy)
        self.lmaxlabel.setObjectName(_fromUtf8("lmaxlabel"))
        self.verticalLayout.addWidget(self.lmaxlabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.lmax = QtGui.QLineEdit(self.layoutWidget)
        self.lmax.setEnabled(True)
        self.lmax.setReadOnly(False)
        self.lmax.setObjectName(_fromUtf8("lmax"))
        self.horizontalLayout_2.addWidget(self.lmax)
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

        self.retranslateUi(LmaxDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), LmaxDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), LmaxDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(LmaxDialog)

    def retranslateUi(self, LmaxDialog):
        LmaxDialog.setWindowTitle(_translate("LmaxDialog", "Dialog", None))
        self.lmaxlabel.setText(_translate("LmaxDialog", "Lmax (bytes):", None))
        self.OK.setText(_translate("LmaxDialog", "OK", None))
        self.Cancel.setText(_translate("LmaxDialog", "Cancel", None))

