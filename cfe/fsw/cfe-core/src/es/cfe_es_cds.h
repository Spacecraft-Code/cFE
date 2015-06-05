/*
**  File: 
**  cfe_es_cds.h
**
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
**  This file contains the Internal interface for the cFE Critical Data Store functions.
**  These functions and data structures manage the Critical Data Store in the cFE.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**  $Log: cfe_es_cds.h  $
**  Revision 1.5 2012/01/13 11:50:01GMT-05:00 acudmore 
**  Changed license text to reflect open source
**  Revision 1.4 2010/10/04 17:01:27EDT jmdagost 
**  Cleaned up copyright symbol.
**  Revision 1.3 2009/06/10 09:08:54EDT acudmore 
**  Converted OS_Mem* and OS_BSP* API to CFE_PSP_* API
**  Revision 1.2 2008/12/08 16:10:52EST dkobe 
**  Correcting errors generated during detailed design document generation
**  Revision 1.1 2008/04/17 08:05:03EDT ruperera 
**  Initial revision
**  Member added to project c:/MKSDATA/MKS-REPOSITORY/MKS-CFE-PROJECT/fsw/cfe-core/src/es/project.pj
**  Revision 1.3.1.6 2007/05/15 11:16:06EDT apcudmore 
**  Added modification log tags.
*/


#ifndef _cfe_es_cds_
#define _cfe_es_cds_

/*
** Include Files
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_es_apps.h"
#include "cfe_platform_cfg.h"
#include "cfe_es.h"
#include "cfe_es_cds_mempool.h"

/*
** Macro Definitions
*/

/** \name Registry Mutex Definitions */
/**  \{ */
#define CFE_ES_CDS_MUT_REG_NAME       "CDS_REG_MUT"  /**< \brief Name of Mutex controlling CDS Registry Access */
#define CFE_ES_CDS_MUT_REG_VALUE       0             /**< \brief Initial Value of CDS Registry Access Mutex */

#define CFE_ES_CDS_NOT_FOUND          (uint32)(0xffffffff)
/*
** Type Definitions
*/

typedef struct
{
    char                      Name[CFE_ES_CDS_MAX_FULL_NAME_LEN];
    CFE_ES_CDSBlockHandle_t   MemHandle;
    uint32                    Size;           /**< \brief Size, in bytes, of the CDS memory block */
    boolean                   Taken;          /**< \brief Flag that indicates whether the registry record is in use */
    boolean                   Table;          /**< \brief Flag that indicates whether CDS contains a Critical Table */
} CFE_ES_CDS_RegRec_t;

typedef struct
{
    uint32               RegistryMutex;                         /**< \brief Mutex that controls access to CDS Registry */
    uint32               CDSSize;                               /**< \brief Total size of the CDS as reported by BSP */
    uint32               MemPoolSize;
    uint32               MaxNumRegEntries;                      /**< \brief Maximum number of Registry entries */
    CFE_ES_CDS_RegRec_t  Registry[CFE_ES_CDS_MAX_NUM_ENTRIES];  /**< \brief CDS Registry (Local Copy) */
    char                 ValidityField[8];
} CFE_ES_CDSVariables_t;

/*****************************************************************************/
/*
** Function prototypes
*/

/*
** CFE_ES_CDS_EarlyInit
*/

int32 CFE_ES_CDS_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Copies the local version of the CDS Registry to the actual CDS
**
** \par Description
**        Copies the local working copy of the CDS Registry to the CDS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \return Any of the return values from #CFE_PSP_WriteToCDS
**                     
******************************************************************************/
int32 CFE_ES_UpdateCDSRegistry(void);


/*****************************************************************************/
/**
** \brief Validates the Application ID associated with calling Application
**
** \par Description
**        Validates Application ID of calling App.  Validation
**        consists of ensuring the AppID is between zero and
**        #CFE_ES_MAX_APPLICATIONS.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  AppIdPtr Pointer to value that will hold AppID on return.
** 
** \param[out] *AppIdPtr The AppID as obtained from #CFE_ES_GetAppID
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_ES_ERR_APPID                \copydoc CFE_ES_ERR_APPID
**                     
******************************************************************************/
int32 CFE_ES_CDS_ValidateAppID(uint32 *AppIdPtr);


