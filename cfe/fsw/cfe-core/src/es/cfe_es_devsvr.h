/*
** NOTE:
**
**   At the present time (01/18/11) this file is not part of the
**   cFE ES application.  This file contains "in progress" code
**   intended to be integrated into ES as part of a future build.
**
**   CFE_ES_RegisterDriver() and CFE_ES_DeviceDriver_t are symbols
**   currently defined in the ES source code.  To avoid duplicate
**   name confusion with the new versions of those symbols defined
**   in this (not yet integrated) file, the symbol names have been
**   temporarily changed to xCFE_ES_RegisterDriver() and
**   xCFE_ES_DeviceDriver_t in this file.
**/

/*
** $Id: cfe_es_devsvr.h 1.4 2012/01/13 11:50:01GMT-05:00 acudmore Exp  $
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
**    This file contains definitions needed for the cFE ES Device Servers.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
** 
** $Log: cfe_es_devsvr.h  $
** Revision 1.4 2012/01/13 11:50:01GMT-05:00 acudmore 
** Changed license text to reflect open source
** Revision 1.3 2011/01/19 14:21:41EST lwalling 
** Fix conflicts with cfe_es_devsvr function and structure names
** Revision 1.2 2010/09/21 16:07:13EDT jmdagost 
** Removed declaration for CFE_ES_DevSvrSem (moved to source file).
** Added file prolog.
**
*/

#ifndef _cfe_es_devsvr_
#define _cfe_es_devsvr_












#include "cfe.h"
#include "cfe_es_task.h"

#if 0
/*NJY  original  declaration from cfe_es.h -= needs to be removed */
typedef struct
{
   uint32   InterruptId;            /**< \brief Identifier for interrupt that device driver is attached to */
   void    *IntInitFuncPtr;         /**< \brief Pointer to function used to initialize device */
   void    *IntHWHandshakeFuncPtr;  /**< \brief Pointer to function executed when servicing device interrupts */
   void    *IntIsrFuncPtr;          /**< \brief Pointer to function that performs most ISR processing but done in a task context */
   
} CFE_ES_DeviceDriver_t;
#endif


/* this stuff should go in cfe_es.h or above */
/*(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((*/
/* Configuration parameters */
#define CFE_ES_MAX_DEVDVR_NAME_LEN     OS_MAX_API_NAME
#define CFE_ES_DRIVER_REG_MAX_SIZE  20
#define CFE_ES_DRIVER_SEM_NAME      "CFE_ES_DrvSvrSem"


typedef void (*CFE_ES_DevInitFuncPtr_t)(void);
typedef void (*CFE_ES_DevHandFuncPtr_t)(void); 
typedef void (*CFE_ES_DevIsrFuncPtr_t)(void) ;      
typedef void (*CFE_ES_DevDisFuncPtr_t)(void) ;

/* NJY Device driver declaration */
typedef struct
{
    uint32                      LogicalIntId;
    CFE_ES_DevInitFuncPtr_t     InitFuncPtr;
    CFE_ES_DevHandFuncPtr_t     HandshakeFuncPtr;
    CFE_ES_DevIsrFuncPtr_t      IsrFuncPtr;
    CFE_ES_DevDisFuncPtr_t      DisableFuncPtr;
    uint32                      TaskPriority;
    uint32                      StackSize;
    char                        Name[CFE_ES_MAX_DEVDVR_NAME_LEN];
} xCFE_ES_DeviceDriver_t;

/* struct for Driver Registry Table */
typedef struct
{
    char                        DriverName[CFE_ES_MAX_DEVDVR_NAME_LEN];
    CFE_ES_DevInitFuncPtr_t     InitFuncPtr;
    CFE_ES_DevHandFuncPtr_t     HandshakeFuncPtr;
    CFE_ES_DevIsrFuncPtr_t      IsrFuncPtr;
    CFE_ES_DevDisFuncPtr_t      DisableFuncPtr;
    uint32                      TaskPriority;
    uint32                      StackSize;
    uint32                      ChildTaskId;
    uint32                      LogicalIntId;
    uint32                      NumIntServiced;
    uint32                      SemId;
    uint8                       ChildTaskExitFlag;
    uint8                       DriverEnabledFlag;
    uint8                       IsValid;
}CFE_ES_DriverRegistry_t;

/* The registry table */

CFE_ES_DriverRegistry_t CFE_ES_DeviceDriverRegistry [CFE_ES_DRIVER_REG_MAX_SIZE];


int32 CFE_ES_DeviceServerInit(void); 

int32 xCFE_ES_RegisterDriver(uint32 *DriverIdPtr, xCFE_ES_DeviceDriver_t *DriverDescPtr);

int32 CFE_ES_GenericIntTask(uint32 DeviceDriverHandle);

void  CFE_ES_InterruptHandler(uint32 InterruptId);
/*)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))*/


/*******************************************************/
/* Event IDs. Go in cfe_es_events.h, start at 86 */

