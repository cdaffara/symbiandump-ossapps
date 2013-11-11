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
* Description:  Implementation of VideoPlaybackStatusPaneControl
*
*/

// Version : %version: 23 %



#include <QFileInfo>

#include <hbmenu.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbgroupbox.h>
#include <hbinstance.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>

#include <w32std.h>

#include "mpxvideo_debug.h"
#include "videobaseplaybackview.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackstatuspanecontrol.h"
#include "videoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::VideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackStatusPaneControl::VideoPlaybackStatusPaneControl(
        VideoPlaybackControlsController* controller,
        TVideoPlaybackControls index,
        HbWidget* widget,
        TUint controlproperties )
    : VideoPlaybackFullScreenControl( controller, index, widget, controlproperties )
    , mActionBack( NULL )
    , mTitleLabel( NULL )
    , mTitleGroupBox( NULL )
    , mTitleLayout( NULL )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackStatusPaneControl::VideoPlaybackStatusPaneControl()"));

    Q_UNUSED( widget );

    mActionBack = new HbAction( Hb::BackNaviAction );

    //
    // Press "back" key means going back to previous view if it's avaiable
    //
    connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    mController->view()->setNavigationAction( mActionBack );

    connect( mController->view()->menu(), SIGNAL( aboutToShow() ), this, SLOT( handleAboutToShow() ) );
    connect( mController->view()->menu(), SIGNAL( aboutToHide() ), this, SLOT( handleAboutToHide() ) );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::~VideoPlaybackStatusPaneControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackStatusPaneControl::~VideoPlaybackStatusPaneControl()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackStatusPaneControl::~VideoPlaybackStatusPaneControl"));

    disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
    disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
    mController->view()->setNavigationAction( 0 );

    disconnect( mController->view()->menu(), SIGNAL( aboutToShow() ), this, SLOT( handleAboutToShow() ) );
    disconnect( mController->view()->menu(), SIGNAL( aboutToHide() ), this, SLOT( handleAboutToHide() ) );

    mController->view()->menu()->close();

    if ( mActionBack )
    {
        delete mActionBack;
        mActionBack = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::setVisible()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::setVisible( bool visible )
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::setVisible visible = %d"), visible);

    if ( mVisible != visible )
    {
        mVisible = visible;

        if ( mVisible )
        {
            mController->view()->setTitleBarVisible( true );
            mController->view()->setStatusBarVisible( true );

            if ( mController->viewMode() == EFullScreenView ||
                 mController->viewMode() == EDetailsView )
            {
                if ( mTitleLayout )
                {
                    mTitleLayout->setVisible( true );
                }
            }
        }
        else
        {
            mController->view()->menu()->close();
            mController->view()->setTitleBarVisible( false );
            mController->view()->setStatusBarVisible( false );

            if ( mTitleLayout )
            {
                mTitleLayout->setVisible( false );
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackStatusPaneControl::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::setVisibility()"));

    switch( state )
    {
        case EPbStatePlaying:
        case EPbStatePaused:
        {
            setMenu( mController->fileDetails() );
            break;
        }
        case EPbStatePluginSeeking:
        case EPbStateBuffering:
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            mController->view()->menu()->clearActions();
            break;
        }
    }

    VideoPlaybackFullScreenControl::setVisibility( state );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::updateControlsWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::updateControlsWithFileDetails()"));

    if ( mController->isFileDetailsAdded() && ! mTitleLabel )
    {
        //
        // If title is not available, show clip name
        //
        QString title = mController->fileDetails()->mTitle;

        if ( title.count() == 0 )
        {
            QFileInfo fileInfo( mController->fileDetails()->mClipName );
            title = fileInfo.completeBaseName();
        }

        QGraphicsWidget *qWidget = mController->layoutLoader()->findWidget( QString( "title" ) );
        mTitleLabel = qobject_cast<HbLabel*>( qWidget );
        mTitleLabel->setPlainText( title );

        qWidget = mController->layoutLoader()->findWidget( QString( "titleGroupBox" ) );
        mTitleGroupBox = qobject_cast<HbGroupBox*>( qWidget );
        mTitleGroupBox->setHeading( title );

        mTitleLayout = mController->layoutLoader()->findWidget( QString( "titleLayout" ) );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameItem *frameItem = new HbFrameItem();
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        frameItem->frameDrawer().setFillWholeRect( true );
        mTitleLabel->setBackgroundItem( frameItem );
    }

    switch( mController->viewMode() )
    {
        case EFullScreenView:
        {
            //
            // Set TitleBar transparent and go back to preview view with back key
            //
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setViewFlags( mController->view()->viewFlags() |
                                               HbView::ViewTitleBarTransparent |
                                               HbView::ViewStatusBarTransparent );

            if ( mTitleLayout )
            {
                mTitleLabel->setVisible( true );
                mTitleGroupBox->setVisible( false );
            }

            break;
        }
        case EDetailsView:
        {
            //
            // Set TitleBar opaque and go back to full screen view with back key
            //
            disconnect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );
            connect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );

            mController->view()->setViewFlags( HbView::ViewFlagNone );

            if ( mTitleLayout )
            {
                mTitleLabel->setVisible( false );
                mTitleGroupBox->setVisible( true );
            }

            break;
        }
        case EAudioOnlyView:
        {
            //
            // Set TitleBar opaque and go back to preview view with back key
            //
            disconnect( mActionBack, SIGNAL( triggered() ), this, SLOT( openFullScreenView() ) );
            connect( mActionBack, SIGNAL( triggered() ), mController->view(), SLOT( closePlaybackView() ) );

            mController->view()->setViewFlags( HbView::ViewFlagNone );
            break;
        }
    }

    setMenu( details );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::setMenu()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::setMenu( VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::setMenu()"));

    //
    // No available menu for now
    //
    Q_UNUSED( details );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::handleAboutToShow()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::handleAboutToShow()
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::handleAboutToShow()"));

    mController->resetDisappearingTimers( ETimerCancel );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::handleAboutToHide()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::handleAboutToHide()
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::handleAboutToHide()"));

    mController->resetDisappearingTimers( ETimerReset );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::controlListUpdated()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::controlListUpdated(
        VideoPlaybackViewFileDetails* details )
{
    setMenu( details );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackStatusPaneControl::openFullScreenView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackStatusPaneControl::openFullScreenView()
{
    MPX_DEBUG(_L("VideoPlaybackStatusPaneControl::openFullScreenView()"));

    mController->evaluateAndChangeViewMode( EFullScreenView, true );
}

// End of file
