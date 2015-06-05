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
**    tbl_UT.c
**
** Purpose:
**    Table Services unit test
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
** $Revision: 1.9 $
**
*/

/*
** Includes
*/
#include "tbl_UT.h"

/*
** External global variables
*/
extern uint32 UT_OS_Fail;
extern uint8  UT_CDS[UT_CDS_SIZE];
extern uint8  UT_Endianess;

extern UT_CDS_Map_t       UT_CDS_Map;
extern CFE_TBL_TaskData_t CFE_TBL_TaskData;

extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t FSWriteHdrRtn;
extern UT_SetRtn_t FSReadHdrRtn;
extern UT_SetRtn_t OSReadRtn;
extern UT_SetRtn_t OSReadRtn2;
extern UT_SetRtn_t SBSendMsgRtn;
extern UT_SetRtn_t FSSetTimestampRtn;
extern UT_SetRtn_t ES_DeleteCDSRtn;
extern UT_SetRtn_t ES_RegisterCDSRtn;
extern UT_SetRtn_t ES_CopyToCDSRtn;
extern UT_SetRtn_t ES_RestoreFromCDSRtn;
extern UT_SetRtn_t ES_RegisterRtn;
extern UT_SetRtn_t SB_CreatePipeRtn;
extern UT_SetRtn_t SB_SubscribeRtn;
extern UT_SetRtn_t EVS_SendEventRtn;
extern UT_SetRtn_t EVS_RegisterRtn;
extern UT_SetRtn_t GetAppIDRtn;
extern UT_SetRtn_t GetPoolRtn;
extern UT_SetRtn_t PutPoolRtn;
extern UT_SetRtn_t MutSemCreateRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t CreatePoolRtn;
extern UT_SetRtn_t SendMsgEventIDRtn;
extern UT_SetRtn_t SetMsgIdRtn;
extern UT_SetRtn_t ES_ExitAppRtn;

/*
** Global variables
*/
CFE_TBL_TaskData_t CFE_TBL_TaskData;
CFE_TBL_Handle_t App1TblHandle1;
CFE_TBL_Handle_t App1TblHandle2;
CFE_TBL_Handle_t App2TblHandle1;
CFE_TBL_Handle_t App2TblHandle2;
CFE_TBL_Handle_t ArrayOfHandles[2];

void *Tbl1Ptr = NULL;
void *Tbl2Ptr = NULL;
void **ArrayOfPtrsToTblPtrs[2];

CFE_TBL_RegistryRec_t Original[CFE_TBL_MAX_NUM_TABLES];
UT_SetRtn_t TBL_ValidationFuncRtn;

/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int tbl_main(void)
#endif
{
    /* Initialize unit test */
    UT_Init("tbl");
    UT_Text("cFE TBL Unit Test Output File\n\n");
    TBL_ValidationFuncRtn.count = 0;

    /* cfe_tbl_task.c functions */
    Test_CFE_TBL_TaskInit();
    Test_CFE_TBL_InitData();
    Test_CFE_TBL_SearchCmdHndlrTbl();

    /* cfe_tbl_task_cmds.c functions */
    /* This should be done first (it initializes working data structures) */
    Test_CFE_TBL_DeleteCDSCmd();
    Test_CFE_TBL_TlmRegCmd();
    Test_CFE_TBL_AbortLoadCmd();
    Test_CFE_TBL_ActivateCmd();
    Test_CFE_TBL_DumpToFile();
    Test_CFE_TBL_ResetCmd();
    Test_CFE_TBL_ValidateCmd();
    Test_CFE_TBL_NoopCmd();
    Test_CFE_TBL_GetTblRegData();
    Test_CFE_TBL_GetHkData();
    Test_CFE_TBL_DumpRegCmd();
    Test_CFE_TBL_DumpCmd();
    Test_CFE_TBL_LoadCmd();
    Test_CFE_TBL_HousekeepingCmd();

    /* cfe_tbl_api.c and cfe_tbl_internal.c functions */
    Test_CFE_TBL_ApiInit();
    Test_CFE_TBL_Register();
    Test_CFE_TBL_Share();
    Test_CFE_TBL_Unregister();
    Test_CFE_TBL_NotifyByMessage();
    Test_CFE_TBL_Load();
    Test_CFE_TBL_GetAddress();
    Test_CFE_TBL_ReleaseAddress();
    Test_CFE_TBL_GetAddresses();
    Test_CFE_TBL_ReleaseAddresses();
    Test_CFE_TBL_Validate();
    Test_CFE_TBL_Manage();
    Test_CFE_TBL_Update();
    Test_CFE_TBL_GetStatus();
    Test_CFE_TBL_GetInfo();
    Test_CFE_TBL_TblMod();

    /* Miscellaneous cfe_tbl_internal.c tests */
    Test_CFE_TBL_Internal();

    /* Final report on number of errors */
    UT_ReportFailures();
    return 0;
}

/*
** Saves the data inside the array passed in and sets the data to a controlled
** set of data
*/
void UT_SwapRegistryforControl(CFE_TBL_RegistryRec_t Registry[],
                               CFE_TBL_RegistryRec_t Desired[])
{
    int length = FindLength(Registry);
    int i, h;

    for (i = 0; i < length; i++)
    {
        strcpy(Original[i].Name, Registry[i].Name);
    }

    for (h = 0; h < length; h++)
    {
        strcpy(Registry[h].Name, Desired[h].Name);
    }
}

/*
** Returns the original array's values passed in by the
** UT_SwapRegistryforControl function
*/
void UT_ReturnOriginValues(CFE_TBL_RegistryRec_t Registry[])
{
    int j;
    int length = FindLength(Registry);

    for (j = 0; j < length; j++)
    {
        strcpy(Registry[j].Name, Original[j].Name);
    }
}

/*
** Finds the number of indexes in the array passed into function
*/
int32 FindLength(CFE_TBL_RegistryRec_t Registry[])
{
    int length = 0;

    if (sizeof(Registry) != 0 && sizeof(Registry[0]) != 0)
    {
        length = (int) (sizeof(Registry) / sizeof(Registry[0]));
    }

    return length;
}

/*
** Unit test specific call to process SB messages
*/
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
    CFE_TBL_TaskPipe(MsgPtr);
}

/*
** Initialize table registry values
*/
void UT_ResetTableRegistry(void)
{
    int32 i = 0;

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_InitRegistryRecord(&CFE_TBL_TaskData.Registry[i]);
    }

    /* Initialize the table access descriptors */
    for (i = 0; i < CFE_TBL_MAX_NUM_HANDLES; i++)
    {
        CFE_TBL_TaskData.Handles[i].AppId = CFE_ES_ERR_APPID;
        CFE_TBL_TaskData.Handles[i].RegIndex = 0;
        CFE_TBL_TaskData.Handles[i].PrevLink = CFE_TBL_END_OF_LIST;
        CFE_TBL_TaskData.Handles[i].NextLink = CFE_TBL_END_OF_LIST;
        CFE_TBL_TaskData.Handles[i].UsedFlag = FALSE;
        CFE_TBL_TaskData.Handles[i].LockFlag = FALSE;
        CFE_TBL_TaskData.Handles[i].Updated = FALSE;
        CFE_TBL_TaskData.Handles[i].BufferIndex = 0;
    }

    /* Initialize the table validation results records */
    for (i = 0; i < CFE_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        CFE_TBL_TaskData.ValidationResults[i].State = CFE_TBL_VALIDATION_FREE;
        CFE_TBL_TaskData.ValidationResults[i].CrcOfTable = 0;
        CFE_TBL_TaskData.ValidationResults[i].Result = 0;
        CFE_TBL_TaskData.ValidationResults[i].ActiveBuffer = FALSE;
        CFE_TBL_TaskData.ValidationResults[i].TableName[0] = '\0';
    }

    /* Initialize the dump-only table dump control blocks */
    for (i = 0; i < CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_TaskData.DumpControlBlocks[i].State = CFE_TBL_DUMP_FREE;
        CFE_TBL_TaskData.DumpControlBlocks[i].DumpBufferPtr = NULL;
        CFE_TBL_TaskData.DumpControlBlocks[i].Size = 0;
        CFE_TBL_TaskData.DumpControlBlocks[i].TableName[0] = '\0';

        /* Free all shared buffers */
        CFE_TBL_TaskData.LoadBuffs[i].Taken = FALSE;
    }

    CFE_TBL_TaskData.ValidationCtr = 0;
    CFE_TBL_TaskData.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    CFE_TBL_TaskData.LastTblUpdated = CFE_TBL_NOT_FOUND;
}

/*
** Tests to cover table task initialization functions
*/
void Test_CFE_TBL_TaskInit(void)
{
    CFE_SB_MsgPtr_t     MsgPtr;
    CFE_TBL_NoArgsCmd_t NoArgsCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Task Init\n");
#endif

    /* Test successful table services main entry point execution */
    UT_InitData();
    MsgPtr = (CFE_SB_MsgPtr_t) &NoArgsCmd;
    CFE_TBL_TaskMain();
    UT_Report(ES_ExitAppRtn.value == CFE_ES_CORE_APP_RUNTIME_ERROR &&
              ES_ExitAppRtn.count == 1,
              "CFE_TBL_TaskMain",
              "Success",
              "01.001");

    /* Test table services main entry point execution with a register
     * application failure
     */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterRtn, -1, 1);
    CFE_TBL_TaskMain();
    UT_Report(ES_ExitAppRtn.value == CFE_ES_CORE_APP_INIT_ERROR +
                                     CFE_ES_CORE_APP_RUNTIME_ERROR &&
              ES_ExitAppRtn.count == 2,
              "CFE_TBL_TaskMain",
              "Application register fail",
              "01.002");

    /* Test successful table services core application initialization */
    UT_InitData();
    UT_Report(CFE_TBL_TaskInit() == CFE_SUCCESS,
              "CFE_TBL_TaskInit",
              "Success",
              "01.003");

    /* Test table services core application initialization response to a pipe
     * creation failure
     */
    UT_InitData();
    UT_SetRtnCode(&SB_CreatePipeRtn, -2, 1);
    UT_Report(CFE_TBL_TaskInit() == -2,
              "CFE_TBL_TaskInit",
              "Create pipe fail",
              "01.004");

    /* Test table services core application initialization response to a
     * housekeeping request subscription error
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeRtn, -3, 1);
    UT_Report(CFE_TBL_TaskInit() == -3,
              "CFE_TBL_TaskInit",
              "Housekeeping request subscribe fail",
              "01.005");

    /* Test table services core application initialization response to a
     * ground command subscription error
     */
    UT_InitData();
    UT_SetRtnCode(&SB_SubscribeRtn, -4, 2);
    UT_Report(CFE_TBL_TaskInit() == -4,
              "CFE_TBL_TaskInit",
              "Ground command subscribe fail",
              "01.006");

    /* Test table services core application initialization response to a
     * send initialization event error
     */
    UT_InitData();
    UT_SetRtnCode(&EVS_SendEventRtn, -5, 1);
    UT_Report(CFE_TBL_TaskInit() == -5,
              "CFE_TBL_TaskInit",
              "Send initialization event fail",
              "01.007");

    /* Test table services core application initialization response to an
     * EVS register failure
     */
    UT_InitData();
    UT_SetRtnCode(&EVS_RegisterRtn, -6, 1);
    UT_Report(CFE_TBL_TaskInit() == -6,
              "CFE_TBL_TaskInit",
              "EVS register fail",
              "01.008");

    /* Test command pipe messages handler response to a valid command */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_TBL_NoArgsCmd_t));
    CFE_SB_SetMsgId(MsgPtr, CFE_TBL_CMD_MID);
    CFE_SB_SetCmdCode(MsgPtr, CFE_TBL_NOOP_CC);
    CFE_TBL_TaskPipe(MsgPtr);
    UT_Report(SendMsgEventIDRtn.value == CFE_TBL_NOOP_INF_EID,
              "CFE_TBL_TaskPipe",
              "Valid command (no-op) - success",
              "01.009");

    /* Test command pipe messages handler response to an invalid
     * message length
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_TBL_NoArgsCmd_t ) - 1);
    CFE_SB_SetMsgId(MsgPtr, CFE_TBL_CMD_MID);
    CFE_SB_SetCmdCode(MsgPtr, CFE_TBL_NOOP_CC);
    CFE_TBL_TaskPipe(MsgPtr);
    UT_Report(SendMsgEventIDRtn.value == CFE_TBL_LEN_ERR_EID,
              "CFE_TBL_TaskPipe",
              "Invalid message length",
              "01.010");

    /* Test command pipe messages handler response to an invalid
     * command code
     */
    UT_InitData();
    UT_SetSBTotalMsgLen(sizeof(CFE_TBL_NoArgsCmd_t ));
    CFE_SB_SetMsgId(MsgPtr, CFE_TBL_CMD_MID);
    CFE_SB_SetCmdCode(MsgPtr, 999);
    CFE_TBL_TaskPipe(MsgPtr);
    UT_Report(SendMsgEventIDRtn.value == CFE_TBL_CC1_ERR_EID,
              "CFE_TBL_TaskPipe",
              "Invalid command code",
              "01.011");
}

