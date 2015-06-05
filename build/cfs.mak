################################################################################
#
# File: CFS supplemental makefile for applications
#
################################################################################

cfs_envs::
	@echo ""
	@echo ">>> [ENVS] MAKEFILE ENVIRONMENT VARIABLES <<<"
	@echo ""
	@echo "MISSION = $(MISSION)"
	@echo "THE_APPS = $(THE_APPS)"
	@echo "THE_TBLS = $(THE_TBLS)"
	@echo ""
	@echo "CFS_MISSION = $(CFS_MISSION)"
	@echo "CFS_MISSION_INC = $(CFS_MISSION_INC)"
	@echo "CFS_APP_SRC = $(CFS_APP_SRC)"
	@echo "CFE_FSW = $(CFE_FSW)"
	@echo "CFE_CORE_SRC = $(CFE_CORE_SRC)"
	@echo "CFE_CORE_PLATFORM_INC_SRC = $(CFE_CORE_PLATFORM_INC_SRC)"
	@echo "CFE_TOOLS = $(CFE_TOOLS)"
	@echo "CFE_PSP_SRC = $(CFE_PSP_SRC)"
	@echo "OSAL = $(OSAL)"
	@echo "OSAL_SRC = $(OSAL_SRC)"
	@echo ""
	@echo "BUILD_PATH = $(BUILD_PATH)"
	@echo "CFS_EXE = $(CFS_EXE)"
	@echo "CFS_PROM = $(CFS_PROM)"
	@echo "CFS_PROM_APPS = $(CFS_PROM_APPS)"
	@echo "CFS_RAM = $(CFS_RAM)"

#===============================================================================
# PRE-BUILD rules
#===============================================================================

