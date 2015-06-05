/*
**      Copyright (c) 2004-2012, United States government as represented by the 
**      administrator of the National Aeronautics Space Administration.  
**      All rights reserved. This software(cFE) was created at NASA's Goddard 
**      Space Flight Center pursuant to government contracts.
**
**      This is governed by the NASA Open Source Agreement and may be used, 
**      distributed and modified only pursuant to the terms of that agreement. 
**
** File:
**    fss_UT.c
**
** Purpose:
**    File Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
** $Date: 2014/05/28 09:21:47GMT-05:00 $
** $Revision: 1.9 $
**
*/

/*
** Includes
*/
#include "fs_UT.h"

/*
** External global variables
*/
extern uint32 gz_outcnt;
extern int32  gGuzError;

extern UT_SetRtn_t FileWriteRtn;
extern UT_SetRtn_t OSlseekRtn;
extern UT_SetRtn_t MutSemGiveRtn;
extern UT_SetRtn_t MutSemTakeRtn;
extern UT_SetRtn_t OSReadRtn; 
extern UT_SetRtn_t OSReadRtn2; 
extern UT_SetRtn_t FileWriteRtn; 
extern UT_SetRtn_t WriteSysLogRtn;

/*
** Functions
*/
#ifdef CFE_LINUX
int main(void)
#else
int fs_main(void)
#endif
{
    /* Initialize unit test */
    UT_Init("fs");
    UT_Text("cFE FS Unit Test Output File\n\n");

    /* Perform tests */
    Test_CFE_FS_API();
    Test_CFE_FS_Private();
    Test_CFE_FS_Decompress();

    UT_ReportFailures();
    return 0;
}

/*
** Tests for FS API functions (cfe_fs_api.c)
*/
void Test_CFE_FS_API(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test API\n");
#endif

    Test_CFE_FS_ReadHeader();
    Test_CFE_FS_WriteHeader();
    Test_CFE_FS_SetTimestamp();
    Test_CFE_FS_ByteSwapCFEHeader();
    Test_CFE_FS_ByteSwapUint32();
    Test_CFE_FS_IsGzFile();
    Test_CFE_FS_ExtractFileNameFromPath();

#ifdef UT_VERBOSE
    UT_Text("End Test API\n\n");
#endif
}

/*
** Test FS API read header function
*/
void Test_CFE_FS_ReadHeader(void)
{
    int32 FileDes = 0;
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Read Header\n");
#endif

    /* Test reading the header with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(CFE_FS_ReadHeader(&Hdr, FileDes) != OS_LSEEK_FAIL,
              "CFE_FS_ReadHeader",
              "Header read lseek failed",
              "01.001");

    /* Test successfully reading the header */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, OS_FS_SUCCESS, 1);
    UT_SetOSFail(OS_READ_FAIL);
    UT_Report(CFE_FS_ReadHeader(&Hdr, FileDes) == (sizeof(CFE_FS_Header_t) - 1),
              "CFE_FS_ReadHeader",
              "Header read - successful",
              "01.002");
}

/*
** Test FS API write header function
*/
void Test_CFE_FS_WriteHeader(void)
{
    int32 FileDes = 0;
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Write Header\n");
#endif

    /* Test writing the header with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(CFE_FS_WriteHeader(FileDes, &Hdr) != OS_LSEEK_FAIL,
              "CFE_FS_WriteHeader",
              "Header write lseek failed",
              "02.001");

    /* Test successfully writing the header */
    UT_InitData();
    UT_SetRtnCode(&OSlseekRtn, OS_FS_SUCCESS, 1);
    UT_SetRtnCode(&FileWriteRtn, OS_FS_SUCCESS, 1);
    UT_Report(CFE_FS_WriteHeader(FileDes, &Hdr) == OS_FS_SUCCESS,
              "CFE_FS_WriteHeader",
              "Header write - successful",
              "02.002");
}

/*
** Test FS API set time stamp function
*/
void Test_CFE_FS_SetTimestamp(void)
{
    int32 FileDes = 0;
    CFE_TIME_SysTime_t NewTimestamp = {0, 0};

#ifdef UT_VERBOSE
    UT_Text("Begin Test Set Time Stamp\n");
#endif

    /* Test setting the time stamp with a lseek failure */
    UT_InitData();
    UT_SetOSFail(OS_LSEEK_FAIL);
    UT_Report(CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_FS_ERROR,
              "CFE_FS_SetTimestamp",
              "Failed to lseek time fields",
              "03.001");

    /* Test setting the time stamp with a seconds write failure */
    UT_InitData();
    UT_SetOSFail(OS_WRITE_FAIL);
    UT_Report(CFE_FS_SetTimestamp(FileDes, NewTimestamp) != OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write seconds",
              "03.002");

    /* Test setting the time stamp with a subeconds write failure */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, 0, 2);
    UT_Report(CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write subseconds",
              "03.003");

    /* Test successfully setting the time stamp */
    UT_InitData();
    UT_Report(CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_FS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Write time stamp - successful",
              "03.004");
}

