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
**    evs_UT.c
**
** Purpose:
**    Event Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/30 13:20:34GMT-05:00 $
** $Revision: 1.8 $
**
*/

/*
** Includes
*/
#include "evs_UT.h"

/*
** External global variables
*/
extern CFE_EVS_GlobalData_t CFE_EVS_GlobalData;

extern UT_SetRtn_t MutSemCreateRtn;
extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SB_SubscribeExRtn;
extern UT_SetRtn_t GetAppIDRtn;
extern UT_SetRtn_t GetResetTypeRtn;
extern UT_SetRtn_t WriteSysLogRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t OSPrintRtn;

/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int evs_main(void)
#endif
{
    /* Initialize unit test */
    UT_Init("evs");
    UT_Text("cFE EVS Unit Test Output File\n\n");

    /* Test_Init is a test but also MUST be called first and only once */
    Test_Init();
    Test_IllegalAppID();
    Test_UnregisteredApp();
    Test_FilterRegistration();
    Test_FilterReset();
    Test_Format();
    Test_Ports();
    Test_Logging();
    Test_WriteApp();
    Test_BadAppCmd();
    Test_EventCmd();
    Test_FilterCmd();
    Test_InvalidCmd();
    Test_Misc();

    /* Final report on number of errors */
    UT_ReportFailures();
    return 0;
}

/*
** Initialization test, must be called first
*/
void Test_Init(void)
{
    CFE_EVS_BitMaskCmd_t        bitmaskcmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    strncpy((char *) appbitcmd.AppName, "ut_cfe_evs", OS_MAX_API_NAME);
    appbitcmd.AppName[OS_MAX_API_NAME - 1] = '\0';

    /* Test successful early initialization of the cFE EVS */
    UT_InitData();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_ES_POWERON_RESET, 1);
    CFE_EVS_EarlyInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 4,
              "CFE_EVS_EarlyInit",
              "Early initialization successful",
              "01.001");

    /* Test TaskMain with a command pipe read failure due to an
     * invalid command packet
     */
    UT_InitData();
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 1);
    CFE_EVS_TaskMain();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 8 &&
              SendMsgEventIDRtn.value == CFE_EVS_ERR_MSGID_EID,
              "CFE_EVS_TaskMain",
              "Error reading command pipe",
              "01.002");

    /* Test TaskMain with a register application failure */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1 ,1);
    CFE_EVS_TaskMain();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 7 &&
              WriteSysLogRtn.count == 2,
              "CFE_EVS_TaskMain",
              "Application initialization failure",
              "01.003");

    /* Test early initialization with a get reset area failure */
    UT_InitData();
    UT_SetStatusBSPResetArea(-1, CFE_TIME_RESET_SIGNATURE, CFE_TIME_TONE_PRI);
    CFE_EVS_EarlyInit();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 1,
              "CFE_EVS_EarlyInit",
              "CFE_PSP_GetResetArea call failure",
              "01.004");

    /* Test early initialization, restoring the event log */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_EarlyInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 6,
              "CFE_EVS_EarlyInit",
              "Event log restored",
              "01.005");

    /* Test early initialization, clearing the event log */
    UT_InitData();
    UT_SetRtnCode(&GetResetTypeRtn, -1, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->Next = CFE_EVS_LOG_MAX;
    CFE_EVS_EarlyInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 5,
              "CFE_EVS_EarlyInit",
              "Event log cleared",
              "01.006");

    /* Test early initialization with a mutex creation failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, -1, 1);
    CFE_EVS_EarlyInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 3,
              "CFE_EVS_EarlyInit",
              "Mutex create failure",
              "01.007");

    /* Test early initialization with an unexpected size returned
     * by CFE_PSP_GetResetArea
     */
    UT_InitData();
    UT_SetSizeofESResetArea(0);
    CFE_EVS_EarlyInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 2,
              "CFE_EVS_EarlyInit",
              "Unexpected size returned by CFE_PSP_GetResetArea",
              "01.008");

    /* Test task initialization where the application registration fails */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 9,
              "CFE_EVS_TaskInit",
              "Call to CFE_ES_RegisterApp failure",
              "01.009");

    /* Test task initialization where the pipe creation fails */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 12,
              "CFE_EVS_TaskInit",
              "Call to CFE_SB_CreatePipe failure",
              "01.010");

    /* Test task initialization where command subscription fails */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -1, 1);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 13,
              "CFE_EVS_TaskInit",
              "Subscribing to commands failure",
              "01.011");

    /* Test task initialization where HK request subscription fails */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeExRtn, -1, 2);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 14,
              "CFE_EVS_TaskInit",
              "Subscribing to HK request failure",
              "01.012");

    /* Test task initialization where getting the application ID fails */
    UT_InitData();
    UT_SetAppID(10000);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == EVS_SYSLOG_OFFSET + 10,
              "CFE_EVS_TaskInit",
              "Call to CFE_ES_GetAppID Failed",
              "01.013");

    /* Test successful task initialization */
    UT_InitData();
    UT_SetAppID(0);
    CFE_EVS_TaskInit();
    UT_Report(WriteSysLogRtn.value == -1,
              "CFE_EVS_TaskInit",
              "Normal init (WARM)",
              "01.014");

    /* Enable DEBUG message output */
    UT_InitData();
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output",
              "01.015");

    /* Disable ports */
    UT_InitData();
    bitmaskcmd.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Disable ports",
              "01.016");
}

