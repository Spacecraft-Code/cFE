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
** $Id: ut_osapi_stubs.c 1.11 2014/09/05 11:42:11GMT-05:00 acudmore Exp  $
**
** Purpose:
** Unit test stubs for Operating System routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
** $Data:$
** $Revision: 1.11 $
** $Log: ut_osapi_stubs.c  $
** Revision 1.11 2014/09/05 11:42:11GMT-05:00 acudmore 
** Updated text for ES unit test ( OS_printf text for Syslog entries )
** Revision 1.10 2014/05/28 09:21:52GMT-05:00 wmoleski 
** Overwriting cFE Unit Test files with the updated JSC files.
** Revision 1.9 2012/01/13 13:59:31EST acudmore
** Added license text
** Revision 1.8 2010/10/25 18:22:41EDT jmdagost
** Updated OS_TimerGetInfo() to return a success status.
** Revision 1.7 2010/09/09 15:06:07EDT jmdagost
** Update argument definition for OS_TaskCreate().
** Added dummy functions for OS_TimerGetInfo() and OS_TimerDelete() that currently only return an error.
** Revision 1.6 2009/08/04 13:34:38EDT aschoeni
** Added heap information to ES telemetry
** Revision 1.5 2009/06/25 12:46:02EDT rmcgraw
** DCR8290:5 cFE API changes to unit test files
** Revision 1.4 2009/04/27 10:38:32EDT rmcgraw
** DCR7366:1 Commented out debug messages to prevent flooding output file
** Revision 1.3 2009/04/01 16:12:03EDT rmcgraw
** DCR7366:1 Removed the printout of OS_TaskGetId called - it was flooding the output file
** Revision 1.2 2008/08/15 11:26:56EDT njyanchik
** Check in of ES Unit Test
** Revision 1.1 2008/04/17 13:05:46BST ruperera
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/unit-test/project.pj
** Revision 1.27 2007/09/26 15:00:47EDT njyanchik
** This is the first update of the ES unit tests for 5.0
** Revision 1.26 2007/07/05 15:04:31EDT David Kobe (dlkobe)
** Baseline following complete TBL unit testing
** Revision 1.25 2007/06/04 13:31:57EDT njyanchik
** Update of the TBL unit test
** Revision 1.24 2007/05/30 15:15:23EDT njyanchik
** check in of ES Unit test
** Revision 1.23 2007/05/30 08:56:41EDT njyanchik
** Update
** Revision 1.22 2007/05/25 13:13:43EDT njyanchik
** update of ES unit test
** Revision 1.21 2007/05/25 09:17:57EDT njyanchik
** I added the rmfs call to the OSAL and updated the unit test stubs to match
** Revision 1.20 2007/05/24 15:48:10EDT njyanchik
** Update for ES Unit Test
** Revision 1.19 2007/05/23 10:54:59EDT njyanchik
** Update again to prevent loss of work
** Revision 1.18 2007/05/16 11:14:37EDT njyanchik
** Update ES's unit test driver to match code for build 4.1
** Revision 1.17 2007/05/04 09:10:33EDT njyanchik
** Check in of Time UT and related changes
** Revision 1.15 2007/05/01 13:28:25EDT njyanchik
** I updated the ut stubs to get the each of the subsytems to compile under the unit test. I did not
** change the unit tests themselves to cover more of the files, however.
** Revision 1.14 2007/04/04 09:28:47EDT njyanchik
** This CP updates the unit test for the changes made to the OS (OS_IntLock/OS_IntUnlock) under
** this DCR
** Revision 1.13 2007/03/19 10:49:48EST njyanchik
** the UT needs to be updated because the API for OS_IntEnableAll changed
** Revision 1.12 2007/03/02 15:18:06EST njyanchik
** Jonathan provided me with a newer tbl unit test to check in
** Revision 1.11 2007/01/17 09:26:09GMT-05:00 njyanchik
** Check in of ES Unit Test changed files
** Revision 1.10 2006/11/28 13:01:02GMT-05:00 rjmcgraw
** Changed OS_write stub to return bytes written
** Revision 1.9 2006/11/08 09:55:19EST rjmcgraw
** Added OS_open
** Revision 1.8 2006/07/28 07:33:03EDT njyanchik
** These changes change the osapi functions OS_slose, OS_write, and OS_creat to
** int32 parameters instead of uint32.
** Revision 1.7 2006/07/26 08:08:27EDT rjmcgraw
** Changed name MutSemGivRtn to MutSemGiveRtn
** Revision 1.6 2006/06/08 19:13:59GMT rjmcgraw
** Added support for function UT_SetRtnCode
** Revision 1.5 2006/05/22 19:22:22GMT jjhageman
** Addition of OS_QueueDelete
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_error.h"
#include "osapi.h"
#include "common_types.h"
#include "ut_stubs.h"
#include "osprintf_priv.h"

#ifdef SOCKET_QUEUE
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#endif

/*
** Macro definitions
*/
#define OS_BASE_PORT 43000
#define MAX_PRIORITY 255

#ifdef OS_USE_EMBEDDED_PRINTF
#define UT_OFFSET_OS_PRINTF 2
#define UT_OFFSET_OS_SPRINTF 3
#define UT_OFFSET_OS_SNPRINTF 4

#define UT_BREAK_OS_PRINTF 7
#define UT_BREAK_OS_SPRINTF 6
#define UT_BREAK_OS_SNPRINTF 5

#define UT_SKIP_OS_PRINTF 94
#define UT_SKIP_OS_SPRINTF 22
#define UT_SKIP_OS_SNPRINTF 22
#endif

/*
** External global variables
*/
extern uint32 UT_OS_Fail;
extern char   UT_ReadBuf[];
extern int32  UT_ReadBufOffset;
extern int32  dummy_function(void);
extern char   UT_appname[80];
extern int    UT_lognum;
extern uint32 UT_OS_Fail;
extern uint32 UT_AppID;
extern uint32 UT_LocTimeSec;
extern uint32 UT_LocTimeMSec;
extern uint32 UT_BinSemFail;
extern char   cMsg[];

extern UT_Queue_t UT_Queue[OS_MAX_QUEUES];

extern UT_SetRtn_t MutSemCreateRtn;
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t QueueCreateRtn;
extern UT_SetRtn_t QueueDelRtn;
extern UT_SetRtn_t QueueGetRtn;
extern UT_SetRtn_t QueuePutRtn;
extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t OSReadRtn;
extern UT_SetRtn_t OSReadRtn2;
extern UT_SetRtn_t OS_BinSemCreateRtn;
extern UT_SetRtn_t OSlseekRtn;
extern UT_SetRtn_t CountSemDelRtn;
extern UT_SetRtn_t MutSemDelRtn;
extern UT_SetRtn_t BinSemDelRtn;
extern UT_SetRtn_t BlocksFreeRtn;
extern UT_SetRtn_t UnmountRtn;
extern UT_SetRtn_t RmfsRtn;
extern UT_SetRtn_t ModuleLoadRtn;
extern UT_SetRtn_t ModuleUnloadRtn;
extern UT_SetRtn_t ModuleInfoRtn;
extern UT_SetRtn_t SymbolLookupRtn;
extern UT_SetRtn_t HeapGetInfoRtn;
extern UT_SetRtn_t OSPrintRtn;
extern UT_SetRtn_t OSTaskExitRtn;
extern UT_SetRtn_t OSBinSemTimedWaitRtn;
extern UT_SetRtn_t OSBinSemFlushRtn;
extern UT_SetRtn_t PSPPanicRtn;
extern UT_SetRtn_t OSCloseRtn;
extern UT_SetRtn_t OSTimerGetInfoRtn;

/*
** Global variables
*/
UT_Task_t UT_Task[OS_MAX_TASKS];

#ifdef OS_USE_EMBEDDED_PRINTF
static int gMaxOutputLen = -1;
static int gCurrentOutputLen = 0;

#ifdef OSP_ARINC653
uint32 OS_PrintfMutexId = 0xffffffff;
int OS_printf_break = -1;
int OS_printf_skip = 0;
#endif
#endif

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief OS_BinSemTake stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemTake.  The user can adjust the response by setting
**        the value of UT_BinSemFail, causing it to return a failure
**        indication (-1) on the first or second call to the function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either -1 or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemTake(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemTake called: %lu", sem_id);
    UT_Text(cMsg);
#endif

    /* Enable a failure return on 1st or 2nd second call.  Used to call
     * CFE_TIME_Local1HzTask and get coverage
     */
    if (UT_BinSemFail == 2)
    {
        UT_BinSemFail = 1;
    }
    else
    {
        if (UT_BinSemFail == 1)
        {
            UT_BinSemFail = 0;
            status = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_close stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_close.  The user determines the response by setting the value of
**        UT_OS_Fail prior to this function being called.  The values in the
**        structure OSCloseRtn are updated and subsequently used by the unit
**        tests to determine if the proper response was made to the calling
**        conditions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_ERROR or OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_close(int32 filedes)
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_CLOSE_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_close called: %ld (FAILURE)", filedes);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_close called: %ld (SUCCESS)", filedes);
        UT_Text(cMsg);
    }
#endif

    OSCloseRtn.value = status;
    OSCloseRtn.count++;
    return status;
}

