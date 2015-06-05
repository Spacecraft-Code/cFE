/*
**  File Name:  cfe_psp_memory.h
**
**  Author:  A. Cudmore
**
**  Purpose:  This file contains PSP support routine internal prototypes 
**            and typedefs. The routines and typedefs are intended for 
**            the local PSP routines only. 
**
**  Modification History:
**
*/

#ifndef _cfe_psp_memory_
#define _cfe_psp_memory_

/*
** Include Files
*/

#include "common_types.h"
#include "cfe_platform_cfg.h"
#include "cfe_psp.h"

/*
** Macro Definitions
*/
#define CFE_PSP_CDS_SIZE            CFE_ES_CDS_SIZE
#define CFE_PSP_RESET_AREA_SIZE     CFE_ES_RESET_AREA_SIZE
#define CFE_PSP_VOLATILE_DISK_SIZE  (CFE_ES_RAM_DISK_SECTOR_SIZE * CFE_ES_RAM_DISK_NUM_SECTORS)
#define CFE_PSP_USER_RESERVED_SIZE  CFE_ES_USER_RESERVED_SIZE


/*
** Typedef for the layout of the vxWorks USER_RESERVED_MEM
*/
typedef struct
{
   uint32 bsp_reset_type;
   uint32 spare1;
   uint32 spare2;
   uint32 spare3;
   uint8  ResetMemory[CFE_PSP_RESET_AREA_SIZE];
   uint8  VolatileDiskMemory[CFE_PSP_VOLATILE_DISK_SIZE]; 
   uint8  CDSMemory[CFE_PSP_CDS_SIZE];
   uint8  UserReservedMemory[CFE_PSP_USER_RESERVED_SIZE];

} CFE_PSP_ReservedMemory_t;

/*
** External variables
*/

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
extern CFE_PSP_ReservedMemory_t *CFE_PSP_ReservedMemoryPtr; 

#endif  /* _cfe_psp_memory_ */
