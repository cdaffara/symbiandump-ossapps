/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   tester for methods in TestVideoPlaybackControlBar
*
*/

// Version : %version:  4 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>

#include "testcontrolbar.h"
#include "videoplaybacktoolbar.h"
#include "videoplaybackprogressbar.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"

#define private public
#include "videoplaybackcontrolbar.h"
#undef private

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestControlBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestControlBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testcontrolbar.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestControlBar::init()
{
    MPX_ENTER_EXIT(_L("TestControlBar::init()"));

    mController = new VideoPlaybackControlsController();
    mControlBar = new VideoPlaybackControlBar( mController );

    mControlBar->initialize();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestControlBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestControlBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mControlBar )
    {
        delete mControlBar;
        mControlBar = NULL;
    }
}

// ---------------------------------------------------------------------------
// testupdateState
// ---------------------------------------------------------------------------
//
void TestControlBar::testupdateState()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testupdateState()"));

    init();

    TMPXPlaybackState state = EPbStatePlaying;
    mControlBar->updateState( state );
    QVERIFY( mControlBar->mToolBar->mState == EPbStatePlaying );

    state = EPbStatePaused;
    mControlBar->updateState( state );
    QVERIFY( mControlBar->mToolBar->mState == EPbStatePaused );

    cleanup();
}

// ---------------------------------------------------------------------------
// testaspectRatioChanged
// ---------------------------------------------------------------------------
//
void TestControlBar::testaspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testaspectRatioChanged()"));

    init();

    int aspectRatio = 1;
    mControlBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mControlBar->mToolBar->mAspectRatio == aspectRatio );

    cleanup();
}

// ---------------------------------------------------------------------------
// testUpdateWithFileDetails
// ---------------------------------------------------------------------------
//
void TestControlBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testUpdateWithFileDetails()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mSeekable = false;
    details->mVideoEnabled = true;

    mControlBar->updateWithFileDetails( details );

    QVERIFY( mControlBar->mToolBar->mFileDetails->mSeekable == details->mSeekable );
    QVERIFY( mControlBar->mProgressBar->mFileDetails->mVideoEnabled == details->mVideoEnabled );

    cleanup();
}

// ---------------------------------------------------------------------------
// testSetVisibleToControlBar
// ---------------------------------------------------------------------------
//
void TestControlBar::testSetVisibleToControlBar()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testSetVisibleToControlBar()"));

    init();

    //
    // Make visible
    //
    mControlBar->setVisible( false );
    mControlBar->setVisibleToControlBar( true );
    QVERIFY( mControlBar->isVisible() == true );

    //
    // Make invisible
    //
    mControlBar->setVisible( true );
    mControlBar->setVisibleToControlBar( false );
    QVERIFY( mControlBar->isVisible() == false );

    cleanup();
}

// ---------------------------------------------------------------------------
// testDurationChanged
// ---------------------------------------------------------------------------
//
void TestControlBar::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testDurationChanged()"));

    init();

    int duration = 12345;
    mControlBar->durationChanged( duration );
    QVERIFY( mControlBar->mProgressBar->mDuration == duration );

    cleanup();
}

// ---------------------------------------------------------------------------
// testPositionChanged
// ---------------------------------------------------------------------------
//
void TestControlBar::testPositionChanged()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testPositionChanged()"));

    init();

    int position = 54321;
    mControlBar->positionChanged( position );
    QVERIFY( mControlBar->mProgressBar->mPosition == position );

    cleanup();
}

// ---------------------------------------------------------------------------
// testResetControl
// ---------------------------------------------------------------------------
//
void TestControlBar::testResetControl()
{
    MPX_ENTER_EXIT(_L("TestControlBar::testResetControl()"));

    init();

    QVERIFY( ! mControlBar->mProgressBar->mReset );
    QVERIFY( ! mControlBar->mToolBar->mReset );

    mControlBar->resetControl();

    QVERIFY( mControlBar->mProgressBar->mReset );
    QVERIFY( mControlBar->mToolBar->mReset );

    cleanup();
}

// End of file
