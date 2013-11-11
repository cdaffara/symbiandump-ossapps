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
* Description:   tester for methods in TestVolumeControl
*
*/

// Version : %version:  1 %


#include <qdebug>
#include <QTimer>
#include <hbmainwindow.h>
#include <hbapplication.h>
#include <qgraphicssceneevent>
#include <hbvolumesliderpopup.h>


#include "testvolumecontrol.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"

#define private public
#include "videoplaybackvolumecontrol.h"
#undef private

// -------------------------------------------------------------------------------------------------
// main
// -------------------------------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::Main()"));

    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVolumeControl tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvolumecontrol.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::init()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::init()"));

    mController = new VideoPlaybackControlsController();
    mVolumeControl = new VideoPlaybackVolumeControl( mController );
}

// -------------------------------------------------------------------------------------------------
// init
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::setup()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::init()"));

    init();
    mController->mVolumeSteps = 10;
    mController->fileDetails()->mAudioEnabled = true;
    mVolumeControl->setVisible( true );
}

// -------------------------------------------------------------------------------------------------
// cleanup
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::cleanup()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::cleanup()"));

    if ( mController )
    {
        delete mController;
        mController = NULL;
    }

    if ( mVolumeControl )
    {
        delete mVolumeControl;
        mVolumeControl = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// testVolumeChanged
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testVolumeChanged()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testVolumeChanged()"));

    setup();

    int changedVolume = 3;
    int currentVolume = 5;
    mVolumeControl->mVolume = 5;

    //
    // If mExpectedVolList is not empty
    //
    mVolumeControl->mExpectedVolList.append( changedVolume );
    mVolumeControl->volumeChanged( changedVolume );

    QVERIFY( mVolumeControl->mVolume == currentVolume );

    //
    // If mExpectedVolList is empty
    //
    mVolumeControl->mExpectedVolList.clear();

    //
    // If user is dragging the thumb, we don't update volume
    //
    mVolumeControl->mDragging = true;

    mVolumeControl->volumeChanged( changedVolume );

    QVERIFY( mVolumeControl->mVolume == currentVolume );
    QVERIFY( mVolumeControl->mVolumePopup->isVisible() );

    //
    // If user isn't dragging the thumb, we update volume
    //
    mVolumeControl->mDragging = false;

    mVolumeControl->volumeChanged( changedVolume );

    QVERIFY( mVolumeControl->mVolume == changedVolume );
    QVERIFY( mVolumeControl->mVolumePopup->isVisible() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testSetVisible
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testSetVisible()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testSetVisible()"));

    init();

    //
    // Audio + video clip
    //
    mController->mVolumeSteps = 30;
    mController->fileDetails()->mAudioEnabled = true;

    bool visible = true;

    mVolumeControl->setVisible( visible );

    QVERIFY( mVolumeControl->mVolumePopup->isVisible() == visible );
    QVERIFY( mVolumeControl->mVolumePopup->mMin == 0 );
    QVERIFY( mVolumeControl->mVolumePopup->mMax ==  mController->mVolumeSteps );
    QVERIFY( mVolumeControl->mDraggingHandlerTimer );

    cleanup();

    //
    // Audio only
    //
    init();

    mController->fileDetails()->mAudioEnabled = false;

    mVolumeControl->setVisible( visible );

    QVERIFY( mVolumeControl->mVolumePopup->isVisible() == visible );
    QVERIFY( ! mVolumeControl->mVolumePopup->isEnabled() );
    QVERIFY( ! mVolumeControl->mDraggingHandlerTimer );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testIsVisible
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testIsVisible()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testIsVisible()"));

    setup();

    bool visible = true;
    mVolumeControl->mVolumePopup->mVisible = visible;

    QVERIFY( mVolumeControl->mVolumePopup->isVisible() == visible );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderValueChanged
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testHandleSliderValueChanged()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testHandleSliderValueChanged()"));

    setup();

    int changedValue = 3;
    mVolumeControl->mVolume = 5;

    connect( this, SIGNAL( commandSignal( int ) ), mVolumeControl, SLOT( handleSliderValueChanged( int ) ) );

    //
    // If mDragging is ture
    //
    mVolumeControl->mDragging = true;
    emit commandSignal( changedValue );

    QVERIFY( mVolumeControl->mDraggingVolume == changedValue * mVolumeControl->mVolumeNormalizer );

    //
    // If mDragging is false
    //
    mVolumeControl->mDragging = false;
    emit commandSignal( changedValue );

    QVERIFY( mVolumeControl->mVolume == changedValue * mVolumeControl->mVolumeNormalizer );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetVolume );
    QVERIFY( mController->mValue == changedValue * mVolumeControl->mVolumeNormalizer );

    disconnect( this, SIGNAL( commandSignal( int ) ), mVolumeControl, SLOT( handleSliderValueChanged( int ) ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleMuteIconClicked
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testHandleMuteIconClicked()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testHandleMuteIconClicked()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleMuteIconClicked() ) );

    //
    // mVolumePopup->value() is not 0
    //
    mVolumeControl->mVolumePopup->mValue = 5;

    emit commandSignal();

    QVERIFY( mController->mCommand == EMPXPbvCmdUnMute );

    //
    // mVolumePopup->value() is 0
    //
    mVolumeControl->mVolumePopup->mValue = 0;
    mVolumeControl->mMutedByMuteIcon = false;

    emit commandSignal();

    QVERIFY( mVolumeControl->mMutedByMuteIcon );
    QVERIFY( mController->mCommand == EMPXPbvCmdMute );

    disconnect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleMuteIconClicked() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderPressed
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testHandleSliderPressed()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testHandleSliderPressed()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleSliderPressed() ) );

    mVolumeControl->mDragging = false;
    mVolumeControl->mDraggingHandlerTimer->start();

    emit commandSignal();

    QVERIFY( mVolumeControl->mDragging );
    QVERIFY( mVolumeControl->mDraggingHandlerTimer->isActive() );

    disconnect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleSliderPressed() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleSliderReleased
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testHandleSliderReleased()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testHandleSliderReleased()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleSliderReleased() ) );

    mVolumeControl->mDragging = true;
    mVolumeControl->mDraggingVolume = 3;
    mVolumeControl->mVolume = 5;

    emit commandSignal();

    QVERIFY( ! mVolumeControl->mDragging );
    QVERIFY( ! mVolumeControl->mDraggingHandlerTimer->isActive() );
    QVERIFY( mVolumeControl->mVolume == mVolumeControl->mDraggingVolume );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetVolume );
    QVERIFY( mController->mValue == mVolumeControl->mDraggingVolume );

    disconnect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleSliderReleased() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// testHandleDraggingTimerTimeOut
