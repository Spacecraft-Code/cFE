/*
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** File:
**    time_UT.c
**
** Purpose:
**    Time Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:21:46GMT-05:00 $
** $Revision: 1.7 $
** $Log: time_UT.c  $
** Revision 1.7 2014/05/28 09:21:46GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.6 2012/10/01 18:47:09EDT aschoeni
** Removed relative paths in includes, this is now done by makefile
** Revision 1.5 2012/01/13 13:59:29EST acudmore
** Added license text
** Revision 1.4 2011/12/07 19:19:32EST aschoeni
** Removed returns for TIME and SB for cleaning up apps
** Revision 1.3 2011/11/30 15:09:08EST jmdagost
** Replaced ifdef/ifndef preprocessor tests with if...==TRUE/if...!=TRUE tests
** Revision 1.2 2008/08/06 22:43:48EDT dkobe
** Updated Unit Tests for CFE_TIME_RegisterSynchCallback, CFE_TIME_UnregisterSynchCallback and CFE_TIME_CleanUpApp
** Revision 1.1 2008/04/17 08:05:44EDT ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.9 2007/05/15 15:16:31EDT njyanchik
** Updated unit test for new code
** Revision 1.8 2007/05/04 09:10:25EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.7 2007/05/01 13:28:14EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.6 2006/11/02 13:53:58EST njyanchik
** Unit test for TIME was updated to match the changes made for this DCR
** Revision 1.5 2006/11/01 12:46:54GMT-05:00 njyanchik
** Changed the Unit test to reflect the changes from removing the CDS functionality from TIME
** Revision 1.4 2006/10/30 14:09:28GMT-05:00 njyanchik
** I changed TIME to use the new ES_CDS implementation. This involved using the
** new functions, as well as modifying the CFE_TIME_TaskData structure as well as
** the CFE_TIME_DiagPacket structure. They both made reference to the address
** of the TIME CDS (the old implementation). Now they both use the new
** CFE_ES_CDSHandle_t. Also, the Unit Test for Time was updated. No new paths
** through the code were created, but since return codes from the CDS functions
** changed, there needed to be updates to the UT.
** Revision 1.3 2006/05/31 08:33:15GMT-05:00 jjhageman
** Addition of CFE_TIME_MET2SCTime testing and added case to CFE_TIME_Sub2MicroSec for full coverage.
** Revision 1.2 2006/05/31 08:29:37EDT jjhageman
** Additions for code coverage with different defines
**
*/

/*
** Includes
*/
#include "time_UT.h"

/*
** External global variables
*/
extern uint32              UT_AppID;
extern CFE_ES_ResetData_t  *UT_CFE_ES_ResetDataPtr;
extern CFE_TIME_TaskData_t CFE_TIME_TaskData;
extern OS_time_t           BSP_Time;

extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t ES_CreateChildRtn;
extern UT_SetRtn_t SB_SubscribeRtn;
extern UT_SetRtn_t SB_SubscribeLocalRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t OS_BinSemCreateRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t WriteSysLogRtn;
extern UT_SetRtn_t SetMsgIdRtn;

/*
** Global variables
*/
int32 ut_time_CallbackCalled = 0;


/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int time_main(void)
#endif
{
    /* Initialize unit test */
    UT_Init("time");
    UT_Text("cFE TIME Unit Test Output File\n\n");

    Test_Main();
    Test_Init();
    Test_GetTime();
    Test_TimeOp();
    Test_ConvertTime();
    Test_ConvertCFEFS();
    Test_Print();
    Test_RegisterSynchCallback(TRUE);
    Test_ExternalTone();
    Test_External();
    Test_PipeCmds();
    Test_ResetArea();
    Test_State();
    Test_GetReference();
    Test_Tone();
    Test_1Hz();
    Test_UnregisterSynchCallback();
    Test_CleanUpApp();

    /* Final report on number of errors */
    UT_ReportFailures();
    return 0;
}

/*
** Test task entry point and main process loop
*/
void Test_Main(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Main\n");
#endif

    /* Test successful run through (a pipe read error is expected) */
    UT_InitData();
    CFE_TIME_TaskMain();
    UT_Report(WriteSysLogRtn.value == TIME_SYSLOG_OFFSET + 1,
              "CFE_TIME_TaskMain",
              "Command pipe read error",
              "01.001");

    /* Test with an application initialization failure */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1 ,1);
    CFE_TIME_TaskMain();
    UT_Report(WriteSysLogRtn.value == TIME_SYSLOG_OFFSET + 2,
              "CFE_TIME_TaskMain",
              "Application initialization fail",
              "01.002");
}

/*
** Test API and time task initialization (must be called prior to
** remaining tests)
*/
void Test_Init(void)
{
    int16 ExpRtn;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

    /* Test successful API initialization */
    UT_InitData();
    ExpRtn = 3;
#if (CFE_TIME_CFG_SERVER == TRUE)
    ExpRtn++;
#endif
#if (CFE_TIME_CFG_SERVER == TRUE)
  #if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    ExpRtn++;
  #endif
#endif
#if (CFE_TIME_ENA_1HZ_CMD_PKT == TRUE)
    ExpRtn++;
#endif
    CFE_TIME_EarlyInit();
    UT_Report(SetMsgIdRtn.count == ExpRtn,
              "CFE_TIME_EarlyInit",
              "Successful",
              "02.001");

    /* Test successful time task initialization */
    UT_InitData();
    UT_Report(CFE_TIME_TaskInit() == CFE_SUCCESS,
              "CFE_TIME_Task_Init",
              "Successful",
              "02.002");

    /* Test response to a failure to register an application */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Register application failure",
              "02.003");

    /* Test response to a failure creating the first child task */
    UT_InitData();
    UT_SetRtnCode(&ES_CreateChildRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Child task 1 create failure",
              "02.004");

    /*  Test response to a failure creating the second child task */
    UT_InitData();
    UT_SetRtnCode(&ES_CreateChildRtn, -3, 2);
    UT_Report(CFE_TIME_TaskInit() == -3,
              "CFE_TIME_Task_Init",
              "Child task 2 create failure",
              "02.005");

    /* Test response to an error creating a command pipe */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Create pipe failure",
              "02.006");

    /* Test response to failure of the HK request subscription */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "HK request subscription failure",
              "02.007");

    /* Test response to failure of the task ground command packets
     * subscription
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeRtn, -2, 2);
    UT_Report(CFE_TIME_TaskInit() == -2,
              "CFE_TIME_Task_Init",
              "Task ground command subscription failure",
              "02.008");

    /* Test response to failure of the time at the tone "signal" commands
     * subscription
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -3, 1);
    UT_Report(CFE_TIME_TaskInit() == -3,
              "CFE_TIME_Task_Init",
              "Tone command subscription failure",
              "02.009");

    /* Test response to failure of the time at the tone "data" commands
     * subscription
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -4, 2);
    UT_Report(CFE_TIME_TaskInit() == -4,
              "CFE_TIME_Task_Init",
              "Time data command subscription failure",
              "02.010");

    /* Test response to failure of the fake tone signal commands
     * subscription
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeLocalRtn, -5, 3);
    UT_Report(CFE_TIME_TaskInit() == -5,
              "CFE_TIME_Task_Init",
              "Fake tone signal commands subscription failure",
              "02.011");

    /* Test response to failure creating a tone semaphore */
    UT_InitData();
    UT_SetRtnCode(&OS_BinSemCreateRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Tone semaphore create failure",
              "02.012");

    /* Test response to failure creating a local semaphore */
    UT_InitData();
    UT_SetRtnCode(&OS_BinSemCreateRtn, -2, 2);
    UT_Report(CFE_TIME_TaskInit() == -2,
              "CFE_TIME_Task_Init",
              "Local semaphore create failure",
              "02.013");

    /* Test response to an EVS register failure */
    UT_InitData();
    UT_SetRtnCode(&EVS_RegisterRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "EVS register failure",
              "02.014");

    /* Test response to an error sending an initialization event */
    UT_InitData();
    UT_SetRtnCode(&EVS_SendEventRtn, -1, 1);
    UT_Report(CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Send initialization event error",
              "02.015");
}

