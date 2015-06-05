Command Gui

This is a simple Python / QT4 based Command GUI for the cmdUtil utility. 
It provides a list of "command pages" with a list of commands to send to a subsystem.

The program is written in Python 2.x with the PyQT4 GUI. The dialogs were created
in the QT4 designer program, and converted to python classes using the "pyuic4" 
utility:
    pyuic4 -o MyDialog.py MyDialog.ui

The list of pages along with the AppID and address for each page is defined in the
file "command-pages.txt"

Each command page has a list of subsystem commands in the text files. For example, 
ES is in cfe-es-cmds.txt .

This first release of the program does not allow editing of the command parameters 
in the GUI. This would be really helpful for on-the-fly commanding ( when you 
want to do something like delete an app, and specify the app name in a dialog )

There is the possiblity to implement multiple command interfaces using
the python class parameter that is passed in. For example, a copy of
UdpCommands.py could be made, which would allow the commands to be sent
over an Xbee RF radio, or TCP connection.. 

The subsystem commands ( cfe-es-cmds.txt ) are interface agnostic, so
they can be resued.

 
