/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Macros for scheduled backup configuration
*     
*
*/

#ifndef __MMCSCBKUPCONFIG_H__
#define __MMCSCBKUPCONFIG_H__

// System includes
#include <e32std.h>
#include <f32file.h>


/**
*
*
* @since 3.0
*/
const TDriveNumber KMMCScBkupTempFileDrive = EDriveC;


/**
*
* Folder for temporary files, which will be cleaned by Starter if temporary files not deleted.
* Currently in 3.0/3.1 folder from C and E drives is cleaned.
*
* @since 3.0
*/
_LIT(KMMCScBkupTempDir, ":\\System\\Temp\\"); 


/**
*
*
* @since 3.0
*/
_LIT(KMMCScBkupMediaGalleryThumbnailFolder, "*\\_PAlbTN\\*"); //Photo Album Thumbnail folder
_LIT(KMMCScBkupVideoCenterFolderPath, "C:\\Data\\videocenter\\"); //video center folder
_LIT(KMMCScBkupVideoCenterFolder, "*\\videocenter\\*"); //video center folder
/**
*
* Allows the data that is received from the SBE or sent to the SBE to be dumped to individual
* files for comparison and inspection purposes.
*
* @since 3.0
*/
//#define DEBUGGING_DATA_TRANSFER

/**
*
* Controls the various directories and files that are generated when debugging
* data transfer
*
* @since 3.0
*/
#ifdef DEBUGGING_DATA_TRANSFER
    _LIT(KMMCScBkupDataTransferDebuggingAllFiles, "*.*");
    _LIT(KMMCScBkupDataTransferDebuggingFileName, "MMCBackupDataTransfer_0x%08x_(%c).dat");
    //
    _LIT(KMMCScBkupDataTransferDebuggingPathRoot, "MMCDataDump\\");
    _LIT(KMMCScBkupDataTransferDebuggingPathDataJava, "JavData\\");
    _LIT(KMMCScBkupDataTransferDebuggingPathDataSystem, "SysData\\");
    _LIT(KMMCScBkupDataTransferDebuggingPathDataPassive, "PasData\\");
    _LIT(KMMCScBkupDataTransferDebuggingPathDataActive, "ActData\\");
    //
    _LIT(KMMCScBkupDataTransferDebuggingPathDataBackup, "Backup\\");
    _LIT(KMMCScBkupDataTransferDebuggingPathDataRestore, "Restore\\");
#endif

/**
* 
* Always makes the SBE logging directory - ensures logs are available
*
* @since 3.0
*/
//#define MAKE_SBE_LOGGING_DIRECTORY

/**
*
* Always makes the MMC Engine logging directory
*
* @since 3.0
*/
//#define MAKE_MMCSCBKUP_LOGGING_DIRECTORY

/**
*
* Controls whether logging is to file or to RDebug
*
* @since 3.0
*/
//#ifdef _DEBUG
//#   define __MMCSCBKUPLOGGING_ENABLED__
//#if defined(__MMCSCBKUPLOGGING_ENABLED__)
//#   define __MMCSCBKUP_DATA_LOGGING_ENABLED__
//#endif
#   define __MMCSCBKUPLOGGING_MODE_FILE__
//#   define __MMCSCBKUPLOGGING_MODE_RDEBUG__
//#endif


/**
* Use these to disable backup for the various data types.
*
* E.g:
*
*  __DEBUG_TESTING_SKIP_BACKUP_REG_DATA return
*
* ... will ensure that registration data is not backed up (for any DO).
*
* @since 3.0
*/
#define __DEBUG_TESTING_SKIP_BACKUP_REG_DATA
#define __DEBUG_TESTING_SKIP_BACKUP_ACT_DATA
#define __DEBUG_TESTING_SKIP_BACKUP_PAS_DATA
#define __DEBUG_TESTING_SKIP_BACKUP_SYS_DATA
#define __DEBUG_TESTING_SKIP_BACKUP_JAV_DATA
#define __DEBUG_TESTING_SKIP_BACKUP_PUB_DATA

/**
* Use these to disable restore for the various data types
*
* E.g:
*
*  __DEBUG_TESTING_SKIP_RESTORE_REG_DATA return
*
* ... will ensure that registration data is not restored (for any DO).
*
* @since 3.0
*/
#define __DEBUG_TESTING_SKIP_RESTORE_REG_DATA
#define __DEBUG_TESTING_SKIP_RESTORE_ACT_DATA
#define __DEBUG_TESTING_SKIP_RESTORE_PAS_DATA
#define __DEBUG_TESTING_SKIP_RESTORE_SYS_DATA
#define __DEBUG_TESTING_SKIP_RESTORE_JAV_DATA
#define __DEBUG_TESTING_SKIP_RESTORE_PUB_DATA


#endif // __MMCSCBKUPCONFIG_H__

//End of File
