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
* Description:  Videoplayerengine test class implementation.
*
*/

// Version : %version:  6 %

// INCLUDES
#include <QtTest/QtTest>
#include <qdebug.h>
#include <hbapplication.h>

#include "mpxhbvideocommondefs.h"
#include "videoplayerengine.h"
#include "testvideoservices.h"
#include "mpxvideo_debug.h"

#define private public
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoservicebrowse.h"
#include "videoserviceplay.h"
#include "videoserviceuri.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoServices tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoServices.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoServices::TestVideoServices()
    : mTestObject( 0 )
    , mEngine( 0 )
{
    MPX_DEBUG(_L("TestVideoServices::TestVideoServices()"));
}

TestVideoServices::~TestVideoServices()
{
    MPX_ENTER_EXIT(_L("TestVideoServices::~TestVideoServices()"));
    
    delete mTestObject;
    mTestObject = 0;
}


void TestVideoServices::init()
{
    MPX_DEBUG(_L("TestVideoServices::init()"));
}

void TestVideoServices::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVideoServices::cleanup()"));
    
    delete mEngine;
    mEngine = 0;
	
    QCOMPARE( mTestObject->mReferenceCount, 1 );

    if ( mTestObject)
    {
        mTestObject->decreaseReferenceCount();
    }

    mTestObject = 0;
}

void TestVideoServices::testCreateDelete()
{
    MPX_DEBUG(_L("TestVideoServices::testCreateDelete()"));
    
    mTestObject = VideoServices::instance();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mServicePlay );
    QVERIFY( mTestObject->mServiceUriFetch );
    QVERIFY( mTestObject->mServiceBrowse );
    QVERIFY( mTestObject->mServiceView );
    QVERIFY( mTestObject->mServiceUriView );
    QVERIFY( mTestObject->mEngine == 0 );
    QVERIFY( mTestObject->mCurrentService == VideoServices::ENoService );
}

void TestVideoServices::testCreateDeleteEngine()
{
    MPX_DEBUG(_L("TestVideoServices::testCreateDeleteEngine()"));
    
    mEngine = new VideoPlayerEngine();
    mTestObject = VideoServices::instance(mEngine);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mServicePlay );
    QVERIFY( mTestObject->mServiceUriFetch );
    QVERIFY( mTestObject->mServiceBrowse );
    QVERIFY( mTestObject->mServiceView );
    QVERIFY( mTestObject->mServiceUriView );    
    QVERIFY( mTestObject->mEngine == mEngine );
    QVERIFY( mTestObject->mCurrentService == VideoServices::ENoService );
}


void TestVideoServices::testCurrentService()
{
    MPX_DEBUG(_L("TestVideoServices::testCurrentService()"));
    
    mTestObject = VideoServices::instance();

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mServicePlay );
    QVERIFY( mTestObject->mServiceUriFetch );
    QVERIFY( mTestObject->mServiceBrowse );
    QVERIFY( mTestObject->mServiceView );
    QVERIFY( mTestObject->mServiceUriView );    
    QVERIFY( mTestObject->mEngine == 0 );
    QVERIFY( mTestObject->mCurrentService == VideoServices::ENoService );

    mTestObject->mServicePlay->playMedia( QString() );
    QVERIFY( mTestObject->mCurrentService == VideoServices::ENoService );

    mTestObject->mServiceBrowse->browseVideos(0, 0);
    QVERIFY( mTestObject->mCurrentService == VideoServices::EBrowse );

    mTestObject->mServiceUriFetch->fetch( );
    QVERIFY( mTestObject->mCurrentService == VideoServices::EUriFetcher );

    mTestObject->mServicePlay->playMedia( QString() );
    QVERIFY( mTestObject->mCurrentService == VideoServices::EUriFetcher );
    
    cleanup();

    mEngine = new VideoPlayerEngine();
    mTestObject = VideoServices::instance(mEngine);

    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mServicePlay );
    QVERIFY( mTestObject->mServiceUriFetch );
    QVERIFY( mTestObject->mServiceBrowse );
    QVERIFY( mTestObject->mServiceView );
    QVERIFY( mTestObject->mEngine == mEngine );
    QVERIFY( mTestObject->mCurrentService == VideoServices::ENoService );

    mTestObject->mServiceBrowse->browseVideos(0, 0);
    QVERIFY( mTestObject->mCurrentService == VideoServices::EBrowse );

    mTestObject->mServiceUriFetch->fetch( );
    QVERIFY( mTestObject->mCurrentService == VideoServices::EUriFetcher );
    
    mTestObject->mServicePlay->playMedia( QString() );
    QVERIFY( mTestObject->mCurrentService == VideoServices::EPlayback );
    
    mTestObject->mServiceUriView->view( QString() );
    QVERIFY( mTestObject->mCurrentService == VideoServices::EUriView );    
}

void TestVideoServices::cleanupTestCase()
{
    MPX_DEBUG(_L("TestVideoServices::cleanupTestCase()"));
    // all common cleanup here
}


// End of file