/*****************************************************************************/
/**
** \brief OS_creat stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_creat.  The user determines the response by setting the value of
**        UT_OS_Fail prior to this function being called.  UT_lognum is
**        incremented each time this function is called; if UT_OS_Fail has not
**        been set to OS_CREAT_FAIL then UT_lognum is returned.  Otherwise
**        OS_FS_ERROR is returned.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_ERROR or the value of UT_lognum.
**
******************************************************************************/
/* Open local log file, increment reporting number on each call */
int32 OS_creat(const char *path, int32 access)
{
    int32 status;

    UT_lognum++;

    if (UT_OS_Fail & OS_CREAT_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_creat called: %d; call to open %s (FAILURE)",
                 UT_lognum, path);
        UT_Text(cMsg);
#endif
    }
    else
    {
        status = UT_lognum;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_creat called: %d; call to open %s (SUCCESS)",
                 UT_lognum, path);
        UT_Text(cMsg);
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_IntLock stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_IntLock.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_IntLock(void)
{
#if 0
#ifndef _ix86_
#error "IntDisableAll unit test stub may not work for your OS"
#endif
#endif

    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_IntUnlock stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_IntUnlock.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_IntUnlock(int32 IntLevel)
{
#if 0
#ifndef _ix86_
#error "IntEnableAll unit test stub may not work for your OS"
#endif
#endif

    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemCpy stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_MemCpy.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemCpy(void *dst, void *src, uint32 size)
{
    memcpy(dst, src, size);
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_MemSet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_MemSet.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_PSP_MemSet(void *dst, uint8 value , uint32 size)
{
    memset(dst, (int)value, (size_t)size);
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_MutSemGive stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemGive.  The user can adjust the response by setting
**        the values in the MutSemGiveRtn structure prior to this function
**        being called.  If the value MutSemGiveRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemGiveRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemGive(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemGiveRtn.count > 0)
    {
        MutSemGiveRtn.count--;

        if (MutSemGiveRtn.count == 0)
        {
            status = MutSemGiveRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemTake stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemTake.  The user can adjust the response by setting
**        the values in the MutSemTakeRtn structure prior to this function
**        being called.  If the value MutSemTakeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemTakeRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemTake(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemTakeRtn.count > 0)
    {
        MutSemTakeRtn.count--;

        if (MutSemTakeRtn.count == 0)
        {
            status = MutSemTakeRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_printf stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_printf.  The user can adjust the response by setting the value of
**        OSPrintRtn.count prior to calling the function.  If count is not
**        negative then the value OSPrintRtn.value is incremented by a fixed
**        amount and OSPrintRtn.count is incremented.  If count is negative the
**        print string is compared to known responses and if a match is found
**        the value OSPrintRtn.value is incremented by a specified constant
**        value.  OSPrintRtn.count is decremented to keep track of the number
**        of messages logged.  The unit test code compares the OSPrintRtn value
**        and count variables against expected totals to ensure the proper
**        response of other functions being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_printf(const char *string, ...)
{
    char    tmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE * 2];
    char    *start;
    char    dash = '-';
    VA_LIST ptr;

    VA_START(ptr, string, UT_OFFSET_OS_PRINTF, UT_BREAK_OS_PRINTF,
             UT_SKIP_OS_PRINTF);
    VSNPRINTF(tmpString, CFE_ES_MAX_SYSLOG_MSG_SIZE * 2, string, ptr);
    VA_END(ptr);

#ifdef UT_VERBOSE
    UT_Text(tmpString);
#endif

    if (OSPrintRtn.count >= 0)
    {
        OSPrintRtn.value += OS_PRINT_INCR;
        OSPrintRtn.count++;
    }
    else
    {
        OSPrintRtn.count--;

        if ((start = strchr(tmpString, dash)))
        {
            start += 2; /* Skip dash and space */

            if (strcmp(start, "CFE_ES_ExitApp: CORE Application CFE_ES Had an "
                       "Init Error.\n") == 0)
            {
                OSPrintRtn.value += 11;
            }
            else if (strcmp(start, "POWER ON RESET due to max proc resets "
                            "(Commanded).\n") == 0) 
            {
                OSPrintRtn.value += 12;
            }
            else if (strcmp(start, "CFE_ES_ExitApp: CORE Application CFE_ES"
                            " Had a Runtime Error.\n") == 0)
            {
                OSPrintRtn.value += 13;
            }
            else if (strcmp(start, "CFE_ES_ExitApp, Cannot Exit CORE "
                            "Application CFE_ES\n") == 0)
            {
                OSPrintRtn.value += 14;
            }
            else if (strcmp(start, "PROCESSOR RESET called from CFE_ES_ResetCFE "
                            "(Commanded).\n") == 0) 
            {
                OSPrintRtn.value += 15;
            }
            else if (strcmp(start, "POWERON RESET called from CFE_ES_ResetCFE "
                            "(Commanded).\n") == 0)
            {
                OSPrintRtn.value += 16;
            }

            else if (strcmp(start, "CFE_ES_ExitChildTask Error: Cannot Call "
                            "from a cFE App Main Task. ID = 1\n") == 0)
            {
                OSPrintRtn.value += 17;
            }
            else if (strcmp(start, "CFE_ES_ExitChildTask Error Calling "
                                   "CFE_ES_GetAppID. Task ID = 1, RC = "
                                   "0xC4000001\n") == 0 ||
                     strcmp(start, "CFE_ES_ExitChildTask Error Calling "
                                   "CFE_ES_GetAppID. Task ID = 1, RC = "
                                   "0xc4000001\n") == 0)
            {
                OSPrintRtn.value += 18;
            }

            else if (strcmp(start, "ES SharedData Mutex Take Err Stat=0x"
                            "ffffffff,App=1,Func=TestAPI,Line=12345\n") == 0 ||
                     strcmp(start, "ES SharedData Mutex Take Err Stat=0x"
                            "FFFFFFFF,App=1,Func=TestAPI,Line=12345\n") == 0)
            {
                OSPrintRtn.value += 19;
            }
            else if (strcmp(start, "ES SharedData Mutex Give Err Stat=0x"
                            "ffffffff,App=1,Func=TestAPI,Line=98765\n") == 0 ||
                     strcmp(start, "ES SharedData Mutex Give Err Stat=0x"
                            "FFFFFFFF,App=1,Func=TestAPI,Line=98765\n") == 0)
            {
                OSPrintRtn.value += 20;
            }

            else if (strcmp(start, "CFE_ES_RestartApp: Cannot Restart "
                            "Application appName, It is not running.\n") == 0)
            {
                OSPrintRtn.value += 21;
            }

            else if (strcmp(start, "ES Startup: Insufficent Free Space on "
                            "Volatile Disk, Reformatting.\n") == 0)
            {
                OSPrintRtn.value += 22;
            }
            else if (strcmp(start, "ES Startup: Error Creating Volatile(RAM) "
                            "Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Creating Volatile(RAM) "
                            "Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 23;
            }
            else if (strcmp(start, "ES Startup: Error Initializing Volatile"
                            "(RAM) Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Initializing Volatile"
                            "(RAM) Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 24;
            }
            else if (strcmp(start, "ES Startup: Error Mounting Volatile(RAM) "
                            "Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Mounting Volatile(RAM) "
                            "Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 25;
            }
            else if (strcmp(start, "ES Startup: Error Re-Mounting Volatile"
                            "(RAM) Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Re-Mounting Volatile"
                            "(RAM) Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 26;
            }
            else if (strcmp(start, "ES Startup: Error Re-Formating Volatile"
                            "(RAM) Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Re-Formating Volatile"
                            "(RAM) Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 27;
            }
            else if (strcmp(start, "ES Startup: Error Removing Volatile"
                            "(RAM) Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Removing Volatile"
                            "(RAM) Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 28;
            }
            else if (strcmp(start, "ES Startup: Error Un-Mounting Volatile"
                            "(RAM) Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Un-Mounting Volatile"
                            "(RAM) Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 29;
            }
            else if (strcmp(start, "ES Startup: Error Determining Blocks Free "
                            "on Volume. EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Determining Blocks Free "
                            "on Volume. EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 30;
            }

            else if (strcmp(start, "ES Startup: OS_TaskCreate error creating "
                            "core App: CFE_TBL: EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: OS_TaskCreate error creating "
                            "core App: CFE_TBL: EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 31;
            }
            else if (strcmp(start, "ES Startup: CFE_ES_Global.TaskTable record"
                            " used error for App: CFE_EVS, "
                            "continuing.\n") == 0)
            {
                OSPrintRtn.value += 32;
            }
            else if (strcmp(start, "ES Startup: Error, No free application "
                            "slots available for CORE App!\n") == 0)
            {
                OSPrintRtn.value += 33;
            }
            else if (strcmp(start, "ES Startup: Error returned when "
                            "calling function: CFE_TBL_EarlyInit: EC = "
                            "0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error returned when "
                            "calling function: CFE_TBL_EarlyInit: EC = "
                            "0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 34;
            }
            else if (strcmp(start, "ES Startup: bad function pointer ( table "
                            "entry = 1).\n") == 0)
            {
                OSPrintRtn.value += 35;
            }

            else if (strcmp(start, "ES Startup: ES Startup File Line is too "
                            "long: 137 bytes.\n") == 0)
            {
                OSPrintRtn.value += 36;
            }
            else if (strcmp(start, "ES Startup: Error Reading Startup file. "
                            "EC = 0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error Reading Startup file. "
                            "EC = 0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 37;
            }
            else if (strcmp(start, "ES Startup: Error, Can't Open ES App "
                            "Startup file: /cf/apps/cfe_es_startup.scr EC = "
                            "0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES Startup: Error, Can't Open ES App "
                            "Startup file: /cf/apps/cfe_es_startup.scr EC = "
                            "0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 38;
            }
            else if (strcmp(start, "ES Startup: Opened ES App Startup file: "
                            "/ram/apps/cfe_es_startup.scr\n") == 0)
            {
                OSPrintRtn.value += 39;
            }

            else if (strcmp(start, "ES:Error reading cmd pipe,RC="
                            "0xFFFFFFFF\n") == 0 ||
                     strcmp(start, "ES:Error reading cmd pipe,RC="
                            "0xffffffff\n") == 0)
            {
                OSPrintRtn.value += 40;
            }
            else if (strcmp(start, "ES:Application Init Failed,RC="
                            "0xC4000017\n") == 0 ||
                     strcmp(start, "ES:Application Init Failed,RC="
                            "0xc4000017\n") == 0)
            {
                OSPrintRtn.value += 41;
            }

            else if (strcmp(start, "ES:Call to CFE_ES_RegisterApp Failed, RC ="
                            " 0xC4000017\n") == 0 ||
                     strcmp(start, "ES:Call to CFE_ES_RegisterApp Failed, RC ="
                            " 0xc4000017\n") == 0)
            {
                OSPrintRtn.value += 42;
            }

            else if (strcmp(start, "ES Startup: Load Shared Library Init "
                            "Error.\n") == 0)
            {
                OSPrintRtn.value += 43;
            }
            else if (strcmp(start, "ES Startup: Unable to decompress library "
                            "file: /cf/apps/tst_lib.bundle.gz\n") == 0)
            {
                OSPrintRtn.value += 44;
            }
            else if (strcmp(start, "ES Startup: Unable to extract filename "
                            "from path: /cf/apps/tst_lib.bundle.gz.\n") == 0)
            {
                OSPrintRtn.value += 45;
            }
            else if (strncmp(start, "ES Startup: Unable to extract filename "
                            "from path: /cf/apps/this_is_a_filename_that_"
                            "exceeds_the_maximum_allowed", 110) == 0)
            {
                OSPrintRtn.value += 46;
            }
            else if (strcmp(start, "ES Startup: Library path plus file name "
                            "length (68) exceeds max allowed (64)\n") == 0)
            {
                OSPrintRtn.value += 47;
            }
            else if (strcmp(start, "ES Startup: Could not load cFE Shared "
                            "Library\n") == 0)
            {
                OSPrintRtn.value += 48;
            }
            else if (strcmp(start, "ES Startup: Could not find Library Init "
                            "symbol:TST_LIB_Init. EC = 0xffffffff\n") == 0 ||
                     strcmp(start, "ES Startup: Could not find Library Init "
                            "symbol:TST_LIB_Init. EC = 0xFFFFFFFF\n") == 0)
            {
                OSPrintRtn.value += 49;
            }
            else if (strcmp(start, "ES Startup: No free library slots "
                            "available\n") == 0)
            {
                OSPrintRtn.value += 50;
            }

            else if (strcmp(start, "ES Startup: AppCreate Error: TaskCreate "
                            "AppName Failed. EC = 0xffffffff!\n") == 0 ||
                     strcmp(start, "ES Startup: AppCreate Error: TaskCreate "
                            "AppName Failed. EC = 0xFFFFFFFF!\n") == 0)
            {
                OSPrintRtn.value += 51;
            }
            else if (strcmp(start, "ES Startup: Unable to decompress "
                            "Application File: ut/filename.gz\n") == 0)
            {
                OSPrintRtn.value += 52;
            }
            else if (strcmp(start, "ES Startup: Error: ES_TaskTable slot in "
                            "use at task creation!\n") == 0)
            {
                OSPrintRtn.value += 53;
            }
            else if (strcmp(start, "ES Startup: Unable to extract filename "
                            "from path: ut/filename.gz.\n") == 0)
            {
                OSPrintRtn.value += 54;
            }
            else if (strcmp(start, "ES Startup: Could not load cFE application"
                            " file:ut/filename.x. EC = 0xffffffff\n") == 0 ||
                     strcmp(start, "ES Startup: Could not load cFE application"
                            " file:ut/filename.x. EC = 0xFFFFFFFF\n") == 0)
            {
                OSPrintRtn.value += 55;
            }
            else if (strcmp(start, "ES Startup: Could not find symbol:"
                            "EntryPoint. EC = 0xffffffff\n") == 0 ||
                     strcmp(start, "ES Startup: Could not find symbol:"
                            "EntryPoint. EC = 0xFFFFFFFF\n") == 0)
            {
                OSPrintRtn.value += 56;
            }
            else if (strncmp(start, "ES Startup: Unable to extract filename "
                             "from path: ut/this_is_a_filename_that_exceeds_"
                             "the_maximum_allowed", 104) == 0)
            {
                OSPrintRtn.value += 57;
            }
            else if (strcmp(start, "ES Startup: Application path plus file "
                            "name length (68) exceeds max allowed "
                            "(64)\n") == 0)
            {
                OSPrintRtn.value += 58;
            }
        }
    }
}

#ifdef SOCKET_QUEUE
/*****************************************************************************/
/**
** \brief OS_QueueCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueCreate.  The user can adjust the response by setting
**        the values in the QueueCreateRtn structure prior to this function
**        being called.  If the value QueueCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueCreateRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        creating the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to the real call, doesn't care about creator or do any
**           mutex locking\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER,
**        OS_ERR_NAME_TOO_LONG, OS_ERR_NO_FREE_IDS, OS_ERR_NAME_TAKEN,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueCreate(uint32 *queue_id, const char *queue_name,
                     uint32 queue_depth, uint32 data_size, uint32 flags)
{
    int                tmpSkt;
    int                returnStat;
    struct sockaddr_in servaddr;
    int                i;
    uint32             possible_qid;
    int32              status = OS_SUCCESS;
    boolean            flag = FALSE;

    if (QueueCreateRtn.count > 0)
    {
        QueueCreateRtn.count--;

        if (QueueCreateRtn.count == 0)
        {
            status = QueueCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (queue_id == NULL || queue_name == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        /* Don't want to allow names too long; if truncated, two names might
         * be the same
         */
        else if (strlen(queue_name) >= OS_MAX_API_NAME)
        {
            status = OS_ERR_NAME_TOO_LONG;
        }
        else
        {
            /* Look for open queue */
            for (possible_qid = 0; possible_qid < OS_MAX_QUEUES;
                 possible_qid++)
            {
                if (UT_Queue[possible_qid].free == TRUE)
                {
                    break;
                }
            }

            if (possible_qid >= OS_MAX_QUEUES ||
                UT_Queue[possible_qid].free != TRUE)
            {
                status = OS_ERR_NO_FREE_IDS;
            }
            else
            {
                /* See if name is already taken */
                for (i = 0; i < OS_MAX_QUEUES; i++)
                {
                    if (strcmp((char*) queue_name, UT_Queue[i].name) == 0)
                    {
                        status = OS_ERR_NAME_TAKEN;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    tmpSkt = socket(AF_INET, SOCK_DGRAM, 0);
                    memset(&servaddr, 0, sizeof(servaddr));
                    servaddr.sin_family = AF_INET;
                    servaddr.sin_port = htons(OS_BASE_PORT + possible_qid);
                    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

                    /* Bind the input socket to a pipe; port numbers are
                     * OS_BASE_PORT + queue_id
                     */
                    returnStat = bind(tmpSkt, (struct sockaddr *) &servaddr,
                                      sizeof(servaddr));

                    if (returnStat == -1)
                    {
                        status = OS_ERROR;
#ifdef UT_VERBOSE
                        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                                 "  Bind failed on OS_QueueCreate, errno = %d",
                                 errno);
                        UT_Text(cMsg);
#endif
                    }
                    else
                    {
                        /* Store socket handle */
                        *queue_id = possible_qid;
                        UT_Queue[*queue_id].id = tmpSkt;
                        UT_Queue[*queue_id].free = FALSE;
                        strncpy(UT_Queue[*queue_id].name, (char*) queue_name,
                                OS_MAX_API_NAME);
                        UT_Queue[*queue_id].name[OS_MAX_API_NAME - 1] = '\0';
                    }
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueDelete.  The user can adjust the response by setting
**        the values in the QueueDelRtn structure prior to this function
**        being called.  If the value QueueDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueDelRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        deleting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to real code without mutex locking\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueDelete(uint32 queue_id)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueDelRtn.count > 0)
    {
        QueueDelRtn.count--;

        if (QueueDelRtn.count == 0)
        {
            status = QueueDelRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check to see if the queue_id given is valid */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        /* Try to delete the queue */
        else if (close(UT_Queue[queue_id].id) != 0)
        {
            status = OS_ERROR;
        }
        /* Now that the queue is deleted, remove its "presence"
         * in OS_message_q_table and OS_message_q_name_table
         */
        else
        {
            UT_Queue[queue_id].free = TRUE;
            strcpy(UT_Queue[queue_id].name, "");
            UT_Queue[queue_id].id = 0;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueGet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGet.  The user can adjust the response by setting
**        the values in the QueueGetRtn structure prior to this function
**        being called.  If the value QueueGetRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueGetRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        getting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Works similar to real function; note that pend on empty queue
**           doesn't block\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_EMPTY, OS_QUEUE_INVALID_SIZE,
**        OS_QUEUE_TIMEOUT, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGet(uint32 queue_id, void *data, uint32 size,
                  uint32 *size_copied, int32 timeout)
{
    int     sizeCopied;
    int     cntlFlags;
    int     timeloop;
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueGetRtn.count > 0)
    {
        QueueGetRtn.count--;

        if (QueueGetRtn.count == 0)
        {
            status = QueueGetRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
            flag = TRUE;
        }
        else
        {
            if ((data == NULL) || (size_copied == NULL))
            {
                status = OS_INVALID_POINTER;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            /* Read the socket for data */
            if (timeout == OS_PEND)
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags|O_NONBLOCK);
                sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                      NULL, NULL);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);

                if (sizeCopied == -1 && errno == EWOULDBLOCK)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
#ifdef UT_VERBOSE
                    UT_Text("WARNING: OS_QueueGet called as pend on an "
                            "empty queue;");
                    UT_Text(" normally would block but in UT environment "
                            "isn't valid");
#endif
                }
                else if (sizeCopied != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
            }
            else if (timeout == OS_CHECK)
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags|O_NONBLOCK);
                sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                      NULL, NULL);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);

                if (sizeCopied == -1 && errno == EWOULDBLOCK)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
                }
                else if (sizeCopied != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }

            }
            else /* Timeout */
            {
                cntlFlags = fcntl(UT_Queue[queue_id].id, F_GETFL, 0);
                fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags | O_NONBLOCK);

                /* This "timeout" will check the socket for data every 100
                 * milliseconds up until the timeout value. Although this works
                 * fine for a desktop environment, it should be written more
                 * efficiently for a flight system.  The proper way will be to
                 * use select or poll with a timeout
                 */
                for (timeloop = timeout; timeloop > 0;
                     timeloop = timeloop - 100)
                {
                    sizeCopied = recvfrom(UT_Queue[queue_id].id, data, size, 0,
                                          NULL, NULL);

                    if (sizeCopied == size)
                    {
                        *size_copied = sizeCopied;
                        fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                        status = OS_SUCCESS;
                        flag = TRUE;
                        break;
                    }
                    else if (sizeCopied == -1 && errno == EWOULDBLOCK)
                    {
                        /* Sleep for 100 milliseconds */
                        usleep(100 * 1000);
                    }
                    else
                    {
                        *size_copied = 0;
                        fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                        status = OS_QUEUE_INVALID_SIZE;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    fcntl(UT_Queue[queue_id].id, F_SETFL, cntlFlags);
                    status = OS_QUEUE_TIMEOUT;
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueuePut stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueuePut.  The user can adjust the response by setting
**        the values in the QueuePutRtn structure prior to this function
**        being called.  If the value QueuePutRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueuePutRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        putting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Same as real function\n
**        2. Uses socket calls for queue
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_FULL, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
{
    struct     sockaddr_in serva;
    static int socketFlags = 0;
    int        bytesSent = 0;
    int        tempSkt = 0;
    int32      status = OS_SUCCESS;
    boolean    flag = FALSE;

    if (QueuePutRtn.count > 0)
    {
        QueuePutRtn.count--;

        if (QueuePutRtn.count == 0)
        {
            status = QueuePutRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        else if (data == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        else
        {
            /* Specify the IP address and port number of destination */
            memset(&serva, 0, sizeof(serva));
            serva.sin_family = AF_INET;
            serva.sin_port = htons(OS_BASE_PORT + queue_id);
            serva.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

            /* Open a temporary socket to transfer the packet to MR */
            tempSkt = socket(AF_INET, SOCK_DGRAM, 0);

            /* Send the packet to the message router task (MR) */
            bytesSent = sendto(tempSkt, (char *) data, size, socketFlags,
                               (struct sockaddr *) &serva, sizeof(serva));

            if (bytesSent != size)
            {
               status = OS_QUEUE_FULL;
            }
            else
            {
                /* Close socket */
                close(tempSkt);
            }
        }
    }

    return status;
}
#else /* Message queue without using sockets */
/*****************************************************************************/
/**
** \brief OS_QueueCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueCreate.  The user can adjust the response by setting
**        the values in the QueueCreateRtn structure prior to this function
**        being called.  If the value QueueCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueCreateRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        creating the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to the real call, doesn't care about creator or do any
**           mutex locking\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER,
**        OS_ERR_NAME_TOO_LONG, OS_ERR_NO_FREE_IDS, OS_ERR_NAME_TAKEN,
**        or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueCreate(uint32 *queue_id,
                     const char *queue_name,
                     uint32 queue_depth,
                     uint32 data_size,
                     uint32 flags)
{
    int     i;
    uint32  possible_qid;
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueCreateRtn.count > 0)
    {
        QueueCreateRtn.count--;

        if (QueueCreateRtn.count == 0)
        {
            status = QueueCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (queue_id == NULL || queue_name == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        /* Don't want to allow names too long; if truncated, two names might
         * be the same
         */
        else if (strlen(queue_name) >= OS_MAX_API_NAME)
        {
            status = OS_ERR_NAME_TOO_LONG;
        }
        else
        {
            /* Look for open queue */
            for (possible_qid = 0; possible_qid < OS_MAX_QUEUES;
                 possible_qid++)
            {
                if (UT_Queue[possible_qid].free == TRUE)
                {
                    break;
                }
            }

            if (possible_qid >= OS_MAX_QUEUES ||
                UT_Queue[possible_qid].free != TRUE)
            {
                status = OS_ERR_NO_FREE_IDS;
            }
            else
            {
                /* See if name is already taken */
                for (i = 0; i < OS_MAX_QUEUES; i++)
                {
                    if (strcmp((char*) queue_name, UT_Queue[i].name) == 0)
                    {
                        status = OS_ERR_NAME_TAKEN;
                        flag = TRUE;
                        break;
                    }
                }

                if (flag == FALSE)
                {
                    *queue_id = possible_qid;
                    UT_Queue[*queue_id].id = possible_qid;
                    UT_Queue[*queue_id].free = FALSE;
                    strncpy(UT_Queue[*queue_id].name, (char*) queue_name,
                            OS_MAX_API_NAME);
                    UT_Queue[*queue_id].name[OS_MAX_API_NAME - 1] = '\0';
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueDelete.  The user can adjust the response by setting
**        the values in the QueueDelRtn structure prior to this function
**        being called.  If the value QueueDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueDelRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        deleting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Similar to real code without mutex locking\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueDelete(uint32 queue_id)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueDelRtn.count > 0)
    {
        QueueDelRtn.count--;

        if (QueueDelRtn.count == 0)
        {
            status = QueueDelRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check to see if the queue_id given is valid */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        /* Now that the queue is deleted, remove its "presence"
         * in OS_message_q_table and OS_message_q_name_table
         */
        else
        {
            UT_Queue[queue_id].free = TRUE;
            strcpy(UT_Queue[queue_id].name, "");
            UT_Queue[queue_id].id = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueueGet stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGet.  The user can adjust the response by setting
**        the values in the QueueGetRtn structure prior to this function
**        being called.  If the value QueueGetRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueueGetRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        getting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Works similar to real function; note that pend on empty queue
**           doesn't block\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_EMPTY, OS_QUEUE_INVALID_SIZE,
**        or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGet(uint32 queue_id,
                  void *data,
                  uint32 size,
                  uint32 *size_copied,
                  int32 timeout)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueueGetRtn.count > 0)
    {
        QueueGetRtn.count--;

        if (QueueGetRtn.count == 0)
        {
            status = QueueGetRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
            flag = TRUE;
        }
        else
        {
            if ((data == NULL) || (size_copied == NULL))
            {
                status = OS_INVALID_POINTER;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            if (timeout == OS_PEND)
            {
                memcpy(data, UT_Queue[queue_id].data, size);

                if (UT_Queue[queue_id].size == 0)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
#ifdef UT_VERBOSE
                    UT_Text("WARNING: OS_QueueGet called as pend on an "
                            "empty queue;");
                    UT_Text(" normally would block but in UT environment "
                            "isn't valid");
#endif
                }
                else if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    UT_Queue[queue_id].size = 0;
                }
            }
            else if (timeout == OS_CHECK)
            {
                memcpy(data, UT_Queue[queue_id].data, size);

                if (UT_Queue[queue_id].size == 0)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_EMPTY;
                }
                else if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                    status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    UT_Queue[queue_id].size = 0;
                }
            }
            else /* Timeout */
            {
                if (UT_Queue[queue_id].size != size)
                {
                    *size_copied = 0;
                     status = OS_QUEUE_INVALID_SIZE;
                }
                else
                {
                    *size_copied = UT_Queue[queue_id].size;
                    memcpy(data, UT_Queue[queue_id].data, size);
                    UT_Queue[queue_id].size = 0;
                }
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_QueuePut stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueuePut.  The user can adjust the response by setting
**        the values in the QueuePutRtn structure prior to this function
**        being called.  If the value QueuePutRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value QueuePutRtn.value.
**        Otherwise the return value is dependent on success or failure in
**        putting the queue.
**
** \par Assumptions, External Events, and Notes:
**        1. Same as real function\n
**        2. Emulates socket queue, without use of sockets
**
** \returns
**        Returns either a user-defined status flag, OS_ERR_INVALID_ID,
**        OS_INVALID_POINTER, OS_QUEUE_FULL, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueuePut(uint32 queue_id, void *data, uint32 size, uint32 flags)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (QueuePutRtn.count > 0)
    {
        QueuePutRtn.count--;

        if (QueuePutRtn.count == 0)
        {
            status = QueuePutRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        /* Check parameters */
        if (queue_id >= OS_MAX_QUEUES || UT_Queue[queue_id].free == TRUE)
        {
            status = OS_ERR_INVALID_ID;
        }
        else if (data == NULL)
        {
            status = OS_INVALID_POINTER;
        }
        else
        {
            UT_Queue[queue_id].size = size;
            memcpy(UT_Queue[queue_id].data, data, size);
        }
    }

    return status;
}
#endif /* End SOCKET_QUEUE */

/*****************************************************************************/
/**
** \brief OS_remove stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_remove.  It always returns 0.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 0.
**
******************************************************************************/
int32 OS_remove(const char *path)
{
#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH, "  OS_remove called: %s", path);
    UT_Text(cMsg);
#endif
    return 0;
}

/*****************************************************************************/
/**
** \brief OS_write stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_write.  The user can adjust the response by setting the values
**        in the FileWriteRtn structure prior to this function being called.
**        If the value FileWriteRtn.count is greater than zero then the counter
**        is decremented; if it then equals zero the return value is set to the
**        user-defined value FileWriteRtn.value.  Alternately, the user can
**        cause the function to return a failure result, OS_FS_ERROR, by
**        setting the value of UT_OS_Fail to OS_WRITE_FAIL prior to this
**        function being called.  The input value, nbytes, is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        Only outputs SUCCESS or FAILURE since packet structure isn't known
**
** \returns
**        Returns either a user-defined status flag, OS_FS_ERROR, or the value
**        of the input variable, nbytes.
**
******************************************************************************/
int32 OS_write(int32 filedes, void *buffer, uint32 nbytes)
{
    int32   status;
    boolean flag = FALSE;

    if (FileWriteRtn.count > 0)
    {
        FileWriteRtn.count--;

        if (FileWriteRtn.count == 0)
        {
            status = FileWriteRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_WRITE_FAIL)
        {
            status = OS_FS_ERROR;
#ifdef UT_VERBOSE
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_write called: %lu (FAILURE)", filedes);
            UT_Text(cMsg);
#endif
        }
        else
        {
            status = nbytes;
#ifdef UT_VERBOSE
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_write called: %lu (SUCCESS)", filedes);
            UT_Text(cMsg);
#endif
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemFlush stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemFlush.  The variable OSBinSemFlushRtn.value is set to the
**        value passed to the function, reset_type, and the variable
**        OSBinSemFlushRtn.count is incremented each time this function is
**        called.  The unit tests compare these values to expected results to
**        verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemFlush(uint32 sem_id)
{
#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemFlush called: %lu", sem_id);
    UT_Text(cMsg);
#endif
    OSBinSemFlushRtn.value = sem_id;
    OSBinSemFlushRtn.count++;
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_TaskRegister stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskRegister.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskRegister(void)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKREGISTER_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskRegister called (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskRegister called (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskCreate.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskCreate(uint32 *task_id, const char *task_name,
                    osal_task_entry function_pointer,
                    const uint32 *stack_pointer,
                    uint32 stack_size, uint32 priority,
                    uint32 flags)
{
    int32 status = OS_SUCCESS;

    *task_id = 1;

    if (UT_OS_Fail & OS_TASKCREATE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_TaskCreate called: %s%lu (FAILURE)",
                 task_name, *task_id);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_TaskCreate called: %s%lu (SUCCESS)",
                 task_name, *task_id);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskGetId stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_TaskGetId.  It always returns 1.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 1.
**
******************************************************************************/
uint32 OS_TaskGetId(void)
{
#ifdef UT_VERBOSE
    UT_Text("  OS_TaskGetId called");
#endif
    return 1;
}

/*****************************************************************************/
/**
** \brief OS_TaskGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskGetInfo.  If the input structure, task_prop, is null, it
**        returns OS_INVALID_POINTER.  Otherwise it sets the task structure
**        variables to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_INVALID_POINTER or OS_SUCCESS.
**
******************************************************************************/
int32 OS_TaskGetInfo(uint32 task_id, OS_task_prop_t *task_prop)
{
    int32 status = OS_SUCCESS;

    if (task_prop == NULL)
    {
        status = OS_INVALID_POINTER;
    }
    else
    {
        task_prop->creator = 0;
        task_prop->OStask_id = OS_MAX_TASKS - 1;
        task_prop->stack_size = UT_Task[task_id].stack_size;
        task_prop->priority = UT_Task[task_id].priority;
        strncpy(task_prop->name, UT_Task[task_id].name, OS_MAX_API_NAME + 1);
        task_prop->name[OS_MAX_API_NAME] = '\0';
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_mkfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_mkfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
int32 OS_mkfs(char *address, char *devname, char * volname, uint32 blocksize,
              uint32 numblocks)
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_MKFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mkfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mkfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_rmfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_rmfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_FS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
int32 OS_rmfs(char *devname)
{
    int32 status = OS_FS_SUCCESS;

    if (UT_OS_Fail & OS_RMFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_rmfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_rmfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_mount stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_mount.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
int32 OS_mount(const char *devname, char* mountpoint)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_MOUNT_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mount called: devname=%s, mountpoint=%s (FAILURE)",
                 devname, mountpoint);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_mount called: devname=%s, mountpoint=%s (SUCCESS)",
                 devname, mountpoint);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_initfs stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_initfs.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_FS_ERROR.
**
******************************************************************************/
int32 OS_initfs(char *address, char *devname, char *volname,
                uint32 blocksize, uint32 numblocks)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_INITFS_FAIL)
    {
        status = OS_FS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_initfs called: %s (FAILURE)", devname);
        UT_Text(cMsg);
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_initfs called: %s (SUCCESS)", devname);
        UT_Text(cMsg);
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ShellOutputToFile stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_ShellOutputToFile.  It always returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_ShellOutputToFile(char* Cmd, int32 OS_fd)
{
#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
            "  OS_ShellOutput to file called: %s", Cmd);
    UT_Text(cMsg);
#endif
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_read stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_read.  The user can adjust the response by setting the values
**        in the OSReadRtn structure prior to this function being called.  If
**        the value OSReadRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value OSReadRtn.value.  Alternately, the user can adjust
**        the response by setting the values in the OSReadRtn2 structure prior
**        to this function being called.  If the value OSReadRtn2.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        OSReadRtn2.value.  Finally, the user can cause the function to return
**        a failure result, nbytes - 1, by setting the value of UT_OS_Fail to
**        OS_READ_FAIL prior to this function being called.  The input value,
**        nbytes, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either one of two user-defined status flags, the value
**        of the input variable, nbytes, - 1 (failure case), or the value of
**        the input variable, nbytes (success case).
**
******************************************************************************/
int32 OS_read(int32 filedes, void *buffer, uint32 nbytes)
{
    int32   status;
    boolean flag = FALSE;

    if (OSReadRtn.count > 0)
    {
        OSReadRtn.count--;

        if (OSReadRtn.count == 0)
        {
            status = OSReadRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (OSReadRtn2.count > 0)
        {
            OSReadRtn2.count--;

            if (OSReadRtn2.count == 0)
            {
                status = OSReadRtn2.value;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            if (UT_OS_Fail & OS_READ_FAIL)
            {
                status = nbytes - 1;
            }
            else
            {
                memcpy(buffer, &UT_ReadBuf[UT_ReadBufOffset], nbytes);
                UT_ReadBufOffset += nbytes;
                status = nbytes;
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_lseek stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_lseek.  The user can adjust the response by setting the values
**        in the OSlseekRtn structure prior to this function being called.  If
**        the value OSlseekRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value OSlseekRtn.value.  Alternately, the user can cause
**        the function to return a failure result, OS_FS_ERROR, by setting the
**        value of UT_OS_Fail to OS_LSEEK_FAIL prior to this function being
**        called.  The input value, offset, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_FS_ERROR, or the value
**        of the input variable, offset.
**
******************************************************************************/
int32 OS_lseek(int32 filedes, int32 offset, uint32 whence)
{
    int32   status;
    boolean flag = FALSE;

    status = offset;

    if (OSlseekRtn.count > 0)
    {
        OSlseekRtn.count--;

        if (OSlseekRtn.count == 0)
        {
            status = OSlseekRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_LSEEK_FAIL)
        {
            status = OS_FS_ERROR;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemCreate.  The user can adjust the response by setting the
**        values in the OS_BinSemCreateRtn structure prior to this function
**        being called.  If the value OS_BinSemCreateRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        OS_BinSemCreateRtn.value.  Alternately, the user can cause the
**        function to return a failure result, OS_ERROR, by setting the value
**        of UT_OS_Fail to OS_SEMCREATE_FAIL prior to this function being
**        called.  OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemCreate(uint32 *sem_id, const char *sem_name,
                      uint32 sem_initial_value, uint32 options)
{
    int32      status = OS_SUCCESS;
    static int sem_ctr = 0;
#ifdef UT_VERBOSE
    boolean    flag = FALSE;
#endif

    *sem_id = sem_ctr;
    sem_ctr++;

    if (UT_OS_Fail & OS_SEMCREATE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                 "  OS_BinSemCreate called: %d (FAILURE)", sem_ctr);
        UT_Text(cMsg);
#endif
    }
    else
    {
        if (OS_BinSemCreateRtn.count > 0)
        {
            OS_BinSemCreateRtn.count--;

            if (OS_BinSemCreateRtn.count == 0)
            {
                status = OS_BinSemCreateRtn.value;
#ifdef UT_VERBOSE
                flag = TRUE;
#endif
            }
        }

#ifdef UT_VERBOSE
        if (flag == FALSE)
        {
            SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
                     "  OS_BinSemCreate called: %d (SUCCESS)", sem_ctr);
            UT_Text(cMsg);
        }
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_MutSemCreate stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemCreate.  The user can adjust the response by setting the
**        values in the MutSemCreateRtn structure prior to this function being
**        called.  If the value MutSemCreateRtn.count is greater than zero
**        then the counter is decremented; if it then equals zero the return
**        value is set to the user-defined value MutSemCreateRtn.value.
**        Alternately, the user can cause the function to return a failure
**        result, OS_ERROR, by setting the value of UT_OS_Fail to
**        OS_MUTCREATE_FAIL prior to this function being called.  OS_SUCCESS
**        is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_ERROR, or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemCreate(uint32 *sem_id, const char *sem_name, uint32 options)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (MutSemCreateRtn.count > 0)
    {
        MutSemCreateRtn.count--;

        if (MutSemCreateRtn.count == 0)
        {
            status = MutSemCreateRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        if (UT_OS_Fail & OS_MUTCREATE_FAIL)
        {
            status = OS_ERROR;
#ifdef UT_VERBOSE
            UT_Text("  OS_MutSemCreate called: (FAILURE)");
#endif
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_open stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_open.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_ERROR or a value that is incremented by 1 each
**        time the function is called.
**
******************************************************************************/
int32 OS_open(const char *path, int32 access, uint32 mode)
{
    int32      status;
    static int NumOpen = 1;

    if (UT_OS_Fail & OS_OPEN_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_open called: (FAILURE)");
#endif
    }
    else
    {
        status = NumOpen;
        NumOpen++;
#ifdef UT_VERBOSE
        UT_Text("  OS_open called: (SUCCESS)");
#endif
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskDelay stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskDelay.  The user can adjust the response by setting the value
**        of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskDelay(uint32 millisecond)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKDELAY_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskDelay called: (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskDelay called: (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemGive stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemGive.  The user can adjust the response by setting the value
**        of UT_BinSemFail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the user-defined value UT_BinSemFail.
**
******************************************************************************/
int32 OS_BinSemGive(uint32 sem_id)
{
#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  OS_BinSemGive called: %lu", sem_id);
    UT_Text(cMsg);
#endif
    return UT_BinSemFail;
}

/*****************************************************************************/
/**
** \brief CFE_PSP_Panic stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        CFE_PSP_Panic.  The variable PSPPanicRtn.value is set equal to the
**        input variable ErrorCode and the variable PSPPanicRtn.count is
**        incremented each time this function is called.  The unit tests
**        compare these values to expected results to verify proper system
**        response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_PSP_Panic(int32 ErrorCode)
{
#ifdef UT_VERBOSE
    SNPRINTF(cMsg, UT_MAX_MESSAGE_LENGTH,
             "  CFE_PSP_Panic called: EC = 0x%lx", (uint32) ErrorCode);
    UT_Text(cMsg);
#endif
    PSPPanicRtn.value = ErrorCode;
    PSPPanicRtn.count++;
}

/*****************************************************************************/
/**
** \brief OS_BinSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemGetInfo.  It sets the binary semaphore structure variables
**        to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemGetInfo(uint32 sem_id, OS_bin_sem_prop_t *bin_prop)
{
    bin_prop->creator =  0;
    strncpy(bin_prop->name, "Name", OS_MAX_API_NAME + 1);
    bin_prop->name[OS_MAX_API_NAME] = '\0';
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_MutSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemGetInfo.  It sets the mutex semaphore structure variables
**        to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemGetInfo(uint32 sem_id, OS_mut_sem_prop_t *mut_prop)
{
    strncpy(mut_prop->name, "Name", OS_MAX_API_NAME + 1);
    mut_prop->name[OS_MAX_API_NAME] = '\0';
    mut_prop->creator =  1;
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_CountSemGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_CountSemGetInfo.  It sets the counting semaphore structure
**        variables to fixed values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_CountSemGetInfo(uint32 sem_id, OS_count_sem_prop_t *count_prop)
{
    count_prop->creator =  0;
    strncpy(count_prop->name, "Name", OS_MAX_API_NAME + 1);
    count_prop->name[OS_MAX_API_NAME] = '\0';
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_QueueGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_QueueGetInfo.  It sets the queue structure variables to fixed
**        values and returns OS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_QueueGetInfo(uint32 sem_id, OS_queue_prop_t *queue_prop)
{
    queue_prop->creator =  0;
    strncpy(queue_prop->name, "Name", OS_MAX_API_NAME + 1);
    queue_prop->name[OS_MAX_API_NAME] = '\0';
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_MutSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_MutSemDelete.  The user can adjust the response by setting
**        the values in the MutSemDelRtn structure prior to this function
**        being called.  If the value MutSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value MutSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_MutSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (MutSemDelRtn.count > 0)
    {
        MutSemDelRtn.count--;

        if (MutSemDelRtn.count == 0)
        {
            status = MutSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemDelete.  The user can adjust the response by setting
**        the values in the BinSemDelRtn structure prior to this function
**        being called.  If the value BinSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BinSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (BinSemDelRtn.count > 0)
    {
        BinSemDelRtn.count--;

        if (BinSemDelRtn.count == 0)
        {
            status = BinSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_CountSemDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_CountSemDelete.  The user can adjust the response by setting
**        the values in the CountSemDelRtn structure prior to this function
**        being called.  If the value CountSemDelRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value CountSemDelRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_CountSemDelete(uint32 sem_id)
{
    int32 status = OS_SUCCESS;

    if (CountSemDelRtn.count > 0)
    {
        CountSemDelRtn.count--;

        if (CountSemDelRtn.count == 0)
        {
            status = CountSemDelRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_FDGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_FDGetInfo.  It sets the table entry structure variables to fixed
**        values and returns OS_FS_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_FDGetInfo(int32 filedes, OS_FDTableEntry *fd_prop)
{
    OS_FDTableEntry fd_prop2;

    fd_prop2.User = 0;
    strncpy(fd_prop2.Path, "PATH", OS_MAX_PATH_LEN);
    fd_prop2.Path[OS_MAX_PATH_LEN - 1] = '\0';
    fd_prop2.IsValid = TRUE;
    *fd_prop = fd_prop2;
    return OS_FS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_TaskDelete stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskDelete.  The user can adjust the response by
**        setting the value of UT_OS_Fail prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either OS_SUCCESS or OS_ERROR.
**
******************************************************************************/
int32 OS_TaskDelete(uint32 task_id)
{
    int32 status = OS_SUCCESS;

    if (UT_OS_Fail & OS_TASKDELETE_FAIL)
    {
        status = OS_ERROR;
#ifdef UT_VERBOSE
        UT_Text("  OS_TaskDelete called (FAILURE)");
#endif
    }
#ifdef UT_VERBOSE
    else
    {
        UT_Text("  OS_TaskDelete called (SUCCESS)");
    }
#endif

    return status;
}

/*****************************************************************************/
/**
** \brief OS_fsBlocksFree stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_fsBlocksFree.  The user can adjust the response by setting
**        the values in the BlocksFreeRtn structure prior to this function
**        being called.  If the value BlocksFreeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value BlocksFreeRtn.value.
**        100 is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or 100.
**
******************************************************************************/
int32 OS_fsBlocksFree(const char *name)
{
    int32 status = 100;

    if (BlocksFreeRtn.count > 0)
    {
        BlocksFreeRtn.count--;

        if (BlocksFreeRtn.count == 0)
        {
            status = BlocksFreeRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_unmount stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_unmount.  The user can adjust the response by setting
**        the values in the UnmountRtn structure prior to this function
**        being called.  If the value UnmountRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value UnmountRtn.value.
**        OS_FS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_FS_SUCCESS.
**
******************************************************************************/
int32 OS_unmount(const char *mountpoint)
{
    int32 status = OS_FS_SUCCESS;

    if (UnmountRtn.count > 0)
    {
        UnmountRtn.count--;

        if (UnmountRtn.count == 0)
        {
            status = UnmountRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TaskExit stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TaskExit.  The variable OSTaskExitRtn.value is set to 1 and the
**        variable OSTaskExitRtn.count is incremented each time this function
**        is called.  The unit tests compare these values to expected results
**        to verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_TaskExit()
{
#ifdef UT_VERBOSE
    UT_Text("  OS_TaskExit called");
#endif
    OSTaskExitRtn.value = 1;
    OSTaskExitRtn.count++;
}

/*****************************************************************************/
/**
** \brief OS_SymbolLookup stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_SymbolLookup.  The user can adjust the response by setting
**        the values in the SymbolLookupRtn structure prior to this function
**        being called.  If the value SymbolLookupRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SymbolLookupRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_SymbolLookup(uint32 *symbol_address, char *symbol_name)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (SymbolLookupRtn.count > 0)
    {
        SymbolLookupRtn.count--;

        if (SymbolLookupRtn.count == 0)
        {
            status = SymbolLookupRtn.value;
            flag = TRUE;
        }
    }

    if (flag == FALSE)
    {
        *symbol_address = (uint32) &dummy_function;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleLoad stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleLoad.  The user can adjust the response by setting
**        the values in the ModuleLoadRtn structure prior to this function
**        being called.  If the value ModuleLoadRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleLoadRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_ModuleLoad(uint32 *module_id, char *module_name, char *filename)
{
    int32 status = OS_SUCCESS;

    if (ModuleLoadRtn.count > 0)
    {
        ModuleLoadRtn.count--;

        if (ModuleLoadRtn.count == 0)
        {
            status = ModuleLoadRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleUnload stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleUnload.  The user can adjust the response by setting
**        the values in the ModuleUnloadRtn structure prior to this function
**        being called.  If the value ModuleUnloadRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleUnloadRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_ModuleUnload(uint32 module_id)
{
    int32 status = OS_SUCCESS;

    if (ModuleUnloadRtn.count > 0)
    {
        ModuleUnloadRtn.count--;

        if (ModuleUnloadRtn.count == 0)
        {
            status = ModuleUnloadRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_ModuleInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_ModuleInfo.  The user can adjust the response by setting
**        the values in the ModuleInfoRtn structure prior to this function
**        being called.  If the value ModuleInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ModuleInfoRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 OS_ModuleInfo(uint32 module_id, OS_module_record_t *module_info)
{
    int32 status = OS_SUCCESS;

    if (ModuleInfoRtn.count > 0)
    {
        ModuleInfoRtn.count--;

        if (ModuleInfoRtn.count == 0)
        {
            status = ModuleInfoRtn.value;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_BinSemTimedWait stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_BinSemTimedWait.  The variable OSBinSemTimedWaitRtn.value is set
**        to the value passed to the function, reset_type, and the variable
**        OSBinSemTimedWaitRtn.count is incremented each time this function is
**        called.  The unit tests compare these values to expected results to
**        verify proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_BinSemTimedWait(uint32 sem_id, uint32 msecs)
{
    OSBinSemTimedWaitRtn.value = sem_id;
    OSBinSemTimedWaitRtn.count++;
    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_HeapGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_HeapGetInfo.  If the input structure, heap_prop, is null then
**        OS_INVALID_POINTER is returned.  If not null, the user can adjust
**        the response by setting the values in the HeapGetInfoRtn structure
**        prior to this function being called.  If the value
**        HeapGetInfoRtn.count is greater than zero then the counter is
**        decremented; if it then equals zero the return value is set to the
**        user-defined value HeapGetInfoRtn.value. OS_SUCCESS is returned
**        otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, OS_INVALID_POINTER, or
**        OS_SUCCESS.
**
******************************************************************************/
int32 OS_HeapGetInfo(OS_heap_prop_t *heap_prop)
{
    int32   status = OS_SUCCESS;
    boolean flag = FALSE;

    if (heap_prop == NULL)
    {
        status = OS_INVALID_POINTER;
    }
    else
    {
        if (HeapGetInfoRtn.count > 0)
        {
            HeapGetInfoRtn.count--;

            if (HeapGetInfoRtn.count == 0)
            {
                status = HeapGetInfoRtn.value;
                flag = TRUE;
            }
        }

        if (flag == FALSE)
        {
            /* Return some random data */
            heap_prop->free_bytes = (uint32) 12345;
            heap_prop->free_blocks = (uint32) 6789;
            heap_prop->largest_free_block = (uint32) 100;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief OS_TimerGetInfo stub function
**
** \par Description
**        This function is used to mimic the response of the OS API function
**        OS_TimerGetInfo.  The user can adjust the response by setting
**        the values in the OSTimerGetInfoRtn structure prior to this function
**        being called.  If the value OSTimerGetInfoRtn.count is greater than
**        zero then the counter is decremented and the timer creator value is
**        set to the user-defined value OSTimerGetInfoRtn.value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 OS_TimerGetInfo(uint32 timer_id, OS_timer_prop_t *timer_prop)
{
    if (OSTimerGetInfoRtn.count > 0)
    {
        timer_prop->creator = OSTimerGetInfoRtn.value;
        OSTimerGetInfoRtn.count--;
    }

    return OS_SUCCESS;
}

/*****************************************************************************/
/**
** \brief OS_TimerDelete stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_TimerDelete.  It always returns OS_ERR_INVALID_ID.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_ERR_INVALID_ID.
**
******************************************************************************/
int32 OS_TimerDelete(uint32 timer_id)
{
    return OS_ERR_INVALID_ID;
}

/*****************************************************************************/
/**
** \brief OS_IntAttachHandler stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_IntAttachHandler.  It always returns OS_ERR_NOT_IMPLEMENTED.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_ERR_NOT_IMPLEMENTED.
**
******************************************************************************/
int32 OS_IntAttachHandler(uint32 InterruptNumber,
                          osal_task_entry InterruptHandler,
                          int32 parameter)
{
    return OS_ERR_NOT_IMPLEMENTED;
}

#ifdef OS_USE_EMBEDDED_PRINTF
/*****************************************************************************/
/**
** \brief OS_GetStringLen stub function
**
** \par Description
**        This function is identical to the OS API function OS_GetStringLen.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  strptr  Pointer to a character string.
**
** \returns
** \retstmt
**        Returns the number of characters in the string. \endcode
** \endreturns
**
******************************************************************************/
int OS_GetStringLen(const char *strptr)
{
    int len = 0;

    /* Find string length */
    for (; *strptr; strptr++)
    {
        len++;
    }

    return len;
}

/*****************************************************************************/
/**
** \brief OS_OutputSingleChar stub function
**
** \par Description
**        This function is identical to the OS API function
**        OS_OutputSingleChar.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] str  Pointer to the output character string buffer.
**
** \param[in]  c    Single character for output to the buffer.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #putchar
**
******************************************************************************/
void OS_OutputSingleChar(char **str, int c)
{
    if (!(gMaxOutputLen >= 0 && gCurrentOutputLen >= gMaxOutputLen))
    {
        if (str)
        {
            /* Output to the character string buffer */
            **str = (char) c;
            (*str)++;
        }
        else
        {
            /* Output to the console */
            /* ~~~ Insert putchar() replacement here ~~~ */
#ifdef OSP_ARINC653_CFE
            TUTF_putchar();
#else
            putchar(c);
#endif
            /* ~~~ Insert putchar() replacement here ~~~ */
        }

        gCurrentOutputLen++;
    }
}

/*****************************************************************************/
/**
** \brief OS_Double2String stub function
**
** \par Description
**        This function is identical to the OS API function OS_Double2String.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out_bfr     Pointer to a character string buffer.
**
** \param[in]  dbl         Double precision floating point value to convert.
**
** \param[in]  dec_digits  Number of digits to display to the right of the
**                         decimal point.
**
** \param[in]  lead        Pointer to the leading character bit flag.
**
** \returns
** \retstmt
**        This function does not return a value. \endcode
** \endreturns
**
** \sa #OS_GetStringLen
**
******************************************************************************/
#ifndef OSP_NO_FLOAT
void OS_Double2String(char *out_bfr, double dbl,
                      unsigned dec_digits, int *lead)
{
    unsigned mult = 1;
    unsigned idx;
    unsigned wholeNum;
    unsigned decimalNum;
    char temp_bfr[FLOAT_BUF_LEN + 1];

    static const double round_nums[8] =
    {
        0.5,
        0.05,
        0.005,
        0.0005,
        0.00005,
        0.000005,
        0.0000005,
        0.00000005
    };

    /* Extract negative info */
    if (dbl < 0.0)
    {
        dbl = -dbl;
        *lead |= LEAD_NEG;
    }

    /* Handling rounding by adding .5LSB to the floating-point data */
    if (dec_digits < 8)
    {
        dbl += round_nums[dec_digits];
    }

    /* Construct fractional multiplier for specified number of digits */
    for (idx = 0; idx < dec_digits; idx++)
    {
        mult *= 10;
    }

    wholeNum = (unsigned) dbl;
    decimalNum = (unsigned) ((dbl - wholeNum) * mult);

    /* Convert integer portion */
    idx = 0;

    while (wholeNum != 0 && idx < FLOAT_BUF_LEN)
    {
        temp_bfr[idx] = '0' + (wholeNum % 10);
        idx++;
        wholeNum /= 10;
    }

    if (idx == 0)
    {
        *out_bfr = '0';
        out_bfr++;
    }
    else
    {
        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    if (dec_digits > 0)
    {
        *out_bfr = '.';
        out_bfr++;

        /* Convert fractional portion */
        idx = 0;

        while (decimalNum != 0)
        {
            temp_bfr[idx] = '0' + (decimalNum % 10);
            idx++;
            decimalNum /= 10;
        }

        /* Pad the decimal portion with 0s as necessary */
        while (idx < dec_digits)
        {
            temp_bfr[idx] = '0';
            idx++;
        }

        while (idx > 0)
        {
            *out_bfr = temp_bfr[idx - 1];
            out_bfr++;
            idx--;
        }
    }

    *out_bfr = 0;
}
#endif

/*****************************************************************************/
/**
** \brief OS_ParseString stub function
**
** \par Description
**        This function is identical to the OS API function OS_ParseString.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out        Pointer to the output character string buffer.
**
** \param[in]  *string    Pointer to the string to format and then output.
**
** \param[in]  min_width  Minimum resulting string length.
**
** \param[in]  max_width  Maximum resulting string length.
**
** \param[in]  pad        Pointer to the leading character bit flag.
**
** \param[in]  lead       Leading character bit flag.
**
** \param[in]  s_flag     Non-zero if this function is called by the %s
**                        handler; zero otherwise.  Used to determine how the
**                        maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_GetStringLen, #OS_OutputSingleChar
**
******************************************************************************/
int OS_ParseString(char **out, const char *string,
                   int min_width, int max_width, int pad,
                   int lead, unsigned s_flag)
{
    register int pc = 0, padchar = ' ';
    int len = 0;

    len = OS_GetStringLen(string);

    /* Set length to # if specified by %.#s format */
    if (s_flag && max_width && len > max_width)
    {
        len = max_width;
    }

    if (min_width > 0)
    {
        if (len >= min_width)
        {
            min_width = 0;
        }
        else
        {
            min_width -= len;
        }

        if ((pad & PAD_ZERO) && !s_flag)
        {
            padchar = '0';
        }
    }

    if (max_width > min_width)
    {
        max_width -= min_width;

        if (len >= max_width)
        {
            max_width = 0;
        }
        else
        {
            max_width -= len;
        }

        if (lead & LEAD_HEX)
        {
            if (max_width >= 2)
            {
                max_width -= 2;
            }
            else
            {
                max_width = 0;
            }
        }
        else if ((lead & LEAD_NEG) || (lead & LEAD_SIGN))
        {
            if (max_width >= 1)
            {
                max_width--;
            }
            else
            {
                max_width = 0;
            }
        }

        for (; max_width > 0; max_width--)
        {
            OS_OutputSingleChar(out, ' ');
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == '0')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }
    else if (lead & LEAD_NEG)
    {
        OS_OutputSingleChar(out, '-');
        pc++;
    }
    else if (lead & LEAD_SIGN)
    {
        OS_OutputSingleChar(out, '+');
        pc++;
    }
    else if (lead & LEAD_SPACE)
    {
        OS_OutputSingleChar(out, ' ');
        pc++;
    }

    if (!(pad & PAD_RIGHT))
    {
        for (; min_width > 0; min_width--)
        {
            OS_OutputSingleChar(out, padchar);
            pc++;
        }
    }

    if ((lead & LEAD_HEX) && padchar == ' ')
    {
        OS_OutputSingleChar(out, '0');
        OS_OutputSingleChar(out, 'x');
        pc += 2;
    }

    for (; len; string++)
    {
        OS_OutputSingleChar(out, *string);
        pc++;
        len--;
    }

    for (; min_width > 0; min_width--)
    {
        OS_OutputSingleChar(out, padchar);
        pc++;
    }

    return pc;
}

/*****************************************************************************/
/**
** \brief OS_ParseInteger stub function
**
** \par Description
**        This function is identical to the OS API function OS_ParseInteger.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] out              Pointer to the output character string buffer.
**
** \param[in]  i                Integer to convert.
**
** \param[in]  base             = 10 for base 10 (decimal); = 16 for base 16
**                              (hexadecimal).
**
** \param[in]  sign             Non-zero for integer values; zero for unsigned
**                              integer values .
**
** \param[in]  space_pad_width  Format modifier minimum field width (e.g.,
**                              the 5 in %5.2d).
**
** \param[in]  zero_pad_width   Format modifier precision specifier (e.g.,
**                              the 2 in %5.2d).
**
** \param[in]  pad              Pointer to the leading character bit flag.
**
** \param[in]  lead             Leading character bit flag.
**
** \param[in]  letbase          Non-zero if this function is called by the %s
**                              handler; zero otherwise.  Used to determine how the
**                              maximum string length is applied.
**
** \returns
** \retstmt
**        Returns the length in characters of the resulting string output.
**        \endcode
** \endreturns
**
** \sa #OS_ParseString
**
******************************************************************************/
int OS_ParseInteger(char **out, int i, unsigned base, int sign,
                    int space_pad_width, int zero_pad_width,
                    int pad, int lead, int letbase)
{
    char print_buf[PRINT_BUF_LEN + 1];
    char *s;
    int t, pc = 0;
    unsigned u = (unsigned) i;
    int rtn;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        rtn = OS_ParseString(out, print_buf, zero_pad_width,
                             space_pad_width, pad, lead, 0);
    }
    else
    {
        if (sign && base == 10 && i < 0)
        {
            u = (unsigned) -i;
            lead |= LEAD_NEG;
        }

        /* Make sure print_buf is NULL-terminated */
        s = print_buf + PRINT_BUF_LEN - 1;
        *s = '\0';

        while (u)
        {
            t = u % base;

            if (t >= 10)
            {
                t += letbase - '0' - 10;
            }

            s--;
            *s = (char) t + '0';
            u /= base;
        }

        rtn = pc + OS_ParseString(out, s, zero_pad_width,
                                  space_pad_width, pad, lead, 0);
    }

    return rtn;
}

/*****************************************************************************/
/**
** \brief OS_vsnprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_vsnprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out     Pointer to character buffer to hold the  output string.
**
** \param[in]  max_len  Maximum allowed length of the output string.
**
** \param[in]  *format  Pointer to a printf-style format string.
**
** \param[in]  varg     Pointer to the beginning of the list of variables
**                      to output per the format string.
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa
**
******************************************************************************/
int OS_vsnprintf(char *out_buffer, int max_len,
                 const char *format, VA_LIST varg)
{
    unsigned post_decimal;
    int width, pad, lead;
    unsigned dec_width;
    int pc = 0;
    char scr[2];
    char *s;
    char **out;

#ifndef OSP_NO_FLOAT
    unsigned dec_width_set;
    double *dblptr;
    double dbl;
    char float_bfr[FLOAT_BUF_LEN + 1];
#endif

#ifdef OSP_ARINC653
    unsigned parm_count = 0;
#endif

    if (out_buffer)
    {
        out = &out_buffer;
    }
    else
    {
        out = 0;
    }

    gMaxOutputLen = max_len;
    gCurrentOutputLen = 0;

    /* Step through and parse the format string */
    for (; *format != '\0'; format++)
    {
        /* Process the format code following the format specifier (%) */
        if (*format == '%')
        {
            format++;
            width = 0;
            dec_width = 0;
#ifndef OSP_NO_FLOAT
            dec_width_set = 0;
#endif
            pad = 0;
            lead = 0;
            post_decimal = 0;

            /* Look for format modifiers */
            while (*format == '-' ||
                   *format == '+' ||
                   *format == ' ' ||
                   *format == '0')
            {
                if (*format == '-')
                {
                    /* Left-justify output */
                    format++;
                    pad |= PAD_RIGHT;
                }
                else if (*format == '+')
                {
                    /* Prepend the plus sign (+) to the output if the value is
                     * positive
                     */
                    format++;
                    lead |= LEAD_SIGN;
                }
                else if (*format == ' ')
                {
                    /* Prepend a space in place of the plus sign (+) if the
                     * value is positive
                     */
                    format++;
                    lead |= LEAD_SPACE;
                }
                else if (*format == '0')
                {
                    /* Pad numerical output with leading zeroes */
                    format++;
                    pad |= PAD_ZERO;
                }
            }

            /* Look for field width and precision specifier (e.g., ###.###) */
            while (1)
            {
                if (*format == '.')
                {
                    /* Decimal point indicates any following numbers are the
                     * precision specifier
                     */
                    if (post_decimal)
                    {
                        /* Already found one decimal point, any others indicate
                         * a format string error; back up pointer so the output
                         * will show all of the erroneous modifier
                         */
                        while (*(format -1) != '%')
                        {
                            format--;
                        }

                        break;
                    }

                    post_decimal = 1;
                    format++;
                }
                else if (*format >= '0' &&  *format <= '9')
                {
                    if (post_decimal)
                    {
                        /* After the decimal is the precision specifier; add
                         * number to the total, accounting for the number of
                         * digits
                         */
                        dec_width *= 10;
                        dec_width += (unsigned) (unsigned char)
                                         (*format - '0');
#ifndef OSP_NO_FLOAT
                        dec_width_set = 1;
#endif
		    }
                    else
                    {
                        /* Prior to the decimal is the field width; add number
                         * to the total, accounting for the number of digits
                         */
                        width *= 10;
                        width += *format - '0';
                    }

                    format++;
                }
                else
                {
                    /* End of field width and precision specifier reached;
                     * exit 'while' loop
                     */
                    break;
                }
            }

            /* Check for long format modifier; if found then skip since all
             * are treated as long regardless
             */
            if (*format == 'l')
            {
                format++;
            }

            switch (*format)
            {
                case 's': /* Character string */
                    s = (char *) *varg;
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    pc += OS_ParseString(out, s, width, dec_width, pad, 0, 1);
                    break;

                case 'd': /* (Long) integer */
                case 'i':
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;

                        if ((int) *varg < 0 || lead)
                        {
                            dec_width--;
                        }
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 1, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    break;

                case 'u': /* (Long) unsigned integer */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if (width && (pad & PAD_ZERO))
                    {
                        dec_width = width;
                        width = 0;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 10, 0, width,
                                          dec_width, pad, 0, 'a');
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    break;

                case 'x': /* Hexadecimal (lower case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    break;

                case 'X': /* Hexadecimal (upper case) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO))
                    {
                        dec_width = width;
                    }

                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'A');
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    break;

                case 'p': /* Address (hexadecimal with '0x' prepended) */
                    if (dec_width)
                    {
                        pad = PAD_ZERO;
                    }
                    else if ((pad & PAD_ZERO) && width >= 2)
                    {
                        dec_width = width - 2;
                    }

                    lead = LEAD_HEX;
                    pc += OS_ParseInteger(out, (int) *varg, 16, 0, width,
                                          dec_width, pad, lead, 'a');
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    break;

                case 'c': /* Single character */
                    scr[0] = (char) *varg;
                    varg++;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif
                    scr[1] = '\0';
                    pc += OS_ParseString(out, scr, width, 0, pad, 0, 0);
                    break;

#ifndef OSP_NO_FLOAT
                case 'f': /* (Long) float */
                    dblptr = (double *) varg;
                    dbl = *dblptr;
                    dblptr++;
                    varg = (VA_LIST) dblptr;
#ifdef OSP_ARINC653	
                    parm_count++;
#endif

                    if (!dec_width_set)
                    {
                        dec_width = 6;
                    }

                    OS_Double2String(float_bfr, dbl, dec_width, &lead);

                    if (width && !(pad & PAD_RIGHT) && !(pad & PAD_ZERO))
                    {
                        dec_width = 0;
                    }
                    else
                    {
                        dec_width = width - (lead != 0);
                        width = 0;
                    }

                    pc += OS_ParseString(out, float_bfr, dec_width,
                                         width, pad, lead, 0);
                    break;
#endif

                case '%': /* Output percent character (%) */
                    OS_OutputSingleChar(out, *format);
                    pc++;
                    break;

                case '\0':
                    /* Premature end of format string; back up one so the
                     * 'for' loop will terminate parsing gracefully
                     */
                    format--;
                    break;

                default:
                    /* Invalid format code; output the format specifier
                     * and code
                     */
                    OS_OutputSingleChar(out, '%');
                    OS_OutputSingleChar(out, *format);
                    pc += 2;
                    break;
            }

#ifdef OSP_ARINC653
            /* If at the end of the first contiguous list of values */
            if (parm_count == OS_printf_break)
            {
                /* Adjust pointer to skip to the second contiguous list */
                parm_count++;
                varg += OS_printf_skip;
            }
#endif
        }
        /* Normal output character (not a format specifier); send to output */
        else
        {
            OS_OutputSingleChar(out, *format);
            pc++;
        }
    }

    if (out)
    {
        **out = '\0';
    }

    return pc;
}

/*****************************************************************************/
/**
** \brief OS_sprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_sprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_sprintf(char *out, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, UT_OFFSET_OS_SPRINTF, UT_BREAK_OS_SPRINTF,
             UT_SKIP_OS_SPRINTF);
    length = OS_vsnprintf(out, -1, format, varg);
    VA_END(varg);
    return(length);
}

/*****************************************************************************/
/**
** \brief OS_snprintf stub function
**
** \par Description
**        This function is identical to the OS API function OS_snprintf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[out] *out         Pointer to character buffer to hold the
**                          output string.
**
** \param[in]  max_len      Maximum allowed length of the output string.
**
** \param[in]  *format      Pointer to a printf-style format string.
**
** \param[in]  {arguments}  Variables to output per the format string (the
**                          number of arguments is variable).
**
** \returns
** \retstmt
**        Returns the number of characters output to the string buffer.
**        \endcode
** \endreturns
**
** \sa #OS_vsnprintf
**
******************************************************************************/
int OS_snprintf(char *out, unsigned max_len, const char *format, ...)
{
    VA_LIST varg;
    int length;

    /* Create a pointer into the stack */
    VA_START(varg, format, UT_OFFSET_OS_SNPRINTF, UT_BREAK_OS_SNPRINTF,
             UT_SKIP_OS_SNPRINTF);
    length = OS_vsnprintf(out, (int) max_len - 1, format, varg);
    VA_END(varg);
    return(length);
}
#endif
