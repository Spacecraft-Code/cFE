# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'EventMessageDialog.ui'
#
# Created: Sun Feb 23 07:13:23 2014
#      by: PyQt4 UI code generator 4.6.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_EventMessageDialog(object):
    def setupUi(self, EventMessageDialog):
        EventMessageDialog.setObjectName("EventMessageDialog")
        EventMessageDialog.resize(681, 640)
        self.buttonBox = QtGui.QDialogButtonBox(EventMessageDialog)
        self.buttonBox.setGeometry(QtCore.QRect(540, 10, 91, 32))
        self.buttonBox.setOrientation(QtCore.Qt.Horizontal)
        self.buttonBox.setStandardButtons(QtGui.QDialogButtonBox.Close)
        self.buttonBox.setObjectName("buttonBox")
        self.label = QtGui.QLabel(EventMessageDialog)
        self.label.setGeometry(QtCore.QRect(300, 10, 62, 17))
        self.label.setObjectName("label")
        self.sequenceCount = QtGui.QTextBrowser(EventMessageDialog)
        self.sequenceCount.setGeometry(QtCore.QRect(130, 10, 81, 31))
        self.sequenceCount.setObjectName("sequenceCount")
        self.label_2 = QtGui.QLabel(EventMessageDialog)
        self.label_2.setGeometry(QtCore.QRect(30, 20, 91, 17))
        self.label_2.setObjectName("label_2")
        self.eventOutput = QtGui.QTextBrowser(EventMessageDialog)
        self.eventOutput.setGeometry(QtCore.QRect(20, 60, 641, 561))
        self.eventOutput.setObjectName("eventOutput")

        self.retranslateUi(EventMessageDialog)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("accepted()"), EventMessageDialog.accept)
        QtCore.QObject.connect(self.buttonBox, QtCore.SIGNAL("rejected()"), EventMessageDialog.reject)
        QtCore.QMetaObject.connectSlotsByName(EventMessageDialog)

    def retranslateUi(self, EventMessageDialog):
        EventMessageDialog.setWindowTitle(QtGui.QApplication.translate("EventMessageDialog", "Event Messages", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("EventMessageDialog", "Events", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("EventMessageDialog", "Packet Count", None, QtGui.QApplication.UnicodeUTF8))

