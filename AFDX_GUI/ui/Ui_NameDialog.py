# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'NameDialog.ui'
#
# Created: Sun May  4 19:24:55 2014
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

class Ui_NameDialog(object):
    def setupUi(self, NameDialog):
        NameDialog.setObjectName(_fromUtf8("NameDialog"))
        NameDialog.resize(219, 117)
        NameDialog.setMinimumSize(QtCore.QSize(219, 117))
        NameDialog.setMaximumSize(QtCore.QSize(219, 117))
        self.layoutWidget = QtGui.QWidget(NameDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(10, 5, 199, 100))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.namelabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.namelabel.sizePolicy().hasHeightForWidth())
        self.namelabel.setSizePolicy(sizePolicy)
        self.namelabel.setObjectName(_fromUtf8("namelabel"))
        self.verticalLayout.addWidget(self.namelabel)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.id = QtGui.QLineEdit(self.layoutWidget)
        self.id.setEnabled(True)
        self.id.setReadOnly(False)
        self.id.setObjectName(_fromUtf8("id"))
        self.horizontalLayout_2.addWidget(self.id)
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

        self.retranslateUi(NameDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), NameDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), NameDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(NameDialog)

    def retranslateUi(self, NameDialog):
        NameDialog.setWindowTitle(_translate("NameDialog", "Dialog", None))
        self.namelabel.setText(_translate("NameDialog", "Name:", None))
        self.OK.setText(_translate("NameDialog", "OK", None))
        self.Cancel.setText(_translate("NameDialog", "Cancel", None))

