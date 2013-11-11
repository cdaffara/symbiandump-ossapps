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
* Description: Music Player song scanner.
*
*/


#ifndef MPSONGSCANNER_H
#define MPSONGSCANNER_H

class MpMpxHarvesterFrameworkWrapper;

#include <QObject>
#include "mpmpxcommondefs.h"

#if defined(BUILD_MPENGINE_LIB)
#define MPENGINE_EXPORT Q_DECL_EXPORT
#else
#define MPENGINE_EXPORT Q_DECL_IMPORT
#endif

class MPENGINE_EXPORT MpSongScanner : public QObject
{
    Q_OBJECT

public:

    enum ScanError{
        ScanErrorNone,
        ScanGeneralError,
        ScanErrorDiskFull,
        ScanInterrupted
    };

    explicit MpSongScanner( MpMpxHarvesterFrameworkWrapper *wrapper, QObject *parent = 0 );
    virtual ~MpSongScanner();

    void scan( bool automaticScan );
    bool isAutomaticScan();

signals:

    void scanStarted();
    void scanCountChanged( int count );
    void scanFinished( int error, int itemsAdded );

public slots:

    void cancelScan();
    void handleScanStarted();
    void handleScanEnded( int numItemsAdded, int error );
    void handleDiskEvent( MpxDiskEvents event );

private:

    MpMpxHarvesterFrameworkWrapper       *mMpxHarvesterWrapper;       // Not own
    bool                                 mScanning;
    bool                                 mAutomaticScan;

};

#endif // MPSONGSCANNER_H

