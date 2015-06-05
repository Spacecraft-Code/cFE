#!/usr/bin/env python
# 
import sys
import csv
import subprocess 
import shlex

from PyQt4 import QtGui, QtNetwork
from CommandSystemDialog import Ui_CommandSystemDialog
from struct import *


class CommandSystem(QtGui.QDialog):

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_CommandSystemDialog()
        self.ui.setupUi(self)
        self.setWindowTitle('Command System Main Page')

        self.ui.pushButton_0.clicked.connect(self.ProcessButton_0)
        self.ui.pushButton_1.clicked.connect(self.ProcessButton_1)
        self.ui.pushButton_2.clicked.connect(self.ProcessButton_2)
        self.ui.pushButton_3.clicked.connect(self.ProcessButton_3)
        self.ui.pushButton_4.clicked.connect(self.ProcessButton_4)
        self.ui.pushButton_5.clicked.connect(self.ProcessButton_5)
        self.ui.pushButton_6.clicked.connect(self.ProcessButton_6)
        self.ui.pushButton_7.clicked.connect(self.ProcessButton_7)
        self.ui.pushButton_8.clicked.connect(self.ProcessButton_8)
        self.ui.pushButton_9.clicked.connect(self.ProcessButton_9)
        self.ui.pushButton_10.clicked.connect(self.ProcessButton_10)
        self.ui.pushButton_11.clicked.connect(self.ProcessButton_11)
        self.ui.pushButton_12.clicked.connect(self.ProcessButton_12)
        self.ui.pushButton_13.clicked.connect(self.ProcessButton_13)
        self.ui.pushButton_14.clicked.connect(self.ProcessButton_14)
        self.ui.pushButton_15.clicked.connect(self.ProcessButton_15)
        self.ui.pushButton_16.clicked.connect(self.ProcessButton_16)
        self.ui.pushButton_17.clicked.connect(self.ProcessButton_17)
        self.ui.pushButton_18.clicked.connect(self.ProcessButton_18)
        self.ui.pushButton_19.clicked.connect(self.ProcessButton_19)
        self.ui.pushButton_20.clicked.connect(self.ProcessButton_20)

    #
    # Button press methods
    #  
    def ProcessButton_0(self):
        self.ProcessButtonGeneric(0)
    def ProcessButton_1(self):
        self.ProcessButtonGeneric(1)
    def ProcessButton_2(self):
        self.ProcessButtonGeneric(2)
    def ProcessButton_3(self):
        self.ProcessButtonGeneric(3)
    def ProcessButton_4(self):
        self.ProcessButtonGeneric(4)
    def ProcessButton_5(self):
        self.ProcessButtonGeneric(5)
    def ProcessButton_6(self):
        self.ProcessButtonGeneric(6)
    def ProcessButton_7(self):
        self.ProcessButtonGeneric(7)
    def ProcessButton_8(self):
        self.ProcessButtonGeneric(8)
    def ProcessButton_9(self):
        self.ProcessButtonGeneric(9)
    def ProcessButton_10(self):
        self.ProcessButtonGeneric(10)
    def ProcessButton_11(self):
        self.ProcessButtonGeneric(11)
    def ProcessButton_12(self):
        self.ProcessButtonGeneric(12)
    def ProcessButton_13(self):
        self.ProcessButtonGeneric(13)
    def ProcessButton_14(self):
        self.ProcessButtonGeneric(14)
    def ProcessButton_15(self):
        self.ProcessButtonGeneric(15)
    def ProcessButton_16(self):
        self.ProcessButtonGeneric(16)
    def ProcessButton_17(self):
        self.ProcessButtonGeneric(17)
    def ProcessButton_18(self):
        self.ProcessButtonGeneric(18)
    def ProcessButton_19(self):
        self.ProcessButtonGeneric(19)
    def ProcessButton_20(self):
        self.ProcessButtonGeneric(20)


    def ProcessButtonGeneric(self, idx):
        if cmdPageIsValid[idx] == True:
           # need to extract data from fields, then launch the page with the right params
           launch_string = 'python ' + cmdClass[0] + ' --title=\"' + cmdPageDesc[idx] + '\" --pktid=' + hex(cmdPageAppid[idx]) + ' --file=' + cmdPageDefFile[idx] + ' --address=\"' + cmdPageAddress[idx] + '\"' + ' --port=' + str(cmdPagePort[idx]) + ' --endian=' + cmdPageEndian[idx] 
           cmd_args = shlex.split(launch_string)
	   # print launch_string
           subprocess.Popen(cmd_args)


