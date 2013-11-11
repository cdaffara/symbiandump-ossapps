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
* Description: Unit test for MusicServices
*
*/

#include <qnamespace.h>


#include "unittest_musicservices.h"

// Do this so we can access all member variables.
#define private public
#include "musicservices.h"
#include "getmusicservice.h"
#include "playmusicservice.h"
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
	  QApplication app(argc, argv);
    TestMusicServices tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_musicservices.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMusicServices::TestMusicServices()
    : mTest( 0 )
{
}

TestMusicServices::~TestMusicServices()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMusicServices::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMusicServices::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMusicServices::init()
{
    mTest = new MusicServices();
}

/*!
 Called after every testfunction.
 */
void TestMusicServices::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor.
*/
void TestMusicServices::testConstructor()
{
    QVERIFY(mTest->mFetchService != 0);
    QVERIFY(mTest->mPlayService != 0);
    QCOMPARE(mTest->mCurrentService, MusicServices::ENoService);
    QCOMPARE(mTest->mFetchService->mServiceApp, mTest);
    QCOMPARE(mTest->mPlayService->mServiceApp, mTest);
    QVERIFY(mTest->mFetchService->mRequestIndex == 0);
    QVERIFY(mTest->mPlayService->mRequestIndex == 0);

}

void TestMusicServices::testCurrentService()
{
    mTest->mCurrentService = MusicServices::EUriFetcher;
    QCOMPARE(mTest->currentService(), MusicServices::EUriFetcher);
    mTest->mCurrentService = MusicServices::EPlayback;
    QCOMPARE(mTest->currentService(), MusicServices::EPlayback);
    mTest->mCurrentService = MusicServices::ENoService;
    QCOMPARE(mTest->currentService(), MusicServices::ENoService);
    
}

void TestMusicServices::testSetCurrentService()
{
    mTest->setCurrentService(MusicServices::EUriFetcher);
    QCOMPARE(mTest->currentService(), MusicServices::EUriFetcher);
    mTest->setCurrentService(MusicServices::EPlayback);
    QCOMPARE(mTest->currentService(), MusicServices::EPlayback);
    mTest->setCurrentService(MusicServices::ENoService);
    QCOMPARE(mTest->currentService(), MusicServices::ENoService);
    
}



//end of file
