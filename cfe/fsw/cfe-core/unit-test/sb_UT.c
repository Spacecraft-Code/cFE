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
**    sb_UT.c
**
** Purpose:
**    Software Bus Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:21:54GMT-05:00 $
** $Revision: 1.15 $
**
*/

/*
** Includes
*/
#include "sb_UT.h"

/*
** External global variables
*/
extern char cMsg[];

extern cfe_sb_t CFE_SB;

extern UT_SetRtn_t QueueCreateRtn;
extern UT_SetRtn_t QueuePutRtn;
extern UT_SetRtn_t QueueGetRtn;
extern UT_SetRtn_t PoolCreateExRtn;
extern UT_SetRtn_t GetPoolRtn;
extern UT_SetRtn_t GetPoolInfoRtn;
extern UT_SetRtn_t PutPoolRtn;
extern UT_SetRtn_t MutSemCreateRtn;
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t FSWriteHdrRtn;
extern UT_SetRtn_t FileWriteRtn;

/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int sb_main(void)
#endif
{
    /* Initialize unit test */
    UT_Init("sb");
    UT_Text("cFE SB Unit Test Output File\n\n");

    Test_SB_App();
    Test_SB_Lib();
    Test_SB_SpecialCases();
    UT_CheckForOpenSockets();

    UT_ReportFailures();
    return 0;
} /* end main */

/*
** Reset variable values and sockets prior to a test
*/
void SB_ResetUnitTest(void)
{
    /* If any sockets were left open then report and close them */
    UT_CheckForOpenSockets();
    UT_InitData();
    CFE_SB_EarlyInit();
} /* end SB_ResetUnitTest */

/*
** Function for calling SB application test functions
*/
void Test_SB_App(void)
{
    Test_SB_AppInit();
    Test_SB_MainRoutine();
    Test_SB_Cmds();
} /* end Test_SB_App */

/*
** Function for calling SB application initialization
** test functions
*/
void Test_SB_AppInit(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test App Init");
#endif

    Test_SB_AppInit_ESRegFail();
    Test_SB_AppInit_EVSRegFail();
    Test_SB_AppInit_EVSSendEvtFail();
    Test_SB_AppInit_CrPipeFail();
    Test_SB_AppInit_Sub1Fail();
    Test_SB_AppInit_Sub2Fail();
    Test_SB_AppInit_GetPoolFail();
    Test_SB_AppInit_PutPoolFail();

#ifdef UT_VERBOSE
    UT_Text("End Test App Init");
#endif
} /* end Test_SB_AppInit */

/*
** Test task init with ES_RegisterApp returning error
*/
void Test_SB_AppInit_ESRegFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 ForcedRtnVal = -1;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Start of ES_RegisterApp Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&ES_RegisterRtn, ForcedRtnVal, 1);
    ExpRtn = ForcedRtnVal;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ForcedRtnVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from AppInit in ES reg error test, "
                   "exp=%ld, act= %ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act= %ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_AppInit", "ES_RegisterApp failure", "01.001");
} /* end Test_SB_AppInit_ESRegFail */

/*
** Test task init with EVS_Register returning error
*/
void Test_SB_AppInit_EVSRegFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;
    int32 ForcedRtnVal = -1;

#ifdef UT_VERBOSE
    UT_Text("Start of EVS_Register Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&EVS_RegisterRtn, ForcedRtnVal, 1);
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ForcedRtnVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from AppInit in EVS Reg Error test, "
                   "exp=%ld, act= %ld",
                 ForcedRtnVal, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act= %ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_AppInit", "EVS register failure", "01.002");
} /* end Test_SB_AppInit_EVSRegFail */

/*
** Test task init with EVS_SendEvent returning error on task init event
*/
void Test_SB_AppInit_EVSSendEvtFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;
    int32 ForcedRtnVal = -1;

#ifdef UT_VERBOSE
    UT_Text("Start of EVS_SendEvent Failure Test");
#endif

    SB_ResetUnitTest();

    /* Use 4 because first three events are pipe created (1) and subscription
     * rcvd (2). Fourth is SB initialized
     */
    UT_SetRtnCode(&EVS_SendEventRtn, ForcedRtnVal, 4);
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ForcedRtnVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from AppInit in EVSSendEvtFail test, "
                   "exp=0x%lx, act= 0x%lx",
                 (uint32) ForcedRtnVal, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act= %ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(CFE_SB.CmdPipe);
    UT_Report(TestStat, "CFE_SB_AppInit", "EVS SendEvent failure", "01.003");
} /* end Test_SB_AppInit_EVSSendEvtFail */

/*
** Test task init with pipe create failure
*/
void Test_SB_AppInit_CrPipeFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

    /* To fail the pipe create, call the SB_AppInit function twice.  The
     * first call to SB_AppInit should pass and the second call should fail
     * because it is an error to use the same pipe name when creating pipes
     */
#ifdef UT_VERBOSE
    UT_Text("Start of SB_CreatePipe Failure Test");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in CreatePipe Error1 "
                   "test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = CFE_SB_PIPE_CR_ERR;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in CreatePipe Error2 "
                   "test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(0);
    UT_Report(TestStat, "CFE_SB_AppInit", "Create pipe failure", "01.004");
} /* end Test_SB_AppInit_CrPipeFail */

/*
** Test task init with a failure on first subscription request
*/
void Test_SB_AppInit_Sub1Fail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Start of Subscription 1 Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&GetPoolRtn, -1, 1);
    ExpRtn = CFE_SB_BUF_ALOC_ERR;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in Sub1 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DEST_BLK_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_DEST_BLK_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_AppInit", "Subscription 1 failure", "01.005");
} /* end Test_SB_AppInit_Sub1Fail */

/*
** Test task init with a failure on second subscription request
*/
void Test_SB_AppInit_Sub2Fail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Start of Subscription 2 Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&GetPoolRtn, -1, 2);
    ExpRtn = CFE_SB_BUF_ALOC_ERR;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in Sub2 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DEST_BLK_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_DEST_BLK_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_AppInit", "Subscription 2 failure", "01.006");
} /* end Test_SB_AppInit_Sub2Fail */

/*
** Test task init with a GetPool failure
*/
void Test_SB_AppInit_GetPoolFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;
    int32           ForcedRtnVal = -1;

#ifdef UT_VERBOSE
    UT_Text("Start of GetPool Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&GetPoolRtn, ForcedRtnVal, 3);
    ExpRtn = ForcedRtnVal;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in GetPool test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_AppInit", "GetPool failure", "01.007");
} /* end Test_SB_AppInit_GetPoolFail */

/*
** Test task init with a PutPool failure
*/
void Test_SB_AppInit_PutPoolFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;
    int32           ForcedRtnVal = -1;

#ifdef UT_VERBOSE
    UT_Text("Start of PutPool Failure Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&PutPoolRtn, ForcedRtnVal, 1);
    ExpRtn = ForcedRtnVal;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CFE_SB_AppInit in PutPool Test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_AppInit", "PutPool Failure", "01.008");
} /* end Test_SB_AppInit_PutPoolFail */

/*
** Function for calling SB main task test functions
*/
void Test_SB_MainRoutine(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test for MainRoutine");
#endif

    Test_SB_Main_RcvErr();
    Test_SB_Main_InitErr();

#ifdef UT_VERBOSE
     UT_Text("End Test for MainRoutine");
#endif
} /* end Test_SB_MainRoutine */

/*
** Test main task with a packet receive error
*/
void Test_SB_Main_RcvErr(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Start of Main Loop, Rcv Error Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&QueueGetRtn, -1, 1);
    CFE_SB_TaskMain();
    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_INIT_EID) == FALSE)
    {
        UT_Text("CFE_SB_INIT_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_Q_RD_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_Q_RD_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_TaskMain", "Rcv Error failure", "02.001");
} /* end Test_SB_Main_RcvErr */

/*
** Test main task with an application initialization error
*/
void Test_SB_Main_InitErr(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Start of Main Loop, Init Error Test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&PutPoolRtn, -1, 1);
    CFE_SB_TaskMain();
    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_TaskMain", "Init Error failure", "02.002");
} /* end Test_SB_Main_InitErr */

/*
** Function for calling SB command test functions
*/
void Test_SB_Cmds(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test for SB Commands");
#endif

    Test_SB_Cmds_Noop();
    Test_SB_Cmds_RstCtrs();
    Test_SB_Cmds_Stats();
    Test_SB_Cmds_RoutingInfoDef();
    Test_SB_Cmds_RoutingInfoSpec();
    Test_SB_Cmds_RoutingInfoCreateFail();
    Test_SB_Cmds_RoutingInfoHdrFail();
    Test_SB_Cmds_RoutingInfoWriteFail();
    Test_SB_Cmds_PipeInfoDef();
    Test_SB_Cmds_PipeInfoSpec();
    Test_SB_Cmds_PipeInfoCreateFail();
    Test_SB_Cmds_PipeInfoHdrFail();
    Test_SB_Cmds_PipeInfoWriteFail();
    Test_SB_Cmds_MapInfoDef();
    Test_SB_Cmds_MapInfoSpec();
    Test_SB_Cmds_MapInfoCreateFail();
    Test_SB_Cmds_MapInfoHdrFail();
    Test_SB_Cmds_MapInfoWriteFail();
    Test_SB_Cmds_EnRouteValParam();
    Test_SB_Cmds_EnRouteNonExist();
    Test_SB_Cmds_EnRouteInvParam();
    Test_SB_Cmds_EnRouteInvParam2();
    Test_SB_Cmds_EnRouteInvParam3();
    Test_SB_Cmds_DisRouteValParam();
    Test_SB_Cmds_DisRouteNonExist();
    Test_SB_Cmds_DisRouteInvParam();
    Test_SB_Cmds_DisRouteInvParam2();
    Test_SB_Cmds_DisRouteInvParam3();
    Test_SB_Cmds_SendHK();
    Test_SB_Cmds_SendPrevSubs();
    Test_SB_Cmds_SubRptOn();
    Test_SB_Cmds_SubRptOff();
    Test_SB_Cmds_UnexpCmdCode();
    Test_SB_Cmds_UnexpMsgId();

#ifdef UT_VERBOSE
    UT_Text("End CFE_SB_ProcessCmdPipePkt");
#endif
} /* end Test_SB_Cmds */

/*
** Test no-op command
*/
void Test_SB_Cmds_Noop(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - No-op");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd, CFE_SB_NOOP_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CMD0_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_CMD0_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "No-op command test", "03.001");
} /* end Test_SB_Cmds_Noop */

/*
** Test reset counters command
*/
void Test_SB_Cmds_RstCtrs(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat= CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Reset Counters");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd, CFE_SB_RESET_CTRS_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CMD1_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_CMD1_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Reset Counters command test", "03.002");
} /* Test_SB_Cmds_RstCtrs */

/*
** Test send SB stats command
*/
void Test_SB_Cmds_Stats(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send SB Stats");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd, CFE_SB_SEND_SB_STATS_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_STATS_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_STATS_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_STATS_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_STATS_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send SB Stats command test", "03.003");
} /* end Test_SB_Cmds_Stats */

/*
** Test send routing information command using the default file name
*/
void Test_SB_Cmds_RoutingInfoDef(void)
{
    CFE_SB_PipeId_t           PipeId = 0;
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Routing Info, Using Default Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_ROUTING_INFO_CC);
    WriteFileCmd.Filename[0] = '\0';

    /* Make some routing info by calling CFE_SB_AppInit */
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_AppInit, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();

    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_INIT_EID) == FALSE)
    {
        UT_Text("CFE_SB_CMD1_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_INIT_EID) == FALSE)
    {
        UT_Text("CFE_SB_INIT_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Routing Info command test", "03.004");
} /* end Test_SB_Cmds_RoutingInfoDef */

/*
** Test send routing information command using a specified file name
*/
void Test_SB_Cmds_RoutingInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Routing Info2, Using Specified "
            "Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_ROUTING_INFO_CC);
    strncpy(WriteFileCmd.Filename, "RoutingTstFile", OS_MAX_PATH_LEN);
    WriteFileCmd.Filename[OS_MAX_PATH_LEN - 1] = '\0';
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Routing Info2 command test", "03.005");
} /* end Test_SB_Cmds_RoutingInfoSpec */

/*
**  Test send routing information command with a file creation failure
*/
void Test_SB_Cmds_RoutingInfoCreateFail(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Routing Info3, File Create Fails");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_ROUTING_INFO_CC);
    strncpy(WriteFileCmd.Filename, "RoutingTstFile", OS_MAX_PATH_LEN);
    WriteFileCmd.Filename[OS_MAX_PATH_LEN - 1] = '\0';
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    /* Make function CFE_SB_SendRtgInfo return CFE_SB_FILE_IO_ERR */
    UT_SetOSFail(OS_CREAT_FAIL);
    CFE_SB_ProcessCmdPipePkt();
    UT_SetOSFail(OS_NO_FAIL);
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_ERR1_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_ERR1_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Routing Info3 command test", "03.006");
} /* end Test_SB_Cmds_RoutingInfoCreateFail */

/*
** Test send routing information command with a file header write failure
*/
void Test_SB_Cmds_RoutingInfoHdrFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Routing Info4, FileHdrWrite Fails");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendRtgInfo("RoutingTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendRtgInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_SendRtgInfo",
              "Send Routing Info4 command test", "03.007");
} /* end Test_SB_Cmds_RoutingInfoHdrFail */

/*
** Test send routing information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_RoutingInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Routing Info5, Second FileWrite Fails");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SUCCESS;

    /* Make some routing info by calling CFE_SB_AppInit */
    ActRtn = CFE_SB_AppInit();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_AppInit, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_SetRtnCode(&FileWriteRtn, -1, 2);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendRtgInfo("RoutingTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendRtgInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_INIT_EID) == FALSE)
    {
        UT_Text("CFE_SB_INIT_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_SendRtgInfo",
              "Send Routing Info5 command test", "03.008");
} /* end Test_SB_Cmds_RoutingInfoWriteFail */

/*
** Test send pipe information command using the default file name
*/
void Test_SB_Cmds_PipeInfoDef(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_SB_PipeId_t           PipeId1;
    CFE_SB_PipeId_t           PipeId2;
    CFE_SB_PipeId_t           PipeId3;
    uint16                    PipeDepth = 10;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Pipe Info, Using Default Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_PIPE_INFO_CC);
    WriteFileCmd.Filename[0] = '\0';

    /* Create some pipe info */
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3");
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 4;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    CFE_SB_DeletePipe(PipeId3);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Pipe Info command test", "03.009");
} /* end Test_SB_Cmds_PipeInfoDef */

/*
** Test send pipe information command using a specified file name
*/
void Test_SB_Cmds_PipeInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Pipe Info2, Using Specified Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_PIPE_INFO_CC);
    strncpy(WriteFileCmd.Filename, "PipeTstFile", OS_MAX_PATH_LEN);
    WriteFileCmd.Filename[OS_MAX_PATH_LEN - 1] = '\0';
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Pipe Info2 command test", "03.010");
} /* end Test_SB_Cmds_PipeInfoSpec */

/*
** Test send pipe information command with a file creation failure
*/
void Test_SB_Cmds_PipeInfoCreateFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Pipe Info3, File Create Fails");
#endif

    SB_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendPipeInfo("PipeTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendPipeInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_SetOSFail(OS_NO_FAIL);
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_ERR1_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_ERR1_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_SendPipeInfo",
              "Send Pipe Info3 command test", "03.011");
} /* end Test_SB_Cmds_PipeInfoCreateFail */

/*
** Test send pipe information command with a file header write failure
*/
void Test_SB_Cmds_PipeInfoHdrFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Pipe Info4, FileHdrWrite Fails");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendPipeInfo("PipeTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendPipeInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_SendPipeInfo",
              "Send Pipe Info4 command test", "03.012");
} /* end Test_SB_Cmds_PipeInfoHdrFail */

/*
** Test send pipe information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_PipeInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_PipeId_t PipeId3;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Pipe Info5, Second FileWrite fails");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3");
    UT_SetRtnCode(&FileWriteRtn, -1, 2);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendPipeInfo("PipeTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendPipeInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 4;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    CFE_SB_DeletePipe(PipeId3);
    UT_Report(TestStat, "CFE_SB_SendPipeInfo",
              "Send Pipe Info5 command test", "03.013");
} /* end Test_SB_Cmds_PipeInfoWriteFail */