/*
** Test table service application data initialization
*/
void Test_CFE_TBL_InitData(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Init Data\n");
#endif

    /* This function has only one possible path with no return code */
    UT_InitData();
    CFE_TBL_InitData();
    UT_Report(SetMsgIdRtn.value == CFE_TBL_REG_TLM_MID &&
              SetMsgIdRtn.count == 2,
              "CFE_TBL_SearchCmdHndlrTbl",
              "Initialize data",
              "02.001");
}

/*
** Test command handler table message ID (or command code) search function
*/
void Test_CFE_TBL_SearchCmdHndlrTbl(void)
{
    int16          TblIndex = 1;
    uint16         CmdCode;
    CFE_SB_MsgId_t MsgID;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Search Command Handler Table\n");
#endif

    /* Test successfully finding a matching message ID and command code */
    UT_InitData();
    MsgID = CFE_TBL_CMD_MID;
    CmdCode = CFE_TBL_NOOP_CC;
    UT_Report(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex,
              "CFE_TBL_SearchCmdHndlrTbl",
              "Found matching message ID and command code",
              "03.001");

    /* Test using a message that is not a command message with specific
     * command code
     */
    UT_InitData();
    TblIndex = 0;
    MsgID = CFE_TBL_SEND_HK_MID;
    UT_Report(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex,
              "CFE_TBL_SearchCmdHndlrTbl",
              "Message is not a command message with specific command code",
              "03.002");

    /* Test with a message ID that matches but the command code does
     * not match
     */
    UT_InitData();
    TblIndex = CFE_TBL_BAD_CMD_CODE;
    MsgID = CFE_TBL_CMD_MID;
    CmdCode = 0xffff;
    UT_Report(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex,
              "CFE_TBL_SearchCmdHndlrTbl",
              "Message ID matches, command code must does not match",
              "03.003");

    /* Test with a message ID that does not match */
    UT_InitData();
    TblIndex = CFE_TBL_BAD_MSG_ID;
    MsgID = 0xffff;
    UT_Report(CFE_TBL_SearchCmdHndlrTbl(MsgID, CmdCode) == TblIndex,
              "CFE_TBL_SearchCmdHndlrTbl",
              "Message ID does not match",
              "03.004");
}

/*
** Test the delete critical table's CDS command message
*/
void Test_CFE_TBL_DeleteCDSCmd(void)
{
    int                 i, j, k;
    CFE_TBL_DelCDSCmd_t DelCDSCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Delete CDS Command\n");
#endif

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(CFE_TBL_TaskData.Registry[i].Name,
                 CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    /* Test successfully finding the table name in the table registry */
    UT_InitData();
    strncpy(DelCDSCmd.TableName, "0", CFE_TBL_MAX_FULL_NAME_LEN);
    DelCDSCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Table name found in table registry",
              "04.001");

    /* Test failure to find table in the critical table registry */
    UT_InitData();
    k = CFE_TBL_MAX_CRITICAL_TABLES + CFE_TBL_MAX_NUM_TABLES;

    for (j = CFE_TBL_MAX_NUM_TABLES; j < k; j++)
    {
        SNPRINTF(CFE_TBL_TaskData.CritReg[j - CFE_TBL_MAX_NUM_TABLES].Name,
                 CFE_TBL_MAX_FULL_NAME_LEN, "%d", j);
    }

    strncpy(DelCDSCmd.TableName, "-1", CFE_TBL_MAX_FULL_NAME_LEN);
    DelCDSCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Table not found in critical table registry",
              "04.002");

    /* Test finding the table in the critical table registry, but CDS is not
     * tagged as a table
     */
    UT_InitData();
    SNPRINTF(DelCDSCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN, "%d",
             CFE_TBL_MAX_CRITICAL_TABLES + CFE_TBL_MAX_NUM_TABLES - 1);
    UT_SetRtnCode(&ES_DeleteCDSRtn, CFE_ES_CDS_WRONG_TYPE_ERR, 1);
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Table is in critical table registry but CDS is not tagged "
                 "as a table",
              "04.003");

    /* Test deletion when CDS owning application is still active */
    UT_InitData();
    UT_SetRtnCode(&ES_DeleteCDSRtn, CFE_ES_CDS_OWNER_ACTIVE_ERR, 1);
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "CDS owning application is still active",
              "04.004");

    /* Test deletion where the table cannot be located in the CDS registry */
    UT_InitData();
    UT_SetRtnCode(&ES_DeleteCDSRtn, CFE_ES_CDS_NOT_FOUND_ERR, 1);
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Unable to locate table in CDS registry",
              "04.005");

    /* Test deletion error while deleting table from the CDS */
    UT_InitData();
    UT_SetRtnCode(&ES_DeleteCDSRtn, CFE_SUCCESS - 1, 1);
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Error while deleting table from CDS",
              "04.006");

    /* Test successful removal of the table from the CDS */
    UT_InitData();
    UT_SetRtnCode(&ES_DeleteCDSRtn, CFE_SUCCESS, 1);
    UT_Report(CFE_TBL_DeleteCDSCmd((CFE_SB_Msg_t *) &DelCDSCmd) ==
                                       CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DeleteCDSCmd",
              "Successfully removed table from CDS",
              "04.007");
}

/*
** Test the processing telemetry table registry command message function
*/
void Test_CFE_TBL_TlmRegCmd(void)
{
    int                   i;
    CFE_TBL_RegistryRec_t Desired[CFE_TBL_MAX_NUM_TABLES];
    CFE_TBL_TlmRegCmd_t   TlmRegCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Telemetry Registry Command\n");
#endif

    /* Test when table name does exist */
    UT_InitData();

    /* Registry[0].Name used because it is confirmed to be a registered
     * table name
     */
    strncpy(TlmRegCmd.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TlmRegCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_Report(CFE_TBL_TlmRegCmd((CFE_SB_Msg_t *) &TlmRegCmd) ==
                                    CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_TlmRegCmd",
              "Table registry entry for telemetry does exist",
              "05.001");

    /* Test when table name does not exist */
    UT_InitData();

    /* Will use swap function to make the whole tbl registry filled with
     * known tbl names in order to test when a table name is not
     * registered
     */
    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(Desired[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    UT_SwapRegistryforControl(CFE_TBL_TaskData.Registry, Desired);
    SNPRINTF(TlmRegCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN,
             "%d", CFE_TBL_MAX_NUM_TABLES + 1);
    UT_Report(CFE_TBL_TlmRegCmd((CFE_SB_Msg_t *) &TlmRegCmd) ==
                                    CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_TlmRegCmd",
              "Table registry entry for telemetry doesn't exist",
              "05.002");

    /* Returning original values */
    UT_ReturnOriginValues(CFE_TBL_TaskData.Registry);
}

/*
** Test the processing abort load command message function
*/
void Test_CFE_TBL_AbortLoadCmd(void)
{
    int i;
    int load = (int) CFE_TBL_TaskData.Registry[0].LoadInProgress;
    CFE_TBL_AbortLdCmd_t  AbortLdCmd;
    CFE_TBL_RegistryRec_t Desired[CFE_TBL_MAX_NUM_TABLES];

#ifdef UT_VERBOSE
    UT_Text("Begin Test Abort Load Command\n");
#endif

    /* Test when table name does exist and a table load is in progress */
    UT_InitData();

    /* Entering the if statement with table name that has to be in
     * the registry
     */
    strncpy(AbortLdCmd.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    AbortLdCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    CFE_TBL_TaskData.Registry[0].LoadInProgress = 1;
    UT_Report(CFE_TBL_AbortLoadCmd((CFE_SB_Msg_t *) &AbortLdCmd) ==
                                       CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_AbortLoadCmd",
              "Table registry entry exists & load in progress",
              "06.001");

    /* Test when table name does exist but no table load is in progress */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(CFE_TBL_AbortLoadCmd((CFE_SB_Msg_t *) &AbortLdCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_AbortLoadCmd",
              "Table registry entry exists but no load in progress",
              "06.002");

    /* Test when table name not found in the registry */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].LoadInProgress = load;

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(Desired[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    UT_SwapRegistryforControl(CFE_TBL_TaskData.Registry, Desired);
    SNPRINTF(AbortLdCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN,
             "%d", CFE_TBL_MAX_NUM_TABLES + 1);
    UT_Report(CFE_TBL_AbortLoadCmd((CFE_SB_Msg_t *) &AbortLdCmd) ==
                                       CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_AbortLoadCmd",
              "Table registry entry doesn't exist",
              "06.003");

    /* Returning original values */
    UT_ReturnOriginValues(CFE_TBL_TaskData.Registry);
}

/*
** Test the activate table command message function
*/
void Test_CFE_TBL_ActivateCmd(void)
{
    int   i;
    int   load = (int) CFE_TBL_TaskData.Registry[0].LoadInProgress;
    uint8 dump = CFE_TBL_TaskData.Registry[0].DumpOnly;
    CFE_TBL_RegistryRec_t Desired[CFE_TBL_MAX_NUM_TABLES];
    CFE_TBL_ActivateCmd_t ActivateCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Activate Command\n");
#endif

    /* Entering the if statement with a table name that has to be in
     * the registry
     */
    strncpy(ActivateCmd.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    ActivateCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';

    /* Test when table name exists, but attempts to activate a dump-only
     * table
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].DumpOnly = TRUE;
    UT_Report(CFE_TBL_ActivateCmd((CFE_SB_Msg_t *) &ActivateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ActivateCmd",
              "Table registry exists, but dump-only table attempted to load",
              "07.001");

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, and the table is double-buffered
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].DumpOnly = FALSE;
    CFE_TBL_TaskData.Registry[0].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_TaskData.Registry[0].DblBuffered = TRUE;
    UT_Report(CFE_TBL_ActivateCmd((CFE_SB_Msg_t *) &ActivateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, and a load in "
                 "progress: Table is double-buffered",
              "07.002");

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, the table isn't double-buffered, and ValidationStatus = TRUE
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].DblBuffered = FALSE;
    CFE_TBL_TaskData.
      LoadBuffs[CFE_TBL_TaskData.Registry[0].LoadInProgress].
        Validated = TRUE;
    UT_Report(CFE_TBL_ActivateCmd((CFE_SB_Msg_t *) &ActivateCmd) ==
                                      CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, and a load in "
                "progress: Table isn't double-buffered",
              "07.003");

    /* Test when table name exists, the table is not a dump-only, and no load
     * is in progress
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(CFE_TBL_ActivateCmd((CFE_SB_Msg_t *) &ActivateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ActivateCmd",
              "Table registry exists, not a dump-only tbl, no load in "
                "progress",
              "07.004");

    /* Test when the table name doesn't exist */
    UT_InitData();

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(Desired[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    UT_SwapRegistryforControl(CFE_TBL_TaskData.Registry, Desired);
    SNPRINTF(ActivateCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN,
             "%d", CFE_TBL_MAX_NUM_TABLES + 1);
    UT_Report(CFE_TBL_ActivateCmd((CFE_SB_Msg_t *) &ActivateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ActivateCmd",
              "Table registry entry doesn't exist",
              "07.005");

    /* Returning original values */
    UT_ReturnOriginValues(CFE_TBL_TaskData.Registry);
    CFE_TBL_TaskData.Registry[0].LoadInProgress = load;
    CFE_TBL_TaskData.Registry[0].DumpOnly = dump;
}

/*
** Test the write table data to a file function
*/
void Test_CFE_TBL_DumpToFile(void)
{
    uint32 TblSizeInBytes = 9;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Dump to File\n");
#endif

    /* Test with an error creating the dump file */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile",
              "Error creating dump file",
              "08.001");

    /* Test with an error writing the cFE file header */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t) - 1, 1);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile",
              "Error writing cFE file header",
              "08.002");

    /* Test with an error writing the table file header */
    UT_InitData();

    /* Set the count for the FSWriteHdrRtn return code variable to a large
     * enough value to pass through every time
     */
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t), 6);
    UT_SetRtnCode(&FileWriteRtn, sizeof(CFE_TBL_File_Hdr_t) - 1, 1);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile",
              "Error writing cFE file header",
              "08.003");

    /* Test with an error writing the table to a file */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, TblSizeInBytes - 1, 2);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpToFile",
              "Error writing cFE file header",
              "08.004");

    /* Test successful file creation and data dumped */
    UT_InitData();
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpToFile",
              "File created and data dumped",
              "08.005");

    /* Test where file already exists so data isoverwritten */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    UT_Report(CFE_TBL_DumpToFile("filename" ,"tablename" ,"dumpaddress",
                                 TblSizeInBytes) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpToFile",
              "File existed previously => data overwritten",
              "08.006");
}

/*
** Test the processing reset counters command message function
*/
void Test_CFE_TBL_ResetCmd(void)
{
    CFE_SB_Msg_t MessagePtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Reset Command\n");
#endif

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UT_Report(CFE_TBL_ResetCmd(&MessagePtr) == CFE_TBL_DONT_INC_CTR,
              "CFE_TBL_ResetCmd",
              "Function run and completed",
              "09.001");
}

