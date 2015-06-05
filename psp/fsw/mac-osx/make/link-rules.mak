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
EXE_TARGET=core-osx.bin

CORE_INSTALL_FILES = $(EXE_TARGET)

##
## Linker flags that are needed
##
LDFLAGS = 

##
## Libraries to link in
##
LIBS = 

##
## cFE Core Link Rule
## 
$(EXE_TARGET): $(CORE_OBJS)
	$(COMPILER) $(DEBUG_FLAGS) -o $(EXE_TARGET) $(CORE_OBJS) $(LDFLAGS) $(LIBS)
	
##
## Application Link Rule
##
$(APPTARGET).$(APP_EXT): $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET=10.3
	$(COMPILER) -bundle -o $@ $(OBJS) -undefined dynamic_lookup

