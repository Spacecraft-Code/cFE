/*
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    es_UT.c
**
** Purpose:
**    Executive Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/09/23 18:20:57GMT-05:00 $
** $Revision: 1.18 $
**
*/

/*
** Includes
*/
#include "es_UT.h"

/*
** External global variables
*/
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t FSWriteHdrRtn;
extern UT_SetRtn_t OSReadRtn;
extern UT_SetRtn_t OSlseekRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t BSPWriteCDSRtn;
extern UT_SetRtn_t BSPReadCDSRtn;
extern UT_SetRtn_t BlocksFreeRtn;
extern UT_SetRtn_t UnmountRtn;
extern UT_SetRtn_t EVSCleanUpRtn;
extern UT_SetRtn_t SB_SubscribeExRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SBSendMsgRtn;
extern UT_SetRtn_t CountSemDelRtn;
extern UT_SetRtn_t MutSemDelRtn;
extern UT_SetRtn_t BinSemDelRtn;
extern UT_SetRtn_t QueueDelRtn;
extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t HeapGetInfoRtn;
extern UT_SetRtn_t BSPGetCFETextRtn;
extern UT_SetRtn_t FSIsGzFileRtn;
extern UT_SetRtn_t ModuleLoadRtn;
extern UT_SetRtn_t ModuleUnloadRtn;
extern UT_SetRtn_t ModuleInfoRtn;
extern UT_SetRtn_t FSDecompressRtn;
extern UT_SetRtn_t FSExtractRtn;
extern UT_SetRtn_t SymbolLookupRtn;
extern UT_SetRtn_t OSPrintRtn;
extern UT_SetRtn_t OSTaskExitRtn;
extern UT_SetRtn_t PSPRestartRtn;
extern UT_SetRtn_t OSBinSemTimedWaitRtn;
extern UT_SetRtn_t OSBinSemFlushRtn;
extern UT_SetRtn_t PSPPanicRtn;
extern UT_SetRtn_t TBLEarlyInitRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t TIMECleanUpRtn;
extern UT_SetRtn_t SBCleanUpRtn;
extern UT_SetRtn_t OSCloseRtn;
extern UT_SetRtn_t PSPMemValRangeRtn;
extern UT_SetRtn_t OSTimerGetInfoRtn;

extern UT_Task_t             UT_Task[OS_MAX_TASKS];
extern CFE_ES_PerfData_t     *Perf;
extern CFE_ES_Global_t       CFE_ES_Global;
extern CFE_ES_CDSBlockDesc_t CFE_ES_CDSBlockDesc;
extern CFE_ES_TaskData_t     CFE_ES_TaskData;
extern CFE_SB_MsgId_t        UT_RcvMsgId;

#ifdef CFE_ARINC653
extern CFE_ES_StaticStartupTable_t CFE_ES_StaticStartupTable[];
#endif

/*
** Global variables
*/
/* Create a startup script buffer for a maximum of 5 lines * 80 chars/line */
char StartupScript[MAX_STARTUP_SCRIPT];

/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int es_main(void)
#endif
{
    /* Set up the performance logging variable */
    Perf = (CFE_ES_PerfData_t *) &CFE_ES_ResetDataPtr->Perf;
    TestInit();
    TestStartupErrorPaths(); /* Done testing cfe_es_start.c */
    TestApps(); /* NOT done testing cfe_es_apps.c */
    TestERLog(); /* Done testing cfe_es_erlog.c */
    TestShell(); /* Done testing cfe_es_shell.c */
    TestTask(); /* Done testing the cfe_es_task.c */
    TestPerf(); /* Done testing cfe_es_perf.c */
    TestAPI(); /* Done testing cfe_es_api.c */
    TestCDS(); /* Done testing cfe_es_cds.c */
    TestCDSMempool(); /* Done testing cfe_es_cds_mempool.c */
    TestESMempool();

#ifdef CFE_ARINC653
    TestStaticApp();
#endif

    UT_ReportFailures();
    return 0;
}

void TestInit(void)
{
    int i;

    UT_Init("es");
    UT_Text("cFE ES Unit Test Output File\n\n");

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    UT_SetCDSSize(128 * 1024);
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));

    /* Initialize the OS Tasks table */
    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        UT_Task[i].free = TRUE;
    }

    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths - POWER On */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    CFE_ES_Main(CFE_ES_POWERON_RESET, CFE_ES_POWER_CYCLE, 1,
                (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(PSPPanicRtn.value == 0 && PSPPanicRtn.count == 0,
              "CFE_ES_Main",
              "Normal startup - Power on - Power cycle",
              "01.001");

    /* Initialize the OS Tasks table */
    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        UT_Task[i].free = TRUE;
    }

    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths  - HW Special command power on*/
    UT_SetDummyFuncRtn(OS_SUCCESS);
    CFE_ES_Main(CFE_ES_POWERON_RESET, CFE_ES_HW_SPECIAL_COMMAND, 1,
                (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(PSPPanicRtn.value == 0 && PSPPanicRtn.count == 0,
              "CFE_ES_Main",
              "Normal startup - Power on - Special command",
              "01.002");

    /* Initialize the OS Tasks table */
    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        UT_Task[i].free = TRUE;
    }
    
    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths  -  Power on, other */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    CFE_ES_Main(CFE_ES_POWERON_RESET, CFE_ES_HW_SPECIAL_COMMAND + 2, 1,
                (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(PSPPanicRtn.value == 0 && PSPPanicRtn.count == 0,
              "CFE_ES_Main",
              "Normal startup - Power on - other",
              "01.003");

    /* Test the path for a special command processor reset */
    UT_InitData();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = FALSE;
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount == 1,
              "CFE_ES_SetupResetVariables",
              "HW Special Command Processor reset",
              "01.004");

    /* Attempt another processor reset after the maximum have occurred */
    UT_InitData();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 2;
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, CFE_ES_HW_SPECIAL_COMMAND, 1);
    UT_Report(CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount > 
                  CFE_ES_MAX_PROCESSOR_RESETS &&
              PSPRestartRtn.value == CFE_ES_POWERON_RESET &&
              PSPRestartRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "HW Special Command Processor Reset - Exceeded maximum processor resets",
              "01.005");

    /* Perform ES main startup with a stuck startup semaphore */
    UT_InitData();
    CFE_ES_Global.StartupSemaphoreReleased = FALSE;
    CFE_ES_Global.AppStartupCounter = 0;
    CFE_ES_Main(CFE_ES_POWERON_RESET, 1, 1, NULL);
}

void TestStartupErrorPaths(void)
{
    int j;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Startup Error Paths\n");
#endif

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';

    /* Perform ES main startup with a mutex creation failure */
    UT_InitData();
    UT_SetOSFail(OS_MUTCREATE_FAIL);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    CFE_ES_Main(CFE_ES_POWERON_RESET, 1, 1,
                (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(PSPPanicRtn.value == CFE_PSP_PANIC_STARTUP_SEM &&
              PSPPanicRtn.count == 1,
              "CFE_ES_Main",
              "Mutex create failure",
              "02.001");

    /* Perform ES main startup with a startup sync semaphore
     * creation failure
     */
    UT_InitData();
    UT_SetOSFail(OS_SEMCREATE_FAIL);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    CFE_ES_Main(CFE_ES_POWERON_RESET, 1, 1,
                (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(PSPPanicRtn.value == CFE_PSP_PANIC_STARTUP_SEM &&
              PSPPanicRtn.count == 1,
              "CFE_ES_Main",
              "Semaphore create failure",
              "02.002");

    /* Perform the maximum number of processor resets */
    UT_InitData();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = FALSE;
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, 1, 1);
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, 1, 1);
    UT_Report(CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount == 2,
              "CFE_ES_SetupResetVariables",
              "Maximum processor resets",
              "02.003");

    /* Attempt another processor reset after the maximum have occurred */
    UT_InitData();
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, 1, 1);
    UT_Report(CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount > 
                  CFE_ES_MAX_PROCESSOR_RESETS &&
              PSPRestartRtn.value == CFE_ES_POWERON_RESET &&
              PSPRestartRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Exceeded maximum processor resets",
              "02.004");

    /* Perform a processor reset with an reset area failure */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_TONE_PRI);
    CFE_ES_ResetDataPtr->ResetVars.ES_CausedReset = TRUE;
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, 1, 1);
    UT_Report(PSPPanicRtn.value == CFE_PSP_PANIC_MEMORY_ALLOC &&
              PSPPanicRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Get reset area error",
              "02.005");

    /* Perform a processor reset with the size of the reset area too small */
    UT_InitData();
    UT_SetSizeofESResetArea(0);
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_TONE_PRI);
    CFE_ES_SetupResetVariables(CFE_ES_PROCESSOR_RESET, 1, 1);
    UT_Report(PSPPanicRtn.value == CFE_PSP_PANIC_MEMORY_ALLOC &&
              PSPPanicRtn.count == 1,
              "CFE_ES_SetupResetVariables",
              "Reset area too small",
              "02.006");

    /* Test initialization of the file systems specifying a power on reset
     * following a failure to create the RAM volume
     */
    UT_InitData();
    UT_SetOSFail(OS_INITFS_FAIL | OS_MOUNT_FAIL | OS_MKFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_ES_POWERON_RESET);
    UT_Report(OSPrintRtn.value == 23 + 25 && OSPrintRtn.count == -3,
              "CFE_ES_InitializeFileSystems",
              "Power on reset; error creating volatile (RAM) volume",
              "02.007");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to reformat the RAM volume
     */
    UT_InitData();
    UT_SetOSFail(OS_INITFS_FAIL | OS_MOUNT_FAIL | OS_MKFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 22 + 23 + 24 + 25 + 27 &&
              OSPrintRtn.count == -8,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error reformatting volatile (RAM) volume",
              "02.008");

    /* Test initialization of the file systems specifying a processor reset
     * following failure to get the volatile disk memory
     */
    UT_InitData();
    UT_SetBSPFail(BSP_GETVOLDISKMEM_FAIL);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 22 &&
              OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; cannot get memory for volatile disk",
              "02.009");

    /* Test initialization of the file systems where the number of free blocks
     * reported is greater than the number of RAM disk sectors
     */
    UT_InitData();
    UT_SetOSFail(OS_MOUNT_FAIL);
    UT_SetRtnCode(&BlocksFreeRtn, CFE_ES_RAM_DISK_NUM_SECTORS + 1, 1);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 25 && OSPrintRtn.count == -3,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; truncate free block count",
              "02.010");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remove the RAM volume
     */
    UT_InitData();
    UT_SetOSFail(OS_RMFS_FAIL);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 22 + 28 && OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error removing volatile (RAM) volume",
              "02.011");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to unmount the RAM volume
     */
    UT_InitData();
    UT_SetRtnCode(&UnmountRtn, -1, 1);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 22 + 29 && OSPrintRtn.count == -4,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error unmounting volatile (RAM) volume",
              "02.012");

    /* Test successful initialization of the file systems */
    UT_InitData();
    CFE_ES_InitializeFileSystems(4564564);
    UT_Report(OSPrintRtn.value == 0 && OSPrintRtn.count == -1,
              "CFE_ES_InitializeFileSystems",
              "Initialize file systems; successful",
              "02.013");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remount the RAM volume
     */
    UT_InitData();
    UT_SetOSFail(OS_MOUNT_FAIL);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 22 + 25 + 26 && OSPrintRtn.count == -5,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error remounting volatile (RAM) volume",
              "02.014");

    /* Test initialization of the file systems with an error determining the
     * number of blocks that are free on the volume
     */
    UT_InitData();
    UT_SetRtnCode(&BlocksFreeRtn, -1, 1);
    CFE_ES_InitializeFileSystems(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 30 && OSPrintRtn.count == -2,
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error determining blocks free on volume",
              "02.015");

    /* Test reading the object table where a record used error occurs */
    UT_InitData();
    CFE_ES_CreateObjects();
    UT_Report(OSPrintRtn.value == 32 && OSPrintRtn.count == -19,
              "CFE_ES_CreateObjects",
              "Record used error",
              "02.016");

    /* Test reading the object table where an error occurs when
     * calling a function
     */
    UT_InitData();
    UT_SetRtnCode(&TBLEarlyInitRtn, 0, 1);
    CFE_ES_CreateObjects();
    UT_Report(OSPrintRtn.value == 32 + 34 && OSPrintRtn.count == -20,
              "CFE_ES_CreateObjects",
              "Error returned when calling function",
              "02.017");

    /* Test reading the object table where an error occurs when
     * creating a core app
     */
    UT_InitData();
    UT_SetOSFail(OS_TASKCREATE_FAIL | OS_SEMCREATE_FAIL);
    CFE_ES_CreateObjects();
    UT_Report(OSPrintRtn.value == 31 && OSPrintRtn.count == -14,
              "CFE_ES_CreateObjects",
              "Error creating core application",
              "02.018");

    /* Test reading the object table where all app slots are taken */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    CFE_ES_CreateObjects();
    UT_Report(OSPrintRtn.value == 33 * 5 && OSPrintRtn.count == -14,
              "CFE_ES_CreateObjects",
              "No free application slots available",
              "02.019");

    /* Test reading the object table with a NULL function pointer */
    UT_InitData();
    CFE_ES_ObjectTable[1].ObjectType = CFE_ES_FUNCTION_CALL;
    CFE_ES_CreateObjects();
    UT_Report(OSPrintRtn.value == 33 * 5 + 35 && OSPrintRtn.count == -15,
              "CFE_ES_CreateObjects",
              "Bad function pointer",
              "02.020");
}