/*
** Test send map information command using the default file name
*/
void Test_SB_Cmds_MapInfoDef(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_SB_PipeId_t           PipeId1;
    CFE_SB_PipeId_t           PipeId2;
    CFE_SB_PipeId_t           PipeId3;
    CFE_SB_MsgId_t            MsgId0 = 0x0809;
    CFE_SB_MsgId_t            MsgId1 = 0x080a;
    CFE_SB_MsgId_t            MsgId2 = 0x080b;
    CFE_SB_MsgId_t            MsgId3 = 0x080c;
    CFE_SB_MsgId_t            MsgId4 = 0x080d;
    CFE_SB_MsgId_t            MsgId5 = 0x080e;
    uint16                    PipeDepth = 10;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Map Info, Using Default Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_MAP_INFO_CC);
    WriteFileCmd.Filename[0] = '\0';

    /* Create some map info */
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3");
    CFE_SB_Subscribe(MsgId0, PipeId1);
    CFE_SB_Subscribe(MsgId0, PipeId2);
    CFE_SB_Subscribe(MsgId1, PipeId1);
    CFE_SB_Subscribe(MsgId2, PipeId3);
    CFE_SB_Subscribe(MsgId3, PipeId3);
    CFE_SB_Subscribe(MsgId4, PipeId3);
    CFE_SB_Subscribe(MsgId5, PipeId2);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 11;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    CFE_SB_DeletePipe(PipeId3);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Map Info command test", "03.014");
} /* end Test_SB_Cmds_MapInfoDef */

/*
** Test send map information command using a specified file name
*/
void Test_SB_Cmds_MapInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    int32                     ExpRtn;
    int32                     ActRtn;
    int32                     TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Map Info2, Using Specified Filename");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&WriteFileCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_WriteFileInfoCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &WriteFileCmd,
                      CFE_SB_SEND_MAP_INFO_CC);
    strncpy(WriteFileCmd.Filename, "MapTstFile", OS_MAX_PATH_LEN);
    WriteFileCmd.Filename[OS_MAX_PATH_LEN - 1] = '\0';
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Map Info2 command test", "03.015");
} /* end Test_SB_Cmds_MapInfoSpec */

/*
** Test send map information command with a file creation failure
*/
void Test_SB_Cmds_MapInfoCreateFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Map Info3, File Create Fails");
#endif

    SB_ResetUnitTest();
    UT_SetOSFail(OS_CREAT_FAIL);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendMapInfo("MapTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendMapInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_SetOSFail(OS_NO_FAIL);
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SND_RTG_ERR1_EID) == FALSE)
    {
        UT_Text("CFE_SB_SND_RTG_ERR1_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_SendMapInfo",
              "Send Map Info3 command test", "03.016");
} /* end Test_SB_Cmds_MapInfoCreateFail */

/*
** Test send map information command with a file header write failure
*/
void Test_SB_Cmds_MapInfoHdrFail(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Map Info4, FileHdrWrite Fails");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&FSWriteHdrRtn, -1, 1);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendMapInfo("MapTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendMapInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_SendMapInfo",
              "Send Map Info4 command test", "03.017");
} /* end Test_SB_Cmds_MapInfoHdrFail */

/*
** Test send map information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_MapInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_PipeId_t PipeId3;
    CFE_SB_MsgId_t  MsgId0 = 0x0809;
    CFE_SB_MsgId_t  MsgId1 = 0x080a;
    CFE_SB_MsgId_t  MsgId2 = 0x080b;
    CFE_SB_MsgId_t  MsgId3 = 0x080c;
    CFE_SB_MsgId_t  MsgId4 = 0x080d;
    CFE_SB_MsgId_t  MsgId5 = 0x080e;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Map Info5, Second FileWrite Fails");
#endif

    SB_ResetUnitTest();

    /* Create some map info */
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3");
    CFE_SB_Subscribe(MsgId0, PipeId1);
    CFE_SB_Subscribe(MsgId0, PipeId2);
    CFE_SB_Subscribe(MsgId1, PipeId1);
    CFE_SB_Subscribe(MsgId2, PipeId3);
    CFE_SB_Subscribe(MsgId3, PipeId3);
    CFE_SB_Subscribe(MsgId4, PipeId3);
    CFE_SB_Subscribe(MsgId5, PipeId2);
    UT_SetRtnCode(&FileWriteRtn, -1, 2);
    ExpRtn = CFE_SB_FILE_IO_ERR;
    ActRtn = CFE_SB_SendMapInfo("MapTstFile");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_SendMapInfo, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 11;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FILEWRITE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_FILEWRITE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    CFE_SB_DeletePipe(PipeId3);
    UT_Report(TestStat, "CFE_SB_SendMapInfo",
              "Send Map Info5 command test", "03.018");
} /* end Test_SB_Cmds_MapInfoWriteFail */

/*
** Test command to enable a specific route using a valid route
*/
void Test_SB_Cmds_EnRouteValParam(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId;
    CFE_SB_MsgId_t     MsgId = 0x0829;
    uint16             PipeDepth = 5;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Enable Route, Valid Param");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "EnRouteTestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_ENABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = MsgId;
    EnDisRouteCmd.Pipe = PipeId;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_ENBL_RTE2_EID) == FALSE)
    {
        UT_Text("CFE_SB_ENBL_RTE2_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Enable Route, valid parameter test", "03.019");
} /* end Test_SB_Cmds_EnRouteValParam */

/*
** Test command to enable a specific route using a non-existent route
*/
void Test_SB_Cmds_EnRouteNonExist(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId1;
    CFE_SB_PipeId_t    PipeId2;
    CFE_SB_MsgId_t     MsgId = 0x0829;
    uint16             PipeDepth = 5;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Enable Route, Non Exist");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "EnRouteTestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "EnRouteTestPipe2");
    CFE_SB_Subscribe(MsgId, PipeId1);
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_ENABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = MsgId;
    EnDisRouteCmd.Pipe = PipeId2;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 4;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_ENBL_RTE1_EID) == FALSE)
    {
        UT_Text("CFE_SB_ENBL_RTE1_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Enable Route, non-existent route test", "03.020");
} /* end Test_SB_Cmds_EnRouteNonExist */

/*
** Test command to enable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_EnRouteInvParam(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Enable Route, Invalid Pipe ID");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_ENABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_HIGHEST_VALID_MSGID;
    EnDisRouteCmd.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_ENBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_ENBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Enable Route, invalid pipe ID test", "03.021");
} /* end Test_SB_Cmds_EnRouteInvParam */

/*
** Test command to enable a specific route using an invalid message ID
*/
void Test_SB_Cmds_EnRouteInvParam2(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Enable Route, Invalid Msg ID");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_ENABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_INVALID_MSG_ID;
    EnDisRouteCmd.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_ENBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_ENBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Enable Route, invalid msg ID test", "03.022");
} /* end Test_SB_Cmds_EnRouteInvParam2 */

/*
** Test command to enable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_EnRouteInvParam3(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Enable Route, Msg ID Too Large");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_ENABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_HIGHEST_VALID_MSGID + 1;
    EnDisRouteCmd.Pipe = 0;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_ENBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_ENBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Enable Route, msg ID too large test", "03.023");
} /* end Test_SB_Cmds_EnRouteInvParam3 */

/*
**  Test command to disable a specific route using a valid route
*/
void Test_SB_Cmds_DisRouteValParam(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId;
    CFE_SB_MsgId_t     MsgId = 0x0829;
    uint16             PipeDepth = 5;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Disable Route, Valid Param");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "DisRouteTestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_DISABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = MsgId;
    EnDisRouteCmd.Pipe = PipeId;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DSBL_RTE2_EID) == FALSE)
    {
        UT_Text("CFE_SB_DSBL_RTE2_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Disable Route, valid parameter test", "03.024");
} /* end Test_SB_Cmds_DisRouteValParam */

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteNonExist(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId1, PipeId2;
    CFE_SB_MsgId_t     MsgId = 0x0829;
    uint16             PipeDepth = 5;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Disable Route, Non Exist");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "DisRouteTestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "DisRouteTestPipe2");
    CFE_SB_Subscribe(MsgId, PipeId1);
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_DISABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = MsgId;
    EnDisRouteCmd.Pipe = PipeId2;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 4;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DSBL_RTE1_EID) == FALSE)
    {
        UT_Text("CFE_SB_DSBL_RTE1_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Disable Route, non-existent route test", "03.025");
} /* end Test_SB_Cmds_DisRouteNonExist */

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteInvParam(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Disable Route, Invalid Pipe ID");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_DISABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_HIGHEST_VALID_MSGID;
    EnDisRouteCmd.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DSBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_DSBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Disable Route, invalid pipe ID test", "03.026");
} /* end Test_SB_Cmds_DisRouteInvParam */

/*
** Test command to disable a specific route using an invalid message ID
*/
void Test_SB_Cmds_DisRouteInvParam2(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Disable Route, Invalid Param2");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_DISABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_INVALID_MSG_ID;
    EnDisRouteCmd.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DSBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_DSBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Disable Route, invalid msg ID test", "03.027");
} /* end Test_SB_Cmds_DisRouteInvParam2 */

/*
** Test command to disable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_DisRouteInvParam3(void)
{
    CFE_SB_EnRoutCmd_t EnDisRouteCmd;
    int32              ExpRtn;
    int32              ActRtn;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Disable Route, Msg ID Too Large");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&EnDisRouteCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_EnRoutCmd_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &EnDisRouteCmd,
                      CFE_SB_DISABLE_ROUTE_CC);
    EnDisRouteCmd.MsgId = CFE_SB_HIGHEST_VALID_MSGID + 1;
    EnDisRouteCmd.Pipe = 0;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DSBL_RTE3_EID) == FALSE)
    {
        UT_Text("CFE_SB_DSBL_RTE3_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Disable Route, msg ID too large test", "03.028");
} /* end Test_SB_Cmds_DisRouteInvParam3 */

/*
** Test send housekeeping information command
*/
void Test_SB_Cmds_SendHK(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Housekeeping Info");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_SEND_HK_MID,
                   sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SEND_NO_SUBS_EID) == FALSE)
    {
        UT_Text("CFE_SB_SEND_NO_SUBS_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send Housekeeping Info command test", "03.029");
} /* end Test_SB_Cmds_SendHK */

/*
** Test command to build and send a SB packet containing a complete
** list of current subscriptions
*/
void Test_SB_Cmds_SendPrevSubs(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_MsgId_t  MsgId = 0x0003;
    uint16          MsgLim = 4;
    uint16          PipeDepth = 50;
    int32           i;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Send Previous Subscriptions");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd, CFE_SB_SEND_PREV_SUBS_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");

    /* 45 is chosen since it allows two full pkts to be sent plus seven entries
     * in a partial pkt
     */
    for (i = 0; i < 45; i++)
    {
        ActRtn = CFE_SB_Subscribe(i, PipeId1);
        ExpRtn = CFE_SUCCESS;

        if (ActRtn != ExpRtn)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "Unexpected return subscribing in SendPrevSubs Test, "
                       "i=%ld, exp=0x%lx, act=0x%lx",
                     i, (uint32) ExpRtn, (uint32) ActRtn);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
        }
    }

    CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim);
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 54;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SEND_NO_SUBS_EID) == FALSE)
    {
        UT_Text("CFE_SB_SEND_NO_SUBS_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_FULL_SUB_PKT_EID) == FALSE)
    {
        UT_Text("CFE_SB_FULL_SUB_PKT_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PART_SUB_PKT_EID) == FALSE)
    {
        UT_Text("CFE_SB_PART_SUB_PKT_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Send previous subscriptions test", "03.030");
} /* end Test_SB_Cmds_SendPrevSubs */

/*
** Test command to enable subscription reporting
*/
void Test_SB_Cmds_SubRptOn(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Subscription Reporting On");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd,
                      CFE_SB_ENABLE_SUB_REPORTING_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Subscription Reporting On test", "03.031");
} /* end Test_SB_Cmds_SubRptOn */

/*
** Test command to disable subscription reporting
*/
void Test_SB_Cmds_SubRptOff(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Subscription Reporting Off");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID,
                   sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd,
                      CFE_SB_DISABLE_SUB_REPORTING_CC);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Subscription Reporting Off test", "03.032");
} /* end Test_SB_Cmds_SubRptOff */

/*
** Test command handler response to an invalid command code
*/
void Test_SB_Cmds_UnexpCmdCode(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Unexpected Command Code");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, CFE_SB_CMD_MID, sizeof(CFE_SB_CmdHdr_t), TRUE);

    /* Use a command code known to be invalid */
    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t) &NoParamCmd, 99);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_BAD_CMD_CODE_EID) == FALSE)
    {
        UT_Text("CFE_SB_BAD_CMD_CODE_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Unexpected command code test", "03.033");
} /* end Test_SB_Cmds_UnexpCmdCode */

/*
** Test command handler response to an invalid message ID
*/
void Test_SB_Cmds_UnexpMsgId(void)
{
    CFE_SB_MsgId_t  MsgId = 0x0865;
    CFE_SB_CmdHdr_t NoParamCmd;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Cmd - Unexpected MsgId on SB Command Pipe");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&NoParamCmd, MsgId, sizeof(CFE_SB_CmdHdr_t), TRUE);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_BAD_MSGID_EID) == FALSE)
    {
        UT_Text("CFE_SB_BAD_MSGID_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_ProcessCmdPipePkt",
              "Unexpected MsgId on SB command pipe test", "03.034");
} /* end Test_SB_Cmds_UnexpMsgId */

/*
** Function for calling SB early initialization, API, and utility
** test functions
*/
void Test_SB_Lib(void)
{
    Test_SB_EarlyInit();
    Test_SB_APIs();
    Test_SB_Utils();
} /* end Test_SB_Lib */

/*
** Function for calling early initialization tests
*/
void Test_SB_EarlyInit(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test EarlyInit");
#endif

    Test_SB_EarlyInit_SemCreateError();
    Test_SB_EarlyInit_PoolCreateError();
    Test_SB_EarlyInit_NoErrors();

#ifdef UT_VERBOSE
    UT_Text("End Test EarlyInit\n");
#endif
} /* end Test_SB_EarlyInit */

/*
** Test early initialization response to a semaphore create failure
*/
void Test_SB_EarlyInit_SemCreateError(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Sem Create Error Logic test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&MutSemCreateRtn, OS_ERR_NO_FREE_IDS, 1);
    UT_Report(CFE_SB_EarlyInit() == OS_ERR_NO_FREE_IDS,
              "CFE_SB_EarlyInit", "Sem Create error logic", "04.001");
} /* end Test_SB_EarlyInit_SemCreateError */

/*
** Test early initialization response to a pool create ex failure
*/
void Test_SB_EarlyInit_PoolCreateError(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin PoolCreateEx Error Logic test");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&PoolCreateExRtn, CFE_ES_BAD_ARGUMENT, 1);
    UT_Report(CFE_SB_EarlyInit() == CFE_ES_BAD_ARGUMENT,
              "CFE_SB_EarlyInit", "PoolCreateEx error logic", "04.002");
} /* end Test_SB_EarlyInit_PoolCreateError */

/*
** Test successful early initialization
*/
void Test_SB_EarlyInit_NoErrors(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin EarlyInit No Errors test");
#endif

    SB_ResetUnitTest();
    CFE_SB_EarlyInit();
    UT_Report(CFE_SB_EarlyInit() == CFE_SUCCESS,
              "CFE_SB_EarlyInit", "No errors test", "04.003");
} /* end Test_SB_EarlyInit_NoErrors */

/*
** Function for calling SB API test functions
*/
void Test_SB_APIs(void)
{
    Test_CreatePipe_API();
    Test_DeletePipe_API();
    Test_Subscribe_API();
    Test_Unsubscribe_API();
    Test_SendMsg_API();
    Test_RcvMsg_API();
    Test_CleanupApp_API();
} /* end Test_SB_APIs */