/*
** Test the validate table command message function
*/
void Test_CFE_TBL_ValidateCmd(void)
{
    int                       i, k;
    uint8                     Buff;
    uint8                     *BuffPtr = &Buff;
    CFE_TBL_RegistryRec_t     Desired[CFE_TBL_MAX_NUM_TABLES];
    CFE_TBL_ValidateCmd_t     ValidateCmd;
    CFE_TBL_CallbackFuncPtr_t ValFuncPtr = (CFE_TBL_CallbackFuncPtr_t)
                                             ((unsigned long )
                                               &UT_SwapRegistryforControl);

#ifdef UT_VERBOSE
    UT_Text("Begin Test Validate Command\n");
#endif

    /* Test when table name is not found in the registry */
    UT_InitData();

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(Desired[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    UT_SwapRegistryforControl(CFE_TBL_TaskData.Registry, Desired);
    SNPRINTF(ValidateCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN,
             "%d", CFE_TBL_MAX_NUM_TABLES + 1);
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ValidateCmd",
              "Table registry entry doesn't exist",
              "10.001");
    UT_ReturnOriginValues(CFE_TBL_TaskData.Registry);

    /* Test where the active buffer has data, but too many table validations
     * have been requested
     */
    UT_InitData();
    strncpy(ValidateCmd.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    ValidateCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    ValidateCmd.ActiveTblFlag = CFE_TBL_ACTIVE_BUFFER;
    CFE_TBL_TaskData.Registry[0].
      Buffers[CFE_TBL_TaskData.Registry[0].ActiveBufferIndex].
        BufferPtr = BuffPtr;

    for (k = 0; k < CFE_TBL_MAX_NUM_VALIDATIONS; k++)
    {
        CFE_TBL_TaskData.ValidationResults[k].State =
            CFE_TBL_VALIDATION_PENDING;
    }

    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ValidateCmd",
              "Active buffer with data: too many table validations have "
                "been requested",
              "10.002");

    /* Test where the active buffer has data, but there is no validation
     * function pointer
     */
    UT_InitData();
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_TaskData.Registry[0].ValidationFuncPtr = NULL;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                  CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_ValidateCmd",
              "Active buffer with data: No validation function pointer",
              "10.003");

    /* Test where the active buffer has data, the validation function pointer
     * exists, and the active table flag is set
     */
    UT_InitData();
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_TaskData.Registry[0].ValidationFuncPtr = ValFuncPtr;
    ValidateCmd.ActiveTblFlag = TRUE;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                 CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_ValidateCmd",
              "Active buffer with data: validation function pointer and "
                "active table flag",
              "10.004");

    /* Test with the buffer inactive, the table is double-buffered, and the
     * validation function pointer exists
     */
    UT_InitData();
    ValidateCmd.ActiveTblFlag = CFE_TBL_INACTIVE_BUFFER;
    CFE_TBL_TaskData.Registry[0].DblBuffered = TRUE;
    CFE_TBL_TaskData.Registry[0].
      Buffers[1 - CFE_TBL_TaskData.Registry[0].ActiveBufferIndex].
        BufferPtr = BuffPtr;
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_TaskData.Registry[0].ValidationFuncPtr = ValFuncPtr;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                      CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_ValidateCmd",
              "Inactive buffer: double buffered table : validation "
                "function pointer",
              "10.005");

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, and the validation function pointer exists
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].DblBuffered = FALSE;
    CFE_TBL_TaskData.
      LoadBuffs[CFE_TBL_TaskData.Registry[0].LoadInProgress].
        BufferPtr = BuffPtr;
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_FREE;
    CFE_TBL_TaskData.Registry[0].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                  CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_ValidateCmd",
              "Inactive buffer: single buffered table with load in progress: "
                "validation function pointer",
              "10.006");

    /* Test where no inactive buffer is present (single-buffered table without
     * load in progress)
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ValidateCmd",
              "Inactive buffer: single buffered table with load in progress",
              "10.007");

    /* Test with an illegal buffer */
    UT_InitData();
    ValidateCmd.ActiveTblFlag = 0xffff;
    UT_Report(CFE_TBL_ValidateCmd((CFE_SB_Msg_t *) &ValidateCmd) ==
                                      CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_ValidateCmd",
              "Illegal buffer",
              "10.008");
}

/*
** Test the processing no-operation command message function
*/
void Test_CFE_TBL_NoopCmd(void)
{
    CFE_SB_Msg_t MessagePtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test No-Op Command\n");
#endif

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UT_Report(CFE_TBL_NoopCmd(&MessagePtr) == CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_NoopCmd",
              "Function run and completed",
              "11.001");
}

/*
** Test the function which converts table registry entries for tables into
** messages
*/
void Test_CFE_TBL_GetTblRegData(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Table Registry Command\n");
#endif

    /* Test using a double buffered table */
    UT_InitData();
    CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr = '\0';
    CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.HkTlmTblRegIndex].DblBuffered =
        TRUE;
    CFE_TBL_GetTblRegData();
    UT_Report(CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr != '\0',
              "CFE_TBL_GetTblRegData",
              "Double buffered table",
              "12.001");

    /* Test using a single buffered table and the buffer is inactive */
    UT_InitData();
    CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr = '\0';
    CFE_TBL_TaskData.Registry[CFE_TBL_TaskData.HkTlmTblRegIndex].DblBuffered =
        FALSE;
    CFE_TBL_TaskData.
      Registry[CFE_TBL_TaskData.HkTlmTblRegIndex].
        LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_GetTblRegData();
    UT_Report(CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr != '\0',
              "CFE_TBL_GetTblRegData",
              "Single buffered table - inactive buffer",
              "12.002");

    /* Test with no inactive buffer */
    UT_InitData();
    CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr = '\0';
    CFE_TBL_TaskData.
      Registry[CFE_TBL_TaskData.HkTlmTblRegIndex].LoadInProgress =
          CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_GetTblRegData();
    UT_Report(CFE_TBL_TaskData.TblRegPacket.InactiveBufferAddr == '\0',
              "CFE_TBL_GetTblRegData",
              "No inactive buffer",
              "12.003");
}

/*
** Test the function that collects data and stores it in the housekeeping
** message
*/
void Test_CFE_TBL_GetHkData(void)
{
    int   i;
    int32 NumLoadPendingIndex = CFE_TBL_MAX_NUM_TABLES - 1;
    int32 FreeSharedBuffIndex = CFE_TBL_MAX_SIMULTANEOUS_LOADS - 1;
    int32 ValTableIndex = CFE_TBL_MAX_NUM_VALIDATIONS - 1;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Housekeeping Data\n");
#endif

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_TaskData.Registry[i].LoadPending = FALSE;
    }

    /* Test raising the count of load pending tables */
    UT_InitData();
    CFE_TBL_TaskData.Registry[NumLoadPendingIndex].LoadPending = TRUE;
    CFE_TBL_GetHkData();
    UT_Report(CFE_TBL_TaskData.HkPacket.NumLoadPending == 1,
              "CFE_TBL_GetHkData",
              "Raise load pending table count",
              "13.001");

    /* Test lowering the count of free shared buffers */
    UT_InitData();
    CFE_TBL_TaskData.LoadBuffs[FreeSharedBuffIndex].Taken = TRUE;
    CFE_TBL_GetHkData();
    UT_Report(CFE_TBL_TaskData.HkPacket.NumFreeSharedBufs ==
                  CFE_TBL_MAX_SIMULTANEOUS_LOADS - 1,
              "CFE_TBL_GetHkData",
              "Lower free shared buffer count",
              "13.002");

    /* Test making a ValPtr with result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_TaskData.SuccessValCtr = 0;
    CFE_TBL_TaskData.ValidationResults[ValTableIndex].State =
        CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_TaskData.ValidationResults[ValTableIndex].Result = CFE_SUCCESS;
    CFE_TBL_GetHkData();
    UT_Report(CFE_TBL_TaskData.SuccessValCtr == 1,
              "CFE_TBL_GetHkData",
              "ValPtr result CFE_SUCCESS",
              "13.003");

    /* Test making a ValPtr without result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_TaskData.FailedValCtr = 0;
    CFE_TBL_TaskData.ValidationResults[ValTableIndex].State =
        CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_TaskData.ValidationResults[ValTableIndex].Result = CFE_SUCCESS - 1;
    CFE_TBL_GetHkData();
    UT_Report(CFE_TBL_TaskData.FailedValCtr == 1,
              "CFE_TBL_GetHkData",
              "ValPtr result != CFE_SUCCESS",
              "13.004");
}

/*
** Test the function that processes dump table registration to file
** command message
*/
void Test_CFE_TBL_DumpRegCmd(void)
{
    int                  q;
    CFE_TBL_DumpRegCmd_t DumpRegCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Dump Register Command\n");
#endif

    for (q = 0; q < CFE_TBL_MAX_NUM_TABLES; q++)
    {
        CFE_TBL_TaskData.Registry[q].HeadOfAccessList = CFE_TBL_END_OF_LIST;
    }

    /* Test with an error creating the dump file */
    UT_InitData();
    DumpRegCmd.DumpFilename[0] = '\0';
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(CFE_TBL_DumpRegCmd((CFE_SB_Msg_t *) &DumpRegCmd) ==
                                     CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegCmd",
              "Error creating dump file",
              "14.001");

    /* Test with an error writing the cFE File header */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t) - 1, 1);
    UT_Report(CFE_TBL_DumpRegCmd((CFE_SB_Msg_t *) &DumpRegCmd) ==
                                     CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegCmd",
              "Error writing cFE File header",
              "14.002");

    /* Test where the table is owned, the file doesn't already exist, and the
     * table is successfully dumped
     */
    UT_InitData();
    UT_SetRtnCode(&FSWriteHdrRtn, sizeof(CFE_FS_Header_t), 10);
    CFE_TBL_TaskData.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED + 1;
    CFE_TBL_TaskData.Registry[0].HeadOfAccessList = CFE_TBL_END_OF_LIST;
    CFE_TBL_TaskData.Registry[1].OwnerAppId = CFE_TBL_NOT_OWNED;
    CFE_TBL_TaskData.Registry[0].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_TaskData.Registry[0].DblBuffered = TRUE;
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(CFE_TBL_DumpRegCmd((CFE_SB_Msg_t *) &DumpRegCmd) ==
                                     CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpRegCmd",
              "Table is owned, file didn't exist previously: successfully "
                "dumped table",
              "14.003");

    /* Test where the file did exist previously and the table is successfully
     * overwritten
     */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    UT_Report(CFE_TBL_DumpRegCmd((CFE_SB_Msg_t *) &DumpRegCmd) ==
                  CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpRegCmd",
              "File did exist previously: successfully overwritten table",
              "14.004");

    /* Test where the table is not owned, the OS write fails, resulting in an
     * error writing to the registry
     */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, sizeof(CFE_TBL_RegDumpRec_t) - 1, 1);
    CFE_TBL_TaskData.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED;
    CFE_TBL_TaskData.Registry[0].HeadOfAccessList = 2;
    CFE_TBL_TaskData.Handles[2].NextLink = CFE_TBL_END_OF_LIST;
    UT_Report(CFE_TBL_DumpRegCmd((CFE_SB_Msg_t *) &DumpRegCmd) ==
                                     CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpRegCmd",
              "Table is not owned, OS_write fails: Error writing Registry",
              "14.005");
}

