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
* Description:   Implementation of Video base playback view
*
*/

// Version : %version: da1mmcf#49.1.3 %



//  Include Files



#include <QTimer>
#include <QGraphicsSceneEvent>

#include <hbmenu.h>
#include <hbaction.h>
#include <hbinstance.h>
#include <hbnotificationdialog.h>
#include <hbmessagebox.h>
#include <hblabel.h>
#include <hbtapgesture.h>
#include <hbpangesture.h>

#include <textresolver.h>
#include <mmf/common/mmferrors.h>
#include <DRMHelper.h>

#include <mpxvideoplaybackdefs.h>
#include <mpxplaybackcommanddefs.h>
#include <mpxhbvideocommondefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "videobaseplaybackview.h"
#include "videoactivitystate.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"

const int KPanGestureThreshold = 200;

//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
    : mVideoMpxWrapper( NULL )
    , mTimerForClosingView( NULL )
    , mActivated( false )
    , mStayPaused ( false )
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::VideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::initializeVideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::initializeVideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::initializeVideoPlaybackView()"));

    //
    // Need to set to control full screen including status pane area
    //
    setContentFullScreen();

    mTimerForClosingView = new QTimer();
    mTimerForClosingView->setSingleShot( true );
    mTimerForClosingView->setInterval( 10 );
    connect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );

    //
    // Read activity data ...
    // If last playback was forced to terminate due to low memory (GOOM, etc.)
    // then the clip needs to be restored to the last played position
    // and the state needs to be paused, since forced termination can only occur for
    // background apps - so if this happened Video Player must have been in background
    // which implies paused state
    //
    int lastViewType = VideoActivityState::instance().getActivityData(KEY_VIEWPLUGIN_TYPE).toInt();
    if ( lastViewType == MpxHbVideoCommon::PlaybackView )
    {
        QVariant data = VideoActivityState::instance().getActivityData(KEY_LAST_PLAY_POSITION_ID);
        mLastPlayPosition = data.toInt();

        mStayPaused = true;
    }

    MPX_TRAPD( err, mVideoMpxWrapper = CMPXVideoViewWrapper::NewL( this ) );
}

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::~VideoBasePlaybackView()"));

    if ( mTimerForClosingView )
    {
        disconnect( mTimerForClosingView, SIGNAL( timeout() ), this, SIGNAL( activatePreviousView() ) );

        if ( mTimerForClosingView->isActive() )
        {
            mTimerForClosingView->stop();
        }

        delete mTimerForClosingView;
        mTimerForClosingView = NULL;
    }

    if ( mVideoMpxWrapper )
    {
        delete mVideoMpxWrapper;
        mVideoMpxWrapper = NULL;
    }

    setParentItem( 0 );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleActivateView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleActivateView()"));

    MPX_TRAPD( err, mVideoMpxWrapper->ActivateL() );

    connect( hbInstance->allMainWindows()[0], SIGNAL( obscured() ), this, SLOT( handleAppBackground() ) );
    connect( hbInstance->allMainWindows()[0], SIGNAL( revealed() ), this, SLOT( handleAppForeground() ) );

    QCoreApplication::instance()->installEventFilter( this );

    //
    //  Request the needed Media from the Playback Plugin
    //
    MPX_TRAP( err, mVideoMpxWrapper->RequestMediaL() );

    menu()->close();

    //
    // Landscape orientation
    // Workaround: Disable orientation transition effect
	// since orbit couldn't emit the orientationChanged signal with transition effect
	//
    hbInstance->allMainWindows()[0]->setOrientation( Qt::Horizontal, false );

    mActivated = true;
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleDeactivateView()"));

    saveActivityData();

    mActivated = false;

    QCoreApplication::instance()->removeEventFilter( this );

    disconnect( hbInstance->allMainWindows()[0], SIGNAL( obscured() ), this, SLOT( handleAppBackground() ) );
    disconnect( hbInstance->allMainWindows()[0], SIGNAL( revealed() ), this, SLOT( handleAppForeground() ) );

    //
    //  Close the playback plugin to release all references to previous clip
    //
    MPX_TRAPD( err, mVideoMpxWrapper->HandleCommandL( EMPXPbvCmdClose ) );

    MPX_TRAP( err, mVideoMpxWrapper->Deactivate() );

    //
    // go back to device orientation
    //
    //hbInstance->allMainWindows()[0]->unsetOrientation();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::saveActivityData()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::saveActivityData()
{
    MPX_DEBUG( _L("VideoBasePlaybackView::saveActivityData()") );

    VideoActivityState& videoActivityState = VideoActivityState::instance();

    // save the activity data
    QVariant data = QString( mVideoMpxWrapper->iFileDetails->mClipName );
    videoActivityState.setActivityData( data, KEY_LAST_PLAYED_CLIP );

    data = int( mVideoMpxWrapper->iPlayPosition );
    videoActivityState.setActivityData( data, KEY_LAST_PLAY_POSITION_ID );

    data = bool( mVideoMpxWrapper->iFileDetails->mPlaybackMode == EMPXVideoLocal );
    videoActivityState.setActivityData( data, KEY_LAST_LOCAL_PLAYBACK );

    data = uint ( mVideoMpxWrapper->GetMediaId() );
    videoActivityState.setActivityData( data, KEY_LAST_PLAYED_MEDIA_ID );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePlaybackView()
{
    MPX_DEBUG(
      _L("VideoBasePlaybackView::handleClosePlaybackView()") );

    if ( mVideoMpxWrapper && mVideoMpxWrapper->IsMultiItemPlaylist() )
    {
        TRAP_IGNORE( mVideoMpxWrapper->HandleCommandL( EMPXPbvCmdNextListItem ) );
    }
    else
    {
        closePlaybackView();
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handlePluginError( int aError )
{
    MPX_DEBUG(
      _L("VideoBasePlaybackView::handlePluginError() aError = %d"), aError );

    switch ( aError )
    {
        case KErrCancel:
		{
            handleClosePlaybackView();
            break;
        }
        case KErrMMDRMNotAuthorized:
        case KErrCANoRights:
        case KErrCANoPermission:
        {
            showDialog( hbTrId( "txt_videos_info_license_has_expired_or_it_is_missi" ) );
            break;
        }
        case KMPXVideoCallOngoingError:
        {
            showDialog( hbTrId( "txt_videos_info_video_playback_is_not_allowed_duri" ), false );
            break;
        }
        default:
        {
            TRAP_IGNORE(

                //
                //  TextResolver instance for error resolving.
                //
                CTextResolver* textresolver = CTextResolver::NewLC();

                //
                //  Resolve the error text
                //
                TPtrC text;
                text.Set( textresolver->ResolveErrorString( aError ) );

                //
                // convert to QString
                //
                const QString qString( (QChar*)text.Ptr(), text.Length() );

                //
                // clean up textresolver
                //
                CleanupStack::PopAndDestroy( textresolver );

                //
                // display error and close playback view
                //
                showDialog( qString );
            );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::showDialog
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::showDialog( const QString& string, bool closeView, bool isError )
{
    MPX_DEBUG(_L("VideoBasePlaybackView::showDialog( %s, %d, %d )"), string.data(), closeView, isError );

    //
    // create pop-up dialog for error notes and notifications,
    // For error notes, HbMessageBox is used:
    //  - Note will be dismissed by using standard timeout or
    //  - If user taps anywhere on the screen.
    // For notifications, HbNotification dialog will be used.
    //

    HbDialog *dlg = 0;
    if(isError)
    {
        dlg = new HbMessageBox( string, HbMessageBox::MessageTypeWarning );
        // dialog will be closed by timeout, no buttons used
        qobject_cast<HbMessageBox*>( dlg )->setStandardButtons( HbMessageBox::NoButton );
    }
    else
    {
        dlg = new HbNotificationDialog();
        qobject_cast<HbNotificationDialog*>( dlg )->setTitle( string );
    }
    dlg->setAttribute( Qt::WA_DeleteOnClose );
    dlg->setDismissPolicy( HbPopup::TapAnywhere );
    dlg->setTimeout( HbPopup::StandardTimeout );
	if ( closeView )
	{
        //
        // connect aboutToClose() signal to handleClosePopupDialog() slot so that
        // when pop-up dialog is closed, playback view is closed also
        //
        connect( dlg, SIGNAL( aboutToClose() ), this, SLOT( handleClosePopupDialog() ) );
    }

    dlg->show();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::doClosePlayer
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::doClosePlayer()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::doClosePlayer()"));

    handleDeactivateView();

    qApp->quit();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::issuePlayCommand
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::issuePlayCommand()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::issuePlayCommand()"));

    int err = KErrNone;

    TRAP( err, mVideoMpxWrapper->CreateGeneralPlaybackCommandL( EPbCmdPlay ));

    MPX_DEBUG(_L("VideoBasePlaybackView::issuePlayCommand() error = %d"), err);
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::retrievePdlInformation
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::retrievePdlInformation()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::retrievePdlInformation()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleBufferingState
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleBufferingState()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::handleBufferingState()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::closePlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::closePlaybackView()"));

    mTimerForClosingView->start( 0 );
}

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::eventFilter
// -------------------------------------------------------------------------------------------------
//
bool VideoBasePlaybackView::eventFilter( QObject *object, QEvent *event )
{
    Q_UNUSED( object );

    switch ( event->type() )
    {
        case QEvent::ApplicationActivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::eventFilter foreground()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL(
                        true, ! hbInstance->allMainWindows()[0]->isObscured() ) );
            }
            break;
        }
        case QEvent::ApplicationDeactivate:
        {
            if ( mActivated )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::eventFilter background()") );
                TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL(
                        false, ! hbInstance->allMainWindows()[0]->isObscured() ) );
            }
            break;
        }
    }

    return QObject::eventFilter( object, event );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::getWindow()
// -------------------------------------------------------------------------------------------------
//
RWindow *VideoBasePlaybackView::getWindow()
{
    return static_cast<RWindow*>( hbInstance->allMainWindows()[0]->effectiveWinId()->DrawableWindow() );
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleClosePopupDialog()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleClosePopupDialog()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::handleClosePopupDialog()") );

    handleClosePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::gestureEvent()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::gestureEvent( QGestureEvent* event )
{
    if ( HbTapGesture * gesture = static_cast<HbTapGesture *>( event->gesture( Qt::TapGesture ) ) )
    {
        if ( gesture->state() == Qt::GestureFinished && mActivated )
        {
            MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() tapGesture finished") );

            emit tappedOnScreen();
        }
    }

    if ( HbPanGesture* gesture = static_cast<HbPanGesture*>( event->gesture( Qt::PanGesture ) ) )
    {
        if ( mActivated && gesture->state() == Qt::GestureFinished )
        {
            MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() pan gesture finished"));

            int nonZeroVelocity = (int)( gesture->sceneVelocity().x() );
            int offset = (int)( gesture->sceneOffset().x() );

            if ( offset > KPanGestureThreshold && nonZeroVelocity > 0 )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() pan gesture right") );

                emit pannedToRight();
            }
            else if ( offset < KPanGestureThreshold * -1.0f && nonZeroVelocity < 0 )
            {
                MPX_DEBUG(_L("VideoBasePlaybackView::gestureEvent() pan gesture left") );

                emit pannedToLeft();
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleAppBackground()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleAppBackground()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleAppBackground()") );

    if ( mActivated )
    {
        TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( false, false ) );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::handleAppForeground()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::handleAppForeground()
{
    MPX_ENTER_EXIT(_L("VideoBasePlaybackView::handleAppForeground()") );

    if ( mActivated )
    {
        TRAP_IGNORE( mVideoMpxWrapper->IssueVideoAppForegroundCmdL( true, true ) );
    }
}

// EOF
