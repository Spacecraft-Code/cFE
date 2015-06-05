#
# Master makefile for unit tests
#

#
# Rule to build all unit tests for cfe core
#
default:
	make -C sb
	make -C es 
	make -C evs 
	make -C time 
	make -C tbl 
	make -C fs

#
# Rule to run the unit tests and run gcov
# 
gcov:
	make -C sb gcov
	make -C es gcov
	make -C evs gcov
	make -C time gcov
	make -C tbl gcov
	make -C fs gcov

# 
# Rule to clean out all unit tests
#
clean:
	make -C sb clean
	make -C es clean
	make -C evs clean
	make -C time clean
	make -C tbl clean
	make -C fs clean

