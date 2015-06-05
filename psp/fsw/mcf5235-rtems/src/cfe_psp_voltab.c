/*
** File   : cfe_psp_voltab.c
** Author : Nicholas Yanchik / GSFC Code 582
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
** OS Volume table for file systems
*/

/****************************************************************************************
                                    INCLUDE FILES
****************************************************************************************/
#include "common_types.h"
#include "osapi.h"


/* 
**  volume table. This table has the OS_ name, since it belongs to the OSAL, not the CFE_PSP
*/
OS_VolumeInfo_t OS_VolumeTable [NUM_TABLE_ENTRIES] = 
{
/* Dev Name  Phys Dev  Vol Type        Volatile?  Free?     IsMounted? Volname  MountPt BlockSz */

/* cFE RAM Disk */
{ "/ramdev0", "/dev/nvda",  RAM_DISK,     TRUE,      TRUE,     FALSE,     " ",   " ",     512        },

/* cFE non-volatile Disk -- Auto-Mapped to an existing CF disk */
{"/eedev0",  "/eeprom",  FS_BASED,  FALSE,     FALSE,  TRUE,  "CF",  "/eeprom",        512   },

/* 
** Spare RAM disks to be used for SSR and other RAM disks 
*/
{"/ramdev1", "/ram1",      FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"/ramdev2", "/ram2",      FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"/ramdev3", "/ram3",      FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"/ramdev4", "/ram4",      FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"/ramdev5", "/ram5",      FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },

/* 
** Hard disk mappings 
*/
{"/ssrdev0",  "/ssr",    FS_BASED,      TRUE,   FALSE,   TRUE,  "SSR",      "/O1/ssr",     512  },
{"/ssrdev1",  "/ssr2",   FS_BASED,      TRUE,   TRUE,    FALSE,  " ",      " ",     0        },
{"/ssrdev2",  "/ssr3",   FS_BASED,      TRUE,   TRUE,    FALSE,  " ",      " ",     0        },

{"unused",   "unused",    FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",    FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",    FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        },
{"unused",   "unused",    FS_BASED,        TRUE,      TRUE,     FALSE,     " ",      " ",     0        }

};



