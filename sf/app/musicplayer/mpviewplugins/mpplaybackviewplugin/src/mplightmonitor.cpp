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
* Description: Light monitor for Playback.
*
*/

#include "mplightmonitor.h"
#include "mplightmonitor_p.h"


/*!
    \class MpLightMonitor
    \brief Wrapper for light monitor.

    MpLightMonitor provides Qt style interface to the Light Observer.
    It's implementation is hidden using private class data pattern.
*/

/*!
    \fn void lcdLightStatus( bool lightOn );

    This signal is emitted when the device light status changes.
 */

/*!
 \internal
 */
MpLightMonitor::MpLightMonitor( QObject *parent )
    : QObject( parent )
{
    d_ptr = new MpLightMonitorPrivate ( this );
    d_ptr->init();
}

/*!
 \internal
 */
MpLightMonitor::~MpLightMonitor()
{
    delete d_ptr;
}

