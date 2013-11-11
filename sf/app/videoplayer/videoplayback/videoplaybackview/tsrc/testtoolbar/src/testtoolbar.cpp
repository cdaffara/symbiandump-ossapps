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
* Description:   tester for methods in TestToolBar
*
*/

// Version : %version:  9 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>

#include "hbtoolbar.h"
#include "hbtoolbutton.h"
#include "testtoolbar.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackdocumentloader.h"

#define private public
#include "videoplaybacktoolbar.h"
#undef private

// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestToolBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestToolBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testtoolbar.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::init( bool attachOperation )
{
    MPX_ENTER_EXIT(_L("TestToolBar::init()"));

    mController = new VideoPlaybackControlsController( attachOperation );
    mVideoToolBar = new VideoPlaybackToolBar( mController );

    mVideoToolBar->setVisible( true );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( retrieveButtons() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( retrieveButtons() ) );
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestToolBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mVideoToolBar )
    {
        delete mVideoToolBar;
        mVideoToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testPlay
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testPlay()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testPlay()"));

    init();

    mVideoToolBar->mButtons[E3rdButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testLongTapOnFF
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testLongTapOnFF()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testLongTapOnFF()"));

    init();

    //
    // Start seeking
    //
    mVideoToolBar->mButtons[E4thButton]->press();
    QVERIFY( mController->mTimerAction == ETimerCancel );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( ffPressing() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( ffPressing() ) );

    QVERIFY( mVideoToolBar->mSeekingState == EFastForwarding );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekForward );

    //
    // End seeking
    //
    mVideoToolBar->mButtons[E4thButton]->release();

    QVERIFY( mVideoToolBar->mSeekingState == ENotSeeking );
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testLongTapOnRW
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testLongTapOnRW()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testLongTapOnRW()"));

    init();

    //
    // Start seeking
    //
    mVideoToolBar->mButtons[E2ndButton]->press();
    QVERIFY( mController->mTimerAction == ETimerCancel );

    connect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( rwPressing() ) );
    emit commandSignal();
    disconnect( this, SIGNAL( commandSignal() ), mVideoToolBar, SLOT( rwPressing() ) );

    QVERIFY( mVideoToolBar->mSeekingState == ERewinding );
    QVERIFY( mController->mCommand == EMPXPbvCmdSeekBackward );

    //
    // End seeking
    //
    mVideoToolBar->mButtons[E2ndButton]->release();

    QVERIFY( mVideoToolBar->mSeekingState == ENotSeeking );
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testShortTapOnFF
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testShortTapOnFF()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testShortTapOnFF()"));

    init();

    //
    // If mPostion + KMPXFastForward < mDuration
    //
    mVideoToolBar->mPosition = 30;
    mVideoToolBar->mDuration = 70;
    mVideoToolBar->mButtons[E4thButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == mVideoToolBar->mPosition + KMPXFastForward );

    //
    // If mPostion + KMPXFastForward < mDuration
    //
    mVideoToolBar->mDuration = 50;
    mController->mCommand = EMPXPbvCmdPlay;

    mVideoToolBar->mButtons[E4thButton]->release();
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlay );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testShortTapOnRW
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testShortTapOnRW()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testShortTapOnRW()"));

    init();

    //
    // If mPostion + KMPXRewind > 0
    //
    mVideoToolBar->mPosition = 30;
    mVideoToolBar->mButtons[E2ndButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == mVideoToolBar->mPosition + KMPXRewind );

    //
    // If mPostion + KMPXRewind < 0
    //
    mVideoToolBar->mPosition = 4;
    mVideoToolBar->mButtons[E2ndButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mCommandValue == 0 );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testPause
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testPause()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testPause()"));

    init();

    mVideoToolBar->mButtons[E3rdButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlayPause );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testChangeAspectRatio
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testChangeAspectRatio()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testChangeAspectRatio()"));

    init();

    //
    // Test natural
    //
    mVideoToolBar->mAspectRatio = EMMFZoom;
    mVideoToolBar->mButtons[E1stButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdNaturalAspectRatio );

    //
    // Test stretch
    //
    mVideoToolBar->mAspectRatio = EMMFStretch;
    mVideoToolBar->mButtons[E1stButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdZoomAspectRatio );

    //
    // Test stretch
    //
    mVideoToolBar->mAspectRatio = EMMFNatural;
    mVideoToolBar->mButtons[E1stButton]->release();

    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdStretchAspectRatio );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateState
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testUpdateState()"));

    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // Playing
    //
    TMPXPlaybackState state = EPbStatePlaying;
    details->mPausableStream = false;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == false );

    //
    // Paused
    //
    state = EPbStatePaused;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );

    //
    // Seeking
    //
    state = EPbStatePluginSeeking;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );

    //
    // Not initialized
    //
    state = EPbStateNotInitialised;
    mVideoToolBar->updateState( state );

    QVERIFY( mVideoToolBar->mToolBar->mEnabled == false );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testAspectRatioChanged
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testAspectRatioChanged()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testAspectRatioChanged()"));

    init();

    //
    // Natural
    //
    int aspectRatio = EMMFNatural;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFNatural );

    //
    // Stretch
    //
    aspectRatio = EMMFStretch;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFStretch );

    //
    // Zoom
    //
    aspectRatio = EMMFZoom;
    mVideoToolBar->aspectRatioChanged( aspectRatio );
    QVERIFY( mVideoToolBar->mAspectRatio == EMMFZoom );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleButtonPressed
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testHandleButtonPressed()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testHandleButtonPressed()"));

    init();

    mVideoToolBar->mButtons[E1stButton]->press();

    QVERIFY( mController->mTimerAction == ETimerCancel );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testUpdateWithFileDetails()"));

    //
    // 'non-attach' mode
    //
    init();

    VideoPlaybackViewFileDetails *details = mController->fileDetails();

    //
    // video clip, view mode is full-screen
    //
    details->mVideoEnabled = true;
    details->mAspectRatioChangeable = true;
    details->mTvOutConnected = false;
    details->mSeekable = true;
    details->mPausableStream = true;
    mController->mViewMode = EFullScreenView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == true );

    //
    // video clip has same aspect ratio as display window
    //
    details->mVideoEnabled = true;
    details->mAspectRatioChangeable = false;
    details->mTvOutConnected = false;
    mController->mViewMode = EFullScreenView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == true );

    //
    // audio-only clip, view mode is full-screen
    //
    details->mVideoEnabled = false;
    details->mAspectRatioChangeable = false;
    details->mTvOutConnected = true;
    details->mSeekable = false;
    details->mPausableStream = true;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == false );

    //
    // local audio-only clip, view mode is audio-only
    //
    details->mVideoEnabled = false;
    details->mAspectRatioChangeable = false;
    details->mTvOutConnected = true;
    details->mSeekable = true;
    details->mPausableStream = false;
    mController->mState = EPbStatePlaying;
    mController->mViewMode = EAudioOnlyView;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == false );

    //
    // streaming audio-only clip, view mode is audio-only, non-pausable stream in 'playing' state
    //
    details->mVideoEnabled = false;
    details->mPlaybackMode = EMPXVideoStreaming;
    mController->mViewMode = EAudioOnlyView;
    details->mSeekable = false;
    details->mPausableStream = false;
    mController->mState = EPbStatePlaying;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == false );

    //
    // streaming audio-only clip, view mode is audio-only, non-pausable stream in 'paused' state
    //
    details->mVideoEnabled = false;
    details->mPlaybackMode = EMPXVideoStreaming;
    mController->mViewMode = EAudioOnlyView;
    details->mSeekable = false;
    details->mPausableStream = false;
    mController->mState = EPbStatePaused;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == false );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == false );

    cleanup();

    //
    // 'attach' mode
    //
    init( true );

    //
    // video clip, view mode is full-screen
    //
    details->mVideoEnabled = true;
    details->mAspectRatioChangeable = false;
    details->mTvOutConnected = false;
    details->mSeekable = true;
    details->mPausableStream = true;

    mVideoToolBar->updateWithFileDetails( details );

    QVERIFY( mVideoToolBar->mButtons[E1stButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E2ndButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E3rdButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E4thButton]->isEnabled() == true );
    QVERIFY( mVideoToolBar->mButtons[E5thButton]->isEnabled() == true );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testOpenDetailsView
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testOpenDetailsView()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testOpenDetailsView()"));

    init();

    QSignalSpy spy( mVideoToolBar, SIGNAL( openDetailsView() ) );

    mController->mViewMode = EFullScreenView;
    mVideoToolBar->mButtons[E5thButton]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EDetailsView;
    mVideoToolBar->mButtons[E5thButton]->release();
    QVERIFY( mController->mViewMode == EDetailsView );

    mController->mViewMode = EAudioOnlyView;
    mVideoToolBar->mButtons[E5thButton]->release();
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testAttach
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testAttach()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testAttach()"));

    //
    // test when 'attach' operation is enabled
    //
    init( true );

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mVideoEnabled = false;
    mVideoToolBar->updateWithFileDetails( details );

    //
    // release 'attach' button
    //
    mVideoToolBar->mButtons[E1stButton]->release();

    //
    // verify the controller attachVideo() slot has been called
    //
    QVERIFY( mController->mCommand == EMPXPbvCmdClose );
    QVERIFY( mController->mAttachVideoDone == true );

    //
    // clean up
    //
    cleanup();

    //
    // test when 'attach' operation is disabled
    //
    init();

    //
    // verify the controller attachVideo() slot is not called
    //
    QVERIFY( mController->mAttachVideoDone == false );

    //
    // clean up
    //
    cleanup();

}

