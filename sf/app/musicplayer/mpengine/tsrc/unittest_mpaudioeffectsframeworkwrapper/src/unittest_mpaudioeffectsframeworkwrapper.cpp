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
* Description: Unit test for MpMpxAsFrameworkWrapper
*
*/

#include <QSignalSpy>
#include <qnamespace.h>

#include "unittest_mpaudioeffectsframeworkwrapper.h"
#include "stub/inc/mpxaudioeffectproperties.h"

// Do this so we can access all member variables.
#define private public
#include "mpaudioeffectsframeworkwrapper.h"
#include "mpaudioeffectsframeworkwrapper_p.h"
#undef private

//This so we can test private functions
#include "mpaudioeffectsframeworkwrapper_p.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpAudioEffectsFrameworkWrapper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpaudioeffectsframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpAudioEffectsFrameworkWrapper::TestMpAudioEffectsFrameworkWrapper()
    : mTest(0)
{
}

TestMpAudioEffectsFrameworkWrapper::~TestMpAudioEffectsFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpAudioEffectsFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpAudioEffectsFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpAudioEffectsFrameworkWrapper::init()
{
    mTest = new MpAudioEffectsFrameworkWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpAudioEffectsFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test Constructor
 */
void TestMpAudioEffectsFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->mAudioEffectProperties != 0);
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iLoadFromFile, TBool(ETrue));
}

/*!
 test init
 */
void TestMpAudioEffectsFrameworkWrapper::testInit()
{
    mTestPrivate->mAudioEffectProperties->iLeave = ETrue;
    mTestPrivate->init();
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iLoadFromFile, TBool(EFalse));
    mTestPrivate->mAudioEffectProperties->iLeave = EFalse;
    mTestPrivate->init();
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iLoadFromFile, TBool(ETrue));
}

/*!
 test balance
 */
void TestMpAudioEffectsFrameworkWrapper::testBalance()
{
    QCOMPARE( mTest->balance(), 0 );
    mTest->setBalance( -100 );
    QCOMPARE( mTest->balance(), -100 );
    mTest->setBalance( 100 );
    QCOMPARE( mTest->balance(), 100 );
}

/*!
 test loudness
 */
void TestMpAudioEffectsFrameworkWrapper::testLoudness()
{
    QCOMPARE(mTest->loudness(), false);
    mTest->setLoudness(true);
    QCOMPARE(mTest->loudness(), true);
}

/*!
 test setBalance
 */
void TestMpAudioEffectsFrameworkWrapper::testSetBalance()
{
    mTest->setBalance(-5);
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iBalance, -5);
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iSaveToFile, TBool(ETrue));
}

/*!
 test setLoudness   
 */
void TestMpAudioEffectsFrameworkWrapper::testSetLoudness()
{   
    mTest->setLoudness(true);
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iLoudness, TBool(ETrue));
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iSaveToFile, TBool(ETrue));
    mTest->setLoudness(false);
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iLoudness, TBool(EFalse));
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iSaveToFile, TBool(ETrue));
}

/*!
 test saveToFile
 */
void TestMpAudioEffectsFrameworkWrapper::testSaveToFile()
{
    mTestPrivate->saveToFile();
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iSaveToFile, TBool(ETrue));
    mTestPrivate->mAudioEffectProperties->iLeave = ETrue;
    mTestPrivate->saveToFile();
    QCOMPARE(mTestPrivate->mAudioEffectProperties->iSaveToFile, TBool(EFalse));
}
    
// end of file
