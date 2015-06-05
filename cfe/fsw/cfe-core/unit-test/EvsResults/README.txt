The EVS Services unit test results are expected to have the
following results for each of the EVS services source files:

ut_cfe_evs PASSED 175 tests.
ut_cfe_evs FAILED 0 tests.

gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/evs/cfe_evs_task.c' 99.45%  541
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/evs/cfe_evs.c' 100.00%  119
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/evs/cfe_evs_log.c' 100.00%  73
gcov: '/mnt/hgfs/Projects/Repo_cFE/fsw/cfe-core/src/evs/cfe_evs_utils.c' 98.48%  132

==========================================================================
cfe_evs_task.c - 99.45% % coverage

A DCR was entered to get full coverage of this file in the next release.

The lines not covered are:

        3:  234:         if (CFE_ES_GetResetType(NULL) == CFE_ES_POWERON_RESET)
        -:  235:         {
        1:  236:            CFE_ES_WriteToSysLog("Event Log cleared following power-on reset\n");
        1:  237:            EVS_ClearLog();
        1:  238:            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_DEFAULT_LOG_MODE;
        -:  239:         }
        9:  240:         else if (((CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LOG_OVERWRITE) &&
        2:  241:                   (CFE_EVS_GlobalData.EVS_LogPtr->LogMode != CFE_EVS_LOG_DISCARD))  ||
        2:  242:                  ((CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != FALSE)   &&
    #####:  243:                   (CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag != TRUE))   ||
        2:  244:                   (CFE_EVS_GlobalData.EVS_LogPtr->Next >= CFE_EVS_LOG_MAX))
        -:  245:         {
        5:  246:            CFE_ES_WriteToSysLog("Event Log cleared, n=%d, c=%d, f=%d, m=%d, o=%d\n",
        1:  247:                                  CFE_EVS_GlobalData.EVS_LogPtr->Next,
        1:  248:                                  CFE_EVS_GlobalData.EVS_LogPtr->LogCount,
        1:  249:                                  CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag,
        1:  250:                                  CFE_EVS_GlobalData.EVS_LogPtr->LogMode,
        1:  251:                                  CFE_EVS_GlobalData.EVS_LogPtr->LogOverflowCounter);
        1:  252:            EVS_ClearLog();
        1:  253:            CFE_EVS_GlobalData.EVS_LogPtr->LogMode = CFE_EVS_DEFAULT_LOG_MODE;
        -:  254:         }



        -:  390:   /* Register EVS task for event services */
        5:  391:   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
        5:  392:   if (Status != CFE_SUCCESS)
        -:  393:   {
    #####:  394:      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register Failed:RC=0x%08X\n",Status);
    #####:  395:      return Status;
        -:  396:   }
        -:  397:

==========================================================================

cfe_evs.c - 100.00 % coverage

==========================================================================

cfe_evs_log.c - 100.00% coverage

==========================================================================

cfe_evs_utils.c - 98.48% coverage

A DCR was entered to get full coverage of this file in the next release.

In EVS_SendEvent...
        -:  627:      /* Were any characters truncated in the buffer? */
      119:  628:      if (ExpandedLength >= CFE_EVS_MAX_MESSAGE_LENGTH)
        -:  629:      {
        -:  630:         /* Mark character before zero terminator to indicate truncation */
    #####:  631:         EVS_Packet.Message[CFE_EVS_MAX_MESSAGE_LENGTH - 2] = CFE_EVS_MSG_TRUNCATED;
    #####:  632:         CFE_EVS_GlobalData.EVS_TlmPkt.MessageTruncCounter++;
        -:  633:      }
        -:  634:

==========================================================================