/*
** Test functions using an illegal application ID
*/
void Test_IllegalAppID(void)
{
    CFE_TIME_SysTime_t time = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Illegal App ID\n");
#endif

    /* Set test up with illegal application ID */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS + 1);

    /* Test registering an event using an illegal application ID */
    UT_Report(CFE_EVS_Register(NULL, 0, 0) == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_Register",
              "Illegal app ID",
              "02.001");

    /* Test unregistering an event using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_Unregister() == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_Unregister",
              "Illegal app ID",
              "02.002");

    /* Test sending an event using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0, 0, "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendEvent",
              "Illegal app ID",
              "02.003");

    /* Test sending a timed event using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_SendTimedEvent(time,
                                     0,
                                     0,
                                     "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendTimedEvent",
              "Illegal app ID",
              "02.004");

    /* Test sending an event with app ID using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_SendEventWithAppID(0,
                                         0,
                                         CFE_ES_MAX_APPLICATIONS + 1,
                                         "NULL") == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_SendEventWithAppID",
              "Illegal app ID",
              "02.005");

    /* Test resetting a filter using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_ResetFilter(0) == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_ResetFilter",
              "Illegal app ID",
              "02.006");

    /* Test resetting all filters using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_ResetAllFilters() == CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_ResetAllFilters",
              "Illegal app ID",
              "02.007");

    /* Test application cleanup using an illegal application ID */
    UT_InitData();
    UT_Report(CFE_EVS_CleanUpApp(CFE_ES_MAX_APPLICATIONS + 1) ==
                  CFE_EVS_APP_ILLEGAL_APP_ID,
              "CFE_EVS_CleanUpApp",
              "Illegal app ID",
              "02.008");

    /* Return application ID to valid value */
    UT_SetAppID(0);
}

/*
** Test functions using an unregistered application
*/
void Test_UnregisteredApp(void)
{
    CFE_TIME_SysTime_t time = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregistered App\n");
#endif

    UT_InitData();

    /* Unregister the application (it was registered in CFE_EVS_TaskInit) */
    CFE_EVS_Unregister();

    /* Test unregistering an already unregistered application */
    UT_Report(CFE_EVS_Unregister() == CFE_SUCCESS,
              "CFE_EVS_Unregister","App not registered",
              "03.001");

    /* Test sending an event to an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0, 0, "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendEvent",
              "App not registered",
              "03.002");

    /* Test resetting a filter using an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_ResetFilter(0) == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_ResetFilter",
              "App not registered",
              "03.003");

    /* Test resetting all filters using an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_ResetAllFilters() == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_ResetAllFilters",
              "App not registered",
              "03.004");

    /* Test sending an event with app ID to an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_SendEventWithAppID(0,
                                         CFE_EVS_INFORMATION,
                                         0,
                                         "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendEventWithAppID",
              "App not registered",
              "03.005");

    /* Test sending a timed event to an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_SendTimedEvent(time,
                                     CFE_EVS_INFORMATION,
                                     0,
                                     "NULL") == CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_SendTimedEvent",
              "App not registered",
              "03.006");

    /* Test application cleanup using an unregistered application */
    UT_InitData();
    UT_Report(CFE_EVS_CleanUpApp(0) == CFE_SUCCESS,
              "CFE_EVS_CleanUpApp",
              "App not registered",
              "03.007");

    /* Re-register the application for subsequent tests */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register app - successful",
              "03.008");
}

/*
** Test filter registration related calls
*/
void Test_FilterRegistration(void)
{
    int i;

    CFE_EVS_BinFilter_t filter[CFE_EVS_MAX_EVENT_FILTERS + 1];
    EVS_BinFilter_t *FilterPtr = NULL;
    uint32 AppID;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Registration\n");
#endif

    /* Test filter registration using an invalid filter option */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL,
                               0,
                               CFE_EVS_BINARY_FILTER + 1) ==
                  CFE_EVS_UNKNOWN_FILTER,
              "CFE_EVS_Register","Illegal filter option",
              "04.001");

    /* Test successful filter registration with no filters */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid w/ no filters",
              "04.002");

    /* Test filter unregistration with failed ES_putPool */
    UT_InitData();
    UT_SetPutPoolFail(1);
    UT_SetRtnCode(&GetAppIDRtn, -1, 1);
    UT_Report(CFE_EVS_Unregister() < 0, "CFE_EVS_Unregister",
              "Unregistration with failed ES_putPool",
              "04.003");

    /* Re-register to test valid unregistration */
    UT_InitData();
    UT_SetPutPoolFail(0);
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid with no filters (re-registration)",
              "04.004");

    /* Test successful filter unregistration */
    UT_InitData();
    UT_Report(CFE_EVS_Unregister() == CFE_SUCCESS,
              "CFE_EVS_Unregister",
              "Valid unregistration",
              "04.005");

    /* Test successful filter registration with a valid filter */
    UT_InitData();
    filter[0].EventID = 0;
    filter[0].Mask = 0x0001;
    UT_Report(CFE_EVS_Register(filter,
                               1,
                               CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid w/ filter",
              "04.006");

    /* Test successful multiple filter registration with valid filters */
    UT_InitData();

    for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS + 1; i++)
    {
        filter[i].EventID = i;
        filter[i].Mask = 1;
    }

    UT_Report(CFE_EVS_Register(filter, CFE_EVS_MAX_EVENT_FILTERS + 1,
              CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Valid over max filters",
              "04.007");

    /* Send 1st information message, should get through */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "OK") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "1st info message should go through",
              "04.008");

    /* Send 2nd information message, should be filtered */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "2nd info message should be filtered",
              "04.009");

    /* Send last information message, which should cause filtering to lock */
    UT_InitData();
    CFE_ES_GetAppID(&AppID);
    FilterPtr = EVS_FindEventID(0,
        (EVS_BinFilter_t *) CFE_EVS_GlobalData.AppData[AppID].BinFilters);
    FilterPtr->Count = CFE_EVS_MAX_FILTER_COUNT - 1;
    UT_Report(CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "OK") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Last info message should go through",
              "04.010");

    /* Test that filter lock is applied */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Locked info message should be filtered",
              "04.011");

    /* Test that filter lock is (still) applied */
    UT_InitData();
    UT_Report(CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                "FAILED") == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Locked info message should still be filtered",
              "04.012");

    /* Return application to original state: re-register application */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "FE_EVS_Register",
              "Re-register app",
              "04.013");
}