/*
** Test functions to retrieve time values
*/
void Test_GetTime(void)
{
    int result;
    uint16 StateFlags, ActFlags;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    /* Note: Time is in seconds + microseconds since 1980-001-00:00:00:00000 */
    /* The time below equals 2013-001-02:03:04.56789 */
    int seconds = 1041472984;
    int microsecs = 567890;
    char *expectedMET = "2013-001-02:03:14.56789";
    char *expectedTAI = "2013-001-03:03:14.56789";
    char *expectedUTC = "2013-001-03:02:42.56789";
    char *expectedSTCF = "1980-001-01:00:00.00000";

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Time\n");
#endif

    /* Test successfully retrieving the mission elapsed time */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_TaskData.AtToneMET.Seconds = 20; /* 20.00000 */
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 3600; /* 01:00:00.00000 */
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLeaps = 32;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0; /* 0.00000 */
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10; /* 10.00000 */
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET; /* Force invalid time */
    CFE_TIME_Print(timeBuf, CFE_TIME_GetMET());
    result = !strcmp(timeBuf, expectedMET);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedMET, timeBuf);
    UT_Report(result,
              "CFE_TIME_GetMET",
              testDesc,
              "03.001");

    /* Test successfully retrieving the mission elapsed time (seconds
     * portion)
     */
    UT_InitData();
    seconds = CFE_TIME_GetMETseconds();
    result = BSP_Time.seconds + CFE_TIME_TaskData.AtToneMET.Seconds -
             CFE_TIME_TaskData.AtToneLatch.Seconds;
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, seconds);
    UT_Report(result == seconds,
              "CFE_TIME_GetMETseconds",
              testDesc,
              "03.002");

    /* Test successfully retrieving the mission elapsed time (sub-seconds
     * portion)
     */
    UT_InitData();
    seconds = CFE_TIME_GetMETsubsecs();
    result = CFE_TIME_Micro2SubSecs(BSP_Time.microsecs) +
             CFE_TIME_TaskData.AtToneMET.Subseconds -
             CFE_TIME_TaskData.AtToneLatch.Subseconds;
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, seconds);
    UT_Report(result == seconds,
              "CFE_TIME_GetMETsubsecs",
              testDesc,
              "03.003");

    /* Test successfully retrieving the leap seconds */
    UT_InitData();
    seconds = CFE_TIME_GetLeapSeconds();
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d",
             CFE_TIME_TaskData.AtToneLeaps, seconds);
    UT_Report(seconds == CFE_TIME_TaskData.AtToneLeaps,
              "CFE_TIME_GetLeapSeconds",
              testDesc,
              "03.004");

    /* Test successfully retrieving the international atomic time (TAI) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTAI());
    result = !strcmp(timeBuf, expectedTAI);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedTAI, timeBuf);
    UT_Report(result,
              "CFE_TIME_GetTAI",
              testDesc,
              "03.005");

    /* Test successfully retrieving the coordinated universal time (UTC) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetUTC());
    result = !strcmp(timeBuf, expectedUTC);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedUTC, timeBuf);
    UT_Report(result,
              "CFE_TIME_GetUTC",
              testDesc,
              "03.006");

    /* Test successfully retrieving the default time (UTC or TAI) */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTime());

#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)
    result = !strcmp(timeBuf, expectedTAI);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = TAI) Expected = %s, actual = %s",
             expectedTAI, timeBuf);
#else
    result = !strcmp(timeBuf, expectedUTC);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = UTC) Expected = %s, actual = %s",
             expectedUTC, timeBuf);
#endif
    UT_Report(result,
              "CFE_TIME_GetTime",
              testDesc,
              "03.007");

    /* Test successfully retrieving the spacecraft time correlation
     * factor (SCTF)
     */
    UT_InitData();
    CFE_TIME_Print(timeBuf, CFE_TIME_GetSTCF());
    result = !strcmp(timeBuf, expectedSTCF);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSTCF, timeBuf);
    UT_Report(result,
              "CFE_TIME_GetSTCF",
              testDesc,
              "03.008");

    /* Test retrieving the time status (invalid time is expected) */
    UT_InitData();
    UT_Report(CFE_TIME_GetClockState() == CFE_TIME_INVALID,
              "CFE_TIME_GetClockState",
              "Invalid time",
              "03.009");

    /* Test successfully converting the clock state data to flag values */
    UT_InitData();
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_WAS_SET;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_TaskData.Forced2Fly = TRUE;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.DelayDirection = CFE_TIME_ADD_ADJUST;
    CFE_TIME_TaskData.IsToneGood = TRUE;
    StateFlags = CFE_TIME_FLAG_CLKSET |
                 CFE_TIME_FLAG_FLYING |
                 CFE_TIME_FLAG_SRCINT |
                 CFE_TIME_FLAG_SIGPRI |
                 CFE_TIME_FLAG_SRVFLY |
                 CFE_TIME_FLAG_CMDFLY |
                 CFE_TIME_FLAG_ADD1HZ |
                 CFE_TIME_FLAG_ADDADJ |
                 CFE_TIME_FLAG_ADDTCL |
                 CFE_TIME_FLAG_GDTONE;

#if (CFE_TIME_CFG_SERVER == TRUE)
    {
        StateFlags |= CFE_TIME_FLAG_SERVER;
    }
