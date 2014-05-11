# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file './BagDialog.ui'
#
# Created: Sun May 11 00:00:46 2014
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

class Ui_BagDialog(object):
    def setupUi(self, BagDialog):
        BagDialog.setObjectName(_fromUtf8("BagDialog"))
        BagDialog.resize(255, 112)
        BagDialog.setMinimumSize(QtCore.QSize(255, 112))
        BagDialog.setMaximumSize(QtCore.QSize(255, 112))
        self.layoutWidget = QtGui.QWidget(BagDialog)
        self.layoutWidget.setGeometry(QtCore.QRect(0, 12, 251, 91))
        self.layoutWidget.setObjectName(_fromUtf8("layoutWidget"))
        self.verticalLayout = QtGui.QVBoxLayout(self.layoutWidget)
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setMargin(0)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.bagLabel = QtGui.QLabel(self.layoutWidget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.bagLabel.sizePolicy().hasHeightForWidth())
        self.bagLabel.setSizePolicy(sizePolicy)
        self.bagLabel.setObjectName(_fromUtf8("bagLabel"))
        self.verticalLayout.addWidget(self.bagLabel)
        self.bag = QtGui.QComboBox(self.layoutWidget)
        self.bag.setObjectName(_fromUtf8("bag"))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.bag.addItem(_fromUtf8(""))
        self.verticalLayout.addWidget(self.bag)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(self.layoutWidget)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(self.layoutWidget)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)

        self.retranslateUi(BagDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), BagDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), BagDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(BagDialog)

    def retranslateUi(self, BagDialog):
        BagDialog.setWindowTitle(_translate("BagDialog", "Dialog", None))
        self.bagLabel.setText(_translate("BagDialog", "BAG:", None))
        self.bag.setItemText(0, _translate("BagDialog", "1 ms", None))
        self.bag.setItemText(1, _translate("BagDialog", "2 ms", None))
        self.bag.setItemText(2, _translate("BagDialog", "4 ms", None))
        self.bag.setItemText(3, _translate("BagDialog", "8 ms", None))
        self.bag.setItemText(4, _translate("BagDialog", "16 ms", None))
        self.bag.setItemText(5, _translate("BagDialog", "32 ms", None))
        self.bag.setItemText(6, _translate("BagDialog", "64 ms", None))
        self.bag.setItemText(7, _translate("BagDialog", "128 ms", None))
        self.OK.setText(_translate("BagDialog", "OK", None))
        self.Cancel.setText(_translate("BagDialog", "Cancel", None))