/*
** Test reset filter calls
*/
void Test_FilterReset(void)
{
    CFE_EVS_BinFilter_t filter;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Reset\n");
#endif

    /* Test filter reset without app registration */
    UT_InitData();
    CFE_EVS_Unregister();
    UT_Report(CFE_EVS_ResetFilter(1) ==
              CFE_EVS_APP_NOT_REGISTERED,
              "CFE_EVS_ResetFilter",
              "App not registered",
              "05.001");

    /* Test successful filter registration */
    UT_InitData();
    filter.EventID = 1;
    filter.Mask = 0x0001;
    UT_Report(CFE_EVS_Register(&filter, 1, CFE_EVS_BINARY_FILTER) ==
              CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register filter - successful",
              "05.002");

    /* Test filter reset using an unregistered event ID */
    UT_InitData();
    UT_Report(CFE_EVS_ResetFilter(2) ==
              CFE_EVS_EVT_NOT_REGISTERED,
              "CFE_EVS_ResetFilter",
              "Unregistered event ID",
              "05.003");

    /* Test successful filter reset */
    UT_InitData();
    UT_Report(CFE_EVS_ResetFilter(1) ==
              CFE_SUCCESS,
              "CFE_EVS_ResetFilter",
              "Valid reset filter",
              "05.004");

    /* Test successful reset of all filters */
    UT_InitData();
    UT_Report(CFE_EVS_ResetAllFilters() ==
              CFE_SUCCESS,
              "CFE_EVS_ResetAllFilters",
              "Valid reset all filters",
              "05.005");

    /* Return application to original state: re-register application */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Re-register app",
              "05.006");
}

/*
** Test long and short format events, and event strings
** greater than the maximum length allowed
*/
void Test_Format(void)
{
    int i;
    char long_msg[CFE_EVS_MAX_MESSAGE_LENGTH + 2];
    int16 EventID[2];

    CFE_TIME_SysTime_t          time = {0, 0};
    CFE_EVS_ModeCmd_t           modecmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Format\n");
#endif

    /* Enable DEBUG message output */
    UT_InitData();
    strncpy((char *) appbitcmd.AppName, "ut_cfe_evs", OS_MAX_API_NAME);
    appbitcmd.AppName[OS_MAX_API_NAME - 1] = '\0';
    modecmd.Mode = CFE_EVS_LONG_FORMAT;
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output",
              "06.001");

    /* Test set event format mode command using an invalid mode */
    UT_InitData();
    modecmd.Mode = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLEGALFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: invalid event format mode = 0xff",
              "06.002");

    /* Test set event format mode command using a valid command to set short
     * format, reports implicitly via event
     */
    UT_InitData();
    modecmd.Mode = CFE_EVS_SHORT_FORMAT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_SETEVTFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: short format",
              "06.003");

    /* Test event short format mode command was successful */
    UT_InitData();
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Short format check *FAILED*");
    UT_Report(SendMsgEventIDRtn.value == 0 && SendMsgEventIDRtn.count == 0,
              "CFE_EVS_SetEventFormatModeCmd",
              "Short event format mode verification",
              "06.004");

    /* Test set event format mode command using a valid command to set long
     * format, reports implicitly via event
     */
    UT_InitData();
    modecmd.Mode = CFE_EVS_LONG_FORMAT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_SETEVTFMTMOD_EID,
              "CFE_EVS_SetEventFormatModeCmd",
              "Set event format mode command: long format",
              "06.005");

    /* Test event long format mode command was successful (the following
     * messages are output if long format selection is successful)
     */
    UT_InitData();
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Long format check (SendEvent)");
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendTimedEvent(time, 0, CFE_EVS_INFORMATION,
                           "Long format check (SendTimedEvent)");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEventWithAppID(0, CFE_EVS_INFORMATION, 0,
                               "Long format check (SendEventWithAppID)");
    UT_Report(EventID[0] == 0 && EventID[1] == 0 &&
              SendMsgEventIDRtn.value == 0,
              "CFE_EVS_SetEventFormatModeCmd",
              "Long event format mode verification",
              "06.006");

    /* Test sending an event using a string length greater than
     * the maximum allowed
     */
    UT_InitData();

    for (i = 0; i <= CFE_EVS_MAX_MESSAGE_LENGTH; i++)
    {
        long_msg[i] = (char)(i % 10 + 48);
    }

    long_msg[CFE_EVS_MAX_MESSAGE_LENGTH + 1] = '\0';
    UT_Report(CFE_EVS_SendEvent(0,
                                CFE_EVS_INFORMATION,
                                long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendEvent",
              "Sent info message with > maximum string length",
              "06.007");

    /* Test sending an event with application ID using a string length
     * greater than the maximum allowed
     */
    UT_Report(CFE_EVS_SendEventWithAppID(0,
                                         CFE_EVS_INFORMATION,
                                         0,
                                         long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendEventWithAppID",
              "Sent info message with > maximum string length",
              "06.008");

    /* Test sending a timed event using a string length greater than
     * the maximum allowed
     */
    UT_Report(CFE_EVS_SendTimedEvent(time,
                                     0,
                                     CFE_EVS_INFORMATION,
                                     long_msg) == CFE_SUCCESS,
              "CFE_EVS_SendTimedEvent",
              "Sent info message with > maximum string length",
              "06.009");
}

/*
** Test enable/disable of port outputs
*/
void Test_Ports(void)
{
    CFE_EVS_BitMaskCmd_t    bitmaskcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Ports\n");
#endif

    /* Test enabling all ports; reports implicitly via port output */
    UT_InitData();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);
    bitmaskcmd.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAPORT_EID &&
              OSPrintRtn.value == 4 * OS_PRINT_INCR
              && OSPrintRtn.count == 4,
              "CFE_EVS_EnablePortsCmd",
              "Enable ports command received with port bit mask = 0x0f",
              "07.001");

    /* Test that ports are enabled by sending a message */
    UT_InitData();
    UT_SetRtnCode(&OSPrintRtn, 0, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Test ports message");
    UT_Report(SendMsgEventIDRtn.value == 0 &&
              OSPrintRtn.value == 4 * OS_PRINT_INCR &&
              OSPrintRtn.count == 4,
              "CFE_EVS_EnablePortsCmd",
              "Test ports output",
              "07.002");

    /* Disable all ports to cut down on unneeded output */
    UT_InitData();
    bitmaskcmd.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT |
                         CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_DISPORT_EID,
              "CFE_EVS_DisablePortsCmd",
              "Disable ports command received with port bit mask = 0x0f",
              "07.003");

    /* Test enabling a port using a bitmask that is out of range */
    UT_InitData();
    bitmaskcmd.BitMask = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnablePortsCmd",
              "Bit mask = 0x000000ff out of range: CC = 11",
              "07.004");

    /* Test disabling a port using a bitmask that is out of range */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisablePortsCmd",
              "Bit Mask = 0x000000ff out of range: CC = 12",
              "07.005");
}

