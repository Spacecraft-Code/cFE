/******************************************************************************
** File:  cfe_psp_ssr.c
**
**
**      Copyright (c) 2004-2006, United States government as represented by the
**      administrator of the National Aeronautics Space Administration.
**      All rights reserved. This software(cFE) was created at NASA Goddard
**      Space Flight Center pursuant to government contracts.
**
**      This software may be used only pursuant to a United States government
**      sponsored project and the United States government may not be charged
**      for use thereof.
**
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  Alan Cudmore    | Initial version,
**
******************************************************************************/


/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "drv/hdisk/ataDrv.h"
#include "cacheLib.h"
#include "xbdBlkDev.h"
#include "xbdRamDisk.h"


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_mission_cfg.h"   /* for spacecraft ID */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"


/******************************************************************************
**  Function:  CFE_PSP_InitSSR
**
**  Purpose:
**    Initializes the Solid State Recorder device. For the MCP750, this simply
**    initializes the Hard disk device.

**
**  Arguments:
**    bus, device, device name
**
**  Return:
**    (none)
*/

int32 CFE_PSP_InitSSR(uint32 bus, uint32 device, char *DeviceName )
{
   int32     ReturnCode;
   device_t  xbd;
   
   xbd = ataXbdDevCreate(bus, device, 0, 0, DeviceName);
   
   if (xbd == NULLDEV)
   {
       ReturnCode = CFE_PSP_ERROR;
   }
   else
   {
       ReturnCode = CFE_PSP_SUCCESS;
   }

   return(ReturnCode);
   
}