/*
** Test the function that processes dump table to file command message
*/
void Test_CFE_TBL_DumpCmd(void)
{
    int                   i, k, u;
    uint8                 Buff;
    uint8                 *BuffPtr = &Buff;
    CFE_TBL_RegistryRec_t Desired[CFE_TBL_MAX_NUM_TABLES];
    CFE_TBL_LoadBuff_t    Load = {0};
    CFE_TBL_DumpCmd_t     DumpCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Dump Command\n");
#endif

    /* Test where the table cannot be found in the registry */
    UT_InitData();

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        SNPRINTF(Desired[i].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", i);
    }

    UT_SwapRegistryforControl(CFE_TBL_TaskData.Registry, Desired);
    SNPRINTF(DumpCmd.TableName, CFE_TBL_MAX_FULL_NAME_LEN,
             "%d", CFE_TBL_MAX_NUM_TABLES + 1);
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Table registry entry doesn't exist",
              "15.001");
    UT_ReturnOriginValues(CFE_TBL_TaskData.Registry);

    /* Test with an active buffer, the pointer is created, validation passes,
     * the table is dump only, no dump is already in progress, and have a
     * working buffer; load in progress, single-buffered
     */
    UT_InitData();
    strncpy(CFE_TBL_TaskData.Registry[2].Name, "DumpCmdTest",
            CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.Registry[2].Name[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    strncpy(DumpCmd.TableName, CFE_TBL_TaskData.Registry[2].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    DumpCmd.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    DumpCmd.ActiveTblFlag = CFE_TBL_ACTIVE_BUFFER;
    CFE_TBL_TaskData.
      Registry[2].
        Buffers[CFE_TBL_TaskData.Registry[2].ActiveBufferIndex].
          BufferPtr = BuffPtr;

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_TaskData.Registry[i].DumpOnly = TRUE;
    }

    CFE_TBL_TaskData.DumpControlBlocks[2].State = CFE_TBL_DUMP_PENDING;
    CFE_TBL_TaskData.DumpControlBlocks[3].State = CFE_TBL_DUMP_FREE;
    CFE_TBL_TaskData.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_TaskData.Registry[2].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_TaskData.Registry[2].DblBuffered = FALSE;
    CFE_TBL_TaskData.LoadBuffs[CFE_TBL_TaskData.Registry[2].LoadInProgress] =
        Load;
    CFE_TBL_TaskData.Registry[2].NotifyByMsg = TRUE;
    UT_SetRtnCode(&SBSendMsgRtn, CFE_SB_INTERNAL_ERR, 1);
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                              CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, validation passes, is a dump "
                "only table, no dump already in progress, got working buffer",
              "15.002");

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, no dump is already progress, and fails to get a working buffer;
     * no load in progress, TableLoadedOnce = TRUE, single-buffered, no buffer
     * available
     */
    UT_InitData();
    CFE_TBL_TaskData.DumpControlBlocks[2].State = CFE_TBL_DUMP_FREE;
    CFE_TBL_TaskData.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;
    CFE_TBL_TaskData.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_TaskData.Registry[2].TableLoadedOnce = TRUE;
    CFE_TBL_TaskData.Registry[2].DblBuffered = FALSE;

    for (u = 0; u < CFE_TBL_MAX_SIMULTANEOUS_LOADS; u++)
    {
        CFE_TBL_TaskData.LoadBuffs[u].Taken = TRUE;
    }

    CFE_TBL_TaskData.Registry[2].NotifyByMsg = TRUE;
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, is a dump only table, no dump "
                "already in progress, fails to get a working buffer: No "
                "working buffers available",
              "15.003");

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, and no dump fails to find a free dump control block; too many
     * dump only table dumps have been requested
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;

    for (k = 0; k < CFE_TBL_MAX_SIMULTANEOUS_LOADS; k++)
    {
        CFE_TBL_TaskData.DumpControlBlocks[k].State = CFE_TBL_DUMP_PENDING;
    }

    CFE_TBL_TaskData.Registry[2].NotifyByMsg = TRUE;
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Active buffer, pointer created, is dump only table, fails to "
                "find a free dump control block: too many dump only table "
                "dumps have been requested",
              "15.004");

    /* Test with an inactive buffer, double-buffered, dump already in progress;
     * dump is already pending
     */
    UT_InitData();
    DumpCmd.ActiveTblFlag = CFE_TBL_INACTIVE_BUFFER;
    CFE_TBL_TaskData.Registry[2].DblBuffered = TRUE;
    CFE_TBL_TaskData.
      Registry[2].
        Buffers[(1 - CFE_TBL_TaskData.Registry[2].ActiveBufferIndex)].
          BufferPtr = BuffPtr;
    CFE_TBL_TaskData.Registry[2].DumpControlIndex = CFE_TBL_NO_DUMP_PENDING +
                                                    1;
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Inactive buffer, double-buffered, dump already in progress: "
                "dump is already pending",
              "15.005");

    /* Test with an inactive buffer, single-buffered, pointer created, is a
     * dump only table
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[2].DblBuffered = FALSE;
    CFE_TBL_TaskData.Registry[2].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_TaskData.
      LoadBuffs[CFE_TBL_TaskData.Registry[2].LoadInProgress].
        BufferPtr = BuffPtr;
    CFE_TBL_TaskData.Registry[2].DumpOnly = FALSE;
    strncpy(DumpCmd.DumpFilename, CFE_TBL_TaskData.Registry[2].LastFileLoaded,
            OS_MAX_PATH_LEN);
    DumpCmd.DumpFilename[OS_MAX_PATH_LEN - 1] = '\0';
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_DumpCmd",
              "Inactive buffer, single-buffered, pointer created, is a dump "
                "only table",
              "15.006");

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[2].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Inactive buffer, single-buffered: no inactive buffer for table",
              "15.007");

    /* Test with an illegal buffer parameter */
    UT_InitData();
    DumpCmd.ActiveTblFlag = CFE_TBL_ACTIVE_BUFFER + 1;
    UT_Report(CFE_TBL_DumpCmd((CFE_SB_Msg_t *) &DumpCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_DumpCmd",
              "Illegal buffer parameter",
              "15.008");
}

/*
** Test the function that processes load table file to buffer command
** message
*/
void Test_CFE_TBL_LoadCmd(void)
{
    int                i, j;
    CFE_TBL_File_Hdr_t TblFileHeader;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_LoadBuff_t BufferPtr = CFE_TBL_TaskData.LoadBuffs[0];
    CFE_TBL_LoadCmd_t  LoadCmd;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Load Command\n");
#endif

    /* Test response to inability to open file */
    UT_InitData();
    strncpy(LoadCmd.LoadFilename, "LoadFileName",
            OS_MAX_PATH_LEN);
    LoadCmd.LoadFilename[OS_MAX_PATH_LEN - 1] = '\0';
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Unable to open file",
              "16.001");

    /* Test response to inability to find the table in the registry */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);

    for (i = 0; i < CFE_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_TaskData.Registry[i].OwnerAppId = CFE_TBL_NOT_OWNED;
        CFE_TBL_TaskData.Registry[i].LoadPending = FALSE;
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    strncpy(StdFileHeader.Description, "FS header description",
            CFE_FS_HDR_DESC_MAX_LEN);
    StdFileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Table registry entry doesn't exist",
              "16.002");

    /* Test attempt to load a dump only table */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED + 1;
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    CFE_TBL_TaskData.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t) + 1;
    CFE_TBL_TaskData.Registry[0].DumpOnly = TRUE;
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Attempting to load a dump only table",
              "16.003");

    /* Test attempt to load a table with a load already pending */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED + 1;
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    CFE_TBL_TaskData.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t) + 1;
    CFE_TBL_TaskData.Registry[0].DumpOnly = FALSE;
    CFE_TBL_TaskData.Registry[0].LoadPending = TRUE;
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Attempting to load a table with load already pending",
              "16.004");
    CFE_TBL_TaskData.Registry[0].LoadPending = FALSE;

    /* Test where the file isn't dump only and passes table checks, get a
     * working buffer, and there is an extra byte (more data than header
     * indicates)
     */
    UT_InitData();
    TblFileHeader.Offset = 0;
    CFE_TBL_TaskData.Registry[0].TableLoadedOnce = TRUE;
    TblFileHeader.NumBytes = sizeof(CFE_TBL_File_Hdr_t);

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    CFE_TBL_TaskData.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t);
    CFE_TBL_TaskData.Registry[0].LoadInProgress =
        CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    CFE_TBL_TaskData.Registry[0].DblBuffered = FALSE;
    CFE_TBL_TaskData.
      LoadBuffs[CFE_TBL_TaskData.Registry[0].LoadInProgress].
        BufferPtr = (uint8 *) &BufferPtr;
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    CFE_TBL_TaskData.Registry[0].DumpOnly = FALSE;
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "There is more data than the file indicates",
              "16.005");

    /* Test with no extra byte => successful load */
    UT_InitData();
    TblFileHeader.NumBytes = sizeof(CFE_TBL_File_Hdr_t);

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    UT_SetRtnCode(&OSReadRtn, 0, 3);
    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_CMD_CTR,
              "CFE_TBL_LoadCmd",
              "Successful load",
              "16.006");

    /* Test with differing amount of data from header's claim */
    UT_InitData();
    TblFileHeader.NumBytes = sizeof(CFE_TBL_File_Hdr_t);

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetRtnCode(&OSReadRtn, 0, 2);
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Incomplete load of file into the working buffer",
              "16.007");

    /* Test with no working buffers available */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    CFE_TBL_TaskData.Registry[0].TableLoadedOnce = TRUE;
    CFE_TBL_TaskData.Registry[0].DblBuffered = FALSE;
    CFE_TBL_TaskData.
      Registry[0].
        Buffers[CFE_TBL_TaskData.Registry[0].ActiveBufferIndex] = BufferPtr;

    for (j = 0; j < CFE_TBL_MAX_SIMULTANEOUS_LOADS; j++)
    {
        CFE_TBL_TaskData.LoadBuffs[j].Taken = TRUE;
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "No working buffers available",
              "16.008");

    /* Test with table header indicating data beyond size of the table */
    UT_InitData();
    TblFileHeader.NumBytes = sizeof(CFE_TBL_File_Hdr_t);

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    CFE_TBL_TaskData.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t) - 1;
    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Table header indicates data beyond size of the table",
              "16.009");

    /* Test with table header indicating no data in the file */
    UT_InitData();
    TblFileHeader.NumBytes = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
    }

    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "Table header indicates no data in file",
              "16.010");

    /* Test where file has partial load for uninitialized table */
    UT_InitData();
    TblFileHeader.NumBytes = 1;
    TblFileHeader.Offset = 1;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    CFE_TBL_TaskData.Registry[0].TableLoadedOnce = FALSE;

    CFE_TBL_TaskData.Registry[0].Size = sizeof(CFE_TBL_File_Hdr_t);
    strncpy(TblFileHeader.TableName, CFE_TBL_TaskData.Registry[0].Name,
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_Report(CFE_TBL_LoadCmd((CFE_SB_Msg_t *) &LoadCmd) ==
                                  CFE_TBL_INC_ERR_CTR,
              "CFE_TBL_LoadCmd",
              "File has partial load for uninitialized table",
              "16.011");
}

