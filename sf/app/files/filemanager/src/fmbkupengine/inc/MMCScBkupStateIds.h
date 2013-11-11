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
* Description: Constants for scheduled backup state IDs
*     
*
*/

#ifndef __MMCSCBKUPSTATEIDS_H__
#define __MMCSCBKUPSTATEIDS_H__

// System includes
#include <e32std.h>

// Type definitions
typedef TUid TMMCScBkupStateId;

// Constants


/**
* Archive Header
* ===========================
*
* BACKUP:  Step 1 - first archive write operation: store archive header
* RESTORE: Step 1 - first archive read operation: restore archive header
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpArchiveHeader = 0x0BAC001;
const TMMCScBkupStateId KMMCScBkupStateArchiveOpArchiveHeader = { KMMCScBkupStateIdValueArchiveOpArchiveHeader };

/**
* Obtain List of Data Owners 
* ==========================
*
* BACKUP:  Step 2 - Get list of data owners from SBE
*/
const TInt32 KMMCScBkupStateIdValueGetDataOwners = 0x0BAC0002;
const TMMCScBkupStateId KMMCScBkupStateIdGetDataOwners = { KMMCScBkupStateIdValueGetDataOwners };

/**
* Set Smartphone into Backup Mode
* ===============================
*
* BACKUP:  Step 3
* RESTORE: 
*/
const TInt32 KMMCScBkupStateIdValueSetPhoneMode = 0x0BAC0003;
const TMMCScBkupStateId KMMCScBkupStateIdSetPhoneMode = { KMMCScBkupStateIdValueSetPhoneMode };

/**
* Supply Data Snapshots (Not Currently Supported)
* ===============================================
*
* BACKUP: Step 4
*/
const TInt32 KMMCScBkupStateIdValueSupplyDataSnapshots = 0x0BAC0004; // Not supported
const TMMCScBkupStateId KMMCScBkupStateIdSupplyDataSnapshots = { KMMCScBkupStateIdValueSupplyDataSnapshots };

/**
* Inform the Device that All Snapshots Have Been Supplied
* =======================================================
*
* BACKUP: Step 5
*/
const TInt32 KMMCScBkupStateIdValueNotifyAllSnapshotsSupplied = 0x0BAC0005;
const TMMCScBkupStateId KMMCScBkupStateIdNotifyAllSnapshotsSupplied = { KMMCScBkupStateIdValueNotifyAllSnapshotsSupplied };

/**
* Request Size of Backup Data
* ===========================
*
* BACKUP: Step 6
*/
const TInt32 KMMCScBkupStateIdValueRequestSizeOfBackupData = 0x0BAC0006;
const TMMCScBkupStateId KMMCScBkupStateIdRequestSizeOfBackupData = { KMMCScBkupStateIdValueRequestSizeOfBackupData };

/**
* Archive Operation: data owners
* ==============================
*
* BACKUP: Step 7 - second archive write operation: store data owners
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpDataOwners = 0x0BAC0007;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpDataOwners = { KMMCScBkupStateIdValueArchiveOpDataOwners };

/**
* Request List of Public Files
* ============================
*
* BACKUP: Step 8
*/
const TInt32 KMMCScBkupStateIdValueRequestListOfPublicFiles = 0x0BAC0008;
const TMMCScBkupStateId KMMCScBkupStateIdRequestListOfPublicFiles = { KMMCScBkupStateIdValueRequestListOfPublicFiles };

/**
* Calculate disk space required for backup or restore
* ============================
*
* BACKUP: Step 8
*/
const TInt32 KMMCScBkupStateIdValueValidateDiskSpace = 0x0BAC0009;
const TMMCScBkupStateId KMMCScBkupStateIdValidateDiskSpace = { KMMCScBkupStateIdValueValidateDiskSpace };


