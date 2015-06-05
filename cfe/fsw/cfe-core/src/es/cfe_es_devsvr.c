/*
** NOTE:
**
**   At the present time (01/18/11) this file is not yet part of the
**   cFE ES application.  This file contains "in progress" code that
**   is intended to be integrated into ES as part of a future build.
**
**   CFE_ES_RegisterDriver() and CFE_ES_DeviceDriver_t are symbols
**   currently defined in the ES source code.  To avoid duplicate
**   name confusion with the new versions of those symbols defined
**   in this (not yet integrated) file, the symbol names have been
**   temporarily changed to xCFE_ES_RegisterDriver() and
**   xCFE_ES_DeviceDriver_t in this file.
**/

/*
** $Id: cfe_es_devsvr.c 1.7 2014/08/22 15:50:02GMT-05:00 lwalling Exp  $
**
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**  Purpose:  
**    This file implements the cFE Executive Services Device Server functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
**  Modification History:
**
** $Log: cfe_es_devsvr.c  $
** Revision 1.7 2014/08/22 15:50:02GMT-05:00 lwalling 
** Changed signed loop counters to unsigned
** Revision 1.6 2012/01/13 11:50:01EST acudmore 
** Changed license text to reflect open source
** Revision 1.5 2012/01/10 14:49:55EST lwalling 
** Replace inappropriate calls to sprintf() with calls to strncpy()
** Revision 1.4 2011/01/19 14:21:40EST lwalling 
** Fix conflicts with cfe_es_devsvr function and structure names
** Revision 1.3 2010/09/21 16:06:45EDT jmdagost 
** Added declaration for CFE_ES_DevSvrSem.
** Added file prolog.
**
*/

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_es_devsvr.h"
#include <string.h>
#include "cfe_es.h"
#include "cfe_psp.h"

uint32  CFE_ES_DevSvrSem;

/* Local function definitions */
int32 CFE_ES_DeviceDriverValidateParams(xCFE_ES_DeviceDriver_t *DriverDescPtr);
int32 CFE_ES_FindDeviceDriverByName(char* DriverName);

