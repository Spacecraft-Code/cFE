/******************************************************************************
** File:  cfe_psp_memory.c
**
**      mcf5235 Coldfire RTEMS version
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
**   cFE PSP Memory related functions. This is the implementation of the cFE 
**   memory areas that have to be preserved, and the API that is designed to allow
**   acccess to them. It also contains memory related routines to return the
**   address of the kernel code used in the cFE checksum.
**
** History:
**   2007/09/23  A. Cudmore      | m5235bcc Coldfire RTEMS version
**
******************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>

/*
** cFE includes 
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h" 
#include "cfe_psp_memory.h"           

/*
**  External Declarations
*/

/*
** Global variables
*/

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
CFE_PSP_ReservedMemory_t *CFE_PSP_ReservedMemoryPtr; 

/*
*********************************************************************************
** CDS related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetCDSSize
**
**  Purpose: 
**    This function fetches the size of the OS Critical Data Store area.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
   int32 return_code;
   
   if ( SizeOfCDS == NULL )
   {
       return_code = OS_ERROR;
   }
   else
   {
       *SizeOfCDS = CFE_PSP_CDS_SIZE;
       return_code = OS_SUCCESS;
   }
   return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_WriteToCDS
**
**  Purpose:
**    This function writes to the CDS Block.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_WriteToCDS(void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
         
   if ( PtrToDataToWrite == NULL )
   {
       return_code = OS_ERROR;
   }
   else
   {
       if ( (CDSOffset < CFE_PSP_CDS_SIZE ) && ( (CDSOffset + NumBytes) <= CFE_PSP_CDS_SIZE ))
       {
          CopyPtr = &(CFE_PSP_ReservedMemoryPtr->CDSMemory[CDSOffset]);
          memcpy(CopyPtr, (char *)PtrToDataToWrite,NumBytes);
          
          return_code = OS_SUCCESS;
       }
       else
       {
          return_code = OS_ERROR;
       }
       
   } /* end if PtrToDataToWrite == NULL */
   
   return(return_code);
}


/******************************************************************************
**  Function: CFE_PSP_ReadFromCDS
**
**  Purpose:
**   This function reads from the CDS Block
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
      
   if ( PtrToDataToRead == NULL )
   {
       return_code = OS_ERROR;
   }
   else
   {
       if ( (CDSOffset < CFE_PSP_CDS_SIZE ) && ( (CDSOffset + NumBytes) <= CFE_PSP_CDS_SIZE ))
       {
          CopyPtr = &(CFE_PSP_ReservedMemoryPtr->CDSMemory[CDSOffset]);
          memcpy((char *)PtrToDataToRead,CopyPtr, NumBytes);
          
          return_code = OS_SUCCESS;
       }
       else
       {
          return_code = OS_ERROR;
       }
       
   } /* end if PtrToDataToWrite == NULL */
   
   return(return_code);
   
}