/*
** Test event logging
*/
void Test_Logging(void)
{
    int                  i;
    uint32               resetAreaSize = 0;
    char                 tmpString[100];
    CFE_EVS_ModeCmd_t    modecmd;
    CFE_EVS_LogFileCmd_t logfilecmd;
    CFE_ES_ResetData_t   *CFE_EVS_ResetDataPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Logging\n");
#endif

    /* Test setting the logging mode with logging disabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.LogEnabled = FALSE;
    modecmd.Mode = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_NO_LOGSET_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode command: event log disabled",
              "08.001");

    /* Re-enable logging and set conditions to allow complete code coverage */
    CFE_EVS_GlobalData.EVS_TlmPkt.LogEnabled = TRUE;
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_ES_POWERON_RESET, 1);
    CFE_PSP_GetResetArea(&CFE_EVS_ResetDataPtr, &resetAreaSize);
    CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

    /* Test setting the logging mode using an invalid mode */
    UT_InitData();
    modecmd.Mode = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_LOGMODE_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to an invalid mode",
              "08.002");

    /* Test setting the logging mode to discard */
    UT_InitData();
    modecmd.Mode = CFE_EVS_LOG_DISCARD;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LOGMODE_EID,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to discard mode",
              "08.003");

    /* Test overfilling the log in discard mode */
    UT_InitData();

    /* Ensure log is filled, then add one more, implicitly testing EVS_AddLog */
    for (i = 0; i < CFE_EVS_LOG_MAX; i++)
    {
        SNPRINTF(tmpString, 100, "Log fill event %d", i);
        CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, tmpString);
    }

    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Log overfill event discard");
    UT_Report(CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE &&
              CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LOG_DISCARD,
              "CFE_EVS_SendEvent",
              "Log overfill event (discard mode)",
              "08.004");

    /* Test setting the logging mode to overwrite */
    UT_InitData();
    modecmd.Mode = CFE_EVS_LOG_OVERWRITE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Log overfill event overwrite");
    UT_Report(CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == TRUE &&
              CFE_EVS_GlobalData.EVS_LogPtr->LogMode == CFE_EVS_LOG_OVERWRITE,
              "CFE_EVS_SetLoggingModeCmd",
              "Log overfill event (overwrite mode)",
              "08.005");

    /* Test writing to the log while it is disabled */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.LogEnabled = FALSE;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_LogFileCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_NO_LOGWR_EID,
              "CFE_EVS_WriteLogFileCmd",
              "Write log command with event log disabled",
              "08.006");

    /* Test clearing the log while it is disabled*/
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_NO_LOGCLR_EID,
              "EVS_ClearLog",
              "Clear log command with event log disabled",
              "08.007");

    /* Test sending a no op command */
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_CMD_MID,
               CFE_EVS_NO_OPERATION_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_NOOP_EID,
              "CFE_EVS_ProcessGroundCommand",
              "No-op command",
              "08.008");

    /* Clear log for next test */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.LogEnabled = TRUE;
    UT_SetSBTotalMsgLen(sizeof(CFE_SB_CmdHdr_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(CFE_EVS_GlobalData.EVS_LogPtr->LogFullFlag == FALSE,
              "EVS_ClearLog",
              "Clear log",
              "08.009");

    /* Test setting the logging mode to overwrite */
    UT_InitData();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetRtnCode(&GetResetTypeRtn, CFE_ES_POWERON_RESET, 1);
    CFE_PSP_GetResetArea(&CFE_EVS_ResetDataPtr, &resetAreaSize);
    CFE_EVS_GlobalData.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;
    modecmd.Mode = CFE_EVS_LOG_OVERWRITE;
    UT_Report(CFE_EVS_SetLoggingModeCmd(&modecmd) == TRUE,
              "CFE_EVS_SetLoggingModeCmd",
              "Set log mode to overwrite mode",
              "08.010");

    /* Test successfully writing a single event log entry */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    logfilecmd.LogFilename[0] = '\0';
    UT_Report(CFE_EVS_WriteLogFileCmd(&logfilecmd) == TRUE,
              "CFE_EVS_WriteLogFileCmd",
              "Write single event log entry - successful",
              "08.011");

    /* Test writing a log entry with a create failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(CFE_EVS_WriteLogFileCmd(&logfilecmd) == FALSE,
              "CFE_EVS_WriteLogFileCmd",
              "OS create fail",
              "08.012");

    /* Test successfully writing all log entries */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    CFE_EVS_GlobalData.EVS_LogPtr->LogCount = CFE_EVS_LOG_MAX;
    UT_Report(CFE_EVS_WriteLogFileCmd(&logfilecmd) == TRUE,
              "CFE_EVS_WriteLogFileCmd",
              "Write all event log entries",
              "08.013");

    /* Test writing a log entry with a write failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_SUCCESS, 1);
    UT_SetOSFail(OS_WRITE_FAIL);
    CFE_EVS_GlobalData.EVS_LogPtr->LogCount = CFE_EVS_LOG_MAX;
    UT_Report(CFE_EVS_WriteLogFileCmd(&logfilecmd) == FALSE,
              "CFE_EVS_WriteLogFileCmd",
              "OS write fail",
              "08.014");
}

/*
** Test writing application data
*/
void Test_WriteApp(void)
{
    CFE_EVS_AppDataCmd_t        AppDataCmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Write App\n");
#endif

    /* Enable DEBUG message output */
    UT_InitData();
    strncpy((char *) appbitcmd.AppName, "ut_cfe_evs", OS_MAX_API_NAME);
    appbitcmd.AppName[OS_MAX_API_NAME - 1] = '\0';
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable debug message output",
              "09.001");

    /* Test resetting counters */
    UT_InitData();
    UT_SetSBTotalMsgLen(8);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_COUNTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_RSTCNT_EID,
              "CFE_EVS_ResetCountersCmd",
              "Reset counters - successful",
              "09.002");

    /* Test writing application data with a create failure */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppDataCmd_t));
    AppDataCmd.AppDataFilename[0] = '\0';
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_CRDATFILE_EID,
              "CFE_EVS_WriteAppDataCmd",
              "OS create fail",
              "09.003");

    /* Test writing application data with a write/close failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL | OS_CLOSE_FAIL);
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_WRDATFILE_EID,
              "CFE_EVS_WriteAppDataCmd",
              "OS write fail",
              "09.004");

    /* Test successfully writing application data */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &AppDataCmd, CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_WRDAT_EID,
              "CFE_EVS_WriteAppDataCmd",
              "Write application data - successful",
              "09.005");
}

