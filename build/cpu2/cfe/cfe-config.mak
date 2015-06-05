##
## cfe-config.mak
##
## This file specifies the OS, and PSP to build for the cFE.
##
## NOTE: Make sure each selection does not have any spaces
##       after it!
##
##---------------------------------------------------------
## Possible Settings
##
##  OS          PSP
##
##  posix       pc-linux            (fully supported)
##  rtems       mcf5235-rtems       (fully supported)
##  vxworks6    rad750-vxworks6.4   (fully supported)
##  vxworks6    mcp750-vxworks6.4   (fully supported)
##
## NOTE: Please check the debug-opts.mak file to ensure
##       that the options are correct for your settings
##---------------------------------------------------------

##---------------------------------------------------------
## Operating System
## OS = The operating system selected for the Abstraction implementation
##---------------------------------------------------------
OS = vxworks6

##---------------------------------------------------------
## PSP -- CFE Platform Support Package
##---------------------------------------------------------
PSP = mcp750-vxworks6.4
