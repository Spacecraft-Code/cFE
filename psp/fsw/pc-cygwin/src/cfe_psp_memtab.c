/*
** File   :	cfe_psp_memtab.c
**
**
**      Copyright (c) 2004-2006, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. 
**
** Author :     Alan Cudmore	
**
** Purpose:     Memory Range Table for cFE/PSP.
**
**
*/

/*
** Includes
*/

#include "common_types.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>


/*
** Defines
*/
#define EEPROM_FILE "EEPROM.DAT"

/*
** Valid memory map for this target.
** If you need to add more entries, increase CFE_PSP_MEM_TABLE_SIZE in the osconfig.h file.
*/
CFE_PSP_MemTable_t CFE_PSP_MemoryTable[CFE_PSP_MEM_TABLE_SIZE] = 
{
   { CFE_PSP_MEM_RAM, CFE_PSP_MEM_SIZE_DWORD, 0, 0xFFFFFFFF, CFE_PSP_MEM_ATTR_READWRITE },
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

/*
** Simulate EEPROM by mapping in a file
*/
int32 CFE_PSP_SetupEEPROM(uint32 EEPROMSize, uint32 *EEPROMAddress)
{
   
   int          FileDescriptor;
   int          ReturnStatus;
   char        *DataBuffer;
   struct stat  StatBuf;
   
   /* 
   ** Check to see if the file has been created.
   ** If not, create it.
   ** If so, then open it for read/write
   */ 
   ReturnStatus = stat(EEPROM_FILE, &StatBuf);
   if ( ReturnStatus == -1 )
   {  
      /*
      ** File does not exist, create it.
      */
      FileDescriptor = open(EEPROM_FILE, O_RDWR | O_CREAT, S_IRWXU);
      if ( FileDescriptor == -1 )
      {
         OS_printf("CFE_PSP: Cannot open EEPROM File: %s\n",EEPROM_FILE);
         return(-1);
      }     
      else
      {
         /*
         ** Need to seek to the desired EEPROM size
         */
         if (lseek (FileDescriptor, EEPROMSize - 1, SEEK_SET) == -1)
         {
            OS_printf("CFE_PSP: Cannot Seek to end of EEPROM file.\n");
            close(FileDescriptor);
            return(-1);
         }

         /* 
         ** Write a byte at the end of the File 
         */
         if (write (FileDescriptor, "", 1) != 1)
         {
            OS_printf("CFE_PSP: Cannot write to EEPROM file\n");
            close(FileDescriptor);
            return(-1);
         }        
      }
   }
   else 
   {  
      /*
      ** File exists
      */
      FileDescriptor = open(EEPROM_FILE, O_RDWR);
      if ( FileDescriptor == -1 )
      {
         OS_printf("CFE_PSP: Cannot open EEPROM File: %s\n",EEPROM_FILE);
         perror("CFE_PSP: open");
         return(-1);
      }
   }
      
   /*
   ** Map the file to a memory space
   */
   if ((DataBuffer = mmap((caddr_t)0, EEPROMSize, PROT_READ | PROT_WRITE, MAP_SHARED, FileDescriptor, 0)) == (caddr_t)(-1)) 
   {
      OS_printf("CFE_PSP: mmap to EEPROM File failed\n");
      close(FileDescriptor);
      return(-1);
   }
  
   /*
   ** Return the address to the caller
   */
   *EEPROMAddress = (uint32)DataBuffer; 
   
   return(0);
}
