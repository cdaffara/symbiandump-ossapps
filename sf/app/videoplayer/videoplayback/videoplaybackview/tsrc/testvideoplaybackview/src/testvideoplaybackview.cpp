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
* Description:   tester for methods in VideoSortFilterProxyModel
*
*/

// Version : %version: 17 %

#include <e32err.h>
#include <w32std.h>

#include <mmf/common/mmferrors.h>
#include <caf/caferr.h>

#include <mpxvideoplaybackdefs.h>
#include <mpxhbvideocommondefs.h>

#include <hbapplication.h>
#include <hbinstance.h>
#include <qgraphicssceneevent>
#include <QDebug>

#include "hbdialog.h"
#include "hbmessagebox.h"
#include "hbnotificationdialog.h"

#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"

#include "testvideoplaybackview.h"
#include "mpxvideoviewwrapper.h"
#include "hbtapgesture.h"
#include "hbpangesture.h"
#include "videoactivitystate.h"

#define private public
#define protected public
#include "videoplaybackview.h"
#include "videobaseplaybackview.h"
#undef protected
#undef private


// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestVideoPlaybackView tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testvideoplaybackview.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::init()
{
    mVideoView = new VideoPlaybackView();
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::cleanup()
{
    delete mVideoView;
    mVideoView = NULL;
}

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
//
void TestVideoPlaybackView::setup()
{
    init();
    mVideoView->handleActivateView();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleActivateView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleActivateView()
{
    init();
    mVideoView->handleActivateView();

    QVERIFY( mVideoView->mVideoMpxWrapper->iMediaRequested );
    QVERIFY( mVideoView->mVideoMpxWrapper->iActivated );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mBitRate, 16000 );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mTitle, QString("Test Video Title") );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandlePluginError()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandlePluginError()
{
    //
    // playback view is closed after displaying these error notes
    //
    verifyHandlePluginError( KErrNotSupported );
    verifyHandlePluginError( KErrUnknown );
    verifyHandlePluginError( KErrMMDecoder );
    verifyHandlePluginError( KErrCorrupt );
    verifyHandlePluginError( KErrTooBig );
    verifyHandlePluginError( KErrMMInvalidProtocol );
    verifyHandlePluginError( KErrMMInvalidURL );
    verifyHandlePluginError( KErrArgument );
    verifyHandlePluginError( KErrSessionClosed );
    verifyHandlePluginError( KErrTimedOut );
    verifyHandlePluginError( KErrNotFound );
    verifyHandlePluginError( KErrMMNotEnoughBandwidth );
    verifyHandlePluginError( KErrDisconnected );
    verifyHandlePluginError( KErrMMProxyServer );
    verifyHandlePluginError( KErrCouldNotConnect );
    verifyHandlePluginError( KErrAbort );
    verifyHandlePluginError( KErrCancel );
    verifyHandlePluginError( KErrMMDRMNotAuthorized );
    verifyHandlePluginError( KErrCANoRights );
    verifyHandlePluginError( KErrCANoPermission );

    //
    // playback view remains open after displaying these error notes
    //
    verifyHandlePluginError( KMPXVideoCallOngoingError, false );
    verifyHandlePluginError( KMPXVideoPlayOver2GDuringVoiceCallError, false );

    //
    // default error case, playback view is closed after displaying error note
    //
    verifyHandlePluginError( KErrGeneral );
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testShowDialog()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testShowDialog()
{
    //
    // construct and activate playback view
    //
    setup();

    HbMessageBox::mMessageBConstructCount = 0;
    HbNotificationDialog::mNotifConstructCount = 0;
    //
    // ensure that playback view is currently activated
    //
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );

    //
    // test showDialog() method using default arguments
    //
    mVideoView->showDialog( "test error msg" );

    QVERIFY(HbMessageBox::mMessageBConstructCount == 1);
    QVERIFY(HbNotificationDialog::mNotifConstructCount == 0);
    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    mVideoView->handleActivateView();
    HbMessageBox::mMessageBConstructCount = 0;
    HbNotificationDialog::mNotifConstructCount = 0;
    mVideoView->mTimerForClosingView->stop();

    //
    // test showDialog() method, error, no closing
    //
    mVideoView->showDialog( "test error msg", false );

    QVERIFY(HbMessageBox::mMessageBConstructCount == 1);
    QVERIFY(HbNotificationDialog::mNotifConstructCount == 0);
    QVERIFY( !mVideoView->mTimerForClosingView->isActive() );

    HbMessageBox::mMessageBConstructCount = 0;
    HbNotificationDialog::mNotifConstructCount = 0;

    //
    // test showDialog() method, nofitification, closing
    //
    mVideoView->showDialog( "test error msg", true, false );

    QVERIFY(HbMessageBox::mMessageBConstructCount == 0);
    QVERIFY(HbNotificationDialog::mNotifConstructCount == 1);
    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    mVideoView->handleActivateView();
    HbMessageBox::mMessageBConstructCount = 0;
    HbNotificationDialog::mNotifConstructCount = 0;
    mVideoView->mTimerForClosingView->stop();

    //
    // test showDialog() method, nofitification, not closing
    //
    mVideoView->showDialog( "test error msg", false, false );

    QVERIFY(HbMessageBox::mMessageBConstructCount == 0);
    QVERIFY(HbNotificationDialog::mNotifConstructCount == 1);
    QVERIFY( !mVideoView->mTimerForClosingView->isActive() );


    //
    // destruct playback view
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testGetWindow()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testGetWindow()
{
    setup();

    QVERIFY( mVideoView->getWindow() != NULL );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testIssuePlayCommand()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testIssuePlayCommand()
{
    setup();

    mVideoView->issuePlayCommand();

    //JAM remove
    QVERIFY( mVideoView->isEnabled() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testEventFilter()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testEventFilter()
{
    setup();

    //
    // ensure default foreground is false
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iViewForeground, 0 );

    //
    // declare foreground/background event
    //
    QEvent* appActivate   = new QEvent( QEvent::ApplicationActivate );
    QEvent* appDeactivate = new QEvent( QEvent::ApplicationDeactivate );

    //
    // verify the returned value (consumed) of VideoBasePlaybackView::event()
    // for QEvent::ApplicationActivate is false
    //
    QVERIFY( ! mVideoView->eventFilter( NULL, appActivate ) );

    //
    // verify view is in foreground
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iViewForeground, 1 );

    //
    // verify the returned value (consumed) of VideoBasePlaybackView::event()
    // for QEvent::ApplicationDeactivate is false
    //
    QVERIFY( ! mVideoView->eventFilter( NULL, appDeactivate ) );

    //
    // verify view is in background
    //
    QCOMPARE( mVideoView->mVideoMpxWrapper->iViewForeground, 0 );

    //
    // clean up
    //
    if ( appActivate )
    {
        delete appActivate;
        appActivate = NULL;
    }

    if ( appDeactivate )
    {
        delete appDeactivate;
        appDeactivate = NULL;
    }

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testClosePlaybackView()
{
    setup();

    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );

    mVideoView->closePlaybackView();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandlePdlStateChange()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandlePdlStateChange()
{
    setup();

    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mClipName, QString("testClip.3gp") );

    mVideoView->handlePdlStateChange( 0 );

    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mClipName, QString("testClip.3gp") );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleStoppedStateNoPrevView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleStoppedStateNoPrevView()
{
    setup();

    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    mVideoView->handleStoppedState();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleStoppedStatePrevView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleStoppedStatePrevView()
{
    setup();

    mVideoView->handleStoppedState();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();

}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleBackNoPrevView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleBackNoPrevView()
{
    setup();

    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    mVideoView->handleBack();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();

}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleBackPrevView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleBackPrevView()
{
    setup();

    mVideoView->handleBack();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleSoftkeyback()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleSoftkeyback()
{
    setup();

    mVideoView->handleSoftkeyBack();

    QVERIFY( mVideoView->mTimerForClosingView->isActive() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testDoClosePlayer()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testDoClosePlayer()
{
    // quits app - which can be problematic for the qvptestrunner app

//    setup();
//
//    mVideoView->handleActivateView();
//
//    mVideoView->doClosePlayer();
//
//    cleanup();

}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleDeactivateView()
{
    setup();

    mVideoView->handleDeactivateView();

    QVERIFY( mVideoView->mVideoMpxWrapper != NULL );
    QVERIFY( ! mVideoView->mVideoMpxWrapper->iActivated );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::verifyHandlePluginError()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::verifyHandlePluginError( TInt error, bool closeView )
{
    //
    // construct and activate playback view
    //
    setup();

    //
    // ensure that playback view is currently open
    //
    QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );

    //
    // test handlePluginError() method
    //
    mVideoView->handlePluginError( error );

    //
    // close playback view after error note has been launched
    //
    if ( closeView )
    {
        //
        // connect and emit signal for handleClosePopupDialog() slot
        //
        connect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
        emit commandSignal();

        //
        // verify that playback view is properly closed
        //
        QVERIFY( mVideoView->mTimerForClosingView->isActive() );

        //
        // disconnect signal for handleClosePopupDialog() slot
        //
        disconnect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleClosePopupDialog() ) );
    }
    else
    {
        //
        // verify that playback view is still open after error note has been launched
        //
        QVERIFY( ! mVideoView->mTimerForClosingView->isActive() );
    }

    //
    // destruct playback view
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testGestureEvent()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testGestureEvent()
{
    setup();

    //
    // 1. Test tap gesture
    //
    QList<QGesture *> list;
    HbTapGesture *tapGesture = new HbTapGesture();
    list.append( tapGesture );
    QGestureEvent* event = new QGestureEvent( list );

    //
    // connect to tappedOnScreen() signal and record its emission
    //
    QSignalSpy spy( mVideoView, SIGNAL( tappedOnScreen() ) );

    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE( spy.count(), 0 );

    //
    // With GestureStarted
    //
    tapGesture->mState = Qt::GestureStarted;
    mVideoView->gestureEvent( event );

    //
    // ensure signal hasn't been emitted and its emission count is not incremented
    //
    QCOMPARE( spy.count(), 0 );

    //
    // With GestureFinished
    //
    tapGesture->mState = Qt::GestureFinished;
    mVideoView->gestureEvent( event );

    //
    // ensure signal has been emitted and its emission count is incremented
    //
    QCOMPARE( spy.count(), 1 );

    //
    // clean up
    //
    spy.clear();

    list.clear();

    if ( tapGesture )
    {
        delete tapGesture;
        tapGesture = NULL;
    }

    if ( event )
    {
        delete event;
        event = NULL;
    }

    //
    // 2. Test pan gesture
    //
    HbPanGesture *panGesture = new HbPanGesture();
    list.append( panGesture );
    event = new QGestureEvent( list );

    //
    // connect to pannedToRight() signal and record its emission
    //
    QSignalSpy spy1( mVideoView, SIGNAL( pannedToRight() ) );

    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE( spy1.count(), 0 );

    //
    // Test pan gesture With GestureStarted
    //
    panGesture->mState = Qt::GestureStarted;
    mVideoView->gestureEvent( event );

    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE( spy1.count(), 0 );

    //
    // 1. Test pan gesture with GestureFinished
    //
    panGesture->mState = Qt::GestureFinished;
    panGesture->mSceneVelocity = QPointF( 5, 0 );
    panGesture->mSceneOffset = QPointF( 205, 0 );
    mVideoView->gestureEvent( event );

    //
    // ensure signal has been emitted
    //
    QCOMPARE( spy1.count(), 1 );

    //
    // 2. Test pan gesture with GestureFinished
    //
    panGesture->mSceneVelocity = QPointF( 5, 0 );
    panGesture->mSceneOffset = QPointF( 105, 0 );
    mVideoView->gestureEvent( event );

    //
    // ensure signal has not been emitted
    //
    QCOMPARE( spy1.count(), 1 );

    spy1.clear();

    //
    // 3. Test pan gesture with GestureFinished
    //

    //
    // connect to pannedToRight() signal and record its emission
    //
    QSignalSpy spy2( mVideoView, SIGNAL( pannedToLeft() ) );

    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE( spy2.count(), 0 );

    panGesture->mState = Qt::GestureFinished;
    panGesture->mSceneVelocity = QPointF( -5, 0 );
    panGesture->mSceneOffset = QPointF( -205, 0 );
    mVideoView->gestureEvent( event );

    //
    // ensure signal has been emitted yet
    //
    QCOMPARE( spy2.count(), 1 );

    //
    // 4. Test pan gesture with GestureFinished
    //
    panGesture->mSceneVelocity = QPointF( -5, 0 );
    panGesture->mSceneOffset = QPointF( -105, 0 );
    mVideoView->gestureEvent( event );

    //
    // ensure signal has not been emitted yet
    //
    QCOMPARE( spy2.count(), 1 );

    //
    // clean up
    //
    spy2.clear();
    list.clear();

    if ( panGesture )
    {
        delete panGesture;
        panGesture = NULL;
    }

    if ( event )
    {
        delete event;
        event = NULL;
    }

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testReactivationAfterPriorTermination()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testReactivationAfterPriorTermination()
{
    QVariant data = int( 10 );
    VideoActivityState::instance().setActivityData(data, KEY_LAST_PLAY_POSITION_ID);

    data = int ( MpxHbVideoCommon::PlaybackView );
    VideoActivityState::instance().setActivityData(data, KEY_VIEWPLUGIN_TYPE);

    init();
    mVideoView->handleActivateView();

    QVERIFY( mVideoView->mVideoMpxWrapper->iMediaRequested == true );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mBitRate, 16000 );
    QCOMPARE( mVideoView->mVideoMpxWrapper->iFileDetails->mTitle, QString("Test Video Title") );

    QCOMPARE( mVideoView->mLastPlayPosition, 10);

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleAppBackground()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleAppBackground()
{
    setup();

    mVideoView->mActivated = true;

    connect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleAppBackground() ) );
    emit commandSignal();

    QVERIFY( ! mVideoView->mVideoMpxWrapper->iViewForeground );
    QVERIFY( ! mVideoView->mVideoMpxWrapper->iAppForeground );

    disconnect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleAppBackground() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
//   TestVideoPlaybackView::testHandleAppForeground()
// -------------------------------------------------------------------------------------------------
//
void TestVideoPlaybackView::testHandleAppForeground()
{
    setup();

    connect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleAppForeground() ) );
    emit commandSignal();

    QVERIFY( mVideoView->mVideoMpxWrapper->iViewForeground );
    QVERIFY( mVideoView->mVideoMpxWrapper->iAppForeground );

    disconnect( this, SIGNAL( commandSignal() ), mVideoView, SLOT( handleAppForeground() ) );

    cleanup();
}

// End of file
