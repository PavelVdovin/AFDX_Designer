# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file './TmaxDialog.ui'
#
# Created: Sat Jul  5 11:51:33 2014
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

class Ui_TmaxDialog(object):
    def setupUi(self, TmaxDialog):
        TmaxDialog.setObjectName(_fromUtf8("TmaxDialog"))
        TmaxDialog.resize(262, 117)
        TmaxDialog.setMinimumSize(QtCore.QSize(219, 117))
        TmaxDialog.setMaximumSize(QtCore.QSize(262, 117))
        self.layoutWidget = QtGui.QWidget(TmaxDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 241, 101))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.tmaxlabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tmaxlabel.sizePolicy().hasHeightForWidth())
        self.tmaxlabel.setSizePolicy(sizePolicy)
        self.tmaxlabel.setObjectName(_fromUtf8("tmaxlabel"))
        self.verticalLayout.addWidget(self.tmaxlabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.tmax = QtGui.QLineEdit(self.layoutWidget)
        self.tmax.setEnabled(True)
        self.tmax.setReadOnly(False)
        self.tmax.setObjectName(_fromUtf8("tmax"))
        self.horizontalLayout_2.addWidget(self.tmax)
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

        self.retranslateUi(TmaxDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), TmaxDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), TmaxDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(TmaxDialog)

    def retranslateUi(self, TmaxDialog):
        TmaxDialog.setWindowTitle(_translate("TmaxDialog", "Dialog", None))
        self.tmaxlabel.setText(_translate("TmaxDialog", "Max transmition time:", None))
        self.OK.setText(_translate("TmaxDialog", "OK", None))
        self.Cancel.setText(_translate("TmaxDialog", "Cancel", None))

