/******************************************************************************
** File: cfe_msgids.h
**
** Purpose:
**   This header file contains the Message Id's for messages used by the
**   cFE core.
**
** Author:   R.McGraw/SSI
**
** Notes:
**   This file should not contain messages defined by cFE external
**   applications.
**
** $Log: cfe_msgids.h  $
** Revision 1.2 2008/07/22 15:53:38GMT-05:00 apcudmore 
** Member moved from cfe_msgids.h in project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/cpu3/inc/project.pj to cfe_msgids.h in project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/platform_inc/cpu3/project.pj.
** Revision 1.1 2008/07/22 15:53:38ACT apcudmore 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/cpu3/inc/project.pj
** Revision 1.2 2008/07/07 14:09:40EDT apcudmore 
** Removed FS Message ID defines from system config files.
** Revision 1.1 2008/04/17 08:02:16EDT ruperera 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/build/cpu3/cfe/inc/project.pj
** Revision 1.2 2007/09/19 15:21:15EDT rjmcgraw 
** DCR4488 Removed all SB network message numbers and message ids
** Revision 1.1 2007/08/24 12:42:26EDT apcudmore
** Initial revision
** Member added to project d:/mksdata/MKS-CFE-PROJECT/fsw/build/cpu3/cfe/inc/project.pj
** Revision 1.6 2007/05/04 20:07:49EDT dlkobe
** Added command to telemeter memory pool statistics
** Revision 1.5 2007/02/23 09:11:29GMT-05:00 njyanchik
** - 1) Add CFE_ES_SHELL_TLM_MSG to cfe_mission_cfg.h
** - 2) Add#define CFE_ES_SHELL_TLM_MID CFE_TLM_MID_BASE_CPUx + CFE_ES_SHELL_TLM_MSG to cfe_msgids.h for CPU's 1, 2, and 3.
** - 3) Add {CFE_TLM_MID_BASE_CPUx + CFE_ES_SHELL_TLM_MSG, {0,0}, 32 } for all 3 CPU's to tomain.c
** - 4) Add CFE_ES_ShellPacket_t to cfe_es_task.h
** - 5) Add CFE_ES_MAX_SHELLL_PKT 32 to cfe_platform_cfg.g in all 3 CPU's
** -6) Add CFE_ES_ShellPacket_t ShellPacket into CFE_ES_TaskData Structure in cfe_es_task.h
** - 7) In CFE_ES_TaskInit, in cfe_es_task.c, add initialization for shell output.
** - 8) In ES_ShellCmd, add logic to break output into chunks and send them to TO.
** Revision 1.4 2006/12/28 16:24:15EST rjmcgraw
** Added msgids for new SB subscription pkts
** Revision 1.3 2006/02/06 16:28:33EST rjmcgraw
** Member moved
** Revision 1.1 2005/06/09 10:58:02EDT rperera
** Initial revision
**
******************************************************************************/
#ifndef _cfe_msgids_
#define _cfe_msgids_

/*
** Includes
*/

#include "cfe_mission_cfg.h"

/*
** cFE Command Message Id's
*/
#define CFE_EVS_CMD_MID         CFE_CMD_MID_BASE_CPU3 + CFE_EVS_CMD_MSG         /* 0x1841 */
                                                       /* Message ID 0x1842 is Available  */
#define CFE_SB_CMD_MID          CFE_CMD_MID_BASE_CPU3 + CFE_SB_CMD_MSG          /* 0x1843 */
#define CFE_TBL_CMD_MID         CFE_CMD_MID_BASE_CPU3 + CFE_TBL_CMD_MSG         /* 0x1844 */
#define CFE_TIME_CMD_MID        CFE_CMD_MID_BASE_CPU3 + CFE_TIME_CMD_MSG        /* 0x1845 */
#define CFE_ES_CMD_MID          CFE_CMD_MID_BASE_CPU3 + CFE_ES_CMD_MSG          /* 0x1846 */

