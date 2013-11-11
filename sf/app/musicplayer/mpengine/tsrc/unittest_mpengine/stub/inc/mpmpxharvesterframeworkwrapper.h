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
* Description: Stub class for mpsongscanner unittest.
*
*/

#ifndef MPMPXHARVESTERFRAMEWORKWRAPPER_H
#define MPMPXHARVESTERFRAMEWORKWRAPPER_H

#include <QObject>

#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"

class MpMpxHarvesterFrameworkWrapper : public QObject
{
    Q_OBJECT

public:
    explicit MpMpxHarvesterFrameworkWrapper( quint32 clientSecureId=MpCommon::KMusicPlayerUid, QObject *parent=0 );
    virtual ~MpMpxHarvesterFrameworkWrapper();

    // mock functions
    void scan();
    void cancelScan();
    void checkForSystemEvents();

    // unittest specific functions
    void emitScanStarted();
    void emitScanEnded( int count, int error );
    void emitScanCountChanged( int count );
    void emitDiskEvent();

signals:
    void scanStarted();
    void scanEnded( int count, int error );
    void scanCountChanged( int count );

    void diskEvent( MpxDiskEvents event );
    void usbEvent( MpxUsbEvents event );

public:
    bool     mScanRequested;
    bool     mScanning;
    int      mScanCount;
    bool     mCheckSystemEventsRequested;

};

#endif // MPMPXHARVESTERFRAMEWORKWRAPPER_H