/*
*********************************************************************************
** ES Reset Area related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetResetArea
**
**  Purpose:
**     This function returns the location and size of the ES Reset information area. 
**     This area is preserved during a processor reset and is used to store the 
**     ER Log, System Log and reset related variables
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetResetArea (void *PtrToResetArea, uint32 *SizeOfResetArea)
{
   int32   return_code;
   uint32 *TempPointer;
   
   if ( SizeOfResetArea == NULL )
   {
      return_code = OS_ERROR;
   }
   else
   {
      TempPointer = (uint32 *)&(CFE_PSP_ReservedMemoryPtr->ResetMemory[0]);
      memcpy(PtrToResetArea,&(TempPointer),sizeof(PtrToResetArea));
      *SizeOfResetArea = CFE_PSP_RESET_AREA_SIZE;
      return_code = OS_SUCCESS;
   }
   
   return(return_code);
}

/*
*********************************************************************************
** ES User Reserved Area related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetUserReservedArea
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**     User reserved area.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetUserReservedArea(void *PtrToUserArea, uint32 *SizeOfUserArea )
{
   int32   return_code;
   uint32 *TempPointer;
   
   if ( SizeOfUserArea == NULL )
   {
      return_code = OS_ERROR;
   }
   else
   {
      TempPointer = (uint32 *)&(CFE_PSP_ReservedMemoryPtr->UserReservedMemory[0]);
      memcpy(PtrToUserArea,&(TempPointer),sizeof(PtrToUserArea));
      *SizeOfUserArea = CFE_PSP_USER_RESERVED_SIZE;
      return_code = OS_SUCCESS;
   }
   
   return(return_code);
}

/*
*********************************************************************************
** ES Volatile disk memory related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetVolatileDiskMem
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**     volatile disk.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetVolatileDiskMem(void *PtrToVolDisk, uint32 *SizeOfVolDisk )
{
   int32   return_code;
   uint32 *TempPointer;
   
   if ( SizeOfVolDisk == NULL )
   {
      return_code = OS_ERROR;
   }
   else
   {
      TempPointer = (uint32 *)&(CFE_PSP_ReservedMemoryPtr->VolatileDiskMemory[0]);
      memcpy(PtrToVolDisk,&(TempPointer),sizeof(PtrToVolDisk));
      *SizeOfVolDisk = CFE_PSP_VOLATILE_DISK_SIZE;
      return_code = OS_SUCCESS;

   }
   
   return(return_code);
   
}

/*
*********************************************************************************
** ES BSP Top Level Reserved memory initialization
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_InitProcessorReservedMemory
**
**  Purpose:
**    This function performs the top level reserved memory initialization.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_InitProcessorReservedMemory( uint32 RestartType )
{
   int32 return_code;
 
   if ( RestartType != CFE_ES_PROCESSOR_RESET )
   {
      OS_printf("CFE_PSP: Clearing Processor Reserved Memory.\n");
      memset((void *)CFE_PSP_ReservedMemoryPtr, 0, sizeof(CFE_PSP_ReservedMemory_t));
      
      /*
      ** Set the default reset type in case a watchdog reset occurs 
      */
      CFE_PSP_ReservedMemoryPtr->bsp_reset_type = CFE_ES_PROCESSOR_RESET;

   }      
   return_code = OS_SUCCESS;
   return(return_code);

}


/*
*********************************************************************************
** ES BSP kernel memory segment functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetKernelTextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the kernel text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetKernelTextSegmentInfo(void *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
   int32 return_code;
   uint32 StartAddress;
   uint32 EndAddress;
   
   if ( SizeOfKernelSegment == NULL )
   {
      return_code = OS_ERROR;
   }
   else
   {
      /*
      ** Get the kernel start and end
      ** addresses from the BSP, because the 
      ** symbol table does not contain the symbls we need for this
      */
      StartAddress = (uint32) 0x0;
      EndAddress = (uint32) 0x1024;

      memcpy(PtrToKernelSegment,&StartAddress,sizeof(PtrToKernelSegment));
      *SizeOfKernelSegment = (uint32) (EndAddress - StartAddress);
      
      return_code = OS_SUCCESS;
   }
   
   return(return_code);
}


/******************************************************************************
**  Function: CFE_PSP_GetCFETextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the CFE text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_GetCFETextSegmentInfo(void *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
   int32 return_code;
   uint32 StartAddress;
   uint32 EndAddress;
   
   if ( SizeOfCFESegment == NULL )
   {
      return_code = OS_ERROR;
   }
   else
   {
      /*
      ** Get the kernel start and end
      ** addresses from the BSP, because the 
      ** symbol table does not contain the symbls we need for this
      */
      StartAddress = (uint32) 0x0;
      EndAddress = (uint32) 0x1024;

      memcpy(PtrToCFESegment,&StartAddress,sizeof(PtrToCFESegment));
      *SizeOfCFESegment = (uint32) (EndAddress - StartAddress);
      
      return_code = OS_SUCCESS;
   }
   
   return(return_code);
}


extern int rtems_shell_main_wkspace_info(int argc,char *argv[]);
extern int rtems_shell_main_malloc_info( int argc, char *argv[]);


void CFE_PSP_MemReport(char *message)
{

   int MemStatus;
   char *malloc_argv[] = {"malloc", "info"};
   char *wkspace_argv[] = {"wkspace", "info"};
   
   OS_printf("------------------------ Memory Stat Report ----------------------------\n");
   OS_printf(" Called from: %s\n",message);
   OS_printf("------------------------ Dumping Malloc Stats: ----------------------------\n");
   MemStatus = rtems_shell_main_malloc_info(2, malloc_argv);
   OS_printf("------------------------ Dumping Wkspace Stats: ----------------------------\n");
   MemStatus = rtems_shell_main_wkspace_info(2, wkspace_argv);
   OS_printf("------------------------ Done ----------------- ----------------------------\n");
   
}

