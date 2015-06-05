ut_cfe_es PASSED 372 tests.
ut_cfe_es FAILED 0 tests.

gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_start.c'
98.54%  206
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_perf.c'
100.00%  156
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_erlog.c'
100.00%  29
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_cds.c'
100.00%  200
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_api.c'
98.25%  515
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_apps.c'
100.00%  449
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_cds_mempool.c'
100.00%  224
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_esmempool.c'
100.00%  206
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_shell.c'
100.00%  136
gcov:
'/home/acudmore/Projects/cFE-6.4-development/cfe/fsw/cfe-core/src/es/cfe_es_task.c'
99.36%  628



The ES Services unit test results are expected to have the
following results for each of the ES services source files:

==========================================================================

cfe_es_api.c - 98.25% coverage (5 Lines of Code NOT executed)


CFE_ES_ExitApp

        -:  504:       else /* It is an external App */
        -:  505:       {
        -:  506:
    #####:  507:          CFE_ES_WriteToSysLog ("Application %s called CFE_ES_ExitApp\n",
        -:  508:                                 CFE_ES_Global.AppTable[AppID].StartParams.Name);
    #####:  509:          CFE_ES_Global.AppTable[AppID].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
        -:  510:
        -:  511:
        -:  512:          /*
        -:  513:          ** Unlock the ES Shared data before suspending the app
        -:  514:          */
    #####:  515:          CFE_ES_UnlockSharedData(__func__,__LINE__);
        -:  516:
        -:  517:          /*
        -:  518:          ** Suspend the Application until ES kills it.
        -:  519:          ** It might be better to have a way of suspending the app in the OS
        -:  520:          */
        -:  521:          while(1)
        -:  522:          {
    #####:  523:             OS_TaskDelay(500);
    #####:  524:          }
        -:  525:
        -:  526:       } /* end if */

REASON: Unit testing this branch is not possible because of the need for
        the infinite loop.
==========================================================================
cfe_es_apps.c - 100.00% coverage

==========================================================================
cfe_es_cds.c - 100.00% coverage

==========================================================================
cfe_es_cds_mempool.c - 100.00% coverage

==========================================================================
cfe_es_erlog.c - 100.00% coverage

==========================================================================
cfe_esmempool.c - 100.00% coverage

==========================================================================
cfe_es_perf.c - 100 % coverage

==========================================================================
cfe_es_shell.c - 100 % coverage

==========================================================================
cfe_es_start.c - 98.54% coverage

CFE_ES_Main

        3:  303:   CFE_ES_LockSharedData(__func__,__LINE__);
        3:  304:   if (( CFE_ES_Global.StartupSemaphoreReleased == FALSE) && ( CFE_ES_Global.AppStartupCounter <= 0 ))
        -:  305:   {
    #####:  306:      CFE_ES_Global.AppStartupCounter = 0;
    #####:  307:      CFE_ES_Global.StartupSemaphoreReleased = TRUE;
    #####:  308:      OS_BinSemFlush(CFE_ES_Global.StartupSyncSemaphore);
        -:  309:   }
        3:  310:   CFE_ES_UnlockSharedData(__func__,__LINE__);
        -:  311:
        -:  312:   /*
        -:  313:   ** Startup is complete
        -:  314:   */
        3:  315:   CFE_ES_WriteToSysLog("ES Startup: CFE Core Startup Complete\n");

REASON: Unit testing this branch is not possible because the variables 
        that cause this path to execute are set earlier in the function. 
==========================================================================

cfe_es_task.c - 99.36%

CFE_ES_StartAppCmd

        5:  841:       else if (cmd->AppEntryPoint == NULL)
        -:  842:       {
    #####:  843:          CFE_ES_TaskData.ErrCounter++;
    #####:  844:          CFE_EVS_SendEvent(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID, CFE_EVS_ERROR,
        -:  845:                           "CFE_ES_StartAppCmd: App Entry Point is NULL.");
        -:  846:       }
        5:  847:       else if (cmd->Application == NULL)
        -:  848:       {
    #####:  849:          CFE_ES_TaskData.ErrCounter++;
    #####:  850:          CFE_EVS_SendEvent(CFE_ES_START_NULL_APP_NAME_ERR_EID, CFE_EVS_ERROR,
        -:  851:                           "CFE_ES_StartAppCmd: App Name is NULL.");
        -:  852:       }
        5:  853:       else if (cmd->StackSize < CFE_ES_DEFAULT_STACK_SIZE)


REASON: The structure used in the unit tests containing the command cannot
        have a null value, so these two checks against null cannot be satisfied.
        A future build will fix the command parameter validation to allow 
        100% on this file.

==========================================================================
