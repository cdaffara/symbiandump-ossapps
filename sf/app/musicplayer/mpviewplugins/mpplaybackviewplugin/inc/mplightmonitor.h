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

#ifndef MPLIGHTMONITOR_H
#define MPLIGHTMONITOR_H

#include <QObject>

class MpLightMonitorPrivate;

class MpLightMonitor : public QObject
{
    Q_OBJECT

    friend class MpLightMonitorPrivate;

public:

    explicit MpLightMonitor( QObject *parent=0 );
    virtual ~MpLightMonitor();

signals:

    void lcdLightStatus( bool lightOn );

private:

    Q_DISABLE_COPY( MpLightMonitor )
    MpLightMonitorPrivate        *d_ptr;

};

#endif  // MPLIGHTMONITOR_H