/****************************************************************************************/
int32 CFE_ES_DeviceServerInit()
{

    uint32 SemId;
    uint32 i;
    int32  ReturnCode;
    int32 Status;
    /* Zero out Device registry. */
    CFE_PSP_MemSet(&CFE_ES_DeviceDriverRegistry,0,sizeof (CFE_ES_DeviceDriverRegistry));


    /* Set IsValid Flag to FALSE, in case value of FALSE changes */    
    for (i = 0; i < CFE_ES_DRIVER_REG_MAX_SIZE; i ++)
    {
        CFE_ES_DeviceDriverRegistry[i].IsValid = FALSE;
    }


    Status = OS_BinSemCreate(&SemId, CFE_ES_DRIVER_SEM_NAME, OS_SEM_FULL, 0);

    if (Status < OS_SUCCESS)
    {
        ReturnCode = CFE_ES_DEVSVR_ERROR_INIT;
    }
    else
    {
        CFE_ES_DevSvrSem = SemId;
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}/* end CFE_ES_DeviceServerInit */
/****************************************************************************************/

int32 xCFE_ES_RegisterDriver(uint32 *DriverIdPtr, xCFE_ES_DeviceDriver_t *DriverDescPtr)
{
    uint32  i;
    int32   IntLevel;
    uint32  DriverId, SemId, TaskId;
    char SemName [OS_MAX_API_NAME];
    char TaskName [OS_MAX_API_NAME];
    int32 Status;
    
    int32   ReturnCode = CFE_SUCCESS;

    Status = CFE_ES_DeviceDriverValidateParams(DriverDescPtr);

    if( Status != CFE_SUCCESS)                
    {
        CFE_EVS_SendEvent(CFE_ES_DEVSVR_INV_PARAM_ERR_EID, CFE_EVS_ERROR,
                          "Device Server Registry: Invalid Parameter, Status: 0x%08X", Status);

        ReturnCode = CFE_ES_DEVSVR_INVALID_PARAM;
    }
    else
    {
        /*Semaphore protect this table */

        OS_BinSemTake(CFE_ES_DevSvrSem);

        /* Allocate Driver Registry Entry */
        for (i = 0; i < CFE_ES_DRIVER_REG_MAX_SIZE; i ++)
        {
            if ( CFE_ES_DeviceDriverRegistry[i].IsValid == FALSE)
            {
                break;
            }
        }

        /* No more valid entries */
        if (i >= CFE_ES_DRIVER_REG_MAX_SIZE)
        {
            OS_BinSemGive(CFE_ES_DevSvrSem);

        CFE_EVS_SendEvent(CFE_ES_DEVSVR_NO_HANDLES_ERR_EID, CFE_EVS_ERROR,
                          "Device Server Registry: No Free Handles");

            ReturnCode = CFE_ES_DEVSVR_NO_FREE_HANDLES;
        }
        else
        {
            DriverId = i;

            /* Store parameters into Driver Registry */
            strncpy(CFE_ES_DeviceDriverRegistry[DriverId].DriverName, DriverDescPtr -> Name, CFE_ES_MAX_DEVDVR_NAME_LEN);
            CFE_ES_DeviceDriverRegistry[DriverId].InitFuncPtr       = DriverDescPtr -> InitFuncPtr;
            CFE_ES_DeviceDriverRegistry[DriverId].HandshakeFuncPtr  = DriverDescPtr -> HandshakeFuncPtr;
            CFE_ES_DeviceDriverRegistry[DriverId].IsrFuncPtr        = DriverDescPtr -> IsrFuncPtr;
            CFE_ES_DeviceDriverRegistry[DriverId].DisableFuncPtr    = DriverDescPtr -> DisableFuncPtr;
            CFE_ES_DeviceDriverRegistry[DriverId].TaskPriority      = DriverDescPtr -> TaskPriority;
            CFE_ES_DeviceDriverRegistry[DriverId].StackSize         = DriverDescPtr -> StackSize;
            CFE_ES_DeviceDriverRegistry[DriverId].LogicalIntId      = DriverDescPtr -> LogicalIntId;
            CFE_ES_DeviceDriverRegistry[DriverId].NumIntServiced    = 0;
            CFE_ES_DeviceDriverRegistry[DriverId].ChildTaskExitFlag = FALSE;
            CFE_ES_DeviceDriverRegistry[DriverId].DriverEnabledFlag = TRUE;
            CFE_ES_DeviceDriverRegistry[DriverId].IsValid           = TRUE;

    
            /* Create a semaphore and store handle in Driver Registry */
            OS_BinSemGive(CFE_ES_DevSvrSem);

            strncpy(SemName, DriverDescPtr->Name, OS_MAX_API_NAME);
            SemName[OS_MAX_API_NAME - 5] = '\0';
            strcat(SemName,"_sem");

            Status = OS_BinSemCreate(&SemId,SemName, OS_SEM_EMPTY, 0);

            OS_BinSemTake(CFE_ES_DevSvrSem);

            /* move down to else ? NJY */
            CFE_ES_DeviceDriverRegistry[DriverId].SemId = SemId; 

            OS_BinSemGive(CFE_ES_DevSvrSem);


            if (Status < OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_SEM_CREATE_ERR_EID, CFE_EVS_ERROR,
                                  "Device Server Registry: Binary Sem creation failed, Status = 0x%08X", Status);

                ReturnCode = Status;
            }
            else
            {
                /* Put Driver Registry Handle into created Queue */
    
                /* Add generic interrupt service routine to interrupt ID via OSAL */

                OS_IntAttachHandler(DriverDescPtr -> LogicalIntId,

                               (void*) &CFE_ES_InterruptHandler,

                                  DriverDescPtr -> LogicalIntId);
    
                /* Lock out interrupts */
                IntLevel = OS_IntLock();

                if (IntLevel < OS_SUCCESS)
                {
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_INT_LOCK_ERR_EID, CFE_EVS_ERROR,
                                  "Device Server Registry: Locking of interrupts failed, Status = 0x%08X", IntLevel);

                    ReturnCode  = CFE_ES_DEVSVR_INTLOCK_FAILED;
                }
            
                else
                {
                    /* Call Allocated Device Driver Hardware Initialization Routine */
                    (*DriverDescPtr -> InitFuncPtr)();
    
                    /* Enable Interrupts */
                    Status = OS_IntUnlock(IntLevel);
    
                    if (Status < OS_SUCCESS)
                    {
                    
                        CFE_EVS_SendEvent(CFE_ES_DEVSVR_INT_UNLOCK_ERR_EID, CFE_EVS_ERROR,
                                      "Device Server Registry: Unlocking of interrupts failed, Status = 0x%08X", Status);

                        ReturnCode  = CFE_ES_DEVSVR_INTUNLOCK_FAILED;
                    }
                    else
                    {

                        /* Spawn generic Child Task with mangled Device Driver Name */
                        strncpy(TaskName, DriverDescPtr->Name, OS_MAX_API_NAME);
                        TaskName[OS_MAX_API_NAME - 1] = '\0';
                        Status = OS_TaskCreate(& TaskId, TaskName, 
                                               (void*) CFE_ES_GenericIntTask(DriverId), 
                                                 NULL,
                                                 DriverDescPtr -> StackSize, 
                                                 DriverDescPtr -> TaskPriority,
                                                 0);

                        OS_BinSemTake(CFE_ES_DevSvrSem);
            
                        CFE_ES_DeviceDriverRegistry[DriverId].ChildTaskId = TaskId;

                        OS_BinSemGive(CFE_ES_DevSvrSem);

                        if (Status < OS_SUCCESS)
                        {
                            CFE_EVS_SendEvent(CFE_ES_DEVSVR_TASK_CREATE_ERR_EID, CFE_EVS_ERROR,
                                              "Device Server Registry: Interrupt Task Create failed, Status = 0x%08X", Status);

                            ReturnCode = CFE_ES_DEVSVR_TASKCREATE_FAILED;
                        }
                        else
                        {
                            /* we are at the end of the line */
                        }


                    }/* end else OS_IntUnlock failed */
                }/* end else IntLock Failed */
            }/* end else OS_BinSemCreate failure */
        } /* end valid table entry */
    }/* end valid parameters */

    /* we had an error, and we have already written something to the table, so we 
     * have to undo those changes */
    if(( ReturnCode != CFE_SUCCESS) && 
        (ReturnCode != CFE_ES_DEVSVR_NO_FREE_HANDLES) && 
        (ReturnCode != CFE_ES_DEVSVR_INVALID_PARAM))
    {
        /* make sure we mark this entry as free */
        /* this sets the IsValid flag to false */
        CFE_PSP_MemSet(&CFE_ES_DeviceDriverRegistry[DriverId], 0 ,sizeof (CFE_ES_DriverRegistry_t));

        /* Set IsValid Flag to FALSE, in case value of FALSE changes */    
        CFE_ES_DeviceDriverRegistry[DriverId].IsValid = FALSE;
    }

    return ReturnCode;
   
}/* end xCFE_ES_RegisterDriver */
/****************************************************************************************/
int32 CFE_ES_GenericIntTask(uint32 DeviceDriverHandle)
{
    uint32 DeviceHandle;

    DeviceHandle = DeviceDriverHandle;

    /* Make sure the flag to exit hasn't been set to TRUE */
    while ( CFE_ES_DeviceDriverRegistry[DeviceHandle].ChildTaskExitFlag == FALSE)
    {
        OS_BinSemTake(CFE_ES_DeviceDriverRegistry[DeviceHandle].SemId);

        /* Since we may have been waiting on a semaphore for a while, check the
         * flag again */
        if(CFE_ES_DeviceDriverRegistry[DeviceHandle].ChildTaskExitFlag == FALSE)
        {   
            /* Call the hardware ISR function */
            (*CFE_ES_DeviceDriverRegistry[DeviceHandle].IsrFuncPtr)();

            /* Update Statistics */
            CFE_ES_DeviceDriverRegistry[DeviceHandle].NumIntServiced++;
        }
    }/* end while */

    return CFE_SUCCESS;
}/* end CFE_ES_GenericIntTask */
/****************************************************************************************/
void  CFE_ES_InterruptHandler(uint32 InterruptId)
{
    uint32 i;

    /* Allocate Driver Registry Entry */
    for (i = 0; i < CFE_ES_DRIVER_REG_MAX_SIZE; i ++)
    {
        if(  (CFE_ES_DeviceDriverRegistry[i].IsValid == TRUE) && 
             (CFE_ES_DeviceDriverRegistry[i].LogicalIntId == InterruptId))
        {
            break;
        }
    }

    /* No more valid entries */
    if (i >= CFE_ES_DRIVER_REG_MAX_SIZE)
    {
        CFE_ES_WriteToSysLog("Device Server: Cannot find given InterruptId: %d\n",InterruptId);
    }
    else
    {
        if (CFE_ES_DeviceDriverRegistry[i].DriverEnabledFlag == TRUE)
        {
            /* Call handshake function for this device */
            (*CFE_ES_DeviceDriverRegistry[i].HandshakeFuncPtr) ();
            
            /* Trigger semaphore for the driver */
            OS_BinSemGive( CFE_ES_DeviceDriverRegistry[i].SemId);

            /*NJY  Update Statistics ? */
        }
        else
        {
            /*NJY UpdateStatistics ? */
        }
    }
}/* end CFE_ES_InterruptHandler */
/****************************************************************************************/

