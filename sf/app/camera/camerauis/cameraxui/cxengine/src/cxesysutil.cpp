/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <algorithm>
#include <exception>
#include <e32const.h>
#include <f32file.h>
#include <driveinfo.h>
#include <sysutil.h>
#include <sysutildomaincrkeys.h>

#include "cxesettings.h"
#include "cxutils.h"
#include "cxesysutil.h"


namespace
{
    const uint KDiskSafetyLimit = 400000; // Amount of free disk space to leave unused
}

/*!
* Checks if free disk drive storage space is or will fall below critical
* level. Static configuration values stored in Central Repository are
* used to determine a critical level for each drive.
*
* Usage example:
* @code
* TInt dataSize = 500;
* if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFsSession, dataSize, EDriveC ) )
*     {
*     // Can not write the data, there's not enough free space on disk.
*     ...
*     }
* else
*     {
*     // It's ok to actually write the data.
*     ...
*     }
* @endcode
*
* @since S60 v2.0
*
* @param aFs File server session. Must be given if available, e.g. from
*            EIKON environment. If NULL, this method will create a
*            temporary session, which causes the method to consume more
*            time and system resources.
* @param aBytesToWrite Number of bytes the caller is about to write to
*                      disk. If value 0 is given, this method checks
*                      if the current disk space is already below critical
*                      level.
* @param aDrive Identifies the disk drive to be checked. Numeric values
*               for identifying disk drives are defined in TDriveNumber
*               enumeration.
*
* @see TDriveNumber in f32file.h.
*
* @return ETrue if disk space would go below critical level after writing
*         aBytesToWrite more data, EFalse otherwise.
*
* @leave Leaves with one of the Symbian error codes if checking the disk
*        space fails, for instance if the drive contains no media or there
*        is not enough free memory to create a temporary connection to
*        file server.
*/
bool CxeSysUtil::DiskSpaceBelowCriticalLevel(
        RFs* aFs,
        TInt aBytesToWrite,
        TInt aDrive )
{
    bool fullDisk = false;
    TRAPD( utilErr,
            fullDisk = SysUtil::DiskSpaceBelowCriticalLevelL(
                    aFs, aBytesToWrite, aDrive ) );
    if(utilErr) {
        fullDisk = false;
    }
    return fullDisk;
}

/*!
* Get the drive index to use for camera.
* Normally it is mass memory, but if that is not available in the device at all,
* using phone memory. It should be noted, that if mass memory is temporarily
* unavailable, storage will not be changed to phone memory, but space available
* will report zero.
*/
int CxeSysUtil::getCameraDrive(RFs &fs)
{
    CX_DEBUG_ENTER_FUNCTION();
    int index(-1);
    try {
        // Get drive index for mass storage
        int status(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, index));
        CX_DEBUG(("status getting mass memory drive index: %d", status));
        qt_symbian_throwIfError(status);

        // Get the drive info
        TDriveInfo driveInfo;
        status = fs.Drive(driveInfo, index);
        CX_DEBUG(("status getting mass memory drive info: %d", status));
        qt_symbian_throwIfError(status);

        // If and only if mass memory drive does not exist at all,
        // we resort to phone internal memory (c-drive).
        // E.g. dismounted mass memory because of USB connection is intentionally
        // *not* causing us to switch to phone memory.
        if (driveInfo.iDriveAtt == KDriveAbsent) {
            CX_DEBUG(("[WARNING]: Mass memory not found at all, resorting to phone memory!"));
            status = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, index);
            CX_DEBUG(("status getting phone memory drive index: %d", status));
        }
        qt_symbian_throwIfError(status);
    } catch(const std::exception& e) {
        Q_UNUSED(e); // avoid warning if trace is not used.
        CX_DEBUG(("[WARNING]: exception [%s]", e.what()));
    }

    CX_DEBUG_EXIT_FUNCTION();
    return index;
}

/*!
* Return space available for Camera to use.
* @param fs File Server session
* @param index Index for the drive to be checked.
* @param settings Camera settings instance
* @return Amount of bytes free for Camera usage.
*/
qint64 CxeSysUtil::spaceAvailable(RFs &fs, int index, CxeSettings &settings)
{
    CX_DEBUG_ENTER_FUNCTION();

    qint64 freeSpace(0);

    try {
        // Get volume info
        TVolumeInfo volumeInfo;
        int status(fs.Volume(volumeInfo, index));
        CX_DEBUG(("status getting volume info: %d", status));
        qt_symbian_throwIfError(status);

        // Get critical treshold for free space on the drive.
        // We must leave the free space above this.
        QVariant criticalThreshold;
        settings.get(KCRUidDiskLevel.iUid, KDiskCriticalThreshold, Cxe::Repository, criticalThreshold);

        // Calculate space that we can still use. A small safety buffer is used above critical value.
        freeSpace = std::max(qint64(0), volumeInfo.iFree - criticalThreshold.toInt() - KDiskSafetyLimit);

    } catch(const std::exception& e) {
        Q_UNUSED(e);
        CX_DEBUG(("[WARNING]: exception [%s]", e.what()));
        freeSpace = 0;
    }

    CX_DEBUG(("CxeSysUtil - free space: %d", freeSpace));
    CX_DEBUG_EXIT_FUNCTION();
    return freeSpace;
}

// end of file