/**
* Archive Operation: public data files
* ====================================
*
* BACKUP: Step 10 - forth archive write operation: store public data files
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpPublicDataFiles = 0x0BAC000A;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpPublicDataFiles = { KMMCScBkupStateIdValueArchiveOpPublicDataFiles };

/**
* Request System Data
* ===================
*
* BACKUP: Step 11 - fifth archive write operation: store system data
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpSystemData = 0x0BAC000B;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpSystemData = { KMMCScBkupStateIdValueArchiveOpSystemData };

/**
* Request Java Data
* ===================
*
* BACKUP: Step 12 - sixth archive write operation: store system data
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpJavaData = 0x0BAC000C;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpJavaData = { KMMCScBkupStateIdValueArchiveOpJavaData };

/**
* Request Request State of Active Data Owners
* ===========================================
*
* BACKUP: Step 13
*/
const TInt32 KMMCScBkupStateIdValueGetDataOwnerStatuses = 0x0BAC000D;
const TMMCScBkupStateId KMMCScBkupStateIdGetDataOwnerStatuses = { KMMCScBkupStateIdValueGetDataOwnerStatuses };

/**
* Request Active Private Data
* ===========================
*
* BACKUP: Step 14 - seventh archive write operation: store active data
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpActiveData = 0x0BAC000E;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpActiveData = { KMMCScBkupStateIdValueArchiveOpActiveData };

/**
* Request Passive Private Data
* ===========================
*
* BACKUP: Step 15 - eighth archive write operation: store passive data
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpPassiveData = 0x0BAC000F;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpPassiveData = { KMMCScBkupStateIdValueArchiveOpPassiveData };

/**
* Archive Footer
* ===========================
*
* BACKUP: Step 16 - nineth archive write operation: store footer
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpArchiveFooter = 0x0BAC0010;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpArchiveFooter = { KMMCScBkupStateIdValueArchiveOpArchiveFooter };

/**
* Archive Header
* ===========================
*
* COMMON: Special step to indicate no more steps required
*/
const TInt32 KMMCScBkupStateIdValueOperationComplete = 0x0BAC0011;
const TMMCScBkupStateId KMMCScBkupStateIdOperationComplete = { KMMCScBkupStateIdValueOperationComplete };



/**
* Archive Operation: read data owners
* ===================================
*
* RESTORE: Step 2 - read from archive, inform to secure backup engine
*/
const TInt32 KMMCScBkupStateIdValueArchiveOpReadDataOwners = 0x0BAC0012;
const TMMCScBkupStateId KMMCScBkupStateIdArchiveOpReadDataOwners = { KMMCScBkupStateIdValueArchiveOpReadDataOwners };




/**
* BACKUP STATE FLOW
* =================
*
*  1) KMMCScBkupStateIdArchiveOpArchiveHeader
*  2) KMMCScBkupStateIdGetDataOwners
*  3) KMMCScBkupStateIdSetPhoneMode
*  4) KMMCScBkupStateIdNotifyAllSnapshotsSupplied
*  5) KMMCScBkupStateIdRequestSizeOfBackupData
*  6) KMMCScBkupStateIdRequestListOfPublicFiles
*  7) KMMCScBkupStateIdValidateDiskSpace
*  8) KMMCScBkupStateIdArchiveOpPublicDataFiles
*  9) KMMCScBkupStateIdArchiveOpSystemData
* 10) KMMCScBkupStateIdArchiveOpJavaData
* 11) KMMCScBkupStateIdGetDataOwnerStatuses
* 12) KMMCScBkupStateIdArchiveOpActiveData
* 13) KMMCScBkupStateIdArchiveOpPassiveData
* 14) KMMCScBkupStateIdArchiveOpDataOwners
* 15) KMMCScBkupStateIdArchiveOpArchiveFooter
* 16) KMMCScBkupStateIdOperationComplete
* 
*/



/**
* RESTORE STATE FLOW
* =================
*
*  1) KMMCScBkupStateIdArchiveOpArchiveHeader
*  2) KMMCScBkupStateIdArchiveOpArchiveFooter
*  3) KMMCScBkupStateIdArchiveOpDataOwners
*  4) KMMCScBkupStateIdValidateDiskSpace
*  5) KMMCScBkupStateIdSetPhoneMode
*  6) KMMCScBkupStateIdArchiveOpSystemData
*  7) KMMCScBkupStateIdArchiveOpJavaData
*  8) KMMCScBkupStateIdArchiveOpPassiveData
*  9) KMMCScBkupStateIdGetDataOwnerStatuses
* 10) KMMCScBkupStateIdArchiveOpActiveData
* 11) KMMCScBkupStateIdArchiveOpPublicDataFiles
* 12) KMMCScBkupStateIdOperationComplete
*
*/

#endif // __MMCSCBKUPSTATEIDS_H__

//End of File
