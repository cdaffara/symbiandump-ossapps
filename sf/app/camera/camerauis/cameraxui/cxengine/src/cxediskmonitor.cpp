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

#include "cxutils.h"
#include "cxediskmonitor.h"
#include "cxediskmonitorprivate.h"

/*!
* Constructor.
*/
CxeDiskMonitor::CxeDiskMonitor(CxeSettings &settings)
{
    CX_DEBUG_ENTER_FUNCTION();
    p = new CxeDiskMonitorPrivate(settings);
    CX_ASSERT_ALWAYS(p);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Destructor.
*/
CxeDiskMonitor::~CxeDiskMonitor()
{
    delete p;
}

/*!
* Set the warning level of free disk space.
* When free disk space falls below this limit, diskSpaceLow() signal is emitted.
* @param bytes Limit in bytes.
*/
void CxeDiskMonitor::setLowWarningLevel(qint64 bytes)
{
    if (p) {
        p->setLowWarningLevel(bytes);
    }
}

/*!
* Slot to start monitoring disk space.
*/
void CxeDiskMonitor::start()
{
    if (p) {
        connect(p, SIGNAL(diskSpaceLow()), this, SIGNAL(diskSpaceLow()), Qt::UniqueConnection);
        connect(p, SIGNAL(diskSpaceChanged()), this, SIGNAL(diskSpaceChanged()), Qt::UniqueConnection);
        p->start();
    }
}

/*!
* Slot to stop monitoring disk space.
*/
void CxeDiskMonitor::stop()
{
    if (p) {
        p->stop();
        disconnect(p, SIGNAL(diskSpaceChanged()), this, SIGNAL(diskSpaceChanged()));
        disconnect(p, SIGNAL(diskSpaceLow()), this, SIGNAL(diskSpaceLow()));
    }
}

/*!
* Is monitoring ongoing at the moment.
*/
bool CxeDiskMonitor::isMonitoring() const
{
    return p ? p->isMonitoring() : false;
}

/*!
* Get free disk space for Camera use in bytes.
*/
qint64 CxeDiskMonitor::free(bool cached) const
{
    return p ? p->free(cached) : 0;
}

// end of file