/*
** Test FS API byte swap cFE header function
*/
void Test_CFE_FS_ByteSwapCFEHeader(void)
{
    CFE_FS_Header_t Hdr;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Byte Swap cFE Header\n");
#endif

    UT_InitData();
    Hdr.ContentType = 0x11223344;
    Hdr.SubType = 0x22334455;
    Hdr.Length = 0x33445566;
    Hdr.SpacecraftID = 0x44556677;
    Hdr.ProcessorID = 0x55667788;
    Hdr.ApplicationID = 0x66778899;
    Hdr.TimeSeconds = 0x778899aa;
    Hdr.TimeSubSeconds = 0x8899aabb;

    /* Test byte-swapping the header values */
    CFE_FS_ByteSwapCFEHeader(&Hdr);
    UT_Report(Hdr.ContentType == 0x44332211 && Hdr.SubType == 0x55443322 &&
              Hdr.Length == 0x66554433 && Hdr.SpacecraftID == 0x77665544 &&
              Hdr.ProcessorID == 0x88776655 && Hdr.ApplicationID == 0x99887766 &&
              Hdr.TimeSeconds == 0xaa998877 && Hdr.TimeSubSeconds == 0xbbaa9988,
              "CFE_FS_ByteSwapUint32",
              "Byte swap header - successful",
              "04.001");
}

/*
** Test FS API byte swap uint32 function
*/
void Test_CFE_FS_ByteSwapUint32(void)
{
    uint32 test = 0x11223344;
    uint32 *testptr = &test;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Byte Swap uint32\n");
#endif

    /* Test byte-swapping a uint32 value */
    UT_InitData();
    CFE_FS_ByteSwapUint32(testptr);
    UT_Report(test == 0x44332211, "CFE_FS_ByteSwapUint32",
              "Byte swap - successful",
              "05.001");
}

/*
** Test FS API is .gz file function
*/
void Test_CFE_FS_IsGzFile(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Is .gz File\n");
#endif

    /* Test if file name ends in .gz with the file name too short */
    UT_InitData();
    UT_Report(CFE_FS_IsGzFile("a") == FALSE,
              "CFE_FS_IsGzFile",
              "File name too short",
              "06.001");

    /* TTest if file name ends in .gz with a missing extension */
    UT_InitData();
    UT_Report(CFE_FS_IsGzFile("Normal") == FALSE,
              "CFE_FS_IsGzFile",
              "File name missing .gz extension",
              "06.002");

    /* Test if file name ends in .gz with no file name */
    UT_InitData();
    UT_Report(CFE_FS_IsGzFile(NULL) == FALSE,
              "CFE_FS_IsGzFile",
              "Null file name",
              "06.003");

    /* Test a valid file name ending in .gz */
    UT_InitData();
    UT_Report(CFE_FS_IsGzFile("tar.gz") == TRUE,
              "CFE_FS_IsGzFile",
              ".gz file name check - successful",
              "06.004");
}

/*
** Test FS API write extract file name from path function
*/
void Test_CFE_FS_ExtractFileNameFromPath(void)
{
    char Original[OS_MAX_PATH_LEN];
    char FileName[OS_MAX_PATH_LEN];
    
#ifdef UT_VERBOSE
    UT_Text("Begin Test Extract File Name from Path\n");
#endif

    /* Test extracting the file name from a path/file name that's
     * missing the path
     */
    UT_InitData();
    strncpy(Original, "/cf/appslibrary.gz", OS_MAX_PATH_LEN);
    Original[OS_MAX_PATH_LEN - 1] = '\0';
    UT_Report(CFE_FS_ExtractFilenameFromPath("name", FileName) ==
                  CFE_FS_INVALID_PATH,
              "CFE_FS_ExtractFilenameFromPath",
              "Missing file path",
              "07.001");
    
    /* Test extracting the file name from a path/file name that's null */
    UT_InitData();
    UT_Report(CFE_FS_ExtractFilenameFromPath(NULL, FileName) ==
                  CFE_FS_BAD_ARGUMENT,
              "CFE_FS_ExtractFilenameFromPath",
              "Null path/file name",
              "07.002");
    
    /* Test extracting the file name from a path/file name that's too long */
    UT_InitData();
    UT_Report(CFE_FS_ExtractFilenameFromPath("/cf/1234567890123456789012345678901234567890123456789012345678901234.toolong", FileName) == CFE_FS_FNAME_TOO_LONG,
              "CFE_FS_ExtractFilenameFromPath",
              "Path/file name too long",
              "07.003");
    
    /* Test successfully extracting the file name from a path/file name */
    UT_InitData();
    UT_Report(CFE_FS_ExtractFilenameFromPath(Original, FileName) == CFE_SUCCESS,
              "CFE_FS_ExtractFilenameFromPath",
              "Extract path name - successful",
              "07.004");
}