#endif

    ActFlags = CFE_TIME_GetClockInfo();
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = 0x%4X, actual = 0x%4X", StateFlags, ActFlags);
    UT_Report(ActFlags == StateFlags,
              "CFE_TIME_GetClockInfo",
              testDesc,
              "03.010");
}

/*
** Test operations on time (add, subtract, compare)
*/
void Test_TimeOp(void)
{
    CFE_TIME_SysTime_t time1, time2, result, exp_result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time Operations\n");
#endif

    /* Initialize to zero time values */
    time1.Subseconds = 0;
    time1.Seconds = 0;
    time2.Subseconds = 0;
    time2.Seconds = 0;
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;

    /* Test adding with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Add(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = 0 seconds/subseconds",
              "04.001");

    /* Test subtracting with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = 0 seconds/subseconds",
              "04.002");

    /* Test comparing with both times equal zero */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = 0 seconds/subseconds",
              "04.003");

    /* Initialize to maximum time values */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffffffff;
    time2.Subseconds = 0xffffffff;
    time2.Seconds = 0xffffffff;

    /* Test adding two maximum time values (extreme time rollover case) */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = maximum seconds/subseconds (rollover)",
              "04.004");

    /* Test subtracting two maximum time values (zero result) */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = maximum seconds/subseconds (zero result)",
              "04.005");

    /* Test comparing two maximum time values */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = maximum seconds/subseconds",
              "04.006");

    /* Initialize to single time value at maximum subseconds */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffff0000;
    time2.Subseconds = 0x00000001;
    time2.Seconds = 0x0000ffff;

    /* Test adding two time values; time A > time B (minimal time
     * rollover case)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A > time B (rollover)",
              "04.007");

    /* Test subtracting two time values; time A > time B */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xfffe0001;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A > time B",
              "04.008");

    /* Test comparing two time values; time A > time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time1, time2) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A > time B",
              "04.009");

    /* Test adding two time values; time A < time B */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A < time B",
              "04.010");

    /* Test subtracting two time values; time A < time B (rollover) */
    UT_InitData();
    exp_result.Subseconds = 0x00000002;
    exp_result.Seconds = 0x0001fffe;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A < time B (rollover)",
              "04.011");

    /* Test comparing two time values; time A < time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time2, time1) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A < time B",
              "04.012");

    /* Initialize so that only subseconds are different; seconds are
     * the same
     */
    time1.Subseconds = 30;
    time1.Seconds = 3;
    time2.Subseconds = 29;
    time2.Seconds = 3;

    /* Test adding two time values; time A subseconds > time B subseconds
     * (seconds same) */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds > time B subseconds (seconds same)",
              "04.013");

    /* Test subtracting two time values; time A subseconds > time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 1;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds > time B subseconds (seconds same)",
              "04.014");

    /* Test comparing two time values; time A subseconds > time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A subseconds > time B subseconds (seconds same)",
              "04.015");

    /* Test adding two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds < time B subseconds (seconds same)",
              "04.016");

    /* Test subtracting two time values; time A subseconds < time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0xffffffff;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds < time B subseconds (seconds same)",
              "04.017");

    /* Test comparing two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A subseconds < time B subseconds (seconds same)",
              "04.018");

    /* Initialize so that only seconds are different; subseconds are
     * the same
     */
    time1.Subseconds = 18;
    time1.Seconds = 8;
    time2.Subseconds = 18;
    time2.Seconds = 7;

    /* Test adding two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds > time B seconds (subseconds same)",
              "04.019");

    /* Test subtracting two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 1;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds > time B seconds (subseconds same)",
              "04.020");

    /* Test comparing two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A seconds > time B seconds (subseconds same)",
              "04.021");

    /* Test adding two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds < time B seconds (subseconds same)",
              "04.022");

    /* Test subtracting two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds < time B seconds (subseconds same)",
              "04.023");

    /* Test comparing two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A seconds < time B seconds (subseconds same)",
              "04.024");
}

/*
** Test time conversion functions
*/
void Test_ConvertTime(void)
{
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    CFE_TIME_SysTime_t METTime;

#if (CFE_TIME_CFG_DEFAULT_TAI == TRUE)
    /* TAI time derived = MET + STCF */
    char *expectedSCTime = "1980-001-02:00:40.00000";
#else
    /* UTC time derived = MET + STCF - Leaps */
    char *expectedSCTime = "1980-001-02:00:08.00000";
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert Time\n");
#endif

    /* Test MET to SCTF conversion */
    UT_InitData();
    METTime.Seconds = 0;
    METTime.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 7240; /* 01:00:00.00000 */
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLeaps = 32;
    CFE_TIME_Print(timeBuf, CFE_TIME_MET2SCTime(METTime));

    /* SC = MET + SCTF [- Leaps for UTC] */
    result = !strcmp(timeBuf, expectedSCTime);
    SNPRINTF(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSCTime, timeBuf);
    UT_Report(result,
              "CFE_TIME_MET2SCTime",
              testDesc,
              "05.001");

    /* Test subseconds to microseconds conversion; zero subsecond value */
    UT_InitData();
    UT_Report(CFE_TIME_Sub2MicroSecs(0) == 0,
              "CFE_TIME_Sub2MicroSecs",
              "Convert 0 subsecond value",
              "05.002");

    /* Test subseconds to microseconds conversion; positive microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(CFE_TIME_Sub2MicroSecs(0xffff) == 16,
              "CFE_TIME_Sub2MicroSecs",
              "+1 microsecond adjustment",
              "05.003");

    /* Test subseconds to microseconds conversion; no microsecond adjust */
    UT_InitData();
    UT_Report(CFE_TIME_Sub2MicroSecs(0x80000000) == 500000,
              "CFE_TIME_Sub2MicroSecs",
              "No microsecond adjustment",
              "05.004");

    /* Test subseconds to microseconds conversion; negative microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(CFE_TIME_Sub2MicroSecs(0x80002000) == 500001,
              "CFE_TIME_Sub2MicroSecs",
              "-1 microsecond adjustment",
              "05.005");

    /* Test subseconds to microseconds conversion; subseconds exceeds
     * microseconds limit
     */
    UT_InitData();
    UT_Report(CFE_TIME_Sub2MicroSecs(0xffffffff) == 999999,
              "CFE_TIME_Sub2MicroSecs",
              "Subseconds exceeds maximum microseconds value (limit ms)",
              "05.006");

    /* Test microseconds to subseconds conversion; zero microsecond value */
    UT_InitData();
    UT_Report(CFE_TIME_Micro2SubSecs(0) == 0,
              "CFE_TIME_Micro2SubSecs",
              "Convert 0 microseconds to 0 subseconds",
              "05.007");

    /* Test microseconds to subseconds conversion; no subsecond adjust */
    UT_InitData();
    UT_Report(CFE_TIME_Micro2SubSecs(0xffff) == 281468928,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (no subsecond adjustment)",
              "05.008");

    /* Test microseconds to subseconds conversion; microseconds below
     * maximum limit
     */
    UT_InitData();
    UT_Report(CFE_TIME_Micro2SubSecs(999998) == 0xffffe000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (subsecond adjustment)",
              "05.009");

    /* Test microseconds to subseconds conversion; microseconds equals
     * maximum limit
     */
    UT_InitData();
    UT_Report(CFE_TIME_Micro2SubSecs(999999) == 0xfffff000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds equals maximum value (subsecond adjustment)",
              "05.010");

    /* Test microseconds to subseconds conversion; microseconds exceeds
     * maximum limit
     */
    UT_InitData();
    UT_Report(CFE_TIME_Micro2SubSecs(0xffffffff) == 0xffffffff,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds exceeds maximum; set maximum subseconds value",
              "05.011");
}