void TestApps(void)
{
    int NumBytes;
    uint32 Id;
    int Return;
    int j;
    CFE_ES_AppInfo_t AppInfo;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Apps\n");
#endif

    /* Test starting an application where the startup script is too long */
    UT_InitData();
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_fghfghfghfghfg"
            "hfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghLIB, "
            "0, 0, 0x0, 1; CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, "
            "70, 4096, 0x0, 1; CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, "
            "SCH_APP, 120, 4096, 0x0, 1; CFE_APP, /cf/apps/to.bundle, "
            "TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_ES_PROCESSOR_RESET,
                             (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(OSPrintRtn.value == 36 + 39 && OSPrintRtn.count == -9,
              "CFE_ES_StartApplications",
              "Line too long",
              "03.001");

    /* Create a valid startup script for subsequent tests */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);

    /* Test starting an application with an error reading the startup file */
    UT_InitData();
    UT_SetRtnCode(&OSReadRtn, -1, 1);
    CFE_ES_StartApplications(CFE_ES_PROCESSOR_RESET,
                             (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(OSPrintRtn.value == 37 + 39 && OSPrintRtn.count == -3,
              "CFE_ES_StartApplications",
              "Error reading startup file",
              "03.002");

    /* Test starting an application with an end-of-file returned by the
     * OS read
     */
    UT_InitData();
    UT_SetRtnCode(&OSReadRtn, 0, 1);
    CFE_ES_StartApplications(CFE_ES_PROCESSOR_RESET,
                             (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(OSPrintRtn.value == 39 && OSPrintRtn.count == -2,
              "CFE_ES_StartApplications",
              "End of file reached",
              "03.003");

    /* Test starting an application with an open failure */
    UT_InitData();
    UT_SetOSFail(OS_OPEN_FAIL);
    CFE_ES_StartApplications(CFE_ES_PROCESSOR_RESET,
                             (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(OSPrintRtn.value == 38 && OSPrintRtn.count == -3,
              "CFE_ES_StartApplications",
              "Can't open ES application startup file",
              "03.004");

    /* Test successfully starting an application */
    UT_InitData();
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_ES_PROCESSOR_RESET,
                             (uint8 *) CFE_ES_NONVOL_STARTUP_FILE);
    UT_Report(OSPrintRtn.value == 39 && OSPrintRtn.count == -9,
              "CFE_ES_StartApplications",
              "Start application; successful",
              "03.005");

    /* Test parsing the startup script with an invalid CFE driver type */
    UT_InitData();
    strncpy(StartupScript,
            "CFE_DRV /cf/apps/tst_lib.bundle TST_LIB_Init TST_LIB 0 0 0x0 1",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Unimplemented CFE driver type",
              "03.006");

    /* Test parsing the startup script with an unknown entry type */
    UT_InitData();
    strncpy(StartupScript,
            "UNKNOWN /cf/apps/tst_lib.bundle TST_LIB_Init TST_LIB 0 0 0x0 1",
            MAX_STARTUP_SCRIPT);
    StartupScript[MAX_STARTUP_SCRIPT - 1] = '\0';
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Unknown entry type",
              "03.007");

    /* Test parsing the startup script with an invalid file entry */
    UT_InitData();
    strcpy(StartupScript, "");
    NumBytes = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_Report(CFE_ES_ParseFileEntry(StartupScript) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_ParseFileEntry",
              "Invalid file entry",
              "03.008");

    /* Test application loading and creation with a task creation failure */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
       CFE_ES_Global.AppTable[j].RecordUsed = FALSE;
    }

    UT_SetOSFail(OS_TASKCREATE_FAIL);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename",
                              "EntryPoint",
                              "AppName",
                              170,
                              4096,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 51,
              "CFE_ES_AppCreate",
              "Task create failure",
              "03.009");

    /* Test successful application loading and creation  */
    UT_InitData();

    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        CFE_ES_Global.TaskTable[j].RecordUsed = FALSE;
    }

    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_SUCCESS,
              "CFE_ES_AppCreate",
              "Application load/create; successful",
              "03.010");

    /* Test application loading and creation with a file
     * decompression failure
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 52,
              "CFE_ES_AppCreate",
              "Decompression failure",
              "03.011");

    /* Test application loading, creation and decompression */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, 0, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_SUCCESS &&
              OSPrintRtn.value == 53,
              "CFE_ES_AppCreate",
              "Decompression; successful",
              "03.012");

    /* Test application loading and creation where the file name cannot be
     * extracted from the path
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSExtractRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 54,
              "CFE_ES_AppCreate",
              "File name extraction failure",
              "03.013");

    /* Test application loading and creation where the file cannot be loaded */
    UT_InitData();
    UT_SetRtnCode(&ModuleLoadRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 55,
              "CFE_ES_AppCreate",
              "File load failure",
              "03.014");

    /* Test application loading and creation where the entry point symbol
     * cannot be found
     */
    UT_InitData();
    UT_SetRtnCode(&SymbolLookupRtn, -1, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/filename.x",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 56,
              "CFE_ES_AppCreate",
              "Entry point symbol lookup failure",
              "03.015");

    /* Test application loading and creation where the application file name
     * is too long
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/this_is_a_filename_that_exceeds_the_maximum_"
                                "allowed_length_when_added_to_path.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 57,
              "CFE_ES_AppCreate",
              "Application file name too long",
              "03.016");

    /* Test application loading and creation where the application path + file
     * name is too long
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    Return = CFE_ES_AppCreate(&Id,
                              "ut/1234567890123456789012345678901234567890"
                                "12345678901234567890.gz",
                              "EntryPoint",
                              "AppName",
                              170,
                              8192,
                              1);
    UT_Report(Return == CFE_ES_ERR_APP_CREATE &&
              OSPrintRtn.value == 58,
              "CFE_ES_AppCreate",
              "Application file name + path too long",
              "03.017");

    /* Test shared library loading and initialization where the initialization
     * routine returns an error
     */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = FALSE;
    }

    UT_SetDummyFuncRtn(OS_ERROR);
    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 43,
              "CFE_ES_LoadLibrary",
              "Load shared library initialization failure",
              "03.018");

    /* Test successful shared library loading and initialization with a
     * gzip'd library
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_SUCCESS,
              "CFE_ES_LoadLibrary",
              "Decompress library; successful",
              "03.019");

    /* Test shared library loading and initialization with a gzip'd library
     * where the decompression fails
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSDecompressRtn, -1, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 44,
              "CFE_ES_LoadLibrary",
              "Unable to decompress library",
              "03.020");

    /* Test shared library loading and initialization where file name cannot
     * be extracted from the path
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetRtnCode(&FSExtractRtn, -1, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 45,
              "CFE_ES_LoadLibrary",
              "Unable to extract file name from path",
              "03.021");

    /* Test shared library loading and initialization where the file name is
     * too long
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/this_is_a_filename_that_"
                                  "exceeds_the_maximum_allowed_length_"
                                  "when_added_to_path.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 46,
              "CFE_ES_LoadLibrary",
              "Unable to extract file name from path; file name too long",
              "03.022");

    /* Test shared library loading and initialization where the library path +
     * file name is too long
     */
    UT_InitData();
    UT_SetRtnCode(&FSIsGzFileRtn, TRUE, 1);
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/123456789012345678901234567890"
                                  "123456789012345678901234567890.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 47,
              "CFE_ES_LoadLibrary",
              "Library path + file name too long",
              "03.023");

    /* Test shared library loading and initialization where the library
     * fails to load
     */
    UT_InitData();
    UT_SetRtnCode(&ModuleLoadRtn, -1, 1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 48,
              "CFE_ES_LoadLibrary",
              "Load shared library failure",
              "03.024");

    /* Test shared library loading and initialization where the library
     * entry point symbol cannot be found
     */
    UT_InitData();
    UT_SetRtnCode(&SymbolLookupRtn, -1, 1);
    Return = CFE_ES_LoadLibrary(&Id,
                                "/cf/apps/tst_lib.bundle.gz",
                                "TST_LIB_Init",
                                "TST_LIB");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 49,
              "CFE_ES_LoadLibrary",
              "Could not find library initialization symbol",
              "03.025");

    /* Test shared library loading and initialization where there are no
     * library slots available
     */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = TRUE;
    }

    Return = CFE_ES_LoadLibrary(&Id,
                                "filename",
                                "EntryPoint",
                                "LibName");
    UT_Report(Return == CFE_ES_ERR_LOAD_LIB &&
              OSPrintRtn.value == 50,
              "CFE_ES_LoadLibrary",
              "No free library slots",
              "03.026");

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    CFE_ES_Global.AppTable[0].StateRecord.AppTimer = 0;
    CFE_ES_ScanAppTable();
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID &&
              CFE_ES_Global.AppTable[0].StateRecord.AppTimer == 0,
              "CFE_ES_ScanAppTable",
              "Waiting; process control request",
              "03.027");

    /* Test scanning and acting on the application table where the timer
     * has not expired for a waiting application
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppTimer = 5;
    CFE_ES_ScanAppTable();
    UT_Report(CFE_ES_Global.AppTable[0].StateRecord.AppTimer == 4,
              "CFE_ES_ScanAppTable",
              "Decrement timer",
              "03.028");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_ScanAppTable();
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID &&
              CFE_ES_Global.AppTable[0].StateRecord.AppTimer > 0,
              "CFE_ES_ScanAppTable",
              "Stopped; process control request",
              "03.029");

    /* Test a control action request on an application with an
     * undefined control request state
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest = 0;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PCR_ERR2_EID,
              "CFE_ES_ProcessControlRequest",
              "Unknown state (default)",
              "03.030");

    /* Test a successful control action request to exit an application */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/Filename", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint,
            "NotNULL", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_EXIT_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application; successful",
              "03.031");

    /* Test a control action request to exit an application where the
     * request fails
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_EXIT_APP_ERR_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application failure",
              "03.032");

    /* Test a control action request to stop an application where the
     * request fails
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_DELETE;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_STOP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Stop application failure",
              "03.033");

    /* Test a control action request to restart an application where the
     * request fails due to a CleanUpApp error
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RESTART;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR4_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; CleanUpApp error",
              "03.034");

    /* Test a control action request to restart an application where the
     * request fails due to an AppCreate error
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RESTART;
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application failure; AppCreate error",
              "03.035");

    /* Test a control action request to reload an application where the
     * request fails due to a CleanUpApp error
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RELOAD;
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR4_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; CleanUpApp error",
              "03.036");

    /* Test a control action request to reload an application where the
     * request fails due to an AppCreate error
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RELOAD;
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR3_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application failure; AppCreate error",
              "03.037");

    /* Test a successful control action request to exit an application that
     * has an error
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest = CFE_ES_APP_ERROR;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERREXIT_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application on error; successful",
              "03.038");

    /* Test a control action request to exit an application that
     * has an error where the request fails
     */
    UT_InitData();
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest = CFE_ES_APP_ERROR;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERREXIT_APP_ERR_EID,
              "CFE_ES_ProcessControlRequest",
              "Exit application on error failure",
              "03.039");

    /* Test a successful control action request to stop an application */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_DELETE;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_STOP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Stop application; successful",
              "03.040");

    /* Test a successful control action request to restart an application */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RESTART;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Restart application; successful",
              "03.041");

    /* Test a successful control action request to reload an application */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_RELOAD;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_INF_EID,
              "CFE_ES_ProcessControlRequest",
              "Reload application; successful",
              "03.042");

    /* Test a control action request for an application that has an invalid
     * state (exception)
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.FileName,
            "/ram/FileName", OS_MAX_PATH_LEN);
    CFE_ES_Global.AppTable[0].StartParams.FileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.EntryPoint, "NULL",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.EntryPoint[OS_MAX_API_NAME - 1] =
        '\0';
    CFE_ES_Global.AppTable[0].StartParams.Priority = 255;
    CFE_ES_Global.AppTable[0].StartParams.StackSize = 8192;
    CFE_ES_Global.AppTable[0].StartParams.ExceptionAction = 0;
    CFE_ES_Global.AppTable[0].StateRecord.AppControlRequest =
        CFE_ES_SYS_EXCEPTION;
    CFE_ES_ProcessControlRequest(0);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PCR_ERR1_EID,
              "CFE_ES_ProcessControlRequest",
              "Invalid state",
              "03.043");

    /* Test listing the OS resources in use */
    UT_InitData();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);
    UT_Report(CFE_ES_ListResourcesDebug() == CFE_SUCCESS &&
              OSPrintRtn.count == 7,
              "CFE_ES_ListResourcesDebug",
              "List resources",
              "03.044");

    /* Test populating the application information structure with data */
    UT_InitData();
    UT_Report(CFE_ES_GetAppInfo(&AppInfo, 1) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Get application information; successful",
              "03.045");

    /* Test populating the application information structure with data using
     * a null application information pointer
     */
    UT_InitData();
    UT_Report(CFE_ES_GetAppInfo(NULL, 1) == CFE_ES_ERR_BUFFER,
              "CFE_ES_GetAppInfo",
              "Null application information pointer",
              "03.046");

    /* Test populating the application information structure using an
     * inactive application ID
     */
    UT_InitData();
    CFE_ES_Global.AppTable[5].RecordUsed = FALSE;
    UT_Report(CFE_ES_GetAppInfo(&AppInfo, 5) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID not active",
              "03.047");

    /* Test populating the application information structure using an
     * application ID value greater than the maximum allowed
     */
    UT_InitData();
    UT_Report(CFE_ES_GetAppInfo(&AppInfo,
                                CFE_ES_MAX_APPLICATIONS) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppInfo",
              "Application ID exceeds maximum",
              "03.048");

    /* Test populating the application information structure using a valid
     * application ID, but with a failure to retrieve the module information
     */
    UT_InitData();
    UT_SetRtnCode(&ModuleInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_GetAppInfo(&AppInfo, 1) == CFE_SUCCESS,
              "CFE_ES_GetAppInfo",
              "Module not found",
              "03.049");

    /* Test deleting an application and cleaning up its resources with OS
     * delete and close failures
     */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[0].AppId = 0;
    UT_SetOSFail(OS_TASKDELETE_FAIL | OS_CLOSE_FAIL);
    UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task OS delete and close failure",
              "03.050");

    /* Test deleting an application and cleaning up its resources with a
     * mutex delete failure
     */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[0].AppId = 0;
    UT_SetRtnCode(&MutSemDelRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Task mutex delete failure",
              "03.051");

    /* Test deleting an application and cleaning up its resources with a
     * failure to unload the module
     */
    UT_InitData();
    CFE_ES_Global.AppTable[0].StartParams.ModuleId = 0x12345678;
    UT_SetRtnCode(&ModuleUnloadRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "Module unload failure",
              "03.052");

    /* Test deleting an application and cleaning up its resources where the
     * EVS application cleanup fails
     */
    UT_InitData();
    UT_SetRtnCode(&EVSCleanUpRtn, -1, 1);
    UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
              "CFE_ES_CleanUpApp",
              "EVS application cleanup failure",
              "03.053");

    /* Test deleting an application and cleaning up its resources where the
     * TIME application cleanup fails
     */
  /* This test does not work since TimeCleanUp does not return anything
   * UT_InitData();
   * UT_SetRtnCode(&TIMECleanUpRtn, -1, 1);
   * UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
   *           "CFE_ES_CleanUpApp",
   *           "TIME application cleanup failure",
   *           "03.054");
   */

    /* Test deleting an application and cleaning up its resources where the
     * SB application cleanup fails
     */
  /* This test does not work since SBCleanUp does not return anything
   * UT_InitData();
   * UT_SetRtnCode(&SBCleanUpRtn, -1, 1);
   * UT_Report(CFE_ES_CleanUpApp(0) == CFE_ES_APP_CLEANUP_ERR,
   *           "CFE_ES_CleanUpApp",
   *           "SB application cleanup failure",
   *           "03.055");
   */

    /* Test cleaning up the OS resources for a task with a failure
     *  deleting mutexes
     */
    UT_InitData();
    UT_SetRtnCode(&MutSemDelRtn, OS_ERROR, 1);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanupTaskResources(1) == CFE_ES_MUT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Mutex delete failure",
              "03.056");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * binary semaphores
     */
    UT_InitData();
    UT_SetRtnCode(&BinSemDelRtn, OS_ERROR, 1);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_ES_BIN_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Binary semaphore delete failure",
              "03.057");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * counting semaphores
     */
    UT_InitData();
    UT_SetRtnCode(&CountSemDelRtn, OS_ERROR, 1);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_ES_COUNT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Counting semaphore failure",
              "03.058");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting queues
     */
    UT_InitData();
    UT_SetRtnCode(&QueueDelRtn, OS_ERROR, 1);
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_ES_QUEUE_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Queue delete failure",
              "03.059");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting timers
     */
    UT_InitData();
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_ES_TIMER_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Timer delete failure",
              "03.060");

    /* Test cleaning up the OS resources for a task with a failure
     * closing files
     */
    UT_InitData();
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_SetOSFail(OS_CLOSE_FAIL);
    UT_Report(CFE_ES_CleanupTaskResources(0) != CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "File close failure",
              "03.061");

    /* Test cleaning up the OS resources for a task with a failure
     * to delete the task
     */
    UT_InitData();
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_SetOSFail(OS_TASKDELETE_FAIL);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_ES_TASK_DELETE_ERR,
              "CFE_ES_CleanupTaskResources",
              "Task delete failure",
              "03.062");

    /* Test successfully cleaning up the OS resources for a task */
    UT_InitData();
    UT_SetRtnCode(&OSTimerGetInfoRtn, OS_ERROR, 1);
    UT_Report(CFE_ES_CleanupTaskResources(0) == CFE_SUCCESS,
              "CFE_ES_CleanupTaskResources",
              "Clean up task OS resources; successful",
              "03.063");
}

