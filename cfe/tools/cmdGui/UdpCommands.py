#!/usr/bin/env python
# 
# UdpCommands.py -- This is a class that creates a simple command dialog and 
#                   sends commands using the cmdUtil UDP C program. 
#     
#                   The commands that can be sent are defined in comma delimited text files.
#                   This class deals with one set of commands in a file ( up to 20 ) 
#                   but multiple subsystems can be represented by spawning this class
#                   multiple times.
#
#                   This class could be duplicated to send over another interface
#                   such as TCP, Cubesat Space Protocol, or Xbee wireless radio
#

import sys
import csv
import getopt 
import subprocess 
import shlex

from PyQt4 import QtGui
from GenericCommandDialog import Ui_GenericCommandDialog
from struct import *

class SubsystemCommands(QtGui.QDialog):

    pktCount = 0

    #
    # Init the class
    #
    def __init__(self):
        QtGui.QDialog.__init__(self)
        self.ui = Ui_GenericCommandDialog()
        self.ui.setupUi(self)
        self.setWindowTitle(pageTitle)

        self.ui.SendButton_1.clicked.connect(self.ProcessSendButton_1)
        self.ui.SendButton_2.clicked.connect(self.ProcessSendButton_2)
        self.ui.SendButton_3.clicked.connect(self.ProcessSendButton_3)
        self.ui.SendButton_4.clicked.connect(self.ProcessSendButton_4)
        self.ui.SendButton_5.clicked.connect(self.ProcessSendButton_5)
        self.ui.SendButton_6.clicked.connect(self.ProcessSendButton_6)
        self.ui.SendButton_7.clicked.connect(self.ProcessSendButton_7)
        self.ui.SendButton_8.clicked.connect(self.ProcessSendButton_8)
        self.ui.SendButton_9.clicked.connect(self.ProcessSendButton_9)
        self.ui.SendButton_10.clicked.connect(self.ProcessSendButton_10)
        self.ui.SendButton_11.clicked.connect(self.ProcessSendButton_11)
        self.ui.SendButton_12.clicked.connect(self.ProcessSendButton_12)
        self.ui.SendButton_13.clicked.connect(self.ProcessSendButton_13)
        self.ui.SendButton_14.clicked.connect(self.ProcessSendButton_14)
        self.ui.SendButton_15.clicked.connect(self.ProcessSendButton_15)
        self.ui.SendButton_16.clicked.connect(self.ProcessSendButton_16)
        self.ui.SendButton_17.clicked.connect(self.ProcessSendButton_17)
        self.ui.SendButton_18.clicked.connect(self.ProcessSendButton_18)
        self.ui.SendButton_19.clicked.connect(self.ProcessSendButton_19)
        self.ui.SendButton_20.clicked.connect(self.ProcessSendButton_20)

        self.ui.paramsButton_1.clicked.connect(self.ProcessParamsButton_1)
        self.ui.paramsButton_2.clicked.connect(self.ProcessParamsButton_2)
        self.ui.paramsButton_3.clicked.connect(self.ProcessParamsButton_3)
        self.ui.paramsButton_4.clicked.connect(self.ProcessParamsButton_4)
        self.ui.paramsButton_5.clicked.connect(self.ProcessParamsButton_5)
        self.ui.paramsButton_6.clicked.connect(self.ProcessParamsButton_6)
        self.ui.paramsButton_7.clicked.connect(self.ProcessParamsButton_7)
        self.ui.paramsButton_8.clicked.connect(self.ProcessParamsButton_8)
        self.ui.paramsButton_9.clicked.connect(self.ProcessParamsButton_9)
        self.ui.paramsButton_10.clicked.connect(self.ProcessParamsButton_10)
        self.ui.paramsButton_11.clicked.connect(self.ProcessParamsButton_11)
        self.ui.paramsButton_12.clicked.connect(self.ProcessParamsButton_12)
        self.ui.paramsButton_13.clicked.connect(self.ProcessParamsButton_13)
        self.ui.paramsButton_14.clicked.connect(self.ProcessParamsButton_14)
        self.ui.paramsButton_15.clicked.connect(self.ProcessParamsButton_15)
        self.ui.paramsButton_16.clicked.connect(self.ProcessParamsButton_16)
        self.ui.paramsButton_17.clicked.connect(self.ProcessParamsButton_17)
        self.ui.paramsButton_18.clicked.connect(self.ProcessParamsButton_18)
        self.ui.paramsButton_19.clicked.connect(self.ProcessParamsButton_19)
        self.ui.paramsButton_20.clicked.connect(self.ProcessParamsButton_20)
    #
    # Button press methods
    #  
    def ProcessSendButton_1(self):
	self.ProcessSendButtonGeneric(0)
    def ProcessSendButton_2(self):
	self.ProcessSendButtonGeneric(1)
    def ProcessSendButton_3(self):
	self.ProcessSendButtonGeneric(2)
    def ProcessSendButton_4(self):
	self.ProcessSendButtonGeneric(3)
    def ProcessSendButton_5(self):
	self.ProcessSendButtonGeneric(4)
    def ProcessSendButton_6(self):
	self.ProcessSendButtonGeneric(5)
    def ProcessSendButton_7(self):
	self.ProcessSendButtonGeneric(6)
    def ProcessSendButton_8(self):
	self.ProcessSendButtonGeneric(7)
    def ProcessSendButton_9(self):
	self.ProcessSendButtonGeneric(8)
    def ProcessSendButton_10(self):
	self.ProcessSendButtonGeneric(9)
    def ProcessSendButton_11(self):
	self.ProcessSendButtonGeneric(10)
    def ProcessSendButton_12(self):
	self.ProcessSendButtonGeneric(11)
    def ProcessSendButton_13(self):
	self.ProcessSendButtonGeneric(12)
    def ProcessSendButton_14(self):
	self.ProcessSendButtonGeneric(13)
    def ProcessSendButton_15(self):
	self.ProcessSendButtonGeneric(14)
    def ProcessSendButton_16(self):
	self.ProcessSendButtonGeneric(15)
    def ProcessSendButton_17(self):
	self.ProcessSendButtonGeneric(16)
    def ProcessSendButton_18(self):
	self.ProcessSendButtonGeneric(17)
    def ProcessSendButton_19(self):
	self.ProcessSendButtonGeneric(18)
    def ProcessSendButton_20(self):
	self.ProcessSendButtonGeneric(19)

    #
    # Generic button press method
    def ProcessSendButtonGeneric(self, idx):
        if cmdItemIsValid[idx] == True:
           launch_string = '../cmdUtil/cmdUtil ' + ' --host=' + pageAddress + ' --port=' + pagePort + ' --pktid=' + pagePktId + ' --endian=' + pageEndian + ' --cmdcode=' + cmdCmdCode[idx]
           if cmdArg1[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg1[idx]
           if cmdArg2[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg2[idx]
           if cmdArg3[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg3[idx]
           if cmdArg4[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg4[idx]
           if cmdArg5[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg5[idx]
           if cmdArg6[idx] != "NULL":
              launch_string = launch_string + ' ' + cmdArg6[idx]
           cmd_args = shlex.split(launch_string)
	   # print launch_string
           subprocess.Popen(cmd_args)

    # 
    # Parameter button wrappers
    #
    def ProcessParamsButton_1(self):
	self.ProcessParameterButtonGeneric(0)
    def ProcessParamsButton_2(self):
	self.ProcessParameterButtonGeneric(1)
    def ProcessParamsButton_3(self):
	self.ProcessParameterButtonGeneric(2)
    def ProcessParamsButton_4(self):
	self.ProcessParameterButtonGeneric(3)
    def ProcessParamsButton_5(self):
	self.ProcessParameterButtonGeneric(4)
    def ProcessParamsButton_6(self):
	self.ProcessParameterButtonGeneric(5)
    def ProcessParamsButton_7(self):
	self.ProcessParameterButtonGeneric(6)
    def ProcessParamsButton_8(self):
	self.ProcessParameterButtonGeneric(7)
    def ProcessParamsButton_9(self):
	self.ProcessParameterButtonGeneric(8)
    def ProcessParamsButton_10(self):
	self.ProcessParameterButtonGeneric(9)
    def ProcessParamsButton_11(self):
	self.ProcessParameterButtonGeneric(10)
    def ProcessParamsButton_12(self):
	self.ProcessParameterButtonGeneric(11)
    def ProcessParamsButton_13(self):
	self.ProcessParameterButtonGeneric(12)
    def ProcessParamsButton_14(self):
	self.ProcessParameterButtonGeneric(13)
    def ProcessParamsButton_15(self):
	self.ProcessParameterButtonGeneric(14)
    def ProcessParamsButton_16(self):
	self.ProcessParameterButtonGeneric(15)
    def ProcessParamsButton_17(self):
	self.ProcessParameterButtonGeneric(16)
    def ProcessParamsButton_18(self):
	self.ProcessParameterButtonGeneric(17)
    def ProcessParamsButton_19(self):
	self.ProcessParameterButtonGeneric(18)
    def ProcessParamsButton_20(self):
	self.ProcessParameterButtonGeneric(19)

    # 
    # Parameter dialog button press method
    #
    def ProcessParameterButtonGeneric(self,idx):
       parameter_string = cmdArg1[idx] + '\n' + cmdArg2[idx] + '\n' + cmdArg3[idx] + '\n'
       parameter_string += cmdArg4[idx] + '\n' + cmdArg5[idx] + '\n' + cmdArg6[idx]
       QtGui.QMessageBox.information(self,"Parameters", parameter_string)

#
# Display usage
#
def usage():
    print "Must specify --title=<page name> --file=<cmd_def_file> --pktid=<packet_app_id(hex)> --endian=<LE|BE> --address=<IP address> --port=<UDP port>" 
    print "     example: --title=\"Executive Services\" --file=cfe-es-cmds.txt --pktid=1806  --endian=LE --address=127.0.0.1 --port=1234" 

#
# Main 
#
if __name__ == '__main__':

    #
    # Set defaults for the arguments
    #
    pageTitle = "Command Page"               
    pagePort  = 1234
    pageAddress = "127.0.0.1"
    pagePktId = 1801 
    pageEndian = "LE"
    pageDefFile = "command-def.txt"

    #
    # process cmd line args 
    #
    try:                                
        opts, args = getopt.getopt(sys.argv[1:], "htpfeap", ["help", "title=", "pktid=", "file=", "endian=", "address=", "port="]) 
    except getopt.GetoptError:           
        usage()                          
        sys.exit(2)                     

    for opt, arg in opts:                
        if opt in ("-h", "--help"):      
            usage()                     
            sys.exit()                  
        elif opt in ("-t", "--title"): 
            pageTitle = arg
        elif opt in ("-f", "--file"): 
            pageDefFile = arg
        elif opt in ("-p", "--pktid"): 
            pagePktId = arg                   
        elif opt in ("-e", "--endian"): 
            pageEndian = arg                          
        elif opt in ("-a", "--address"): 
            pageAddress = arg               
        elif opt in ("-p", "--port"): 
            pagePort = arg               

    #
    # Init the QT application and the command class
    #
    app = QtGui.QApplication(sys.argv)
    Commands = SubsystemCommands()
    Commands.ui.subSystemTextBrowser.setText(pageTitle)
    Commands.ui.packetId.display(pagePktId)
    Commands.ui.commandAddressTextBrowser.setText(pageAddress)

    #
    # Read in the contents of the command defintion file
    #
    cmdItemIsValid = []
    cmdDesc = []
    cmdCmdCode = []
    cmdArg1 = []
    cmdArg2 = []
    cmdArg3 = []
    cmdArg4 = []
    cmdArg5 = []
    cmdArg6 = []
    i = 0

    with open(pageDefFile, 'rb') as cmdfile:
       reader = csv.reader(cmdfile, skipinitialspace = True)
       for row in reader:
          if row[0][0] != '#': 
             cmdItemIsValid.append(True)
             cmdDesc.append(row[0])
             cmdCmdCode.append(row[1])
             cmdArg1.append(row[2])
             cmdArg2.append(row[3])
             cmdArg3.append(row[4])
             cmdArg4.append(row[5])
             cmdArg5.append(row[6])
             cmdArg6.append(row[7])
             i = i + 1
    # 
    # Mark the remaining values ad invalid
    #
    for j in range (i, 21):
       cmdItemIsValid.append(False)

    #
    # fill the data fields on the page
    #
    if cmdItemIsValid[0] == True:
       Commands.ui.itemLabelTextBrowser_1.setText(cmdDesc[0])
    else:
       Commands.ui.itemLabelTextBrowser_1.setText("(unused)")

    if cmdItemIsValid[1] == True:
       Commands.ui.itemLabelTextBrowser_2.setText(cmdDesc[1])
    else:
       Commands.ui.itemLabelTextBrowser_2.setText("(unused)")

    if cmdItemIsValid[2] == True:
       Commands.ui.itemLabelTextBrowser_3.setText(cmdDesc[2])
    else:
       Commands.ui.itemLabelTextBrowser_3.setText("(unused)")

    if cmdItemIsValid[3] == True:
       Commands.ui.itemLabelTextBrowser_4.setText(cmdDesc[3])
    else:
       Commands.ui.itemLabelTextBrowser_4.setText("(unused)")

    if cmdItemIsValid[4] == True:
       Commands.ui.itemLabelTextBrowser_5.setText(cmdDesc[4])
    else:
       Commands.ui.itemLabelTextBrowser_5.setText("(unused)")

    if cmdItemIsValid[5] == True:
       Commands.ui.itemLabelTextBrowser_6.setText(cmdDesc[5])
    else:
       Commands.ui.itemLabelTextBrowser_6.setText("(unused)")

    if cmdItemIsValid[6] == True:
       Commands.ui.itemLabelTextBrowser_7.setText(cmdDesc[6])
    else:
       Commands.ui.itemLabelTextBrowser_7.setText("(unused)")

    if cmdItemIsValid[7] == True:
       Commands.ui.itemLabelTextBrowser_8.setText(cmdDesc[7])
    else:
       Commands.ui.itemLabelTextBrowser_8.setText("(unused)")

    if cmdItemIsValid[8] == True:
       Commands.ui.itemLabelTextBrowser_9.setText(cmdDesc[8])
    else:
       Commands.ui.itemLabelTextBrowser_9.setText("(unused)")

    if cmdItemIsValid[9] == True:
       Commands.ui.itemLabelTextBrowser_10.setText(cmdDesc[9])
    else:
       Commands.ui.itemLabelTextBrowser_10.setText("(unused)")

    if cmdItemIsValid[10] == True:
       Commands.ui.itemLabelTextBrowser_11.setText(cmdDesc[10])
    else:
       Commands.ui.itemLabelTextBrowser_11.setText("(unused)")

    if cmdItemIsValid[11] == True:
       Commands.ui.itemLabelTextBrowser_12.setText(cmdDesc[11])
    else:
       Commands.ui.itemLabelTextBrowser_12.setText("(unused)")

    if cmdItemIsValid[12] == True:
       Commands.ui.itemLabelTextBrowser_13.setText(cmdDesc[12])
    else:
       Commands.ui.itemLabelTextBrowser_13.setText("(unused)")

    if cmdItemIsValid[13] == True:
       Commands.ui.itemLabelTextBrowser_14.setText(cmdDesc[13])
    else:
       Commands.ui.itemLabelTextBrowser_14.setText("(unused)")

    if cmdItemIsValid[14] == True:
       Commands.ui.itemLabelTextBrowser_15.setText(cmdDesc[14])
    else:
       Commands.ui.itemLabelTextBrowser_15.setText("(unused)")

    if cmdItemIsValid[15] == True:
       Commands.ui.itemLabelTextBrowser_16.setText(cmdDesc[15])
    else:
       Commands.ui.itemLabelTextBrowser_16.setText("(unused)")

    if cmdItemIsValid[16] == True:
       Commands.ui.itemLabelTextBrowser_17.setText(cmdDesc[16])
    else:
       Commands.ui.itemLabelTextBrowser_17.setText("(unused)")

    if cmdItemIsValid[17] == True:
       Commands.ui.itemLabelTextBrowser_18.setText(cmdDesc[17])
    else:
       Commands.ui.itemLabelTextBrowser_18.setText("(unused)")

    if cmdItemIsValid[18] == True:
       Commands.ui.itemLabelTextBrowser_19.setText(cmdDesc[18])
    else:
       Commands.ui.itemLabelTextBrowser_19.setText("(unused)")

    if cmdItemIsValid[19] == True:
       Commands.ui.itemLabelTextBrowser_20.setText(cmdDesc[19])
    else:
       Commands.ui.itemLabelTextBrowser_20.setText("(unused)")

    # 
    # Display the page 
    #
    Commands.show()
    sys.exit(app.exec_())

