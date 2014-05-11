# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'PeriodDialog.ui'
#
# Created: Sun May 11 12:06:18 2014
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

class Ui_PeriodDialog(object):
    def setupUi(self, PeriodDialog):
        PeriodDialog.setObjectName(_fromUtf8("PeriodDialog"))
        PeriodDialog.resize(219, 117)
        PeriodDialog.setMinimumSize(QtCore.QSize(219, 117))
        PeriodDialog.setMaximumSize(QtCore.QSize(219, 117))
        self.layoutWidget = QtGui.QWidget(PeriodDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 199, 100))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.periodlabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.periodlabel.sizePolicy().hasHeightForWidth())
        self.periodlabel.setSizePolicy(sizePolicy)
        self.periodlabel.setObjectName(_fromUtf8("periodlabel"))
        self.verticalLayout.addWidget(self.periodlabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.period = QtGui.QLineEdit(self.layoutWidget)
        self.period.setEnabled(True)
        self.period.setReadOnly(False)
        self.period.setObjectName(_fromUtf8("period"))
        self.horizontalLayout_2.addWidget(self.period)
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

        self.retranslateUi(PeriodDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), PeriodDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), PeriodDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(PeriodDialog)

    def retranslateUi(self, PeriodDialog):
        PeriodDialog.setWindowTitle(_translate("PeriodDialog", "Dialog", None))
        self.periodlabel.setText(_translate("PeriodDialog", "Period:", None))
        self.OK.setText(_translate("PeriodDialog", "OK", None))
        self.Cancel.setText(_translate("PeriodDialog", "Cancel", None))