/*
** Function for calling SB create pipe API test functions
*/
void Test_CreatePipe_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_CreatePipe_API");
#endif

    Test_CreatePipe_NullPtr();
    Test_CreatePipe_ValPipeDepth();
    Test_CreatePipe_InvalPipeDepth();
    Test_CreatePipe_EmptyPipeName();
    Test_CreatePipe_LongPipeName();
    Test_CreatePipe_SamePipeName();
    Test_CreatePipe_MaxPipes();

#ifdef UT_VERBOSE
    UT_Text("End Test_CreatePipe_API\n");
#endif
} /* end Test_CreatePipe_API */

/*
** Test create pipe response to a null pipe ID pointer
*/
void Test_CreatePipe_NullPtr(void)
{
    uint16 PipeDepth = 10;
    int32  ExpRtn;
    int32  ActRtn;
    int32  TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Null Pointer");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1); /* Avoids creating socket */
    ActRtn = CFE_SB_CreatePipe(NULL, PipeDepth, "TestPipe");

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in null pointer test, exp=%ld, act=%ld",
                 CFE_SB_BAD_ARGUMENT, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_BAD_ARG_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_BAD_ARG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CreatePipe_API", "Null pointer test", "05.001");
} /* end Test_CreatePipe_NullPtr */

/*
** Test create pipe response to valid pipe depths
*/
void Test_CreatePipe_ValPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];
    int32           Rtn[3];
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Valid Pipe Depth");
#endif

    /* Test a few valid pipe depth values */
    SB_ResetUnitTest();
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1); /* Avoids creating socket */
    Rtn[0] = CFE_SB_CreatePipe(&PipeIdReturned[0], 99, "TestPipe99");
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1);
    Rtn[1] = CFE_SB_CreatePipe(&PipeIdReturned[1], 255, "TestPipe255");
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1);
    Rtn[2] = CFE_SB_CreatePipe(&PipeIdReturned[2],
             CFE_SB_MAX_PIPE_DEPTH, "TestPipeMaxDepth");

    if (Rtn[0] != CFE_SUCCESS ||
        Rtn[1] != CFE_SUCCESS ||
        Rtn[2] != CFE_SUCCESS)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in valid pipe depth test\n "
                   "Val=99, Rtn1=0x%lx, act=0x%lx\n "
                   "Val=255, Rtn2=0x%lx, act=0x%lx\n "
                   "Val=%d, Rtn3=0x%lx, Expected Rtn3=0x%lx",
                 (uint32) Rtn[0], CFE_SUCCESS,
                 (uint32) Rtn[1], CFE_SUCCESS,
                 CFE_SB_MAX_PIPE_DEPTH,
                 (uint32) Rtn[2], CFE_SUCCESS);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CreatePipe_API",
              "Valid pipe depth test", "05.002");
} /* end Test_CreatePipe_ValPipeDepth */

/*
** Test create pipe response to invalid pipe depths
*/
void Test_CreatePipe_InvalPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];
    int32           Rtn[3];
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe Depth");
#endif

    /* Test a few invalid pipe depth values */
    SB_ResetUnitTest();
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1); /* Avoid creating socket */
    Rtn[0] = CFE_SB_CreatePipe(&PipeIdReturned[0], 0, "TestPipe1");
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1);
    Rtn[1] = CFE_SB_CreatePipe(&PipeIdReturned[1],
             CFE_SB_MAX_PIPE_DEPTH + 1, "TestPipeMaxDepPlus1");
    UT_SetRtnCode(&QueueCreateRtn, OS_SUCCESS, 1);
    Rtn[2] = CFE_SB_CreatePipe(&PipeIdReturned[2], 0xffff, "TestPipeffff");

    if (Rtn[0] != CFE_SB_BAD_ARGUMENT ||
        Rtn[1] != CFE_SB_BAD_ARGUMENT ||
        Rtn[2] != CFE_SB_BAD_ARGUMENT)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in pipe depth test for invalid settings\n "
                   "Val=0, Rtn1=0x%lx, act=0x%lx\n "
                   "Val=%d, Rtn2=0x%lx, act=0x%lx\n "
                   "Val=65535, Rtn3=0x%lx, Expected Rtn3=0x%lx",
                 (uint32) Rtn[0], CFE_SB_BAD_ARGUMENT,
                 CFE_SB_MAX_PIPE_DEPTH + 1,
                 (uint32) Rtn[1], CFE_SB_BAD_ARGUMENT,
                 (uint32) Rtn[2], CFE_SB_BAD_ARGUMENT);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_BAD_ARG_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_BAD_ARG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CreatePipe_API",
              "Invalid pipe depth test", "05.003");
} /* end Test_CreatePipe_InvalPipeDepth */

/*
** Test create pipe response to an empty pipe name
*/
void Test_CreatePipe_EmptyPipeName(void)
{
    CFE_SB_PipeId_t PipeIdReturned;
    uint16          PipeDepth = 50;
    int32           Rtn;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Empty Pipe Name");
#endif

    SB_ResetUnitTest();

    /* Empty string test - OS_QueueCreate currently returns OS_ERR_NAME_TAKEN
     * error because all queues are initialized to an empty string; see
     * DCR 501 */
    Rtn = CFE_SB_CreatePipe(&PipeIdReturned, PipeDepth, "");

    if (Rtn != CFE_SB_PIPE_CR_ERR)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in empty pipe name test, "
                   "exp=0x%lx, act=0x%lx",
                 CFE_SUCCESS, (uint32) Rtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CreatePipe_API", "Empty pipe name test", "05.004");
} /* end Test_CreatePipe_EmptyPipeName */

/*
** Test create pipe response to a pipe name longer than allowed
*/
void Test_CreatePipe_LongPipeName(void)
{
    CFE_SB_PipeId_t PipeIdReturned;
    char            PipeName[OS_MAX_API_NAME + 2];
    uint16          PipeDepth = 50;
    int16           i;
    int32           Rtn;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Long Pipe Name");
#endif

    /* Oversized string test */
    SB_ResetUnitTest();

    for (i = 0; i < OS_MAX_API_NAME + 1; i++)
    {
        PipeName[i] = 'A';
    }

    PipeName[i] = '\0';
    Rtn = CFE_SB_CreatePipe(&PipeIdReturned, PipeDepth, PipeName);

    if (Rtn != CFE_SB_PIPE_CR_ERR)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in pipe name, oversized string test, "
                   "exp=0x%lx, act=0x%lx",
                 CFE_SB_PIPE_CR_ERR, (uint32) Rtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CreatePipe_API", "Long PipeName Test", "05.005");
} /* end Test_CreatePipe_LongPipeName */

/*
** Test create pipe response to duplicate pipe names
*/
void Test_CreatePipe_SamePipeName(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];
    uint16          PipeDepth = 50;
    int32           Rtn[3];
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Same Pipe Name");
#endif

    SB_ResetUnitTest();

    /* Same pipe name test */
    Rtn[1] = CFE_SB_CreatePipe(&PipeIdReturned[1], PipeDepth, "SamePipeName");
    Rtn[2] = CFE_SB_CreatePipe(&PipeIdReturned[2], PipeDepth, "SamePipeName");

    if (Rtn[1] != CFE_SUCCESS || Rtn[2] != CFE_SB_PIPE_CR_ERR)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in same pipe name test, Rtn1=0x%lx, "
                   "ExpRtn1=0x%lx, Rtn2=0x%lx, ExpRtn2=0x%lx",
                 (uint32) Rtn[1], CFE_SUCCESS,
                 (uint32) Rtn[2], CFE_SB_PIPE_CR_ERR);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_CR_PIPE_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_CR_PIPE_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Cleanup the pipe that was created */
    CFE_SB_DeletePipe(PipeIdReturned[1]);

    UT_Report(TestStat, "Test_CreatePipe_API", "Same pipe name test", "05.006");
} /* end Test_CreatePipe_SamePipeName */

/*
** Test create pipe response to too many pipes
*/
void Test_CreatePipe_MaxPipes(void)
{
    CFE_SB_PipeId_t PipeIdReturned[CFE_SB_MAX_PIPES + 1];
    uint16          PipeDepth = 50;
    int32           Rtn[CFE_SB_MAX_PIPES + 1];
    int32           i;
    int32           ExpRtn;
    int32           TestStat = CFE_PASS;
    char            PipeName[OS_MAX_API_NAME];

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Max Pipes");
#endif

    SB_ResetUnitTest();

    /* Create maximum number of pipes + 1. Only one 'create pipe' failure
     * expected
     */
    for (i = 0; i < CFE_SB_MAX_PIPES + 1; i++)
    {
        SNPRINTF(PipeName, OS_MAX_API_NAME, "TestPipe%ld", i);
        Rtn[i] = CFE_SB_CreatePipe(&PipeIdReturned[i],
                                   PipeDepth, &PipeName[0]);

        if (i < CFE_SB_MAX_PIPES)
        {
            ExpRtn = CFE_SUCCESS;
        }
        else
        {
            ExpRtn = CFE_SB_MAX_PIPES_MET;
        }

        if (Rtn[i] != ExpRtn)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "Unexpected return in max pipes test, "
                       "i=%ld, exp=0x%lx, act=0x%lx",
                     i, (uint32) ExpRtn, (uint32) Rtn[i]);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    if (UT_EventIsInHistory(CFE_SB_MAX_PIPES_MET_EID) == FALSE)
    {
        UT_Text("CFE_SB_MAX_PIPES_MET_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Clean up */
    for (i = 0; i <= CFE_SB_MAX_PIPES - 1; i++)
    {
        CFE_SB_DeletePipe(i);
    }

    UT_Report(TestStat, "Test_CreatePipe_API", "Max pipes test", "05.007");
} /* end Test_CreatePipe_MaxPipes */

/*
** Function for calling SB delete pipe API test functions
*/
void Test_DeletePipe_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_DeletePipe_API");
#endif

    Test_DeletePipe_NoSubs();
    Test_DeletePipe_WithSubs();
    Test_DeletePipe_InvalidPipeId();
    Test_DeletePipe_InvalidPipeOwner();
    Test_DeletePipe_WithAppid();

#ifdef UT_VERBOSE
    UT_Text("End Test_DeletePipe_API\n");
#endif
} /* end Test_DeletePipe_API */

/*
** Test delete pipe response to deleting a pipe with no subscriptions
*/
void Test_DeletePipe_NoSubs(void)
{
    CFE_SB_PipeId_t PipedId;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for No Subscriptions");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe");
    ActRtn = CFE_SB_DeletePipe(PipedId);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in delete pipe test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_DELETED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_DELETED_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_DeletePipe_API",
              "No subscriptions test", "06.001");
} /* end Test_DeletePipe_NoSubs */

/*
** Test delete pipe response to deleting a pipe with subscriptions
*/
void Test_DeletePipe_WithSubs(void)
{
    CFE_SB_PipeId_t PipedId;
    CFE_SB_MsgId_t  MsgId0 = 0x1801;
    CFE_SB_MsgId_t  MsgId1 = 0x1802;
    CFE_SB_MsgId_t  MsgId2 = 0x1803;
    CFE_SB_MsgId_t  MsgId3 = 0x1804;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test With Subscriptions");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId0, PipedId);
    CFE_SB_Subscribe(MsgId1, PipedId);
    CFE_SB_Subscribe(MsgId2, PipedId);
    CFE_SB_Subscribe(MsgId3, PipedId);
    ActRtn = CFE_SB_DeletePipe(PipedId);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in delete pipe test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 10;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_DELETED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_DELETED_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_DeletePipe_API",
              "With subscriptions test", "06.002");
} /* end Test_DeletePipe_WithSubs */

/*
** Test delete pipe response to an invalid pipe ID
*/
void Test_DeletePipe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = 30;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe ID");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_DeletePipe(PipeId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in delete pipe test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DEL_PIPE_ERR1_EID) == FALSE)
    {
        UT_Text("CFE_SB_DEL_PIPE_ERR1_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_DeletePipe_API",
              "Invalid pipe ID test", "06.003");
} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test delete pipe response to an invalid pipe owner
*/
void Test_DeletePipe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipedId;
    uint32          RealOwner;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe Owner");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CreatePipe in DeletePipe, "
                   "invalid owner test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipedId].AppId;

    /* Choose a value that is sure not to be owner */
    CFE_SB.PipeTbl[PipedId].AppId = RealOwner + 1;
    ActRtn = CFE_SB_DeletePipe(PipedId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from DeletePipe in DeletePipe, "
                   "invalid owner test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from UT_GetNumEventsSent, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_DEL_PIPE_ERR2_EID) == FALSE)
    {
        UT_Text("CFE_SB_DEL_PIPE_ERR2_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Restore owner id and delete pipe since test is complete */
    CFE_SB.PipeTbl[PipedId].AppId = RealOwner;
    CFE_SB_DeletePipe(PipedId);
    UT_Report(TestStat, "Test_DeletePipe_API",
              "Invalid pipe owner test", "06.004");
} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test successful pipe delete with app ID
*/
void Test_DeletePipe_WithAppid(void)
{
    CFE_SB_PipeId_t PipedId;
    CFE_SB_MsgId_t  MsgId0 = 0x1801;
    CFE_SB_MsgId_t  MsgId1 = 0x1802;
    CFE_SB_MsgId_t  MsgId2 = 0x1803;
    CFE_SB_MsgId_t  MsgId3 = 0x1804;
    uint32          AppId = 0;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test DeletePipeWithAppId API");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId0, PipedId);
    CFE_SB_Subscribe(MsgId1, PipedId);
    CFE_SB_Subscribe(MsgId2, PipedId);
    CFE_SB_Subscribe(MsgId3, PipedId);
    ActRtn = CFE_SB_DeletePipeWithAppId(PipedId, AppId);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in delete pipe w/app ID test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 10;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_DeletePipe_API", "With app ID test", "06.005");
} /* end Test_DeletePipe_WithAppid */

/*
** Function for calling SB subscribe API test functions
*/
void Test_Subscribe_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_Subscribe_API\n");
#endif

    Test_Subscribe_SubscribeEx();
    Test_Subscribe_InvalidPipeId();
    Test_Subscribe_InvalidMsgId();
    Test_Subscribe_MaxMsgLim();
    Test_Subscribe_DuplicateSubscription();
    Test_Subscribe_LocalSubscription();
    Test_Subscribe_MaxDestCount();
    Test_Subscribe_MaxMsgIdCount();
    Test_Subscribe_SendPrevSubs();
    Test_Subscribe_FindGlobalMsgIdCnt();
    Test_Subscribe_PipeNonexistent();
    Test_Subscribe_SubscriptionReporting();
    Test_Subscribe_InvalidPipeOwner();

#ifdef UT_VERBOSE
    UT_Text("End Test_Subscribe_API\n");
#endif
} /* end Test_Subscribe_API */

/*
** Test API to globally subscribe to a message
*/
void Test_Subscribe_SubscribeEx(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x1806;
    CFE_SB_Qos_t    Quality = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim = 8;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for SubscribeEx");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in SubscribeEx Test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API", "SubscribeEx test", "07.001");
} /* end Test_Subscribe_SubscribeEx */

/*
** Test message subscription response to an invalid pipe ID
*/
void Test_Subscribe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = 2;
    CFE_SB_MsgId_t  MsgId = CFE_SB_HIGHEST_VALID_MSGID + 1;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe ID");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid pipe ID subscribe test, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUB_INV_PIPE_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUB_INV_PIPE_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_Subscribe_API", "Invalid pipe ID test", "07.002");
} /* end Test_Subscribe_InvalidPipeId */

/*
** Test message subscription response to an invalid message ID
*/
void Test_Subscribe_InvalidMsgId(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = CFE_SB_HIGHEST_VALID_MSGID + 1;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Message Id");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid message ID subscribe test, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUB_ARG_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUB_ARG_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Invalid message ID test", "07.003");
} /* end Test_Subscribe_InvalidMsgId */

/*
** Test message subscription response to the maximum message limit
*/
void Test_Subscribe_MaxMsgLim(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x1805;
    CFE_SB_Qos_t    Quality = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Maximum Message Limit");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    MsgLim = 0xffff;
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in maximum message limit subscribe test, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Maximum message limit test", "07.004");
} /* end Test_Subscribe_MaxMsgLim */

