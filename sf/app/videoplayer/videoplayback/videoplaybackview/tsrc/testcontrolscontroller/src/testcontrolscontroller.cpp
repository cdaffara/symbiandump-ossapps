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
* Description:   tester for methods in Video Playback Controls Controller
*
*/

// Version : %version:  18 %

#include <e32err.h>
#include <w32std.h>
#include <qobject>

#include <hbapplication.h>
#include <hbinstance.h>
#include <hblabel.h>
#include <MMFScalingCustomCommandConstants.h>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrol.hrh"
#include "mpxcommonvideoplaybackview.hrh"
#include "testcontrolscontroller.h"
#include "videobaseplaybackview.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxvideoviewwrapper.h"
#include "videoplaybackfullscreencontrol.h"
#include "videoplaybackcontrolconfiguration.h"
#include "thumbnailmanager_qt.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackvolumecontrol.h"
#include "videoservices.h"
#include "xqserviceutilxtra.h"

#define private public
#include "videoplaybackcontrolscontroller.h"
#undef private

const TInt KControlListsUpdated = 12; // additional extension of TMPXPlaybackState

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
//
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;
    TestControlsController tv;

    char *pass[3];
    pass[0] = argv[0];
    pass[1] = "-o";
    pass[2] = "c:\\data\\testcontrolscontroller.txt";

    int res = QTest::qExec(&tv, 3, pass);

    return res;
}

// ---------------------------------------------------------------------------
// init
// ---------------------------------------------------------------------------
//
void TestControlsController::init()
{
    MPX_ENTER_EXIT(_L("TestControlsController::init()"));

    mBaseVideoView = new VideoBasePlaybackView();

    if ( ! mFileDetails )
    {
        mFileDetails   = new VideoPlaybackViewFileDetails();
    }

    mViewWrapper   = CMPXVideoViewWrapper::NewL( mBaseVideoView );
    mController    = new VideoPlaybackControlsController( mBaseVideoView,
                                                          mViewWrapper,
                                                          mFileDetails );
}