// -------------------------------------------------------------------------------------------------
//
void TestVolumeControl::testHandleDraggingTimerTimeOut()
{
    MPX_ENTER_EXIT(_L("TestVolumeControl::testHandleDraggingTimerTimeOut()"));

    setup();

    connect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleDraggingTimerTimeOut() ) );

    //
    // mDragging is false
    //
    mVolumeControl->mDragging = false;
    mVolumeControl->mVolume = 6;
    mVolumeControl->mDraggingVolume = 10;

    emit commandSignal();

    QVERIFY( mVolumeControl->mVolume != mVolumeControl->mDraggingVolume );

    //
    // mDragging is true
    //
    mVolumeControl->mDragging = true;

    emit commandSignal();

    QVERIFY( mVolumeControl->mVolume == mVolumeControl->mDraggingVolume );
    QVERIFY( mController->mCommand == EMPXPbvCmdSetVolume );
    QVERIFY( mController->mValue == mVolumeControl->mVolume );


    //
    // mDragging is true && vol == 0 && mMutedByMuteIcon is true
    //
    mVolumeControl->mDragging = true;
    mVolumeControl->mDraggingVolume = 0;
    mVolumeControl->mMutedByMuteIcon = true;

    emit commandSignal();

    QVERIFY( ! mVolumeControl->mMutedByMuteIcon );

    disconnect( this, SIGNAL( commandSignal() ), mVolumeControl, SLOT( handleDraggingTimerTimeOut() ) );

    cleanup();
}

// End of file