/*
** Test message subscription response to a duplicate subscription
*/
void Test_Subscribe_DuplicateSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x1805;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Duplicate Subscription");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in first subscribe of duplicate "
                   "subscription test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in second subscribe of duplicate "
                   "subscription test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Duplicate subscription test", "07.005");
} /* end Test_Subscribe_DuplicateSubscription */

/*
** Test API to locally subscribe to a message
*/
void Test_Subscribe_LocalSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x0809;
    uint16          PipeDepth = 10;
    uint16          MsgLim = 4;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Local Subscription");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_SubscribeLocal(MsgId, PipeId, MsgLim);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in local subscription test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Local subscription test", "07.006");
} /* end Test_Subscribe_LocalSubscription */

/*
** Test message subscription response to reaching the maximum destination count
*/
void Test_Subscribe_MaxDestCount(void)
{
    CFE_SB_PipeId_t PipeId[CFE_SB_MAX_DEST_PER_PKT + 1];
    CFE_SB_MsgId_t  MsgId = 0x0811;
    char            PipeName[OS_MAX_API_NAME];
    uint16          PipeDepth = 50;
    int32           i;
    int32           Rtn[CFE_SB_MAX_DEST_PER_PKT + 1];
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Maximum Destination Count");
#endif

    SB_ResetUnitTest();

    /* Create pipes */
    for (i = 0; i < CFE_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        SNPRINTF(PipeName, OS_MAX_API_NAME, "TestPipe%ld", i);
        Rtn[i] = CFE_SB_CreatePipe(&PipeId[i], PipeDepth, &PipeName[0]);
        ExpRtn = CFE_SUCCESS;

        if (Rtn[i] != ExpRtn)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "Unexpected error creating pipes in maximum destination "
                       "count test, i=%ld, exp=0x%lx, act=0x%lx",
                     i, (uint32) ExpRtn, (uint32) Rtn[i]);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    /* Do subscriptions */
    for (i = 0; i < CFE_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        ActRtn = CFE_SB_Subscribe(MsgId, i);

        if (i < CFE_SB_MAX_DEST_PER_PKT)
        {
            ExpRtn = CFE_SUCCESS;
        }
        else
        {
            ExpRtn = CFE_SB_MAX_DESTS_MET;
        }

        if (ActRtn != ExpRtn)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "Unexpected return in maximum destination count test, "
                       "i=%ld, exp=0x%lx, act=0x%lx",
                     i, (uint32) ExpRtn, (uint32) ActRtn);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    ExpRtn = 2 * (CFE_SB_MAX_DEST_PER_PKT + 1);
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Delete pipes */
    for (i = 0; i < CFE_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        CFE_SB_DeletePipe(PipeId[i]);
    }

    UT_Report(TestStat, "Test_Subscribe_API",
              "Maximum destination count test", "07.007");
} /* end Test_Subscribe_MaxDestCount */

/*
** Test message subscription response to reaching the maximum message ID count
*/
void Test_Subscribe_MaxMsgIdCount(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    uint16          PipeDepth = 50;
    int32           i;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Maximum Message ID Count");
#endif

    CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0");
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");

    for (i = 0; i < CFE_SB_MAX_MSG_IDS + 1; i++)
    {
        ActRtn = CFE_SB_Subscribe(i, PipeId2);

        if (i < CFE_SB_MAX_MSG_IDS)
        {
            ExpRtn = CFE_SUCCESS;
        }
        else
        {
            ExpRtn = CFE_SB_MAX_MSGS_MET;
        }

        if (ActRtn != ExpRtn)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "Unexpected return in maximum message ID count test, "
                       "i=%ld, exp=0x%lx, act=0x%lx",
                     i, (uint32) ExpRtn, (uint32) ActRtn);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
        }
    }

    if (UT_EventIsInHistory(CFE_SB_MAX_MSGS_MET_EID) == FALSE)
    {
        UT_Text("CFE_SB_MAX_MSGS_MET_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId0);
    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Maximum message ID count test", "07.008");
} /* end Test_Subscribe_MaxMsgIdCount */

/*
** Test obtaining the list of current message subscriptions
*/
void Test_Subscribe_SendPrevSubs(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_MsgId_t  MsgId0 = 0x0811;
    CFE_SB_MsgId_t  MsgId1 = 0x0812;
    CFE_SB_MsgId_t  MsgId2 = 0x0813;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Send Previous Subscriptions");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0");
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_Subscribe(MsgId0, PipeId0);
    CFE_SB_Subscribe(MsgId1, PipeId0);
    CFE_SB_Subscribe(MsgId2, PipeId0);
    CFE_SB_Subscribe(MsgId0, PipeId1);
    CFE_SB_Subscribe(MsgId1, PipeId1);
    CFE_SB_Subscribe(MsgId2, PipeId1);
    CFE_SB_Subscribe(MsgId0, PipeId2);
    CFE_SB_SendPrevSubs();
    ExpRtn = 12;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PART_SUB_PKT_EID) == FALSE)
    {
        UT_Text("CFE_SB_PART_SUB_PKT_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId0);
    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Send previous subscriptions test", "07.009");
} /* end Test_Subscribe_SendPrevSubs */

/*
** Test function to get a count of the global message ids in use
*/
void Test_Subscribe_FindGlobalMsgIdCnt(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_MsgId_t  MsgId0 = 0x0811;
    CFE_SB_MsgId_t  MsgId1 = 0x0812;
    CFE_SB_MsgId_t  MsgId2 = 0x0813;
    uint16          PipeDepth = 50;
    uint16          MsgLim = 4;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Find Global Message ID Count");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0");
    CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2");
    CFE_SB_Subscribe(MsgId0, PipeId0);
    CFE_SB_Subscribe(MsgId1, PipeId0);
    CFE_SB_Subscribe(MsgId2, PipeId0);
    CFE_SB_Subscribe(MsgId0, PipeId1);
    CFE_SB_Subscribe(MsgId1, PipeId1);
    CFE_SB_Subscribe(MsgId2, PipeId1);
    CFE_SB_SubscribeLocal(MsgId0, PipeId2, MsgLim);
    ActRtn = CFE_SB_FindGlobalMsgIdCnt();
    ExpRtn = 3; /* 3 unique msg ids */

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in find global message ID count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 10;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId0);
    CFE_SB_DeletePipe(PipeId1);
    CFE_SB_DeletePipe(PipeId2);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Find global message ID count test", "07.010");
} /* end Test_Subscribe_FindGlobalMsgIdCnt */

/*
** Test message subscription response to nonexistent pipe
*/
void Test_Subscribe_PipeNonexistent(void)
{
    CFE_SB_MsgId_t  MsgId = 0x0800;
    CFE_SB_PipeId_t PipeId = 55;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Pipe Nonexistent");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in pipe nonexistent test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUB_INV_PIPE_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUB_INV_PIPE_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_Subscribe_API",
              "Pipe nonexistent test", "07.011");
} /* end Test_Subscribe_PipeNonexistent */

/*
** Test enabling and disabling subscription reporting
*/
void Test_Subscribe_SubscriptionReporting(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x08fd;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test SubscribeFull, Subscription Reporting");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from CreatePipe in subscription reporting "
                   "test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);
    ActRtn = CFE_SB_Subscribe(MsgId, PipeId);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from Subscribe in Subscription reporting "
                   "test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 4;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RPT_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RPT_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);
    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Subscription reporting test", "07.012");
} /* end Test_Subscribe_SubscriptionReporting */

/*
** Test message subscription response to an invalid pipe owner
*/
void Test_Subscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x0877;
    uint16          PipeDepth = 10;
    int32           RealOwner;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe Owner");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "First return was unexpected in subscribe - non owner test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipeId].AppId;

    /* Choose a value that is sure not to be owner */
    CFE_SB.PipeTbl[PipeId].AppId = RealOwner + 1;
    CFE_SB_Subscribe(MsgId, PipeId);
    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUB_INV_CALLER_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUB_INV_CALLER_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Restore owner id and delete pipe since test is complete */
    CFE_SB.PipeTbl[PipeId].AppId = RealOwner;
    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Subscribe_API",
              "Invalid pipe owner test", "07.013");
} /* end Test_Subscribe_InvalidPipeOwner */

/*
** Function for calling SB unsubscribe API test functions
*/
void Test_Unsubscribe_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_Unsubscribe_API");
#endif

    Test_Unsubscribe_Basic();
    Test_Unsubscribe_Local();
    Test_Unsubscribe_InvalParam();
    Test_Unsubscribe_NoMatch();
    Test_Unsubscribe_SubscriptionReporting();
    Test_Unsubscribe_InvalidPipe();
    Test_Unsubscribe_InvalidPipeOwner();
    Test_Unsubscribe_FirstDestWithMany();
    Test_Unsubscribe_MiddleDestWithMany();
    Test_Unsubscribe_GetDestPtr();

#ifdef UT_VERBOSE
    UT_Text("End Test_Unsubscribe_API\n");
#endif
} /* end Test_Unsubscribe_API */

/*
** Test API used to unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Basic(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = CFE_SB_HIGHEST_VALID_MSGID / 2 + 1;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Unsubscribe Basic");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, TestPipe);
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Unsubscribe(MsgId, TestPipe);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in unsubscribe basic, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API", "Unsubscribe basic", "08.001");
} /* end Test_Unsubscribe_Basic */

/*
** Test CFE internal API used to locally unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Local(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = 0x0801;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Local Unsubscribe");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, TestPipe);
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_UnsubscribeLocal(CFE_SB_HIGHEST_VALID_MSGID, TestPipe);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in local unsubscribe test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Local unsubscribe test", "08.002");
} /* end Test_Unsubscribe_Local */

/*
** Test message unsubscription response to an invalid message ID
*/
void Test_Unsubscribe_InvalParam(void)
{
    CFE_SB_PipeId_t TestPipe;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Param, Unsubscribe");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_Unsubscribe(CFE_SB_HIGHEST_VALID_MSGID + 1, TestPipe);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid param unsubscribe test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_UNSUB_ARG_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_UNSUB_ARG_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Invalid param, unsubscribe test", "08.003");
} /* end Test_Unsubscribe_InvalParam */

/*
** Test message unsubscription response to a message ID that is not subscribed
*/
void Test_Unsubscribe_NoMatch(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = 0x0001;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for No Match");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, TestPipe);
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Unsubscribe(MsgId + 1, TestPipe);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in no match test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_UNSUB_NO_SUBS_EID) == FALSE)
    {
        UT_Text("CFE_SB_UNSUB_NO_SUBS_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API", "No match test", "08.004");
} /* end Test_Unsubscribe_NoMatch */

/*
** Test message unsubscription response to enabling/disabling subscription
** reporting
*/
void Test_Unsubscribe_SubscriptionReporting(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = 0x08fd;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unsubscribe Subscription Reporting");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, TestPipe);
    CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);
    CFE_SB_Unsubscribe(MsgId, TestPipe);
    ExpRtn = CFE_SB_UNSUBSCRIPTION;
    ActRtn = CFE_SB.SubRprtMsg.SubType;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unsubscribe not enabled as expected in CFE_SB_Unsubscribe, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);
    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_UNSUBSCRIPTION_RPT_EID) == FALSE)
    {
        UT_Text("CFE_SB_UNSUBSCRIPTION_RPT_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Subscription reporting test", "08.005");
} /* end Test_Unsubscribe_SubscriptionReporting */

/*
** Test message unsubscription response to an invalid pipe ID
*/
void Test_Unsubscribe_InvalidPipe(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = 0x08fd;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unsubscribe, Invalid Pipe");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, TestPipe);
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_Unsubscribe(MsgId, TestPipe + 1);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid pipe test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_UNSUB_INV_PIPE_EID) == FALSE)
    {
        UT_Text("CFE_SB_UNSUB_INV_PIPE_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe);
    UT_Report(TestStat, "Test_Unsubscribe_API", "Invalid pipe test", "08.006");
} /* end Test_Unsubscribe_InvalidPipe */

/*
** Test message unsubscription response to an invalid pipe owner
*/
void Test_Unsubscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x0877;
    uint32          RealOwner;
    uint16          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Pipe Owner");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "First return was unexpected in unsubscribe - non owner "
                   "Test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_Subscribe(MsgId, PipeId);

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipeId].AppId;

    /* Choose a value that is sure not be owner */
    CFE_SB.PipeTbl[PipeId].AppId = RealOwner + 1;
    ActRtn = CFE_SB_Unsubscribe(MsgId, PipeId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Second return was unexpected in unsubscribe - non owner "
                   "Test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_UNSUB_INV_CALLER_EID) == FALSE)
    {
        UT_Text("CFE_SB_UNSUB_INV_CALLER_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB.PipeTbl[PipeId].AppId = RealOwner;
    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Invalid pipe owner test", "08.007");
} /* end Test_Unsubscribe_InvalidPipeOwner */

/*
** Test message unsubscription response to the first pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_FirstDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId = 0x1805;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    CFE_SB_PipeId_t TestPipe3;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Unsubscribe First Destination With Many");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3");
    CFE_SB_Subscribe(MsgId, TestPipe1);
    CFE_SB_Subscribe(MsgId, TestPipe2);
    CFE_SB_Subscribe(MsgId, TestPipe3);
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Unsubscribe(MsgId, TestPipe1);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in unsubscribe first destination with "
                   "many, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 7;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe1);
    CFE_SB_DeletePipe(TestPipe2);
    CFE_SB_DeletePipe(TestPipe3);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Unsubscribe first destination with many", "08.008");
} /* end Test_Unsubscribe_FirstDestWithMany */

/*
** Test message unsubscription response to a middle pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_MiddleDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId = 0x1805;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    CFE_SB_PipeId_t TestPipe3;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Unsubscribe Middle Destination With Many");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2");
    CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3");
    CFE_SB_Subscribe(MsgId, TestPipe1);
    CFE_SB_Subscribe(MsgId, TestPipe2);
    CFE_SB_Subscribe(MsgId, TestPipe3);
    ExpRtn = CFE_SUCCESS;
    ActRtn = CFE_SB_Unsubscribe(MsgId, TestPipe2);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in unsubscribe middle destination with "
                   "many, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 7;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe1);
    CFE_SB_DeletePipe(TestPipe2);
    CFE_SB_DeletePipe(TestPipe3);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Unsubscribe middle destination with many", "08.009");
} /* end Test_Unsubscribe_MiddleDestWithMany */

/*
** Test message unsubscription by verifying the message destination pointer no
** longer points to the pipe
*/
void Test_Unsubscribe_GetDestPtr(void)
{
    CFE_SB_MsgId_t  MsgId = 0x1805;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Unsubscribe, Get Destination Pointer");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1");
    CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2");
    CFE_SB_Subscribe(MsgId, TestPipe1);
    CFE_SB_Subscribe(MsgId, TestPipe2);
    CFE_SB_Unsubscribe(MsgId, TestPipe2);

    if (CFE_SB_GetDestPtr(MsgId, TestPipe2) != NULL)
    {
        UT_Text("Unexpected return in unsubscribe, get destination pointer, "
                   "exp NULL");
        TestStat = CFE_FAIL;
    }

    ExpRtn = 5;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(TestPipe1);
    CFE_SB_DeletePipe(TestPipe2);
    UT_Report(TestStat, "Test_Unsubscribe_API",
              "Get destination pointer", "08.010");
} /* end Test_Unsubscribe_GetDestPtr */

/*
** Function for calling SB send message API test functions
*/
void Test_SendMsg_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_SendMsg_API");
#endif

    Test_SendMsg_NullPtr();
    Test_SendMsg_InvalidMsgId();
    Test_SendMsg_NoSubscribers();
    Test_SendMsg_MaxMsgSizePlusOne();
    Test_SendMsg_BasicSend();
    Test_SendMsg_SequenceCount();
    Test_SendMsg_QueuePutError();
    Test_SendMsg_PipeFull();
    Test_SendMsg_MsgLimitExceeded();
    Test_SendMsg_GetPoolBufErr();
    Test_SendMsg_ZeroCopyGetPtr();
    Test_SendMsg_ZeroCopySend();
    Test_SendMsg_ZeroCopyPass();
    Test_SendMsg_ZeroCopyReleasePtr();
    Test_SendMsg_DisabledDestination();
    Test_SendMsg_SendWithMetadata();

#ifdef UT_VERBOSE
    UT_Text("End Test_SendMsg_API\n");
#endif
} /* end Test_SendMsg_API */

