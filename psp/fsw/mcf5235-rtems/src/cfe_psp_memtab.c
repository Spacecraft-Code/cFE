/*
** File   :	cfe_psp_memtab.c
**
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. 
**
** Author : Alan Cudmore	
**
** Purpose: define the memory table for the memory validation API
**
**
*/

/*
** Includes
*/

#include "common_types.h"
#include "osapi.h"
#include "cfe_psp.h"


/*
** Valid memory map for this target.
** If you need to add more entries, increase CFE_PSP_MEM_TABLE_SIZE in the osconfig.h file.
*/
CFE_PSP_MemTable_t CFE_PSP_MemoryTable[CFE_PSP_MEM_TABLE_SIZE] = 
{
   { CFE_PSP_MEM_RAM, CFE_PSP_MEM_SIZE_DWORD, 0, 0x1000000, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
   { CFE_PSP_MEM_INVALID, 0, 0, 0, CFE_PSP_MEM_ATTR_READWRITE },
};
