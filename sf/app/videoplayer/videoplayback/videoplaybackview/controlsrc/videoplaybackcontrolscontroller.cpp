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
* Description:  Implementation of VideoPlaybackControlsController
*
*/

// Version : %version: da1mmcf#57 %



// INCLUDE FILES
#include <coecntrl.h>
#include <bautils.h>
#include <barsread.h>
#include <f32file.h>

#include <QTimer>
#include <thumbnailmanager_qt.h>

#include <hblabel.h>
#include <hbtransparentwindow.h>
#include <hbiconanimationmanager.h>
#include <shareui.h>
#include <hbinstance.h>
#include <hbtapgesture.h>
#include <hbpangesture.h>

#include "videoservices.h"
#include "videobaseplaybackview.h"
#include "videoplaybackcontrolbar.h"
#include "videoplaybackcontrolpolicy.h"
#include "videoplaybackvolumecontrol.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackstatuspanecontrol.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackfullscreencontrol.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackcontrolconfiguration.h"
#include "videoplaybackdetailsplaybackwindow.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController(
        VideoBasePlaybackView *view,
        CMPXVideoViewWrapper *viewWrapper,
        VideoPlaybackViewFileDetails *details )
    : mView( view )
    , mViewWrapper( viewWrapper )
    , mFileDetails( details )
    , mControlsPolicy( NULL )
    , mControlsConfig( NULL )
    , mControlsTimer( NULL )
    , mRNLogoTimer( NULL )
    , mLoader( NULL )
    , mVolumeControl( NULL )
    , mThumbnailManager( NULL )
    , mVideoServices( 0 )
    , mViewTransitionIsGoingOn( false )
    , mIsAttachOperation( false )
    , mFileDetailsAdded( false )
    , mShowControlsWhenInHorizontal( false )
    , mThumbNailState( EThumbNailEmpty )
    , mState( EPbStateNotInitialised )
    , mViewMode( EFullScreenView )
    , mShareUi( NULL )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::VideoPlaybackControlsController()"));

    initializeController();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::initializeController()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::initializeController()"));

    mOrientation = hbInstance->allMainWindows()[0]->orientation();
    bool ok = connect( hbInstance->allMainWindows()[0], SIGNAL( orientationChanged( Qt::Orientation ) ),
                       this, SLOT( handleOrientationChanged( Qt::Orientation ) ) );

    MPX_DEBUG(
        _L("VideoPlaybackControlsController::initializeController() orientation = %d, ok =%d"),
        mOrientation, ok );

    setParent( mView );

    mView->hideItems( Hb::AllItems );

    //
    // Create layout loader
    //
    ok = false;
    mLoader = new VideoPlaybackDocumentLoader( this );
    mLoader->load( KPLAYBACKVIEW_DOCML, &ok );

    if ( ok )
    {
        QGraphicsWidget *widget = mLoader->findWidget( QString( "content" ) );
        mView->setWidget( widget );
    }
    else
    {
        MPX_DEBUG(
                _L("VideoPlaybackControlsController::initializeController()- can't find xml"));

        //
        // Can't find xml for layout. Delete mLoader
        //
        delete mLoader;
        mLoader = NULL;
    }

    mFileDetails->mRNFormat = realFormat( mFileDetails->mClipName );

    //
    // Controls dismissing timer
    //
    mControlsTimer = new QTimer( this );
    mControlsTimer->setInterval( KControlsTimeOut );
    mControlsTimer->setSingleShot( false );
    connect( mControlsTimer, SIGNAL( timeout() ), this, SLOT( hideAllControls() ) );

    mControlsPolicy = new VideoPlaybackControlPolicy();

    mControlsConfig = new VideoPlaybackControlConfiguration( this );
    connect( mControlsConfig, SIGNAL( controlListUpdated() ), this, SLOT( controlsListUpdated() ) );
    mControlsConfig->createControlList();

    //
    // grab tap gesture
    //
    mView->grabGesture( Qt::TapGesture );
    connect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );

    //
    // if videoplayback is in service mode, create a videoservices instance
    //
    if ( isService() && ! mVideoServices )
    {
        //
        // obtain VideoServices instance
        //
        mVideoServices = VideoServices::instance();

        //
        // allow 'attach' operation only for non-streaming media clips
        //
        if ( mVideoServices && mFileDetails->mPlaybackMode == EMPXVideoLocal )
        {
            //
            // determine if this is 'attach' operation
            //
            mIsAttachOperation = ( mVideoServices->currentService() == VideoServices::EUriFetcher );

            if ( mIsAttachOperation )
            {
                //
                // connect signal filePath() to videoservices slot itemSelected()
                //
                connect( this, SIGNAL( attachVideoPath( const QString& ) ),
                         mVideoServices, SLOT( itemSelected( const QString& ) ) );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::~VideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::~VideoPlaybackControlsController()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::~VideoPlaybackControlsController()"));

    mView->ungrabGesture( Qt::TapGesture );
    disconnect( mView, SIGNAL( tappedOnScreen() ), this, SLOT( handleTappedOnScreen() ) );

    if ( mFileDetails->mMultiItemPlaylist && mFileDetails->mSeekable )
    {
        mView->ungrabGesture( Qt::PanGesture );

        disconnect( mView, SIGNAL( pannedToRight() ), this, SLOT( skipToNextVideoItem() ) );
        disconnect( mView, SIGNAL( pannedToLeft() ), this, SLOT( skipToPreviousVideoItem() ) );
    }

    disconnect( mControlsConfig, SIGNAL( controlListUpdated() ), this, SLOT( controlsListUpdated() ) );
    disconnect( mControlsTimer, SIGNAL( timeout() ), this, SLOT( hideAllControls() ) );
    disconnect( hbInstance->allMainWindows()[0], SIGNAL( orientationChanged( Qt::Orientation ) ),
                this, SLOT( handleOrientationChanged( Qt::Orientation ) ) );

    mView->setWidget( NULL );

    mControls.clear();

    if ( mControlsTimer )
    {
        delete mControlsTimer;
        mControlsTimer = NULL;
    }

    if ( mRNLogoTimer )
    {
        disconnect( mRNLogoTimer, SIGNAL( timeout() ), this, SLOT( handleRNLogoTimeout() ) );

        delete mRNLogoTimer;
        mRNLogoTimer = NULL;
    }

    if ( mControlsPolicy )
    {
        delete mControlsPolicy;
        mControlsPolicy = NULL;
    }

    if ( mControlsConfig )
    {
        delete mControlsConfig;
        mControlsConfig = NULL;
    }

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }

    if ( mThumbnailManager )
    {
        delete mThumbnailManager;
        mThumbnailManager = NULL;
    }

    if ( mVolumeControl )
    {
        delete mVolumeControl;
        mVolumeControl = NULL;
    }

    if ( mIsAttachOperation )
    {
        //
        // disable connection for 'attach' operation
        //
        disconnect( this, SIGNAL( attachVideoPath( const QString& ) ),
                    mVideoServices, SLOT( itemSelected( const QString& ) ) );
    }

    if ( mVideoServices )
    {
        //
        // decrease videoservices instance count
        //
    	mVideoServices->decreaseReferenceCount();
    	mVideoServices = 0;
    }

    if( mShareUi )
    {
        delete mShareUi;
        mShareUi = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::addFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::addFileDetails( VideoPlaybackViewFileDetails* details )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::addFileDetails"));

    mFileDetailsAdded = true;

    mFileDetails = details;

    mFileDetails->mRNFormat = realFormat( mFileDetails->mClipName );

    mControlsConfig->updateControlsWithFileDetails();

    evaluateAndChangeViewMode();

    //
    // grab pan gesture for playlist and seekable(skippable) clip
    //
    if ( mFileDetails->mMultiItemPlaylist && mFileDetails->mSeekable )
    {
        mView->grabGesture( Qt::PanGesture );
        connect( mView, SIGNAL( pannedToRight() ), this, SLOT( skipToPreviousVideoItem() ) );
        connect( mView, SIGNAL( pannedToLeft() ), this, SLOT( skipToNextVideoItem() ) );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleEvent
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleEvent(
    TVideoPlaybackControlCommandIds event, int value )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::handleEvent(%d)"), event);

    switch ( event )
    {
        case EControlCmdSetPosition:
        {
            MPX_DEBUG(_L("    [EControlCmdSetPosition [%d]]"), value );

            positionChanged( (qreal)value / (qreal)KPbMilliMultiplier);
            break;
        }
        case EControlCmdSetDuration:
        {
            MPX_DEBUG(_L("    [EControlCmdSetDuration [%d]]"), value );

            durationChanged( (qreal)value / (qreal)KPbMilliMultiplier );
            break;
        }
        case EControlCmdStateChanged:
        {
            MPX_DEBUG(_L("    [EControlCmdStateChanged]"));

            handleStateChange( (TMPXPlaybackState)value );
            break;
        }
        case EControlCmdSetVolume:
        {
            MPX_DEBUG(_L("    [EControlCmdSetVolume [%d]]"), value );

            volumeChanged( value );
            break;
        }
        case EControlCmdSetAspectRatio:
        {
            MPX_DEBUG(_L("    [EControlCmdSetAspectRatio %d]"), value );

            aspectRatioChanged( value );

            break;
        }
        case EControlCmdSetDownloadSize:
        {
            MPX_DEBUG(_L("    [EControlCmdSetDownloadSize [%d]]"), value );
            setDownloadSize( value );
            break;
        }
        case EControlCmdDownloadUpdated:
        {
            MPX_DEBUG(_L("    [EControlCmdDownloadUpdated [%d]]"), value );
            updateDownloadPosition( value );
            break;
        }
        case EControlCmdDownloadComplete:
        {
            MPX_DEBUG(_L("    [EControlCmdDownloadComplete [%d]]"), value);
            updateDownloadPosition( value );
            break;
        }
        case EControlCmdSurfaceAttached:
        case EControlCmdSurfaceDetached:
        case EControlCmdSetDownloadPaused:
        case EControlCmdClearDownloadPaused:
        {
            mControlsConfig->updateControlList( event );
            break;
        }
        case EControlCmdTvOutConnected:
        {
            MPX_DEBUG(_L("    [EControlCmdTvOutConnected]"));

            mFileDetails->mTvOutConnected = true;
            evaluateAndChangeViewMode();

            break;
        }
        case EControlCmdTvOutDisconnected:
        {
            MPX_DEBUG(_L("    [EControlCmdTvOutDisConnected]"));

            mFileDetails->mTvOutConnected = false;
            evaluateAndChangeViewMode();

            break;
        }
        case EControlCmdHandleErrors:
        {
            MPX_DEBUG(_L("    [EControlCmdHandleErrors]"));

            handleErrors();

            break;
        }
        case EControlCmdShowVolumeControls:
        {
            MPX_DEBUG(_L("    [EControlCmdShowVolumeControls]"));

            showVolumeControls();
            break;
        }
        case EControlCmdHandleBackgroundEvent:
        {
            MPX_DEBUG(_L("    [EControlCmdHandleBackgroundEvent]"));

            resetControls();
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleStateChange
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleStateChange( TMPXPlaybackState newState )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::handleStateChange()"),
                   _L("new state = %d"), newState );

    //
    // Somehow EPbStatePlaying, EPbStatePaused gets called twice continously
    //
    if ( newState != mState )
    {
        mState = newState;

        switch ( newState )
        {
            case EPbStatePlaying:
            {
                if ( mViewMode == EFullScreenView )
                {
                    hideAllControls();
                }
                else
                {
                    showControls();
                }

                updateState();

                break;
            }
            case EPbStateBuffering:
            {
                if ( mFileDetails->mPlaybackMode != EMPXVideoLocal )
                {
                    showControls();
                }

                updateState();

                break;
            }
            case EPbStatePaused:
            case EPbStateInitialising:
            case EPbStateNotInitialised:
            case EPbStatePluginSeeking:
            {
                //
                //  Show all the controls
                //
                showControls();

                updateState();

                break;
            }
            default:
            {
                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::controlsListUpdated()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::controlsListUpdated()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::controlsListUpdated()"));

    hideAllControls();

    QList<TVideoPlaybackControls>& updatedList = mControlsConfig->controlList();

    QList<TVideoPlaybackControls> myList = updatedList;

    int controlCount = mControls.count();

    int i = 0 ;
    int index = KErrNotFound;


    for ( int iCnt = 0 ; iCnt < controlCount ; iCnt++ )
    {
        index = myList.indexOf( mControls[i]->controlIndex() );

        if ( index == KErrNotFound )
        {
            //
            //  Delete control since it doesn't exist in new list
            //
            mControls.removeAt( i );
        }
        else
        {
            //
            //  Control exists in new list.
            //  Update the policy property based on file details and view mode to the controls
            //
            TUint properties = 0;
            mControlsPolicy->setControlProperties(
                    mControls[i]->controlIndex(), properties, mFileDetails, mViewMode );
            mControls[i]->updateControlProperties( properties );

            //
            //  Control exists in new list.
            //
            myList.removeAt( index );
            i++;
        }
    }

    //
    //  The updated list will contain added controls only
    //
    for ( int j = 0 ; j < myList.count() ; j++ )
    {
        appendControl( myList[j] );
    }

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->updateControlsWithFileDetails( mFileDetails );
    }

    showControls();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::appendControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::appendControl( TVideoPlaybackControls controlIndex )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::appendControl()"),
                   _L("control index = %d"), controlIndex );

    TUint properties = 0;

    mControlsPolicy->setControlProperties( controlIndex, properties, mFileDetails, mViewMode );
    VideoPlaybackFullScreenControl* control;

    switch ( controlIndex )
    {
        case EBufferingAnimation:
        {
            //
            // Load animation icons
            //
            QString animationName = ":/videoplaybackview/animations/";

            if ( mFileDetails->mRNFormat )
            {
                animationName.append( "rn_preroll_anim.axml" );
            }
            else
            {
                animationName.append( "generic_preroll_anim.axml" );
            }

            HbIconAnimationManager* manager = HbIconAnimationManager::global();
            bool ok = manager->addDefinitionFile( animationName );

            MPX_DEBUG(_L("    EBufferingAnimation add animation definition ok = %d"), ok);

            //
            // Buffering animation icon
            //
            mLoader->load( KPLAYBACKVIEW_DOCML, "BufferingAnimation", &ok );
            MPX_DEBUG(_L("    EBufferingAnimation load BufferingAnimation ok = %d"), ok);

            QGraphicsWidget *widget = mLoader->findWidget( QString( "bufferingIcon" ) );

            HbLabel *bufferingAnim = qobject_cast<HbLabel*>( widget );

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          bufferingAnim,
                                                          properties );
            mControls.append( control );

            break;
        }
        case EStatusPane:
        {
            //
            // Status key (signal + title + back key)
            //
            control = new VideoPlaybackStatusPaneControl( this,
                                                          controlIndex,
                                                          NULL,
                                                          properties );
            mControls.append( control );

            break;
        }
        case EControlBar:
        {
            //
            // Button bar
            //
            QGraphicsWidget *widget = mLoader->findWidget( QString( "controlBarLayout" ) );
            VideoPlaybackControlBar *controlBar =
                qobject_cast<VideoPlaybackControlBar*>( widget );
            controlBar->initialize();

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          controlBar,
                                                          properties );
            mControls.append( control );

            break;
        }
        case EFileDetailsWidget:
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "fileDetailsLayout" ) );
            VideoPlaybackFileDetailsWidget *fileDetails =
                qobject_cast<VideoPlaybackFileDetailsWidget*>( widget );

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          fileDetails,
                                                          properties );

            mControls.append( control );
            break;
        }
        case EIndicatorBitmap:
        {
            bool ok = true;

            QGraphicsWidget *widget = mLoader->findWidget( QString( "bitmapLayout" ) );

            if ( widget == NULL )
            {
                mLoader->load( KPLAYBACKVIEW_DOCML, "IndicatorBitmaps", &ok );
                widget = mLoader->findWidget( QString( "bitmapLayout" ) );

                MPX_DEBUG(_L("    EIndicatorBitmap load IndicatorBitmaps ok = %d"), ok);
            }

            HbWidget *bitmapWidget = qobject_cast<HbWidget*>( widget );

            setDefaultBitmap();

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          bitmapWidget,
                                                          properties );
            mControls.append( control );

            break;
        }
        case ERealLogoBitmap:
        {
            bool ok = false;

            mLoader->load( KPLAYBACKVIEW_DOCML, "RNBitmap", &ok );
            MPX_DEBUG(_L("    ERealLogoBitmap load RNBitmap ok = %d"), ok);

            QGraphicsWidget *widget = mLoader->findWidget( QString( "rnLogoBitmap" ) );

            HbWidget *bitmapWidget = qobject_cast<HbWidget*>( widget );

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          bitmapWidget,
                                                          properties );
            mControls.append( control );

            connect( bitmapWidget, SIGNAL( visibleChanged() ),
                     this, SLOT( handleRNLogoVisibleChanged() ) );

            break;
        }
        case EDetailsViewPlaybackWindow:
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "detailsPlaybackWindow" ) );
            VideoPlaybackDetailsPlaybackWindow *detailsPlaybackWindow =
                    qobject_cast<VideoPlaybackDetailsPlaybackWindow*>( widget );
            detailsPlaybackWindow->initialize();

            control = new VideoPlaybackFullScreenControl( this,
                                                          controlIndex,
                                                          detailsPlaybackWindow,
                                                          properties );
            mControls.append( control );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleTappedOnScreen()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleTappedOnScreen()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::handleTappedOnScreen()"));

    //
    // If we are in full screen view, toggle the visibility when playing and paused
    // If we are in details view, issue playpause
    // If we are in audio only view, ignore
    //
    switch( mViewMode )
    {
        case EFullScreenView:
        {
            if ( mState == EPbStatePlaying || mState == EPbStatePaused )
            {
                if ( isVisible() )
                {
                    //
                    // If the volume control is visible, hide it
                    //
                    if ( mVolumeControl && mVolumeControl->isVisible() )
                    {
                        mVolumeControl->setVisible( false );
                    }

                    hideAllControls();
                }
                else
                {
                    showControls();
                }
            }

            break;
        }
        case EDetailsView:
        {
            //
            // ignore the tap for 'Pause' action for non-pausable stream in
            // 'Details' view during 'Playing' state
            //
            if ( mState != EPbStatePlaying || mFileDetails->mPausableStream )
            {
                handleCommand( EMPXPbvCmdPlayPause );
            }

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::resetDisappearingTimers( TTimerAction timerAction )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::resetDisappearingTimers()"));

    if ( mControlsTimer->isActive() )
    {
        mControlsTimer->stop();
    }

    //
    //  Do not start the time for DetailsView and AudioOnlyView
    //
    if ( timerAction == ETimerReset &&
         mState == EPbStatePlaying &&
         mViewMode == EFullScreenView )
    {
        MPX_DEBUG(_L("    Starting Controls Timer"));

        mControlsTimer->start();
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::hideAllControls()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::hideAllControls()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::hideControls()"));

    resetDisappearingTimers( ETimerCancel );

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->setVisible( false );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::showControls()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::showControls()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::showControls()"));

    resetDisappearingTimers( ETimerReset );

    if ( ! mViewTransitionIsGoingOn )
    {
        if ( mOrientation == Qt::Horizontal )
        {
            for ( int i = 0 ; i < mControls.count() ; i++ )
            {
                mControls[i]->setVisibility( mState );
            }
        }
        else
        {
            mShowControlsWhenInHorizontal = true;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::isVisible()
{
    bool visible = false;

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->controlIndex() == EControlBar )
        {
            if ( mControls[i]->isVisible() )
            {
                visible = true;
            }

            break;
        }
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::isVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleCommand(
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::handleCommand(%d)"), command);

    switch( command )
    {
        case EMPXPbvCmdSetPosition:
        {
            TRAP_IGNORE( mViewWrapper->SetPropertyL( EPbPropertyPosition,
                                                     value * KPbMilliMultiplier ) );
            break;
        }
        case EMPXPbvCmdSetVolume:
        {
            TRAP_IGNORE( mViewWrapper->SetPropertyL( EPbPropertyVolume, value ) );
            break;
        }
        default:
        {
            TRAP_IGNORE( mViewWrapper->HandleCommandL( command ) );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::volumeChanged( int volume )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::volumeChanged() [%d]"), volume);

    if ( ! mVolumeControl )
    {
        mVolumeControl = new VideoPlaybackVolumeControl( this );
    }

    mVolumeControl->volumeChanged( volume );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::showVolumeControls()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::showVolumeControls()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::showVolumeControls()"));

    if ( ! mVolumeControl )
    {
        mVolumeControl = new VideoPlaybackVolumeControl( this );
    }

    mVolumeControl->setVisible( true );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::durationChanged() [%d]"), duration);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->durationChanged( duration ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::positionChanged() [%d]"), position);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->positionChanged( position ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("CMPXVideoPlaybackControlsController::aspectRatioChanged() [%d]"), aspectRatio);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->aspectRatioChanged( aspectRatio ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::updateState()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::updateState() [%d]"), mState);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->updateState( mState );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::realFormat()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::realFormat( QString filename )
{
    bool realFormat = false;

    if ( !filename.isNull() && !filename.isEmpty() )
    {
        if ( mFileDetails->mPlaybackMode == EMPXVideoStreaming ||
             mFileDetails->mPlaybackMode == EMPXVideoLiveStreaming )
        {
            TBufC<255> file(filename.utf16());
            realFormat = realFormatForStreaming( file );
        }
        else
        {
            realFormat = realFormatForLocal();
        }
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::realFormat() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::realFormatForStreaming()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::realFormatForStreaming( const TDesC& des )
{
    bool realFormat = false;
    TParse filePath;

    _LIT(KMPXRMEXT, ".R" );

    int err = filePath.Set( des, NULL, NULL );

    //
    // It is valid to have a "\" character in a url, but parsing fails,
    // switch these characters to "/" for the local string and try parsing again.
    //
    if ( err == KErrBadName )
    {
        int backsashPos = des.LocateF('\\');

        if( backsashPos != KErrNotFound )
        {
            HBufC* fileName = NULL;

            TRAP( err,  fileName = des.AllocL() );

            if ( err == KErrNone )
            {
                int count( fileName->Des().Length() );

                for ( int j = backsashPos ; j < count; ++j )
                {
                    if ( fileName->Des()[j]== '\\' )
                    {
                        fileName->Des()[j]='/';
                    }
                }
                err = filePath.Set( fileName->Des(), NULL, NULL );
            }

            if ( fileName )
            {
                delete fileName;
                fileName = NULL;
            }
        }
    }

    if ( err == KErrNone )
    {
        //
        // The requirement to support arguments with the extension in streaming links
        // for-ex: /video.3gp?start=10&end=40 will have to supported
        // as well. So just by doing p.Ext() would retrieve all the string
        // after "." so by doing a Left(2) would retrieve only
        // .3g or .rm and so on. This would help better
        // interpret the extension and decide the branding accordingly
        //
        filePath.Set( filePath.NameAndExt(), NULL, NULL );
        TPtrC extension = filePath.Ext().Left( 2 );

        TBuf<2> buf;
        buf.Format( extension );
        buf.UpperCase();

        // RealMedia Branding
        if ( ! buf.Compare( KMPXRMEXT ) )
        {
            realFormat = true;
        }
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::realFormatForStreaming()[%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::realFormatForLocal()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::realFormatForLocal()
{
    bool realFormat = false;

    QString real( "real" );
    QString rn( "rn" );

    if ( mFileDetails->mMimeType.contains( real, Qt::CaseInsensitive ) ||
         mFileDetails->mMimeType.contains( rn, Qt::CaseInsensitive ) )
    {
        realFormat = true;
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::realFormatForLocal() [%d]"), realFormat);

    return realFormat;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::setDownloadSize( int size )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::setDownloadSize() [%d]"), size);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->setDownloadSize( size ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::updateDownloadPosition( int size )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::updateDownloadPosition() [%d]"), size);

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->updateDownloadPosition( size ) )
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleErrors
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleErrors()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::handleErrors()"),
                   _L("mState = %d"), mState );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::isSoftKeyVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::isSoftKeyVisible()
{
    bool visible = false;

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->controlIndex() == EStatusPane )
        {
            if ( mControls[i]->isVisible() )
            {
                visible = true;
            }

            break;
        }
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::isSoftKeyVisible() [%d]"), visible);

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::changeViewMode
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::evaluateAndChangeViewMode(
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::evaluateAndChangeViewMode( %d, %d )"),
            viewMode, transitionEffect );

    switch ( viewMode )
    {
        case EFullScreenView:
        case EDetailsView:
        {
            if ( mFileDetails->mVideoEnabled &&
                 ! mFileDetails->mTvOutConnected )
            {
                if ( viewMode != mViewMode )
                {
                    mViewMode = viewMode;

                    updateVideoRect( transitionEffect );
                }

                break;
            }
            //
            // Internal fall-through
            // If it is not possible to change the mode to full screen/details
            // coz of tvout or audio only clip
            // then open audio only view instead
            //
        }
        case EAudioOnlyView:
        {
            //
            // If there is any event changed within audio only view,
            // update bitmap to show current status
            //
            if ( ! mFileDetails->mVideoEnabled || mFileDetails->mTvOutConnected )
            {
                if ( mViewMode == EAudioOnlyView )
                {
                    setDefaultBitmap();
                }
                else
                {
                    mViewMode = EAudioOnlyView;

                    mControlsConfig->updateControlList( EControlCmdAudionOnlyViewOpened );

                    if ( mFileDetails->mTvOutConnected )
                    {
                        generateThumbNail();
                    }
                }
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::updateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::updateVideoRectDone()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::updateVideoRectDone()"));

    mViewTransitionIsGoingOn = false;

    TVideoPlaybackControlCommandIds event = EControlCmdFullScreenViewOpened;

    if ( mViewMode == EDetailsView )
    {
        event = EControlCmdDetailsViewOpened;
    }

    mControlsConfig->updateControlList( event );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::skipToPreviousVideoItem
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::skipToPreviousVideoItem()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::skipToPreviousVideoItem()"));

    if ( mViewMode == EFullScreenView )
    {
        handleCommand( EMPXPbvCmdPreviousListItem );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::skipToNextVideoItem
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::skipToNextVideoItem()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::skipToNextVideoItem()"));

    if ( mViewMode == EFullScreenView )
    {
        handleCommand( EMPXPbvCmdNextListItem );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::updateVideoRect()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::updateVideoRect( bool transitionEffect )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::updateVideoRect( %d )"),
            transitionEffect );

    if ( mFileDetails->mVideoEnabled && ! mFileDetails->mTvOutConnected )
    {
        mViewTransitionIsGoingOn = true;

        QRectF rect;

        MPX_DEBUG(_L("VideoPlaybackControlsController::updateVideoRect() : mViewMode = %d )"),
                mViewMode );

        if ( mViewMode == EDetailsView )
        {
            QGraphicsWidget *parent = mLoader->findWidget( QString( "detailsPlaybackWindow" ) );
            QGraphicsWidget *child = mLoader->findWidget( QString( "small_transparentWindow" ) );

            rect = parent->mapRectToScene( child->geometry() );
        }
        else if ( mViewMode == EFullScreenView )
        {
            QGraphicsWidget *widget = mLoader->findWidget( QString( "content" ) );
            rect = widget->geometry();
        }

        //
        // Turn off the transition effect since it hits performance with high resolution clip
        // Need to test again with transition effect on with IVE 3.5
        //
        mViewWrapper->UpdateVideoRect(
                rect.x(), rect.y(), rect.width(), rect.height(), false );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::setDefaultBitmap()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::setDefaultBitmap()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::setDefaultBitmap()"));

    //
    // set specific bitmaps for audio only and tv out cases
    // If we already requested a thumbnail, we will set the thumbnail after we receive the thumbnail
    //
    if ( mLoader && mThumbNailState != EThumbNailRequsted )
    {
        QGraphicsWidget *tvOutBitmap = mLoader->findWidget( "tvOutBitmap" );
        tvOutBitmap->setVisible( false );

        QGraphicsWidget *realAudioOnlyBitmap = mLoader->findWidget( "realAudioOnlyBitmap" );
        realAudioOnlyBitmap->setVisible( false );

        QGraphicsWidget *partialAudioOnlyBitmap = mLoader->findWidget( "partialAudioOnlyBitmap" );
        partialAudioOnlyBitmap->setVisible( false );

        QGraphicsWidget *audioOnlyBitmap = mLoader->findWidget( "audioOnlyBitmap" );
        audioOnlyBitmap->setVisible( false );

        if ( mFileDetails->mTvOutConnected )
        {
            tvOutBitmap->setVisible( true );
        }
        else if ( mFileDetails->mRNFormat )
        {
            realAudioOnlyBitmap->setVisible( true );
        }
        else if ( mFileDetails->mPartialPlayback )
        {
            partialAudioOnlyBitmap->setVisible( true );
        }
        else
        {
            audioOnlyBitmap->setVisible( true );
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::generateThumbNail()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::generateThumbNail()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::generateThumbNail()"));

    //
    // Generate thumbnail if it is local playback
    //
    if ( mFileDetails->mPlaybackMode == EMPXVideoLocal )
    {
        if ( mThumbNailState == EThumbNailEmpty )
        {
            mThumbnailManager = new ThumbnailManager(); ;

            mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailLarge );
            mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForPerformance );

            if ( connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
                          this, SLOT( handleThumbnailReady( QPixmap , void * , int , int ) ) ) )
            {
                mThumbnailManager->getThumbnail( mFileDetails->mClipName );
                mThumbNailState = EThumbNailRequsted;
            }
        }
    }
    else
    {
        mThumbNailState = EThumbNailNotAvailable;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleThumbnailReady()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleThumbnailReady(
        QPixmap tnData, void *internal , int id, int error )
{
    Q_UNUSED( internal );
    Q_UNUSED( id );

    if( ! error && mLoader )
    {
        QGraphicsWidget *tvOutBitmap = mLoader->findWidget( "tvOutBitmap" );
        HbLabel *tvOutLabel = qobject_cast<HbLabel*>( tvOutBitmap );

        QIcon qicon( tnData );

        HbIcon hbIcon( qicon );
        hbIcon.setSize( tvOutBitmap->size() );
        tvOutLabel->setIcon( hbIcon );

        mThumbNailState = EThumbNailSet;
    }
    else
    {
        mThumbNailState = EThumbNailNotAvailable;
    }

    disconnect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
                this, SLOT( handleThumbnailReady( QPixmap , void * , int , int ) ) );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::attachVideo()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::attachVideo()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::attachVideo()"),
                   _L("file = %s"), mFileDetails->mClipName.data() );

    //
    // close playback view
    //
    mView->closePlaybackView();

    //
    // emit signal to launch videoservices itemSelected() slot
    //
    emit( attachVideoPath( mFileDetails->mClipName ) );

}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::sendVideo()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::sendVideo()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::sendVideo()"),
                   _L("file = %s"), mFileDetails->mClipName.data() );

    //
    // send video to shareUI
    //
    if( ! mShareUi )
    {
        mShareUi = new ShareUi();
    }

    QStringList fileList;
    fileList.append( mFileDetails->mClipName );
    mShareUi->send( fileList, true );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleRNLogoVisibleChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleRNLogoVisibleChanged()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::handleRNLogoVisibleChanged()"));

    QGraphicsWidget *widget = mLoader->findWidget( QString( "rnLogoBitmap" ) );

    if ( widget->isVisible() )
    {
        if ( mRNLogoTimer )
        {
            delete mRNLogoTimer;
            mRNLogoTimer = NULL;
        }

        mRNLogoTimer = new QTimer( this );
        mRNLogoTimer->setInterval( KRNLogoTimeOut );
        mRNLogoTimer->setSingleShot( true );
        connect( mRNLogoTimer, SIGNAL( timeout() ), this, SLOT( handleRNLogoTimeout() ) );

        mRNLogoTimer->start();
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleRNLogoTimeout()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleRNLogoTimeout()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::handleRNLogoTimeout()"));

    if ( mRNLogoTimer->isActive() )
    {
        mRNLogoTimer->stop();
    }

    //
    // Remove RN logo from the controls list and issue play command if needed
    //
    mControlsConfig->updateControlList( EControlCmdRemoveRNLogo );
    handleCommand( EMPXPbvCmdRealOneBitmapTimeout );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::isRNLogoBitmapInControlList()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::isRNLogoBitmapInControlList()
{
    bool exist = false;

    //
    // If we have a rn logo in the list, we are supposed to show the logo all the time
    // unless we are in the middle of orientation transition
    //
    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        if ( mControls[i]->controlIndex() == ERealLogoBitmap )
        {
            exist = true;
            break;
        }
    }

    MPX_DEBUG(_L("VideoPlaybackControlsController::isRNLogoBitmapInControlList() [%d]"), exist);

    return exist;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleOrientationChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleOrientationChanged( Qt::Orientation orientation )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::handleOrientationChanged()"),
                   _L("old orientation %d, new orientation = %d"), mOrientation, orientation );

    Qt::Orientation  oldOrientaiton = mOrientation;
    mOrientation = orientation;

    if ( oldOrientaiton == Qt::Vertical &&
         orientation == Qt::Horizontal &&
         mShowControlsWhenInHorizontal )
    {
        showControls();
        mShowControlsWhenInHorizontal = false;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::shouldShowRNLogo()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::shouldShowRNLogo()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::shouldShowRNLogo()"));

    bool showRNLogo = false;

    if ( mFileDetails->mRNFormat &&
         ! mViewWrapper->IsResumingPlaybackAfterTermination() )
    {
        showRNLogo = true;
    }

    return showRNLogo;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::resetControls()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::resetControls()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::resetControls()"));

    for ( int i = 0 ; i < mControls.count() ; i++ )
    {
        mControls[i]->resetControl();
    }
}

// End of File
