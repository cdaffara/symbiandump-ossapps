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

// Version : %version: 18 %

// INCLUDES
#include <QtTest/QtTest>
#include <qdebug.h>
#include "stub/inc/hbinstance.h"
#include <hbwidget.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <qabstractitemmodel.h>
#include <xqsharablefile.h>
#include <f32file.h>
#include <qfile.h>

#include "afactivitystorage.h"
#include "mpxhbvideocommondefs.h"
#include "testvideoplayerengine.h"
#include "stub/inc/mpxviewpluginqt.h"
#include "stub/inc/videoservices.h"
#include "stub/inc/xqpluginloader.h"
#include "stub/inc/videoactivitystate.h"
#include "../stub/inc/videoplaybackwrapper.h"
#include "mpxvideo_debug.h"

#define private public
#include "videoplayerengine.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);

    TestVideoPlayerEngine tc;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\TestVideoPlayerEngine.txt";

    return QTest::qExec(&tc, 3, pass);
}


TestVideoPlayerEngine::TestVideoPlayerEngine()
    : mTestObject( 0 )
    , mVideoServices( 0 )
    , mCurrentViewPlugin( 0 )
    , mPlaybackViewPlugin( 0 )
    , mCollectionViewPlugin( 0 )
    , mFileDetailsViewPlugin( 0 )
{
    MPX_DEBUG(_L("TestVideoPlayerEngine::TestVideoPlayerEngine()"));
}

TestVideoPlayerEngine::~TestVideoPlayerEngine()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::~TestVideoPlayerEngine()"));
    
    XQPluginLoader::cleanup();
    
    if ( ! mTestObject.isNull() )
    {
        delete mTestObject;
    }
}


void TestVideoPlayerEngine::init( bool isService )
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::init() "), 
                   _L("isService = %d"), isService );
    
    XQPluginLoader::mFailToLoadPluginUid  = -1; 
    
    mCurrentViewPlugin     = 0;
    mPlaybackViewPlugin    = 0;
    mCollectionViewPlugin  = 0;
    mFileDetailsViewPlugin = 0;
    mVideoServices         = 0;

    mTestObject = new VideoPlayerEngine( isService );
    
    if ( isService )
    {
        mTestObject->mIsPlayService = true;
    }
    		
    
}

void TestVideoPlayerEngine::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::cleanup()"));
    
    mCurrentViewPlugin     = 0;
    mPlaybackViewPlugin    = 0;
    mCollectionViewPlugin  = 0;
    mFileDetailsViewPlugin = 0;
    mVideoServices         = 0;

    if ( ! mTestObject.isNull() )
    {
        delete mTestObject;
    }
}


void TestVideoPlayerEngine::testCreateDelete()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testCreateDelete()"));
    
    //Test object creation and deletion
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper == 0 );    
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}


void TestVideoPlayerEngine::testCreateDeleteService()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testCreateDeleteService()"));
    
    //Test object creation and deletion
    init( true );

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper == 0 );    
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == true );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();
    
    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testInitialize()
{
    MPX_ENTER_EXIT(_L("TestVideoServices::testInitialize()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );
    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper == 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper != 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCollectionViewPlugin != mTestObject->mPlaybackViewPlugin );
    QVERIFY( mTestObject->mCollectionViewPlugin != mTestObject->mFileDetailsViewPlugin );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testInitializeService()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testInitializeService()"));
    
    init( true );

    QVERIFY( ! mTestObject.isNull() );
    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper == 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == true );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    
    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper != 0 );
    QVERIFY( mTestObject->mVideoServices != 0 );
    QVERIFY( mTestObject->mIsService == true );
    QVERIFY( VideoServices::mReferenceCount == 1 );
    QVERIFY( mTestObject->mCollectionViewPlugin != mTestObject->mPlaybackViewPlugin );
    // during service initialization, we do not remove possible exiting activation state
    QVERIFY( AfActivityStorage::mActivityCount == 0 );
    QVERIFY( AfActivityStorage::mLastHandledActivity.isEmpty() );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testMultipleInitialize()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testMultipleInitialize()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );
    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );
