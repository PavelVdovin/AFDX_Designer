from PyQt4 import QtGui
import sys

from AFDX_GUI.MainWindow import MainWindow

if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())