/*
** Test commands with bad application names (unknown, illegal, unregistered)
*/
void Test_BadAppCmd(void)
{
    char app_name[OS_MAX_API_NAME];

    CFE_EVS_AppNameBitMaskCmd_t     appbitcmd;
    CFE_EVS_AppNameCmd_t            appnamecmd;
    CFE_EVS_AppNameEventIDMaskCmd_t appmaskcmd;
    CFE_EVS_AppNameEventIDCmd_t     appcmdcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Bad App Command\n");
#endif

    UT_InitData();

    /* Set up event and mask */
    appbitcmd.BitMask = 0;
    appmaskcmd.Mask = 0;
    appmaskcmd.EventID = 0;
    appcmdcmd.EventID = 0;

    strncpy(app_name,"unknown_name", OS_MAX_API_NAME);
    app_name[OS_MAX_API_NAME - 1] = '\0';
    strcpy((char *) appbitcmd.AppName, app_name);
    strcpy((char *) appnamecmd.AppName, app_name);
    strcpy((char *) appmaskcmd.AppName, app_name);
    strcpy((char *) appcmdcmd.AppName, app_name);

    /* Test disabling application event types with an unknown application ID */
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Unable to retrieve application ID while disabling event types",
              "10.001");

    /* Test enabling application event types with an unknown application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Unable to retrieve application ID while enabling event types",
              "10.002");

    /* Test disabling application events with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Unable to retrieve application ID while disabling events",
              "10.003");

    /* Test enabling application events with an unknown application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Unable to retrieve application ID while enabling events",
              "10.004");

    /* Test resetting the application event counter with an unknown
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Unable to retrieve application ID while resetting events",
              "10.005");

    /* Test modifying event filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Unable to retrieve application ID while modifying event filters",
              "10.006");

    /* Test deleting event filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Unable to retrieve application ID while deleting event filters",
              "10.007");

    /* Test setting the event filter mask with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Unable to retrieve application ID while setting the event filter mask",
              "10.008");

    /* Test resetting the filter with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetFilterCmd",
              "Unable to retrieve application ID while resetting the filter",
              "10.009");

    /* Test resetting all filters with an unknown application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_NOAPPIDFOUND_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Unable to retrieve application ID while resetting all filters",
              "10.010");

    /* Test disabling application event types with an illegal application ID */
    UT_InitData();
    strncpy(app_name, "illegal_id", OS_MAX_API_NAME);
    app_name[OS_MAX_API_NAME - 1] = '\0';
    strcpy((char *) appbitcmd.AppName, app_name);
    strcpy((char *) appnamecmd.AppName, app_name);
    strcpy((char *) appmaskcmd.AppName, app_name);
    strcpy((char *) appcmdcmd.AppName, app_name);
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Illegal application ID while disabling application event types",
              "10.011");

    /* Test enabling application event types with an illegal application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Illegal application ID while enabling application event types",
              "10.012");

    /* Test disabling application events with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Illegal application ID while disabling application events",
              "10.014");

    /* Test enabling application events with an illegal application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Illegal application ID while enabling application events",
              "10.013");

    /* Test resetting the application event counter with an illegal application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Illegal application ID while resetting the application event counter",
              "10.015");

    /* Test adding the event filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Illegal application ID while adding the event filter",
              "10.016");

    /* Test deleting the event filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Illegal application ID while deleting the event filter",
              "10.017");

    /* Test setting the filter mask with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Illegal application ID while setting the filter mask",
              "10.018");

    /* Test resetting the filter with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetFilterCmd",
              "Illegal application ID while resetting the filter",
              "10.019");

    /* Test resetting all filters with an illegal application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_ILLAPPIDRANGE_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Illegal application ID while resetting all filters",
              "10.020");

    /* Test disabling application event types with an unregistered application ID */
    UT_InitData();
    strncpy(app_name,"unregistered_app", OS_MAX_API_NAME);
    app_name[OS_MAX_API_NAME - 1] = '\0';
    strcpy((char *) appbitcmd.AppName, app_name);
    strcpy((char *) appnamecmd.AppName, app_name);
    strcpy((char *) appmaskcmd.AppName, app_name);
    strcpy((char *) appcmdcmd.AppName, app_name);
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Application not registered while disabling application event types",
              "10.021");

    /* Test enabling application event types with an unregistered application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Application not registered while enabling application event types",
              "10.022");

    /* Test disabling application events with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DisableAppEventsCmd",
              "Application not registered while disabling application events",
              "10.024");

    /* Test enabling application events with an unregistered application ID */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Application not registered while enabling application events",
              "10.023");

    /* Test resetting the application event counter with an unregistered
     * application ID
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Application not registered while resetting the application event counter",
              "10.025");

    /* Test adding the event filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Application not registered while adding the event filter",
              "10.026");

    /* Test deleting the event filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Application not registered while deleting the event filter",
              "10.027");

    /* Test setting the filter mask with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Application not registered while setting the filter mask",
              "10.028");

    /* Test resetting the filter with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetFilterCmd",
              "Application not registered while resetting the filter",
              "10.029");

    /* Test resetting all filters with an unregistered application ID */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_APPNOREGS_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Application not registered while resetting all filters",
              "10.030");
}

