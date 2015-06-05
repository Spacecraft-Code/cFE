###############################################################################
# File: link-rules.mak
#
# Purpose:
#   Makefile for linking code and producing the cFE Core executable image.
#
# History:
#
###############################################################################
##
## Executable target. This is target specific
##
EXE_TARGET=core-cygwin.bin
SHARED_LIB=cfe.dll

CORE_INSTALL_FILES = $(EXE_TARGET) $(SHARED_LIB) 


##
## Linker flags that are needed
##
LDFLAGS = 

##
## Libraries to link in
##
LIBS+=-lc
LIBS+=-lm
LIBS+=-lstdc++
LIBS+=-lpthread

##
## cFE Core Link Rule
## 
$(EXE_TARGET): $(CORE_OBJS)
	$(COMPILER) -shared -o $(SHARED_LIB) $(CORE_OBJS)
	$(COMPILER) -g -o $(EXE_TARGET) ../bsp/bsp_start.o $(SHARED_LIB) $(LDFLAGS) $(LIBS)
	
##
## Application Link Rule
## Cygwin needs to link in the cfe.dll shared lib.
##
$(APPTARGET).$(APP_EXT): $(OBJS)
	$(COMPILER) -shared -o $@ $(OBJS) $(SHARED_LIB_LINK) $(EXEDIR)/cfe.$(APP_EXT) -lc  
