###############################################################################
## compiler-opts.mak - compiler definitions and options for building the cFE 
##
## Target: m5235BCC Coldfire board with RTEMS and CEXP dynamic loader
##
## Modifications:
##
###############################################################################
## 
## Warning Level Configuration
##
## WARNINGS=-Wall -ansi -pedantic -Wstrict-prototypes
WARNINGS	= -Wall 

SYSINCS = 
# SYSINCS = -I/opt/rtems-devkit/cexp/m68k-rtems4.9-cexp/include/

##
## Target Defines for the OS, Hardware Arch, etc..
##
TARGET_DEFS = -D_RTEMS_OS_ -D_m68k_ -DOS_HWARCH=$(HWARCH) $(CFE_SB_NET) -D$(OS) -DM5235BCC -D_EMBED_

## 
## Endian Defines
##
ENDIAN_DEFS=-D_EB -DENDIAN=_EB -DSOFTWARE_BIG_BIT_ORDER 

##
## Compiler Architecture Switches ( double check arch switch -m52xx, m523x etc.. )
## 
ARCH_OPTS = --pipe -fomit-frame-pointer -malign-int -m528x  -B/opt/rtems-devkit/rtems-bsps/rtems-4.10/m68k-rtems4.10/mcf5235/lib/ -specs bsp_specs -qrtems

##
## Application specific compiler switches 
##
ifeq ($(BUILD_TYPE),CFE_APP)
   APP_COPTS =   
   APP_ASOPTS   =
else
   APP_COPTS =  
   APP_ASOPTS   =
endif

##
## Extra Cflags for Assembly listings, etc.
##
LIST_OPTS    = -Wa,-a=$*.lis 

##
## gcc options for dependancy generation
## 
COPTS_D = $(APP_COPTS) $(ENDIAN_DEFS) $(TARGET_DEFS) $(ARCH_OPTS) $(SYSINCS) $(WARNINGS)

## 
## General gcc options that apply to compiling and dependency generation.
##
COPTS=$(LIST_OPTS) $(COPTS_D)

##
## Extra defines and switches for assembly code
##
ASOPTS = $(APP_ASOPTS) -P -xassembler-with-cpp 

##---------------------------------------------------------
## Application file extention type
## This is the defined application extention.
## Known extentions: Mac OS X: .bundle, Linux: .so, RTEMS:
##   .s3r, vxWorks: .o etc.. 
##---------------------------------------------------------
APP_EXT = o

####################################################
## Host Development System and Toolchain defintions
##
## Host OS utils
##
RM=rm -f
CP=cp

##
## Compiler tools
##
COMPILER  = m68k-rtems4.10-gcc
ASSEMBLER  = m68k-rtems4.10-gcc
LINKER	  = m68k-rtems4.10-ld
AR	  = m68k-rtems4.10-ar
NM        = m68k-rtems4.10-nm
OBJCPY    = m68k-rtems4.10-objcopy
TABLE_BIN = elf2cfetbl