// ---------------------------------------------------------------------------
// cleanup
// ---------------------------------------------------------------------------
//
void TestControlsController::cleanup()
{
    MPX_ENTER_EXIT(_L("TestControlsController::cleanup()"));

    delete mController;
    mController = NULL;

    delete mFileDetails;
    mFileDetails = NULL;

    delete mViewWrapper;
    mViewWrapper = NULL;

    delete mBaseVideoView;
    mBaseVideoView = NULL;
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testAddFileDetails
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testAddFileDetails()
{
    MPX_DEBUG(_L("TestControlsController::testAddFileDetails()") );

    //
    // local real media with no video
    //
    mFileDetails = new VideoPlaybackViewFileDetails();
    mFileDetails->mClipName = QString( "testClip.rm" );
    mFileDetails->mMimeType = QString( "video/x-pn-realvideo" );

    init();

    mController->addFileDetails( mFileDetails );

    QVERIFY( mController->mFileDetails->mRNFormat == true );
    QVERIFY( mController->mViewMode == EAudioOnlyView );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true );
    QVERIFY( mController->mControlsConfig->mState == EControlCmdTvOutConnected );

    cleanup();

    //
    // local 3GPP media with video-only
    //
    mFileDetails = new VideoPlaybackViewFileDetails();

    mFileDetails->mClipName = QString("testClip.3gp");
    mFileDetails->mMimeType = QString("video/3gpp");
    mFileDetails->mVideoEnabled = true;
    mFileDetails->mTvOutConnected = true;

    init();

    mController->mViewMode = EFullScreenView;
    QFileInfo videoOnlyFile( mFileDetails->mClipName );

    mController->addFileDetails( mFileDetails );

    QVERIFY( mController->mFileDetails->mRNFormat == false );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true );

    //
    // validate 'TvOutConnected'
    //
    verifyHandleEventTvOutResult( true, true );

    //
    // verify 'title' (via mClipName) is set properly
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {
        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "title" ) )
        {
            HbLabel *titleLabel = qobject_cast<HbLabel*>( mController->mLoader->mWidgets[i] );
            QVERIFY( titleLabel->plainText() == videoOnlyFile.baseName() );
            break;
        }
    }
    cleanup();

    //
    // non-local audio-video media, TvOut is not connected
    //
    mFileDetails = new VideoPlaybackViewFileDetails();

    mFileDetails->mClipName = QString("rtsp:://www.youtube.com/testClip.rm");
    mFileDetails->mPlaybackMode = EMPXVideoStreaming;
    mFileDetails->mTvOutConnected = false;
    mFileDetails->mAudioEnabled = true;
    mFileDetails->mTitle = QString("Clip Title");

    init();

    mController->addFileDetails( mFileDetails );

    QVERIFY( mController->mFileDetails->mRNFormat == true );
    QVERIFY( mController->mControlsConfig->mUpdateControlsWithFileDetails == true );

    //
    // verify 'title' (via mTitle) is set properly
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {
        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "title" ) )
        {
            HbLabel *titleLabel = qobject_cast<HbLabel*>( mController->mLoader->mWidgets[i] );
            QVERIFY( titleLabel->plainText() == mFileDetails->mTitle );
            break;
        }
    }

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetPosition
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetPosition()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetPosition()"));

    init();
    int value = 20000;

    mController->handleEvent( EControlCmdSetPosition, value );

    QVERIFY( mController->mControls[0]->mPosition == ( value / KPbMilliMultiplier ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetDuration
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetDuration()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetDuration()"));

    init();
    int value = 30000;

    mController->handleEvent( EControlCmdSetDuration, value );

    QVERIFY( mController->mControls[0]->mDuration == ( value / KPbMilliMultiplier ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventStateChanged
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventStateChanged()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventStateChanged()"));

    init();
    TVideoPlaybackControlCommandIds event = EControlCmdStateChanged;
    mController->mOrientation = Qt::Horizontal;

    //
    // state change (EPbStateInitialising)
    //
    int value = EPbStateInitialising;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateBuffering)
    //
    value = EPbStateBuffering;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStatePlaying)
    //
    value = EPbStatePlaying;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStatePaused)
    //
    value = EPbStatePaused;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateNotInitialised)
    //
    value = EPbStateNotInitialised;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateStopped)
    //
    value = EPbStateStopped;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateSeekingForward)
    //
    value = EPbStateSeekingForward;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateSeekingBackward)
    //
    value = EPbStateSeekingBackward;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateShuttingDown)
    //
    value = EPbStateShuttingDown;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStateDownloading)
    //
    value = EPbStateDownloading;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    //
    // state change (EPbStatePluginSeeking)
    //
    value = EPbStatePluginSeeking;
    mController->handleEvent( event, value );
    verifyHandleEventStateChangedResult( value );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::verifyHandleEventStateChangedResult
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::verifyHandleEventStateChangedResult( int value )
{
    MPX_DEBUG(_L("TestControlsController::verifyHandleEventStateChangedResult()"));

    mController->mOrientation = Qt::Horizontal;

    switch( value )
    {
        case EPbStatePlaying:
        {
            bool visible = true;

            if ( mController->mViewMode == EFullScreenView )
            {
                visible = false;
            }

            for ( int i = 0 ; i < mController->mControls.count() ; i++ )
            {
                QVERIFY( mController->mControls[i]->mVisible == visible );
                QVERIFY( mController->mControls[i]->mState == value );
            }

            break;
        }
        case EPbStateBuffering:
        {
            if ( mFileDetails->mPlaybackMode == EMPXVideoLocal )
            {
                for ( int i = 0 ; i < mController->mControls.count() ; i++ )
                {
                    QVERIFY( mController->mControls[i]->mState == value );
                }
            }
            else
            {
                for ( int i = 0 ; i < mController->mControls.count() ; i++ )
                {
                    QVERIFY( mController->mControls[i]->mState == value );
                    QVERIFY( mController->mControls[i]->mVisible == true );
                    QVERIFY( mController->mControls[i]->mVisibilityState == value );
                }
            }

            break;
        }
        case EPbStatePaused:
        case EPbStateInitialising:
        case EPbStateNotInitialised:
        case EPbStatePluginSeeking:
        {
            for ( int i = 0 ; i < mController->mControls.count() ; i++ )
            {
                QVERIFY( mController->mControls[i]->mState == value );
                QVERIFY( mController->mControls[i]->mVisibilityState == value );
            }
            break;
        }
        default:
        {
            QVERIFY( mController->mState == value );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetVolume
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetVolume()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetVolume()"));

    init();
    int value = 40;

    mController->handleEvent( EControlCmdSetVolume, value );
    QVERIFY( mController->mVolumeControl->mVolume == value );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetAspectRatio
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetAspectRatio()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetAspectRatio()"));

    init();
    int numScalingType = 5;

    for ( int i = 0; i < numScalingType; i++ )
    {
        mController->handleEvent( EControlCmdSetAspectRatio, i );

        QVERIFY( mController->mControls[0]->mAspectRatio == i );
    }

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetDownloadSize
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetDownloadSize()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetDownloadSize()") );

    init();
    int value = 50;

    mController->handleEvent( EControlCmdSetDownloadSize, value );

    QVERIFY( mController->mControls[0]->mDownloadSize == value );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventDownloadUpdated
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventDownloadUpdated()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventDownloadUpdated()") );

    init();
    int value = 55;

    mController->handleEvent( EControlCmdDownloadUpdated, value );

    QVERIFY( mController->mControls[0]->mDownloadPosition == value );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventDownloadComplete
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventDownloadComplete()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventDownloadComplete()") );

    init();
    int value = 60;

    mController->handleEvent( EControlCmdDownloadComplete, value );

    QVERIFY( mController->mControls[0]->mDownloadPosition == value );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventSetDownloadPaused
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventSetDownloadPaused()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventSetDownloadPaused()") );

    init();
    int value = 0;

    mController->handleEvent( EControlCmdSetDownloadPaused, value );

    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventClearDownloadPaused
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventClearDownloadPaused()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventClearDownloadPaused()") );

    init();
    int value = 0;

    mController->handleEvent( EControlCmdClearDownloadPaused, value );

    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventTvOutConnected
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventTvOutConnected()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventTvOutConnected()") );

    init();

    //
    // TV-Out Connected, EMPXVideoLocal
    //
    mController->mThumbNailState = EThumbNailEmpty;
    mController->mViewMode = EFullScreenView;

    mController->handleEvent( EControlCmdTvOutConnected );

    verifyHandleEventTvOutResult( true, true );
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    //
    // TV-Out Connected, non-EMPXVideoLocal
    //
    mController->mFileDetails->mPlaybackMode = EMPXVideoStreaming;
    mController->mThumbNailState = EThumbNailEmpty;
    mController->mViewMode = EDetailsView;

    mController->handleEvent( EControlCmdTvOutConnected, false );

    verifyHandleEventTvOutResult( true, false );
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::verifyHandleEventTvOutResult
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::verifyHandleEventTvOutResult( bool tvOutConnected,
        bool value )
{
    MPX_DEBUG(_L("TestControlsController::verifyHandleEventTvOutResult(%d,%d)"),
            tvOutConnected, value );

    QVERIFY( mController->mFileDetails->mTvOutConnected == tvOutConnected );

    //
    // validate setDefaultBitmap()
    //
    for ( int i=0 ; i < mController->mLoader->mWidgets.count() ; i++ )
    {
        bool val = mController->mLoader->isVisible(i);

        if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "tvOutBitmap" ) )
        {
            QVERIFY( tvOutConnected? val == true : val == false );
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "realAudioOnlyBitmap" )  )
        {
            QVERIFY( mFileDetails->mRNFormat? val == true : val == false );
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "partialAudioOnlyBitmap" )  )
        {
            QVERIFY( mFileDetails->mPartialPlayback? val == true : val == false );
        }
        else if( mController->mLoader->mWidgets[i]->objectName() ==  QString( "audioOnlyBitmap" )  )
        {
            QVERIFY( tvOutConnected? val == false : val == true );
        }
    }

    //
    // validate generateThumbNail()
    //
    if ( tvOutConnected )
    {
        QVERIFY( ( mController->mFileDetails->mPlaybackMode == EMPXVideoLocal )?
                   mController->mThumbNailState == EThumbNailRequsted :
                   mController->mThumbNailState == EThumbNailNotAvailable );
        QVERIFY( mController->mViewMode == EAudioOnlyView );
        QVERIFY( mController->mControlsConfig->mState == EControlCmdTvOutConnected );
        QVERIFY( mController->mThumbnailManager->mThumbSize == ThumbnailManager::ThumbnailLarge );
        QVERIFY( mController->mThumbnailManager->mQuality == ThumbnailManager::OptimizeForPerformance );
        QVERIFY( mController->mViewTransitionIsGoingOn == false );
    }
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventTvOutDisconnected
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventTvOutDisconnected()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventTvOutDisconnected()") );

    init();

    //
    // TV-Out Disconnected, move to full screen view
    //
    mController->mViewMode = EAudioOnlyView;
    mFileDetails->mVideoEnabled = true;
    mController->handleEvent( EControlCmdTvOutDisconnected );

    verifyHandleEventTvOutResult( false, true );
    QVERIFY( mController->mViewMode == EFullScreenView );

    //
    // TV-Out Disconnected, move to audio only view
    //
    mController->mViewMode = EAudioOnlyView;
    mFileDetails->mVideoEnabled = false;
    mController->handleEvent( EControlCmdTvOutDisconnected );

    verifyHandleEventTvOutResult( false, true );
    QVERIFY( mController->mViewMode == EAudioOnlyView );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventHandleErrors
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventHandleErrors()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventHandleErrors()") );

    init();

    //
    // handle error
    //
    mController->handleEvent( EControlCmdHandleErrors, 0 );

    QVERIFY( mController->mThumbNailState == EThumbNailEmpty );
    QVERIFY( mController->mState == EPbStateNotInitialised );
    QVERIFY( mController->mViewMode == EFullScreenView );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventShowVolumeControls
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventShowVolumeControls()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventShowVolumeControls()") );

    init();

    mController->handleEvent( EControlCmdShowVolumeControls, 0 );

    QVERIFY( mController->mVolumeControl->mVisible == true );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleEventBackground
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleEventBackground()
{
    MPX_DEBUG(_L("TestControlsController::testHandleEventBackground()") );

    init();

    mController->handleEvent( EControlCmdHandleBackgroundEvent, 0 );

    QVERIFY( mController->mControls[0]->mControlReset );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleCommand
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleCommand()
{
    MPX_DEBUG(_L("TestControlsController::testHandleCommand()") );

    init();

    mController->handleCommand( EMPXPbvCmdSetPosition, 0 );
    QVERIFY( mViewWrapper->mProperty == EPbPropertyPosition );

    mController->handleCommand( EMPXPbvCmdSetVolume, 0 );
    QVERIFY( mViewWrapper->mProperty == EPbPropertyVolume );

    mController->handleCommand( EMPXPbvCmdPlay, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPlay );

    mController->handleCommand( EMPXPbvCmdPause, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPause );

    mController->handleCommand( EMPXPbvCmdPlayPause, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPlayPause );

    mController->handleCommand( EMPXPbvCmdStop, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdStop );

    mController->handleCommand( EMPXPbvCmdClose, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdClose );

    mController->handleCommand( EMPXPbvCmdMute, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdMute );

    mController->handleCommand( EMPXPbvCmdUnMute, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdUnMute );

    mController->handleCommand( EMPXPbvCmdChangeAspectRatio, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdChangeAspectRatio );

    mController->handleCommand( EMPXPbvCmdNaturalAspectRatio, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNaturalAspectRatio );

    mController->handleCommand( EMPXPbvCmdZoomAspectRatio, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdZoomAspectRatio );

    mController->handleCommand( EMPXPbvCmdStretchAspectRatio, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdStretchAspectRatio );

    mController->handleCommand( EMPXPbvCmdSeekForward, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSeekForward );

    mController->handleCommand( EMPXPbvCmdSeekBackward, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSeekBackward );

    mController->handleCommand( EMPXPbvCmdEndSeek, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdEndSeek );

    mController->handleCommand( EMPXPbvCmdNextListItem, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNextListItem );

    mController->handleCommand( EMPXPbvCmdPreviousListItem, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPreviousListItem );

    mController->handleCommand( EMPXPbvCmdDecreaseVolume, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdDecreaseVolume );

    mController->handleCommand( EMPXPbvCmdIncreaseVolume, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdIncreaseVolume );

    mController->handleCommand( EMPXPbvCmdPosition, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPosition );

    mController->handleCommand( EMPXPbvCmdSave, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdSave );

    mController->handleCommand( EMPXPbvCmdResetControls, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdResetControls );

    mController->handleCommand( EMPXPbvCmdShortPressForward, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShortPressForward );

    mController->handleCommand( EMPXPbvCmdShortPressBackward, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShortPressBackward );

    mController->handleCommand( EMPXPbvCmdShowFileDetails, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdShowFileDetails );

    mController->handleCommand( EMPXPbvCmdEndOfClip, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdEndOfClip );

    mController->handleCommand( EMPXPbvCmdCustomPause, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdCustomPause );

    mController->handleCommand( EMPXPbvCmdCustomPlay, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdCustomPlay );

    mController->handleCommand( EMPXPbvCmdExit, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdExit );

    mController->handleCommand( EMPXPbvLaunchDRMDetails, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvLaunchDRMDetails );

    mController->handleCommand( EMPXPbvSurfaceCreated, 0 );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvSurfaceCreated );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testHandleTappedOnScreen
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testHandleTappedOnScreen()
{
    MPX_DEBUG(_L("TestControlsController::testHandleTappedOnScreen()") );

    init();
    mController->mOrientation = Qt::Horizontal;
    connect( this, SIGNAL( commandSignal() ), mController, SLOT( handleTappedOnScreen() ) );

    //
    // 1. Test full screen view
    //
    mController->mViewMode = EFullScreenView;

    //
    // (1a) playing state
    //
    mController->mState = EPbStatePlaying;
    emit commandSignal();

    QVERIFY( mController->mControlsTimer->isActive() == true );
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );
    }

    //
    // (1b) pause state
    //
    mController->mState = EPbStatePaused;
    emit commandSignal();

    QVERIFY( mController->mControlsTimer->isActive() == false );
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );
    }


    //
    // 2. Test details screen view
    //
    mController->mViewMode = EDetailsView;

    //
    // (2a) non-pausable stream, currently in 'playing' state
    //
    mController->mFileDetails->mPausableStream = false;
    mController->mViewWrapper->HandleCommandL( EMPXPbvCmdSetVolume ); // set a random cmd
    mController->mState = EPbStatePlaying;

    emit commandSignal();
    QVERIFY( mController->mViewWrapper->mCommandId == EMPXPbvCmdSetVolume );

    //
    // (2b) pausable stream, currently in 'playing' state
    //
    mController->mFileDetails->mPausableStream = true;
    mController->mViewWrapper->HandleCommandL( EMPXPbvCmdChangeAspectRatio ); // set a random cmd
    mController->mState = EPbStatePlaying;

    emit commandSignal();
    QVERIFY( mController->mViewWrapper->mCommandId == EMPXPbvCmdPlayPause );

    //
    // (2c) non-pausable stream, currently in 'paused' state
    //
    mController->mFileDetails->mPausableStream = false;
    mController->mViewWrapper->HandleCommandL( EMPXPbvCmdEndSeek ); // set a random cmd
    mController->mState = EPbStatePaused;

    emit commandSignal();
    QVERIFY( mController->mViewWrapper->mCommandId == EMPXPbvCmdPlayPause );

    //
    // (2d) pausable stream, currently in 'paused' state
    //
    mController->mFileDetails->mPausableStream = true;
    mController->mViewWrapper->HandleCommandL( EMPXPbvCmdSave ); // set a random cmd
    mController->mState = EPbStatePaused;

    emit commandSignal();
    QVERIFY( mController->mViewWrapper->mCommandId == EMPXPbvCmdPlayPause );

    //
    // disconnect signal and clean up
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( handleTappedOnScreen() ) );
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testUpdateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testUpdateVideoRectDone()
{
    MPX_DEBUG(_L("TestControlsController::testUpdateVideoRectDone()") );

    init();

    mController->updateVideoRectDone();

    //
    // default is full-screen
    //
    QVERIFY( mController->mControlsConfig->mState == EControlCmdFullScreenViewOpened );

    //
    // validate Details view
    //
    mController->mViewMode = EDetailsView;

    mController->updateVideoRectDone();

    QVERIFY( mController->mControlsConfig->mState == EControlCmdDetailsViewOpened );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testIsAttachOperation
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testIsAttachOperation()
{
    MPX_DEBUG(_L("TestControlsController::testIsAttachOperation()") );

    //
    // get access to XQServiceUtilXtra stub
    //
    XQServiceUtilXtra* extraUtil = XQServiceUtilXtra::instance();

    //
    // set service enable
    //
    extraUtil->setCurrentService( true );

    //
    // initialize controlscontroller
    //
    init();

    //
    // verify 'mIsAttachOperation' flag is enabled
    //
    QVERIFY( mController->mIsAttachOperation == true );

    //
    // clean up
    //
    cleanup();

    //
    // set service disable
    //
    extraUtil->setCurrentService( false );

    //
    // initialize controlscontroller
    //
    init();

    //
    // verify 'mIsAttachOperation' flag is disabled
    //
    QVERIFY( mController->mIsAttachOperation == false );

    //
    // clean up
    //
    cleanup();

    //
    // dereference extraUtil count
    //
    extraUtil->decreaseReferenceCount();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_skipToNextVideoItem
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_skipToNextVideoItem()
{
    MPX_DEBUG(_L("TestControlsController::testslot_skipToNextVideoItem()") );

    init();

    connect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToNextVideoItem() ) );

    //
    // If we are not in full screen view, ignore
    //
    mViewWrapper->mCommandId = EMPXPbvCmdPosition;
    mController->mViewMode = EDetailsView;

    emit commandSignal();

    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPosition );

    //
    // If we are in full screen view, issue EMPXPbvCmdNextListItem;
    //
    mController->mViewMode = EFullScreenView;

    emit commandSignal();

    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdNextListItem );

    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToNextVideoItem() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_skipToPreviousVideoItem
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_skipToPreviousVideoItem()
{
    MPX_DEBUG(_L("TestControlsController::testslot_skipToPreviousVideoItem()") );

    init();

    connect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToPreviousVideoItem() ) );

    //
    // If we are not in full screen view, ignore
    //
    mViewWrapper->mCommandId = EMPXPbvCmdPosition;
    mController->mViewMode = EDetailsView;

    emit commandSignal();

    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPosition );

    //
    // If we are in full screen view, issue EMPXPbvCmdPreviousListItem
    //
    mController->mViewMode = EFullScreenView;

    emit commandSignal();

    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdPreviousListItem );

    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( skipToPreviousVideoItem() ) );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_attachVideo
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_attachVideo()
{
    MPX_DEBUG(_L("TestControlsController::testslot_attachVideo()") );

    //
    // get access to XQServiceUtilXtra stub to enable XQServiceUtil::isService() method
    //
    XQServiceUtilXtra* extraUtil = XQServiceUtilXtra::instance();

    //
    // set service enable
    //
    extraUtil->setCurrentService( true );

    //
    // initialize controlscontroller
    //
    init();

    //
    // verify view closePlaybackView() slot is not yet called
    //
    QVERIFY( mController->mView->mViewClosed == false );

    //
    // connect signal with controller attachVideo() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );

    //
    // emit signal, this will in turns invoke videoservices itemSelected() slot
    //
    emit commandSignal();

    //
    // verify view closePlaybackView() slot is called
    //
    QVERIFY( mController->mView->mViewClosed == true );

    //
    // verify videoservices itemSelected() slot is called
    //
    QVERIFY( mController->mVideoServices->mItemSelected == true );

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( attachVideo() ) );

    //
    // clean up
    //
    cleanup();

    //
    // dereference extraUtil count
    //
    extraUtil->decreaseReferenceCount();

}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_sendVideo
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_sendVideo()
{
    MPX_DEBUG(_L("TestControlsController::testslot_sendVideo()") );

    //
    // initialize controlscontroller
    //
    init();

    //
    // connect signal with controller sendVideo() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );

    //
    // emit signal, this will in turns invoke mController sendVideo() slot
    //
    emit commandSignal();

    //
    // emit again.
    //
    emit commandSignal();

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ), mController, SLOT( sendVideo() ) );

    //
    // clean up
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_handleOrientationChanged
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_handleOrientationChanged()
{
    MPX_DEBUG(_L("TestControlsController::testslot_handleOrientationChanged()") );

    //
    // initialize controlscontroller
    //
    init();

    //
    // connect signal with controller handleOrientationChanged() slot
    //
    bool res = connect( this, SIGNAL( commandSignal( Qt::Orientation ) ),
                        mController, SLOT( handleOrientationChanged( Qt::Orientation ) ) );

    //
    // emit signal, this will in turns invoke mController handleOrientationChanged() slot
    //
    mController->mOrientation = Qt::Vertical;
    mController->mState = EPbStatePlaying;

    emit commandSignal( Qt::Horizontal );

    QVERIFY( mController->mOrientation == Qt::Horizontal );

    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        QVERIFY( mController->mControls[i]->mVisibilityState == mController->mState );
    }

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal( Qt::Orientation ) ),
                mController, SLOT( handleOrientationChanged( Qt::Orientation ) ) );

    //
    // clean up
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_handleRNLogoVisibleChanged
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_handleRNLogoVisibleChanged()
{
    MPX_DEBUG(_L("TestControlsController::testslot_handleRNLogoVisibleChanged()") );

    //
    // initialize controlscontroller
    //
    init();

    //
    // connect signal with controller handleRNLogoVisibleChanged() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ),
                        mController, SLOT( handleRNLogoVisibleChanged() ) );

    //
    // emit signal, this will in turns invoke mController handleRNLogoVisibleChanged() slot
    //
    emit commandSignal();

    QVERIFY( mController->mRNLogoTimer->isActive() );
    QVERIFY( mController->mRNLogoTimer->interval() == KRNLogoTimeOut );

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ),
                mController, SLOT( handleRNLogoVisibleChanged() ) );

    //
    // clean up
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testslot_handleRNLogoTimeout
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testslot_handleRNLogoTimeout()
{
    MPX_DEBUG(_L("TestControlsController::testslot_handleRNLogoTimeout()") );

    //
    // initialize controlscontroller
    //
    init();

    if ( ! mController->mRNLogoTimer )
    {
        mController->mRNLogoTimer = new QTimer( mController );
    }

    //
    // connect signal with controller handleRNLogoTimeout() slot
    //
    bool res = connect( this, SIGNAL( commandSignal() ),
                        mController, SLOT( handleRNLogoTimeout() ) );

    //
    // emit signal, this will in turns invoke mController handleRNLogoTimeout() slot
    //
    emit commandSignal();

    QVERIFY( ! mController->mRNLogoTimer->isActive() );
    QVERIFY( mController->mControlsConfig->mState == KControlListsUpdated );
    QVERIFY( mViewWrapper->mCommandId == EMPXPbvCmdRealOneBitmapTimeout );

    //
    // disconnect signal
    //
    disconnect( this, SIGNAL( commandSignal() ),
                mController, SLOT( handleRNLogoTimeout() ) );

    //
    // clean up
    //
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testIsRNLogoBitmapVisible
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testIsRNLogoBitmapVisible()
{
    MPX_DEBUG(_L("TestControlsController::testIsRNLogoBitmapVisible()"));

    init();

    QVERIFY( mController->isRNLogoBitmapInControlList() );

    //
    // Remove RN logo from the list
    //
    for ( int i = 0 ; i < mController->mControls.count() ; i++ )
    {
        if( mController->mControls[i]->mControlIndex == ERealLogoBitmap )
        {
            mController->mControls.removeAt( i );
        }
    }

    QVERIFY( ! mController->isRNLogoBitmapInControlList() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testIsFileDetailsAdded
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testIsFileDetailsAdded()
{
    MPX_DEBUG(_L("TestControlsController::testIsFileDetailsAdded()"));

    init();

    QVERIFY( ! mController->isFileDetailsAdded() );

    mFileDetails = new VideoPlaybackViewFileDetails();
    mFileDetails->mClipName = QString( "testClip.rm" );
    mFileDetails->mMimeType = QString( "video/x-pn-realvideo" );

    mController->addFileDetails( mFileDetails );

    QVERIFY( mController->isFileDetailsAdded() );

    cleanup();
}

// -------------------------------------------------------------------------------------------------
// TestControlsController::testVolumeSteps
// -------------------------------------------------------------------------------------------------
//
void TestControlsController::testVolumeSteps()
{
    init();

    int volume = 5;
    mViewWrapper->mVolumeSteps = volume;

    QVERIFY( mViewWrapper->VolumeSteps() == volume );

    cleanup();
}

// End of file