/*
** Test the function that processes housekeeping request message
*/
void Test_CFE_TBL_HousekeepingCmd(void)
{
    int                   i;
    CFE_TBL_LoadBuff_t    DumpBuff;
    CFE_TBL_LoadBuff_t    *DumpBuffPtr = &DumpBuff;
    CFE_TBL_RegistryRec_t RegRecPtr;
    CFE_TBL_DumpControl_t DumpControl = CFE_TBL_TaskData.DumpControlBlocks[0];
    uint8                 Buff;
    uint8                 *BuffPtr = &Buff;
    char                  DataSrc[OS_MAX_PATH_LEN];
    uint32                Secs = 0;
    uint32                SubSecs = 0;
    int32                 LoadInProg = 0;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Housekeeping Command\n");
#endif

    /* Test response to inability to update timestamp in dump file + inability
     * to send Hk packet
     */
    UT_InitData();
    strncpy(CFE_TBL_TaskData.DumpControlBlocks[0].TableName,
           "housekeepingtest", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.DumpControlBlocks[0].
        TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    CFE_TBL_TaskData.DumpControlBlocks[0].Size = 10;
    LoadInProg = CFE_TBL_NO_LOAD_IN_PROGRESS + 1;
    RegRecPtr.LoadInProgress = LoadInProg;
    CFE_TBL_TaskData.DumpControlBlocks[0].RegRecPtr = &RegRecPtr;
    DumpBuffPtr->Taken = TRUE;
    DumpBuffPtr->Validated = TRUE;
    DumpBuffPtr->BufferPtr = BuffPtr;
    DumpBuffPtr->FileCreateTimeSecs = Secs;
    DumpBuffPtr->FileCreateTimeSubSecs = SubSecs;
    strncpy(DumpBuffPtr->DataSource, DataSrc, OS_MAX_PATH_LEN);
    DumpBuffPtr->DataSource[OS_MAX_PATH_LEN - 1] = '\0';
    CFE_TBL_TaskData.DumpControlBlocks[0].DumpBufferPtr = DumpBuffPtr;
    CFE_TBL_TaskData.DumpControlBlocks[0].State = CFE_TBL_DUMP_PERFORMED;

    for (i = 1; i < CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_TaskData.DumpControlBlocks[i].State = CFE_TBL_DUMP_PENDING;
    }

    UT_SetRtnCode(&SBSendMsgRtn, CFE_SUCCESS - 1, 1);
    UT_SetRtnCode(&FSSetTimestampRtn, OS_FS_SUCCESS - 1, 1);
    CFE_TBL_TaskData.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND + 1;
    UT_Report(CFE_TBL_HousekeepingCmd((CFE_SB_Msg_t *) &DumpControl) ==
                                      CFE_TBL_DONT_INC_CTR,
              "CFE_TBL_HousekeepingCmd",
              "Able to open dump file",
              "17.001");

    for (i = 1; i < CFE_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        CFE_TBL_TaskData.DumpControlBlocks[i].State = CFE_TBL_DUMP_PENDING;
    }

    RegRecPtr.LoadInProgress = LoadInProg;
    CFE_TBL_TaskData.DumpControlBlocks[0].RegRecPtr = &RegRecPtr;

    /* Test response to inability to open dump file */
    UT_InitData();
    CFE_TBL_TaskData.DumpControlBlocks[0].State = CFE_TBL_DUMP_PERFORMED;
    CFE_TBL_TaskData.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND + 1;
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(CFE_TBL_HousekeepingCmd((CFE_SB_Msg_t *) &DumpControl) ==
                                          CFE_TBL_DONT_INC_CTR,
              "CFE_TBL_HousekeepingCmd",
              "Unable to open dump file",
              "17.002");
}

/*
** Function to prepare for test table API functions
*/
void Test_CFE_TBL_ApiInit(void)
{
    UT_SetAppID(1);
    UT_ResetCDS();
    CFE_TBL_EarlyInit();
    CFE_TBL_TaskData.TableTaskAppId = 10;
}

/*
** Test function that registers a table with cFE to obtain table management
** services
*/
void Test_CFE_TBL_Register(void)
{
    int32            RtnCode;
    int32            RtnCode2;
    CFE_TBL_Handle_t TblHandle1;
    CFE_TBL_Handle_t TblHandle2;
    CFE_TBL_Handle_t TblHandle3;
    boolean          EventsCorrect;
    char             TblName[CFE_TBL_MAX_NAME_LENGTH + 2];
    int16            i;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Register\n");
#endif

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetRtnCode(&GetAppIDRtn, CFE_ES_ERR_APPID, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_ES_ERR_APPID && EventsCorrect,
              "CFE_TBL_Register",
              "Invalid application ID",
              "18.001");

    /* Test response to an application ID larger than the maximum allowed */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS);
    UT_SetRtnCode(&GetAppIDRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_APP_ID && EventsCorrect,
              "CFE_TBL_Register",
              "Application ID > maximum allowed",
              "18.002");

    /* Test response to a table name longer than the maximum allowed */
    UT_InitData();

    /* Create a table name longer than alowed */
    for (i = 0; i < CFE_TBL_MAX_NAME_LENGTH + 1; i++)
    {
        TblName[i] = 'A';
    }

    TblName[i] = '\0';
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Register(&TblHandle1, TblName,
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect,
              "CFE_TBL_Register",
              "Table name too long",
              "18.003");

    /* Test response to a table name shorter than the minimum allowed */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect,
              "CFE_TBL_Register",
              "Table name too short",
              "18.004");

    /*  Test response to a table size of zero */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               0, CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Size of table = 0",
              "18.005");

    /* Test response to a table size larger than the maximum allowed */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               (CFE_TBL_MAX_SNGL_TABLE_SIZE + 1),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Table size too large",
              "18.006");

    /* Test response to a double-buffered table size larger than the
     * maximum allowed
     */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               (CFE_TBL_MAX_DBL_TABLE_SIZE + 1),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Double-buffered table size too large",
              "18.007");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               ((CFE_TBL_OPT_USR_DEF_ADDR &
                                 ~CFE_TBL_OPT_LD_DMP_MSK) |
                                CFE_TBL_OPT_DBL_BUFFER), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect,
              "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
                "CFE_TBL_OPT_DBL_BUFFER)",
              "18.008");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_LOAD_DUMP)
     */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_USR_DEF_ADDR &
                                ~CFE_TBL_OPT_LD_DMP_MSK), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect,
              "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
                "CFE_TBL_OPT_LOAD_DUMP)",
              "18.009");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               ((CFE_TBL_OPT_USR_DEF_ADDR &
                                 ~CFE_TBL_OPT_LD_DMP_MSK) |
                                CFE_TBL_OPT_CRITICAL), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect,
              "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_USR_DEF_ADDR | "
                "CFE_TBL_OPT_CRITICAL)",
              "18.010");

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_DUMP_ONLY |
                                CFE_TBL_OPT_DBL_BUFFER), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_OPTIONS && EventsCorrect,
              "CFE_TBL_Register",
              "Invalid table option combination (CFE_TBL_OPT_DUMP_ONLY | "
                "CFE_TBL_OPT_DBL_BUFFER)",
              "18.011");

    /* Test response to a memory handle error */
    UT_InitData();
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_ES_ERR_MEM_HANDLE && EventsCorrect,
              "CFE_TBL_Register",
              "Memory handle error",
              "18.012");

    /* Test response to a memory block size error */
    UT_InitData();
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_BLOCK_SIZE, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_ES_ERR_MEM_BLOCK_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Memory block size error",
              "18.013");

    /* Test response to a memory block size error (for a second buffer) */
    UT_InitData();
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_BLOCK_SIZE, 2);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_ES_ERR_MEM_BLOCK_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Memory block size error (for second buffer)",
              "18.014");

    /* Test successfully getting a double buffered table */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Get a double buffered table - successful",
              "18.015");

    /* Test attempt to register table owned by another application */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Register(&TblHandle3, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_DUPLICATE_NOT_OWNED && EventsCorrect,
              "CFE_TBL_Register",
              "Table owned by another application",
              "18.016");

    /* Test attempt to register existing table with a different size */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Register(&TblHandle3, "UT_Table1",
                               sizeof(UT_Table2_t),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_DUPLICATE_DIFF_SIZE && EventsCorrect,
              "CFE_TBL_Register",
              "Table size mismatch",
              "18.017");

    /* Test attempt to register a table with the same size and name */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode = CFE_TBL_Share(&TblHandle3, "ut_cfe_tbl.UT_Table1");
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Table with same size and name (setup)",
              "18.018");

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(1); /* Restore AppID to proper value */
    RtnCode = CFE_TBL_Register(&TblHandle2, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_WARN_DUPLICATE && EventsCorrect &&
              TblHandle1 == TblHandle2,
              "CFE_TBL_Register",
              "Table with same size and name",
              "18.019");

    /* c. Test cleanup: unregister tables */
    UT_ClearEventHistory();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Unregister(TblHandle2);
    UT_SetAppID(2);
    RtnCode2 = CFE_TBL_Unregister(TblHandle3);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && RtnCode2 == CFE_SUCCESS &&
              EventsCorrect,
              "CFE_TBL_Unregister",
              "Table with same size and name (cleanup)",
              "18.020");

    /* Test registering a single buffered table */
    /* a. Perform test */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register single buffered table",
              "18.021");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register single buffered table (cleanup)",
              "18.022");

    /* Test registering a single buffered dump-only table */
    /* a. Perform test */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               (CFE_TBL_OPT_SNGL_BUFFER |
                                CFE_TBL_OPT_DUMP_ONLY), NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register single buffered dump-only table",
              "18.023");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register single buffered dump-only table (cleanup)",
              "18.024");

    /* Test registering a user defined address table */
    /* a. Perform test */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_USR_DEF_ADDR, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register user defined address table",
              "18.025");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register user defined address table (cleanup)",
              "18.026");

    /* Test registering a critical table */
    /* a. Perform test */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register critical table",
              "18.027");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register critical table (cleanup)",
              "18.028");

    /* Test registering a critical table that already has an allocated CDS */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_CDS_ALREADY_EXISTS, 1);
    UT_CDS_Map.NextHandle--;
    CFE_TBL_TaskData.CritReg[0].TableLoadedOnce = TRUE;
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_RECOVERED_TBL && EventsCorrect,
              "CFE_TBL_Register",
              "Register critical table that already has an allocated CDS",
              "18.029");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register critical table that already has an allocated "
                "CDS (cleanup)",
              "18.030");

    /* Test registering a critical table that already has an allocated CDS but
     * fails recovery
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_CDS_ALREADY_EXISTS, 1);
    UT_SetRtnCode(&ES_RestoreFromCDSRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register critical table that already has an allocated CDS but "
                "fails recovery",
              "18.031");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register critical table that already has an allocated CDS but "
                "fails recovery (cleanup)",
              "18.032");

    /* Test registering a critical table that already has an allocated CDS but
     * no critical table registry entry
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_CDS_ALREADY_EXISTS, 1);

    /* Remove all entries from critical table registry */
    for (i = 0; i < CFE_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_TaskData.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register Critical table that already has an allocated CDS "
                "but no critical table registry entry",
              "18.033");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register Critical table that already has an allocated CDS "
                "but no critical table registry entry (cleanup)",
              "18.034");

    /* Test registering a critical table but no critical table registry entry
     * is free
     */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_SUCCESS, 1);

    /* Remove all entries from critical table registry */
    /* a. Perform test */
    for (i = 0; i < CFE_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_TaskData.CritReg[i].CDSHandle = 1;
    }

    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Register Critical table but no critical table registry entry "
                "is free",
              "18.035");

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Register Critical table but no critical table registry entry "
                "is free (cleanup)",
              "18.036");

    /* Test attempt to register a critical table when the CDS registry
     * is full
     */
    UT_InitData();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_CDS_REGISTRY_FULL, 1);
    RtnCode = CFE_TBL_Register(&TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_WARN_NOT_CRITICAL && EventsCorrect,
              "CFE_TBL_Register",
              "Register critical table when CDS registry is full",
              "18.037");

    /* Test attempt to register a table when the registry is full */
    /* a. Test setup */
    UT_InitData();
    UT_ResetTableRegistry();
    i = 0;
    RtnCode = CFE_SUCCESS;

    while (i < CFE_TBL_MAX_NUM_TABLES && RtnCode == CFE_SUCCESS)
    {
        SNPRINTF(TblName, CFE_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
        RtnCode = CFE_TBL_Register(&TblHandle1, TblName,
                                   sizeof(UT_Table1_t),
                                   CFE_TBL_OPT_DEFAULT, NULL);
        i++;
    }

    UT_Report(RtnCode == CFE_SUCCESS,
              "CFE_TBL_Register",
              "Registry full (setup)",
              "18.038");

    /* b. Perform test */
    UT_ClearEventHistory();
    SNPRINTF(TblName, CFE_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
    RtnCode = CFE_TBL_Register(&TblHandle2, TblName,
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_REGISTRY_FULL && EventsCorrect,
              "CFE_TBL_Register",
              "Registry full",
              "18.039");

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Registry full (cleanup)",
              "18.040");

    /* Test response to no available handles */
    /* a. Test setup */
    UT_InitData();

    do
    {
        RtnCode = CFE_TBL_Share(&TblHandle1, "ut_cfe_tbl.UT_Table2");
    } while ((TblHandle1 < CFE_TBL_MAX_NUM_HANDLES - 1) &&
             RtnCode == CFE_SUCCESS);

        UT_Report(RtnCode == CFE_SUCCESS,
                  "CFE_TBL_Share",
                  "No available handles (setup)",
                  "18.041");

    /* b. Perform test */
    UT_ClearEventHistory();
    SNPRINTF(TblName, CFE_TBL_MAX_NAME_LENGTH, "UT_Table%d",
             CFE_TBL_MAX_NUM_TABLES);
    RtnCode = CFE_TBL_Register(&TblHandle1,
                               TblName,
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT,
                               NULL);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_HANDLES_FULL && EventsCorrect,
              "CFE_TBL_Register",
              "No available handles",
              "18.042");
}

/*
** Test function that obtains the handle of table registered by another
** application
*/
void Test_CFE_TBL_Share(void)
{
    int32              RtnCode;
    boolean            EventsCorrect;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Share\n");
#endif

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetRtnCode(&GetAppIDRtn, CFE_ES_ERR_APPID, 1);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table2");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_ES_ERR_APPID && EventsCorrect,
              "CFE_TBL_Share",
              "Invalid application ID",
              "19.001");

    /* Test response to an application ID larger than the maximum allowed */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS);
    UT_SetRtnCode(&GetAppIDRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table2");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_APP_ID && EventsCorrect,
              "CFE_TBL_Share",
              "Application ID > maximum allowed",
              "19.002");

    /* Test response when table name is not in the registry */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.NOT_Table2");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect,
              "CFE_TBL_Share",
              "Table name not in registry",
              "19.003");

    /* Test response when there are no available table handles */
    UT_InitData();
    RtnCode = CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.UT_Table3");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_SHARE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_HANDLES_FULL && EventsCorrect,
              "CFE_TBL_Share",
              "No available table handles",
              "19.004");

    /* Test unregistering tables to free handles */
    UT_InitData();
    RtnCode = CFE_TBL_Unregister(CFE_TBL_MAX_NUM_HANDLES / 2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Free handles",
              "19.005");

    /* Test unregister response to a PutPoolBuf error */
    UT_InitData();
    UT_SetRtnCode(&PutPoolRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    RtnCode = CFE_TBL_Unregister(CFE_TBL_MAX_NUM_HANDLES / 2 + 1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "PutPoolBuf error",
              "19.006");

    /* Test successful first load of a table */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    strncpy(StdFileHeader.Description, "FS header description",
            CFE_FS_HDR_DESC_MAX_LEN);
    StdFileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table4",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(3, CFE_TBL_SRC_FILE, "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "First load of a table - successful",
              "19.007");

    /* Test successful share of a table that has not been loaded once */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table3");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Share",
              "Share of table that has not been loaded - successful",
              "19.008");

    /* Test successful share of a table that has been loaded once */
    UT_InitData();
    RtnCode = CFE_TBL_Share(&App2TblHandle2, "ut_cfe_tbl.UT_Table4");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Share",
              "Share of table that has been loaded - successful",
              "19.009");
}

/*
** Function to test unregistering a previously registered table and freeing
** associated resources
*/
void Test_CFE_TBL_Unregister(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregister\n");
#endif

    /* Test response to unregistering a table with an invalid handle */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Unregister(CFE_TBL_MAX_NUM_HANDLES);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UNREGISTER_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report((RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect),
              "CFE_TBL_Unregister",
              "Invalid handle",
              "20.001");

    /* Unregister a shared table to make it unowned */
    UT_InitData();
    RtnCode = CFE_TBL_Unregister(3);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Unregister shared table to make it unowned",
              "20.002");

    /* Test response to unregistering an unowned table */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Unregister(App2TblHandle2);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_UNREGISTER_ERR_EID) == FALSE &&
         UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Unregister unowned table",
              "20.003");
}