void CFE_ES_DriverEnable(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_DeviceServerCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        CFE_ES_DeviceServerCmd_t *cmd = (CFE_ES_DeviceServerCmd_t *) Msg;

        if (cmd -> DeviceEntry >= CFE_ES_DRIVER_REG_MAX_SIZE)
        {
            /* Bad device driver */
            CFE_EVS_SendEvent(CFE_ES_DEVSVR_BAD_DEV_DRV_ENABLE_ERR_EID, CFE_EVS_ERROR,
                             "Device Server Driver Enable: bad device driver given: 0x%08X",cmd -> DeviceEntry);

        }
        else
        {

            if (CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].DriverEnabledFlag == TRUE)
            {
                /* Driver already enabled, Send Event*/
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_ALREADY_ENABLED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Enable: Driver already enabled");
            }
            else
            {
                CFE_ES_DeviceDriverRegistry[cmd-> DeviceEntry].DriverEnabledFlag = TRUE;
                /* Send Event, enabled */
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_ENABLED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Enable: Driver enabled");
            }
        }/* end if Device Entry valid */
    } /* end if length verified */
}
/****************************************************************************************/
void CFE_ES_DriverDisable(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_DeviceServerCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        CFE_ES_DeviceServerCmd_t *cmd = (CFE_ES_DeviceServerCmd_t *) Msg;

        if (cmd -> DeviceEntry >= CFE_ES_DRIVER_REG_MAX_SIZE)
        {
            /* Bad device driver */
            CFE_EVS_SendEvent(CFE_ES_DEVSVR_BAD_DEV_DRV_DISABLE_ERR_EID, CFE_EVS_ERROR,
                              "Device Server Driver Disable: bad device driver given: 0x%08X",cmd -> DeviceEntry);
        }
        else
        {

            if (CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].DriverEnabledFlag == FALSE)
            {
                /* Driver already disabled, Send Event*/
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_ALREADY_DISABLED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Disable: Driver already disabled");
            }

            
            else
            {
                CFE_ES_DeviceDriverRegistry[cmd-> DeviceEntry].DriverEnabledFlag = FALSE;
                /*Send Event, disabled */
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_DISABLED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Disable: Driver disbled");

            }
        }/* end if Device Entry valid */
    } /* end if length verified */
}
/****************************************************************************************/
void CFE_ES_DriverUnregister(CFE_SB_MsgPtr_t Msg)
{
    uint16 ExpectedLength = sizeof(CFE_ES_DeviceServerCmd_t);

    /*
    ** Verify command packet length.
    */
    if (CFE_ES_VerifyCmdLength(Msg, ExpectedLength))
    {
        CFE_ES_DeviceServerCmd_t *cmd = (CFE_ES_DeviceServerCmd_t *) Msg;

        if (cmd -> DeviceEntry >= CFE_ES_DRIVER_REG_MAX_SIZE)
        {
            /* NJY Bad device driver */        
            CFE_EVS_SendEvent(CFE_ES_DEVSVR_BAD_DEV_DRV_UNREGISTERED_ERR_EID, CFE_EVS_ERROR,
                              "Device Server Driver Unregister: bad device driver given: 0x%08X",cmd -> DeviceEntry);
        }
        else
        {

            if (CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].IsValid == FALSE)
            {
                /* Driver already unregistered, Send Event NJY */
                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_ALREADY_UNREGISTERED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Unregister: Driver already unregistered");
                
            }
            else
            {

                /* Set the exit flag to exit the next time the task is run */
                CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].ChildTaskExitFlag = TRUE;

                /* Give the semphore to force the interrupt task to run */
          
                OS_BinSemGive(CFE_ES_DeviceDriverRegistry[cmd-> DeviceEntry].SemId);

                /* Delay to wait for the task to exit */
                OS_TaskDelay(300);

                /* Delete the task and semaphore that were created upon registry */

                OS_TaskDelete (CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].ChildTaskId);
                OS_BinSemDelete( CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].SemId);

                /* Let this device Id be used again */
                CFE_ES_DeviceDriverRegistry[cmd -> DeviceEntry].IsValid = FALSE;

                CFE_EVS_SendEvent(CFE_ES_DEVSVR_DRIVER_UNREGISTERED_INF_EID, CFE_EVS_INFORMATION,
                                  "Device Server Driver Unregister: Driver unregistered");

            }
        }/* end if Device Entry valid */
    } /* end if length verified */
}
/****************************************************************************************/

int32 CFE_ES_DeviceDriverValidateParams(xCFE_ES_DeviceDriver_t *DriverDescPtr)
{
    int32 ReturnCode = CFE_SUCCESS;
    int32 Status;
    
    Status = CFE_ES_FindDeviceDriverByName(DriverDescPtr -> Name);

    if( Status != CFE_ES_DEVSVR_NAME_NOT_FOUND)
    {
        ReturnCode =  CFE_ES_DEVSVR_NAME_TAKEN;
    }

    return ReturnCode;
}
/****************************************************************************************/

int32 CFE_ES_FindDeviceDriverByName(char* DriverName)
{
    int32 Status;
    uint32 i;


    for (i = 0; i < CFE_ES_DRIVER_REG_MAX_SIZE; i++)
    {
       if (CFE_ES_DeviceDriverRegistry[i].IsValid == TRUE &&
          strcmp(DriverName, CFE_ES_DeviceDriverRegistry[i].DriverName) ==0)
       {
           break;
       }
    }

    if (i >= CFE_ES_DRIVER_REG_MAX_SIZE)
    {
        Status = CFE_ES_DEVSVR_NAME_NOT_FOUND;
    }
    else
    {
        Status = i;
    }

    return Status;
}

