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
* Description: mpengine stub
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>

class MpSongScanner;

class MpEngine : public QObject
{
    Q_OBJECT

public:
    enum EngineMode{
        StandAlone,
        Fetch,
        Embedded,
        MediaBrowsing
    };
    friend class MpEngineFactory;
    
public:     
    MpSongScanner *songScanner();    

private:
    explicit MpEngine();
    void initialize( quint32 clientSecureId, EngineMode mode);
    
signals:
    void libraryAboutToUpdate();    
    void unableToCotinueDueUSB();
    void usbSynchronizationStarted();
    void usbSynchronizationFinished();

public:
    virtual ~MpEngine();
    
public:
    MpSongScanner                        *mSongScanner;          
};

#endif // MPENGINE_H
