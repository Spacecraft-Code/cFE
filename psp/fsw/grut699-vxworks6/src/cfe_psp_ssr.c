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
#include "cacheLib.h"
#include "xbdBlkDev.h"
#include "xbdRamDisk.h"
#include <hrFsLib.h>
#include <xbdPartition.h>


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


     

#define  BLOCKSIZE         512
#define  DISKSIZE          (BLOCKSIZE * 2000)


/******************************************************************************
**  Function:  CFE_PSP_InitSSR
**
**  Purpose:
**    Initializes the Solid State Recorder device. For the grut699, this simply
**    creates a ram drive

**
**  Arguments:
**    DeviceName
**
**  Return:
**    (none)
*/

int32 CFE_PSP_InitSSR( uint32 bus, uint32 device, char * DeviceName )
{
   int32     ReturnCode = CFE_PSP_ERROR;
   device_t  xbd = NULLDEV;
   STATUS error = OK;

#ifdef USE_VXWORKS_ATA_DRIVER
   xbd = xbdRamDiskDevCreate (BLOCKSIZE, DISKSIZE, 0, DeviceName);
   if (xbd == NULLDEV)
   {
       ReturnCode = CFE_PSP_ERROR;
   }
   else
   {
      /* 
      ** Format the RAM disk for HRFS. Allow for upto a 1000 files/directories 
      ** and let HRFS determine the logical block size.
      */

      error = hrfsFormat(DeviceName, 0ll, 0, 1000);
      if (error != OK)
      {
         ReturnCode = CFE_PSP_ERROR;
      }
      else
      {
         ReturnCode = CFE_PSP_SUCCESS;
      }
   }
#endif

   return(ReturnCode);
   
}