/*
** Tests for FS private functions (cfe_fs_priv.c)
*/
void Test_CFE_FS_Private(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test Private\n");
#endif

    /* Test successful FS initialization */
    UT_InitData();
    UT_Report(CFE_FS_EarlyInit() == CFE_SUCCESS,
              "CFE_FS_EarlyInit",
              "FS initialization - successful",
              "08.001");

    /* Test FS initialization with a mutex creation failure */
    UT_InitData();
    UT_SetOSFail(OS_MUTCREATE_FAIL);
    UT_Report(CFE_FS_EarlyInit() == -1,
              "CFE_FS_EarlyInit",
              "Mutex creation failure",
              "08.002");

    /* Test successful locking of shared data */
    UT_InitData();
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(WriteSysLogRtn.value == -1 && WriteSysLogRtn.count == 0,
              "CFE_FS_LockSharedData",
              "Lock shared data - successful",
              "08.003");

    /* Test locking of shared data with a mutex take error */
    UT_InitData();
    UT_SetRtnCode(&MutSemTakeRtn, -1, 1);
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(WriteSysLogRtn.value == FS_SYSLOG_OFFSET + 1 &&
              WriteSysLogRtn.count == 1,
              "CFE_FS_LockSharedData",
              "Shared data mutex take error",
              "08.004");
    
    /* Test successful unlocking of shared data */
    UT_InitData();
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(WriteSysLogRtn.value == -1 && WriteSysLogRtn.count == 0,
              "CFE_FS_UnlockSharedData",
              "Unlock shared data - successful",
              "08.005");

    /* Test unlocking of shared data with a mutex give error */
    UT_InitData();
    UT_SetRtnCode(&MutSemGiveRtn, -1, 1);
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(WriteSysLogRtn.value == FS_SYSLOG_OFFSET + 2 &&
              WriteSysLogRtn.count == 1,
              "CFE_FS_UnlockSharedData",
              "SharedData mutex give error",
              "08.006");

#ifdef UT_VERBOSE
    UT_Text("End Test Private\n\n");
#endif
}

/*
** Tests for FS decompress functions (cfe_fs_decompress.c)
*/
void Test_CFE_FS_Decompress(void)
{
    int NumBytes = 35400;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Decompress\n");
#endif

    /* Test file decompression with a file open failure */
    UT_InitData();
    UT_SetOSFail(OS_OPEN_FAIL);
    UT_Report(CFE_FS_Decompress("Filename.gz", "Output") ==
                  CFE_FS_GZIP_OPEN_INPUT,
              "CFE_FS_Decompress",
              "Open failed",
              "09.001");

    /* Test file decompression with a file create failure */
    UT_InitData();
    UT_SetOSFail(OS_CREAT_FAIL);
    UT_Report(CFE_FS_Decompress("Filename.gz", "Output") ==
                  CFE_FS_GZIP_OPEN_OUTPUT,
              "CFE_FS_Decompress",
              "Create failed",
              "09.002");

    /* Test successful file decompression using local buffered data
     * instead of real file I/O
     */
    UT_InitData();
    UT_SetReadBuffer(fs_gz_test, NumBytes);
    UT_Report(CFE_FS_Decompress("fs_test.gz", "Output") == CFE_SUCCESS,
              "CFE_FS_Decompress",
              "Decompress file - successful",
              "09.003");

    /* Test file decompression with a non-existent file */
    UT_InitData();
    UT_SetReadBuffer(&fs_gz_test[1], NumBytes - 1);
    UT_Report(CFE_FS_Decompress("fake", "Output") ==
                  CFE_FS_GZIP_NON_ZIP_FILE,
              "CFE_FS_Decompress",
              "File does not exist",
              "09.004");

    /* Test successful decompression of an inflated type 1 (fixed Huffman
     * codes) block
     */
    UT_InitData();
    UT_Report(FS_gz_inflate_fixed() == CFE_SUCCESS,
              "FS_gz_inflate_fixed",
              "Inflated type 1 decompression - successful",
              "09.005");

    /* Test decompressing an inflated type 0 (stored) block */
    UT_InitData();
    UT_Report(FS_gz_inflate_stored() != CFE_SUCCESS,
              "FS_gz_inflate_stored",
              "Failed",
              "09.006");

    /* Test filling the input buffer with a FS error  */
    UT_InitData();
    UT_SetRtnCode(&OSReadRtn, 4, 1);
    UT_SetRtnCode(&OSReadRtn2, OS_FS_ERROR, 1);
    UT_Report(FS_gz_fill_inbuf() == EOF,
              "FS_gz_fill_inbuf",
              "gzip read error",
              "09.007");

    /* Test writing the output window and updating the CRC */
    UT_InitData();
    UT_SetRtnCode(&FileWriteRtn, -1, 1);
    gz_outcnt = 435;
    FS_gz_flush_window();
    UT_Report(gz_outcnt == 0 && gGuzError == CFE_FS_GZIP_WRITE_ERROR,
              "FS_gz_flush_window",
              "Write output - successful",
              "09.008");

#ifdef UT_VERBOSE
    UT_Text("End Test Decompress\n\n");
#endif
}

/* Unit test specific call to process SB messages */
void UT_ProcessSBMsg(CFE_SB_Msg_t *MsgPtr)
{
}