/*
** Test function for converting cFE seconds to file system (FS) seconds and
** vice versa
*/
void Test_ConvertCFEFS(void)
{
    uint32 result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert cFE and FS Seconds\n");
#endif

    /* Test cFE to FS conversion using 0 for the cFE seconds value */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_TIME_FS_FACTOR < 0 && -CFE_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = CFE_TIME_FS_FACTOR;
    }

    UT_Report(CFE_TIME_CFE2FSSeconds(0) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert 0 cFE seconds to FS seconds",
              "06.001");

    /* Test cFE to FS conversion using mid-range value for cFE seconds */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_TIME_FS_FACTOR < 0 && -CFE_TIME_FS_FACTOR > 0xffff)
    {
        result = 0;
    }
    else
    {
        result = CFE_TIME_FS_FACTOR + 0xffff;
    }

    UT_Report(CFE_TIME_CFE2FSSeconds(0xffff) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert mid-range cFE seconds to FS seconds",
              "06.002");

    /* Test cFE to FS conversion using the maximum cFE seconds value */
    UT_InitData();
    UT_Report(CFE_TIME_CFE2FSSeconds(0xffffffff) ==
              (uint32) (CFE_TIME_FS_FACTOR - 1),
              "CFE_TIME_CFE2FSSeconds",
              "Maximum cFE seconds value",
              "06.003");

    /* Test FS to cFE conversion using 0 for the FS seconds value */
    UT_InitData();

    if (CFE_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = -(uint32) CFE_TIME_FS_FACTOR;
    }

    UT_Report(CFE_TIME_FS2CFESeconds(0) == result,
              "CFE_TIME_FS2CFESeconds",
              "Convert 0 FS seconds to cFE seconds",
              "06.004");

    /* Test FS to cFE conversion response to a FS seconds value that results
     * in a negative cFE time (forces cFE seconds to zero)
     */
    UT_InitData();
    UT_Report(CFE_TIME_FS2CFESeconds(CFE_TIME_FS_FACTOR - 1) == 0,
              "CFE_TIME_FS2CFESeconds",
              "Negative cFE seconds conversion (force to zero)",
              "06.005");

    /* Test FS to cFE conversion using the minimum convertible FS
     * seconds value
     */
    UT_InitData();

    if (CFE_TIME_FS_FACTOR > (uint32) (CFE_TIME_FS_FACTOR + 1))
    {
        result = 0;
    }
    else
    {
        result = 1;
    }

    UT_Report(CFE_TIME_FS2CFESeconds(CFE_TIME_FS_FACTOR + 1) == result,
              "CFE_TIME_FS2CFESeconds",
              "Minimum convertible FS seconds value",
              "06.006");

    /* Test FS to cFE conversion using the maximum FS seconds value */
    UT_InitData();
    UT_Report(CFE_TIME_FS2CFESeconds(0xffffffff) == 0xffffffff -
              CFE_TIME_FS_FACTOR,
              "CFE_TIME_FS2CFESeconds",
              "Maximum FS seconds value",
              "06.007");
}

