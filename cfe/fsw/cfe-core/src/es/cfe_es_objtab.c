/*
**   File:
**    cfe_es_objtab.c
**
**
**
**      Copyright (c) 2004-2012, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA's Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**   Purpose:
**     This file contains the OS_object_table for MAP Build1.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**  $Log: cfe_es_objtab.c  $
**  Revision 1.5 2014/07/25 10:56:45GMT-05:00 lwalling 
**  Changed INCLUDE_CFE_TBL to EXCLUDE_CFE_TBL
**  Revision 1.4 2014/07/23 11:33:21EDT lwalling 
**  Made Table Services conditionsal based on new environment variable INCLUDE_CFE_TBL
**  Revision 1.3 2012/01/13 11:50:03EST acudmore 
**  Changed license text to reflect open source
**  Revision 1.2 2008/07/08 15:40:39EDT apcudmore 
**  Added CFE_FS global data, lock/unlock functions, log messages for decompress API.
**  Revision 1.1 2008/04/17 08:05:06EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.15 2007/09/21 15:40:22EDT David Kobe (dlkobe) 
**  Modified pointer type definitions to eliminate Linux gcc compile warnings
**  Revision 1.14 2007/05/15 11:16:05EDT apcudmore 
**  Added modification log tags.
**
*/

/*
** Include files
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_global.h"
#include "cfe_es_start.h"

/*
** cFE Core task prototypes
*/
void CFE_TIME_TaskMain(void);
void CFE_SB_TaskMain(void);
void CFE_EVS_TaskMain(void);
void CFE_ES_TaskMain(void);
#ifndef EXCLUDE_CFE_TBL
void CFE_TBL_TaskMain(void);
#endif

/*
** External functions to call during startup
*/
int32 CFE_EVS_EarlyInit(void);
int32 CFE_SB_EarlyInit(void);
int32 CFE_TIME_EarlyInit(void);
#ifndef EXCLUDE_CFE_TBL
int32 CFE_TBL_EarlyInit(void);
#endif
int32 CFE_ES_CDS_EarlyInit(void);
int32 CFE_FS_EarlyInit(void);

/*
**
** ES_object_table
** Note: The name field in this table should be no more than OS_MAX_API_NAME -1 characters.
**
*/
CFE_ES_ObjectTable_t  CFE_ES_ObjectTable[CFE_ES_OBJECT_TABLE_SIZE] =
{
   /*
   ** Spare entries -- The spares should be distributed evenly through this table
   */
   { CFE_ES_NULL_ENTRY,    "NULL",           {NULL},                        0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",           {NULL},                        0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",           {NULL},                        0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",           {NULL},                        0, 0, 0, 0, 0, 0, 0, 0},
   
   /*
   ** cFE core early initialization calls. These must be done before the tasks start
   */
   { CFE_ES_FUNCTION_CALL, "CFE_ES_CDSEarlyInit", {CFE_ES_CDS_EarlyInit},    0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_FUNCTION_CALL, "CFE_EVS_EarlyInit",   {CFE_EVS_EarlyInit},       0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_FUNCTION_CALL, "CFE_SB_EarlyInit",    {CFE_SB_EarlyInit},        0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_FUNCTION_CALL, "CFE_TIME_EarlyInit",  {CFE_TIME_EarlyInit},      0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
#ifndef EXCLUDE_CFE_TBL
   { CFE_ES_FUNCTION_CALL, "CFE_TBL_EarlyInit",   {CFE_TBL_EarlyInit},       0, 0, 0, 0, 0, 0, 0, 0},
#else
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
#endif
   { CFE_ES_NULL_ENTRY,    "NULL",                {NULL},                    0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_FUNCTION_CALL, "CFE_FS_EarlyInit",    {CFE_FS_EarlyInit},        0, 0, 0, 0, 0, 0, 0, 0},

   /*
   ** Spare entries
   */
   { CFE_ES_NULL_ENTRY,    "NULL",            {NULL},                       0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,    "NULL",            {NULL},                       0, 0, 0, 0, 0, 0, 0, 0},

   /*
   ** cFE core tasks
   **
   ** NOTE: In order to inhibit unnecessary warnings, it is necessary to cast Main Function Pointers
   **       to the first pointer type in the field's union type (which, in this case, is CFE_ES_EarlyInitFuncPtr_t)
   */
   { CFE_ES_CORE_TASK, "CFE_EVS",  {(CFE_ES_EarlyInitFuncPtr_t)CFE_EVS_TaskMain},   0, 0,  0,   CFE_EVS_START_TASK_PRIORITY, CFE_EVS_START_TASK_STACK_SIZE,  0, 0, 0},
   { CFE_ES_NULL_ENTRY,"NULL",     {NULL},                                          0, 0,  0,   0,                           0,                              0, 0, 0},
   { CFE_ES_CORE_TASK, "CFE_SB",   {(CFE_ES_EarlyInitFuncPtr_t)CFE_SB_TaskMain},    0, 0,  0,   CFE_SB_START_TASK_PRIORITY,  CFE_SB_START_TASK_STACK_SIZE,   0, 0, 0},
   { CFE_ES_NULL_ENTRY,"NULL",     {NULL},                                          0, 0,  0,   0,                           0,                              0, 0, 0},
   { CFE_ES_CORE_TASK, "CFE_ES",   {(CFE_ES_EarlyInitFuncPtr_t)CFE_ES_TaskMain},    0, 0,  0,   CFE_ES_START_TASK_PRIORITY,  CFE_ES_START_TASK_STACK_SIZE,   0, 0, 0},
   { CFE_ES_NULL_ENTRY,"NULL",     {NULL},                                          0, 0,  0,   0,                           0,                              0, 0, 0},
   { CFE_ES_CORE_TASK, "CFE_TIME", {(CFE_ES_EarlyInitFuncPtr_t)CFE_TIME_TaskMain},  0, 0,  0,   CFE_TIME_START_TASK_PRIORITY,CFE_TIME_START_TASK_STACK_SIZE, 0, 0, 0},
   { CFE_ES_NULL_ENTRY,"NULL",     {NULL},                                          0, 0,  0,   0,                           0,                              0, 0, 0},
#ifndef EXCLUDE_CFE_TBL
   { CFE_ES_CORE_TASK, "CFE_TBL",  {(CFE_ES_EarlyInitFuncPtr_t)CFE_TBL_TaskMain},   0, 0,  0,   CFE_TBL_START_TASK_PRIORITY, CFE_TBL_START_TASK_STACK_SIZE,  0, 0, 0},
#else
   { CFE_ES_NULL_ENTRY,"NULL",     {NULL},                                          0, 0,  0,   0,                           0,                              0, 0, 0},
#endif

   /*
   ** Spare entries
   */
   { CFE_ES_NULL_ENTRY, "NULL", {NULL}, 0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY, "NULL", {NULL}, 0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY, "NULL", {NULL}, 0, 0, 0, 0, 0, 0, 0, 0},
   { CFE_ES_NULL_ENTRY, "NULL", {NULL}, 0, 0, 0, 0, 0, 0, 0, 0}

};
