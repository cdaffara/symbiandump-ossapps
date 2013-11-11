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
* Description:   tester for methods in TestProgressBar
*
*/

// Version : %version:  11 %


#include <qdebug>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <qgraphicssceneevent>
#include <hbprogressslider.h>


#include "testprogressbar.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"

#define private public
#include "videoplaybackprogressbar.h"
#undef private

// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestProgressBar::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestProgressBar tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testprogressbar.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}


// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::init()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::init()"));
}

// -------------------------------------------------------------------------------------------------
// setup
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::setup()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::setup()"));

    mController = new VideoPlaybackControlsController();
    mProgBar = new VideoPlaybackProgressBar( mController );

    mProgBar->initialize();
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::cleanup()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mProgBar )
    {
        delete mProgBar;
        mProgBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderPressed
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testHandleSliderPressed()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testHandleSliderPressed()"));

    setup();

    //
    // If it is playing state
    //
    mController->mState = EPbStatePlaying;

    emit mProgBar->mProgressSlider->press();

    QVERIFY( mController->mTimerAction == ETimerCancel );
    QVERIFY( mController->mCommand == EMPXPbvCmdCustomPause );
    QVERIFY( mProgBar->mSliderDragging );
    QVERIFY( mProgBar->mNeedToResumeAfterSetPosition );

    //
    // If it is pause state
    //
    mController->mState = EPbStatePaused;
    mController->mCommand = EMPXPbvCmdStop;

    emit mProgBar->mProgressSlider->press();

    QVERIFY( mController->mTimerAction == ETimerCancel );
    QVERIFY( mController->mCommand == EMPXPbvCmdStop );
    QVERIFY( mProgBar->mSliderDragging );
    QVERIFY( mProgBar->mNeedToResumeAfterSetPosition );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderReleased
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testHandleSliderReleased()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testHandleSliderReleased()"));

    setup();

    //
    // Moved position >= duration
    //
    mProgBar->mDuration = 120;
    mProgBar->mProgressSlider->mSliderValue = 130;
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdStop;

    emit mProgBar->mProgressSlider->release();
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( ! mProgBar->mSliderDragging );
    QVERIFY( mController->mCommand == EMPXPbvCmdEndOfClip );

    //
    // Moved position < 0
    //
    mProgBar->mProgressSlider->mSliderValue = -1;
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdStop;
    mController->mValue = 100;

    emit mProgBar->mProgressSlider->release();
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( ! mProgBar->mSliderDragging );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mValue == 0 );

    //
    // 0 <= moved position < duration
    //
    mProgBar->mProgressSlider->mSliderValue = 60;
    mController->mTimerAction = ETimerCancel;
    mController->mCommand = EMPXPbvCmdStop;

    emit mProgBar->mProgressSlider->release();
    QVERIFY( mController->mTimerAction == ETimerReset );
    QVERIFY( ! mProgBar->mSliderDragging );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mValue == mProgBar->mProgressSlider->mSliderValue );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderMoved
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testHandleSliderMoved()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testHandleSliderMoved()"));

    setup();

    //
    // If user is dragging the slider
    //
    int movedPositoin = 5;
    mProgBar->mDuration = 20;
    mProgBar->mSliderDragging = true;
    emit mProgBar->mProgressSlider->move( movedPositoin );

    QVERIFY( mProgBar->mDraggingPosition == movedPositoin );
    QVERIFY( mProgBar->mProgressSlider->progressValue() == movedPositoin );
    QVERIFY( mProgBar->mProgressSlider->sliderValue() == movedPositoin );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testUpdateWithFileDetails()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testUpdateWithFileDetails()"));

    setup();

    //
    // pausable + non seekable
    //
    mController->mFileDetails->mPausableStream = true;
    mController->mFileDetails->mSeekable = false;
    mProgBar->updateWithFileDetails( mController->mFileDetails );
    QVERIFY( ! mProgBar->mProgressSlider->isEnabled() );

    //
    // non pausable + seekable
    //
    mController->mFileDetails->mPausableStream = false;
    mController->mFileDetails->mSeekable = true;
    mProgBar->updateWithFileDetails( mController->mFileDetails );
    QVERIFY( ! mProgBar->mProgressSlider->isEnabled() );

    //
    // non pausable + non seekable
    //
    mController->mFileDetails->mPausableStream = false;
    mController->mFileDetails->mSeekable = false;
    mProgBar->updateWithFileDetails( mController->mFileDetails );
    QVERIFY( ! mProgBar->mProgressSlider->isEnabled() );

    //
    // other cases
    //
    mController->mFileDetails->mPausableStream = true;
    mController->mFileDetails->mSeekable = true;
    mProgBar->updateWithFileDetails( mController->mFileDetails );
    QVERIFY( mProgBar->mProgressSlider->isEnabled() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testUpdateState
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testUpdateState()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testUpdateState()"));

    setup();

    //
    // test for 1st block of cases
    //
    mController->mFileDetails->mPausableStream = true;
    mController->mFileDetails->mSeekable = false;
    mProgBar->updateState( EPbStatePlaying );
    QVERIFY( ! mProgBar->mProgressSlider->isEnabled() );

    mController->mFileDetails->mPausableStream = true;
    mController->mFileDetails->mSeekable = true;
    mProgBar->updateState( EPbStatePaused );
    QVERIFY( mProgBar->mProgressSlider->isEnabled() );

    //
    // test for 2nd block of cases
    //
    mProgBar->updateState( EPbStatePluginSeeking );
    QVERIFY( mProgBar->mProgressSlider->isEnabled() );

    //
    // test for 3rd block of cases
    //
    mProgBar->updateState( EPbStateBuffering );
    QVERIFY( ! mProgBar->mProgressSlider->isEnabled() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testDurationChanged
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testDurationChanged()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testDurationChanged()"));

    setup();

    int duration = 121;
    mProgBar->durationChanged( duration );

    QVERIFY( mProgBar->mDuration == duration );
    QVERIFY( mProgBar->mProgressSlider->maximum() == duration );
    QVERIFY( mProgBar->mProgressSlider->maxText() == "2:01" );

    duration = 730;
    mProgBar->durationChanged( duration );

    QVERIFY( mProgBar->mDuration == duration );
    QVERIFY( mProgBar->mProgressSlider->maximum() == duration );
    QVERIFY( mProgBar->mProgressSlider->maxText() == "12:10" );

    duration = 7413;
    mProgBar->durationChanged( duration );

    QVERIFY( mProgBar->mDuration == duration );
    QVERIFY( mProgBar->mProgressSlider->maximum() == duration );
    QVERIFY( mProgBar->mProgressSlider->maxText() == "2:03:33" );

    duration = 37803;
    mProgBar->durationChanged( duration );

    QVERIFY( mProgBar->mDuration == duration );
    QVERIFY( mProgBar->mProgressSlider->maximum() == duration );
    QVERIFY( mProgBar->mProgressSlider->maxText() == "10:30:03" );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testPositionChanged
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testPositionChanged()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testPositionChanged()"));

    setup();

    //
    // position < duration
    //
    int position = 60;
    int duration = 120;
    mProgBar->durationChanged( duration );

    mProgBar->mSliderDragging = false;
    mProgBar->positionChanged( position );

    QVERIFY( mProgBar->mProgressSlider->minText() == "1:00" );
    QVERIFY( mProgBar->mProgressSlider->sliderValue() == position );
    QVERIFY( mProgBar->mProgressSlider->progressValue() == position );

    //
    // position > duration
    //
    position = 130;

    mProgBar->positionChanged( position );

    QVERIFY( mProgBar->mProgressSlider->minText() == "2:00" );
    QVERIFY( mProgBar->mProgressSlider->sliderValue() == duration );
    QVERIFY( mProgBar->mProgressSlider->progressValue() == duration );

    //
    // position < 0
    //
    position = -1;

    mProgBar->positionChanged( position );

    QVERIFY( mProgBar->mProgressSlider->minText() == "0:00" );
    QVERIFY( mProgBar->mProgressSlider->sliderValue() == 0 );
    QVERIFY( mProgBar->mProgressSlider->progressValue() == 0 );

    //
    // duration >= 1 hour
    //
    duration = 3600;
    mProgBar->durationChanged( duration );

    position = 5;
    mProgBar->positionChanged( position );

    QVERIFY( mProgBar->mProgressSlider->minText() == "0:00:05" );
    QVERIFY( mProgBar->mProgressSlider->sliderValue() == position );
    QVERIFY( mProgBar->mProgressSlider->progressValue() == position );
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSeekingTimeout
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testHandleSeekingTimeout()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testHandleSeekingTimeout()"));

    setup();

    mProgBar->mDraggingPosition = 10;
    mProgBar->mDuration = 30;
    mProgBar->mSetPosition = 0;

    connect( this, SIGNAL( commandSignal() ), mProgBar, SLOT( handleSeekingTimeout() ) );
    emit commandSignal();

    QVERIFY( mController->mCommand == EMPXPbvCmdSetPosition );
    QVERIFY( mController->mValue == mProgBar->mDraggingPosition );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testResetControl
// -------------------------------------------------------------------------------------------------
//
void TestProgressBar::testResetControl()
{
    MPX_ENTER_EXIT(_L("TestProgressBar::testResetControl()"));

    setup();

    mController->mTimerAction = ETimerCancel;
    mProgBar->mSliderDragging = true;
    mProgBar->resetControl();

    QVERIFY( ! mProgBar->mSliderDragging );
    QVERIFY( mController->mTimerAction == ETimerReset );

    mController->mTimerAction = ETimerCancel;
    mProgBar->mSliderDragging = false;
    mProgBar->resetControl();

    QVERIFY( ! mProgBar->mSliderDragging );
    QVERIFY( mController->mTimerAction == ETimerCancel );

    cleanup();
}

// End of file