void TestERLog(void)
{
    int Return;
    uint32 Context = 4;
    char Context2[1000];

#ifdef UT_VERBOSE
    UT_Text("Begin Test Exception and Reset Log\n");
#endif

    /* Test initial rolling over log entry,
     * null description,
     * and non-null context with small size
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->ERLogIndex = CFE_ES_ER_LOG_ENTRIES + 1;
    CFE_ES_ResetDataPtr->ERLog[0].ContextIsPresent = FALSE;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_ES_POWERON_RESET,
                                 1,
                                 NULL,
                                 &Context,
                                 sizeof(int));
    UT_Report(Return == CFE_SUCCESS &&
              !strcmp(CFE_ES_ResetDataPtr->ERLog[0].Description,
                      "No Description String Given.") &&
              CFE_ES_ResetDataPtr->ERLogIndex == 1 &&
              CFE_ES_ResetDataPtr->ERLog[0].ContextIsPresent == TRUE,
              "CFE_ES_WriteToERLog",
              "Log entries exceeded; no description; valid context size",
              "04.001");

    /* Test rolling over log entry at end,
     * non-null description,
     * and non-null context with large size
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->ERLogIndex = CFE_ES_ER_LOG_ENTRIES - 1;
    CFE_ES_ResetDataPtr->ERLog[CFE_ES_ER_LOG_ENTRIES - 1].ContextIsPresent =
        FALSE;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_ES_POWERON_RESET,
                                 1,
                                 "LogDescription",
                                 (uint32 *) &Context2,
                                 9999999);
    UT_Report(Return == CFE_SUCCESS &&
              !strcmp(CFE_ES_ResetDataPtr->ERLog[
                          CFE_ES_ER_LOG_ENTRIES - 1].Description,
                      "LogDescription") &&
              CFE_ES_ResetDataPtr->ERLogIndex == 0 &&
              CFE_ES_ResetDataPtr->ERLog[
                  CFE_ES_ER_LOG_ENTRIES - 1].ContextIsPresent == TRUE,
              "CFE_ES_WriteToERLog",
              "Log entries at maximum; description; oversized context",
              "04.002");

    /* Test non-rolling over log entry,
     * null description,
     * and null context
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->ERLogIndex = 0;
    CFE_ES_ResetDataPtr->ERLog[0].ContextIsPresent = TRUE;
    Return = CFE_ES_WriteToERLog(CFE_ES_CORE_LOG_ENTRY,
                                 CFE_ES_POWERON_RESET,
                                 1,
                                 NULL,
                                 NULL,
                                 1);
    UT_Report(Return == CFE_SUCCESS &&
            CFE_ES_ResetDataPtr->ERLogIndex == 1 &&
            CFE_ES_ResetDataPtr->ERLog[0].ContextIsPresent == FALSE,
              "CFE_ES_WriteToERLog",
              "No log entry rollover; no description; no context",
              "04.003");
}

void TestShell(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Shell Command\n");
#endif

    /* Test shell output command using a non-existent ES command */
    UT_InitData();
    UT_Report(CFE_ES_ShellOutputCommand("ES_NoSuchThing",
                                        "./FS_Test_File_Name") ==
                  CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "Invalid ES command",
              "05.001");

    /* Test shell output command using a valid OS command */
    UT_InitData();
    UT_Report(CFE_ES_ShellOutputCommand("ls", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send OS command",
              "05.002");

    /* Test shell output command using an ES list applications command */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_Report(CFE_ES_ShellOutputCommand("ES_ListApps", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list applications command",
              "05.003");

    /* Test shell output command using an ES list tasks command */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, 0, 0);
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    UT_Report(CFE_ES_ShellOutputCommand("ES_ListTasks", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list tasks command",
              "05.004");
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;

    /* Test shell output command using an ES list resources command */
    UT_InitData();
    UT_Report(CFE_ES_ShellOutputCommand("ES_ListResources", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Send ES list resources command",
              "05.005");

    /* Test shell output command using a valid OS command but with a failed
     * OS lseek
     */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, CFE_ES_MAX_SHELL_PKT - 2, 1);
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(CFE_ES_ShellOutputCommand("ls", "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "OS command; OS lseek failure",
              "05.006");

    /* Test shell output command using a valid OS command to start sending
     * packets down
     */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, CFE_ES_MAX_SHELL_PKT * 2 + 1, 2);
    UT_Report(CFE_ES_ShellOutputCommand("ls", "") == CFE_SUCCESS,
              "CFE_ES_ShellOutputCommand",
              "Multiple packets to send down",
              "05.007");

    /* Test shell output command using a valid ES command but with a failed
     * OS create
     */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(CFE_ES_ShellOutputCommand("ES_ListApps",
                                        "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "ES command; OS create failure",
              "05.008");

    /* Test shell output command using a valid ES command but with a failed
     * OS lseek
     */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(CFE_ES_ShellOutputCommand("ES_ListApps",
                                        "") == CFE_ES_ERR_SHELL_CMD,
              "CFE_ES_ShellOutputCommand",
              "ES command; OS lseek failed",
              "05.009");
}

void TestTask(void)
{
    CFE_SB_MsgPtr_t             msgptr;
    CFE_ES_NoArgsCmd_t          NoArgsCmd;
    CFE_ES_RestartCmd_t         RestartCmd;
    CFE_ES_ShellCmd_t           ShellCmd;
    CFE_ES_StartAppCmd_t        StartAppCmd;
    CFE_ES_AppNameCmd_t         AppNameCmd;
    CFE_ES_AppReloadCmd_t       ReloadAppCmd;
    CFE_ES_QueryAllCmd_t        QueryAllCmd;
    CFE_ES_OverWriteSysLogCmd_t OverwriteSysLogCmd;
    CFE_ES_WriteSyslogCmd_t     WriteSyslogCmd;
    CFE_ES_WriteERlogCmd_t      WriteERlogCmd;
    CFE_ES_SetMaxPRCountCmd_t   SetMaxPRCountCmd;
    CFE_ES_DeleteCDSCmd_t       DeleteCDSCmd;
    CFE_ES_TlmPoolStatsCmd_t    TlmPoolStatsCmd;
    CFE_ES_DumpCDSRegCmd_t      DumpCDSRegCmd;
    CFE_ES_QueryAllTasksCmd_t   QueryAllTasksCmd;

    Pool_t *PoolPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Task\n");
#endif

    /* Reset the log index; CFE_ES_TaskMain() sets SystemLogMode to DISCARD,
     * which can result in a log overflow depending on the value that the
     * index has reached from previous tests
     */
    CFE_ES_ResetDataPtr->SystemLogIndex = 0;

    /* Test task main process loop with a command pipe error */
    UT_InitData();
    UT_RcvMsgId = 0xab;
    CFE_ES_TaskMain();
    UT_Report(OSPrintRtn.value == 40 && OSPrintRtn.count == -2,
              "CFE_ES_TaskMain",
              "Command pipe error",
              "06.001");

    /* Test task main process loop with an initialization failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    CFE_ES_TaskMain();
    UT_Report(OSPrintRtn.value == 41 + 42 && OSPrintRtn.count == -3,
              "CFE_ES_TaskMain",
              "Task initialization fail",
              "06.002");

    /* Test task main process loop with bad checksum information */
    UT_InitData();
    UT_SetRtnCode(&BSPGetCFETextRtn, -1, 1);
    UT_Report(CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.CFECoreChecksum == 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum fail",
              "06.003");

    /* Test successful task main process loop */
    UT_InitData();
    UT_Report(CFE_ES_TaskInit() == CFE_SUCCESS &&
              CFE_ES_TaskData.HkPacket.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit",
              "Checksum success",
              "06.004");

    /* Test task main process loop with a register app failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    UT_Report(CFE_ES_TaskInit() == CFE_ES_ERR_APP_REGISTER,
             "CFE_ES_TaskInit",
              "Register application fail",
              "06.005");

    /* Test task main process loop with a with an EVS register failure */
    UT_InitData();
    UT_SetRtnCode(&EVS_RegisterRtn, -1, 1);
    UT_Report(CFE_ES_TaskInit() == -1,
              "CFE_ES_TaskInit",
              "EVS register fail",
              "06.006");

    /* Test task main process loop with a SB pipe create failure */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -2, 1);
    UT_Report(CFE_ES_TaskInit() == -2,
              "CFE_ES_TaskInit",
              "SB pipe create fail",
              "06.007");

    /* Test task main process loop with a HK packet subscribe failure */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -3, 1);
    UT_Report(CFE_ES_TaskInit() == -3,
              "CFE_ES_TaskInit",
              "HK packet subscribe fail",
              "06.008");

    /* Test task main process loop with a ground command subscribe failure */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -4, 2);
    UT_Report(CFE_ES_TaskInit() == -4,
              "CFE_ES_TaskInit",
              "Ground command subscribe fail",
              "06.009");

    /* Test task main process loop with an init event send failure */
    UT_InitData();
    UT_SetRtnCode(&EVS_SendEventRtn, -5, 1);
    UT_Report(CFE_ES_TaskInit() == -5,
              "CFE_ES_TaskInit",
              "Initialization event send fail",
              "06.010");

    /* Test task main process loop with version event send failure */
    UT_InitData();
    UT_SetRtnCode(&EVS_SendEventRtn, -6, 2);
    UT_Report(CFE_ES_TaskInit() == -6,
              "CFE_ES_TaskInit",
              "Version event send fail",
              "06.011");

    /* Test a successful HK request */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_SEND_HK_MID, 0);
    UT_Report(CFE_ES_TaskData.HkPacket.HeapBytesFree > 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap successful",
              "06.012");

    /* Test the HK request with a get heap failure */
    UT_InitData();
    UT_SetRtnCode(&HeapGetInfoRtn, -1, 1);
    UT_SendMsg(msgptr, CFE_ES_SEND_HK_MID, 0);
    UT_Report(CFE_ES_TaskData.HkPacket.HeapBytesFree == 0,
              "CFE_ES_HousekeepingCmd",
              "HK packet - get heap fail",
              "06.013");

    /* Test successful no-op command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_NOOP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_NOOP_INF_EID,
              "CFE_ES_NoopCmd",
              "No-op",
              "06.014");

    /* Test successful reset counters command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESET_INF_EID,
              "CFE_ES_ResetCmd",
              "Reset counters",
              "06.015");

    /* Test successful cFE restart */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_RestartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &RestartCmd;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    RestartCmd.RestartType = CFE_ES_PROCESSOR_RESET;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(PSPRestartRtn.value == CFE_ES_PROCESSOR_RESET &&
              PSPRestartRtn.count == 1,
              "CFE_ES_RestartCmd",
              "Restart cFE",
              "06.016");

    /* Test cFE restart with bad restart type */
    UT_InitData();
    RestartCmd.RestartType = 4524;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_BOOT_ERR_EID,
              "CFE_ES_RestartCmd",
              "Invalid restart type",
              "06.017");

    /* Test shell command failure */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_ShellCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &ShellCmd;
    strncpy((char *) ShellCmd.CmdString, "ES_NOAPP", CFE_ES_MAX_SHELL_CMD);
    ShellCmd.CmdString[CFE_ES_MAX_SHELL_CMD - 1] = '\0';
    ShellCmd.OutputFilename[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SHELL_CMD_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SHELL_ERR_EID,
              "CFE_ES_ShellCmd",
              "Shell command fail",
              "06.018");

    /* Test successful shell command */
    UT_InitData();
    strncpy((char *) ShellCmd.CmdString, "ls", CFE_ES_MAX_SHELL_CMD);
    ShellCmd.CmdString[CFE_ES_MAX_SHELL_CMD - 1] = '\0';
    ShellCmd.OutputFilename[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SHELL_CMD_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SHELL_INF_EID,
              "CFE_ES_ShellCmd",
              "Shell command success",
              "06.019");

    /* Test successful app create */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_StartAppCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &StartAppCmd;
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "entrypoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 160;
    StartAppCmd.StackSize = 8192;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_INF_EID,
              "CFE_ES_StartAppCmd",
              "Start application from file name",
              "06.020");

    /* Test app create with an OS task create failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Start application from file name fail",
              "06.021");

    /* Test app create with the file name too short */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "123", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "entrypoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 160;
    StartAppCmd.StackSize = 12096;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_INVALID_FILENAME_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Invalid file name",
              "06.022");

    /* Test app create with with an invalid exception action */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "entrypoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 160;
    StartAppCmd.StackSize = 12096;
    StartAppCmd.ExceptionAction = 65534;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_EXC_ACTION_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Invalid exception action",
              "06.023");

    /* Test app create with a bad stack size */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "entrypoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 160;
    StartAppCmd.StackSize = 0;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_STACK_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Stack size too small",
              "06.024");

    /* Test app create with a bad priority */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "entrypoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 1000;
    StartAppCmd.StackSize = 12096;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_PRIORITY_ERR_EID,
              "CFE_ES_StartAppCmd",
              "Priority is too large",
              "06.025");

/*
 * Note: The ES StartApp command checks to see if the 
 * AppFileName and AppEntryPoint are NULL.
 *  This is not really a valid comparison for a string. When the 
 *  code is fixed, then these unit tests can be uncommented
 *  allowing 100% coverage in cfe_es_task.c
 */
