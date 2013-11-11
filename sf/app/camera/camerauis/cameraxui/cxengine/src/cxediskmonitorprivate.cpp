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
* Description:
*
*/

#include <QObject> // Compiler flags, keep first
#ifdef Q_OS_SYMBIAN
#include <coemain.h>
#endif // Q_OS_SYMBIAN
#include "cxutils.h"
#include "cxesysutil.h"
#include "cxesettings.h"
#include "cxediskmonitor.h"
#include "cxediskmonitorprivate.h"


namespace
{
    const int MONITORING_INTERVAL = 2*1000; // in milliseconds
    const int UNKNOWN = -1;
}


/*!
* Constructor.
*/
CxeDiskMonitorPrivate::CxeDiskMonitorPrivate(CxeSettings &settings)
    : mSettings(settings),
      mTimer(),
#ifdef Q_OS_SYMBIAN
      mFs(CCoeEnv::Static()->FsSession()),
      mCameraDrive(CxeSysUtil::getCameraDrive(mFs)),
#endif // Q_OS_SYMBIAN
      mTriggerLevelBytes(UNKNOWN),
      mLatestFreeBytes(UNKNOWN)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor.
*/
CxeDiskMonitorPrivate::~CxeDiskMonitorPrivate()
{
    CX_DEBUG_ENTER_FUNCTION();
    stop();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for checking if the available disk space is below warning trigger level.
*/
void CxeDiskMonitorPrivate::checkSpace()
{
    CX_DEBUG_ENTER_FUNCTION();

    qint64 previousFree(mLatestFreeBytes);

    // Get the current free space.
    mLatestFreeBytes = free(false);

    // If space has changed during monitoring, signal it now.
    if (previousFree != mLatestFreeBytes && isMonitoring()) {
        CX_DEBUG(("CxeDiskMonitorPrivate - Disk space changed %d -> %d bytes", previousFree, mLatestFreeBytes));
        emit diskSpaceChanged();
    }

    // Only emit warning if trigger level is set to some positive value.
    // Otherwise updating cached free space value is enough.
    if (mTriggerLevelBytes != UNKNOWN && mLatestFreeBytes < mTriggerLevelBytes) {
        CX_DEBUG(("[WARNING] CxeDiskMonitorPrivate - Disk space low!"));
        emit diskSpaceLow();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Set the warning level of free disk space.
* When free disk space falls below this limit, diskSpaceLow() signal is emitted.
* @param bytes Limit in bytes.
*/
void CxeDiskMonitorPrivate::setLowWarningLevel(qint64 bytes)
{
    mTriggerLevelBytes = bytes;
}

/*!
* Start monitoring disk space.
*/
void CxeDiskMonitorPrivate::start()
{
    if (!mTimer.isActive()) {
        connect(&mTimer, SIGNAL(timeout()), this, SLOT(checkSpace()), Qt::UniqueConnection);
        mTimer.start(MONITORING_INTERVAL);
    }
}

/*!
* Stop monitoring disk space.
*/
void CxeDiskMonitorPrivate::stop()
{
    mTimer.stop();
    mLatestFreeBytes = UNKNOWN;
}

/*!
* Is monitoring ongoing at the moment.
*/
bool CxeDiskMonitorPrivate::isMonitoring() const
{
    return mTimer.isActive();
}

/*!
* Get free disk space for Camera use in bytes.
* @param cached Can cached value be used.
* If true (default value), value of last constant-interval-check is used (if it exists).
* If false (or there's no cached value), forced reading of free space is done now.
* This may cause significant delay compared to returning cached value.
*/
qint64 CxeDiskMonitorPrivate::free(bool cached) const
{
    qint64 value(0);
    if (cached && mLatestFreeBytes != UNKNOWN) {
        value = mLatestFreeBytes;
    } else {
        value = CxeSysUtil::spaceAvailable(mFs, mCameraDrive, mSettings);
    }
    return value;
}

// end of file