/*
** Test response to sending a null message on the software bus
*/
void Test_SendMsg_NullPtr(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Null Pointer");
#endif

    SB_ResetUnitTest();
    ActRtn = CFE_SB_SendMsg(NULL);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in null pointer Test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SEND_BAD_ARG_EID) == FALSE)
    {
        UT_Text("CFE_SB_SEND_BAD_ARG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API", "Null pointer test", "09.001");
} /* end Test_SendMsg_NullPtr */

/*
** Test response to sending a message with an invalid ID
*/
void Test_SendMsg_InvalidMsgId(void)
{
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Invalid Message Id");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&TlmPkt, CFE_SB_HIGHEST_VALID_MSGID + 1,
                   sizeof(SB_UT_Test_Tlm_t), TRUE);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid message test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SEND_INV_MSGID_EID) == FALSE)
    {
        UT_Text("CFE_SB_SEND_INV_MSGID_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API", "Invalid message ID test", "09.002");
} /* end Test_SendMsg_InvalidMsgId */

/*
** Test response to sending a message which has no subscribers
*/
void Test_SendMsg_NoSubscribers(void)
{
    CFE_SB_MsgId_t   MsgId = 0x0815;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for No Subscribers");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in no subscribers test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SEND_NO_SUBS_EID) == FALSE)
    {
        UT_Text("CFE_SB_SEND_NO_SUBS_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API", "No subscribers test", "09.003");
} /* end Test_SendMsg_NoSubscribers */

/*
** Test response to sending a message with the message size larger than allowed
*/
void Test_SendMsg_MaxMsgSizePlusOne(void)
{
    CFE_SB_MsgId_t   MsgId = 0x0808;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Maximum Message Size Plus One");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&TlmPkt, MsgId, CFE_SB_MAX_SB_MSG_SIZE + 1, FALSE);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SB_MSG_TOO_BIG;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in maximum message size plus one test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_MSG_TOO_BIG_EID) == FALSE)
    {
        UT_Text("CFE_SB_MSG_TOO_BIG_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API",
              "Maximum message size plus one test", "09.004");
} /* end Test_SendMsg_MaxMsgSizePlusOne */

/*
** Test successfully sending a message on the software bus
*/
void Test_SendMsg_BasicSend(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = 0x0816;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Basic Send");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in basic send test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API", "Basic send test", "09.005");
} /* end Test_SendMsg_BasicSend */

/*
** Test successful send/receive for packet sequence count
*/
void Test_SendMsg_SequenceCount(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = 0x0817;
    CFE_SB_MsgPtr_t  PtrToMsg;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    uint32           PipeDepth = 10;
    int32            TestStat = CFE_PASS;
    int32            ExpRtn;
    int32            ActRtn;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Telemetry Sequence Count");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "SeqCntTestPipe");
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    CFE_SB_Subscribe(MsgId, PipeId);
    CCSDS_WR_SEQ(TlmPktPtr->Hdr, 22);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from send in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from rcv 1 in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }
    else if (PtrToMsg == NULL)
    {
        UT_Text("Unexpected NULL return from rcv 1 in sequence count test");
        TestStat = CFE_FAIL;
    }
    else if (CCSDS_RD_SEQ(PtrToMsg->Hdr) != 1)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected sequence count for send in sequence count test, "
                   "exp=1, act=%d",
                 CCSDS_RD_SEQ(PtrToMsg->Hdr));
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_PassMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from pass in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from rcv 2 in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }
    else if (PtrToMsg == NULL)
    {
        UT_Text("Unexpected NULL return from rcv 2 in sequence count test");
        TestStat = CFE_FAIL;
    }
    else if (CCSDS_RD_SEQ(PtrToMsg->Hdr) != 22)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected sequence count for pass in sequence count test, "
                   "exp=22, act=%d",
                 CCSDS_RD_SEQ(PtrToMsg->Hdr));
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from send 2 in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from rcv 3 in sequence count test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }
    else if (PtrToMsg == NULL)
    {
        UT_Text("Unexpected NULL return from rcv 3 in sequence count test");
        TestStat = CFE_FAIL;
    }
    else if (CCSDS_RD_SEQ(PtrToMsg->Hdr) != 2)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected sequence count for send in sequence count test, "
                   "exp=2, act=%d",
                 CCSDS_RD_SEQ(PtrToMsg->Hdr));
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "Telemetry sequence count test", "09.006");
} /* end Test_SendMsg_SequenceCount */

/*
** Test send message response to a socket queue 'put' error
*/
void Test_SendMsg_QueuePutError(void)
{
    CFE_SB_PipeId_t  PipeId4Error;
    CFE_SB_MsgId_t   MsgId = 0x0818;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    int32            RealQueueId;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for QueuePut Error");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId4Error, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, PipeId4Error);
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), FALSE);

    /* Before sending this message, manipulate the queue_id in the pipe table
     * so that the QueuePut returns an error.  If QueuePut returns an error, an
     * error event is sent, but the expected return from SendMsg (by design)
     * is CFE_SUCCESS
     */
    RealQueueId = CFE_SB.PipeTbl[PipeId4Error].SysQueueId;
    CFE_SB.PipeTbl[PipeId4Error].SysQueueId = OS_MAX_QUEUES + 1;
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in QueuePut error test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_Q_WR_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_Q_WR_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB.PipeTbl[PipeId4Error].SysQueueId = RealQueueId;
    CFE_SB_DeletePipe(PipeId4Error);
    UT_Report(TestStat, "Test_SendMsg_API", "QueuePut error test", "09.007");
} /* end Test_SendMsg_QueuePutError */

/*
** Test send message response when the socket queue is full
*/
void Test_SendMsg_PipeFull(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = 0x0812;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 1;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Pipe Full");
#endif

    SB_ResetUnitTest();
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);

    /* This send should pass */
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return1 in pipe full test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetRtnCode(&QueuePutRtn, OS_QUEUE_FULL, 1);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);

    /* Pipe overflow causes SendMsg to return CFE_SUCCESS */
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return2 in pipe full test, exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_Q_FULL_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_Q_FULL_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API", "Pipe full test", "09.008");
} /* end Test_SendMsg_PipeFull */

/*
** Test send message response to too many messages sent to the pipe
*/
void Test_SendMsg_MsgLimitExceeded(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = 0x0811;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Msg Limit Exceeded");
#endif

    SB_ResetUnitTest();
    PipeDepth = 5;
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), FALSE);
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe");

    /* Set maximum allowed messages on the pipe at one time to 1 */
    CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_Default_Qos, 1);

    /* First send should pass */
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in message limit test1, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* This send should produce a MsgId to Pipe Limit Exceeded message, but
     * return success
     */
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in message limit test2, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_MSGID_LIM_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_MSGID_LIM_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "Msg Limit Exceeded test", "09.009");
} /* end Test_SendMsg_MsgLimitExceeded */

/*
** Test send message response to a buffer descriptor allocation failure
*/
void Test_SendMsg_GetPoolBufErr(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = 0x0809;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for GetPoolBufErr");
#endif

    SB_ResetUnitTest();
    PipeDepth = 1;
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "GetPoolErrPipe");
    CFE_SB_Subscribe(MsgId, PipeId);

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_BLOCK_SIZE, 1);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SB_BUF_ALOC_ERR;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in GetPoolBufErr1 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 3;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_GET_BUF_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_GET_BUF_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API", "GetPoolBufErr test", "09.010");
} /* end Test_SendMsg_GetPoolBufErr */

/*
** Test getting a pointer to a buffer for zero copy mode with buffer
** allocation failures
*/
void Test_SendMsg_ZeroCopyGetPtr(void)
{
    uint32 ZeroCpyBufHndl;
    uint16 MsgSize = 10;
    int32  ExpRtn;
    int32  ActRtn;
    int32  TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for ZeroCopyGetPtr");
#endif

    SB_ResetUnitTest();

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_BLOCK_SIZE, 1);
    ActRtn = (int32) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl);
    ExpRtn = (int32) NULL;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyGetPtr1 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* Have GetPoolBuf stub return error on its second call (actual buffer
     * allocation failed)
     */
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_BLOCK_SIZE, 2);
    ActRtn = (int32) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl);
    ExpRtn = (int32) NULL;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyGetPtr2 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API", "ZeroCopyGetPtr test", "09.011");
} /* end Test_SendMsg_ZeroCopyGetPtr */

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count increments)
*/
void Test_SendMsg_ZeroCopySend(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x0809;
    CFE_SB_MsgPtr_t ZeroCpyMsgPtr = NULL;
    uint32          PipeDepth = 10;
    uint32          ZeroCpyBufHndl = 0;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for CFE_SB_ZeroCopySend");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyTestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    ZeroCpyMsgPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),
                                          &ZeroCpyBufHndl);

    if (ZeroCpyMsgPtr == NULL)
    {
        UT_Text("Unexpected NULL pointer returned from ZeroCopyGetPtr");
        TestStat = CFE_FAIL;
    }
    else
    {
        CFE_SB_InitMsg(ZeroCpyMsgPtr, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
        CCSDS_WR_SEQ(ZeroCpyMsgPtr->Hdr, 22);
    }

    ActRtn = CFE_SB_ZeroCopySend(ZeroCpyMsgPtr, ZeroCpyBufHndl);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from send in zero copy send test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from rcv in zero copy send test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (PtrToMsg == NULL)
    {
        UT_Text("Unexpected NULL return from rcv in zero copy send test");
        TestStat = CFE_FAIL;
    }
    else if (CCSDS_RD_SEQ(PtrToMsg->Hdr) != 1)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected sequence count for send in sequence count test, "
                   "exp=1, act=%d",
                 CCSDS_RD_SEQ(PtrToMsg->Hdr));
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "CFE_SB_ZeroCopySend test", "09.012");
} /* end Test_SendMsg_ZeroCopySend */

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count is unchanged)
*/
void Test_SendMsg_ZeroCopyPass(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x0809;
    CFE_SB_MsgPtr_t ZeroCpyMsgPtr = NULL;
    uint32          PipeDepth = 10;
    uint32          ZeroCpyBufHndl = 0;
    uint16          Seq = 22;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for CFE_SB_ZeroCopyPass");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyPassTestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    ZeroCpyMsgPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),
                                          &ZeroCpyBufHndl);

    if (ZeroCpyMsgPtr == NULL)
    {
        UT_Text("Unexpected NULL pointer returned from ZeroCopyGetPtr");
        TestStat = CFE_FAIL;
    }
    else
    {
      CFE_SB_InitMsg(ZeroCpyMsgPtr, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
      CCSDS_WR_SEQ(ZeroCpyMsgPtr->Hdr, Seq);
    }

    ActRtn = CFE_SB_ZeroCopyPass(ZeroCpyMsgPtr, ZeroCpyBufHndl);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from send in zero copy pass test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from receive in zero copy pass test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (PtrToMsg == NULL)
    {
        UT_Text("Unexpected NULL return from receive in zero copy pass test");
        TestStat = CFE_FAIL;
    }
    else if (CCSDS_RD_SEQ(PtrToMsg->Hdr) != Seq)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected sequence count for send in sequence count test, "
                   "exp=%d, act=%d",
                 Seq, CCSDS_RD_SEQ(PtrToMsg->Hdr));
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "CFE_SB_ZeroCopyPass test", "09.013");
} /* end Test_SendMsg_ZeroCopyPass */

/*
** Test releasing a pointer to a buffer for zero copy mode
*/
void Test_SendMsg_ZeroCopyReleasePtr(void)
{
    CFE_SB_MsgPtr_t ZeroCpyMsgPtr1 = NULL;
    CFE_SB_MsgPtr_t ZeroCpyMsgPtr2 = NULL;
    CFE_SB_MsgPtr_t ZeroCpyMsgPtr3 = NULL;
    uint32          ZeroCpyBufHndl1 = 0;
    uint32          ZeroCpyBufHndl2 = 0;
    uint32          ZeroCpyBufHndl3 = 0;
    uint16          MsgSize = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for ZeroCopyReleasePtr");
#endif

    SB_ResetUnitTest();
    ZeroCpyMsgPtr1 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl1);
    ZeroCpyMsgPtr2 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl2);
    ZeroCpyMsgPtr3 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl3);
    ActRtn = CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyReleasePtr1 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_SetRtnCode(&GetPoolInfoRtn, -1, 1);
    ActRtn = CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2);
    ExpRtn = CFE_SB_BUFFER_INVALID;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyReleasePtr2 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr3, ZeroCpyBufHndl3);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyReleasePtr3 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr1, ZeroCpyBufHndl1);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in ZeroCopyReleasePtr4 test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_SendMsg_API",
              "ZeroCopyReleasePtr test", "09.014");
} /* end Test_SendMsg_ZeroCopyReleasePtr */

/*
** Test send message response with the destination disabled
*/
void Test_SendMsg_DisabledDestination(void)
{
    CFE_SB_PipeId_t       PipeId;
    CFE_SB_MsgId_t        MsgId = 0x0809;
    SB_UT_Test_Tlm_t      TlmPkt;
    CFE_SB_MsgPtr_t       TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_SB_DestinationD_t *DestPtr;
    int32                 PipeDepth;
    int32                 ExpRtn;
    int32                 ActRtn;
    int32                 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Send w/Disabled Destination");
#endif

    SB_ResetUnitTest();
    PipeDepth = 2;
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    DestPtr = CFE_SB_GetDestPtr(MsgId, PipeId);
    DestPtr->Active = CFE_SB_INACTIVE;
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in disabled destination test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "Disabled destination test", "09.015");
} /* end Test_SendMsg_DisabledDestination */

/*
** Test successfully sending a message with the metadata
*/
void Test_SendMsg_SendWithMetadata(void)
{
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_MsgId_t    MsgId = 0x0809;
    SB_UT_Test_Tlm_t  TlmPkt;
    CFE_SB_MsgPtr_t   TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_SB_SenderId_t Sender;
    int32             PipeDepth;
    int32             ExpRtn;
    int32             ActRtn;
    int32             TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Send with Metadata");
#endif

    SB_ResetUnitTest();
    PipeDepth = 2;
/*    Sender.ProcessorId = 123; */
/*    strncpy(Sender.AppName, "Sender", OS_MAX_API_NAME); */
/*    Sender.AppName[OS_MAX_API_NAME] = '\0'; */
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    ActRtn = CFE_SB_SendMsgFull(TlmPktPtr, CFE_SB_DO_NOT_INCREMENT,
                                CFE_SB_SEND_ZEROCOPY);
/*                                CFE_SB_SEND_ZEROCOPY, &Sender); */
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in send with metadata test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_SendMsg_API",
              "Send with metadata test", "09.016");
} /* end Test_SendMsg_SendWithMetadata */

/*
** Function for calling SB receive message API test functions
*/
void Test_RcvMsg_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_RcvMsg_API");
#endif

    Test_RcvMsg_InvalidPipeId();
    Test_RcvMsg_InvalidTimeout();
    Test_RcvMsg_Poll();
    Test_RcvMsg_GetLastSenderNull();
    Test_RcvMsg_GetLastSenderInvalidPipe();
    Test_RcvMsg_GetLastSenderInvalidCaller();
    Test_RcvMsg_GetLastSenderSuccess();
    Test_RcvMsg_Timeout();
    Test_RcvMsg_PipeReadError();
    Test_RcvMsg_PendForever();

#ifdef UT_VERBOSE
    UT_Text("End Test_RcvMsg_API\n");
#endif
} /* end Test_RcvMsg_API */