/** \brief <tt> 'Device Server Registry: Invalid Parameter, Status = 0x\%08X' </tt>
**  \event <tt> 'Device Server Registry: Invalid Parameter, Status = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and is passed invalid parameters
**
**  The 0x\%08X is the return code describing what parameter failed.
**/
#define CFE_ES_DEVSVR_INV_PARAM_ERR_EID      86

/** \brief <tt> 'Device Server Registry: No Free Handles' </tt>
**  \event <tt> 'Device Server Registry: No Free Handles' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and there are no free device handles
**/
#define CFE_ES_DEVSVR_NO_HANDLES_ERR_EID      87

/** \brief <tt> 'Device Server Registry: Binary Sem creation failed, Status = 0x\%08X' </tt>
**  \event <tt> 'Device Server Registry: Binary Sem creation failed, Status = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and the synchronization semaphore cannot be created
**  
**  The 0x\%08X is the return code of OS_BinSemCreate
**/
#define CFE_ES_DEVSVR_SEM_CREATE_ERR_EID      88



/** \brief <tt> 'Device Server Registry: Unlocking of interrupts failed, Status = 0x\%08X' </tt>
**  \event <tt> 'Device Server Registry: Unlocking of interrupts failed, Status = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and the routine fails to unlock interrupts
**  
**  The 0x\%08X is the return code of OS_IntUnlock
**/
#define CFE_ES_DEVSVR_INT_UNLOCK_ERR_EID      89

/** \brief <tt> 'Device Server Registry: Locking of interrupts failed, Status = 0x\%08X' </tt>
**  \event <tt> 'Device Server Registry: Locking of interrupts failed, Status = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and the routine fails to lock interrupts
**  
**  The 0x\%08X is the return code of OS_IntLock
**/
#define CFE_ES_DEVSVR_INT_LOCK_ERR_EID      90


/** \brief <tt> 'Device Server Registry: Interrupt Task Create failed, Status = 0x\%08X' </tt>
**  \event <tt> 'Device Server Registry: Interrupt Task Create failed, Status = 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Server tries to register
**  a driver, and the routine fails to create the generic interrupt task
**  
**  The 0x\%08X is the return code of OS_TaskCreate
**/
#define CFE_ES_DEVSVR_TASK_CREATE_ERR_EID      91


/** \brief <tt> 'Device Server Driver Enable: bad device driver given: 0x\%08X' </tt>
**  \event <tt> 'Device Server Driver Enable: bad device driver given: 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to Enable 
**  a driver, and bad driver handle is given
**  
**  The 0x\%08X is the driver handle given
**/
#define CFE_ES_DEVSVR_BAD_DEV_DRV_ENABLE_ERR_EID      92


/** \brief <tt> 'Device Server Driver Enable: Driver already enabled' </tt>
**  \event <tt> 'Device Server Driver Enable: Driver already enabled' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to Enable 
**  a driver, and the driver is already enabled
**  
**/
#define CFE_ES_DEVSVR_DRIVER_ALREADY_ENABLED_INF_EID      93

/** \brief <tt> 'Device Server Driver Enable: Driver enabled' </tt>
**  \event <tt> 'Device Server Driver Enable: Driver enabled' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to Enable 
**  a driver, and the driver is enabled
**  
**/
#define CFE_ES_DEVSVR_DRIVER_ENABLED_INF_EID      94

/** \brief <tt> 'Device Server Driver Disable: bad device driver given: 0x\%08X' </tt>
**  \event <tt> 'Device Server Driver Disable: bad device driver given: 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to disable 
**  a driver, and bad driver handle is given
**  
**  The 0x\%08X is the driver handle given
**/
#define CFE_ES_DEVSVR_BAD_DEV_DRV_DISABLE_ERR_EID      95


/** \brief <tt> 'Device Server Driver Disable: Driver already disabled' </tt>
**  \event <tt> 'Device Server Driver Disable: Driver already disabled' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to disable 
**  a driver, and the driver is already disabled
**  
**/
#define CFE_ES_DEVSVR_DRIVER_ALREADY_DISABLED_INF_EID      96

/** \brief <tt> 'Device Server Driver Disable: Driver disabled' </tt>
**  \event <tt> 'Device Server Driver Disable: Driver disabled' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to disable
**  a driver, and the driver is disiabled
**  
**/
#define CFE_ES_DEVSVR_DRIVER_DISABLED_INF_EID      97

/** \brief <tt> 'Device Server Driver Unregister: bad device driver given: 0x\%08X' </tt>
**  \event <tt> 'Device Server Driver Unregister: bad device driver given: 0x\%08X' </tt> 
**
**  \par Type: ERROR
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to unregister 
**  a driver, and bad driver handle is given
**  
**  The 0x\%08X is the driver handle given
**/
#define CFE_ES_DEVSVR_BAD_DEV_DRV_UNREGISTERED_ERR_EID      98