/*
** Test function that instructs table services to notify calling application
** whenever the specified table requires management
*/
void Test_CFE_TBL_NotifyByMessage(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Notify by Message\n");
#endif

    /* Set up notify by message tests */
    UT_InitData();
    UT_SetAppID(1);
    UT_ResetCDS();
    CFE_TBL_EarlyInit();
    UT_ResetPoolBufferIndex();

    RtnCode = CFE_TBL_Register(&App1TblHandle1, "NBMsg_Tbl",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Notify by message (setup)",
              "21.001");

    /* Test successful notification */
    UT_InitData();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode = CFE_TBL_NotifyByMessage(App1TblHandle1, 1, 1, 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_NotifyByMessage",
              "Notify - success",
              "21.002");

    /* Test response to notification request when the application doesn't
     * own the table handle
     */
    UT_InitData();
    CFE_TBL_TaskData.Registry[0].OwnerAppId = CFE_TBL_NOT_OWNED;
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    RtnCode = CFE_TBL_NotifyByMessage(App1TblHandle1, 1, 1, 1);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect,
              "CFE_TBL_NotifyByMessage",
              "Notify - no access",
              "21.003");
}

/*
** Function to test loading a specified table with data from the
** specified source
*/
void Test_CFE_TBL_Load(void)
{
    CFE_TBL_Handle_t   DumpOnlyTblHandle;
    CFE_TBL_Handle_t   UserDefTblHandle;
    UT_Table1_t        TestTable1;
    int32              RtnCode;
    boolean            EventsCorrect;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    UT_Table1_t        *App2TblPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Load\n");
#endif

    /* Set up for table load test */
    UT_InitData();
    UT_SetAppID(1);
    UT_ResetTableRegistry();
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Load setup - single buffered table",
              "22.001");

    /* Test attempt to perform partial INITIAL load */
    UT_InitData();
    UT_SetOSFail(OS_NO_FAIL);
    strncpy(StdFileHeader.Description,"Test description",
            CFE_FS_HDR_DESC_MAX_LEN);
    StdFileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 1;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle1,
                           CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_PARTIAL_LOAD && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to perform partial INITIAL load",
              "22.002");

    /* Test attempt to load a file that has incompatible data for the
     * specified table
     */
    UT_InitData();
    strncpy(StdFileHeader.Description,"Test description",
            CFE_FS_HDR_DESC_MAX_LEN);
    StdFileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.NotUT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle1,
                           CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_FILE_FOR_WRONG_TABLE && EventsCorrect,
              "CFE_TBL_Load",
              "File data incompatible with table",
              "22.003");

    /* Test performing a Load from memory */
    UT_InitData();
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Perform load from memory",
              "22.004");

    /* Test attempt to load from an illegal source type (not a file or
     * from memory)
     */
    UT_InitData();
    RtnCode = CFE_TBL_Load(App1TblHandle1,
                           (CFE_TBL_SrcEnum_t) 99,
                           &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_TYPE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_ILLEGAL_SRC_TYPE && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to load from illegal source type",
              "22.005");

    /* Test failure of validation function on table load using a negative
     * return code
     */
    UT_InitData();
    UT_SetRtnCode(&TBL_ValidationFuncRtn, -1, 1);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == -1 && EventsCorrect,
              "CFE_TBL_Load",
              "Fail validation function on table load (negative return code)",
              "22.006");

    /* Test failure of validation function on table load using a positive
     * return code
     */
    UT_InitData();
    UT_SetRtnCode(&TBL_ValidationFuncRtn, 1, 1);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == -1 && EventsCorrect,
              "CFE_TBL_Load",
              "Fail validation function on table load (positive return code)",
              "22.007");

    /* Test attempt to load a table with a bad handle */
    UT_InitData();
    RtnCode = CFE_TBL_Load(CFE_TBL_MAX_NUM_HANDLES,
                           CFE_TBL_SRC_ADDRESS,
                           &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to load table with bad handle",
              "22.008");

    /* Test attempt to load a dump-only table */
    /* a. Test setup */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&DumpOnlyTblHandle, "UT_Table2",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DUMP_ONLY, NULL);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Attempt to load a dump-only table (setup)",
              "22.009");

    /* b. Perform test */
    UT_InitData();
    RtnCode = CFE_TBL_Load(DumpOnlyTblHandle,
                           CFE_TBL_SRC_ADDRESS,
                           &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_DUMP_ONLY && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to load a dump-only table",
              "22.010");

    /* Test specifying a table address for a user defined table */
    /* a. Test setup */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&UserDefTblHandle,
                               "UT_Table3", sizeof(UT_Table1_t),
                               CFE_TBL_OPT_USR_DEF_ADDR, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Specify table address for a user defined table",
              "22.011");

    /* Perform test */
    UT_InitData();
    RtnCode = CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Specify table address for a user defined table",
              "22.012");

    /* Test attempt to load a locked shared table */
    /* a. Test setup part 1 */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Share",
              "Attempt to load locked shared table (setup part 1)",
              "22.013");

    /* a. Test setup part 2 */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress((void **) &App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Attempt to load locked shared table (setup part 2)",
              "22.014");

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_TABLE_LOCKED && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to load locked shared table",
              "22.015");

    /* d. Test cleanup */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_ReleaseAddress(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_ReleaseAddress",
              "Attempt to load locked shared table (cleanup)",
              "22.016");
}

/*
** Function to test obtaining the current address of the contents
** of the specified table
*/
void Test_CFE_TBL_GetAddress(void)
{
    int32       RtnCode;
    boolean     EventsCorrect;
    UT_Table1_t *App3TblPtr;
    UT_Table1_t *App2TblPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Address\n");
#endif

    /* Test attempt to get the address of a table for which the application
     * does not have access
     */
    UT_InitData();
    UT_SetAppID(3);
    RtnCode = CFE_TBL_GetAddress((void **) &App3TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Application does not have access to table",
              "23.001");

    /* Test attempt to get the address with an invalid application ID */
    UT_InitData();
    UT_SetRtnCode(&GetAppIDRtn, CFE_ES_ERR_APPID, 1);
    RtnCode = CFE_TBL_GetAddress((void **) &App3TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_ES_ERR_APPID && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Invalid application ID",
              "23.002");

    /* Test attempt to get the address with an invalid handle */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress((void **) &App3TblPtr,
                                 CFE_TBL_MAX_NUM_HANDLES);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Invalid table handle",
              "23.003");

    /* Attempt to get the address of an unregistered (unowned) table */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Unregister(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Attempt to get address of unregistered table (setup)",
              "23.004");

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_GetAddress((void **) &App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_UNREGISTERED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Attempt to get address of unregistered table",
              "23.005");
}

/*
** Function to test release of a previously obtained pointer to the
** contents of the specified table
*/
void Test_CFE_TBL_ReleaseAddress(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Release Address\n");
#endif

    /* Test address release using an invalid application ID */
    /* a. Test setup - register single buffered table */
    UT_InitData();
    UT_SetAppID(1);
    UT_ResetTableRegistry();
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Attempt to release address with invalid application ID (setup)",
              "24.001");

    /* b. Perform test */
    UT_InitData();
    UT_SetRtnCode(&GetAppIDRtn, CFE_ES_ERR_APPID, 1);
    RtnCode = CFE_TBL_ReleaseAddress(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_ES_ERR_APPID && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Attempt to release address with invalid application ID",
              "24.002");
}

/*
** Test function that obtains the current addresses of the contents of a
** collection of tables
*/
void Test_CFE_TBL_GetAddresses(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Addresses\n");
#endif

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Register(&App1TblHandle2,
                               "UT_Table2",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "CFE_TBL_GetAddresses setup - register a double buffered table",
              "25.001");

    /* b. Perform test */
    UT_InitData();

    /* Test getting the addresses of two tables that have not been loaded */
    ArrayOfHandles[0] = App1TblHandle1;
    ArrayOfHandles[1] = App1TblHandle2;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    ArrayOfPtrsToTblPtrs[1] = &Tbl2Ptr;

    RtnCode = CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect &&
              Tbl1Ptr != NULL && Tbl2Ptr != NULL,
              "CFE_TBL_GetAddresses",
              "Get addresses of two tables (neither of which have "
                "been loaded)",
              "25.002");

    /* Test attempt to get addresses of tables that the application is not
     * allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS);
    RtnCode = CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_APP_ID && EventsCorrect,
              "CFE_TBL_Validate",
              "Attempt to get addresses of tables that application is not "
                "allowed to see",
              "25.003");
}

/*
** Test function that releases previously obtained pointers to the contents
** of the specified tables
*/
void Test_CFE_TBL_ReleaseAddresses(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Release Addresses\n");
#endif

    /* Test response to releasing two tables that have not been loaded */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_ReleaseAddresses(2, ArrayOfHandles);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect,
              "CFE_TBL_ReleaseAddresses",
              "Release addresses of two tables (neither of which have "
                "been loaded)",
              "26.001");
}

/*
** Test function for validating the table image for a specified table
*/
void Test_CFE_TBL_Validate(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Validate\n");
#endif

    /* Test response to attempt to validate a table that an application is
     * not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS);
    RtnCode = CFE_TBL_Validate(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_APP_ID && EventsCorrect,
              "CFE_TBL_Validate",
              "Attempt to validate table that application is not allowed "
                "to see",
              "27.001");

    /* Test response to attempt to validate a table when no validation is
     * pending
     */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Validate(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_NO_VALIDATION_PENDING &&
              EventsCorrect,
              "CFE_TBL_Validate",
              "Attempt to validate table when no validation is pending",
              "27.002");
}

/*
** Test function for performing standard table maintenance on the
** specified table
*/
void Test_CFE_TBL_Manage(void)
{
    int32                 RtnCode;
    boolean               EventsCorrect;
    int32                 RegIndex;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t    *WorkingBufferPtr;
    UT_Table1_t           TestTable1;
    UT_Table1_t           *App2TblPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Manage\n");
#endif

    /* Test response to attempt to manage a table that doesn't need managing */
    UT_InitData();
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Manage",
              "Manage table that doesn't need managing",
              "28.001");

    /* Test response to attempt to load while a load is in progress */
    UT_InitData();

    /* "Load" image into inactive buffer for table */
    RegIndex = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table1");
    RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_LOAD_IN_PROGRESS && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to load while a load is in progress",
              "28.002");

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer; validation function return code is valid
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = FALSE;
    RegRecPtr->ValidateInactiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, -1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on "
                "inactive buffer (valid function return code)",
              "28.003");

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer ; validation function return code is invalid
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = FALSE;
    RegRecPtr->ValidateInactiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, 1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on "
                "inactive buffer (invalid function return code)",
              "28.004");

    /* Test response to processing a successful validation request on an
     * inactive buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = FALSE;
    RegRecPtr->ValidateInactiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on "
                "an inactive buffer",
              "28.005");

    /* Test response to processing an unsuccessful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = TRUE;
    RegRecPtr->ValidateActiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, -1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending on "
                "an active buffer",
              "28.006");

    /* Test response to processing an unsuccessful validation request on
     * an active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = TRUE;
    RegRecPtr->ValidateActiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, 1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending "
                "on an active buffer",
              "28.007");

    /* Test response to processing a successful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table1", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = TRUE;
    RegRecPtr->ValidateActiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on "
                "an active buffer",
              "28.008");

    /* Test response to processing an update request on a locked table */
    /* a. Test setup - part 1 */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Share",
              "Process an update request on a locked table (setup - part 1)",
              "28.009");

    /* a. Test setup - part 2 */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress((void **) &App2TblPtr, App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Process an update request on a locked table (setup - part 2)",
              "28.010");

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(1);

    /* Configure table for update */
    RegRecPtr->LoadPending = TRUE;
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_TABLE_LOCKED && EventsCorrect,
              "CFE_TBL_Manage",
              "Process an update request on a locked table",
              "28.011");

    /* Test unlocking a table by releasing the address */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_ReleaseAddress(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_NEVER_LOADED && EventsCorrect,
              "CFE_TBL_ReleaseAddress",
              "Release address to unlock shared table",
              "28.012");

    /* Test response to processing an update request on a single
     * buffered table
     */
    UT_InitData();
    UT_SetAppID(1);

    /* Configure table for Update */
    RegRecPtr->LoadPending = TRUE;
    RtnCode = CFE_TBL_Manage(App1TblHandle1);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_UPDATE_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect,
              "CFE_TBL_Manage",
              "Process an update request on a single buffered table",
              "28.013");

    /* Test response to processing an unsuccessful validation request on an
     * inactive buffer (double buffered)
     */
    UT_InitData();

    /* "Load" image into inactive buffer for table */
    RegIndex = CFE_TBL_FindTableInRegistry("ut_cfe_tbl.UT_Table2");
    RegRecPtr = &CFE_TBL_TaskData.Registry[RegIndex];
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table2", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = FALSE;
    RegRecPtr->ValidateInactiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, -1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has a failed validation pending on an "
                "inactive buffer (double buffered)",
              "28.014");

    /* Test successfully processing a validation request on an inactive buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table2", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = FALSE;
    RegRecPtr->ValidateInactiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on an "
                "inactive buffer (double buffered)",
              "28.015");

    /* Test processing an unsuccessful validation request on an active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 0;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table2", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = TRUE;
    RegRecPtr->ValidateActiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, -1, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == -1,
              "CFE_TBL_Manage",
              "Manage table that has an unsuccessful validation pending on an "
                "active buffer (double buffered)",
              "28.016");

    /* Test successfully processing a validation request on active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    CFE_TBL_TaskData.ValidationResults[0].State = CFE_TBL_VALIDATION_PENDING;
    CFE_TBL_TaskData.ValidationResults[0].Result = 1;
    strncpy(CFE_TBL_TaskData.ValidationResults[0].TableName,
            "ut_cfe_tbl.UT_Table2", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.ValidationResults[0].CrcOfTable = 0;
    CFE_TBL_TaskData.ValidationResults[0].ActiveBuffer = TRUE;
    RegRecPtr->ValidateActiveIndex = 0;

    /* Perform validation via manage call */
    UT_SetRtnCode(&TBL_ValidationFuncRtn, CFE_SUCCESS, 1);
    RtnCode = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_VALIDATION_INF_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
              CFE_TBL_TaskData.ValidationResults[0].Result == 0,
              "CFE_TBL_Manage",
              "Manage table that has a successful validation pending on an "
                "active buffer (double buffered)",
              "28.017");

    /* Test successfully processing a table dump request */
    UT_InitData();
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);
    CFE_TBL_TaskData.DumpControlBlocks[0].State = CFE_TBL_DUMP_PENDING;
    CFE_TBL_TaskData.DumpControlBlocks[0].RegRecPtr = RegRecPtr;

    /* Save the name of the desired dump filename, table name, and size for
     * later
     */
    CFE_TBL_TaskData.DumpControlBlocks[0].DumpBufferPtr = WorkingBufferPtr;
    CFE_PSP_MemCpy(CFE_TBL_TaskData.DumpControlBlocks[0].
                     DumpBufferPtr->DataSource,
                   "MyDumpFilename", OS_MAX_PATH_LEN);
    CFE_PSP_MemCpy(CFE_TBL_TaskData.DumpControlBlocks[0].TableName,
                   "ut_cfe_tbl.UT_Table2", CFE_TBL_MAX_FULL_NAME_LEN);
    CFE_TBL_TaskData.DumpControlBlocks[0].Size = RegRecPtr->Size;
    RegRecPtr->DumpControlIndex = 0;
    RtnCode = CFE_TBL_Manage(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Manage",
              "Manage table that has a dump request pending",
              "28.018");
}