/*
** Test event commands (and reset counter)
*/
void Test_EventCmd(void)
{
    int16                       EventID[4];
    char                        app_name[OS_MAX_API_NAME];
    CFE_EVS_BitMaskCmd_t        bitmaskcmd;
    CFE_EVS_AppNameBitMaskCmd_t appbitcmd;
    CFE_EVS_AppNameCmd_t        appnamecmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Event Command\n");
#endif

    UT_InitData();

    /* Run the next series of tests with valid, registered application name */
    strncpy((char *) app_name, "ut_cfe_evs", OS_MAX_API_NAME);
    app_name[OS_MAX_API_NAME - 1] = '\0';
    strcpy((char *) appbitcmd.AppName, app_name);
    strcpy((char *) appnamecmd.AppName, app_name);

    /* Test disabling of all events */
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_DEBUG, "FAIL : Debug message disabled");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "FAIL : Info message disabled");
    EventID[2] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_ERROR, "FAIL : Error message disabled");
    EventID[3] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "FAIL : Critical message disabled");
    UT_Report(EventID[0] == -1  && EventID[1] == -1 &&
              EventID[2] == -1 && EventID[3] == -1 &&
              SendMsgEventIDRtn.value == -1,
              "CFE_EVS_SendEvent",
              "Disable all events",
              "11.001");

    /* Test enabling of all events */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    EventID[0] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_DEBUG, "Debug message enabled");
    EventID[1] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_INFORMATION, "Info message enabled");
    EventID[2] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_ERROR, "Error message enabled");
    EventID[3] = SendMsgEventIDRtn.value;
    UT_SetRtnCode(&SendMsgEventIDRtn, -1, 0);
    CFE_EVS_SendEvent(0, CFE_EVS_CRITICAL, "Critical message enabled");
    UT_Report(EventID[0] == CFE_EVS_ENAAPPEVTTYPE_EID &&
              EventID[1] == 0 && EventID[2] == 0 && EventID[3] == 0 &&
              SendMsgEventIDRtn.value == 0,
              "CFE_EVS_SendEvent",
              "Enable all event types",
              "11.002");

    /* Test disabling event type using an illegal type */
    UT_InitData();
    appbitcmd.BitMask = 0xff;
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    CFE_EVS_SendEvent(0, 0xffff, "FAIL : Illegal type disabled");
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Disable events using an illegal event type",
              "11.003");

    /* Test enabling event type using an illegal type */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable events using an illegal event type",
              "11.004");

    /* Test successful disabling of application events */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == -1,
              "CFE_EVS_DisableAppEventsCmd",
              "Disable application events - successful",
              "11.005");

    /* Test successful enabling of application events */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVT_EID,
              "CFE_EVS_EnableAppEventsCmd",
              "Enable application events - successful",
              "11.006");

    /* Test disabling event types (leave debug enabled) */
    UT_InitData();
    bitmaskcmd.BitMask = CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT |
                         CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_DISEVTTYPE_EID,
              "CFE_EVS_DisableEventTypesCmd",
              "Disable event types except debug",
              "11.007");

    /* Test enabling all event types (debug already enabled) */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAEVTTYPE_EID,
              "CFE_EVS_EnableEventTypesCmd",
              "Enable all event types",
              "11.008");

    /* Test successfully resetting the application event counter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_RSTEVTCNT_EID,
              "CFE_EVS_ResetAppEventCounterCmd",
              "Reset event application counter - successful",
              "11.009");

    /* Test disabling an event type using an out of range bit mask */
    UT_InitData();
    bitmaskcmd.BitMask = 0xff;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_BitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableEventTypesCmd",
              "Disable event types - bit mask out of range",
              "11.010");

    /* Test enabling an event type using an out of range bit mask */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &bitmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableEventTypesCmd",
              "Enable event types - bit mask out of range",
              "11.011");

    /* Test disabling an application event type using an out of
     * range bit mask
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_DisableAppEventTypesCmd",
              "Disable application event types - bit mask out of range",
              "11.012");

    /* Test enabling an application event type using an out of
     * range bit mask
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_INVALID_BITMASK_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable application event types - bit mask out of range",
              "11.013");
}

/*
** Test filter commands
*/
void Test_FilterCmd(void)
{
    int                             i;
    char                            app_name[OS_MAX_API_NAME];
    CFE_EVS_AppNameCmd_t            appnamecmd;
    CFE_EVS_AppNameEventIDMaskCmd_t appmaskcmd;
    CFE_EVS_AppNameEventIDCmd_t     appcmdcmd;
    CFE_EVS_AppNameBitMaskCmd_t     appbitcmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Filter Command\n");
#endif

    UT_InitData();

    /* Set up event and mask */
    appmaskcmd.Mask = 0;
    appmaskcmd.EventID = 0;
    appcmdcmd.EventID = 0;

    /* Run the next series of tests with valid, registered application name */
    strncpy((char *) app_name, "ut_cfe_evs", OS_MAX_API_NAME);
    app_name[OS_MAX_API_NAME - 1] = '\0';
    strcpy((char *) appnamecmd.AppName, app_name);
    strcpy((char *) appmaskcmd.AppName, app_name);
    strcpy((char *) appcmdcmd.AppName, app_name);
    strcpy((char *) appbitcmd.AppName, app_name);

    /* Enable all application event message output */
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable all application event types - successful",
              "12.001");

    /* Ensure there is no filter for the next tests */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete event filter - successful",
              "12.002");

    /* Test setting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Set filter - application is not registered for filtering",
              "12.003");

    /* Test resetting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_EVTIDNOREGS_EID,
              "CFE_EVS_ResetFilterCmd",
              "Reset filter - application is not registered for filtering",
              "12.004");


    /* Test resetting all filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_RSTALLFILTER_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Reset all filters - successful",
              "12.005");

    /* Test successfully adding an event filter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - successful",
              "12.006");

    /* Test adding an event filter to an event already registered
     * for filtering
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_EVT_FILTERED_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - event already registered for filtering",
              "12.007");

    /* Test successfully setting a filter mask */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_SETFILTERMSK_EID,
              "CFE_EVS_SetFilterMaskCmd",
              "Set filter mask - successful",
              "12.008");

    /* Test successfully resetting a filter mask */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_RSTFILTER_EID,
              "CFE_EVS_ResetFilterCmd",
              "Reset filter mask - successful",
              "12.009");

    /* Test successfully resetting all filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appnamecmd, CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_RSTALLFILTER_EID,
              "CFE_EVS_ResetAllFiltersCmd",
              "Reset all filters  - successful",
              "12.010");

    /* Test successfully deleting an event filter */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_DELFILTER_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete event filter - successful",
              "12.011");

    /* Test filling the event filters */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));

    for (i = 0; i < CFE_EVS_MAX_EVENT_FILTERS; i++)
    {
        UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
                   CFE_EVS_ADD_EVENT_FILTER_CC);
        appmaskcmd.EventID++;
    }

    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID &&
              SendMsgEventIDRtn.count == CFE_EVS_MAX_EVENT_FILTERS,
              "CFE_EVS_AddEventFilterCmd",
              "Maximum event filters added",
              "12.012");

    /* Test overfilling the event filters */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appmaskcmd, CFE_EVS_CMD_MID,
              CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_MAXREGSFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Maximum event filters exceeded",
              "12.013");

    /* Return application to original state, re-register application */
    UT_InitData();
    appmaskcmd.EventID = 0;
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register application - successful",
              "12.014");

    /* Enable all application event message output */
    UT_InitData();
    appbitcmd.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT |
                        CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameBitMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appbitcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ENAAPPEVTTYPE_EID,
              "CFE_EVS_EnableAppEventTypesCmd",
              "Enable all application event message output",
              "12.015");

    /* Set-up to test filtering the same event twice */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDMaskCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ADDFILTER_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - successful",
              "12.016");

    /* Test filtering the same event again */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_EVT_FILTERED_EID,
              "CFE_EVS_AddEventFilterCmd",
              "Add event filter - event is already registered for filtering",
              "12.017");

    /* Test successful event filer deletion */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_AppNameEventIDCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &appcmdcmd, CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_DELFILTER_EID,
              "CFE_EVS_DeleteEventFilterCmd",
              "Delete filter - successful",
              "12.018");

    /* Return application to original state, re-register application */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER) == CFE_SUCCESS,
              "CFE_EVS_Register",
              "Register application - successful",
              "12.019");
}