#define CFE_ES_SEND_HK_MID      CFE_CMD_MID_BASE_CPU3 + CFE_ES_SEND_HK_MSG      /* 0x1848 */
#define CFE_EVS_SEND_HK_MID     CFE_CMD_MID_BASE_CPU3 + CFE_EVS_SEND_HK_MSG     /* 0x1849 */
                                                       /* Message ID 0x184A is Available  */
#define CFE_SB_SEND_HK_MID      CFE_CMD_MID_BASE_CPU3 + CFE_SB_SEND_HK_MSG      /* 0x184B */
#define CFE_TBL_SEND_HK_MID     CFE_CMD_MID_BASE_CPU3 + CFE_TBL_SEND_HK_MSG     /* 0x184C */
#define CFE_TIME_SEND_HK_MID    CFE_CMD_MID_BASE_CPU3 + CFE_TIME_SEND_HK_MSG    /* 0x184D */

#define CFE_TIME_TONE_CMD_MID   CFE_CMD_MID_BASE_CPU3 + CFE_TIME_TONE_CMD_MSG   /* 0x1850 */
#define CFE_TIME_1HZ_CMD_MID    CFE_CMD_MID_BASE_CPU3 + CFE_TIME_1HZ_CMD_MSG    /* 0x1851 */

/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_DATA_CMD_MSG   /* 0x1860 */
#define CFE_TIME_FAKE_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_FAKE_CMD_MSG   /* 0x1861 */
#define CFE_TIME_SEND_CMD_MID   CFE_CMD_MID_BASE_GLOB + CFE_TIME_SEND_CMD_MSG   /* 0x1862 */


/*
** CFE Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID       CFE_TLM_MID_BASE_CPU3 + CFE_ES_HK_TLM_MSG       /* 0x0840 */
#define CFE_EVS_HK_TLM_MID      CFE_TLM_MID_BASE_CPU3 + CFE_EVS_HK_TLM_MSG      /* 0x0841 */
                                                       /* Message ID 0x1842 is Available  */
#define CFE_SB_HK_TLM_MID       CFE_TLM_MID_BASE_CPU3 + CFE_SB_HK_TLM_MSG       /* 0x0843 */
#define CFE_TBL_HK_TLM_MID      CFE_TLM_MID_BASE_CPU3 + CFE_TBL_HK_TLM_MSG      /* 0x0844 */
#define CFE_TIME_HK_TLM_MID     CFE_TLM_MID_BASE_CPU3 + CFE_TIME_HK_TLM_MSG     /* 0x0845 */
#define CFE_TIME_DIAG_TLM_MID   CFE_TLM_MID_BASE_CPU3 + CFE_TIME_DIAG_TLM_MSG   /* 0x0846 */
#define CFE_EVS_EVENT_MSG_MID   CFE_TLM_MID_BASE_CPU3 + CFE_EVS_EVENT_MSG_MSG   /* 0x0848 */
#define CFE_SB_STATS_TLM_MID    CFE_TLM_MID_BASE_CPU3 + CFE_SB_STATS_TLM_MSG    /* 0x084A */
#define CFE_ES_APP_TLM_MID      CFE_TLM_MID_BASE_CPU3 + CFE_ES_APP_TLM_MSG      /* 0x084B */
#define CFE_TBL_REG_TLM_MID     CFE_TLM_MID_BASE_CPU3 + CFE_TBL_REG_TLM_MSG     /* 0x084C */
#define CFE_SB_ALLSUBS_TLM_MID  CFE_TLM_MID_BASE_CPU3 + CFE_SB_ALLSUBS_TLM_MSG  /* 0x084D */
#define CFE_SB_ONESUB_TLM_MID   CFE_TLM_MID_BASE_CPU3 + CFE_SB_ONESUB_TLM_MSG   /* 0x084E */
#define CFE_ES_SHELL_TLM_MID    CFE_TLM_MID_BASE_CPU3 + CFE_ES_SHELL_TLM_MSG    /* 0x084F */
#define CFE_ES_MEMSTATS_TLM_MID CFE_TLM_MID_BASE_CPU3 + CFE_ES_MEMSTATS_TLM_MSG /* 0x0850 */

#endif