/*
    mCurrentViewPlugin = mTestObject->mCurrentViewPlugin;
    mPlaybackViewPlugin = mTestObject->mPlaybackViewPlugin;
    mCollectionViewPlugin = mTestObject->mCollectionViewPlugin;
    mFileDetailsViewPlugin = mTestObject->mFileDetailsViewPlugin;

    mTestObject->initialize();

    QVERIFY( mCurrentViewPlugin == mTestObject->mCurrentViewPlugin );
    QVERIFY( mPlaybackViewPlugin == mTestObject->mPlaybackViewPlugin );
    QVERIFY( mCollectionViewPlugin == mTestObject->mCollectionViewPlugin );
    QVERIFY( mFileDetailsViewPlugin == mTestObject->mFileDetailsViewPlugin );
    QVERIFY( VideoServices::mReferenceCount == 0 );
*/
    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}


void TestVideoPlayerEngine::testMultipleInitializeService()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testMultipleInitializeService()"));
    
    init(true);

    QVERIFY( ! mTestObject.isNull() );
    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mVideoServices == 0 );
    QVERIFY( mTestObject->mIsService == true );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mVideoServices != 0);
    QVERIFY( mTestObject->mIsService == true );
    QVERIFY( VideoServices::mReferenceCount == 1 );

    mCurrentViewPlugin = mTestObject->mCurrentViewPlugin;
    mPlaybackViewPlugin = mTestObject->mPlaybackViewPlugin;
    mCollectionViewPlugin = mTestObject->mCollectionViewPlugin;
    mFileDetailsViewPlugin = mTestObject->mFileDetailsViewPlugin;
    mVideoServices = mTestObject->mVideoServices;

    mTestObject->initialize();

    QVERIFY( mCurrentViewPlugin == mTestObject->mCurrentViewPlugin );
    QVERIFY( mPlaybackViewPlugin == mTestObject->mPlaybackViewPlugin );
    QVERIFY( mCollectionViewPlugin == mTestObject->mCollectionViewPlugin );
    QVERIFY( mFileDetailsViewPlugin == mTestObject->mFileDetailsViewPlugin );
    QVERIFY( mVideoServices == mTestObject->mVideoServices );
    QVERIFY( VideoServices::mReferenceCount == 1 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 ); 
}


void TestVideoPlayerEngine::testHandleCommandPreInit()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPreInit()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateCollectionView );
    mTestObject->handleCommand( MpxHbVideoCommon::ActivatePlaybackView );
    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );
    
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testHandleCommandPostInit()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPostInit()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );
    mTestObject->handleCommand( MpxHbVideoCommon::DoDelayedLoad );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );

    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );
    
    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView-1111);

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testHandleCommandPreAndPostInit()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPreAndPostInit()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateCollectionView );
    mTestObject->handleCommand( MpxHbVideoCommon::ActivatePlaybackView );
    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );
    
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->initialize();


    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );
    mTestObject->handleCommand( MpxHbVideoCommon::DoDelayedLoad );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );

    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testHandleCommandPluginFail()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPluginFail()"));
    
    init();

    QVERIFY( !mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllFileDetailsUid;

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );
    mTestObject->handleCommand( MpxHbVideoCommon::DoDelayedLoad );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivatePlaybackView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0);
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY( mTestObject->mCollectionViewPlugin != 0);
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0);
    QVERIFY( mTestObject->mIsService == false);
    QVERIFY( VideoServices::mReferenceCount == 0);

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateVideoDetailsView);

    QVERIFY( mTestObject->mCurrentViewPlugin == 0);

    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false);
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false);

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0);
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testHandleCommandPluginFail1()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPluginFail1()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllCollectionUid;

    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );
    mTestObject->handleCommand( MpxHbVideoCommon::DoDelayedLoad );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );
    
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivatePlaybackView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mPlaybackViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );

    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated()  == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin != 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0);
}