/*
** Test function that updates the contents of a table if an update is pending
*/
void Test_CFE_TBL_Update(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Update\n");
#endif

    /* Test processing an update on a single buffered table without
     * privileges
     */
    UT_InitData();
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Update(App1TblHandle1);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_UPDATE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_ACCESS && EventsCorrect,
              "CFE_TBL_Update",
              "Process an update on a single buffered table "
                "without privileges",
              "29.001");

    /* Test processing an update on a single buffered table when no update
     * is pending
     */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Update(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_NO_UPDATE_PENDING && EventsCorrect,
              "CFE_TBL_Update",
              "Process an update on a single buffered table when no update "
                "is pending",
              "29.002");
}

/*
** Test function that obtains the pending action status for specified table
*/
void Test_CFE_TBL_GetStatus(void)
{
    int32   RtnCode;
    boolean EventsCorrect;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Status\n");
#endif

    /* Test response to an attempt to get the status on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_MAX_APPLICATIONS);
    RtnCode = CFE_TBL_GetStatus(App1TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_APP_ID && EventsCorrect,
              "CFE_TBL_GetStatus",
              "Attempt to get status on a table that the application is not "
                "allowed to see",
              "30.001");
}

/*
** Test function that obtains characteristics/information for a specified table
*/
void Test_CFE_TBL_GetInfo(void)
{
    int32          RtnCode;
    boolean        EventsCorrect;
    CFE_TBL_Info_t TblInfo;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Info\n");
#endif

    /* Test successfully getting information on a table */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table1");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_GetInfo",
              "Get information on UT_Table1",
              "31.001");

    /* Test response to attempt to get information on a non-existent table */
    UT_InitData();
    RtnCode = CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table_Not");
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_NAME && EventsCorrect,
              "CFE_TBL_GetInfo",
              "Get information on non-existent table",
              "31.002");
}

/*
** Test function that loads a specified table with data from the
** specified source
*/
void Test_CFE_TBL_TblMod(void)
{
    int32           RtnCode;
    int32           RtnCode2;
    boolean         EventsCorrect;
    CFE_FS_Header_t FileHeader;
    UT_TempFile_t   File;
    uint32          Index;
    CFE_TBL_Info_t  TblInfo1;
    UT_Table1_t     *TblDataPtr;
    char            MyFilename[OS_MAX_PATH_LEN];

#ifdef UT_VERBOSE
    UT_Text("Begin Test Table Modified\n");
#endif

    /* Test adding a TBL API for notifying table services that the table has
     * been updated by the application
     */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(1);
    UT_ResetCDS();
    CFE_TBL_EarlyInit();
    UT_ResetPoolBufferIndex();

    /* Test setup for CFE_TBL_Modified; register a non critical table */
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_CRITICAL, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Add TBL API for notifying table services that table has "
                "been updated by application (setup)",
              "32.001");

    /* b. Perform test */
    UT_ClearEventHistory();

    /* Configure for successful file read to initialize table */
    strncpy(FileHeader.Description, "FS header description",
            CFE_FS_HDR_DESC_MAX_LEN);
    FileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    FileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    FileHeader.TimeSeconds = 1704;
    FileHeader.TimeSubSeconds = 104;
    strncpy(File.TblHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    File.TblHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    File.TblHeader.NumBytes = sizeof(UT_Table1_t);
    File.TblHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&File.TblHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&File.TblHeader.Offset);
        File.TblData.TblElement1 = 0x04030201;
        File.TblData.TblElement2 = 0x08070605;
    }
    else
    {
        File.TblData.TblElement1 = 0x01020304;
        File.TblData.TblElement2 = 0x05060708;
    }

    UT_SetReadBuffer(&File, sizeof(UT_TempFile_t));
    UT_SetReadHeader(&FileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);

    /* Perform load */
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile");
    EventsCorrect =
        (UT_GetNumEventsSent() == 1 &&
         UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE);

    /* Modify the contents of the table */
    CFE_TBL_GetAddress((void **)&TblDataPtr, App1TblHandle1);
    CFE_TBL_ByteSwapUint32(&TblDataPtr->TblElement1);

    /* Notify Table Services that the table has been modified */
    RtnCode = CFE_TBL_Modified(App1TblHandle1);
    RtnCode2 = CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table1");
    Index =
        CFE_TBL_TaskData.
          Registry[CFE_TBL_TaskData.Handles[App1TblHandle1].RegIndex].
            CDSHandle;
    UT_Report(RtnCode == CFE_SUCCESS &&
              RtnCode2 == CFE_SUCCESS &&
              EventsCorrect &&
              TblInfo1.TimeOfLastUpdate.Seconds ==
                  TblInfo1.TimeOfLastUpdate.Subseconds &&
              strcmp(TblInfo1.LastFileLoaded, "MyInputFile(*)") == 0 &&
              UT_CDS[Index + 0] == 0x04 && UT_CDS[Index + 1] == 0x03 &&
              UT_CDS[Index + 2] == 0x02 && UT_CDS[Index + 3] == 0x01 &&
              UT_CDS[Index + 4] == 0x05 && UT_CDS[Index + 5] == 0x06 &&
              UT_CDS[Index + 6] == 0x07 && UT_CDS[Index + 7] == 0x08,
              "CFE_TBL_Modified",
              "Add TBL API for notifying table services that table has "
                "been updated by application",
              "32.002");

    /* Test response to adding a TBL API for notifying table services that
     * the table has been updated by application
     */
    /* a. Test setup */
    UT_InitData();

    /* Register a non critical table */
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table2",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT, NULL);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "Add TBL API for notifying table services that table has "
                "been updated by application (setup)",
              "32.003");

    /* Configure for successful file read to initialize table */
    strncpy(FileHeader.Description, "FS header description",
            CFE_FS_HDR_DESC_MAX_LEN);
    FileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    FileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    FileHeader.TimeSeconds = 1704;
    FileHeader.TimeSubSeconds = 104;
    strncpy(File.TblHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    File.TblHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    File.TblHeader.NumBytes = sizeof(UT_Table1_t);
    File.TblHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&File.TblHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&File.TblHeader.Offset);
    }

    File.TblData.TblElement1 = 0x04030201;
    File.TblData.TblElement2 = 0x08070605;
    UT_SetReadBuffer(&File, sizeof(UT_TempFile_t));
    UT_SetReadHeader(&FileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);

    /* Perform load with extra long filename */
    for (Index = 0; Index < OS_MAX_PATH_LEN - 1; Index++)
    {
        MyFilename[Index] = 'a';
    }

    MyFilename[(OS_MAX_PATH_LEN - 1)] = '\0';
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, MyFilename);
    EventsCorrect = (UT_GetNumEventsSent() == 1 &&
                     UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) ==
                         TRUE);

    /* Notify Table Services that the table has been modified */
    RtnCode = CFE_TBL_Modified(App1TblHandle1);
    RtnCode2 = CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table2");
    UT_Report(RtnCode == CFE_SUCCESS &&
              RtnCode2 == CFE_SUCCESS &&
              EventsCorrect &&
              TblInfo1.TimeOfLastUpdate.Seconds ==
                  TblInfo1.TimeOfLastUpdate.Subseconds &&
              strncmp(TblInfo1.LastFileLoaded,
                      MyFilename, OS_MAX_PATH_LEN - 4) == 0 &&
              strncmp(&TblInfo1.LastFileLoaded[OS_MAX_PATH_LEN - 4],
                      "(*)", 3) == 0,
              "CFE_TBL_Modified",
              "Add TBL API for notifying table services that table has "
                "been updated by application",
              "32.004");

    /* Test response to an invalid handle */
    RtnCode = CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect,
              "CFE_TBL_Modified",
              "Invalid table handle",
              "32.005");
}

