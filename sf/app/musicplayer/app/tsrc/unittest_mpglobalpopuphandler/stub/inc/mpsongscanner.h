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
* Description: stub for song scanner.
*
*/


#ifndef MPSONGSCANNER_H
#define MPSONGSCANNER_H

class MpMpxHarvesterFrameworkWrapper;

#include <QObject>

class MpSongScanner : public QObject
{
    Q_OBJECT

public:
    enum ScanError{
        ScanErrorNone,
        ScanGeneralError,
        ScanErrorDiskFull,
        ScanInterrupted
    };
    
    MpSongScanner();
    virtual ~MpSongScanner();

    void scan( bool automaticScan );
    bool isAutomaticScan();
    
signals:
    void scanStarted();
    void scanCountChanged( int count );
    void scanFinished( int error, int itemsAdded );    

public:
    bool                                 mAutomaticScan;
};

#endif // MPSONGSCANNER_H