void TestVideoPlayerEngine::testHandleCommandPluginFail2()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleCommandPluginFail2()"));
    
    init();

    QVERIFY( ! mTestObject.isNull() );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    XQPluginLoader::mFailToLoadPluginUid = MpxHbVideoCommon::KMpxVideoPluginDllPlaybackUid;
    
    mTestObject->initialize();

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );
    mTestObject->handleCommand( MpxHbVideoCommon::DoDelayedLoad );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivatePlaybackView );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );

    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand(MpxHbVideoCommon::ActivateCollectionView );

    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mCollectionViewPlugin );

    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() == false );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    mTestObject->handleCommand( MpxHbVideoCommon::ActivateVideoDetailsView );

    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mCurrentViewPlugin == mTestObject->mFileDetailsViewPlugin );

    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() == false );
    QVERIFY( mTestObject->mFileDetailsViewPlugin->activated() );

    QVERIFY( mTestObject->mCurrentViewPlugin != 0 );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin != 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin != 0 );
    QVERIFY( mTestObject->mIsService == false );
    QVERIFY( VideoServices::mReferenceCount == 0 );

    cleanup();

    QVERIFY( VideoServices::mReferenceCount == 0 );
}

void TestVideoPlayerEngine::testPlayMedia()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testPlayMedia()"));
    
    // 1. test with a string
    init();    
    QVERIFY( ! mTestObject.isNull() );
    
    mTestObject->playMedia( QString("c:\\data\\videos\\test.3gp") );
    QVERIFY( mTestObject );
    cleanup();
    
    // 2. test with a file handle
    init();
    QVERIFY( !mTestObject.isNull() );
    
    XQSharableFile sf;

    if ( sf.open("C:\\data\\videos\\test.3gp") )
    {
        RFile rFile;
        bool handleOK = sf.getHandle( rFile );  
        if ( handleOK )
        {
            mTestObject->playMedia( rFile );
        }    
         
        QVERIFY( mTestObject);
        
        sf.close();
    }

    cleanup();

    // 3. test with a filename
    init();
    QVERIFY( ! mTestObject.isNull() );
    
    QFile qfile("C:\\data\\videos\\test.3gp");
    mTestObject->playMedia( qfile.fileName() );
    QVERIFY( mTestObject );
    cleanup();
    
}

void TestVideoPlayerEngine::testSetEmbedded()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testSetEmbedded()"));
    
    init();
    QVERIFY( !mTestObject.isNull() );
    mTestObject->setEmbedded();
    QVERIFY( mTestObject->mEmbedded == true );
    cleanup();
}

void TestVideoPlayerEngine::testInitWithActivityData()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testInitialize()"));
       
    init();
    QVERIFY( !mTestObject.isNull() );
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    
    mTestObject->initialize();
    
    // by default, colleciton view plugin is activated
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mCurrentViewPlugin );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper );
    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
    init();
    QVERIFY( !mTestObject.isNull() );
    QHash<QString, QVariant> activityHash; 
    
    // playback plugin 
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    QVariant data = int( MpxHbVideoCommon::PlaybackView );
    activityHash[KEY_VIEWPLUGIN_TYPE] = data;
    AfActivityStorage::mDataToReturn = activityHash;    
    mTestObject->initialize();
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mCurrentViewPlugin );
    QVERIFY( mTestObject->mPlaybackViewPlugin );
    QVERIFY( mTestObject->mCollectionViewPlugin == 0 );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper );
    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mPlaybackViewPlugin->activated() );
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
    init();
    QVERIFY( !mTestObject.isNull() );
    
    // only collection view and playback view are accepted, so all other cases 
    // ends up into default: collectionview
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    data = int( MpxHbVideoCommon::VideoDetailsView );
    activityHash[KEY_VIEWPLUGIN_TYPE] = data;
    AfActivityStorage::mDataToReturn = activityHash;  
    mTestObject->initialize();
    QVERIFY( mTestObject );
    QVERIFY( mTestObject->mCurrentViewPlugin );
    QVERIFY( mTestObject->mPlaybackViewPlugin == 0 );
    QVERIFY( mTestObject->mCollectionViewPlugin );
    QVERIFY( mTestObject->mFileDetailsViewPlugin == 0 );
    QVERIFY( mTestObject->mPlaybackWrapper );
    QVERIFY( mTestObject->mCurrentViewPlugin->activated() );
    QVERIFY( mTestObject->mCollectionViewPlugin->activated() );
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
    
}