/*
** Test function for creating a text string representing the date and time
**
** NOTE: Test results depend on the epoch values in cfe_mission_cfg.h (the
**       tests below assume an epoch of 1980-001-00:00:00.00000).  Full
**       coverage is possible only when CFE_TIME_EPOCH_SECOND > 0
*/
void Test_Print(void)
{
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    CFE_TIME_SysTime_t time;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Print\n");
#endif

    /* Test with zero time value */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 0;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:00.00000");
    strncat(testDesc," Zero time value", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(result,
              "CFE_TIME_Print",
              testDesc,
              "07.001");

    /* Test with a time value that causes seconds >= 60 when
     * CFE_TIME_EPOCH_SECOND > 0
     */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 59;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:59.00000");
    strncat(testDesc,
            " Seconds overflow if CFE_TIME_EPOCH_SECOND > 0",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(result,
              "CFE_TIME_Print",
              testDesc,
              "07.002");

    /* Test with mission representative time values */
    UT_InitData();
    time.Subseconds = 215000;
    time.Seconds = 1041472984;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2013-001-02:03:04.00005");
    strncat(testDesc," Mission representative time", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(result,
              "CFE_TIME_Print",
              testDesc,
              "07.003");

    /* Test with maximum seconds and subseconds values */
    UT_InitData();
    time.Subseconds = 0xffffffff;
    time.Seconds = 0xffffffff;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2116-038-06:28:15.99999");
    strncat(testDesc," Maximum seconds/subseconds values",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(result,
              "CFE_TIME_Print",
              testDesc,
              "07.004");
}

/*
** Test function for use with register and unregister synch callback API tests
*/
void ut_time_MyCallbackFunc(void)
{
    ut_time_CallbackCalled++;
}

/*
** Test registering a synchronization callback function
*/
void Test_RegisterSynchCallback(boolean reportResults)
{
    uint32  i = 0;
    int32   Result = CFE_SUCCESS;
    boolean SuccessfulTestResult = TRUE;
    boolean UnsuccessfulTestResult = TRUE;

#ifdef UT_VERBOSE
    if (reportResults)
    {
        UT_Text("Begin Test Register Synch Callback\n");
    }
#endif

    /* Test registering the callback function the maximum number of times,
     * then attempt registering one more time
     */
    UT_InitData();

    /* Register callback function one too many times to test all cases */
    for (i = 0; i <= CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Result = CFE_TIME_RegisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                                &ut_time_MyCallbackFunc);

        if (i < CFE_TIME_MAX_NUM_SYNCH_FUNCS && Result != CFE_SUCCESS)
        {
            SuccessfulTestResult = FALSE;
        }
        else if (i == CFE_TIME_MAX_NUM_SYNCH_FUNCS &&
                 Result != CFE_TIME_TOO_MANY_SYNCH_CALLBACKS)
        {
            UnsuccessfulTestResult = FALSE;
        }
    }

    if (reportResults)
    {
        UT_Report(SuccessfulTestResult,
                  "CFE_TIME_RegisterSynchCallback",
                  "Successfully registered callbacks",
                  "08.001");

        UT_Report(UnsuccessfulTestResult,
                  "CFE_TIME_RegisterSynchCallback",
                  "Unsuccessfully registered callback(s)",
                  "08.002");
    }
}

/*
** Test external tone signal detection
*/
void Test_ExternalTone(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test External Tone\n");
#endif

    UT_InitData();
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_ExternalTone();
    UT_Report(CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_ExternalTone",
              "External tone",
              "09.001");
}

/*
** Test setting time data from an external source
*/
void Test_External(void)
{
#if (CFE_TIME_CFG_SRC_MET == TRUE || \
     CFE_TIME_CFG_SRC_GPS == TRUE || \
     CFE_TIME_CFG_SRC_TIME == TRUE)

    CFE_TIME_SysTime_t settime;

    settime.Seconds = 5;
    settime.Subseconds = 4;
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test External Time Set\n");
#endif

    /* Test setting time data from MET */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_EXTERN;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_NOT_SET;

#if (CFE_TIME_CFG_SRC_MET == TRUE)
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_ExternalMET(settime);
    UT_Report(CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET",
              "10.001");
#else
    UT_Report(TRUE,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET",
              "10.001");
#endif

    /* Test setting time data from GPS */
    UT_InitData();

#if (CFE_TIME_CFG_SRC_GPS == TRUE)
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS",
              "10.002");
#else
    UT_Report(TRUE,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS",
              "10.002");
#endif

    /* Test setting time data from an external source (e.g., spacecraft) */
    UT_InitData();

#if (CFE_TIME_CFG_SRC_TIME == TRUE)
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_ExternalTime(settime);
    UT_Report(CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External time",
              "10.003");
#else
    UT_Report(TRUE,
              "CFE_TIME_ExternalTime",
              "*Not tested* External time",
              "10.003");
#endif

    /* Reset to normal value for subsequent tests */
    CFE_TIME_TaskData.ClockSource = CFE_TIME_USE_INTERN;
}

/*
** Test processing command pipe messages
*/
void Test_PipeCmds(void)
{
    int flag;
    uint32 count;
    CFE_SB_MsgPtr_t msgptr;
    CFE_SB_Msg_t message;
    CFE_TIME_ToneDataCmd_t tonedatacmd;
    CFE_TIME_StateCmd_t statecmd;
    CFE_TIME_SourceCmd_t sourcecmd;
    CFE_TIME_SignalCmd_t signalcmd;
    CFE_TIME_TimeCmd_t timecmd;
    CFE_TIME_LeapsCmd_t leapscmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Pipe Commands\n");
#endif

    /* Test sending the housekeeping telemetry request command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_SEND_HK_MID, 0);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_HK_TLM_MID,
              "CFE_TIME_HousekeepingCmd",
              "Housekeeping telemetry request",
              "11.001");

    /* Test sending the time at the tone "signal" command */
    UT_InitData();
    CFE_TIME_TaskData.ToneSignalCount = 0;
    UT_SendMsg(&message, CFE_TIME_TONE_CMD_MID, 0);
    UT_Report(CFE_TIME_TaskData.ToneSignalCount == 1,
              "CFE_TIME_ToneSignalCmd",
              "Time at tone signal",
              "11.002");

    /* Test sending the time at the tone "data" command */
    UT_InitData();
    CFE_TIME_TaskData.ToneDataCount = 0;
    msgptr = (CFE_SB_MsgPtr_t) &tonedatacmd;
    UT_SendMsg(msgptr, CFE_TIME_DATA_CMD_MID, 0);
    UT_Report(CFE_TIME_TaskData.ToneDataCount == 1,
              "CFE_TIME_ToneDataCmd",
              "Time at tone data",
              "11.003");

    /* Test sending the simulate time at the tone "signal" command */
    UT_InitData();
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    UT_SendMsg(&message, CFE_TIME_FAKE_CMD_MID, 0);

#if (CFE_TIME_CFG_FAKE_TONE == TRUE)
    UT_Report(CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_FakeToneCmd",
              "Simulated time at tone signal",
              "11.004");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
              "CFE_TIME_FakeToneCmd",
              "*Not tested* Simulated time at tone signal",
              "11.004");
#endif

    /* Test sending the request time at the tone "data" command */
    UT_InitData();
    flag = FALSE;
    count = CFE_TIME_TaskData.InternalCount;
    UT_SendMsg(&message, CFE_TIME_SEND_CMD_MID, 0);

#if (CFE_TIME_CFG_SERVER == TRUE)
 #if (CFE_TIME_CFG_SOURCE != TRUE)
  #if (CFE_TIME_CFG_FAKE_TONE != TRUE)
    flag = TRUE;;
  #endif
 #endif
#endif

    if (flag)
    {
        UT_Report(SendMsgEventIDRtn.value != CFE_TIME_ID_ERR_EID &&
                count == CFE_TIME_TaskData.InternalCount + 1,
                  "CFE_TIME_ToneSendCmd",
                  "Request time at tone data",
                  "11.005");
    }
    else
    {
        UT_Report(SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
                  "CFE_TIME_ToneSendCmd",
                  "*Not tested* Request time at tone data",
                  "11.005");
    }

    /* Test sending the no-op command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_NOOP_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_NOOP_EID,
              "CFE_TIME_NoopCmd",
              "No-op",
              "11.006");

    /* Test sending the reset counters command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_RESET_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_RESET_EID,
              "CFE_TIME_ResetCmd",
              "Reset counters",
              "11.007");

    /* Test sending the request diagnostics command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, CFE_TIME_DIAG_TLM_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DIAG_EID,
              "CFE_TIME_DiagCmd",
              "Request diagnostics",
              "11.008");

    /* Test sending a clock state = invalid command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t)&statecmd;
    statecmd.ClockState = CFE_TIME_INVALID;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = invalid",
              "11.009");

    /* Test sending a clock state = valid command */
    UT_InitData();
    statecmd.ClockState = CFE_TIME_VALID;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = valid",
              "11.010");

    /* Test sending a clock state = flywheel command */
    UT_InitData();
    statecmd.ClockState = CFE_TIME_FLYWHEEL;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STATE_EID,
              "CFE_TIME_SetStateCmd",
              "Set clock state = flywheel",
              "11.011");

    /* Test response to sending a clock state command using an
     * invalid state
     */
    UT_InitData();
    statecmd.ClockState = 99;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STATE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STATE_ERR_EID,
              "CFE_TIME_SetStateCmd",
              "Invalid clock state",
              "11.012");

    /* Test sending the set time source = internal command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &sourcecmd;
    sourcecmd.TimeSource = CFE_TIME_USE_INTERN;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SOURCE_EID,
              "CFE_TIME_SetSourceCmd",
              "Set internal source",
              "11.013");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SOURCE_CFG_EID,
              "CFE_TIME_SetSourceCmd",
              "Set internal source invalid",
              "11.013");
#endif

    /* Test sending the set time source = external command */
    UT_InitData();
    sourcecmd.TimeSource = CFE_TIME_USE_EXTERN;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SOURCE_EID,
              "CFE_TIME_SetSourceCmd",
              "Set external source",
              "11.014");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SOURCE_CFG_EID,
              "CFE_TIME_SetSourceCmd",
              "Set external source invalid",
              "11.014");
