# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file './JmaxDialog.ui'
#
# Created: Wed Jul  9 13:11:58 2014
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

class Ui_JmaxDialog(object):
    def setupUi(self, JmaxDialog):
        JmaxDialog.setObjectName(_fromUtf8("JmaxDialog"))
        JmaxDialog.resize(262, 117)
        JmaxDialog.setMinimumSize(QtCore.QSize(219, 117))
        JmaxDialog.setMaximumSize(QtCore.QSize(262, 117))
        self.layoutWidget = QtGui.QWidget(JmaxDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 241, 101))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.jmaxlabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.jmaxlabel.sizePolicy().hasHeightForWidth())
        self.jmaxlabel.setSizePolicy(sizePolicy)
        self.jmaxlabel.setObjectName(_fromUtf8("jmaxlabel"))
        self.verticalLayout.addWidget(self.jmaxlabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.jmax = QtGui.QLineEdit(self.layoutWidget)
        self.jmax.setEnabled(True)
        self.jmax.setReadOnly(False)
        self.jmax.setObjectName(_fromUtf8("jmax"))
        self.horizontalLayout_2.addWidget(self.jmax)
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

        self.retranslateUi(JmaxDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), JmaxDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), JmaxDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(JmaxDialog)

    def retranslateUi(self, JmaxDialog):
        JmaxDialog.setWindowTitle(_translate("JmaxDialog", "Dialog", None))
        self.jmaxlabel.setText(_translate("JmaxDialog", "Max message jitter:", None))
        self.OK.setText(_translate("JmaxDialog", "OK", None))
        self.Cancel.setText(_translate("JmaxDialog", "Cancel", None))

