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
* Description: Unit test for mpengine
*
*/

#ifndef TESTMPENGINE_H
#define TESTMPENGINE_H

#include <QtTest/QtTest>

class MpEngine;

class TestMpEngine : public QObject
{
    Q_OBJECT

public:

    TestMpEngine();
    ~TestMpEngine();

public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
private slots:
    void testInitialize();
    void testVerifyUsbBlocking();
    void testSongScanner(); 
    void testRefreshLibrary();
    void testHandleScanStarted();
    void testHandleScanEnded();
    void testHandleDiskEvent(); 
    void testHandleUsbEvent();
    void testHandleUsbMassStorageStartEvent();
    void testHandleUsbMassStorageEndEvent();
    void testHandleUsbMtpStartEvent(); 
    void testHandleUsbMtpEndEvent(); 
    void testHandleUsbMtpNotActive(); 
    void testChangeUsbBlockingState();
    void testHandleDeleteStarted(); 
    void testHandleDeleteEnded(); 
    void testPlaybackData(); 
    void testSongData(); 
    void testBalance();
    void testLoudness();
    void testSetBalance();
    void testSetLoudness();
    void testApplyPreset();
    void testDisableEqualizer();
    void testActivePreset();
    void testPresetNames();
    void testHandleEqualizerReady(); 

private:
    MpEngine *mTest;
};


// to test private functions
#include <QObject>
#include "mpcommondefs.h"
// Do this so we can access all member variables.
#define private public
#include "../../inc/mpengine.h"
#undef private

class MpEngineFactory : QObject
{
    Q_OBJECT
private:
    explicit MpEngineFactory(){    
    }

public:
    virtual ~MpEngineFactory(){
    
    }
    static MpEngineFactory * instance() {
        static MpEngineFactory instance;
        return &instance;
    }
    static MpEngine *createIsolatedEngine( MpEngine::EngineMode mode ) {
        instance()->mEngines.append( new MpEngine() );
        instance()->mEngines.last()->initialize( (MpCommon::KMusicPlayerUid + instance()->mEngines.count()), mode );
        return instance()->mEngines.last();
    }    
    static void close() {
        MpEngine *ptr;
        foreach ( ptr, instance()->mEngines ) {
            delete ptr;
            ptr = 0;
        }
    }
    
private:    
    QList<MpEngine *>    mEngines;
};


#endif  // TESTMPENGINE_H




