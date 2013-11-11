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
* Description:  Implementation of Video playback view
*
*/

// Version : %version:  da1mmcf#15 %



//  Include Files
#include <hbinstance.h>

#include <mmf/common/mmferrors.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideo_debug.h"
#include "videoplaybackview.h"
#include "mpxvideoviewwrapper.h"
#include "mpxvideoplaybackuids.hrh"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoPlaybackView::VideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackView::VideoPlaybackView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackView::VideoPlaybackView()"));

    initializeVideoPlaybackView();
    mSyncClose = false;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::~VideoPlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackView::~VideoPlaybackView()
{
    MPX_DEBUG(_L("VideoPlaybackView::~VideoPlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::handleActivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackView::handleActivateView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackView::handleActivateView()"));

    VideoBasePlaybackView::handleActivateView();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::handleDeactivateView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackView::handleDeactivateView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackView::handleDeactivateView()"));

    VideoBasePlaybackView::handleDeactivateView();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::handleBack()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackView::handleBack()
{   
    MPX_ENTER_EXIT(_L("VideoPlaybackView::handleBack()"));

    closePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::handleSoftkeyBack()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackView::handleSoftkeyBack()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackView::handleSoftkeyBack()"));

    closePlaybackView();
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackView::handlePluginError()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackView::handlePluginError( int aError )
{
    MPX_DEBUG(_L("VideoPlaybackView::handlePluginError() aError = %d"), aError );
   
    switch( aError )
    {
        case KErrNotSupported:
        case KErrUnknown:
        case KErrMMDecoder:
        case KErrCorrupt:
        case KErrTooBig:
        {
            showDialog( hbTrId( "txt_videos_info_invalid_clip_operation_canceled" ) );
            break;
        }

        case KErrMMInvalidProtocol:
        case KErrMMInvalidURL:
        case KErrArgument:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_invalid_url" ) );
            break;
        }

        case KErrSessionClosed:
        {
            showDialog( hbTrId( "txt_videos_info_resource_lost" ) );
            break;
        }
        case KErrTimedOut:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_connection_time" ) );
            break;
        }
        case KErrNotFound:
        {
            showDialog( hbTrId( "txt_videos_info_file_not_found" ) );
            break;
        }
        case KErrMMNotEnoughBandwidth:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect_not_enough_band" ) );
            break;
        }
        case KErrDisconnected:
        case KErrMMProxyServer:
        case KErrCouldNotConnect:
        case KErrAbort:
        {
            showDialog( hbTrId( "txt_videos_info_unable_to_connect" ) );
            break;
        }
        case KMPXVideoPlayOver2GDuringVoiceCallError:
        {
            showDialog( hbTrId( "txt_videos_info_video_play_not_allowed_during_voic" ), false );
            break;
        }
        default:
        {
            VideoBasePlaybackView::handlePluginError( aError );

            break;
        }
    }
}

// -----------------------------------------------------------------------------
//   VideoPlaybackView::handlePdlStateChange
// -----------------------------------------------------------------------------
//
void VideoPlaybackView::handlePdlStateChange( int /*aState*/ )
{
    MPX_DEBUG(_L("VideoPlaybackView::handlePdlStateChange()"));
}

// -----------------------------------------------------------------------------
//   VideoPlaybackView::handleStoppedState
// -----------------------------------------------------------------------------
//
void VideoPlaybackView::handleStoppedState()
{
    MPX_DEBUG(_L("VideoPlaybackView::HandleStoppedState()"));

    if ( ! mVideoMpxWrapper->IsPlaylist() )
    {        
        handleBack();
    }
}

// EOF