cfs_copy_core_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING HEADER FILES FROM <CFS_MISSION> CORE <<<"
	@echo ""
	@echo "Copying <CFE_FSW>/mission_inc/*.h to <CFS_MISSION_INC>"
	@-cp -f $(CFE_FSW)/mission_inc/*.h $(CFS_MISSION_INC)
	@echo "Copying <CFE_FSW>/platform_inc/<CFE_CORE_PLATFORM_INC_SRC>/cfe_platform_cfg.h to ./inc"
	@-cp -f $(CFE_FSW)/platform_inc/$(CFE_CORE_PLATFORM_INC_SRC)/*.h ./inc
	@echo "Copying <OSAL>/build/inc/osconfig.h to ./inc"
	@-cp -f $(OSAL)/build/inc/osconfig.h ./inc

#-------------------------------------------------------------------------------

cfs_copy_app_shared_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING HEADER FILES FROM <CFS_APP_SRC>/INC <<<"
	@echo ""
	@echo "Copying <CFS_APP_SRC>/inc/*.h to <CFS_MISSION_INC>"
	@-cp -f $(CFS_APP_SRC)/inc/*.h $(CFS_MISSION_INC)

#-------------------------------------------------------------------------------

cfs_copy_app_public_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING HEADER FILES FROM <APP>/PUBLIC_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do \
           appIncDir=$(CFS_APP_SRC)/$$app/fsw/public_inc; \
           if test -d $${appIncDir}; then \
              for dirEntry in "$${appIncDir}"/*.h ; do \
                 test -f "$${dirEntry}" || continue ; \
                 echo "Copying $${dirEntry}"; \
                 cp -f $${dirEntry} ./inc; \
              done ; \
           fi ; \
        done

#-------------------------------------------------------------------------------

cfs_copy_app_mission_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING HEADER FILES FROM <APP>/MISSION_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do \
           appIncDir=$(CFS_APP_SRC)/$$app/fsw/mission_inc; \
           if test -d $${appIncDir}; then \
              for dirEntry in "$${appIncDir}"/*.h ; do \
                 test -f "$${dirEntry}" || continue ; \
                 echo "Copying $${dirEntry}"; \
                 cp -f $${dirEntry} $(CFS_MISSION_INC); \
              done ; \
           fi ; \
           if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_copy_app_platform_incs::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING HEADER FILES FROM <APP>/PLATFORM_INC <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            appIncDir=`echo $(CFS_APP_SRC)/$$app/fsw/platform_inc`; \
            if test -d $${appIncDir}; then \
              for dirEntry in "$${appIncDir}"/*.h ; do \
                 test -f "$${dirEntry}" || continue ; \
                 echo "Copying $${dirEntry}"; \
                 cp -f $${dirEntry} ./inc; \
              done ; \
            fi ; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_make_app_build_dirs::
	@echo ""
	@echo ">>> [PRE-BUILD] CREATING <APP> BUILD DIRECTORY <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            appDir=`echo ./$$app`; \
            if test -d $${appDir}; then \
                continue; \
            else \
                echo "Creating dir $${appDir}"; \
                mkdir -p $${appDir}; \
            fi; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_copy_app_makefiles::
	@echo ""
	@echo ">>> [PRE-BUILD] COPYING MAKEFILES FROM <APP>/FOR_BUILD <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            echo "Copying <CFS_APP_SRC>/$${app}/fsw/for_build/* to ./$${app}"; \
            cp -f $(CFS_APP_SRC)/$${app}/fsw/for_build/* ./$${app}; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_prebuild:: cfs_copy_core_incs         \
               cfs_copy_app_shared_incs   \
               cfs_copy_app_public_incs   \
               cfs_copy_app_mission_incs  \
               cfs_copy_app_platform_incs \
               cfs_make_app_build_dirs    \
               cfs_copy_app_makefiles

#===============================================================================
# IN-BUILD RULES
#===============================================================================

cfs_build_core::
	@echo ""
	@echo ">>> [BUILD] BUILD CFE <<<"
	@echo ""
	$(MAKE) -C cfe

#-------------------------------------------------------------------------------

cfs_build_apps::
	@echo ""
	@echo ">>> [BUILD] BUILD APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            $(MAKE) -C $${app}; \
	    if test $${app} == "sch"; then \
	        $(MAKE) -f schlib.mak -C sch; \
            fi; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_build_app_tbls::
	@echo ""
	@echo ">>> [BUILD] BUILD APP TABLES <<<"
	@echo ""
	@for app in $(THE_TBLS); do \
            if test -f ./$${app}/$${app}tables.mak; then \
                $(MAKE) -f $${app}tables.mak -C $${app}; \
                if test $$? != 0; then exit 1; fi; \
            fi; \
            if test -f ./$${app}/$${app}_tables.mak; then \
                $(MAKE) -f $${app}_tables.mak -C $${app}; \
                if test $$? != 0; then exit 1; fi; \
            fi; \
        done

#-------------------------------------------------------------------------------

cfs_inbuild:: cfs_build_core     \
              cfs_build_apps     \
              cfs_build_app_tbls 

#===============================================================================
# POST-BUILD RULES
#===============================================================================

cfs_install_core::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL CORE <<<"
	@echo ""
	$(MAKE) -C cfe install

#-------------------------------------------------------------------------------

cfs_install_apps::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            $(MAKE) -C $${app} install; \
	    if test $${app} == "sch"; then \
	        $(MAKE) -f schlib.mak -C sch install; \
            fi; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_install_app_tbls::
	@echo ""
	@echo ">>> [POST-BUILD] INSTALL APP TABLES <<<"
	@echo ""
	@for app in $(THE_TBLS); do \
            if test -f $${app}/$${app}tables.mak; then \
                $(MAKE) -f $${app}tables.mak -C $${app} install; \
                if test $$? != 0; then exit 1; fi; \
            fi; \
            if test -f $${app}/$${app}_tables.mak; then \
                $(MAKE) -f $${app}_tables.mak -C $${app} install; \
                if test $$? != 0; then exit 1; fi; \
            fi; \
        done

#-------------------------------------------------------------------------------

cfs_copy_installs::
	@echo ""
	@echo ">>> [POST-BUILD] COPY BUILT FILES TO <CFS_PROM_APPS> <<<"
	@echo " >> Create Directories <<"
	-mkdir -p $(CFS_PROM)
	-mkdir -p $(CFS_PROM_APPS)
	-mkdir -p $(CFS_PROM)/log
	-mkdir -p $(CFS_PROM)/upload
	-mkdir -p $(CFS_PROM)/download
	-mkdir -p $(CFS_RAM)
	@echo ""
	@echo " >> Change Permissions <<"
	-chmod 775 $(CFS_EXE)/*.$(CFE_CORE_EXE_TYPE) 
	-chmod 644 $(CFS_EXE)/*.$(CFS_APP_EXE_TYPE) 
	-chmod 644 $(CFS_EXE)/*.tbl
	-chmod 644 $(CFS_EXE)/*.scr
	@echo ""
	@echo " >> Copy Files <<"
	-cp -f $(CFS_EXE)/*.$(CFS_APP_EXE_TYPE) $(CFS_PROM_APPS)
	-cp -f $(CFS_EXE)/*.tbl $(CFS_PROM_APPS)
	@echo ""
	@if test -f $(CFS_EXE)/run; then \
            echo "cp -f $(CFS_EXE)/run $(CFS_PROM_APPS)"; \
            cp -f $(CFS_EXE)/run $(CFS_PROM_APPS); \
        fi;
	-cp -f $(CFS_EXE)/cfe_es_startup.scr $(CFS_PROM_APPS)

#-------------------------------------------------------------------------------

cfs_postbuild:: cfs_install_core     \
                cfs_install_apps     \
                cfs_install_app_tbls \
                cfs_copy_installs
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#===============================================================================
# CLEAN BUILD RULES
#===============================================================================

cfs_clean_core::
	@echo ""
	@echo ">>> [CLEAN] CLEAN CORE <<<"
	@echo ""
	$(MAKE) -C cfe clean

#-------------------------------------------------------------------------------

cfs_clean_apps::
	@echo ""
	@echo ">>> [CLEAN] CLEAN APPS <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            if test -d $${app}; then \
                $(MAKE) -C $${app} clean; \
            fi; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_clean_build_exe::
	@echo ""
	@echo ">>> [CLEAN] REMOVE BUILD/EXE BUILT FILES <<<"
	@echo ""
	-rm -f ./exe/*.$(CFE_CORE_EXE_TYPE)
	-rm -f ./exe/*.$(CFS_APP_EXE_TYPE)
	-rm -f ./exe/*.tbl

#-------------------------------------------------------------------------------

cfs_clean_build_mission_incs::
	@echo ""
	@echo ">>> [CLEAN] - REMOVE <CFS_MISSION_INC> HEADER FILES <<<"
	@echo ""
	-rm -f $(CFS_MISSION_INC)/*

#-------------------------------------------------------------------------------

cfs_clean_build_platform_incs::
	@echo ""
	@echo ">>> [CLEAN] REMOVE BUILD/INC HEADER FILES <<<"
	@echo ""
	-rm -f ./inc/*

#-------------------------------------------------------------------------------

cfs_clean:: cfs_clean_core                \
            cfs_clean_apps                \
            cfs_clean_build_exe           
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#-------------------------------------------------------------------------------

cfs_clean_install_dirs::
	@echo ""
	@echo ">>> [REALCLEAN] REMOVE INSTALL DIRS <<<"
	@echo ""
	-rm -rf $(CFS_PROM)
	-rm -rf $(CFS_RAM)

#-------------------------------------------------------------------------------

cfs_clean_app_build_dirs::
	@echo ""
	@echo ">>> [REALCLEAN] REMOVE APP DIRS <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            if test -d $${app}; then \
                echo "Removing dir $${app}"; \
                rm -rf ./$${app}; \
            fi; \
            if test $$? != 0; then exit 1; fi; \
	done

#-------------------------------------------------------------------------------

cfs_realclean:: cfs_clean_core                \
                cfs_clean_apps                \
                cfs_clean_build_exe           \
                cfs_clean_build_mission_incs  \
                cfs_clean_build_platform_incs \
                cfs_clean_app_build_dirs      \
                cfs_clean_install_dirs        
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

#===============================================================================
# DEPEND BUILD RULES
#===============================================================================

cfs_depend_core::
	@echo ""
	@echo ">>> [DEPEND] RUN CORE DEPENDENCIES <<<"
	@echo ""
	$(MAKE) -C cfe depend

#-------------------------------------------------------------------------------

cfs_depend_apps::
	@echo ""
	@echo ">>> [DEPEND] RUN APP DEPENDENCIES <<<"
	@echo ""
	@for app in $(THE_APPS); do \
            $(MAKE) -C $${app} depend; \
            if test $$? != 0; then exit 1; fi; \
        done

#-------------------------------------------------------------------------------

cfs_depend:: cfs_depend_core \
             cfs_depend_apps 

#===============================================================================
# Tool RULES
#===============================================================================
cfs_tool:: cfs_copy_core_incs cfs_make_tool_build_dirs
	@echo ""
	@echo ">>> [TOOL] BUILD elf2cfetbl TOOL <<<"
	@echo ""
	$(MAKE) -C elf2cfetbl 
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

cfs_cleantool::
	@echo ""
	@echo ">>> [CLEANTOOL] REMOVE elf2cfetbl TOOL <<<"
	@echo ""
	$(RM) -rf elf2cfetbl
	@echo ""
	@echo ">>> DONE! <<<"
	@echo ""

cfs_make_tool_build_dirs::
	@echo ""
	@echo ">>> [PRE-BUILD] CREATE TOOL BUILD DIRECTORIES <<<"
	@if ((test -d elf2cfetbl) && (test -f elf2cfetbl/Makefile)); then \
		continue; \
	else \
		echo "Creating dir elf2cfetbl"; \
		mkdir -p elf2cfetbl; \
		echo "Copying <CFE_TOOLS>/elf2cfetbl/for_build/Makefile to elf2cfetbl"; \
		cp -f $(CFE_TOOLS)/elf2cfetbl/for_build/Makefile elf2cfetbl ; \
	fi; 

################################################################################
# End of CFS supplemental makefile for applications
################################################################################