/*
** Test commands with invalid command, command packet, and command length
*/
void Test_InvalidCmd(void)
{
    CFE_EVS_ModeCmd_t modecmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Invalid Command\n");
#endif

    modecmd.Mode = CFE_EVS_LOG_OVERWRITE;

    /* Test invalid command packet */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, 0xffff, 0);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_MSGID_EID,
              "CFE_EVS_ProcessGroundCommand",
              "Invalid command packet",
              "13.001");

    /* Test invalid command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               0xffff);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_ERR_CC_EID,
              "CFE_EVS_ProcessGroundCommand",
              "Invalid command",
              "13.002");

    /* Test invalid command length with no op command*/
    UT_InitData();
    UT_SetSBTotalMsgLen(6);
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_NO_OPERATION_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with no op command",
              "13.003");

    /* Test invalid command length with reset counters command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_COUNTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset counters command",
              "13.004");

    /* Test invalid command length with enable event type command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable event type command",
              "13.005");

    /* Test invalid command length with disable event type command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable event type command",
              "13.006");

    /* Test invalid command length with set event format mode command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_EVENT_FORMAT_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set event format mode command",
              "13.007");

    /* Test invalid command length with enable application event
     * type command
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable application event type command",
              "13.008");

    /* Test invalid command length with disable application event
     * type command
     */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENT_TYPE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable application event type command",
              "13.009");

    /* Test invalid command length with enable application events command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable application events command",
              "13.010");

    /* Test invalid command length with disable application events command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_APP_EVENTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable application events command",
              "13.011");

    /* Test invalid command length with reset application counter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_APP_COUNTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset application counter command",
              "13.012");

    /* Test invalid command length with set filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set filter command",
              "13.013");

    /* Test invalid command length with enable ports command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ENABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with enable ports command",
              "13.014");

    /* Test invalid command length with disable ports command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DISABLE_PORTS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with disable ports command",
              "13.015");

    /* Test invalid command length with reset filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset filter command",
              "13.016");

    /* Test invalid command length with reset all filters command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_RESET_ALL_FILTERS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with reset all filters command",
              "13.017");

    /* Test invalid command length with add event filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_ADD_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with add event filter command",
              "13.018");

    /* Test invalid command length with delete event filter command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_DELETE_EVENT_FILTER_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with delete event filter command",
              "13.019");

    /* Test invalid command length with write application data command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_APP_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with write application data command",
              "13.020");

    /* Test invalid command length with write log data command */
    UT_InitData();
    CFE_EVS_GlobalData.EVS_TlmPkt.LogEnabled = TRUE;
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with write log data command",
              "13.021");

    /* Test invalid command length with set log mode command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with set log mode command",
              "13.022");

    /* Test invalid command length with clear log command */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_CLEAR_LOG_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_LEN_ERR_EID,
              "CFE_EVS_VerifyCmdLength",
              "Invalid command length with clear log command",
              "13.023");
}