#if 0
    /* Test app create with a bad entry point */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "\0", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "appName", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 100;
    StartAppCmd.StackSize = 12096;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID,
              "CFE_ES_StartAppCmd",
              "AppEntryPoint null string",
              "06.025");

    /* Test app create with a bad App name */
    UT_InitData();
    strncpy((char *) StartAppCmd.AppFileName, "filename", OS_MAX_PATH_LEN);
    StartAppCmd.AppFileName[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy((char *) StartAppCmd.AppEntryPoint, "AppEntryPoint", OS_MAX_API_NAME);
    StartAppCmd.AppEntryPoint[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) StartAppCmd.Application, "\0", OS_MAX_API_NAME);
    StartAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    StartAppCmd.Priority = 100;
    StartAppCmd.StackSize = 12096;
    StartAppCmd.ExceptionAction = CFE_ES_APP_EXCEPTION_RESTART_APP;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_START_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_START_NULL_APP_NAME_ERR_EID,
              "CFE_ES_StartAppCmd",
              "App Name null string",
              "06.025");
#endif

    /* Test successful app stop */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    strncpy((char *) AppNameCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_STOP_DBG_EID,
              "CFE_ES_StopAppCmd",
              "Stop application initiated",
              "06.026");

    /* Test app stop failure */
    UT_InitData();
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_STOP_ERR1_EID,
              "CFE_ES_StopAppCmd",
              "Stop application failed",
              "06.027");

    /* Test app stop with a bad app name */
    UT_InitData();
    strncpy((char *) AppNameCmd.Application, "BAD_APP_NAME", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_STOP_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_STOP_ERR2_EID,
              "CFE_ES_StopAppCmd",
              "Stop application bad name",
              "06.028");

    /* Test successful app restart */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    strncpy((char *) AppNameCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name,
            "CFE_ES", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_DBG_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application initiated",
              "06.029");

    /* Test app restart with a bad app name */
    UT_InitData();
    strncpy((char *) AppNameCmd.Application, "BAD_APP_NAME", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR2_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application bad name",
              "06.030");

    /* Test failed app restart */
    UT_InitData();
    strncpy((char *) AppNameCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name,
            "CFE_ES", OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESTART_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESTART_APP_ERR1_EID,
              "CFE_ES_RestartAppCmd",
              "Restart application failed",
              "06.031");

    /* Test successful app reload */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &ReloadAppCmd;
    strncpy((char *) ReloadAppCmd.AppFileName, "New_Name", OS_MAX_API_NAME);
    ReloadAppCmd.AppFileName[OS_MAX_API_NAME - 1] = '\0';
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppReloadCmd_t));
    strncpy((char *) ReloadAppCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    ReloadAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_DBG_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application initiated",
              "06.032");

    /* Test app reload with a bad app name */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    strncpy((char *) ReloadAppCmd.Application, "BAD_APP_NAME",
            OS_MAX_API_NAME);
    ReloadAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR2_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application bad name",
              "06.033");
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;

    /* Test failed app reload */
    UT_InitData();
    CFE_ES_Global.AppTable[0].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_Global.AppTable[0].StateRecord.AppState = CFE_ES_APP_STATE_WAITING;
    strncpy((char *) ReloadAppCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    ReloadAppCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RELOAD_APP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RELOAD_APP_ERR1_EID,
              "CFE_ES_ReloadAppCmd",
              "Reload application failed",
              "06.034");

    /* Test successful telemetry packet request for single app data */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &AppNameCmd;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_AppNameCmd_t));
    strncpy((char *) AppNameCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ONE_APP_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - success",
              "06.035");

    /* Test telemetry packet request for single app data with failure of
     * CFE_SB_SendMsg; SBSendMsgRtn count must be set to 2 in order to
     * allow the command message through, but then fail sending the
     * application status telemetry packet message
     */
    UT_InitData();
    UT_SetRtnCode(&SBSendMsgRtn, -1, 2);
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) AppNameCmd.Application, "CFE_ES", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ONE_ERR_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - SB send message fail",
              "06.036");

    /* Test telemetry packet request for single app data with a bad app name */
    UT_InitData();
    strncpy((char *) AppNameCmd.Application, "BAD_APP_NAME", OS_MAX_API_NAME);
    AppNameCmd.Application[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ONE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ONE_APPID_ERR_EID,
              "CFE_ES_QueryOneCmd",
              "Query application - bad application name",
              "06.037");

    /* Test successful write of all app data to file */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_QueryAllCmd_t));
    strncpy((char *) QueryAllCmd.QueryAllFileName, "AllFilename",
            OS_MAX_PATH_LEN);
    QueryAllCmd.QueryAllFileName[OS_MAX_PATH_LEN - 1] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ALL_APPS_EID,
              "CFE_ES_QueryAllCmd",
              "Query all applications - success",
              "06.038");

    /* Test write of all app data to file with a null file name */
    UT_InitData();
    QueryAllCmd.QueryAllFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ALL_APPS_EID,
              "CFE_ES_QueryAllCmd",
              "Query all applications - null file name",
              "06.039");

    /* Test write of all app data to file with a write header failure */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_WRHDR_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; write header",
              "06.040");

    /* Test write of all app data to file with a file write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TASKWR_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; task write",
              "06.041");

    /* Test write of all app data to file with a file create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_OSCREATE_ERR_EID,
              "CFE_ES_QueryAllCmd",
              "Write application information file fail; OS create",
              "06.042");
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;

    /* Test successful write of all task data to a file */
    UT_InitData();
    QueryAllTasksCmd.QueryAllFileName[0] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &QueryAllTasksCmd;
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TASKINFO_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file written",
              "06.043");
    CFE_ES_Global.TaskTable[0].RecordUsed = FALSE;
    strncpy((char *) QueryAllTasksCmd.QueryAllFileName, "filename",
            OS_MAX_PATH_LEN);
    QueryAllTasksCmd.QueryAllFileName[OS_MAX_PATH_LEN - 1] = '\0';

    /* Test write of all task data to a file with write header failure */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TASKINFO_WRHDR_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; write header",
              "06.044");

    /* Test write of all task data to a file with a task write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TASKINFO_WR_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; task write",
              "06.045");
    CFE_ES_Global.TaskTable[0].RecordUsed = FALSE;

    /* Test write of all task data to a file with an OS create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_QUERY_ALL_TASKS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TASKINFO_OSCREATE_ERR_EID,
              "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; OS create",
              "06.046");

    /* Test successful clearing of the system log */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SYSLOG1_INF_EID,
              "CFE_ES_ClearSyslogCmd",
              "Clear ES log data",
              "06.047");

    /* Test successful overwriting of the system log */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_OverWriteSysLogCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &OverwriteSysLogCmd;
    OverwriteSysLogCmd.Mode = CFE_ES_LOG_OVERWRITE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SYSLOGMODE_EID,
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log received",
              "06.048");

    /* Test overwriting the system log using an invalid mode */
    UT_InitData();
    OverwriteSysLogCmd.Mode = 9342;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_OVERWRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERR_SYSLOGMODE_EID,
              "CFE_ES_OverWriteSyslogCmd",
              "Overwrite system log using invalid mode",
              "06.049");

    /* Test successful writing of the system log */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_WriteSyslogCmd_t));
    strncpy(WriteSyslogCmd.SysLogFileName, "filename", OS_MAX_PATH_LEN);
    WriteSyslogCmd.SysLogFileName[OS_MAX_PATH_LEN - 1] = '\0';
    msgptr = (CFE_SB_MsgPtr_t) &WriteSyslogCmd;
    CFE_ES_TaskData.HkPacket.SysLogEntries = 123;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; success",
              "06.050");

    /* Test writing the system log using a null file name */
    UT_InitData();
    WriteSyslogCmd.SysLogFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; null file name",
              "06.051");

    /* Test writing the system log with an OS create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    WriteSyslogCmd.SysLogFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SYSLOG2_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS create",
              "06.052");

    /* Test writing the system log with an OS write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    WriteSyslogCmd.SysLogFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; OS write",
              "06.053");

    /* Test writing the system log with a write header failure */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_SYSLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteSyslogCmd",
              "Write system log; write header",
              "06.054");

    /* Test successful clearing of the E&R log */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERLOG1_INF_EID,
              "CFE_ES_ClearERlogCmd",
              "Clear E&R log",
              "06.055");

    /* Test successful writing of the E&R log */
    UT_InitData();
    strncpy(WriteERlogCmd.ERLogFileName, "filename", OS_MAX_PATH_LEN);
    WriteERlogCmd.ERLogFileName[OS_MAX_PATH_LEN - 1] = '\0';
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_WriteERlogCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &WriteERlogCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERLOG2_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; success",
              "06.056");

    /* Test writing the E&R log with an OS create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    WriteERlogCmd.ERLogFileName[0] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_ERLOG2_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; OS create",
              "06.057");

    /* Test writing the E&R log with an OS write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    WriteERlogCmd.ERLogFileName[0] = 'n';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; OS write",
              "06.058");

    /* Test writing the E&R log with a write header failure */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, OS_FS_ERROR, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_FILEWRITE_ERR_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; write header",
              "06.059");

    /* Test writing the E&R log with a reset area failure */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_TONE_PRI);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_WRITE_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RST_ACCESS_EID,
              "CFE_ES_WriteERlogCmd",
              "Write E&R log; reset area",
              "06.060");

    /* Test clearing the log with a bad size in the verify command
     * length call
     */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_TONE_PRI);
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_SetMaxPRCountCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_CLEAR_ERLOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_LEN_ERR_EID,
              "CFE_ES_ClearERlogCmd",
              "Packet length error",
              "06.061");

    /* Test resetting and setting the max for the processor reset count */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_NoArgsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_RESET_PR_COUNT_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_RESET_PR_COUNT_EID,
              "CFE_ES_ResetPRCountCmd",
              "Set processor reset count to zero",
              "06.062");

    /* Test setting the maximum processor reset count */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_SetMaxPRCountCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &SetMaxPRCountCmd;
    SetMaxPRCountCmd.MaxPRCount = 3;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_SET_MAX_PR_COUNT_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_SET_MAX_PR_COUNT_EID,
              "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count",
              "06.063");

    /* Test failed deletion of specified CDS */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_DeleteCDSCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &DeleteCDSCmd;
    strncpy(DeleteCDSCmd.CdsName, "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    DeleteCDSCmd.CdsName[OS_MAX_API_NAME - 1] = '\0';
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_DELETE_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; error",
              "06.064");

    /* Test failed deletion of specified critical table CDS */
    UT_InitData();
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_DELETE_TBL_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; wrong type",
              "06.065");
    CFE_ES_Global.CDSVars.Registry[0].Table = FALSE;

    /* Test successful deletion of a specified CDS */
    UT_InitData();
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);

    /* Set up the block to read what we need to from the CDS */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_DELETED_INFO_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; success",
              "06.066");

    /* Test deletion of a specified CDS with the owning app being active */
    UT_InitData();
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.CDSVars.Registry[0].Table = FALSE;
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_OWNER_ACTIVE_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; owner active",
              "06.067");

    /* Test deletion of a specified CDS with the name not found */
    UT_InitData();
    CFE_ES_Global.CDSVars.Registry[0].MemHandle =
        sizeof(CFE_ES_Global.CDSVars.ValidityField);
    UT_SetRtnCode(&BSPReadCDSRtn, 0, 1);
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_BAD",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    CFE_ES_Global.CDSVars.Registry[0].Taken = FALSE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DELETE_CDS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_NAME_ERR_EID,
              "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; not found",
              "06.068");

    /* Test successful dump of CDS to file */
    UT_InitData();
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_DumpCDSRegCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &DumpCDSRegCmd;
    DumpCDSRegCmd.DumpFilename[0] = '\0';
    CFE_ES_Global.CDSVars.Registry[0]. Taken = TRUE;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_REG_DUMP_INF_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; success",
              "06.069");

    /* Test dumping of the CDS to a file with a bad FS write header */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_WRITE_CFE_HDR_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; write header",
              "06.070");

    /* Test dumping of the CDS to a file with an OS create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CREATING_CDS_DUMP_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; OS create",
              "06.071");

    /* Test dumping of the CDS to a file with an OS write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CDS_DUMP_ERR_EID,
              "CFE_ES_DumpCDSRegCmd",
              "Dump CDS; OS write",
              "06.072");

    /* Test telemetry pool statistics retrieval with an invalid handle */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_TlmPoolStatsCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &TlmPoolStatsCmd;
    PoolPtr = (Pool_t *) &TlmPoolStatsCmd.PoolHandle ;
    PoolPtr->Start = &TlmPoolStatsCmd.PoolHandle;
    PoolPtr->Size = 64;
    PoolPtr->End = *PoolPtr->Start + 0;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_TLM_POOL_STATS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_INVALID_POOL_HANDLE_ERR_EID,
              "CFE_ES_TlmPoolStatsCmd",
              "Telemetry pool; bad handle",
              "06.073");

    /* Test successful telemetry pool statistics retrieval */
    UT_InitData();
    PoolPtr = (Pool_t *) &TlmPoolStatsCmd.PoolHandle ;
    PoolPtr->Start = &TlmPoolStatsCmd.PoolHandle;
    PoolPtr->Size = 64;
    PoolPtr->End = *PoolPtr->Start + PoolPtr->Size;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_TLM_POOL_STATS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_TLM_POOL_STATS_INFO_EID,
              "CFE_ES_TlmPoolStatsCmd",
              "Telemetry pool; success",
              "06.074");

    /* Test the command pipe message process with an invalid command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_DUMP_CDS_REG_CC + 2);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_CC1_ERR_EID,
              "CFE_ES_TaskPipe",
              "Invalid ground command",
              "06.075");
} /* end TestTask */

