/************************************************************************************************
** File:  cfe_psp_watchdog.c
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
**   2009/07/20  A. Cudmore    | Initial version,
**   2014/08/08  S. Duran   | Modified for ut699 SPARC LEON3 board -- TODO
**
*************************************************************************************************/

/*
**  Include Files
*/


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "vxLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
/* RAM
#include "drv/ppci/ppciPtim.h"
*/

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"

#define TIMER4_COUNTER        0x80000340
#define TIMER4_RELOAD         0x80000344
#define TIMER4_CONTROL        0x80000348

#define TIMER4_CONTROL_ENA    0x00000001  /* Enable watchdog timer */
#define TIMER4_CONTROL_DIS    0x00000001  /* Disable watchdog timer */
#define TIMER4_CONTROL_LOAD   0x00000004  /* Reload watchdog timer */

#define TIMER4_CONTROL_DH     0x00000040  /* Debug Halt */
#define TIMER4_CONTROL_CH     0x00000020  /* Chain */
#define TIMER4_CONTROL_IP     0x00000010  /* Interrupt pending */
#define TIMER4_CONTROL_IE     0x00000008  /* Interrupt Enable */
#define TIMER4_CONTROL_LD     0x00000004  /* Load  */
#define TIMER4_CONTROL_RS     0x00000002  /* Restart */
#define TIMER4_CONTROL_EN     0x00000001  /* Enable */


/*
** Global data
*/

/*
** The watchdog time in milliseconds
*/
uint32 CFE_PSP_WatchdogValue = CFE_PSP_WATCHDOG_MAX;

/*  Function:  CFE_PSP_WatchdogInit()
**
**  Purpose:
**    To setup the timer resolution and/or other settings custom to this platform.
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogInit(void)
{
    CFE_PSP_WatchdogValue = CFE_PSP_WATCHDOG_MAX;

    /*
    OS_printf("WatchdogInit Tmr: %08X\n", CFE_PSP_WatchdogGet());
    OS_printf("WatchdogInit Lod: %08X\n", *(unsigned long *)(0x80000344));
    OS_printf("WatchdogInit ctl: %08X\n", *(unsigned long *)(0x80000348));
    */

    *(unsigned long *)TIMER4_CONTROL = TIMER4_CONTROL_RS;

    *(unsigned long *)TIMER4_COUNTER = CFE_PSP_WATCHDOG_MAX;

    *(unsigned long *)TIMER4_RELOAD = CFE_PSP_WATCHDOG_MAX;

    /*
    OS_printf("WatchdogInit Tmr: %08X\n", CFE_PSP_WatchdogGet());
    OS_printf("WatchdogInit Lod: %08X\n", *(unsigned long *)(0x80000344));
    OS_printf("WatchdogInit ctl: %08X\n", *(unsigned long *)(0x80000348));
    */

    return;
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogEnable()
**
**  Purpose:
**    Enable the watchdog timer
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogEnable(void)
{
   unsigned long reg = *(unsigned long *)TIMER4_CONTROL;
   
         OS_printf("WatchdogEna Tmr: %08X\n", CFE_PSP_WatchdogGet());
         OS_printf("WatchdogEna Lod: %08X\n", *(unsigned long *)(0x80000344));
         OS_printf("WatchdogEna ctl: %08X\n", *(unsigned long *)(0x80000348));
   
   /*
   ** Enable the timer is logic zero
   */
   
   reg &= ~TIMER4_CONTROL_IP;   /* clear an interrupt pending */
   
   reg |= TIMER4_CONTROL_IE;    /* Enable interrupt */
   
   reg |= TIMER4_CONTROL_EN;    /* Enable timer */
   
   *(unsigned long *)TIMER4_CONTROL = reg;

         OS_printf("WatchdogEna Tmr: %08X\n", CFE_PSP_WatchdogGet());
         OS_printf("WatchdogEna Lod: %08X\n", *(unsigned long *)(0x80000344));
         OS_printf("WatchdogEna ctl: %08X\n", *(unsigned long *)(0x80000348));
   
   return;
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogDisable()
**
**  Purpose:
**    Disable the watchdog timer
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogDisable(void)
{
   unsigned long reg = *(unsigned long *)TIMER4_CONTROL;
   
   /*
   ** Disable the timer
   */
   
   *(unsigned long *)TIMER4_CONTROL = reg & ~TIMER4_CONTROL_EN;
   
   return;
}

/******************************************************************************
**  Function:  CFE_PSP_WatchdogService()
**
**  Purpose:
**    Load the watchdog timer with a count that corresponds to the millisecond
**    time given in the parameter.
**
**  Arguments:
**    None.
**
**  Return:
**    None
**
**  Notes:
**    Rad750 watchdog register max count is 0xFFFFFFFF.
**    This turns out to be a time of 34.7 minutes(2082secs) which is calculated by:
**    (66.0MHz/8)/4) = 2062500 ticks/second (given by BAE)
**    484.85nS/tick * 0xFFFFFFFF = 2082 seconds.
**
*/
void CFE_PSP_WatchdogService(void)
{
   unsigned long reg = *(unsigned long *)TIMER4_CONTROL;
   
   /*
   ** Reload the timer with value in the reload reload register
   */
   
   *(unsigned long *)TIMER4_CONTROL = reg | TIMER4_CONTROL_LD;
   
   return;
}

/******************************************************************************
**  Function:  CFE_PSP_WatchdogGet
**
**  Purpose:
**    Get the current watchdog value. 
**
**  Arguments:
**    none 
**
**  Return:
**    the current watchdog value 
**
**  Notes:
**
*/
uint32 CFE_PSP_WatchdogGet(void)
{
   unsigned long reg = *(unsigned long *)TIMER4_COUNTER;
   
   return reg;
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogSet
**
**  Purpose:
**    Get the current watchdog value. 
**
**  Arguments:
**    The new watchdog value 
**
**  Return:
**    nothing 
**
**  Notes:
**
*/
void CFE_PSP_WatchdogSet(uint32 WatchdogValue)
{
   *(unsigned long *)TIMER4_COUNTER = WatchdogValue;

   return;
}