/*
** Test miscellaneous functionality
*/
void Test_Misc(void)
{
    CFE_EVS_ModeCmd_t modecmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Miscellaneous\n");
#endif

    modecmd.Mode = CFE_EVS_LOG_OVERWRITE;

    /* Test null inputs to exercise EVS_GetApplicationInfo */
    UT_InitData();
    UT_Report(EVS_GetApplicationInfo(NULL, NULL) == CFE_ES_ERR_BUFFER,
              "EVS_GetApplicationInfo",
              "Get application info with null inputs",
              "14.001");

    /* Test successful log data file write */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_LogFileCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_FILE_WRITE_LOG_DATA_CC);
    UT_Report(SendMsgEventIDRtn.value == -1,
              "CFE_EVS_WriteLogFileCmd",
              "Write log data - successful",
              "14.002");

    /* Test successfully setting the logging mode */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_EVS_ModeCmd_t));
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd,
               CFE_EVS_CMD_MID,
               CFE_EVS_SET_LOG_MODE_CC);
    UT_Report(SendMsgEventIDRtn.value == -1,
              "CFE_EVS_SetLoggingModeCmd",
              "Set logging mode - successful",
              "14.003");

    /* Test housekeeping report */
    UT_InitData();
    UT_SendMsg((CFE_SB_MsgPtr_t) &modecmd, CFE_EVS_SEND_HK_MID, 0);
    UT_Report(SendMsgEventIDRtn.value == CFE_EVS_HK_TLM_MID,
              "CFE_EVS_ReportHousekeepingCmd",
              "Housekeeping report - successful",
              "14.004");

    /* Test successful application cleanup */
    UT_InitData();
    UT_Report(CFE_EVS_CleanUpApp(0) == CFE_SUCCESS,
              "CFE_EVS_CleanUpApp",
              "Application cleanup - successful",
              "14.005");

    /* Test registering an application with invalid filter argument */
    UT_InitData();
    UT_Report(CFE_EVS_Register(NULL, 1, 0) == CFE_ES_ERR_BUFFER,
              "CFE_EVS_Register",
              "Register application with invalid arguments",
              "14.006");
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
    CFE_EVS_ProcessCommandPacket(MsgPtr);
}