/** \brief <tt> 'Device Server Driver Unregister: Driver already unregistered' </tt>
**  \event <tt> 'Device Server Driver Unregister: Driver already disabled' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to unregister 
**  a driver, and the driver is already unregistered
**  
**/
#define CFE_ES_DEVSVR_DRIVER_ALREADY_UNREGISTERED_INF_EID      99

/** \brief <tt> 'Device Server Driver Enable: Driver unregistered' </tt>
**  \event <tt> 'Device Server Driver Enable: Driver unregistered' </tt> 
**
**  \par Type: INFORMATION
**
**  \par Cause:
**
**  This event message is generated when the cFE Device Service is commanded to unregistered
**  a driver, and the driver is unregistered
**  
**/
#define CFE_ES_DEVSVR_DRIVER_UNREGISTERED_INF_EID      100


/**********************************************************************/
/*Error codes, go in cfe_error.h */

/**
** Occurs when the ES Device Server already has a driver with the 
** given name already. 
*/
#define CFE_ES_DEVSVR_NAME_TAKEN    (0xc4000023L)

/**
** Occurs when the given name was not found in the ES Device Server registry. 
** There will be an event sent detailing the problem.
*/
#define CFE_ES_DEVSVR_NAME_NOT_FOUND    (0xc4000024L)

/**
** Occurs when here was an error initializing the Device Server. 
** There will be an event sent detailing the problem.
*/
#define CFE_ES_DEVSVR_ERROR_INIT        (0xc4000025L)


/**
** Occurs when a parameter passed into DeviceRegister was invalid. 
** There will be an event sent detailing the problem.
*/
#define CFE_ES_DEVSVR_INVALID_PARAM     (0xc4000026L)


/**
** There were no more available handles in the Device Server registry.
** There will be an event sent detailing the problem.
*/
#define CFE_ES_DEVSVR_NO_FREE_HANDLES     (0xc4000027L)


/**
** There was an error locking interrupts.
** There will be an event sent detailing the problem.
*/

#define CFE_ES_DEVSVR_INTLOCK_FAILED      (0xc4000028L)


/**
** There was an error unlocking interrupts. 
** There will be an event sent detailing the problem.
*/
#define CFE_ES_DEVSVR_INTUNLOCK_FAILED        (0xc4000029L)


/**
** Occurs when there was an error with creating a new task in the 
** ES Device Server.
** There will be an event sent detailing the problem.
*/

#define CFE_ES_DEVSVR_TASKCREATE_FAILED     (0xc400002AL)


/****************************************************/
/* Command codes: Go in cfe_es_msg.h */

typedef struct
{
  uint8                 CmdHeader[CFE_SB_CMD_HDR_SIZE]; /**< \brief cFE Software Bus Command Message Header */
  uint32                 DeviceEntry;                   /**< \brief Entry of the device in the Device Registry Table */
} CFE_ES_DeviceServerCmd_t;


/** 
**  \cfeestlm OS Shell Output Packet
**/
typedef struct
{
    uint8                 TlmHeader[CFE_SB_TLM_HDR_SIZE];     /**< \brief cFE Software Bus Telemetry Message Header */
    CFE_ES_DriverRegistry_t CFE_ES_DeviceDriverRegistry [CFE_ES_DRIVER_REG_MAX_SIZE];  /**< \brief ommand */
}CFE_ES_DriverRegistryPacket_t;


/** \cfeescmd Enable Device Driver
**
**  \par Description
**       This command allows the user to enable a device driver that has been registered with the system.
**
**  \cfecmdmnemonic \ES_DEVICEDRIVERENABLE
**
**  \par Command Structure
**       #CFE_ES_DeviceServerCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - TBD
**
**  \par Error Conditions
**       TBD
**
**  \par Criticality
**       TBD
**
**  \sa 
*/
/* NJY #define CFE_ES_DEVICE_DRIVER_ENABLE_CC          24 */

/** \cfeescmd Disable Device Driver
**
**  \par Description
**       This command allows the user to disable a device driver that has been registered with the system.
**
**  \cfecmdmnemonic \ES_DEVICEDRIVERDISABLE
**
**  \par Command Structure
**       #CFE_ES_DeviceServerCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - TBD
**
**  \par Error Conditions
**       TBD
**
**  \par Criticality
**       TBD
**
**  \sa 
*/
#define CFE_ES_DEVICE_DRIVER_DISABLE_CC          25

/** \cfeescmd Unregister
**
**  \par Description
**       This command allows the user to unregister a device driver that has been registered with the system.
**
**  \cfecmdmnemonic \ES_DEVICEDRIVERUNREGISTER
**
**  \par Command Structure
**       #CFE_ES_DeviceServerCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with 
**       the following telemetry:
**       - TBD
**
**  \par Error Conditions
**       TBD
**
**  \par Criticality
**       TBD
**
**  \sa 
*/
#define CFE_ES_DEVICE_DRIVER_ENABLE_CC         26

#endif