#endif

    /* Test response to sending a set time source command using an
     * invalid source
     */
    UT_InitData();
    sourcecmd.TimeSource = -1;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SOURCE_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SOURCE_ERR_EID,
              "CFE_TIME_SetSourceCmd",
              "Invalid time source",
              "11.015");

    /* Test sending a set tone signal source = primary command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &signalcmd;
    signalcmd.ToneSource = CFE_TIME_TONE_PRI;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_EID,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = primary",
              "11.016");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_CFG_EID,
              "CFE_TIME_SetSignalCmd",
              "Set tone source = primary invalid",
              "11.016");
#endif

    /* Test sending a set tone signal source = redundant command */
    UT_InitData();
    signalcmd.ToneSource = CFE_TIME_TONE_RED;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_EID,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant",
              "11.017");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_CFG_EID,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant invalid",
              "11.017");
#endif

    /* Test response to sending a set tone signal source command using an
     * invalid source
     */
    UT_InitData();
    signalcmd.ToneSource = -1;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_SIGNAL_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_SIGNAL_ERR_EID,
              "CFE_TIME_SetSignalCmd",
              "Invalid tone source",
              "11.018");

    /* Test sending a time tone add delay command */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &timecmd;
    timecmd.MicroSeconds = 0;
    timecmd.Seconds = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_DELAY_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELAY_EID,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay",
              "11.019");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELAY_CFG_EID,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay invalid",
              "11.019");
#endif

    /* Test sending a time tone subtract delay command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_DELAY_CC);

#if (CFE_TIME_CFG_SOURCE == TRUE)
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELAY_EID,
              "CFE_TIME_SetDelayCmd",
              "Set tone subtract delay",
              "11.020");
#else
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELAY_CFG_EID,
              "CFE_TIME_SetDelayCmd",
              "Set subtract delay invalid",
              "11.020");
#endif

    /* Test sending a set time command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_TIME_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_TIME_EID,
              "CFE_TIME_SetTimeCmd",
              "Set time",
              "11.021");

    /* Test sending a set MET command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_MET_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_MET_EID,
              "CFE_TIME_SetMETCmd",
              "Set MET",
              "11.022");

    /* Test sending a set STCF command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STCF_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STCF_EID,
              "CFE_TIME_SetSTCFCmd",
              "Set STCF",
              "11.023");

    /* Test sending an adjust STCF positive command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_ADJUST_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELTA_EID,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust positive",
              "11.024");

    /* Test sending an adjust STCF negative command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_ADJUST_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELTA_EID,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust negative",
              "11.025");

    /* Test sending an adjust STCF 1 Hz positive command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_1HZADJ_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_1HZ_EID,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust positive",
              "11.026");

    /* Test sending an adjust STCF 1 Hz negative command */
    UT_InitData();
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SUB_1HZADJ_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_1HZ_EID,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust negative",
              "11.027");

    /* Test response to sending a tone delay command using an invalid time */
    UT_InitData();
    timecmd.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_DELAY_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELAY_ERR_EID,
              "CFE_TIME_SetDelayCmd",
              "Invalid tone delay",
              "11.028");

    /* Test response to sending a set time command using an invalid time */
    UT_InitData();
    timecmd.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_TIME_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_TIME_ERR_EID,
              "CFE_TIME_SetTimeCmd",
              "Invalid time",
              "11.029");

    /* Test response to sending a set MET command using an invalid time */
    UT_InitData();
    timecmd.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_MET_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_MET_ERR_EID,
              "CFE_TIME_SetMETCmd",
              "Invalid MET",
              "11.030");

    /* Test response to sending a set STCF command using an invalid time */
    UT_InitData();
    timecmd.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_STCF_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_STCF_ERR_EID,
              "CFE_TIME_SetSTCFCmd",
              "Invalid STCF",
              "11.031");

    /* Test response to sending an adjust STCF command using an invalid time */
    UT_InitData();
    timecmd.MicroSeconds = 1000001;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_ADD_ADJUST_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_DELTA_ERR_EID,
              "CFE_TIME_AdjustCmd",
              "Invalid STCF adjust",
              "11.032");

    /* Test sending a set leap seconds commands */
    UT_InitData();
    msgptr = (CFE_SB_MsgPtr_t) &leapscmd;
    leapscmd.LeapSeconds = 0;
    UT_SendMsg(msgptr, CFE_TIME_CMD_MID, CFE_TIME_SET_LEAPS_CC);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_LEAPS_EID,
              "CFE_TIME_SetLeapsCmd",
              "Set leap seconds",
              "11.033");

    /* Test response to sending an invalid command */
    UT_InitData();
    UT_SendMsg(&message, CFE_TIME_CMD_MID, 0xffff);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_CC_ERR_EID,
              "CFE_TIME_TaskPipe",
              "Invalid command code",
              "11.034");

    /* Test response to sending a command using an invalid message ID */
    UT_InitData();
    UT_SendMsg(&message, 0xffff, 0);
    UT_Report(SendMsgEventIDRtn.value == CFE_TIME_ID_ERR_EID,
              "CFE_TIME_TaskPipe",
              "Invalid message ID",
              "11.035");
}