void TestPerf(void)
{
    CFE_SB_MsgPtr_t msgptr;
    CFE_ES_PerfStartCmd_t PerfStartCmd;
    CFE_ES_PerfStopCmd_t PerfStopCmd;
    CFE_ES_PerfSetFilterMaskCmd_t PerfSetFilterMaskCmd;
    CFE_ES_PerfSetTrigMaskCmd_t PerfSetTrigMaskCmd;

    extern CFE_ES_PerfLogDump_t CFE_ES_PerfLogDumpStatus;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Performance Log\n");
#endif

    /* Test successful performance mask and value initialization */
    UT_InitData();
    Perf->MetaData.State = CFE_ES_PERF_MAX_STATES;
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 0;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId = 1;
    CFE_ES_SetupPerfVariables(CFE_ES_PROCESSOR_RESET);
    UT_Report(Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_SetupPerfVariables",
              "Idle data collection",
              "07.001");

    /* Test successful performance data collection start in START
     * trigger mode
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode START",
              "07.002");

    /* Test successful performance data collection start in CENTER
     * trigger mode
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = CFE_ES_PERF_TRIGGER_CENTER;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode CENTER",
              "07.003");

    /* Test successful performance data collection start in END
     * trigger mode
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = CFE_ES_PERF_TRIGGER_END;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Collect performance data; mode END",
              "07.004");

    /* Test performance data collection start with an invalid trigger mode
     * (too high)
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = (CFE_ES_PERF_TRIGGER_END + 1);
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_TRIG_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Trigger mode out of range (high)",
              "07.005");

    /* Test performance data collection start with an invalid trigger mode
     * (too low)
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = 0xffffffff;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_TRIG_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Trigger mode out of range (low)",
              "07.006");

    /* Test performance data collection start with a file write in progress */
    UT_InitData();
    CFE_ES_PerfLogDumpStatus.DataToWrite = 1;
    PerfStartCmd.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_ERR_EID,
              "CFE_ES_PerfStartDataCmd",
              "Cannot collect performance data; write in progress",
              "07.007");
    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;

    /* Test performance data collection by sending another valid
     * start command
     */
    UT_InitData();
    PerfStartCmd.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStartCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfStartCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STARTDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STARTCMD_EID,
              "CFE_ES_PerfStartDataCmd",
              "Start collecting performance data",
              "07.008");

    /* Test successful performance data collection stop */
    UT_InitData();
    PerfStopCmd.DataFileName[0] = '\0';
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfStopCmd_t));
    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
    msgptr = (CFE_SB_MsgPtr_t) &PerfStopCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop collecting performance data",
              "07.009");

    /* Test performance data collection stop with an OS create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_LOG_ERR_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command; OS create fail",
              "07.010");

    /* Test performance data collection stop with an OS task create failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_ERR1_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command; OS task create fail",
              "07.011");

    /* Test successful performance data collection stop with a non-default
         file name */
    UT_InitData();
    strncpy(PerfStopCmd.DataFileName, "filename", OS_MAX_PATH_LEN);
    PerfStopCmd.DataFileName[OS_MAX_PATH_LEN - 1] = '\0';
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop collecting performance data (non-default file name)",
              "07.012");

    /* Test performance data collection stop with a file write in progress */
    UT_InitData();
    CFE_ES_PerfLogDumpStatus.DataToWrite = 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_STOPDATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_STOPCMD_ERR2_EID,
              "CFE_ES_PerfStopDataCmd",
              "Stop performance data command ignored",
              "07.013");

    /* Test performance filter mask command with out of range filter
         mask value */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetFilterMaskCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetFilterMaskCmd;
    PerfSetFilterMaskCmd.FilterMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETFILTERMASK_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_FILTMSKERR_EID,
              "CFE_ES_PerfSetFilterMaskCmd",
              "Performance filter mask command error; index out of range",
              "07.014");

    /* Test successful performance filter mask command */
    UT_InitData();
    PerfSetFilterMaskCmd.FilterMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK / 2;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETFILTERMASK_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_FILTMSKCMD_EID,
              "CFE_ES_PerfSetFilterMaskCmd",
              "Set performance filter mask command received",
              "07.015");

    /* Test successful performance filter mask command with minimum filter
         mask value */
    UT_InitData();
    PerfSetTrigMaskCmd.TriggerMaskNum = 0;
    UT_SetSBTotalMsgLen(sizeof(CFE_ES_PerfSetTrigMaskCmd_t));
    msgptr = (CFE_SB_MsgPtr_t) &PerfSetTrigMaskCmd;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKCMD_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Set performance trigger mask command received; minimum index",
              "07.016");

    /* Test successful performance filter mask command with maximum filter
     * mask value
     */
    UT_InitData();
    PerfSetTrigMaskCmd.TriggerMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK - 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKCMD_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Set performance trigger mask command received; maximum index",
              "07.017");

    UT_InitData();
    PerfSetTrigMaskCmd.TriggerMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK + 1;
    UT_SendMsg(msgptr, CFE_ES_CMD_MID, CFE_ES_PERF_SETTRIGMASK_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_TRIGMSKERR_EID,
              "CFE_ES_PerfSetTriggerMaskCmd",
              "Performance trigger mask command error; index out of range",
              "07.018");

    /* Test successful performance log dump */
    UT_InitData();
    CFE_ES_PerfLogDump();
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_DATAWRITTEN_EID &&
              OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 1,
              "CFE_ES_PerfLogDump",
              "Performance log dump; success",
              "07.019");

    /* Test performance log dump with OS create and write failures */
    UT_InitData();
    Perf->MetaData.DataCount = 7;
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    UT_SetOSFail(OS_CREAT_FAIL | OS_WRITE_FAIL);
    CFE_ES_PerfLogDump();
    UT_Report(SendMsgEventIDRtn.value == CFE_ES_PERF_DATAWRITTEN_EID &&
              OSCloseRtn.value == OS_FS_SUCCESS && OSCloseRtn.count == 10,
              "CFE_ES_PerfLogDump",
              "Performance log dump; OS create & write failed",
              "07.020");

    /* Test successful addition of a new entry to the performance log */
    UT_InitData();
    Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerCount = CFE_ES_PERF_DATA_BUFFER_SIZE +1;
    Perf->MetaData.InvalidMarkerReported = FALSE;
    Perf->MetaData.DataEnd = CFE_ES_PERF_DATA_BUFFER_SIZE +1 ;
    CFE_ES_PerfLogAdd(CFE_ES_PERF_MAX_IDS, 0);
    UT_Report(Perf->MetaData.InvalidMarkerReported == TRUE,
              "CFE_ES_PerfLogAdd",
              "Invalid performance marker",
              "07.021");

    /* Test addition of a new entry to the performance log with START
     * trigger mode
     */
    UT_InitData();
    Perf->MetaData.InvalidMarkerReported = TRUE;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.DataCount = CFE_ES_PERF_DATA_BUFFER_SIZE + 1;
    Perf->MetaData.TriggerMask[0] = 0xFFFF;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; START",
              "07.022");

    /* Test addition of a new entry to the performance log with CENTER
     * trigger mode
     */
    UT_InitData();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_CENTER;
    Perf->MetaData.TriggerCount = CFE_ES_PERF_DATA_BUFFER_SIZE / 2 + 1 ;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; CENTER",
              "07.023");

    /* Test addition of a new entry to the performance log with END
     * trigger mode
     */
    UT_InitData();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_END;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END &&
              Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd",
              "Triggered; END",
              "07.024");
}

void TestAPI(void)
{
    char AppName[32];
    char CounterName[11];
    int i;
    int32 Return;
    uint8 Data[12];
    uint32 ResetType;
    uint32 AppId;
    uint32 TaskId;
    uint32 TempSize;
    uint32 RunStatus;
    uint32 CounterId;
    uint32 CounterCount;
    CFE_ES_CDSHandle_t CDSHandle;
    CFE_ES_TaskInfo_t TaskInfo;

#ifdef UT_VERBOSE
    UT_Text("Begin Test API\n");
#endif

    /* Test resetting the cFE with a processor reset */
    UT_InitData();
    ResetType = CFE_ES_PROCESSOR_RESET;
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount - 1;
    CFE_ES_ResetCFE(ResetType);
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount;
    UT_Report(CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
              OSPrintRtn.value == 12 + 15 && OSPrintRtn.count == -3,
              "CFE_ES_ResetCFE",
              "Processor reset",
              "08.001");

    /* Test getting the reset type */
    UT_InitData();
    UT_Report(CFE_ES_GetResetType(&ResetType) == CFE_ES_PROCESSOR_RESET,
              "CFE_ES_GetResetType",
              "Get reset type successful",
              "08.002");

    /* Test resetting the cFE with a power on reset */
    UT_InitData();
    ResetType = CFE_ES_POWERON_RESET;
    UT_Report(CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
              OSPrintRtn.value == 16 && OSPrintRtn.count == -2,
              "CFE_ES_ResetCFE",
              "Power on reset",
              "08.003");

    /* Test resetting the cFE with an invalid argument */
    UT_InitData();
    ResetType = CFE_ES_POWERON_RESET + 3;
    UT_Report(CFE_ES_ResetCFE(ResetType) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ResetCFE",
              "Bad reset type",
              "08.004");

    /* Test restarting an app that doesn't exist */
    UT_InitData();
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(CFE_ES_RestartApp(1) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Bad application ID",
              "08.005");

    /* Test restarting an app with an ID out of range (high) */
    UT_Report(CFE_ES_RestartApp(40000) == CFE_ES_ERR_APPID,
              "CFE_ES_RestartApp",
              "Application ID too large",
              "08.006");

    /* Test reloading an app that doesn't exist */
    UT_InitData();
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(CFE_ES_ReloadApp(1, "filename") == CFE_ES_ERR_APPID,
              "CFE_ES_ReloadApp",
              "Bad application ID",
              "08.007");

    /* Test deleting an app that doesn't exist */
    UT_InitData();
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    UT_Report(CFE_ES_DeleteApp(1) == CFE_ES_ERR_APPID,
              "CFE_ES_DeleteApp",
              "Bad application ID",
              "08.008");

    /* Test exiting an app with an init error */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(CFE_ES_CORE_APP_INIT_ERROR);
    UT_Report(OSPrintRtn.value == 11 + 12 && OSPrintRtn.count == -3,
              "CFE_ES_ExitApp",
              "Application initialization error",
              "08.009");

    /* Test exiting an app with a runtime error */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(CFE_ES_CORE_APP_RUNTIME_ERROR);
    UT_Report(OSPrintRtn.value == 13 && OSPrintRtn.count == -2,
              "CFE_ES_ExitApp",
              "Application runtime error",
              "08.010");

    /* Test exiting an app with an exit error */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_STOPPED;
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_CORE;
    CFE_ES_ExitApp(1000);
    UT_Report(OSPrintRtn.value == 14 && OSPrintRtn.count == -2,
              "CFE_ES_ExitApp",
              "Application exit error",
              "08.011");

    /* Can't cover this path since it contains a while(1) (i.e.,
     * infinite) loop
     */
#if 0
    CFE_ES_Global.TaskTable[1].RecordUsed =TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].Type = CFE_ES_APP_TYPE_EXTERNAL;
    CFE_ES_Global.AppTable[1].StateRecord.AppState = CFE_ES_APP_STATE_RUNNING;
    CFE_ES_ExitApp(CFE_ES_CORE_APP_RUNTIME_ERROR);
