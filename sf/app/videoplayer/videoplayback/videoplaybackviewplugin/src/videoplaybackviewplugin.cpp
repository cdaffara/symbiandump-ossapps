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
* Description:  VideoPlaybackViewPlugin
*
*/

// Version : %version:  8 %



#include <QtGui>
#include <hbview.h>
#include <xqplugin.h>
#include <hbinstance.h>

#include "mpxvideo_debug.h"
#include "videoplaybackview.h"
#include "mpxhbvideocommondefs.h"
#include "videoplaybackviewplugin.h"


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoPlaybackViewPlugin::VideoPlaybackViewPlugin()
    : mView( NULL )
    , mViewActivated ( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::VideoPlaybackViewPlugin()"));
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoPlaybackViewPlugin::~VideoPlaybackViewPlugin()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::~VideoPlaybackViewPlugin()"));
    destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::createView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::createView()"));
 
    if ( ! mView )
    {
        mView = new VideoPlaybackView();
        connect( mView, SIGNAL( activatePreviousView() ), this, SLOT( back() ) );
    }
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::destroyView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::destroyView()"));

    if ( mView )
    {
        disconnect( mView, SIGNAL( activatePreviousView() ), this, SLOT( back() ) );
    
        delete mView;
        mView = NULL;
    }
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::activateView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::activateView()"));

    if ( mView && !mViewActivated )
    {
        mView->handleActivateView();
        mViewActivated = true;
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::deactivateView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::deactivateView()"));

    if ( mView && mViewActivated )
    {
        mView->handleDeactivateView();
        mViewActivated = false;
    }
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoPlaybackViewPlugin::getView()
{
    return mView;
}

// ---------------------------------------------------------------------------
// VideoPlaybackViewPlugin::orientationChange
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::orientationChange( Qt::Orientation orientation )
{
    Q_UNUSED( orientation );
}

// ---------------------------------------------------------------------------
// VideoPlaybackViewPlugin::back
// ---------------------------------------------------------------------------
//
void VideoPlaybackViewPlugin::back()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackViewPlugin::back()"));

    emit command( MpxHbVideoCommon::CollectionView );
}

XQ_EXPORT_PLUGIN2( videoplaybackviewplugin, VideoPlaybackViewPlugin );

// End of File
