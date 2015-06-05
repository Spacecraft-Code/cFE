###############################################################################
# File: makefile
#
# Purpose:
#   Main makefile for Mission build and all apps
#
# $Id: Makefile 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw Exp  $
#
# $Log: Makefile  $
# Revision 1.5 2014/06/27 15:44:43GMT-05:00 rmcgraw 
# DCR22713:1 Removed extra tabs and inserted history log
#
###############################################################################
##
## Top Level rules:
## If you want to build the testapps every time, just add the testapps targets
## to the primary targets below ( or uncomment the testapps versions )
##
export MISSIONNAME:=Mission

##############################################################################
## Build rules

all ::
	$(MAKE) -C cpu1 all
	$(MAKE) -C cpu2 all
	$(MAKE) -C cpu3 all

clean ::
	$(MAKE) -C cpu1 clean
	$(MAKE) -C cpu2 clean
	$(MAKE) -C cpu3 clean

install ::
	$(MAKE) -C cpu1 install
	$(MAKE) -C cpu2 install
	$(MAKE) -C cpu3 install

depend ::
	$(MAKE) -C cpu1 depend
	$(MAKE) -C cpu2 depend
	$(MAKE) -C cpu3 depend

#-------------------------------------------------------------------------------
# Doxygen rules


usersguide ::
	$(MAKE) -C cpu1  usersguide
	$(MAKE) -C cpu2  usersguide
	$(MAKE) -C cpu3  usersguide

detaileddocs ::
	$(MAKE) -C cpu1  detaileddocs
	$(MAKE) -C cpu2  detaileddocs
	$(MAKE) -C cpu3  detaileddocs

installdocs ::
	$(MAKE) -C cpu1  installdocs
	$(MAKE) -C cpu2  installdocs
	$(MAKE) -C cpu3  installdocs

cleandocs ::
	$(MAKE) -C cpu1  cleandocs
	$(MAKE) -C cpu2  cleandocs
	$(MAKE) -C cpu3  cleandocs