#endif

    /* Test successful run loop app run request */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = CFE_ES_APP_RUN;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_RUN;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == TRUE,
              "CFE_ES_RunLoop",
              "Request to run application",
              "08.012");

    /* Test successful run loop app stop request */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = CFE_ES_APP_RUN;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Request to stop running application",
              "08.013");

    /* Test successful run loop app exit request */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = CFE_ES_APP_EXIT;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Request to exit application",
              "08.014");

    /* Test run loop with bad app ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = CFE_ES_APP_EXIT;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Bad internal application ID",
              "08.015");

    /* Test run loop with an invalid run status */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = 1000;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_EXIT;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == FALSE,
              "CFE_ES_RunLoop",
              "Invalid run status",
              "08.016");

    /* Test run loop with startup sync code */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    RunStatus = CFE_ES_APP_RUN;
    CFE_ES_Global.AppTable[1].StateRecord.AppControlRequest = CFE_ES_APP_RUN;
    CFE_ES_Global.AppTable[1].StateRecord.AppState =
        CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_Global.StartupSemaphoreReleased = FALSE;
    CFE_ES_Global.StartupFileComplete = TRUE;
    CFE_ES_Global.AppStartupCounter = -1;
    UT_Report(CFE_ES_RunLoop(&RunStatus) == TRUE &&
              CFE_ES_Global.StartupSemaphoreReleased == TRUE,
              "CFE_ES_RunLoop",
              "Status change from initializing to run",
              "08.017");

    /* Test successful CFE application registration */
    UT_InitData();
    UT_Report(CFE_ES_RegisterApp() == CFE_SUCCESS,
              "CFE_ES_RegisterApp",
              "Application registration successful",
              "08.018");

    /* Test getting the cFE application ID by its name */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    UT_Report(CFE_ES_GetAppID(&AppId) == CFE_SUCCESS,
              "CFE_ES_GetAppID",
              "Get application ID by name successful",
              "08.019");

    /* Test getting the app name with a bad app ID */
    CFE_ES_Global.AppTable[4].RecordUsed = FALSE;
    UT_InitData();
    UT_Report(CFE_ES_GetAppName(AppName, 4, 32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; bad application ID",
              "08.020");

    /* Test getting the app name with that app ID out of range */
    UT_InitData();
    UT_Report(CFE_ES_GetAppName(AppName,
                                CFE_ES_MAX_APPLICATIONS + 2,
                                32) == CFE_ES_ERR_APPID,
              "CFE_ES_GetAppName",
              "Get application name by ID; ID out of range",
              "08.021");

    /* Test successfully getting the app name using the app ID */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_Report(CFE_ES_GetAppName(AppName, 0, 32) == CFE_SUCCESS,
              "CFE_ES_GetAppName",
              "Get application name by ID successful",
              "08.022");

    /* Test getting task information using the task ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    UT_Report(CFE_ES_GetTaskInfo(&TaskInfo, 1) == CFE_SUCCESS,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID successful",
              "08.023");

    /* Test getting task information using the task ID with parent inactive */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].RecordUsed = FALSE;
    UT_Report(CFE_ES_GetTaskInfo(&TaskInfo, 1) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; parent application not active",
              "08.024");

    /* Test getting task information using the task ID with task inactive */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_Report(CFE_ES_GetTaskInfo(&TaskInfo, 1) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; task not active",
              "08.025");

    /* Test getting task information using the task ID with invalid task ID */
    UT_InitData();
    UT_Report(CFE_ES_GetTaskInfo(&TaskInfo, 1000) == CFE_ES_ERR_TASKID,
              "CFE_ES_GetTaskInfo",
              "Get task info by ID; invalid task ID",
              "08.026");

    /* Test creating a child task with a bad app ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_ERR_APPID,
              "CFE_ES_ChildTaskCreate",
              "Bad application ID",
              "08.027");

    /* Test creating a child task with an OS task create failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_Task[1].free = TRUE;
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_ChildTaskCreate",
              "OS task create failed",
              "08.028");

    /* Test creating a child task with a null task ID */
    UT_InitData();
    Return = CFE_ES_CreateChildTask(NULL,
                                    "TaskName",
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task ID null",
              "08.029");

    /* Test creating a child task with a null task name */
    UT_InitData();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    NULL,
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name null",
              "08.030");

    /* Test creating a child task with a null task ID and name */
    UT_InitData();
    Return = CFE_ES_CreateChildTask(NULL,
                                    NULL,
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Task name and ID null",
              "08.031");

    /* Test creating a child task with a null function pointer */
    UT_InitData();
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    NULL,
                                    (const uint32*) AppName,
                                    32,
                                    2,
                                    0);
    UT_Report(Return == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_ChildTaskCreate",
              "Function pointer null",
              "08.032");

    /* Test creating a child task within a child task */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 3;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_ES_ERR_CHILD_TASK_CREATE,
              "CFE_ES_CreateChildTask",
              "Cannot call from a child task",
              "08.033");

    /* Test successfully creating a child task */
    UT_InitData();
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 1;
    Return = CFE_ES_CreateChildTask(&TaskId,
                                    "TaskName",
                                    TestAPI,
                                    (const uint32*) AppName,
                                    32,
                                    400,
                                    0);
    UT_Report(Return == CFE_SUCCESS, "CFE_ES_CreateChildTask",
              "Create child task successful",
              "08.034");

    /* Test deleting a child task with an invalid task ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = FALSE;
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_Report(CFE_ES_DeleteChildTask(1) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID not in use",
              "08.035");

    /* Test deleting a child task using a main task's ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 1;
    UT_Report(CFE_ES_DeleteChildTask(1) ==
                  CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK,
              "CFE_ES_DeleteChildTask",
              "Task ID belongs to a main task",
              "08.036");

    /* Test successfully deleting a child task */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId = 15;
    UT_Report(CFE_ES_DeleteChildTask(0) == CFE_SUCCESS,
              "CFE_ES_DeleteChildTask",
              "Delete child task successful",
              "08.037");

    /* Test deleting a child task with an OS task delete failure */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[0].TaskInfo.MainTaskId = 15;
    UT_SetOSFail(OS_TASKDELETE_FAIL);
    UT_Report(CFE_ES_DeleteChildTask(0) <= 0,
              "CFE_ES_DeleteChildTask",
              "OS task delete failure",
              "08.038");

    /* Test deleting a child task with the task ID out of range */
    UT_InitData();
    UT_Report(CFE_ES_DeleteChildTask(OS_MAX_TASKS + 1) == CFE_ES_ERR_TASKID,
              "CFE_ES_DeleteChildTask",
              "Task ID too large",
              "08.039");

    /* Test successfully exiting a child task */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = FALSE;
    CFE_ES_Global.AppTable[0].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 3;
    CFE_ES_ExitChildTask();
    UT_Report(OSTaskExitRtn.value == 1 && OSTaskExitRtn.count == 1,
              "CFE_ES_ExitChildTask",
              "Exit child task successful",
              "08.040");

    /* Test exiting a child task within an app main task */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].TaskInfo.MainTaskId = 1;
    CFE_ES_ExitChildTask();
    UT_Report(OSPrintRtn.value == 17 && OSPrintRtn.count == -2,
              "CFE_ES_ExitChildTask",
              "Cannot call from a cFE application main task",
              "08.041");

    /* Test exiting a child task with an error retrieving the app ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    CFE_ES_ExitChildTask();
    UT_Report(OSPrintRtn.value == 18 && OSPrintRtn.count == -2,
              "CFE_ES_ExitChildTask",
              "Error calling CFE_ES_GetAppID",
              "08.042");

    /* Test registering a child task with an OS task register failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKREGISTER_FAIL);
    UT_Report(CFE_ES_RegisterChildTask() == CFE_ES_ERR_CHILD_TASK_REGISTER,
              "CFE_ES_RegisterChildTask",
              "OS task register failed",
              "08.043");

    /* Test successfully registering a child task */
    UT_InitData();
    UT_Report(CFE_ES_RegisterChildTask() == CFE_SUCCESS,
              "CFE_ES_RegisterChildTask",
              "Register child task successful",
              "08.044");

    /* Test successfully adding a time-stamped message to the system log that
         must be truncated */
    UT_InitData();
    CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_SYSTEM_LOG_SIZE - 10;
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_DISCARD;
    UT_Report(CFE_ES_WriteToSysLog("SysLogText") == CFE_SUCCESS,
              "CFE_ES_WriteToSysLog",
              "Add message to log that must be truncated",
              "08.045");

    /* Test registering a device driver */
    /* NOTE: This capability is not currently implemented in cFE */
    UT_InitData();
    UT_Report(CFE_ES_RegisterDriver(NULL, NULL) == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_RegisterDriver",
              "*Not implemented* Register driver successful",
              "08.046");

    /* Test unloading a device driver
     * NOTE: This capability is not currently implemented in cFE
     */
    UT_InitData();
    UT_Report(CFE_ES_UnloadDriver(0) == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_UnloadDriver",
              "*Not implemented* Unload driver successful",
              "08.047");

    /* Test calculating a CRC on a range of memory using CRC type 8
     * NOTE: This capability is not currently implemented in cFE
     */
    UT_InitData();
    UT_Report(CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_8) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-8 algorithm",
              "08.048");

    /* Test calculating a CRC on a range of memory using CRC type 16 */
    UT_InitData();
    UT_Report(CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_16) == 2688,
              "CFE_ES_CalculateCRC",
              "CRC-16 algorithm",
              "08.049");

    /* Test calculating a CRC on a range of memory using CRC type 32
     * NOTE: This capability is not currently implemented in cFE
     */
    UT_InitData();
    UT_Report(CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_ES_CRC_32) == 0,
              "CFE_ES_CalculateCRC",
              "*Not implemented* CRC-32 algorithm",
              "08.050");

    /* Test CDS registering with a write CDS failure */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId =1;
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 4, "Name3") == OS_ERROR,
              "CFE_ES_RegisterCDS",
              "Writing to BSP CDS failure",
              "08.051");

    /* Test successful CDS registering */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful",
              "08.052");

    /* Test CDS registering using an already registered name */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_CDS_ALREADY_EXISTS,
              "CFE_ES_RegisterCDS",
              "Retrieve existing CDS",
              "08.053");

    /* Test CDS registering using the same name, but a different size */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 6, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Get CDS of same name, but new size",
              "08.054");

    /* Test CDS registering using a null name */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 4, "") == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS",
              "Invalid name size",
              "08.055");

    /* Test CDS registering with no memory pool available */
    UT_InitData();
    TempSize = CFE_ES_Global.CDSVars.MemPoolSize;
    CFE_ES_Global.CDSVars.MemPoolSize = 0;
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name") == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_RegisterCDS",
              "No memory pool available",
              "08.056");
    CFE_ES_Global.CDSVars.MemPoolSize = TempSize;

    /* Test CDS registering with a block size of zero */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle,
                                 0,
                                 "Name") == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_RegisterCDS",
              "Block size zero",
              "08.057");

    /* Test CDS registering with all the CDS registries taken */
    UT_InitData();

    /* Set all the CDS registries to 'taken' */
    for (i = 0; i < CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Taken = TRUE;
    }

    UT_Report(CFE_ES_RegisterCDS(&CDSHandle,
                                 4,
                                 "Name2") == CFE_ES_CDS_REGISTRY_FULL,
              "CFE_ES_RegisterCDS",
              "No available entries",
              "08.058");

    /* Restore all the CDS registries back to 'not taken' */
    for (i = 0; i < CFE_ES_Global.CDSVars.MaxNumRegEntries; i++)
    {
        CFE_ES_Global.CDSVars.Registry[i].Taken = FALSE;
    }

    /* Test CDS registering using a bad app ID */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 4, "Name2") == CFE_ES_ERR_APPID,
              "CFE_ES_RegisterCDS",
              "Bad application ID",
              "08.059");
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;

    /* Register CDS to set up for the copy test */
    UT_InitData();
    UT_Report(CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS,
              "CFE_ES_RegisterCDS",
              "Register CDS successful (set up for copy test)",
              "08.060");

    /* Test successfully copying to CDS */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_CopyToCDS(CDSHandle, &TempSize) == CFE_SUCCESS,
              "CFE_ES_CopyToCDS",
              "Copy to CDS successful",
              "08.061");

    /* Test successfully restoring from a CDS */
    UT_InitData();
    UT_Report(CFE_ES_RestoreFromCDS(&TempSize, CDSHandle) == CFE_SUCCESS,
              "CFE_ES_RestoreFromCDS",
              "Restore from CDS successful",
              "08.062");

    /* Test shared mutex take with a take error */
    UT_InitData();
    UT_SetRtnCode(&MutSemTakeRtn, -1, 1);
    CFE_ES_LockSharedData(__func__, 12345);
    UT_Report(OSPrintRtn.value == 19 && OSPrintRtn.count == -3,
              "CFE_ES_LockSharedData",
              "Mutex take error",
              "08.063");

    /* Test shared mutex release with a release error */
    UT_InitData();
    UT_SetRtnCode(&MutSemGiveRtn, -1, 1);
    CFE_ES_UnlockSharedData(__func__, 98765);
    UT_Report(OSPrintRtn.value == 20 && OSPrintRtn.count == -3,
              "CFE_ES_UnlockSharedData",
              "Mutex release error",
              "08.064");

    /* Test successfully registering a generic counter */
    UT_InitData();
    UT_Report(CFE_ES_RegisterGenCounter(&CounterId, "Counter1") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter successful",
              "08.065");

    /* Test registering a generic counter that is already registered */
    UT_InitData();
    UT_Report(CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter1") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Attempt to register an existing counter",
              "08.066");

    /* Test registering the maximum number of generic counters */
    UT_InitData();

    for (i = 1; i < CFE_ES_MAX_GEN_COUNTERS; i++)
    {

        SNPRINTF(CounterName, 11, "Counter%d", i + 1);

        if (CFE_ES_RegisterGenCounter(&CounterId, CounterName) != CFE_SUCCESS)
        {
            break;
        }
    }

    UT_Report(i == CFE_ES_MAX_GEN_COUNTERS,
              "CFE_ES_RegisterGenCounter",
              "Register maximum number of counters",
              "08.067");

    /* Test registering a generic counter after the maximum are registered */
    UT_InitData();
    UT_Report(CFE_ES_RegisterGenCounter(&CounterId,
                                        "Counter999") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter",
              "Maximum number of counters exceeded",
              "08.068");

    /* Test getting a registered generic counter that doesn't exist */
    UT_InitData();
    UT_Report(CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter999") ==
                  CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCounterIDByName",
              "Cannot get counter that does not exist",
              "08.069");

    /* Test successfully getting a registered generic counter ID by name */
    UT_InitData();
    UT_Report(CFE_ES_GetGenCounterIDByName(&CounterId,
                                           "Counter5") == CFE_SUCCESS,
              "CFE_ES_GetGenCounterIDByName",
              "Get generic counter ID successful",
              "08.070");

    /* Test deleting a registered generic counter that doesn't exist */
    UT_InitData();
    UT_Report(CFE_ES_DeleteGenCounter(123456) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_DeleteGenCounter",
              "Cannot delete counter that does not exist",
              "08.071");

    /* Test successfully deleting a registered generic counter by ID */
    UT_InitData();
    UT_Report(CFE_ES_DeleteGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_DeleteGenCounter",
              "Successful",
              "08.072");

    /* Test successfully registering a generic counter to verify a place for
     * it is now available and to provide an ID for subsequent tests
     */
    UT_InitData();
    UT_Report(CFE_ES_RegisterGenCounter(&CounterId, "CounterX") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter",
              "Register counter; back to maximum number",
              "08.073");

    /* Test incrementing a generic counter that doesn't exist */
    UT_InitData();
    UT_Report(CFE_ES_IncrementGenCounter(123456) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter",
              "Bad counter ID",
              "08.074");

    /* Test successfully incrementing a generic counter */
    UT_InitData();
    UT_Report(CFE_ES_IncrementGenCounter(CounterId) == CFE_SUCCESS,
              "CFE_ES_IncrementGenCounter",
              "Increment counter successful",
              "08.075");

    /* Test getting a generic counter value for a counter that doesn't exist */
    UT_InitData();
    UT_Report(CFE_ES_GetGenCount(123456, &CounterCount) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount",
              "Bad counter ID",
              "08.076");

    /* Test successfully getting a generic counter value */
    UT_InitData();
    UT_Report(CFE_ES_GetGenCount(CounterId, &CounterCount) == CFE_SUCCESS &&
              CounterCount == 1,
              "CFE_ES_GetGenCount",
              "Get counter value successful",
              "08.077");

    /* Test setting a generic counter value for a counter that doesn't exist */
    UT_InitData();
    UT_Report(CFE_ES_SetGenCount(123456, 5) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_SetGenCount",
              "Bad counter ID",
              "08.078");

    /* Test successfully setting a generic counter value */
    UT_InitData();
    UT_Report(CFE_ES_SetGenCount(CounterId, 5) == CFE_SUCCESS,
              "CFE_ES_SetGenCount",
              "Set counter value successful",
              "08.079");

    /* Test value retrieved from a generic counter value */
    UT_InitData();
    CFE_ES_GetGenCount(CounterId, &CounterCount);
    UT_Report((CounterCount == 5), "CFE_ES_SetGenCount",
              "Check value for counter set",
              "08.080");

    /* Test handling of logging and reset after a core exception using
     * a non-running app
     */
    UT_InitData();
    CFE_ES_Global.TaskTable[OS_MAX_TASKS - 1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[OS_MAX_TASKS - 1].AppId = 3;
    CFE_ES_Global.AppTable[3].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[3].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP;
    CFE_ES_ProcessCoreException(OS_MAX_TASKS - 1,
                                (uint8 *) "Reason String",
                                (uint32 *) &i,
                                sizeof(i));
    UT_Report(OSPrintRtn.value == 21 && OSPrintRtn.count == -3,
              "CFE_ES_ProcessCoreException",
              "Cannot restart a non-running application",
              "08.081");

    /* Test handling of logging and reset after a core exception; processor
     * reset with no app restart
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
    CFE_ES_Global.AppTable[3].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_ProcessCoreException(OS_MAX_TASKS - 1,
                                (uint8 *) "Reason String",
                                (uint32 *) &i,
                                sizeof(i));
    UT_Report(PSPRestartRtn.value == CFE_ES_PROCESSOR_RESET &&
              PSPRestartRtn.count == 1,
              "CFE_ES_ProcessCoreException",
              "Processor reset with no application restart",
              "08.082");

    /* Test handling of logging and reset after a core exception; power on
     * reset with no app restart
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->ResetVars.ProcessorResetCount = 100;
    CFE_ES_ResetDataPtr->ResetVars.MaxProcessorResetCount = 5;
    CFE_ES_Global.AppTable[3].StartParams.ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_ProcessCoreException(OS_MAX_TASKS - 1,
                                (uint8 *) "Reason String",
                                (uint32 *) &i,
                                sizeof(i));
    UT_Report(PSPRestartRtn.value == CFE_ES_POWERON_RESET &&
              PSPRestartRtn.count == 1,
              "CFE_ES_ProcessCoreException",
              "Power on reset with no application restart",
              "08.083");

    /* Test waiting for apps to initialize before continuing; transition from
     * initializing to running
     */
    UT_InitData();
    CFE_ES_Global.StartupSemaphoreReleased = FALSE;
    CFE_ES_Global.StartupFileComplete = TRUE;
    CFE_ES_Global.AppStartupCounter = 0;
    CFE_ES_Global.AppTable[1].StateRecord.AppState =
        CFE_ES_APP_STATE_INITIALIZING;
    CFE_ES_WaitForStartupSync(0);
    UT_Report(OSBinSemFlushRtn.value == CFE_ES_Global.StartupSyncSemaphore &&
              OSBinSemFlushRtn.count == 1 &&
              CFE_ES_Global.AppTable[1].StateRecord.AppState ==
                  CFE_ES_APP_STATE_RUNNING,
              "CFE_ES_WaitForStartupSync",
              "Transition from initializing to running and release semaphore",
              "08.084");

    /* Test waiting for apps to initialize before continuing with the semaphore
     * already released
     */
    UT_InitData();
    CFE_ES_Global.StartupFileComplete = FALSE;
    CFE_ES_WaitForStartupSync(99);
    UT_Report(OSBinSemFlushRtn.value == -1 &&
              OSBinSemFlushRtn.count == 0 &&
              OSBinSemTimedWaitRtn.value == -1 &&
              OSBinSemTimedWaitRtn.count == 0,
              "CFE_ES_WaitForStartupSync",
              "Semaphore already released",
              "08.085");

    /* Test waiting for apps to initialize before continuing; wait
     * for semaphore
     */
    UT_InitData();
    CFE_ES_Global.StartupSemaphoreReleased = FALSE;
    CFE_ES_Global.StartupFileComplete = FALSE;
    CFE_ES_WaitForStartupSync(99);
    UT_Report(OSBinSemTimedWaitRtn.value ==
                  CFE_ES_Global.StartupSyncSemaphore &&
              OSBinSemTimedWaitRtn.count == 1,
              "CFE_ES_WaitForStartupSync",
              "Wait on semaphore",
              "08.086");

    /* Test falling through CFE_ES_WaitForStartupSync() without wait
     */
    UT_InitData();
    CFE_ES_Global.StartupSemaphoreReleased = TRUE;
    UT_Report(CFE_ES_Global.StartupSemaphoreReleased,
             "CFE_ES_WaitForStartupSync",
              "Do not wait on semaphore",
              "08.087");

    /* Test exit from CFE_ES_WriteToSysLog() due to invalid log index
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->SystemLogIndex = CFE_ES_SYSTEM_LOG_SIZE + 2;
    CFE_ES_ResetDataPtr->SystemLogMode = CFE_ES_LOG_OVERWRITE;
    CFE_ES_WriteToSysLog("SysLogText");
    UT_Report(TRUE,
             "CFE_ES_WriteToSysLog",
              "Invalid log index",
              "08.088");

    /* Test exit from CFE_ES_WriteToSysLog() due to invalid log mode
     */
    UT_InitData();
    CFE_ES_ResetDataPtr->SystemLogMode = 99;
    CFE_ES_WriteToSysLog("SysLogText");
    UT_Report(TRUE,
             "CFE_ES_WriteToSysLog",
              "Invalid log index",
              "08.089");

}/* end TestAPI */

