# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'OptionsDialog.ui'
#
# Created: Fri Oct 31 11:22:30 2014
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

class Ui_OptionsDialog(object):
    def setupUi(self, OptionsDialog):
        OptionsDialog.setObjectName(_fromUtf8("OptionsDialog"))
        OptionsDialog.resize(457, 250)
        self.gridLayout = QtGui.QGridLayout(OptionsDialog)
        self.gridLayout.setObjectName(_fromUtf8("gridLayout"))
        self.verticalLayout = QtGui.QVBoxLayout()
        self.verticalLayout.setSizeConstraint(QtGui.QLayout.SetNoConstraint)
        self.verticalLayout.setObjectName(_fromUtf8("verticalLayout"))
        self.limitJitter = QtGui.QCheckBox(OptionsDialog)
        self.limitJitter.setObjectName(_fromUtf8("limitJitter"))
        self.verticalLayout.addWidget(self.limitJitter)
        self.allowMultipleLinks = QtGui.QCheckBox(OptionsDialog)
        self.allowMultipleLinks.setObjectName(_fromUtf8("allowMultipleLinks"))
        self.verticalLayout.addWidget(self.allowMultipleLinks)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.verticalLayout.addItem(spacerItem)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName(_fromUtf8("horizontalLayout_2"))
        self.esDelayEdit = QtGui.QLineEdit(OptionsDialog)
        self.esDelayEdit.setMinimumSize(QtCore.QSize(124, 0))
        self.esDelayEdit.setMaximumSize(QtCore.QSize(124, 28))
        self.esDelayEdit.setObjectName(_fromUtf8("esDelayEdit"))
        self.horizontalLayout_2.addWidget(self.esDelayEdit)
        self.label = QtGui.QLabel(OptionsDialog)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(5)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setObjectName(_fromUtf8("label"))
        self.horizontalLayout_2.addWidget(self.label)
        self.verticalLayout.addLayout(self.horizontalLayout_2)
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName(_fromUtf8("horizontalLayout_3"))
        self.switchDelayEdit = QtGui.QLineEdit(OptionsDialog)
        self.switchDelayEdit.setMinimumSize(QtCore.QSize(124, 0))
        self.switchDelayEdit.setMaximumSize(QtCore.QSize(124, 16777215))
        self.switchDelayEdit.setObjectName(_fromUtf8("switchDelayEdit"))
        self.horizontalLayout_3.addWidget(self.switchDelayEdit)
        self.label_2 = QtGui.QLabel(OptionsDialog)
        self.label_2.setObjectName(_fromUtf8("label_2"))
        self.horizontalLayout_3.addWidget(self.label_2)
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName(_fromUtf8("horizontalLayout_4"))
        self.ifgEdit = QtGui.QLineEdit(OptionsDialog)
        self.ifgEdit.setMinimumSize(QtCore.QSize(124, 0))
        self.ifgEdit.setMaximumSize(QtCore.QSize(124, 16777215))
        self.ifgEdit.setObjectName(_fromUtf8("ifgEdit"))
        self.horizontalLayout_4.addWidget(self.ifgEdit)
        self.label_3 = QtGui.QLabel(OptionsDialog)
        self.label_3.setObjectName(_fromUtf8("label_3"))
        self.horizontalLayout_4.addWidget(self.label_3)
        self.verticalLayout.addLayout(self.horizontalLayout_4)
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.OK = QtGui.QPushButton(OptionsDialog)
        self.OK.setObjectName(_fromUtf8("OK"))
        self.horizontalLayout.addWidget(self.OK)
        self.Cancel = QtGui.QPushButton(OptionsDialog)
        self.Cancel.setObjectName(_fromUtf8("Cancel"))
        self.horizontalLayout.addWidget(self.Cancel)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.gridLayout.addLayout(self.verticalLayout, 0, 0, 1, 1)

        self.retranslateUi(OptionsDialog)
        QtCore.QObject.connect(self.OK, QtCore.SIGNAL(_fromUtf8("clicked()")), OptionsDialog.accept)
        QtCore.QObject.connect(self.Cancel, QtCore.SIGNAL(_fromUtf8("clicked()")), OptionsDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(OptionsDialog)

    def retranslateUi(self, OptionsDialog):
        OptionsDialog.setWindowTitle(_translate("OptionsDialog", "Dialog", None))
        self.limitJitter.setText(_translate("OptionsDialog", "Limit jitter to 500 microseconds", None))
        self.allowMultipleLinks.setText(_translate("OptionsDialog", "Allow multiple links from end systems", None))
        self.label.setText(_translate("OptionsDialog", "End-system delay (microseconds)", None))
        self.label_2.setText(_translate("OptionsDialog", "Switch delay (microseconds)", None))
        self.label_3.setText(_translate("OptionsDialog", "Iterframe gap (microseconds)", None))
        self.OK.setText(_translate("OptionsDialog", "OK", None))
        self.Cancel.setText(_translate("OptionsDialog", "Cancel", None))