/*
** Test receiving a message from the software bus with an invalid pipe ID
*/
void Test_RcvMsg_InvalidPipeId(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t InvalidPipeId = 20;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Rcv - Invalid PipeId Rcv");
#endif

    SB_ResetUnitTest();
    CFE_SB.PipeTbl[InvalidPipeId].InUse = CFE_SB_NOT_IN_USE;
    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, InvalidPipeId, CFE_SB_POLL);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid pipe ID test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_BAD_PIPEID_EID) == FALSE)
    {
        UT_Text("CFE_SB_BAD_PIPEID_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_RcvMsg_API", "Invalid pipe ID test", "10.001");
} /* end Test_RcvMsg_InvalidPipeId */

/*
** Test receiving a message response to an invalid timeout value (< -1)
*/
void Test_RcvMsg_InvalidTimeout(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           TimeOut = -5;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Rcv - Invalid Timeout Rcv");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, TimeOut);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in invalid timeout test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_RCV_BAD_ARG_EID) == FALSE)
    {
        UT_Text("CFE_SB_RCV_BAD_ARG_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API", "Invalid timeout test", "10.002");
} /* end Test_RcvMsg_InvalidTimeout */

/*
** Test receiving a message response when there is no message on the queue
*/
void Test_RcvMsg_Poll(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Poll Receive");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_POLL);
    ExpRtn = CFE_SB_NO_MESSAGE;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in poll receive test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API", "Poll receive test", "10.003");
} /* end Test_RcvMsg_Poll */

/*
** Test receive last message response to a null sender ID
*/
void Test_RcvMsg_GetLastSenderNull(void)
{
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for GetLastSender Null Ptr");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    ActRtn = CFE_SB_GetLastSenderId(NULL, PipeId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in GetLastSenderId null pointer test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_LSTSNDER_ERR1_EID) == FALSE)
    {
        UT_Text("CFE_SB_LSTSNDER_ERR1_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API",
              "GetLastSenderId null pointer test",
              "10.004");
} /* end Test_RcvMsg_GetLastSenderNull */

/*
** Test receive last message response to an invalid pipe ID
*/
void Test_RcvMsg_GetLastSenderInvalidPipe(void)
{
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_PipeId_t   InvalidPipeId = 250;
    CFE_SB_SenderId_t *GLSPtr;
    uint32            PipeDepth = 10;
    int32             ExpRtn;
    int32             ActRtn;
    int32             TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for GetLastSender Invalid Pipe");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    ActRtn = CFE_SB_GetLastSenderId(&GLSPtr, InvalidPipeId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in GetLastSenderId invalid pipe test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_LSTSNDER_ERR2_EID) == FALSE)
    {
        UT_Text("CFE_SB_LSTSNDER_ERR2_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API",
              "GetLastSenderId invalid pipe test", "10.005");
} /* end Test_RcvMsg_GetLastSenderInvalidPipe */

/*
** Test receive last message response to an invalid owner ID
*/
void Test_RcvMsg_GetLastSenderInvalidCaller(void)
{
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_SenderId_t *GLSPtr;
    uint32            PipeDepth = 10;
    uint32            OrigPipeOwner;
    int32             ExpRtn;
    int32             ActRtn;
    int32             TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for GetLastSender Invalid Caller");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");

    /* Change pipe owner ID to execute 'invalid caller' code */
    OrigPipeOwner = CFE_SB.PipeTbl[PipeId].AppId;
    CFE_SB.PipeTbl[PipeId].AppId = OrigPipeOwner + 1;
    ActRtn = CFE_SB_GetLastSenderId(&GLSPtr, PipeId);
    ExpRtn = CFE_SB_BAD_ARGUMENT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in GetLastSenderId invalid caller test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_GLS_INV_CALLER_EID) == FALSE)
    {
        UT_Text("CFE_SB_GLS_INV_CALLER_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Restore original pipe owner apid */
    CFE_SB.PipeTbl[PipeId].AppId = OrigPipeOwner;
    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API",
              "GetLastSenderId invalid caller test", "10.006");
} /* end Test_RcvMsg_GetLastSenderInvalidCaller */

/*
** Test successful receive last message request
*/
void Test_RcvMsg_GetLastSenderSuccess(void)
{
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_SenderId_t *GLSPtr;
    uint32            PipeDepth = 10;
    int32             ExpRtn;
    int32             ActRtn;
    int32             TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for GetLastSender Success");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    ActRtn = CFE_SB_GetLastSenderId(&GLSPtr, PipeId);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in GetLastSenderId Success Test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API",
              "GetLastSenderId Success Test", "10.007");
} /* end Test_RcvMsg_GetLastSenderSuccess */

/*
** Test receiving a message response to a timeout
*/
void Test_RcvMsg_Timeout(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           TimeOut = 200;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Timeout Rcv");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");

#ifndef SOCKET_QUEUE
    UT_SetRtnCode(&QueueGetRtn, OS_QUEUE_TIMEOUT, 1);
#endif

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, TimeOut);
    ExpRtn = CFE_SB_TIME_OUT;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in timeout receive test, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API", "Timeout receive test", "10.008");
} /* end Test_RcvMsg_Timeout */

/*
** Test receiving a message response to a pipe read error
*/
void Test_RcvMsg_PipeReadError(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           Val2Restore;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Pipe Read Error");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");

    /* Corrupt the system queueID to get OS_QueueGet to return an error */
    Val2Restore = CFE_SB.PipeTbl[PipeId].SysQueueId;
    CFE_SB.PipeTbl[PipeId].SysQueueId = 100;
    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SB_PIPE_RD_ERR;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return in pipe read error test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_Q_RD_ERR_EID) == FALSE)
    {
        UT_Text("CFE_SB_Q_RD_ERR_EID not sent");
        TestStat = CFE_FAIL;
    }

    /* Restore system queue id */
    CFE_SB.PipeTbl[PipeId].SysQueueId = Val2Restore;

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API", "Pipe read error test", "10.009");
} /* end Test_RcvMsg_PipeReadError */

/*
** Test receiving a message response to a "pend forever" timeout
*/
void Test_RcvMsg_PendForever(void)
{
    CFE_SB_MsgPtr_t  PtrToMsg;
    CFE_SB_MsgId_t   MsgId = 0x0809;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_SB_PipeD_t   *PipeDscPtr;
    uint32           PipeDepth = 10;
    int32            ExpRtn;
    int32            ActRtn;
    int32            TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for Pend Forever");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe");
    CFE_SB_InitMsg(&TlmPkt, MsgId, sizeof(SB_UT_Test_Tlm_t), TRUE);
    CFE_SB_Subscribe(MsgId, PipeId);
    ActRtn = CFE_SB_SendMsg(TlmPktPtr);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from send in pend forever test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ActRtn = CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER);
    ExpRtn = CFE_SUCCESS;

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from receive in pend forever test, "
                   "exp=0x%lx, act=0x%lx",
                 (uint32) ExpRtn, (uint32) ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (PtrToMsg != NULL)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Received Msg 0x%x", CFE_SB_GetMsgId(PtrToMsg));
#ifdef UT_VERBOSE
        UT_Text(cMsg);
#endif
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    PipeDscPtr = CFE_SB_GetPipePtr(PipeId);
    PipeDscPtr->ToTrashBuff = PipeDscPtr->CurrentBuff;
    PipeDscPtr->CurrentBuff = NULL;
    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "Test_RcvMsg_API", "Pend forever test", "10.010");
} /* end Test_RcvMsg_PendForever */

/*
** Test releasing zero copy buffers for all pipes owned by a given app ID
*/
void Test_CleanupApp_API(void)
{
    CFE_SB_PipeId_t PipeId;
    uint32          ZeroCpyBufHndl = 0;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test for CleanupApp");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CFE_SB_ZeroCopyGetPtr(PipeDepth, &ZeroCpyBufHndl);
    CFE_SB_ZeroCopyGetPtr(PipeDepth, &ZeroCpyBufHndl);

    if (CFE_SB.ZeroCopyTail == NULL)
    {
        UT_Text("Unexpected ZeroCopyTail UT_GetNumEventsSent, exp = not NULL");
        TestStat = CFE_FAIL;
    }

    CFE_SB_CleanUpApp(0);

    if (CFE_SB.ZeroCopyTail != NULL)
    {
        UT_Text("Unexpected ZeroCopyTail UT_GetNumEventsSent, exp = NULL");
        TestStat = CFE_FAIL;
    }

    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected return from UT_GetNumEventsSent, "
                   "exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_DELETED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_DELETED_EID not sent");
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CleanupApp_API", "CleanupApp test", "10.011");
} /* end Test_CleanupApp_API */

/*
** Test SB Utility APIs
*/
void Test_SB_Utils(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test for SB Utils");
#endif

    Test_CFE_SB_InitMsg();
    Test_CFE_SB_MsgHdrSize();
    Test_CFE_SB_GetUserData();
    Test_CFE_SB_SetGetMsgId();
    Test_CFE_SB_SetGetUserDataLength();
    Test_CFE_SB_SetGetTotalMsgLength();
    Test_CFE_SB_SetGetMsgTime();
    Test_CFE_SB_TimeStampMsg();
    Test_CFE_SB_SetGetCmdCode();
    Test_CFE_SB_ChecksumUtils();
#ifdef UT_VERBOSE
    UT_Text("End Test for SB Utils\n");
#endif
} /* end Test_SB_Utils */

/*
**
*/
void Test_CFE_SB_InitMsg(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_InitMsg");
#endif

    Test_CFE_SB_InitMsg_True();
    Test_CFE_SB_InitMsg_False();
#ifdef UT_VERBOSE
    UT_Text("End Test_CFE_SB_InitMsg\n");
#endif
} /* end Test_CFE_SB_InitMsg */

/*
** Test message initialization, clearing the message content
*/
void Test_CFE_SB_InitMsg_True(void)
{
    int              result;
    SB_UT_Test_Cmd_t SBCmd;
    CFE_SB_MsgPtr_t  SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin SB_TestInitMsg_True");
#endif

    SB_ResetUnitTest();

    /* Set entire cmd packet to all f's */
    memset(SBCmdPtr, 0xff, sizeof(SB_UT_Test_Cmd_t));
    CFE_SB_InitMsg(SBCmdPtr, CFE_SB_CMD_MID, sizeof(SB_UT_Test_Cmd_t), TRUE);
    result = SBCmd.Cmd32Param1 == 0 &&
             SBCmd.Cmd16Param1 == 0 &&
             SBCmd.Cmd16Param2 == 0 &&
             SBCmd.Cmd8Param1 == 0 &&
             SBCmd.Cmd8Param2 == 0 &&
             SBCmd.Cmd8Param3 == 0 &&
             SBCmd.Cmd8Param4 == 0;
    UT_Report(result, "SB_TestInitMsg_True",
              "Clear message content", "11.001");
} /* end Test_CFE_SB_InitMsg_True */

/*
** Test message initialization, leaving the message content unchanged
*/
void Test_CFE_SB_InitMsg_False(void)
{
    int              result;
    SB_UT_Test_Cmd_t SBCmd;
    CFE_SB_MsgPtr_t  SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin SB_TestInitMsg_False");
#endif

    SB_ResetUnitTest();

    /* Set entire cmd packet to all f's */
    memset(SBCmdPtr, 0xff, sizeof(SB_UT_Test_Cmd_t));
    result = SBCmd.Cmd32Param1 == 0xffffffff &&
             SBCmd.Cmd16Param1 == 0xffff &&
             SBCmd.Cmd16Param2 == 0xffff &&
             SBCmd.Cmd8Param1 == 0xff &&
             SBCmd.Cmd8Param2 == 0xff &&
             SBCmd.Cmd8Param3 == 0xff &&
             SBCmd.Cmd8Param4 == 0xff;
    CFE_SB_InitMsg(SBCmdPtr, CFE_SB_CMD_MID, sizeof(SB_UT_Test_Cmd_t), FALSE);
    UT_Report(result, "SB_TestInitMsg_False",
              "Leave message content", "11.002");
} /* end Test_CFE_SB_InitMsg_False */

/*
** Test getting the size of a command/telemetry message header
*/
void Test_CFE_SB_MsgHdrSize(void)
{
    int32 TestStat;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_MsgHdrSize");
#endif

    /* Test for cmds w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    if (CFE_SB_MsgHdrSize(CFE_SB_CMD_MID) != 8)
    {
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_MsgHdrSize",
              "Commands with secondary header", "11.003");

    /* Test for cmds wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    if (CFE_SB_MsgHdrSize(0x1005) != 6)
    {
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_MsgHdrSize",
              "Commands without secondary header", "11.004");

    /* Test for tlm w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    if (CFE_SB_MsgHdrSize(CFE_ES_HK_TLM_MID) != 12)
    {
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_MsgHdrSize",
              "Telemetry with secondary header", "11.005");

    /* Test for tlm wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    if (CFE_SB_MsgHdrSize(0x0005) != 6)
    {
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_MsgHdrSize",
              "Telemetry without secondary header", "11.006");
} /* end Test_CFE_SB_MsgHdrSize */

/*
** Test getting a pointer to the user data portion of a message
*/
void Test_CFE_SB_GetUserData(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    int32                  TestStat;
    uint8                  *ActualAdrReturned;
    uint8                  *ExpAdrReturned;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_GetUserData");
#endif

    /* Test address returned for cmd pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBCmdPtr, 0x1805);
    ActualAdrReturned = CFE_SB_GetUserData(SBCmdPtr);
    ExpAdrReturned = (uint8 *) SBCmdPtr + 8;

    if (ActualAdrReturned != ExpAdrReturned)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Address of data for commands with secondary header is "
                   "packet address + 8\n PktAddr %p, Rtn %p, Exp %p",
                 (void *) SBCmdPtr, ActualAdrReturned, ExpAdrReturned);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_GetUserData",
              "Command packet with secondary header test", "11.007");

    /* Test address returned for cmd pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1005);
    ActualAdrReturned = CFE_SB_GetUserData(SBNoSecHdrPktPtr);
    ExpAdrReturned = (uint8 *) SBNoSecHdrPktPtr + 6;

    if (ActualAdrReturned != ExpAdrReturned)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Address of data for commands without secondary header is "
                   "packet address + 6\n PktAddr %p, Rtn %p, Exp %p",
                 (void *) SBNoSecHdrPktPtr, ActualAdrReturned, ExpAdrReturned);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_GetUserData",
              "Command packet without secondary header test", "11.008");

    /* Test address returned for tlm pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);
    ActualAdrReturned = CFE_SB_GetUserData(SBTlmPtr);
    ExpAdrReturned = (uint8 *) SBTlmPtr + 12;

    if (ActualAdrReturned != ExpAdrReturned)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Address of data for telemetry packets with secondary header "
                   "is Pkt Addr + 12\n PktAddr %p, Rtn %p, Exp %p",
                 (void *) SBTlmPtr, ActualAdrReturned, ExpAdrReturned);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_GetUserData",
              "Telemetry packets with secondary header test", "11.009");

    /* Test address returned for tlm pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0005);
    ActualAdrReturned = CFE_SB_GetUserData(SBNoSecHdrPktPtr);
    ExpAdrReturned = (uint8 *) SBNoSecHdrPktPtr + 6;

    if (ActualAdrReturned != ExpAdrReturned)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Address of data for telemetry packets without secondary "
                   "header is Pkt Addr + 6\n PktAddr %p, Rtn %p, Exp %p",
                 (void *) SBNoSecHdrPktPtr, ActualAdrReturned, ExpAdrReturned);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_GetUserData",
              "Telemetry packets without secondary header test", "11.010");
} /* end Test_CFE_SB_GetUserData */

/*
** Test setting and getting the message ID of a message
*/
void Test_CFE_SB_SetGetMsgId(void)
{
    SB_UT_Test_Cmd_t SBCmd;
    CFE_SB_MsgPtr_t  SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    CFE_SB_MsgId_t   MsgIdReturned;
    uint32           TestStat;
    uint32           i;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_SetGetMsgId");
#endif

    /* Test setting and getting the message ID of a message */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set entire command packet to all f's */
    memset(SBCmdPtr, 0xff, sizeof(SB_UT_Test_Cmd_t));
    CFE_SB_SetMsgId(SBCmdPtr, CFE_SB_CMD_MID);
    MsgIdReturned = CFE_SB_GetMsgId(SBCmdPtr);

    if (MsgIdReturned != CFE_SB_CMD_MID)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "CFE_SB_GetMsgId returned 0x%lx, expected 0x%lx",
                 (uint32) MsgIdReturned, (uint32) CFE_SB_CMD_MID);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgId",
              "Get the set message ID test", "11.011");

    /* Test setting and getting the message ID of a message looping through
     * all values
     */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Looping through every value from 0 to 0xffff */
    for (i = 0; i < 0x10000; i++)
    {
        CFE_SB_SetMsgId(SBCmdPtr, i);

        if (CFE_SB_GetMsgId(SBCmdPtr) != i)
        {
            break;
        }
    }

    UT_Report(i == 0x10000, "Test_CFE_SB_SetGetMsgId", "Loop test", "11.012");
} /* end Test_CFE_SB_SetGetMsgId */

