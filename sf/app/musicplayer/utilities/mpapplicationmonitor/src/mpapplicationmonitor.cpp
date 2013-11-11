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
* Description: Music Player now playing widget - private implementation.
*
*/

#include "mpapplicationmonitor.h"
#include "mpapplicationmonitor_p.h"
#include "mptrace.h"

/*!
    \class MpApplicationMonitor
    \brief Application monitor.

    This utility monitors an application status, i.e. whether the application
    is running or not.
*/

/*!
    \fn applicationStatusChanged( bool isRunning )

    This signal is emitted when the application status changes. Application
    status is indicated with \a isRunning.
*/

/*!
 Constructs the utility. Default monitored application is Music Player.
 */
MpApplicationMonitor::MpApplicationMonitor( quint32 clientSecureId, QObject *parent )
    : QObject(parent)
{
    d_ptr = new MpApplicationMonitorPrivate(this);
    d_ptr->init( clientSecureId );
}

/*!
 Destructs the utility.
 */
MpApplicationMonitor::~MpApplicationMonitor()
{
    delete d_ptr;
}

/*!
 Returns whether the monitoring application is running.
 */
bool MpApplicationMonitor::isApplicationRunning()
{
    return d_ptr->isApplicationRunning();
}