// -------------------------------------------------------------------------------------------------
// testSend
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testSend()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testSend()"));

    //
    // test when 'share' operation is enabled
    //
    init( false );

    VideoPlaybackViewFileDetails *details = mController->fileDetails();
    details->mVideoEnabled = false;
    mController->mViewMode = EAudioOnlyView;
    mVideoToolBar->updateWithFileDetails( details );

    //
    // release 'share' button
    //
    mVideoToolBar->mButtons[E1stButton]->release();

    //
    // verify the controller sendVideo() slot has been called
    //
    QVERIFY( mController->mCommand == EMPXPbvCmdPause );
    QVERIFY( mController->mSendVideoDone == true );

    //
    // clean up
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testResetControl
// -------------------------------------------------------------------------------------------------
//
void TestToolBar::testResetControl()
{
    MPX_ENTER_EXIT(_L("TestToolBar::testResetControl()"));

    init( true );

    //
    // FF
    //
    mVideoToolBar->mSeekingState = EFastForwarding;
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdSeekForward;

    mVideoToolBar->resetControl();

    QVERIFY( mVideoToolBar->mSeekingState == ENotSeeking );
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    //
    // RW
    //
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdSeekForward;
    mVideoToolBar->mSeekingState = ERewinding;

    mVideoToolBar->resetControl();

    QVERIFY( mVideoToolBar->mSeekingState == ENotSeeking );
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndSeek );

    //
    // Not seeking
    //
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdPlay;
    mVideoToolBar->mSeekingState = ENotSeeking;

    mVideoToolBar->resetControl();

    QVERIFY( mVideoToolBar->mSeekingState == ENotSeeking );
    QVERIFY( mController->mTimerAction == ETimerCancel );
    QVERIFY( mController->mCommand == EMPXPbvCmdPlay );

    //
    // clean up
    //
    cleanup();
}

// End of file
