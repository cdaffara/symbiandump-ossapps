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
* Description: Music Player application monitor utility.
*
*/

#ifndef MPAPPLICATIONMONITOR_H
#define MPAPPLICATIONMONITOR_H

#include <QObject>
#include "mpcommondefs.h"

#ifdef BUILD_MPAPPLICATIONMONITOR
#define MPMPAPPLICATIONMONITOR_EXPORT Q_DECL_EXPORT
#else
#define MPMPAPPLICATIONMONITOR_EXPORT Q_DECL_IMPORT
#endif

class MpApplicationMonitorPrivate;

class MPMPAPPLICATIONMONITOR_EXPORT MpApplicationMonitor : public QObject
{
    Q_OBJECT

    friend class MpApplicationMonitorPrivate;

public:

    explicit MpApplicationMonitor( quint32 clientSecureId=MpCommon::KMusicPlayerUid, QObject *parent=0 );
    virtual ~MpApplicationMonitor();

    bool isApplicationRunning();

signals:

    void applicationStatusChanged( bool isRunning );

private:

    Q_DISABLE_COPY( MpApplicationMonitor )
    MpApplicationMonitorPrivate *d_ptr;

};

#endif  // MPAPPLICATIONMONITOR_H