void TestCDS()
{
    uint32 Temp;

#ifdef UT_VERBOSE
    UT_Text("Begin Test CDS\n");
#endif

    /* Test validating the app ID using a bad ID value */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].RecordUsed = TRUE;
    CFE_ES_Global.TaskTable[1].AppId = CFE_ES_MAX_APPLICATIONS + 1;
    UT_Report(CFE_ES_CDS_ValidateAppID(&Temp) == CFE_ES_ERR_APPID,
              "CFE_ES_CDS_ValidateAppID",
              "Bad app ID",
              "09.001");

    /* Test memory pool rebuild and registry recovery with an
     * unreadable registry
     */
    UT_InitData();
    CFE_ES_Global.TaskTable[1].AppId = 1;
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES + 2;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Second read from CDS bad",
              "09.002");

    /* Test CDS registry initialization with a CDS write failure */
    UT_InitData();
    UT_SetRtnCode(&BSPWriteCDSRtn, -1, 1);
    UT_Report(CFE_ES_InitCDSRegistry() == -1,
              "CFE_ES_InitCDSRegistry",
              "Failed to write registry size",
              "09.003");

    /* Test successful CDS initialization */
    UT_InitData();
    UT_SetCDSSize(128 * 1024);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_CDS_EarlyInit() == CFE_SUCCESS,
              "CFE_ES_CDS_EarlyInit",
              "Initialization successful",
              "09.004");

    /* Test CDS initialization with a read error */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(CFE_ES_CDS_EarlyInit() == -1,
              "CFE_ES_CDS_EarlyInit",
              "Unrecoverable read error",
              "09.005");

    /* Test CDS initialization with size below the minimum */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetCDSSize(1024);
    UT_Report(CFE_ES_CDS_EarlyInit() == OS_SUCCESS &&
              BSPReadCDSRtn.count == 1,
              "CFE_ES_CDS_EarlyInit",
              "CDS size less than minimum",
              "09.006");

    /* Test CDS initialization with size not obtainable */
    UT_InitData();
    UT_SetBSPFail(BSP_GETCDSSIZE_FAIL);
    UT_SetCDSSize(128 * 1024);
    UT_Report(CFE_ES_CDS_EarlyInit() == OS_ERROR,
              "CFE_ES_CDS_EarlyInit",
              "Unable to obtain CDS size",
              "09.007");

    /* Test CDS initialization with rebuilding not possible */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_Report(CFE_ES_CDS_EarlyInit() == OS_SUCCESS &&
              BSPReadCDSRtn.count == 0,
              "CFE_ES_CDS_EarlyInit",
              "Rebuilding not possible; create new CDS",
              "09.008");

    /* Test CDS validation with second CDS read call failure */
    UT_InitData();
    UT_SetCDSBSPCheckValidity(TRUE);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_ValidateCDS() == OS_ERROR,
              "CFE_ES_ValidateCDS",
              "CDS read (second call) failed",
              "09.009");

    /* Test CDS validation with CDS read end check failure */
    UT_InitData();
    UT_SetCDSReadGoodEnd(FALSE);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_ValidateCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_ValidateCDS",
              "Reading from CDS failed end check",
              "09.010");
    UT_SetCDSReadGoodEnd(TRUE);

    /* Test CDS validation with first CDS read call failure */
    UT_InitData();
    UT_SetCDSBSPCheckValidity(FALSE);
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(CFE_ES_ValidateCDS() == -1,
              "CFE_ES_ValidateCDS",
              "CDS read (first call) failed",
              "09.011");

    /* Test CDS initialization where first write call to the CDS fails */
    UT_InitData();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_InitializeCDS(128 * 1024) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "Clear CDS failed",
              "09.012");

    /* Test CDS initialization where second write call to the CDS fails */
    UT_InitData();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_InitializeCDS(0) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (second call) failed",
              "09.013");

    /* Test CDS initialization where third write call to the CDS fails */
    UT_InitData();
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_InitializeCDS(0) == OS_ERROR,
              "CFE_ES_InitializeCDS",
              "CDS write (third call) failed",
              "09.014");

    /* Test rebuilding the CDS where the registry is too large */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES + 1;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "Registry too large to recover",
              "09.015");

    /* Test successfully rebuilding the CDS */
    UT_InitData();
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_Report(CFE_ES_RebuildCDS() == CFE_SUCCESS,
              "CFE_ES_RebuildCDS",
              "CDS rebuild successful",
              "09.016");

    /* Test rebuilding the CDS with the registry unreadable */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    CFE_ES_Global.CDSVars.MaxNumRegEntries = CFE_ES_CDS_MAX_NUM_ENTRIES - 4;
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID,
              "CFE_ES_RebuildCDS",
              "CDS registry unreadable",
              "09.017");

    /* Test deleting the CDS from the registry with a registry write failure */
    UT_InitData();
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].MemHandle = 200;
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "NO_APP.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    UT_SetRtnCode(&BSPWriteCDSRtn, -1, 2);
    UT_Report(CFE_ES_DeleteCDS("NO_APP.CDS_NAME", TRUE) == -1,
              "CFE_ES_DeleteCDS",
              "CDS registry write failed",
              "09.018");

    /* Test deleting the CDS from the registry with the owner app
         still active */
    UT_InitData();
    CFE_ES_Global.CDSVars.Registry[0].Taken = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].Table = TRUE;
    CFE_ES_Global.CDSVars.Registry[0].MemHandle = 200;
    strncpy(CFE_ES_Global.CDSVars.Registry[0].Name,
            "CFE_ES.CDS_NAME", OS_MAX_API_NAME);
    CFE_ES_Global.CDSVars.Registry[0].Name[OS_MAX_API_NAME - 1] = '\0';
    strncpy((char *) CFE_ES_Global.AppTable[0].StartParams.Name, "CFE_ES",
            OS_MAX_API_NAME);
    CFE_ES_Global.AppTable[0].StartParams.Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    UT_Report(CFE_ES_DeleteCDS("CFE_ES.CDS_NAME", TRUE) ==
                  CFE_ES_CDS_OWNER_ACTIVE_ERR,
              "CFE_ES_DeleteCDS",
              "Owner application still active",
              "09.019");
}

void TestCDSMempool(void)
{
    uint32 MinCDSSize = CFE_ES_CDS_MIN_BLOCK_SIZE +
                        sizeof(CFE_ES_CDSBlockDesc_t);
    CFE_ES_CDSBlockHandle_t BlockHandle;
    int Data;

    extern uint32 CFE_ES_CDSMemPoolDefSize[];

#ifdef UT_VERBOSE
    UT_Text("Begin Test CDS memory pool\n");
#endif

    /* Set up the CDS block to read in the following tests */
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    CFE_ES_CDSBlockDesc.ActualSize =  512;
    CFE_ES_CDSBlockDesc.SizeUsed =  512;

    /* Test creating the CDS pool with the pool size too small */
    UT_InitData();
    UT_Report(CFE_ES_CreateCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_CreateCDSPool",
              "CDS pool size too small",
              "10.001");

    /* Test rebuilding the CDS pool with the pool size too small */
    UT_InitData();
    UT_Report(CFE_ES_RebuildCDSPool(0, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RebuildCDSPool",
              "CDS pool size too small",
              "10.002");

    /* Test rebuilding the CDS pool with the CDS block unused */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_SUCCESS,
              "CFE_ES_RebuildCDSPool",
              "CDS block unused",
              "10.003");

    /* Test rebuilding the CDS pool with a CDS read failure */
    UT_InitData();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error reading CDS",
              "10.004");

    /* Test rebuilding the CDS pool with a CDS write failure */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL | BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Error writing CDS",
              "10.005");

    /* Test rebuilding the CDS pool with a block not previously used */
    UT_InitData();
    CFE_ES_CDSBlockDesc.CheckBits = 0;

    /* Set flags so as to fail on second CDS read */
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_SetBSPFail(BSP_READCDS_FAIL);

    UT_Report(CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "CDS block not used before",
              "10.006");
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;

    /* Test rebuilding the CDS pool with an invalid block descriptor */
    UT_InitData();
    CFE_ES_CDSMemPoolDefSize[0] = 0;
    UT_Report(CFE_ES_RebuildCDSPool(MinCDSSize, 0) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool",
              "Invalid block descriptor",
              "10.007");
    CFE_ES_CDSMemPoolDefSize[0] = CFE_ES_CDS_MAX_BLOCK_SIZE;

    /* Test successfully creating a pool where the offset = 0 */
    UT_InitData();
    UT_Report(CFE_ES_CreateCDSPool(1000000, 0) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with zero offset; successful",
              "10.008");

    /* Test allocating a CDS block with a block size error */
    UT_InitData();
    UT_Report(CFE_ES_GetCDSBlock(&BlockHandle,
                                 800) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size error",
              "10.009");

    /* Test returning a CDS block to the memory pool using an invalid
         block descriptor */
    UT_InitData();
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor",
              "10.010");

    /* Test creating a new pool and set up for getting a pre-made block */
    UT_InitData();
    BlockHandle = 0;
    UT_Report(CFE_ES_CreateCDSPool(1000000, 8) == CFE_SUCCESS,
              "CFE_ES_CreateCDSPool",
              "Create with non-zero offset; successful",
              "10.011");

    /* Test successfully allocating a pre-made CDS block */
    UT_InitData();
    UT_Report(CFE_ES_GetCDSBlock(&BlockHandle, 800) == OS_SUCCESS,
              "CFE_ES_GetCDSBlock",
              "Get a CDS block; successful",
              "10.012");

    /* Test successfully returning a CDS block back to the memory pool */
    UT_InitData();
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == OS_SUCCESS,
              "CFE_ES_PutCDSBlock",
              "Return a CDS block; successful",
              "10.013");

    /* Test allocating a CDS block with a CDS read failure */
    UT_InitData();
    UT_SetRtnCode(&BSPReadCDSRtn, -1, 1);
    UT_Report(CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error reading CDS",
              "10.014");

    /* Test allocating a CDS block with a CDS write failure */
    UT_InitData();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_SetRtnCode(&BSPWriteCDSRtn, -1, 1);
    UT_Report(CFE_ES_GetCDSBlock(&BlockHandle, 800) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_GetCDSBlock",
              "Error writing CDS",
              "10.015");

    /* Test allocating a CDS block using a block size that's too large */
    UT_InitData();
    UT_Report(CFE_ES_GetCDSBlock(&BlockHandle,
                                 CFE_ES_CDS_MAX_BLOCK_SIZE + 1) ==
                  CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetCDSBlock",
              "Block size too large",
              "10.016");

    /* Test returning a CDS block to the memory pool with an
     * invalid CDS handle
     */
    UT_InitData();
    BlockHandle = 7;
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid CDS handle",
              "10.017");

    /* Test returning a CDS block to the memory pool with a CDS read error */
    UT_InitData();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    BlockHandle = 10;
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error reading CDS",
              "10.018");

    /* Test returning a CDS block to the memory pool with an invalid
     * block descriptor
     */
    UT_InitData();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid block descriptor",
              "10.019");

    /* Test returning a CDS block to the memory pool with the block size
     * too large
     */
    UT_InitData();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutCDSBlock",
              "Invalid memory handle",
              "10.020");

    /* Test returning a CDS block to the memory pool with a CDS write error */
    UT_InitData();
    CFE_ES_CDSBlockDesc.ActualSize  = 452;
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_PutCDSBlock(BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_PutCDSBlock",
              "Error writing CDS",
              "10.021");

    /* Test CDS block write using an invalid memory handle */
    UT_InitData();
    BlockHandle = 7;
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid memory handle",
              "10.022");
    BlockHandle = 10;

    /* Test CDS block write with the block size too large */
    UT_InitData();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Actual size too large",
              "10.023");
    CFE_ES_CDSBlockDesc.ActualSize  = 452;

    /* Test CDS block write using an invalid (unused) block */
    UT_InitData();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle,
                                   &Data) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockWrite",
              "Invalid CDS block",
              "10.024");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block write with a CDS write error (block descriptor) */
    UT_InitData();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing block descriptor to CDS",
              "10.025");

    /* Test CDS block write with a CDS write error (new data) */
    UT_InitData();
    UT_SetBSPFail(BSP_WRITECDS_FAIL);
    UT_SetRtnCode(&BSPWriteCDSRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error writing new data to CDS",
              "10.026");

    /* Test CDS block write with a CDS read error */
    UT_InitData();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR,
              "CFE_ES_CDSBlockWrite",
              "Error reading CDS",
              "10.027");

    /* Test CDS block read with an invalid memory handle */
    UT_InitData();
    BlockHandle = 7;
    UT_Report(CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid memory handle",
              "10.028");
    BlockHandle = 10;

    /* Test CDS block read with the block size too large */
    UT_InitData();
    CFE_ES_CDSBlockDesc.ActualSize  = CFE_ES_CDS_MAX_BLOCK_SIZE + 1;
    CFE_ES_CDSBlockDesc.CheckBits = CFE_ES_CDS_CHECK_PATTERN;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
    UT_Report(CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Actual size too large",
              "10.029");
    CFE_ES_CDSBlockDesc.ActualSize = 452;

    /* Test CDS block read using an invalid (unused) block */
    UT_InitData();
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    UT_Report(CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_CDSBlockRead",
              "Invalid CDS block",
              "10.030");
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;

    /* Test CDS block read with a CRC mismatch */
    UT_InitData();
    Data = CFE_ES_CDSBlockDesc.CRC;
    CFE_ES_CDSBlockDesc.CRC = 56456464;
    UT_Report(CFE_ES_CDSBlockRead(&Data,
                                  BlockHandle) == CFE_ES_CDS_BLOCK_CRC_ERR,
              "CFE_ES_CDSBlockRead",
              "CRC doesn't match",
              "10.031");
    CFE_ES_CDSBlockDesc.CRC = Data;

    /* Test CDS block read with a CDS read error (block descriptor) */
    UT_InitData();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_Report(CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block descriptor from CDS",
              "10.032");

    /* Test CDS block read with a CDS read error (block data) */
    UT_InitData();
    UT_SetBSPFail(BSP_READCDS_FAIL);
    UT_SetRtnCode(&BSPReadCDSRtn, OS_SUCCESS, 1);
    UT_Report(CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR,
              "CFE_ES_CDSBlockRead",
              "Error reading block data from CDS",
              "10.033");
}