/*
** Test the reset area functionality
*/
void Test_ResetArea(void)
{
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Reset Area\n");
#endif

    /* Test successfully updating the reset area */
    UT_InitData();
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_TONE_PRI;
    UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal = CFE_TIME_TONE_RED;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(UT_CFE_ES_ResetDataPtr->TimeResetVars.ClockSignal ==
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Successful update",
              "12.001");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone PRI",
              "12.002");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_RED);
    CFE_TIME_QueryResetVars();
    UT_Report(CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone RED",
              "12.003");

    /* Test response to a bad reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_BAD,
              "CFE_TIME_QueryResetVars",
              "Reset area error",
              "12.004");

    /* Test initializing to default time values */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE + 1,
                             CFE_TIME_TONE_PRI);
    CFE_TIME_QueryResetVars();
    UT_Report(CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_NEW,
              "CFE_TIME_QueryResetVars",
              "Initialize to default values",
              "12.005");
}

/*
** Test time state functions
*/
void Test_State(void)
{
    uint16 flag;
    int16  ExpState;
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time State\n");
#endif

    /* Test determining if the clock state = valid (flywheel if
     * CFE_TIME_CFG_CLIENT is TRUE)
     */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_WAS_SET;
    Reference.ClockFlyState = CFE_TIME_NO_FLY;
#if (CFE_TIME_CFG_CLIENT == TRUE)
    ExpState = CFE_TIME_FLYWHEEL;
#else
    ExpState = CFE_TIME_VALID;
#endif
    UT_Report(CFE_TIME_CalculateState(&Reference) == ExpState,
              "CFE_TIME_CalculateState",
              "Valid time state",
              "13.001");

    /* Test determining if the clock state = flywheel */
    UT_InitData();
    Reference.ClockFlyState = CFE_TIME_IS_FLY;
    UT_Report(CFE_TIME_CalculateState(&Reference) == CFE_TIME_FLYWHEEL,
              "CFE_TIME_CalculateState",
              "Flywheel time state",
              "13.002");

    /* Test determining if the clock state = invalid */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_INVALID;
    UT_Report(CFE_TIME_CalculateState(&Reference) == CFE_TIME_INVALID,
              "CFE_TIME_CalculateState",
              "Invalid time state",
              "13.003");

    /* Test converting state data to flag values */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_VALID);
    CFE_TIME_SetState(CFE_TIME_FLYWHEEL);
    flag = CFE_TIME_FLAG_CLKSET | CFE_TIME_FLAG_FLYING |
           CFE_TIME_FLAG_SRCINT | CFE_TIME_FLAG_SIGPRI |
           CFE_TIME_FLAG_CMDFLY;

    /* Add server or client flag depending on configuration */
#if (CFE_TIME_CFG_SERVER == TRUE)
    flag |= CFE_TIME_FLAG_SERVER | CFE_TIME_FLAG_SRVFLY |
            CFE_TIME_FLAG_ADDTCL;
#else
    flag |= CFE_TIME_FLAG_ADDADJ | CFE_TIME_FLAG_ADD1HZ;
#endif

    UT_Report(CFE_TIME_GetStateFlags() == flag,
              "CFE_TIME_GetStateFlags",
              "State data to flag conversion",
              "13.004");
}

/*
** Test getting reference data (time at "tone")
*/
void Test_GetReference(void)
{
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Reference\n");
#endif

    /* Test with local clock rollover */
    UT_InitData();
    CFE_TIME_TaskData.AtToneMET.Seconds = 20;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 1000;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + MaxLocalClock - AtToneLatch +
     *              BSP_Time [+ or - AtToneDelay]
     */
    UT_Report(Reference.CurrentMET.Seconds == 1010 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock < latch at tone time",
              "14.001");

    /* Test without local clock rollover */
    UT_InitData();
    CFE_TIME_TaskData.AtToneMET.Seconds = 20;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 10;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(15, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + BSP_Time - AtToneLatch [+ or - AtToneDelay]
     */
    UT_Report(Reference.CurrentMET.Seconds == 25 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock > latch at tone time",
              "14.002");
}

/*
** Test send tone, and validate tone and data packet functions
*/
void Test_Tone(void)
{
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;
    int seconds = 7654321;
    int virtualSeconds = 1234567;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Tone\n");
#endif

    /* Test time at the tone in flywheel mode */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_FLYWHEEL);

#if (CFE_TIME_CFG_SERVER == TRUE)
    UT_SetBSP_Time(seconds, 0);
    CFE_TIME_TaskData.AtToneMET.Seconds = 0; /* 20.00000 */
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0; /* 0.00000 */
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0; /* 10.00000 */
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.VirtualMET = virtualSeconds;
    CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Subseconds = seconds;
    CFE_TIME_ToneSend();

#if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    seconds++;
#endif

    UT_Report(CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Seconds == seconds &&
              CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel ON",
              "15.001");

    /* Test time at the tone when not in flywheel mode */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_ToneSend();

#if (CFE_TIME_AT_TONE_WILL_BE == TRUE)
    virtualSeconds++;
#endif

    UT_Report(CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Seconds ==
                  virtualSeconds &&
              CFE_TIME_TaskData.ToneDataCmd.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel OFF",
              "15.002");

#else
    UT_Report(TRUE,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel ON",
              "15.001"); /* Added to prevent a missing test # */

    UT_Report(TRUE,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel OFF",
              "15.002"); /* Added to prevent a missing test # */
#endif

    time1.Seconds = 10;
    time1.Subseconds = 0;
    time2.Seconds = 10;
    time2.Subseconds = 100;

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * FALSE.  This test also serves to set the internal PrevTime variables
     * to known values
     */
    UT_InitData();
    CFE_TIME_TaskData.VersionCount = 0; /* Verifies 'ForcedToFly' path */
    CFE_TIME_TaskData.ToneMatchCount = 0;
    CFE_TIME_TaskData.Forced2Fly = FALSE; /* Exercises '!ForcedToFly' path */
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(CFE_TIME_TaskData.VersionCount == 1 &&
              CFE_TIME_TaskData.ToneMatchCount == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 < time 2, Forced2Fly FALSE",
              "15.003");

    /* Test tone validation when time 1 equals the previous time 1 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time1, time1);
    UT_Report(CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 same as previous time 1",
              "15.004");

    /* Test tone validation when time 2 equals the previous time 2 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Time 2 same as previous time 2",
              "15.005");

    /* Test tone validation with time 1 > time 2 value (clock rollover) */
    UT_InitData();
    time1.Seconds = 20;
    time2.Seconds = 0;
    time1.Subseconds = 0;
    time2.Subseconds = 100;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 20; /* 1000.00000 */
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchCount = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(CFE_TIME_TaskData.ToneMatchCount == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 > time 2 (clock rollover)",
              "15.006");

    /* Test tone validation when time between packet and tone is out
     * of limits
     */
    UT_InitData();
    time1.Seconds = 10;
    time2.Seconds = 11;
    time1.Subseconds = 0;
    time2.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchErrors = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(CFE_TIME_TaskData.ToneMatchErrors == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits",
              "15.007");
}