/*
** Test setting and getting the user data size of a message
*/
void Test_CFE_SB_SetGetUserDataLength(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    int32                  SetSize, TestStat;
    uint16                 SizeReturned;
    int16                  ActualPktLenField;
    int16                  ExpPktLenField;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_SetGetUserDataLength");
#endif

    /* CCSDS pkt length field = SecHdrSize + data - 1 */

    /* Loop through all pkt length values for cmd pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBCmdPtr, 0x1805);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetUserDataLength(SBCmdPtr, SetSize);
        SizeReturned = CFE_SB_GetUserDataLength(SBCmdPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBCmdPtr);
        ExpPktLenField = 2 + SetSize - 1; /* SecHdrSize + data - 1 */

        if (SizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     SizeReturned, SetSize, ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetUserDataLength",
              "Command packet with secondary header test", "11.013");

    /* Loop through all pkt length values for cmd pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1005);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetUserDataLength(SBNoSecHdrPktPtr, SetSize);
        SizeReturned = CFE_SB_GetUserDataLength(SBNoSecHdrPktPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBNoSecHdrPktPtr);
        ExpPktLenField = 0 + SetSize - 1; /* SecHdrSize + data - 1 */

        if (SizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     SizeReturned, SetSize, ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetUserDataLength",
              "Command packet without secondary header test", "11.014");

    /* Loop through all pkt length values for tlm pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetUserDataLength(SBTlmPtr, SetSize);
        SizeReturned = CFE_SB_GetUserDataLength(SBTlmPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBTlmPtr);
        ExpPktLenField = 6 + SetSize - 1; /* SecHdrSize + data - 1 */

        if (SizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     SizeReturned, SetSize, ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetUserDataLength",
              "Telemetry packet with secondary header test", "11.015");

    /* Loop through all pkt length values for tlm pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0005);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetUserDataLength(SBNoSecHdrPktPtr, SetSize);
        SizeReturned = CFE_SB_GetUserDataLength(SBNoSecHdrPktPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBNoSecHdrPktPtr);
        ExpPktLenField = 0 + SetSize - 1; /* SecHdrSize + data - 1 */

        if (SizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     SizeReturned, SetSize, ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetUserDataLength",
              "Telemetry packet without secondary header test", "11.016");
} /* end Test_CFE_SB_SetGetUserDataLength */

/*
** Test setting and getting the total message size
*/
void Test_CFE_SB_SetGetTotalMsgLength(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    int32                  SetSize, TestStat;
    uint16                 TotalMsgSizeReturned;
    int16                  ActualPktLenField;
    int16                  ExpPktLenField;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_SetGetTotalMsgLength");
#endif

    /* CCSDS pkt length field = TotalPktSize - 7 */

    /* Loop through all pkt length values for cmd pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBCmdPtr, 0x1805);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetTotalMsgLength(SBCmdPtr, SetSize);
        TotalMsgSizeReturned = CFE_SB_GetTotalMsgLength(SBCmdPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBCmdPtr);
        ExpPktLenField = SetSize - 7; /* TotalPktSize - 7 */

        if (TotalMsgSizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     TotalMsgSizeReturned, SetSize,
                     ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetTotalMsgLength",
              "Command packet with secondary header test", "11.017");

    /* Loop through all pkt length values for cmd pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1005);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetTotalMsgLength(SBNoSecHdrPktPtr, SetSize);
        TotalMsgSizeReturned = CFE_SB_GetTotalMsgLength(SBNoSecHdrPktPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBNoSecHdrPktPtr);
        ExpPktLenField = SetSize - 7; /* TotalPktSize - 7 */

        if (TotalMsgSizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     TotalMsgSizeReturned, SetSize,
                     ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetTotalMsgLength",
              "Command packet without secondary header test", "11.018");

    /* Loop through all pkt length values for tlm pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetTotalMsgLength(SBTlmPtr, SetSize);
        TotalMsgSizeReturned = CFE_SB_GetTotalMsgLength(SBTlmPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBTlmPtr);
        ExpPktLenField = SetSize - 7; /* TotalPktSize - 7 */

        if (TotalMsgSizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     TotalMsgSizeReturned, SetSize,
                     ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetTotalMsgLength",
              "Telemetry packet with secondary header test", "11.019");

    /* Loop through all pkt length values for tlm pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0005);

    for (SetSize = 0; SetSize < 0x10000; SetSize++)
    {
        CFE_SB_SetTotalMsgLength(SBNoSecHdrPktPtr, SetSize);
        TotalMsgSizeReturned = CFE_SB_GetTotalMsgLength(SBNoSecHdrPktPtr);
        ActualPktLenField = UT_GetActualPktLenField(SBNoSecHdrPktPtr);
        ExpPktLenField = SetSize - 7; /* TotalPktSize - 7 */

        if (TotalMsgSizeReturned != SetSize ||
            ActualPktLenField != ExpPktLenField)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "SzRet=%u, SetSz=%ld, ActPL=%d, ExpPL=%d",
                     TotalMsgSizeReturned, SetSize,
                     ActualPktLenField, ExpPktLenField);
            UT_Text(cMsg);
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetTotalMsgLength",
              "Telemetry packet without secondary header test", "11.020");
} /* end Test_CFE_SB_SetGetTotalMsgLength */

/*
** Test setting and getting the message time field
*/
void Test_CFE_SB_SetGetMsgTime(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    CFE_TIME_SysTime_t     SetTime, GetTime;
    int32                  RtnFromSet, TestStat;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_SetGetMsgTime");
#endif

    /* Begin test for cmd pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBCmdPtr, 0xff, sizeof(SB_UT_Test_Cmd_t));

    /* Set MsgId to 0x1805 */
    CFE_SB_SetMsgId(SBCmdPtr, 0x1805);
    SetTime.Seconds = 0x4321;
    SetTime.Subseconds = 0x8765;
    RtnFromSet = CFE_SB_SetMsgTime(SBCmdPtr, SetTime);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBCmdPtr);

    /* Verify CFE_SB_SetMsgTime returns CFE_SB_WRONG_MSG_TYPE for cmd
     * pkts w/sec hdr
     */
    if (RtnFromSet != CFE_SB_WRONG_MSG_TYPE)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "RtnFromSet=%ld, ExpReturn=0x%lx",
                 RtnFromSet, CFE_SB_WRONG_MSG_TYPE);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBCmdPtr, sizeof(SB_UT_Test_Cmd_t));
    }
    /* Verify the call to CFE_SB_GetMsgTime returns a time value of zero */
    else if (GetTime.Seconds != 0 || GetTime.Subseconds != 0)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "GetTime.Sec=%lu, GetTime.Subsec=%lu",
                 GetTime.Seconds, GetTime.Subseconds);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBCmdPtr, sizeof(SB_UT_Test_Cmd_t));
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgTime",
              "Command packet with secondary header test", "11.021");

    /* Begin test for cmd pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBNoSecHdrPktPtr, 0xff, sizeof(SB_UT_TstPktWoSecHdr_t));

    /* Set MsgId to 0x1005 */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1005);
    SetTime.Seconds = 0x4321;
    SetTime.Subseconds = 0x8765;
    RtnFromSet = CFE_SB_SetMsgTime(SBNoSecHdrPktPtr, SetTime);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBNoSecHdrPktPtr);

    /* Verify CFE_SB_SetMsgTime returns CFE_SB_WRONG_MSG_TYPE for cmd
     * pkts wo/sec hdr
     */
    if (RtnFromSet != CFE_SB_WRONG_MSG_TYPE)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "RtnFromSet=%ld, ExpReturn=0x%lx",
                 RtnFromSet, CFE_SB_WRONG_MSG_TYPE);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
    }
    /* Verify the call to CFE_SB_GetMsgTime returns a time value of zero */
    else if (GetTime.Seconds != 0 || GetTime.Subseconds != 0)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "GetTime.Sec=%lu, GetTime.Subsec=%lu",
                 GetTime.Seconds, GetTime.Subseconds);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgTime",
              "Command packet without secondary header test", "11.022");

    /* Begin test for tlm pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBTlmPtr, 0xff, sizeof(SB_UT_Test_Tlm_t));

    /* Set MsgId to 0x0805 */
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);
    SetTime.Seconds = 0x01234567;
    SetTime.Subseconds = 0x89abcdef;
    RtnFromSet = CFE_SB_SetMsgTime(SBTlmPtr, SetTime);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBTlmPtr);

    /* Verify CFE_SB_SetMsgTime returns CFE_SUCCESS for tlm pkts w/sec hdr */
    if (RtnFromSet != CFE_SUCCESS)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "RtnFromSet=%ld, ExpReturn=0", RtnFromSet);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
    }
    /* Verify CFE_SB_GetMsgTime returns the SetTime value w/2 LSBytes
     * of subseconds zeroed out
     */
    else if (GetTime.Seconds != SetTime.Seconds ||
             GetTime.Subseconds != (SetTime.Subseconds & 0xffff0000))
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "SetTime.Sec=%lu, GetTime.Sec=%lu, SetTime.Subsec=%lu, "
                   "GetTime.Subsec=%lu",
                 SetTime.Seconds, GetTime.Seconds,
                 SetTime.Subseconds, GetTime.Subseconds);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgTime",
              "Telemetry packet with secondary header test", "11.023");

    /* Begin test for tlm pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBNoSecHdrPktPtr, 0xff, sizeof(SB_UT_TstPktWoSecHdr_t));

    /* Set MsgId to 0x0005 */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0005);
    SetTime.Seconds = 0x01234567;
    SetTime.Subseconds = 0x89abcdef;
    RtnFromSet = CFE_SB_SetMsgTime(SBNoSecHdrPktPtr, SetTime);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBNoSecHdrPktPtr);

    /* Verify CFE_SB_SetMsgTime returns CFE_SB_WRONG_MSG_TYPE for tlm
     * pkts wo/sec hdr
     */
    if (RtnFromSet != CFE_SB_WRONG_MSG_TYPE)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "RtnFromSet=%ld, ExpReturn=0x%lx",
                 RtnFromSet, CFE_SB_WRONG_MSG_TYPE);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
    }
    /* Verify the call to CFE_SB_GetMsgTime returns a time value of zero */
    else if (GetTime.Seconds != 0 || GetTime.Subseconds != 0)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "GetTime.Sec=%lu, GetTime.Subsec=%lu",
                 GetTime.Seconds, GetTime.Subseconds);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgTime",
              "Telemetry packet without secondary header test", "11.024");
} /* end Test_CFE_SB_SetGetMsgTime */

/*
** Test setting the time field to the current time
*/
void Test_CFE_SB_TimeStampMsg(void)
{
    SB_UT_Test_Tlm_t   SBTlm;
    CFE_SB_MsgPtr_t    SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    CFE_TIME_SysTime_t GetTime;
    int32              TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_TimeStampMsg");
#endif

    /* Begin test for tlm pkts w/sec hdr */
    SB_ResetUnitTest();

    /* Note: Only Tlm Pkt with Sec Hdr is tested here because this function
     * (Test_CFE_SB_TimeStampMsg) is simply a call to
     * Test_CFE_SB_SetGetMsgTime.  Test_CFE_SB_SetGetMsgTime has covered
     * the other pkt types
     */

    /* Set MsgId to all f's */
    memset(SBTlmPtr, 0xff, sizeof(SB_UT_Test_Tlm_t));

    /* Set MsgId to 0x0805 */
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);
    CFE_SB_TimeStampMsg(SBTlmPtr);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBTlmPtr);
    CFE_SB_TimeStampMsg(SBTlmPtr);
    GetTime.Seconds = 0xffff;
    GetTime.Subseconds = 0xffff;
    GetTime = CFE_SB_GetMsgTime(SBTlmPtr);

    /* Verify CFE_SB_GetMsgTime returns the time value expected by
     * CFE_TIME_GetTime.  The stub for CFE_TIME_GetTime simply increments
     * the seconds cnt on each call
     */
    if (GetTime.Seconds != 3 || GetTime.Subseconds != 0)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "GetTime.Sec=%lu, ExpSecs=3; GetTime.Subsec=%lu, "
                   "ExpSubsecs=0",
                 GetTime.Seconds, GetTime.Subseconds);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
        UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetMsgTime",
              "Telemetry packet with secondary header test", "11.025");
} /* end Test_CFE_SB_TimeStampMsg */

