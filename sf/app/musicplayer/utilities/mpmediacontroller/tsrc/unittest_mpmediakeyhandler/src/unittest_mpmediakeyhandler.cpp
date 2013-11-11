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

#include <QMetaType>

#include "stub/inc/mpenginefactory.h"
#include "stub/inc/remconcoreapitarget.h"
#include "stub/inc/remconcoreapitargetobserver.h"
#include "stub/inc/remconinterfaceselector.h"
#include "unittest_mpmediakeyhandler.h"
#include "mpcommondefs.h"


// Do this so we can access all member variables.
#define private public
#include "mpmediakeyhandler.h"
#include "mpmediakeyhandler_p.h"
#undef private

//This so stub classes can be loaded
#include "mpmediakeyhandler_p.cpp"
#include "mpmediakeyremconresponse.cpp"


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TestMpMediaKeyHandler tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmediakeyhandler.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMediaKeyHandler::TestMpMediaKeyHandler()
    : mTest(0)
{
}

TestMpMediaKeyHandler::~TestMpMediaKeyHandler()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaKeyHandler::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaKeyHandler::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaKeyHandler::init()
{
    mTest = new MpMediaKeyHandler();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpMediaKeyHandler::cleanup()
{
    if (mTest)
    {
        delete mTest;
        mTest = 0;
        mTestPrivate = 0;
        
        MpEngineFactory::close();
    }
}

void TestMpMediaKeyHandler::testConstructor()
{
    QVERIFY( mTest );
    QVERIFY( mTestPrivate );
    QVERIFY( mTestPrivate->iInterfaceSelector );
    QVERIFY( mTestPrivate->iResponseHandler );
    QVERIFY( mTestPrivate->iMpEngine );
}

void TestMpMediaKeyHandler::testDestructor()
{
    // it is been observed that CRemConCoreApiTarget is not being deleted by anyone
    // the creator claim it does not own it, while others never claim the ownership neither
    cleanup();
    QVERIFY( !mTest );
    //QCOMPARE(CRemConCoreApiTarget::getCount(), 0);
    QCOMPARE(CRemConCoreApiTarget::getCount(), 2);
    QCOMPARE(CRemConInterfaceSelector::getCount(), 0);
}

void TestMpMediaKeyHandler::testCommandPausePlayKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPausePlayFunction, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdPlayPause);

}

void TestMpMediaKeyHandler::testCommandPlayKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPlay, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdPlay);
}

void TestMpMediaKeyHandler::testCommandStopKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiStop, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdStop);
}

void TestMpMediaKeyHandler::testCommandPauseKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiPause, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdPause);
}

void TestMpMediaKeyHandler::testCommandRewindKeyPress()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiRewind, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdStartSeekBackward);
}

void TestMpMediaKeyHandler::testCommandRewindKeyRelease()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiRewind, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdStopSeeking);
}

void TestMpMediaKeyHandler::testCommandFastForwardKeyPress()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiFastForward, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdStartSeekForward);
}

void TestMpMediaKeyHandler::testCommandFastForwardKeyRelease()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiFastForward, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdStopSeeking);
}

void TestMpMediaKeyHandler::testCommandBackwardKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiBackward, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdSkipBackward);
}

void TestMpMediaKeyHandler::testCommandForwardKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoCommand(ERemConCoreApiForward, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE(mTestPrivate->iMpEngine->iMediaCommandReceived, CmdSkipForward);
}

void TestMpMediaKeyHandler::testCommandVolumeUpKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();

    // Simulate hardware side key clicked.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeUp, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdIncreaseVolume );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Simulate holding the hardware side key.
    mTestPrivate->iMpEngine->iMediaCommandReceived = CmdNone;
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeUp, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+2);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Release hardware side key.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeUp, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+3);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
    QVERIFY( !mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Simulate headset volume key pressed.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeUp, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+4);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdIncreaseVolume );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
}

void TestMpMediaKeyHandler::testCommandVolumeDownKey()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();

    // Simulate hardware side key clicked.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeDown, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdDecreaseVolume );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Simulate holding the hardware side key.
    mTestPrivate->iMpEngine->iMediaCommandReceived = CmdNone;
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeDown, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+2);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Release hardware side key.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeDown, ERemConCoreApiButtonRelease);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+3);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
    QVERIFY( !mTestPrivate->iSideKeyAlreadyClicked );
    QTest::qWait(100);

    // Simulate headset volume key pressed.
    mTestPrivate->MrccatoCommand(ERemConCoreApiVolumeDown, ERemConCoreApiButtonPress);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+4);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdDecreaseVolume );
    QVERIFY( mTestPrivate->iSideKeyAlreadyClicked );
}

void TestMpMediaKeyHandler::testPlay()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoPlay(ERemConCoreApiPlaybackSpeedX1, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdPlay );
}

void TestMpMediaKeyHandler::testTuneFunction()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoTuneFunction(ETrue, 0, 1, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
}

void TestMpMediaKeyHandler::testSelectDiskFunction()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectDiskFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
}

void TestMpMediaKeyHandler::testAudioInputFunction()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectAudioInputFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
}

void TestMpMediaKeyHandler::testSelectAvInputFunction()
{
    int oldCount = CRemConCoreApiTarget::getSendResponseCount();
    mTestPrivate->MrccatoSelectAvInputFunction(0, ERemConCoreApiButtonClick);
    QCOMPARE(CRemConCoreApiTarget::getSendResponseCount(), oldCount+1);
    QCOMPARE( mTestPrivate->iMpEngine->iMediaCommandReceived, CmdNone );
}

// End of file
