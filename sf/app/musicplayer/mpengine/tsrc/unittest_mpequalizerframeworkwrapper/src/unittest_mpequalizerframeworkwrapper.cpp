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
* Description: Unit test for mpequalizerframeworkwrapper
*
*/

#include <QSignalSpy>
#include <QStringList>

#include "unittest_mpequalizerframeworkwrapper.h"
#include "stub/inc/audioequalizerutility.h"
#include "stub/inc/mpxuser.h"

// Do this so we can access all member variables.
#define private public
#include "mpequalizerframeworkwrapper.h"
#include "mpequalizerframeworkwrapper_p.h"
#undef private

//This so we can test private functions
#include "mpequalizerframeworkwrapper_p.cpp"

//    Predifined values
//sa stub/inc/audioequalizerutility.cpp for Predifined Values
//PresetName    PresetEngineId  PresetId
//  Off              -1             -1
//  Bass             0x0A           0
//  Classical        0x0B           1
//  Pop              0x0C           2
//  Jazz             0x0D           3
//  Rock             0x0E           4

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    TestMpEqualizerFrameworkWrapper tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpequalizerframeworkwrapper.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpEqualizerFrameworkWrapper::TestMpEqualizerFrameworkWrapper()
    : mTest(0)
{
}

TestMpEqualizerFrameworkWrapper::~TestMpEqualizerFrameworkWrapper()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpEqualizerFrameworkWrapper::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpEqualizerFrameworkWrapper::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpEqualizerFrameworkWrapper::init()
{
    mTest = new MpEqualizerFrameworkWrapper();
    mTestPrivate = mTest->d_ptr;
}

/*!
 Called after every testfunction.
 */
void TestMpEqualizerFrameworkWrapper::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
 */
void TestMpEqualizerFrameworkWrapper::testConstructor()
{
    QVERIFY(mTestPrivate->mMdaPlayUtility != 0);
}

/*!
 Tests MapcInitComplete.
 */
void TestMpEqualizerFrameworkWrapper::testMapcInitComplete()
{   
    QSignalSpy spyEqualizerReady(mTest, SIGNAL(equalizerReady()));
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);

    QVERIFY(mTestPrivate->mEqualizerUtility != 0);
    QCOMPARE(spyEqualizerReady.count(), 1);
}

/*!
 Tests getPresetId
 */
void TestMpEqualizerFrameworkWrapper::testGetPresetIndex()
{       
    TInt presetIndex(0);
    
    // When EqualizerUtility is not ready
    presetIndex = mTestPrivate->getPresetIndex(0x0B);    
    QVERIFY(presetIndex == 0);
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);
    
    //When a valid preset is currently set
    presetIndex = mTestPrivate->getPresetIndex(0x0B);    
    QVERIFY(presetIndex == 2);
    
    //When there is no any preset set
    presetIndex = mTestPrivate->getPresetIndex(KEqualizerPresetNone);    
    QVERIFY(presetIndex == 0);

    //When an invalid preset is currently set
    presetIndex = mTestPrivate->getPresetIndex(0xFF);    
    QVERIFY(presetIndex == 0);
}

/*!
 Tests GetEngineIndexL.
 */
void TestMpEqualizerFrameworkWrapper::testGetPresetNameKey()
{   
    // When EqualizerUtility is not ready
    QVERIFY(mTestPrivate->getPresetNameKey(2) == KEqualizerPresetNone);    
    
    mTestPrivate->MapcInitComplete(KErrNone, 0);    
    
    // test valid index.
    QVERIFY(mTestPrivate->getPresetNameKey(2) == 0x0C);    
    
    // test invalid index.
    QVERIFY(mTestPrivate->getPresetNameKey(99) == KEqualizerPresetNone);    
}

/*!
 Tests presetNames
 */
void TestMpEqualizerFrameworkWrapper::testPresetNames()
{
    // test negative case - don't call MapcInitComplete so EqualizerUtility is null.
    QStringList presetNames = mTest->presetNames();
    QVERIFY(presetNames.count() == 0);
    
    // test valid case
    mTestPrivate->MapcInitComplete(KErrNone, 0);
    presetNames = mTest->presetNames();
    
    QVERIFY(presetNames.at(0) == "Bass");
    QVERIFY(presetNames.at(1) == "Classical");
    QVERIFY(presetNames.at(2) == "Pop");
    QVERIFY(presetNames.at(3) == "Jazz");
    QVERIFY(presetNames.at(4) == "Rock");    
}

//end of file