/*
** Test setting and getting the opcode field of message
*/
void Test_CFE_SB_SetGetCmdCode(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    int32                  TestStat, RtnFromSet, ExpRtnFrmSet;
    uint16                 CmdCodeSet, CmdCodeReturned;
    uint8                  ActualCmdCodeField;
    int16                  ExpCmdCode;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_SetGetCmdCode");
#endif

    /* Loop through all cmd code values(plus a few invalid) for cmd
     * pkts w/sec hdr
     */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBCmdPtr, 0xff, sizeof(SB_UT_Test_Cmd_t));

    /* Set MsgId to 0x1805 */
    CFE_SB_SetMsgId(SBCmdPtr, 0x1805);

    for (CmdCodeSet = 0; CmdCodeSet < 0x100; CmdCodeSet++)
    {
        RtnFromSet = CFE_SB_SetCmdCode(SBCmdPtr, CmdCodeSet);
        ExpRtnFrmSet = CFE_SUCCESS;
        CmdCodeReturned = CFE_SB_GetCmdCode(SBCmdPtr);
        ActualCmdCodeField = UT_GetActualCmdCodeField(SBCmdPtr);

        /* GSFC CmdCode is the 7 LSBs of the 1st byte of cmd sec hdr */
        ExpCmdCode = CmdCodeSet & 0x007f;

        if (CmdCodeReturned != ExpCmdCode ||
            ActualCmdCodeField != ExpCmdCode ||
            RtnFromSet != ExpRtnFrmSet)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "CCRet=%d, CCSet=%d, ActCC=%d, ExpCC=%d, "
                       "RtnFrmSet=0x%lx, ExpRtFrmSet=0x%lx",
                     CmdCodeReturned, CmdCodeSet, ActualCmdCodeField,
                     ExpCmdCode, (uint32) RtnFromSet, (uint32) ExpRtnFrmSet);
            UT_Text(cMsg);
            UT_DisplayPkt(SBCmdPtr, sizeof(SB_UT_Test_Cmd_t));
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetCmdCode",
              "Command packet with secondary header test", "11.026");

    /* Loop through all cmd code values (plus a few invalid) for cmd
     * pkts wo/sec hdr
     */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBNoSecHdrPktPtr, 0xff, sizeof(SB_UT_TstPktWoSecHdr_t));

    /* Set MsgId to 0x1005 */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1005);

    for (CmdCodeSet = 0; CmdCodeSet < 0x100; CmdCodeSet++)
    {
        RtnFromSet = CFE_SB_SetCmdCode(SBNoSecHdrPktPtr, CmdCodeSet);
        ExpRtnFrmSet = CFE_SB_WRONG_MSG_TYPE;
        CmdCodeReturned = CFE_SB_GetCmdCode(SBNoSecHdrPktPtr);
        ActualCmdCodeField = UT_GetActualCmdCodeField(SBNoSecHdrPktPtr);

        /* GSFC CmdCode is the 7 LSBs of the 1st byte of cmd Sec hdr */
        ExpCmdCode = CmdCodeSet & 0x007f;

        if (RtnFromSet != ExpRtnFrmSet)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "CCRet=%d, CCSet=%d, ActCC=%d, ExpCC=%d, "
                       "RtnFrmSet=0x%lx, ExpRtFrmSet=0x%lx",
                     CmdCodeReturned, CmdCodeSet, ActualCmdCodeField,
                     ExpCmdCode, (uint32) RtnFromSet, (uint32) ExpRtnFrmSet);
            UT_Text(cMsg);
            UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetCmdCode",
              "Command packet without secondary header test", "11.027");

    /* Loop through all cmd code values (plus a few invalid) for tlm
     * pkts w/sec hdr
     */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set MsgId to all f's */
    memset(SBTlmPtr, 0xff, sizeof(SB_UT_Test_Tlm_t));

    /* Set MsgId to 0x0805 */
    CFE_SB_SetMsgId(SBTlmPtr, 0x0805);

    for (CmdCodeSet = 0; CmdCodeSet < 0x100; CmdCodeSet++)
    {
        RtnFromSet = CFE_SB_SetCmdCode(SBTlmPtr, CmdCodeSet);
        ExpRtnFrmSet = CFE_SB_WRONG_MSG_TYPE;
        CmdCodeReturned = CFE_SB_GetCmdCode(SBTlmPtr);
        ActualCmdCodeField = UT_GetActualCmdCodeField(SBTlmPtr);

        /* GSFC CmdCode is the 7 LSBs of the 1st byte of cmd Sec hdr */
        ExpCmdCode = CmdCodeSet & 0x007f;

        if (RtnFromSet != ExpRtnFrmSet)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "CCRet=%d, CCSet=%d, ActCC=%d, ExpCC=%d, "
                       "RtnFrmSet=0x%lx, ExpRtFrmSet=0x%lx",
                     CmdCodeReturned, CmdCodeSet, ActualCmdCodeField,
                     ExpCmdCode, (uint32) RtnFromSet, (uint32) ExpRtnFrmSet);
            UT_Text(cMsg);
            UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetCmdCode",
              "Telemetry packet with secondary header test", "11.028");

    /* Loop through all cmd code values (plus a few invalid) for tlm
     * pkts wo/sec hdr
     */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Set pkt to all f's */
    memset(SBNoSecHdrPktPtr, 0xff, sizeof(SB_UT_TstPktWoSecHdr_t));

    /* Set MsgId to 0x0005 */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0005);

    for (CmdCodeSet = 0; CmdCodeSet < 0x100; CmdCodeSet++)
    {
        RtnFromSet = CFE_SB_SetCmdCode(SBNoSecHdrPktPtr, CmdCodeSet);
        ExpRtnFrmSet = CFE_SB_WRONG_MSG_TYPE;
        CmdCodeReturned = CFE_SB_GetCmdCode(SBNoSecHdrPktPtr);
        ActualCmdCodeField = UT_GetActualCmdCodeField(SBNoSecHdrPktPtr);

        /* GSFC CmdCode is the 7 LSBs of the 1st byte of cmd sec hdr */
        ExpCmdCode = CmdCodeSet & 0x007f;

        if (RtnFromSet != ExpRtnFrmSet)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "CCRet=%d, CCSet=%d, ActCC=%d, ExpCC=%d, "
                       "RtnFrmSet=0x%lx, ExpRtFrmSet=0x%lx",
                     CmdCodeReturned, CmdCodeSet, ActualCmdCodeField,
                     ExpCmdCode, (uint32) RtnFromSet, (uint32) ExpRtnFrmSet);
            UT_Text(cMsg);
            UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
            TestStat = CFE_FAIL;
            break;
        }
    }

    UT_Report(TestStat, "Test_CFE_SB_SetGetCmdCode",
              "Telemetry packet without secondary header test", "11.029");
} /* end Test_CFE_SB_SetGetCmdCode */

/*
** Test generating, setting, getting, and validating a checksum field
** for a message
*/
void Test_CFE_SB_ChecksumUtils(void)
{
    SB_UT_Test_Cmd_t       SBCmd;
    CFE_SB_MsgPtr_t        SBCmdPtr = (CFE_SB_MsgPtr_t) &SBCmd;
    SB_UT_Test_Tlm_t       SBTlm;
    CFE_SB_MsgPtr_t        SBTlmPtr = (CFE_SB_MsgPtr_t) &SBTlm;
    SB_UT_TstPktWoSecHdr_t SBNoSecHdrPkt;
    CFE_SB_MsgPtr_t        SBNoSecHdrPktPtr = (CFE_SB_MsgPtr_t) &SBNoSecHdrPkt;
    uint16                 RtnFrmGet, ExpRtnFrmGet;
    boolean                RtnFrmValidate, ExpRtnFrmVal;
    int32                  TestStat;

#ifdef UT_VERBOSE
    UT_Text("Begin Test_CFE_SB_ChecksumUtils");
#endif

    /* Begin test for cmd pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Initialize pkt, setting data to zero */
    CFE_SB_InitMsg(SBCmdPtr, 0x1805, sizeof(SB_UT_Test_Cmd_t), TRUE);

    /* Set checksum field */
    CFE_SB_GenerateChecksum(SBCmdPtr);
    RtnFrmGet = CFE_SB_GetChecksum(SBCmdPtr);
    ExpRtnFrmGet = 0x2f;

    /* Validation expected to return TRUE */
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBCmdPtr);
    ExpRtnFrmVal = TRUE;

    if (RtnFrmGet != ExpRtnFrmGet || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected results in 1st check, RtnFrmValidate=%d, "
                   "ExpRtnFrmVal=%d, RtnFrmGet=0x%x, ExpRtnFrmGet=0x%x",
                 RtnFrmValidate, ExpRtnFrmVal, RtnFrmGet, ExpRtnFrmGet);
        UT_Text(cMsg);
        UT_DisplayPkt(SBCmdPtr, sizeof(SB_UT_Test_Cmd_t));
        TestStat = CFE_FAIL;
    }

    /* Change 1 byte in pkt and verify checksum is no longer valid.
     * Increment MsgId by 1 to 0x1806.  Validation expected to
     * return FALSE
     */
    CFE_SB_SetMsgId(SBCmdPtr, 0x1806);
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBCmdPtr);
    ExpRtnFrmVal = FALSE;

    if (TestStat == CFE_FAIL || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Second validate...RtnFrmValidate=%d, ExpRtnFrmVal=%d",
                 RtnFrmValidate, ExpRtnFrmVal);
        UT_Text(cMsg);
        UT_DisplayPkt(SBCmdPtr, sizeof(SB_UT_Test_Cmd_t));
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_ChecksumUtils",
              "Command packet with secondary header test", "11.030");

    /* Begin test for cmd pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Initialize pkt, setting data to zero */
    CFE_SB_InitMsg(SBNoSecHdrPktPtr, 0x1005,
                   sizeof(SB_UT_TstPktWoSecHdr_t), TRUE);

    /* Set checksum field */
    CFE_SB_GenerateChecksum(SBNoSecHdrPktPtr);
    RtnFrmGet = CFE_SB_GetChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmGet = 0;

    /* Validation expected to return FALSE */
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmVal = FALSE;

    if (RtnFrmGet != ExpRtnFrmGet || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected results in 1st check, RtnFrmValidate=%d, "
                   "ExpRtnFrmVal=%d, RtnFrmGet=0x%x, ExpRtnFrmGet=0x%x",
                 RtnFrmValidate, ExpRtnFrmVal, RtnFrmGet, ExpRtnFrmGet);
        UT_Text(cMsg);
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
        TestStat = CFE_FAIL;
    }

    /* Change 1 byte in pkt and verify checksum is no longer valid.
     * Increment MsgId by 1 to 0x1006.  Validation expected to
     * return FALSE
     */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x1006);
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmVal = FALSE;

    if (TestStat == CFE_FAIL || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Second validate...RtnFrmValidate=%d, ExpRtnFrmVal=%d",
                 RtnFrmValidate, ExpRtnFrmVal);
        UT_Text(cMsg);
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_ChecksumUtils",
              "Command packet without secondary header test", "11.031");

    /* Begin test for tlm pkts w/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Initialize pkt, setting data to zero */
    CFE_SB_InitMsg(SBTlmPtr, 0x0805, sizeof(SB_UT_Test_Tlm_t), TRUE);

    /* Set checksum field */
    CFE_SB_GenerateChecksum(SBTlmPtr);
    RtnFrmGet = CFE_SB_GetChecksum(SBTlmPtr);
    ExpRtnFrmGet = 0;

    /* Validation expected to return FALSE */
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBTlmPtr);
    ExpRtnFrmVal = FALSE;

    if (RtnFrmGet != ExpRtnFrmGet || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected results in 1st check, RtnFrmValidate=%d, "
                   "ExpRtnFrmVal=%d, RtnFrmGet=0x%x, ExpRtnFrmGet=0x%x",
                 RtnFrmValidate, ExpRtnFrmVal, RtnFrmGet, ExpRtnFrmGet);
        UT_Text(cMsg);
        UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
        TestStat = CFE_FAIL;
    }

    /* Change 1 byte in pkt and verify checksum is no longer valid.
     * Increment MsgId by 1 to 0x0806.  Validation expected to return FALSE
     */
    CFE_SB_SetMsgId(SBTlmPtr, 0x1806);
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBTlmPtr);
    ExpRtnFrmVal = FALSE;

    if (TestStat == CFE_FAIL || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Second validate...RtnFrmValidate=%d, ExpRtnFrmVal=%d",
                 RtnFrmValidate, ExpRtnFrmVal);
        UT_Text(cMsg);
        UT_DisplayPkt(SBTlmPtr, sizeof(SB_UT_Test_Tlm_t));
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_ChecksumUtils",
              "Telemetry packet with secondary header test", "11.032");

    /* Begin test for tlm pkts wo/sec hdr */
    SB_ResetUnitTest();
    TestStat = CFE_PASS;

    /* Initialize pkt, setting data to zero */
    CFE_SB_InitMsg(SBNoSecHdrPktPtr, 0x0005,
                   sizeof(SB_UT_TstPktWoSecHdr_t), TRUE);

    /* Setting checksum field */
    CFE_SB_GenerateChecksum(SBNoSecHdrPktPtr);
    RtnFrmGet = CFE_SB_GetChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmGet = 0;

    /* Validation expected to return FALSE */
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmVal = FALSE;

    if (RtnFrmGet != ExpRtnFrmGet || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected results in 1st check, RtnFrmValidate=%d, "
                   "ExpRtnFrmVal=%d, RtnFrmGet=0x%x, ExpRtnFrmGet=0x%x",
                 RtnFrmValidate, ExpRtnFrmVal, RtnFrmGet, ExpRtnFrmGet);
        UT_Text(cMsg);
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
        TestStat = CFE_FAIL;
    }

    /* Change 1 byte in pkt and verify checksum is no longer valid.
     * Increment MsgId by 1 to 0x0006.  Validation expected to
     * return FALSE
     */
    CFE_SB_SetMsgId(SBNoSecHdrPktPtr, 0x0006);
    RtnFrmValidate = CFE_SB_ValidateChecksum(SBNoSecHdrPktPtr);
    ExpRtnFrmVal = FALSE;

    if (TestStat == CFE_FAIL || RtnFrmValidate != ExpRtnFrmVal)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Second validate...RtnFrmValidate=%d, ExpRtnFrmVal=%d",
                 RtnFrmValidate, ExpRtnFrmVal);
        UT_Text(cMsg);
        UT_DisplayPkt(SBNoSecHdrPktPtr, sizeof(SB_UT_TstPktWoSecHdr_t));
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "Test_CFE_SB_ChecksumUtils",
              "Telemetry packet without secondary header test", "11.033");
} /* end Test_CFE_SB_ChecksumUtils */

/*
** Function for calling SB special test cases functions
*/
void Test_SB_SpecialCases(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Special Cases");
#endif

    Test_OS_MutSem_ErrLogic();
    Test_GetPipeName_ErrLogic();
    Test_ReqToSendEvent_ErrLogic();
    Test_PutDestBlk_ErrLogic();

#ifdef UT_VERBOSE
    UT_Text("End Test Special Cases\n");
#endif
} /* end Test_SB_SpecialCases */

/*
** Test pipe creation with semaphore take and give failures
*/
void Test_OS_MutSem_ErrLogic(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = 0x1801;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Shared Semaphore Give/Take Failure");
#endif

    SB_ResetUnitTest();
    UT_SetRtnCode(&MutSemTakeRtn, CFE_OS_SEM_FAILURE, 1);
    UT_SetRtnCode(&MutSemGiveRtn, CFE_OS_SEM_FAILURE, 2);
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CFE_SB_Subscribe(MsgId, PipeId);
    ExpRtn = 2;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_SUBSCRIPTION_RCVD_EID) == FALSE)
    {
        UT_Text("CFE_SB_SUBSCRIPTION_RCVD_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_(Un)LockSharedData",
              "Semaphore give/take failure test", "12.001");
} /* end Test_OS_MutSemTake_ErrLogic */

/*
** Test getting a pipe name using an invalid pipe ID
*/
void Test_GetPipeName_ErrLogic(void)
{
    CFE_SB_PipeId_t PipeId;
    char            *CharStar;
    uint16          PipeDepth = 50;
    int32           ExpRtn;
    int32           ActRtn;
    int32           TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test GetPipeName Error");
#endif

    SB_ResetUnitTest();
    CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe");
    CharStar = CFE_SB_GetPipeName(CFE_SB_MAX_PIPES);

    if (*CharStar != '\0')
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from CFE_SB_CreatePipe, exp=NULL, act=%s",
                 CharStar);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 1;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    if (UT_EventIsInHistory(CFE_SB_PIPE_ADDED_EID) == FALSE)
    {
        UT_Text("CFE_SB_PIPE_ADDED_EID not sent");
        TestStat = CFE_FAIL;
    }

    CFE_SB_DeletePipe(PipeId);
    UT_Report(TestStat, "CFE_SB_GetPipeName",
              "Get pipe name error test", "12.002");
} /* end Test_GetPipeName_ErrLogic */

/*
** Test successful recursive event prevention
*/
void Test_ReqToSendEvent_ErrLogic(void)
{
    uint32 TaskId = 13;
    uint32 Bit = 5;
    int32  ExpRtn;
    int32  ActRtn;
    int32  TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test ReqToSendEvent Error");
#endif

    SB_ResetUnitTest();

    /* Clear task bits, then call function, which should set the bit for
     * the specified task
     */
    CFE_SB.StopRecurseFlags[TaskId] = 0x0000;
    ExpRtn = CFE_SB_GRANTED;
    ActRtn = CFE_SB_RequestToSendEvent(TaskId, Bit);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn when bit not set, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    /* Call the function a second time; the result should indicate that the
     * bit is already set
     */
    ExpRtn = CFE_SB_DENIED;
    ActRtn = CFE_SB_RequestToSendEvent(TaskId, Bit);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn when bit set, exp=%ld, act=%ld", ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_RequestToSendEvent",
              "Request to send event error test", "12.003");
} /* end Test_ReqToSendEvent_ErrLogic */

/*
** Test getting a destination descriptor to the SB memory pool using a null
** destination pointer
*/
void Test_PutDestBlk_ErrLogic(void)
{
    int32 ExpRtn;
    int32 ActRtn;
    int32 TestStat = CFE_PASS;

#ifdef UT_VERBOSE
    UT_Text("Begin Test PutDestBlk Error");
#endif

    SB_ResetUnitTest();
    ExpRtn = CFE_SB_BAD_ARGUMENT;
    ActRtn = CFE_SB_PutDestinationBlk(NULL);

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn, exp=%ld, act=%ld", ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    ExpRtn = 0;
    ActRtn = UT_GetNumEventsSent();

    if (ActRtn != ExpRtn)
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "Unexpected rtn from UT_GetNumEventsSent, exp=%ld, act=%ld",
                 ExpRtn, ActRtn);
        UT_Text(cMsg);
        TestStat = CFE_FAIL;
    }

    UT_Report(TestStat, "CFE_SB_PutDestinationBlk",
              "PutDestBlk error test", "12.004");
} /* end Test_PutDestBlk_ErrLogic */