/*
** Test the 1Hz STCF adjustment, local interrupt service request and task
** (non-tone), and tone task functions
*/
void Test_1Hz(void)
{
    uint32             delSec = 3;
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;

#if (CFE_TIME_CFG_SERVER == TRUE)
    /* Test 1Hz STCF adjustment in positive direction */
    UT_InitData();
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 20;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 60;
    time1.Seconds = 10;
    time1.Subseconds = 30;
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_ADD_ADJUST);
    CFE_TIME_Local1HzISR();
    UT_Report(CFE_TIME_TaskData.AtToneSTCF.Seconds == 30 &&
              CFE_TIME_TaskData.AtToneSTCF.Subseconds == 90,
              "CFE_TIME_Set1HzAdj",
              "Positive adjustment",
              "16.001");

    /* Test 1Hz STCF adjustment in negative direction */
    UT_InitData();
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_SUB_ADJUST);
    CFE_TIME_Local1HzISR();
    UT_Report(CFE_TIME_TaskData.AtToneSTCF.Seconds == 20 &&
              CFE_TIME_TaskData.AtToneSTCF.Subseconds == 60,
              "CFE_TIME_Set1HzAdj",
              "Negative adjustment",
              "16.002");

    /* Test local 1Hz interrupt when enough time has elapsed since receiving a
     * time update to automatically change the state to flywheel
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_NO_FLY;
    CFE_TIME_TaskData.AutoStartFly = FALSE;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 1;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzISR();
    UT_Report(CFE_TIME_TaskData.AutoStartFly == TRUE,
              "CFE_TIME_Local1HzISR",
              "Auto start flywheel",
              "16.003");

    /* Test local 1Hz interrupt when enough time has elapsed since receiving a
     * time update to automatically update the MET
     */
    UT_InitData();
    time1.Seconds = 11;
    time2.Seconds = 1;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = time1.Seconds;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = time2.Seconds;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.MaxLocalClock.Seconds = CFE_TIME_CFG_LATCH_FLY + delSec;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ClockFlyState = CFE_TIME_IS_FLY;
    CFE_TIME_Local1HzISR();
    UT_Report(CFE_TIME_TaskData.AtToneMET.Seconds == time1.Seconds +
                                                     CFE_TIME_CFG_LATCH_FLY +
                                                     delSec - time2.Seconds &&
              CFE_TIME_TaskData.AtToneLatch.Seconds == 0,
              "CFE_TIME_Local1HzISR",
              "Auto update MET",
              "16.004");

#else
    /* These prevent missing test numbers when CFE_TIME_CFG_SERVER is FALSE */
    UT_Report(TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Positive adjustment",
              "16.001");
    UT_Report(TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Negative adjustment",
              "16.002");
    UT_Report(TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Auto start flywheel",
              "16.003");
    UT_Report(TRUE,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Auto update MET",
              "16.004");
#endif

    /* Test the tone signal ISR when the tone doesn't occur ~1 second after
     * the previous one
     */
    UT_InitData();
    CFE_TIME_TaskData.IsToneGood = TRUE;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 1;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(CFE_TIME_TaskData.IsToneGood == FALSE,
              "CFE_TIME_Tone1HzISR",
              "Invalid tone signal interrupt",
              "16.005");

    /* Test the tone signal ISR call to the application synch callback
     * function by verifying the number of callbacks made matches the number
     * of registered callbacks
     */
    UT_InitData();
    UT_SetBSP_Time(1, 0);
    CFE_TIME_Tone1HzISR();
    UT_Report(ut_time_CallbackCalled == CFE_TIME_MAX_NUM_SYNCH_FUNCS,
              "CFE_TIME_Tone1HzISR",
              "Proper number of callbacks made",
              "16.006");

    /* Test the local 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.LocalTaskCount = 0;
    UT_SetBinSemFail(2);
    CFE_TIME_Local1HzTask();
    UT_Report(CFE_TIME_TaskData.LocalTaskCount == 1,
              "CFE_TIME_Local1HzTask",
              "Semaphore creation pass, then fail",
              "16.007");

    /* Test the tone 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.ToneTaskCount = 0;
    UT_SetBinSemFail(2);
    CFE_TIME_Tone1HzTask();
    UT_Report(CFE_TIME_TaskData.ToneTaskCount == 1,
              "CFE_TIME_Tone1HzTask",
              "Semaphore creation pass, then fail",
              "16.008");
}

/*
** Test unregistering synchronization callback function
*/
void Test_UnregisterSynchCallback(void)
{
    uint32  i = 0;
    int32   Result = CFE_SUCCESS;
    boolean SuccessfulTestResult = TRUE;
    boolean UnsuccessfulTestResult = TRUE;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregister Callback\n");
#endif

    /* Unregister callback function one too many times to test both valid and
     * invalid cases
     */
    UT_InitData();

    for (i = 0; i <= CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Result =
            CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                             &ut_time_MyCallbackFunc);

        if (i < CFE_TIME_MAX_NUM_SYNCH_FUNCS && Result != CFE_SUCCESS)
        {
            SuccessfulTestResult = FALSE;
        }
        else if (i == CFE_TIME_MAX_NUM_SYNCH_FUNCS &&
                 Result != CFE_TIME_CALLBACK_NOT_REGISTERED)
        {
            UnsuccessfulTestResult = FALSE;
        }
    }

    UT_Report(SuccessfulTestResult,
              "CFE_TIME_UnregisterSynchCallback",
              "Successfully unregister callback",
              "17.001");

    UT_Report(UnsuccessfulTestResult,
              "CFE_TIME_UnregisterSynchCallback",
              "Unsuccessful unregister callback",
              "17.002");
    return;
}

/*
** Test function to free resources associated with an application
*/
void Test_CleanUpApp(void)
{
    uint16   i;
    uint16 Status1 = 0;
    int32  Status2 = CFE_SUCCESS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Cleanup App\n");
#endif

    UT_InitData();

    /* Add multiple entries into callback registry table */
    Test_RegisterSynchCallback(FALSE);

    /* Clean up callbacks */
    CFE_TIME_CleanUpApp(UT_AppID);

    for (i = 0; i < CFE_TIME_MAX_NUM_SYNCH_FUNCS; i++)
    {
        Status1 += CFE_TIME_TaskData.SynchCallback[i].App;
    }

    /* Try to unregister an entry again to make sure all are gone */
    Status2 = CFE_TIME_UnregisterSynchCallback((CFE_TIME_SynchCallbackPtr_t)
                                               &ut_time_MyCallbackFunc);
    UT_Report(Status1 == 0 && Status2 == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_CleanUpApp",
              "Removed all callback entries for app",
              "18.001");
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
  CFE_TIME_TaskPipe(MsgPtr);
}