void TestVideoPlayerEngine::testHandleQuitWithActivityData()
{
    MPX_ENTER_EXIT(_L("TestVideoPlayerEngine::testHandleQuitWihtActivityData()"));
    
    // we make sure that engine saves correct plugin type before exit
    // using VideoActivityState since value is saved there before actually
    // being save to activitymanager
    QHash<QString, QVariant> activityHash; 
    QVariant data = int( MpxHbVideoCommon::CollectionView );
    activityHash[KEY_VIEWPLUGIN_TYPE] = data;  
    AfActivityStorage::mDataToReturn = activityHash;  
    init();
    
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    QVERIFY( !mTestObject.isNull() );
    connect( this, SIGNAL(aboutToQuit()), mTestObject, SLOT(handleQuit()) );

    mTestObject->initialize();
    VideoActivityState::mAllDataGetCount = 0;
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    AfActivityStorage::mLastHandledActivity = "";
    emit aboutToQuit();
      
    data = QVariant();    
    data = VideoActivityState::instance().getActivityData( KEY_VIEWPLUGIN_TYPE );
    QVERIFY( data.isValid() );
    QVERIFY( data.toInt() == MpxHbVideoCommon::CollectionView );
    QVERIFY( VideoActivityState::mAllDataGetCount == 1 );
    QVERIFY( AfActivityStorage::mActivityCount == 0 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
    init();
    QVERIFY( ! mTestObject.isNull() );
    connect( this, SIGNAL(aboutToQuit()), mTestObject, SLOT(handleQuit()) );
    
    
    data = int(MpxHbVideoCommon::PlaybackView);
    activityHash[KEY_VIEWPLUGIN_TYPE] = data;
    data.clear();
    data = bool( true );
    activityHash[KEY_LAST_LOCAL_PLAYBACK] = data;
    AfActivityStorage::mDataToReturn = activityHash;  
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    
    mTestObject->initialize();
    VideoActivityState::mAllDataGetCount = 0;
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    AfActivityStorage::mLastHandledActivity = "";
    
    emit aboutToQuit();
    
    data = QVariant();
    data = VideoActivityState::instance().getActivityData( KEY_VIEWPLUGIN_TYPE );
    QVERIFY( data.isValid() );
    QVERIFY( data.toInt() == MpxHbVideoCommon::PlaybackView );
    QVERIFY( VideoActivityState::mAllDataGetCount == 1 );
    QVERIFY( AfActivityStorage::mActivityCount == 0 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
    init();
    QVERIFY( ! mTestObject.isNull() );
    connect( this, SIGNAL(aboutToQuit()), mTestObject, SLOT(handleQuit()) );
    activityHash.clear();
    data.clear();
    data = int( MpxHbVideoCommon::VideoDetailsView );
    activityHash[KEY_VIEWPLUGIN_TYPE] = data;
    AfActivityStorage::mDataToReturn = activityHash;  
    AfActivityStorage::mLastHandledActivity = "";
    AfActivityStorage::mActivityCount = 0;
    
    mTestObject->initialize();
    VideoActivityState::mAllDataGetCount = 0;
    QVERIFY( AfActivityStorage::mActivityCount == -1 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    AfActivityStorage::mLastHandledActivity = "";      
    
    emit aboutToQuit();
       
    data = QVariant();
    data = VideoActivityState::instance().getActivityData( KEY_VIEWPLUGIN_TYPE );
    QVERIFY( data.isValid() );
    QVERIFY( data.toInt() == MpxHbVideoCommon::CollectionView );
    QVERIFY( VideoActivityState::mAllDataGetCount == 1 );
    QVERIFY( AfActivityStorage::mActivityCount == 0 );
    QVERIFY( AfActivityStorage::mLastHandledActivity == ACTIVITY_VIDEOPLAYER_MAINVIEW );
    
    cleanup();
}


void TestVideoPlayerEngine::cleanupTestCase()
{
    MPX_DEBUG(_L("TestVideoPlayerEngine::cleanupTestCase()"));    
    // all common cleanup here
}

// End of file