/*****************************************************************************/
/**
** \brief Creates a Full CDS name from application name and CDS name
**
** \par Description
**        Takes a given CDS Name and combines it with the calling
**        Application's name to make a processor specific name of the
**        form: "AppName.CDSName"
**
** \par Assumptions, External Events, and Notes:
**        Note: AppName portion will be truncated to OS_MAX_API_NAME.
**
** \param[in]  FullCDSName pointer to character buffer of #CFE_ES_CDS_MAX_FULL_NAME_LEN size
**                         that will be filled with the processor specific CDS Name.
** 
** \param[in]  CDSName pointer to character string containing the Application's local name for
**                     the CDS.
**
** \param[in]  ThisAppId the Application ID of the Application making the call. 
**
** \param[out] *FullCDSName processor specific CDS Name of the form "AppName.CDSName".
**
** \retval None                     
******************************************************************************/
void CFE_ES_FormCDSName(char *FullCDSName, const char *CDSName, uint32 ThisAppId);

/*****************************************************************************/
/**
** \brief Returns the Registry Index for the specified CDS Name
**
** \par Description
**        Locates given CDS Name in the CDS Registry and
**        returns the appropriate Registry Index.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CDSName - Pointer to character string containing complete
**                       CDS Name (of the format "AppName.CDSName").
** 
** \retval #CFE_ES_CDS_NOT_FOUND or the Index into Registry for Table with specified name
**                     
******************************************************************************/
int32  CFE_ES_FindCDSInRegistry(const char *CDSName);

/*****************************************************************************/
/**
** \brief Locates a free slot in the CDS Registry.
**
** \par Description
**        Locates a free slot in the CDS Registry.
**
** \par Assumptions, External Events, and Notes:
**        Note: This function assumes the registry has been locked.
**
** \retval #CFE_ES_CDS_NOT_FOUND or Index into CDS Registry of unused entry                     
******************************************************************************/
int32  CFE_ES_FindFreeCDSRegistryEntry(void);

/*****************************************************************************/
/**
** \brief Locks access to the CDS Registry
**
** \par Description
**        Locks the CDS Registry to prevent multiple tasks/threads
**        from modifying it at once.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS                     
******************************************************************************/
int32   CFE_ES_LockCDSRegistry(void);

/*****************************************************************************/
/**
** \brief Unlocks access to the CDS Registry
**
** \par Description
**        Unlocks CDS Registry to allow other tasks/threads to
**        modify the CDS Registry contents.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**                     
******************************************************************************/
int32   CFE_ES_UnlockCDSRegistry(void);

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine is identical to #CFE_ES_RegisterCDS except it identifies the contents
**        of the CDS as a critical table.  This is crucial because a critical table CDS must
**        only be deleted by cFE Table Services, not via an ES delete CDS command.  Otherwise,
**        Table Services may be out of sync with the contents of the CDS. 
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes input parameters are error free and have met size/value restrictions.
**        -# The calling function is responsible for issuing any event messages associated with errors.
**
** \param[in]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle.
**
** \param[in]   BlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        Pointer to character string containing the Application's local name for
**                          the CDS.
**
** \param[in]   CriticalTbl   Indicates whether the CDS is to be used as a Critical Table or not
**
** \param[out]  *HandlePtr  The handle of the CDS block that can be used in #CFE_ES_CopyToCDS and #CFE_ES_RestoreFromCDS.
**
** \return See return codes for #CFE_ES_RegisterCDS
**
******************************************************************************/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name, boolean CriticalTbl);

/*****************************************************************************/
/**
** \brief Deletes the specified CDS from the CDS Registry and frees CDS Memory
**
** \par Description
**        Removes the record of the specified CDS from the CDS Registry and
**        frees the associated CDS memory for future use.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  CDSName - Pointer to character string containing complete
**                       CDS Name (of the format "AppName.CDSName").
**
** \param[in]  CalledByTblServices - Flag that identifies whether the CDS is supposed to
**                       be a Critical Table Image or not.
** 
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_WRONG_TYPE_ERR       \copydoc CFE_ES_CDS_WRONG_TYPE_ERR
** \return #CFE_ES_CDS_OWNER_ACTIVE_ERR     \copydoc CFE_ES_CDS_OWNER_ACTIVE_ERR
** \return #CFE_ES_CDS_NOT_FOUND_ERR        \copydoc CFE_ES_CDS_NOT_FOUND_ERR
** \return Any of the return values from #CFE_ES_UpdateCDSRegistry
** \return Any of the return values from #CFE_ES_PutCDSBlock
**                     
******************************************************************************/
int32  CFE_ES_DeleteCDS(const char *CDSName, boolean CalledByTblServices);

#endif  /* _cfe_es_cds_ */