void TestESMempool(void)
{
    CFE_ES_MemHandle_t    HandlePtr;
    uint8                 Buffer[CFE_ES_MAX_BLOCK_SIZE];
    Pool_t                blk_address;
    uint8                 *address = NULL;
    uint8                 *address2 = NULL;
    Pool_t                *PoolPtr;
    CFE_ES_MemPoolStats_t Stats;
    uint32                BlockSizes[4];
    BD_t                  *BdPtr;
    CFE_ES_MemHandle_t    HandlePtr2;
    CFE_ES_MemHandle_t    HandlePtrSave;

#ifdef UT_VERBOSE
    UT_Text("Begin Test ES memory pool\n");
#endif

    /* Test creating memory pool without using a mutex with the pool size
      * too small
      */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateNoSem",
              "Pool size too small",
              "11.001");

    /* Test successfully creating memory pool without using a mutex */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateNoSem(&HandlePtr,
                                     Buffer,
                                     CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreateNoSem",
              "Memory pool create; successful",
              "11.002");

    /* Test creating memory pool using a mutex with the pool size too small */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreate(&HandlePtr, Buffer, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreate",
              "Pool size too small",
              "11.003");

    /* Test successfully creating memory pool using a mutex */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [1]; successful",
              "11.004");

    /* Test successfully allocating a pool buffer */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [1]; successful",
              "11.005");

    /* Test successfully getting the size of an existing pool buffer */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBufInfo(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful",
              "11.006");

    /* Test successfully returning a pool buffer to the memory pool */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the memory pool; successful",
              "11.007");

    /* Test successfully allocating an additional pool buffer */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [2]; successful",
              "11.008");

    /* Test successfully returning a pool buffer to the second memory pool */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr, (uint32 *) address) > 0,
              "CFE_ES_PutPoolBuf",
              "Return buffer to the second memory pool; successful",
              "11.009");

    /* Test handle validation using a handle with an address not on a
         32-bit boundary */
    UT_InitData();
    UT_Report(CFE_ES_ValidateHandle(3) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; not on 32-bit boundary",
              "11.010");

    /* Test handle validation using a handle with an invalid memory address */
    UT_InitData();
    PoolPtr = (Pool_t *) &HandlePtr2;
    PoolPtr->Start = &HandlePtrSave;
    PoolPtr->Size = 64;
    UT_SetRtnCode(&PSPMemValRangeRtn, -1, 1);
    UT_Report(CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; bad memory address",
              "11.011");

    /* Test handle validation using a handle where the first pool structure
     * field is not the pool start address
     */
    UT_InitData();
    UT_Report(CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; not pool start address",
              "11.012");

    /* Test allocating a pool buffer where the memory handle is not the pool
     * start address
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address,
                                HandlePtr2,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "Invalid handle; not pool start address",
              "11.013");

    /* Test getting memory pool statistics where the memory handle is not
     * the pool start address
     */
    UT_InitData();
    UT_Report(CFE_ES_GetMemPoolStats(&Stats,
                                     HandlePtr2) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetMemPoolStats",
              "Invalid handle; not pool start address",
              "11.014");

    /* Test handle validation using a handle where the pool size is not a
     * multiple of 4 bytes
     */
    UT_InitData();
    PoolPtr = (Pool_t *) &HandlePtr2;
    PoolPtr->Start = &HandlePtr2;
    PoolPtr->Size = 63;
    UT_Report(CFE_ES_ValidateHandle(HandlePtr2) == FALSE,
              "CFE_ES_ValidateHandle",
              "Invalid handle; pool size not multiple of 4 bytes",
              "11.015");

    /* Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address,
                                HandlePtr,
                                75000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Requested pool size too large",
              "11.016");

    /* Test getting the size of an existing pool buffer using an
     * invalid handle
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBufInfo(HandlePtr, (uint32 *) address) < 0,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle",
              "11.017");

    /* Test returning a pool buffer using an invalid handle */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid memory pool handle or memory block",
              "11.018");

    /* Test initializing a pre-allocated pool specifying a number of block
     * sizes greater than the maximum
     */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES + 2,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Number of block sizes exceeds maximum",
              "11.019");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and using the default block size
     */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  NULL,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (default block size)",
              "11.020");

    /* Test initializing a pre-allocated pool where the memory pool address
     * is not 32-bit aligned
     */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) 145453,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  BlockSizes,
                                  CFE_ES_USE_MUTEX)== CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool address not 32-bit aligned",
              "11.021");

    /* Test initializing a pre-allocated pool using an invalid mutex option */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreateEx(&HandlePtr,
                                  (uint8 *) &blk_address,
                                  0,
                                  CFE_ES_MAX_MEMPOOL_BLOCK_SIZES - 2,
                                  BlockSizes,
                                  2) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Invalid mutex option",
              "11.022");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and specifying the block size
     */
    UT_InitData();
    BlockSizes[0] = 10 ;
    BlockSizes[1] = 50 ;
    BlockSizes[2] = 100;
    BlockSizes[3] = 1000 ;
    UT_Report(CFE_ES_PoolCreateEx(&HandlePtr,
                                 (uint8 *) &blk_address,
                                 0,
                                 4,
                                 BlockSizes,
                                 CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx",
              "Memory pool size too small (block size specified)",
              "11.023");

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [2]; successful",
              "11.024");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful",
              "11.025");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block
     */
    UT_InitData();
    BdPtr = (BD_t *) ((uint8 *) address - sizeof(BD_t));
    BdPtr->Allocated = 717;
    UT_Report(CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; unallocated block",
              "11.026");

    /* Test returning a pool buffer using an unallocated block */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Deallocate an unallocated block",
              "11.027");

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern
     */
    UT_InitData();
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits = 717;
    UT_Report(CFE_ES_GetPoolBufInfo(HandlePtr,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "Invalid memory pool handle; check bit pattern",
              "11.028");

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor
     */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Invalid/corrupted memory descriptor",
              "11.029");

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    UT_InitData();
    UT_Report(CFE_ES_PoolCreate(&HandlePtr,
                                Buffer,
                                CFE_ES_MAX_BLOCK_SIZE) == CFE_SUCCESS,
              "CFE_ES_PoolCreate",
              "Create memory pool (using mutex) [3]; successful",
              "11.030");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address, HandlePtr, 256) > 0,
              "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [4]; successful",
              "11.031");

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum
     */
    UT_InitData();
    BdPtr->CheckBits = 0x5a5a;
    BdPtr->Size =CFE_ES_MAX_BLOCK_SIZE +1;
    UT_Report(CFE_ES_PutPoolBuf(HandlePtr,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "Pool buffer size exceeds maximum",
              "11.032");

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum
     */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address2,
                                HandlePtr,
                                99000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf",
              "Pool buffer size exceeds maximum",
              "11.033");

    /* Test handle validation using a null handle */
    UT_InitData();
    UT_Report(CFE_ES_ValidateHandle(0) == FALSE,
              "CFE_ES_ValidateHandle",
              "NULL handle",
              "11.034");

    /* Test returning a pool buffer using a null handle */
    UT_InitData();
    UT_Report(CFE_ES_PutPoolBuf(0,
                                (uint32 *) address) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_PutPoolBuf",
              "NULL memory handle",
              "11.035");

    /* Test allocating a pool buffer using a null handle */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBuf((uint32 **) &address,
                                0,
                                256) == CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBuf",
              "NULL memory handle",
              "11.036");

    /* Test getting the size of an existing pool buffer using a null handle */
    UT_InitData();
    UT_Report(CFE_ES_GetPoolBufInfo(0,
                                    (uint32 *) address) ==
                  CFE_ES_ERR_MEM_HANDLE,
              "CFE_ES_GetPoolBufInfo",
              "NULL memory handle",
              "11.037");
}

#ifdef CFE_ARINC653
void TestStaticApp(void)
{
    int                     j;
    uint32                  Id;
    uint32                  LibraryIdPtr;
    CFE_ES_AppMainFuncPtr_t MainFunc = 0;

    /* Test successful static app creation */
    UT_InitData();
    CFE_ES_Global.AppTable[0].RecordUsed = TRUE;
    CFE_ES_Global.AppTable[1].RecordUsed = FALSE;
    CFE_ES_Global.TaskTable[1].RecordUsed = FALSE;
    UT_Report(CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_SUCCESS &&
              CFE_ES_Global.TaskTable[1].RecordUsed == TRUE,
              "CFE_ES_StaticAppCreate",
              "Create static application; successful",
              "12.001");

    /* Test static app creation with a task create failure */
    UT_InitData();
    UT_SetOSFail(OS_TASKCREATE_FAIL);
    UT_Report(CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_StaticAppCreate",
              "Task create error",
              "12.002");

    /* Test static app creation specifying a slot already in use */
    UT_InitData();
    CFE_ES_Global.TaskTable[0].RecordUsed = TRUE;
    UT_Report(CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_SUCCESS,
              "CFE_ES_StaticAppCreate",
              "Task slot in use",
              "12.003");

    /* Test static app creation with no free slots available */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_APPLICATIONS; j++)
    {
        CFE_ES_Global.AppTable[j].RecordUsed = TRUE;
    }

    UT_Report(CFE_ES_StaticAppCreate(&Id,
                                     MainFunc,
                                     "AppName",
                                     170,
                                     4096,
                                     1) == CFE_ES_ERR_APP_CREATE,
              "CFE_ES_StaticAppCreate",
              "No free application slots available",
              "12.004");

    /* Test successful static shared library initialization */
    UT_InitData();
    UT_SetLibInitRtn(CFE_SUCCESS);
    LibraryIdPtr = 1;
    CFE_ES_Global.LibTable[0].RecordUsed = FALSE;
    UT_Report(CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_SUCCESS &&
              LibraryIdPtr == 0,
              "CFE_ES_InitStaticLibrary",
              "Static library initialization; successful",
              "12.005");

    /* Test static shared library initialization with a library load failure */
    UT_InitData();
    UT_SetLibInitRtn(CFE_ES_ERR_LOAD_LIB);
    CFE_ES_Global.LibTable[0].RecordUsed = FALSE;
    UT_Report(CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_InitStaticLibrary",
              "Shared library initialization failure",
              "12.006");

    /* Test static shared library initialization with no free
     * slots available
     */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = TRUE;
    }

    UT_Report(CFE_ES_InitStaticLibrary(&LibraryIdPtr,
                                       (CFE_ES_LibInitFuncPtr_t) UT_LibInit,
                                       "LibName") == CFE_ES_ERR_LOAD_LIB,
              "CFE_ES_InitStaticLibrary",
              "No free library slots available",
              "12.007");

    /* Test successful static application initialization */
    UT_InitData();

    for (j = 0; j < CFE_ES_MAX_LIBRARIES; j++)
    {
        CFE_ES_Global.LibTable[j].RecordUsed = FALSE;
    }

    strncpy(CFE_ES_StaticStartupTable[0].Name,
            "StaticAppName", OS_MAX_API_NAME);
    CFE_ES_StaticStartupTable[0].Name[OS_MAX_API_NAME - 1] = '\0';
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_APP;
    CFE_ES_StaticStartupTable[0].ExceptionAction =
        CFE_ES_APP_EXCEPTION_RESTART_APP + 1;
    CFE_ES_StartStaticApplications(CFE_ES_PROCESSOR_RESET);
    UT_Report(CFE_ES_StaticStartupTable[0].ExceptionAction ==
                  CFE_ES_APP_EXCEPTION_PROC_RESTART &&
              OSPrintRtn.value == 0,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Initialize static application; successful",
              "12.008");

    /* Test successful static library initialization */
    UT_InitData();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_LIB;
    CFE_ES_StartStaticApplications(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 0,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Initialize static shared library; successful",
              "12.009");

    /* Test successful device driver initialization */
    UT_InitData();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_DRV;
    CFE_ES_StartStaticApplications(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 0,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Device drivers not yet implemented",
              "12.010");

    /* Test successful static application initialization null action */
    UT_InitData();
    CFE_ES_StaticStartupTable[0].ObjectType = CFE_ES_STATIC_NULL;
    CFE_ES_StartStaticApplications(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 0,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* No action",
              "12.011");

    /* Test successful static library initialization with an invalid
     * entry type
     */
    UT_InitData();
    CFE_ES_StaticStartupTable[0].ObjectType = 0xff;
    CFE_ES_StartStaticApplications(CFE_ES_PROCESSOR_RESET);
    UT_Report(OSPrintRtn.value == 0,
              "CFE_ES_StartStaticApplications",
              "*Not implemented* Unexpected entry type",
              "12.012");
}
#endif

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
    CFE_ES_TaskPipe(MsgPtr);
}