#
# Main 
#
if __name__ == '__main__':

    #
    # Set defaults for the arguments
    #
    cmdDefFile = "command-pages.txt"

    #
    # Init the QT application and the telemetry dialog class
    #
    app = QtGui.QApplication(sys.argv)
    Command = CommandSystem()

    #
    # Read in the contents of the telemetry packet defintion
    #
    cmdPageIsValid = []
    cmdPageDesc = []
    cmdPageDefFile = []
    cmdPageAppid = []
    cmdPageEndian = []
    cmdClass = []
    cmdPageAddress = []
    cmdPagePort = []

    i = 0

    with open(cmdDefFile, 'rb') as cmdfile:
       reader = csv.reader(cmdfile, skipinitialspace = True)
       for cmdRow in reader:
          if cmdRow[0][0] != '#':
             cmdPageIsValid.append(True)
             cmdPageDesc.append(cmdRow[0])
             cmdPageDefFile.append(cmdRow[1])
             cmdPageAppid.append(int(cmdRow[2],16))
             cmdPageEndian.append(cmdRow[3])
             cmdClass.append(cmdRow[4])
             cmdPageAddress.append(cmdRow[5])
             cmdPagePort.append(int(cmdRow[6]))
             i = i + 1

    # 
    # Mark the remaining values as invalid
    #
    for j in range (i, 22):
       cmdPageAppid.append(0)
       cmdPageIsValid.append(False)

    #
    # fill the data fields on the page
    #
    if cmdPageIsValid[0] == True:
       Command.ui.SubsysBrowser_0.setText(cmdPageDesc[0])
       Command.ui.pktidBrowser_0.setText(hex(cmdPageAppid[0]))
       Command.ui.addressBrowser_0.setText(cmdPageAddress[0])
    else:
       Command.ui.SubsysBrowser_0.setText("(unused)")

    if cmdPageIsValid[1] == True:
       Command.ui.SubsysBrowser_1.setText(cmdPageDesc[1])
       Command.ui.pktidBrowser_1.setText(hex(cmdPageAppid[1]))
       Command.ui.addressBrowser_1.setText(cmdPageAddress[1])
    else:
       Command.ui.SubsysBrowser_1.setText("(unused)")

    if cmdPageIsValid[2] == True:
       Command.ui.SubsysBrowser_2.setText(cmdPageDesc[2])
       Command.ui.pktidBrowser_2.setText(hex(cmdPageAppid[2]))
       Command.ui.addressBrowser_2.setText(cmdPageAddress[2])
    else:
       Command.ui.SubsysBrowser_2.setText("(unused)")

    if cmdPageIsValid[3] == True:
       Command.ui.SubsysBrowser_3.setText(cmdPageDesc[3])
       Command.ui.pktidBrowser_3.setText(hex(cmdPageAppid[3]))
       Command.ui.addressBrowser_3.setText(cmdPageAddress[3])
    else:
       Command.ui.SubsysBrowser_3.setText("(unused)")

    if cmdPageIsValid[4] == True:
       Command.ui.SubsysBrowser_4.setText(cmdPageDesc[4])
       Command.ui.pktidBrowser_4.setText(hex(cmdPageAppid[4]))
       Command.ui.addressBrowser_4.setText(cmdPageAddress[4])
    else:
       Command.ui.SubsysBrowser_4.setText("(unused)")

    if cmdPageIsValid[5] == True:
       Command.ui.SubsysBrowser_5.setText(cmdPageDesc[5])
       Command.ui.pktidBrowser_5.setText(hex(cmdPageAppid[5]))
       Command.ui.addressBrowser_5.setText(cmdPageAddress[5])
    else:
       Command.ui.SubsysBrowser_5.setText("(unused)")

    if cmdPageIsValid[6] == True:
       Command.ui.SubsysBrowser_6.setText(cmdPageDesc[6])
       Command.ui.pktidBrowser_6.setText(hex(cmdPageAppid[6]))
       Command.ui.addressBrowser_6.setText(cmdPageAddress[6])
    else:
       Command.ui.SubsysBrowser_6.setText("(unused)")

    if cmdPageIsValid[7] == True:
       Command.ui.SubsysBrowser_7.setText(cmdPageDesc[7])
       Command.ui.pktidBrowser_7.setText(hex(cmdPageAppid[7]))
       Command.ui.addressBrowser_7.setText(cmdPageAddress[7])
    else:
       Command.ui.SubsysBrowser_7.setText("(unused)")

    if cmdPageIsValid[8] == True:
       Command.ui.SubsysBrowser_8.setText(cmdPageDesc[8])
       Command.ui.pktidBrowser_8.setText(hex(cmdPageAppid[8]))
       Command.ui.addressBrowser_8.setText(cmdPageAddress[8])
    else:
       Command.ui.SubsysBrowser_8.setText("(unused)")

    if cmdPageIsValid[9] == True:
       Command.ui.SubsysBrowser_9.setText(cmdPageDesc[9])
       Command.ui.pktidBrowser_9.setText(hex(cmdPageAppid[9]))
       Command.ui.addressBrowser_9.setText(cmdPageAddress[9])
    else:
       Command.ui.SubsysBrowser_9.setText("(unused)")

    if cmdPageIsValid[10] == True:
       Command.ui.SubsysBrowser_10.setText(cmdPageDesc[10])
       Command.ui.pktidBrowser_10.setText(hex(cmdPageAppid[10]))
       Command.ui.addressBrowser_10.setText(cmdPageAddress[10])
    else:
       Command.ui.SubsysBrowser_10.setText("(unused)")

    if cmdPageIsValid[11] == True:
       Command.ui.SubsysBrowser_11.setText(cmdPageDesc[11])
       Command.ui.pktidBrowser_11.setText(hex(cmdPageAppid[11]))
       Command.ui.addressBrowser_11.setText(cmdPageAddress[11])
    else:
       Command.ui.SubsysBrowser_11.setText("(unused)")

    if cmdPageIsValid[12] == True:
       Command.ui.SubsysBrowser_12.setText(cmdPageDesc[12])
       Command.ui.pktidBrowser_12.setText(hex(cmdPageAppid[12]))
       Command.ui.addressBrowser_12.setText(cmdPageAddress[12])
    else:
       Command.ui.SubsysBrowser_12.setText("(unused)")

    if cmdPageIsValid[13] == True:
       Command.ui.SubsysBrowser_13.setText(cmdPageDesc[13])
       Command.ui.pktidBrowser_13.setText(hex(cmdPageAppid[13]))
       Command.ui.addressBrowser_13.setText(cmdPageAddress[13])
    else:
       Command.ui.SubsysBrowser_13.setText("(unused)")

    if cmdPageIsValid[14] == True:
       Command.ui.SubsysBrowser_14.setText(cmdPageDesc[14])
       Command.ui.pktidBrowser_14.setText(hex(cmdPageAppid[14]))
       Command.ui.addressBrowser_14.setText(cmdPageAddress[14])
    else:
       Command.ui.SubsysBrowser_14.setText("(unused)")

    if cmdPageIsValid[15] == True:
       Command.ui.SubsysBrowser_15.setText(cmdPageDesc[15])
       Command.ui.pktidBrowser_15.setText(hex(cmdPageAppid[15]))
       Command.ui.addressBrowser_15.setText(cmdPageAddress[15])
    else:
       Command.ui.SubsysBrowser_15.setText("(unused)")

    if cmdPageIsValid[16] == True:
       Command.ui.SubsysBrowser_16.setText(cmdPageDesc[16])
       Command.ui.pktidBrowser_16.setText(hex(cmdPageAppid[16]))
       Command.ui.addressBrowser_16.setText(cmdPageAddress[16])
    else:
       Command.ui.SubsysBrowser_16.setText("(unused)")

    if cmdPageIsValid[17] == True:
       Command.ui.SubsysBrowser_17.setText(cmdPageDesc[17])
       Command.ui.pktidBrowser_17.setText(hex(cmdPageAppid[17]))
       Command.ui.addressBrowser_17.setText(cmdPageAddress[17])
    else:
       Command.ui.SubsysBrowser_17.setText("(unused)")

    if cmdPageIsValid[18] == True:
       Command.ui.SubsysBrowser_18.setText(cmdPageDesc[18])
       Command.ui.pktidBrowser_18.setText(hex(cmdPageAppid[18]))
       Command.ui.addressBrowser_18.setText(cmdPageAddress[18])
    else:
       Command.ui.SubsysBrowser_18.setText("(unused)")

    if cmdPageIsValid[19] == True:
       Command.ui.SubsysBrowser_19.setText(cmdPageDesc[19])
       Command.ui.pktidBrowser_19.setText(hex(cmdPageAppid[19]))
       Command.ui.addressBrowser_19.setText(cmdPageAddress[19])
    else:
       Command.ui.SubsysBrowser_19.setText("(unused)")

    if cmdPageIsValid[20] == True:
       Command.ui.SubsysBrowser_20.setText(cmdPageDesc[20])
       Command.ui.pktidBrowser_20.setText(hex(cmdPageAppid[20]))
       Command.ui.addressBrowser_20.setText(cmdPageAddress[20])
    else:
       Command.ui.SubsysBrowser_20.setText("(unused)")

    if cmdPageIsValid[21] == True:
       Command.ui.SubsysBrowser_21.setText(cmdPageDesc[21])
       Command.ui.pktidBrowser_21.setText(hex(cmdPageAppid[21]))
       Command.ui.addressBrowser_21.setText(cmdPageAddress[21])
    else:
       Command.ui.SubsysBrowser_21.setText("(unused)")

    # 
    # Display the page 
    #
    Command.show()
    sys.exit(app.exec_())

