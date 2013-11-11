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
* Description:  Implementation of VideoPlaybackDetailsPlaybackWindow
*
*/

// Version : %version:  20 %



#include <QGraphicsSceneMouseEvent>

#include <hbframeitem.h>
#include <hbpushbutton.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackdetailsplaybackwindow.h"

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDetailsPlaybackWindow::VideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDetailsPlaybackWindow::VideoPlaybackDetailsPlaybackWindow(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mInitialized( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDetailsPlaybackWindow::VideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDetailsPlaybackWindow::~VideoPlaybackDetailsPlaybackWindow
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDetailsPlaybackWindow::~VideoPlaybackDetailsPlaybackWindow()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDetailsPlaybackWindow::~VideoPlaybackDetailsPlaybackWindow()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDetailsPlaybackWindow::initialize
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackDetailsPlaybackWindow::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDetailsPlaybackWindow::initialize()"));

    VideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( loader && ! mInitialized )
    {
        mInitialized = true;

        VideoPlaybackViewFileDetails *details = mController->fileDetails();

        //
        // Connect signal and slot for play button
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "detailsViewPlayButton" ) );
        mPlayButton = qobject_cast<HbPushButton*>( widget );
        mPlayButton->setFlag( QGraphicsItem::ItemIsFocusable, false );

        connect( mPlayButton, SIGNAL( released() ), this, SLOT( play() ) );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameDrawer *drawer = mPlayButton->frameBackground();

        if( drawer == NULL )
        {
            drawer = new HbFrameDrawer();
        }

        drawer->setFillWholeRect( true );
        drawer->setFrameType( HbFrameDrawer::NinePieces );
        drawer->setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        mPlayButton->setFrameBackground( drawer );

        //
        // create 'attach' button and connect corresponding signal/slot
        //
        QGraphicsWidget *detailsAttachWidget = loader->findWidget( QString( "detailsAttachButton" ) );
        HbPushButton *attachButton = qobject_cast<HbPushButton*>( detailsAttachWidget );
        connect( attachButton, SIGNAL( released() ), mController, SLOT( attachVideo() ) );

        //
        // create 'share' button and connect corresponding signal/slot
        //
        QGraphicsWidget *detailsShareWidget = loader->findWidget( QString( "detailsShareButton" ) );
        HbPushButton *shareButton = qobject_cast<HbPushButton*>( detailsShareWidget );
        connect( shareButton, SIGNAL( released() ), mController, SLOT( sendVideo() ) );

        //
        // by default in xml layout, attachButton is not visible while shareButton is visible.
        // if it's an 'attach' operation, reverse the visibility order
        //
        if ( mController->isAttachOperation() )
        {
            attachButton->setVisible( true );
            shareButton->setVisible( false );
        }
        else
        {
            //
            // dim "share" button for streaming
            //
            if ( details->mPlaybackMode == EMPXVideoStreaming ||
                 details->mPlaybackMode == EMPXVideoLiveStreaming )
            {
                shareButton->setEnabled( false );
            }
        }
    }

    updateState( mController->state() );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDetailsPlaybackWindow::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackDetailsPlaybackWindow::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackDetailsPlaybackWindow::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePaused:
        {
            mPlayButton->setVisible( true );
            break;
        }
        default:
        {
            mPlayButton->setVisible( false );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDetailsPlaybackWindow::play()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackDetailsPlaybackWindow::play()
{
    MPX_DEBUG(_L("VideoPlaybackDetailsPlaybackWindow::play()"));

    //
    // Only 'Play' button is created in this Details Playback Window,
    // hence when user touches on the 'Play' button, 'Play' command will be issued here.
    // On the other hand, when user touches anywhere in the Details Playback Window, 
    // the 'PlayPause' command will be handled via handleTappedOnScreen() method in the controller
    // 
    mController->handleCommand( EMPXPbvCmdPlay );
}

//End of file
