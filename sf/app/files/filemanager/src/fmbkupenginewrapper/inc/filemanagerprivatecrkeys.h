/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 * 
 * Contributors:
 *     
 * 
 * Description:
 *      The header file of the backup const configuration
 */


#ifndef FILEMANAGERPRIVATECRKEYS_H
#define FILEMANAGERPRIVATECRKEYS_H


/** File manager repository uid */
const TUid KCRUidFileManagerSettings = { 0x1020506B };

/** Defines the backup content */
const TUint32 KFileManagerBackupContent = 0x1;

/** Defines the backup scheduling mode */
const TUint32 KFileManagerBackupScheduling = 0x2;

/** Contains the possible values of KFileManagerBackupScheduling key. */
enum TFileManagerBackupSchedule
    {
    EFileManagerBackupScheduleNone = 0,
    EFileManagerBackupScheduleDaily,
    EFileManagerBackupScheduleWeekly
    };

/** Defines the scheduled backup weekday. See day definitions from TDay.
 * Negative value means the first day of the week.
 */
const TUint32 KFileManagerBackupDay = 0x3;

/** Defines the scheduled backup time in minutes starting from 00:00 midnight */
const TUint32 KFileManagerBackupTime = 0x4;

/** Defines the backup target path as string */
const TUint32 KFileManagerBackupTargetPath = 0x5;

/** Defines the status of the last scheduled backup */
const TUint32 KFileManagerLastSchBackupStatus = 0x6;

/** Contains the possible values of KFileManagerLastSchBackupStatus key. */
enum TFileManagerSchBackupStatus
    {
    EFileManagerSchBackupStatusOk = 0,
    EFileManagerSchBackupStatusFailed,
    EFileManagerSchBackupStatusStarting,
    EFileManagerSchBackupStatusInProgress,
    EFileManagerSchBackupStatusCanceled
    };

/** Defines file manager local variation configuration */
const TUint32 KFileManagerFeatures = 0x7;

/** Contains the possible bitmask values of KFileManagerFeatures key. */
enum TFileManagerFeatures
    {
    EFileManagerFeatureBackupAllowAllDrives = 0x1,
    EFileManagerFeatureNotUsedAnymore = 0x2,
    EFileManagerFeatureRightLeftNaviSupported = 0x4,
    EFileManagerFeatureScheduledBackupDisabled = 0x8
    };

/** Defines the file manager application to be launched for the AIW service.
The value of this key is an application UID or zero. */
const TUint32 KFileManagerAppUidForAiwService = 0x8;

/** Defines default volume name for non user nameable mass storage. 
The default name will be used after format only if the stored volume name is undefined 
for the drive. The name is visible only in Windows Explorer during USB mass storage mode and 
in 3rd party File Manager(s). */
const TUint32 KFileManagerDefaultInternalMassStorageVolumeName = 0x9;

/** Defines a store for keeping existing non user nameable mass storage volume name over 
format operation. The data is stored in binary as TFileManagerVolumeNameStore. */
const TUint32 KFileManagerStoredInternalMassStorageVolumeName = 0xA;

enum TFileManagerBkupStatusType
    {
    EFileManagerBkupStatusUnset   = 0x00000000,
    EFileManagerBkupStatusBackup  = 0x00000001,
    EFileManagerBkupStatusRestore = 0x00000002
    };

#endif // FILEMANAGERPRIVATECRKEYS_H

// End of File
