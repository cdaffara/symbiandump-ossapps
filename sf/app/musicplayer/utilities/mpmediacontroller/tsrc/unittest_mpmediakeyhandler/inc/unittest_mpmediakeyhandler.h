/**
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
* Description: Unit test for mpmediakeyhandler
*
*/

#ifndef TESTMPMEDIAKEYHANDLER_H
#define TESTMPMEDIAKEYHANDLER_H

#include <QtTest/QtTest>

class MpMediaKeyHandler;
class MpMediaKeyHandlerPrivate;

class TestMpMediaKeyHandler : public QObject
{
    Q_OBJECT

public:
    
    TestMpMediaKeyHandler();
    virtual ~TestMpMediaKeyHandler();

// from QtTest
public slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

// internal
private slots:

    void testConstructor();
    void testDestructor();
    void testCommandPausePlayKey();
    void testCommandPlayKey();
    void testCommandStopKey();
    void testCommandPauseKey();
    void testCommandRewindKeyPress();
    void testCommandRewindKeyRelease();
    void testCommandFastForwardKeyPress();
    void testCommandFastForwardKeyRelease();
    void testCommandBackwardKey();
    void testCommandForwardKey();
    void testCommandVolumeUpKey();
    void testCommandVolumeDownKey();

    void testPlay();
    void testTuneFunction();
    void testSelectDiskFunction();
    void testAudioInputFunction();
    void testSelectAvInputFunction();

private:

    MpMediaKeyHandler           *mTest;
    MpMediaKeyHandlerPrivate    *mTestPrivate;

};

#endif  // TESTMPMEDIAKEYHANDLER_H