/*
** Tests for the remaining functions in cfe_tbl_internal.c
*/
void Test_CFE_TBL_Internal(void)
{
    int32                      RtnCode;
    boolean                    EventsCorrect;
    CFE_TBL_LoadBuff_t         *WorkingBufferPtr;
    CFE_TBL_RegistryRec_t      *RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    char                       Filename[OS_MAX_PATH_LEN + 10];
    int32                      i;
    CFE_FS_Header_t            StdFileHeader;
    CFE_TBL_File_Hdr_t         TblFileHeader;
    int32                      OldOSFail;
    int32                      FileDescriptor = 0;
    void                       *TblPtr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Internal\n");
#endif


    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(1);
    RtnCode = CFE_TBL_Register(&App1TblHandle2,
                               "UT_Table3",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER,
                               Test_CFE_TBL_ValidationFunc);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_REGISTER_ERR_EID) == FALSE &&
                     UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Register",
              "CFE_TBL_GetWorkingBuffer setup - register a double buffered table",
              "33.001");

    /* Test successful initial load of double buffered table */
    UT_InitData();
    AccessDescPtr = &CFE_TBL_TaskData.Handles[App1TblHandle2];
    RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    strncpy(RegRecPtr->Name, "ut_cfe_tbl.UT_Table3",
            CFE_TBL_MAX_FULL_NAME_LEN);
    RegRecPtr->Name[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    RegRecPtr->TableLoadedOnce = FALSE;
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, TRUE);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect &&
               WorkingBufferPtr ==
                   &RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex],
              "CFE_TBL_GetWorkingBuffer",
              "Initial load of double buffered table",
              "33.002");

    /* Test response to a single buffered table with a mutex sem take
     * failure
     */
    UT_InitData();
    UT_SetRtnCode(&MutSemTakeRtn, OS_ERROR, 1);
    AccessDescPtr = &CFE_TBL_TaskData.Handles[App1TblHandle1];
    RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    RtnCode = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, FALSE);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_GetWorkingBuffer",
              "Single buffered table has mutex sem take failure",
              "33.003");

    /* Test CFE_TBL_LoadFromFile response to a file name that is too long */
    UT_InitData();

    for (i = 0; i < OS_MAX_PATH_LEN + 9; i++)
    {
        Filename[i] = 'a';
    }

    Filename[i] = '\0'; /* Null terminate file name string */
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_FILENAME_TOO_LONG && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "Filename too long",
              "33.004");

    /* Test CFE_TBL_LoadFromFile response to a file that's content is too large
     * (according to the header)
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    strncpy(StdFileHeader.Description, "FS header description",
            CFE_FS_HDR_DESC_MAX_LEN);
    StdFileHeader.Description[CFE_FS_HDR_DESC_MAX_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 1;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_FILE_TOO_LARGE && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "File content too large (according to header)",
              "33.005");

    /* Test CFE_TBL_LoadFromFile response that's file content is too large
     * (too much content)
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, sizeof(UT_Table1_t), 2);
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_FILE_TOO_LARGE && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "File content too large (too much content)",
              "33.006");

    /* Test CFE_TBL_LoadFromFile response to the file content being
     * incomplete
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, sizeof(UT_Table1_t) - 1, 2);
    UT_SetRtnCode(&OSReadRtn2, 0, 2);
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_LOAD_INCOMPLETE && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "File content incomplete",
              "33.007");

    /* Test CFE_TBL_LoadFromFile response to the file being for the
     * wrong table
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.NotUT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_ERR_FILE_FOR_WRONG_TABLE && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "File for wrong table",
              "33.008");

    /* Test CFE_TBL_LoadFromFile response to an OS open error */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    OldOSFail = UT_OS_Fail;
    UT_OS_Fail = OS_OPEN_FAIL;
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == OS_ERROR && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "OS open error",
              "33.009");
    UT_OS_Fail = OldOSFail;

    /* Test CFE_TBL_LoadFromFile response to a file too short warning */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_LoadFromFile(WorkingBufferPtr, RegRecPtr, Filename);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_WARN_SHORT_FILE && EventsCorrect,
              "CFE_TBL_LoadFromFile",
              "File too short warning",
              "33.010");

    /* Test CFE_TBL_ReadHeaders response to a failure reading the standard cFE
     * file header
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    strncpy(Filename, "MyTestInputFilename", OS_MAX_PATH_LEN + 10);
    Filename[OS_MAX_PATH_LEN + 9] = '\0';
    UT_SetRtnCode(&FSReadHdrRtn, sizeof(CFE_FS_Header_t) - 1, 1);
    RtnCode = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader,
                                  &TblFileHeader, Filename);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_FILE_STD_HDR_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_STD_HEADER && EventsCorrect,
              "CFE_TBL_ReadHeaders",
              "Failure reading standard cFE file header",
              "33.011");

    /* Test CFE_TBL_ReadHeaders response to a bad magic number in cFE
     * standard header
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID - 1;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    RtnCode = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader,
                                  &TblFileHeader, Filename);
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_FILE_TYPE_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_CONTENT_ID && EventsCorrect,
              "CFE_TBL_ReadHeaders",
              "Bad magic number in cFE standard header",
              "33.012");

    /* Test CFE_TBL_ReadHeaders response to a wrong cFE file subtype */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE - 1;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    RtnCode = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader,
                                  &TblFileHeader, Filename);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_FILE_SUBTYPE_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_BAD_SUBTYPE_ID && EventsCorrect,
              "CFE_TBL_ReadHeaders",
              "Wrong cFE file subType",
              "33.013");

    /* Test CFE_TBL_ReadHeaders response to a failure reading the cFE
     * table header
     */
    UT_InitData();
    Filename[OS_MAX_PATH_LEN - 1] = '\0';
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t) - 1;
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, sizeof(CFE_TBL_File_Hdr_t) - 1, 1);
    RtnCode = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader,
                                  &TblFileHeader, Filename);
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_FILE_TBL_HDR_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_TBL_HEADER && EventsCorrect,
              "CFE_TBL_ReadHeaders",
              "Failure reading cFE table header",
              "33.014");

    /* Test CFE_TBL_RemoveAccessLink response to a failure to put back the
     * memory buffer for a double buffered table
     */
    UT_InitData();
    UT_SetRtnCode(&PutPoolRtn, CFE_ES_ERR_MEM_HANDLE, 2);
    RtnCode = CFE_TBL_RemoveAccessLink(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_ES_ERR_MEM_HANDLE && EventsCorrect,
              "CFE_TBL_RemoveAccessLink",
              "Fail to put back memory buffer for double buffered table",
              "33.015");

    /* EarlyInit - Table Registry Mutex Create Failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_ERROR, 1);
    UT_ResetCDS();
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == OS_ERROR && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "Table registry mutex create failure",
              "33.016");

    /* Test CFE_TBL_EarlyInit response to a work buffer mutex create failure */
    UT_InitData();
    UT_SetRtnCode(&MutSemCreateRtn, OS_ERROR, 2);
    UT_ResetCDS();
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == OS_ERROR && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "Work buffer mutex create failure",
              "33.017");

    /* Test CFE_TBL_EarlyInit response to a memory pool create failure */
    UT_InitData();
    UT_SetRtnCode(&CreatePoolRtn, CFE_ES_BAD_ARGUMENT, 1);
    UT_ResetCDS();
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_ES_BAD_ARGUMENT && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "Memory pool create failure",
              "33.018");

    /* Test CFE_TBL_EarlyInit reponse to a get pool buffer failure */
    UT_InitData();
    UT_SetRtnCode(&GetPoolRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    UT_ResetCDS();
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_ES_ERR_MEM_HANDLE && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "Get pool buffer failure",
              "33.019");

    /* Test CFE_TBL_EarlyInit response where the CDS already exists but
     * restore fails
     */
    UT_InitData();
    UT_ResetCDS();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_CDS_ALREADY_EXISTS, 1);
    UT_SetRtnCode(&ES_RestoreFromCDSRtn, CFE_ES_CDS_BLOCK_CRC_ERR, 1);
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "CDS already exists but restore fails",
              "33.020");

    /* Test CFE_TBL_EarlyInit response when no CDS is available */
    UT_InitData();
    UT_ResetCDS();
    UT_SetRtnCode(&ES_RegisterCDSRtn, CFE_ES_NOT_IMPLEMENTED, 1);
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "No CDS available",
              "33.021");

    /* Test CFE_TBL_EarlyInit response to a failure to save a critical table
     * registry to the CDS
     */
    UT_InitData();
    UT_ResetCDS();
    UT_SetRtnCode(&ES_CopyToCDSRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    RtnCode = CFE_TBL_EarlyInit();
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_EarlyInit",
              "Fail to save critical table registry to CDS",
              "33.022");

    /* Reset, then register tables for subsequent tests */
    /* a. Reset tables */
    UT_InitData();
    UT_SetAppID(1);
    UT_ResetCDS();
    RtnCode = CFE_TBL_EarlyInit();
    UT_Report((RtnCode == CFE_SUCCESS),
              "CFE_TBL_EarlyInit",
              "Reset (setup - part 1)",
              "33.023");

    /* b. Register critical single buffered table */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&App1TblHandle1, "UT_Table1",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL,
                               Test_CFE_TBL_ValidationFunc);
    UT_Report(RtnCode == CFE_SUCCESS,
              "CFE_TBL_Register",
              "Critical single buffered table (setup - part 2)",
              "33.024");

    /* c. Register critical double buffered table */
    UT_InitData();
    RtnCode = CFE_TBL_Register(&App1TblHandle2, "UT_Table2",
                               sizeof(UT_Table1_t),
                               CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_CRITICAL,
                               Test_CFE_TBL_ValidationFunc);
    UT_Report(RtnCode == CFE_SUCCESS,
              "CFE_TBL_Register",
              "Critical double buffered table (setup - part 3)",
              "33.025");

    /* d. Perform an initial load on the critical single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Critical single buffered table (setup - part 4)",
              "33.026");

    /* e. Update CDS for single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table1",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Update CDS for single buffered table (setup - part 5)",
              "33.027");

    /* f. Perform an initial load on the critical double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Critical single buffered table (setup - part 6)",
              "33.028");

    /* g. Update CDS for double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Update CDS for single buffered table (setup - part 7)",
              "33.029");

    /* Test CFE_TBL_GetWorkingBuffer response when both double buffered table
     * buffers are locked
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_NO_BUFFER_AVAIL && EventsCorrect,
              "CFE_TBL_Load",
              "Both double buffered table buffers locked",
              "33.030");

    /* Release buffer for error writing to CDS test */
    /* a. Get table address */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Error writing to CDS (setup - part 1)",
              "33.031");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_ReleaseAddress",
              "Error writing to CDS (setup - part 2)",
              "33.032");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    UT_SetRtnCode(&ES_CopyToCDSRtn, CFE_ES_ERR_MEM_HANDLE, 1);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(((RtnCode == CFE_SUCCESS) && EventsCorrect),
              "CFE_TBL_Load",
              "Error writing to CDS",
              "33.033");

    /* Release buffer for error writing to CDS (second time) test */
    /* a. Get table address */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Error writing to CDS second time (setup - part 1)",
              "33.034");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_ReleaseAddress",
              "Error writing to CDS second time (setup - part 2)",
              "33.035");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    UT_SetRtnCode(&ES_CopyToCDSRtn, CFE_ES_ERR_MEM_HANDLE, 2);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Error writing to CDS (second time)",
              "33.036");

    /* Release buffer for failure to find the CDS handle in the CDS
     * registry test
     */
    /* a. Get table address */
    UT_InitData();
    RtnCode = CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_TBL_INFO_UPDATED && EventsCorrect,
              "CFE_TBL_GetAddress",
              "Failure to find CDS handle in CDS registry (setup - part 1)",
              "33.037");

    /* b. Release table address */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_ReleaseAddress(App1TblHandle2);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_ReleaseAddress",
              "Failure to find CDS handle in CDS registry (setup - part 2)",
              "33.038");

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    UT_SetRtnCode(&ES_CopyToCDSRtn, CFE_ES_ERR_MEM_HANDLE, 2);
    AccessDescPtr = &CFE_TBL_TaskData.Handles[App1TblHandle2];
    RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];

    for (i = 0; i < CFE_TBL_MAX_CRITICAL_TABLES; i++)
    {
        if (CFE_TBL_TaskData.CritReg[i].CDSHandle == RegRecPtr->CDSHandle)
        {
            CFE_TBL_TaskData.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE - 1;
        }
    }

    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect =
        (UT_EventIsInHistory(CFE_TBL_LOAD_SUCCESS_INF_EID) == TRUE &&
         UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Load",
              "Failure to find CDS handle in CDS registry",
              "33.039");

    /* Test unregistering a shared table */
    /* a. Share table */
    UT_InitData();
    CFE_TBL_TaskData.CritReg[i].CDSHandle = RegRecPtr->CDSHandle;
    UT_SetAppID(2);
    RtnCode = CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1");
    UT_Report(RtnCode == CFE_SUCCESS,
              "CFE_TBL_Share",
              "Unregister shared table (setup)",
              "33.040");

    /* b. Perform test */
    UT_ClearEventHistory();
    RtnCode = CFE_TBL_Unregister(App2TblHandle1);
    EventsCorrect = (UT_GetNumEventsSent() == 0);
    UT_Report(RtnCode == CFE_SUCCESS && EventsCorrect,
              "CFE_TBL_Unregister",
              "Unregister shared table",
              "33.041");

    /* Test successful application cleanup */
    UT_InitData();
    UT_SetAppID(1);
    UT_SetPutPoolFail(TRUE);
    AccessDescPtr = &CFE_TBL_TaskData.Handles[App1TblHandle1];
    RegRecPtr = &CFE_TBL_TaskData.Registry[AccessDescPtr->RegIndex];
    CFE_TBL_TaskData.DumpControlBlocks[3].State = CFE_TBL_DUMP_PENDING;
    CFE_TBL_TaskData.DumpControlBlocks[3].RegRecPtr = RegRecPtr;
    RegRecPtr->LoadInProgress = 1;
    CFE_TBL_TaskData.LoadBuffs[1].Taken = TRUE;
    CFE_TBL_CleanUpApp(1);
    UT_Report(CFE_TBL_TaskData.DumpControlBlocks[3].State ==
                  CFE_TBL_DUMP_FREE &&
              RegRecPtr->OwnerAppId == (uint32) CFE_TBL_NOT_OWNED &&
              CFE_TBL_TaskData.LoadBuffs[RegRecPtr->LoadInProgress].Taken ==
                  FALSE &&
              RegRecPtr->LoadInProgress == CFE_TBL_NO_LOAD_IN_PROGRESS,
              "CFE_TBL_CleanUpApp",
              "Execute clean up - success",
              "33.042");

    /* Test response to an attempt to use an invalid table handle */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType = CFE_FS_TBL_IMG_SUBTYPE;
    strncpy(TblFileHeader.TableName, "ut_cfe_tbl.UT_Table2",
            CFE_TBL_MAX_FULL_NAME_LEN);
    TblFileHeader.TableName[CFE_TBL_MAX_FULL_NAME_LEN - 1] = '\0';
    TblFileHeader.NumBytes = sizeof(UT_Table1_t);
    TblFileHeader.Offset = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader.NumBytes);
        CFE_TBL_ByteSwapUint32(&TblFileHeader.Offset);
    }

    UT_SetReadBuffer(&TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));
    UT_SetReadHeader(&StdFileHeader, sizeof(CFE_FS_Header_t));
    UT_SetRtnCode(&OSReadRtn, 0, 3);
    UT_SetRtnCode(&ES_CopyToCDSRtn, CFE_ES_ERR_MEM_HANDLE, 2);
    RtnCode = CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE,
                           "TblSrcFileName.dat");
    EventsCorrect = (UT_EventIsInHistory(CFE_TBL_LOAD_ERR_EID) == TRUE &&
                     UT_GetNumEventsSent() == 1);
    UT_Report(RtnCode == CFE_TBL_ERR_INVALID_HANDLE && EventsCorrect,
              "CFE_TBL_Load",
              "Attempt to use an invalid handle",
              "33.043");
}

/*
** Test function executed when the contents of a table need to be validated
*/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr)
{
    if (TBL_ValidationFuncRtn.count > 0)
    {
        TBL_ValidationFuncRtn.count--;

        if (TBL_ValidationFuncRtn.count == 0)
        {
            return TBL_ValidationFuncRtn.value;
        }
    }

    return 0;
}
