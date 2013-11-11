/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  LiveComm Videotelephony's Local Video Player
*
*/

#include "clcvtlocalvideoplayer.h"
#include "tlcvtcamerahandler.h"
#include "lcvtplugincommand.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//

CLcVtLocalVideoPlayer* CLcVtLocalVideoPlayer::NewL( 
    CLcVtSession* avtSession,
    MLcAudioControl& aLcAudioControl,
    TVtCameraHandler& aCameraHandler)
    {
    CLcVtLocalVideoPlayer* self = 
        new( ELeave )CLcVtLocalVideoPlayer( 
            avtSession,
            aLcAudioControl, 
            aCameraHandler);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtLocalVideoPlayer::CLcVtLocalVideoPlayer( 
    CLcVtSession* avtSession,
    MLcAudioControl& aLcAudioControl,
    TVtCameraHandler& aCameraHandler) :
    CLcVtVideoPlayerBase( avtSession, aLcAudioControl ),
    iCameraHandler( aCameraHandler ) 
    {
    iOrientationChanged = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CLcVtLocalVideoPlayer::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtLocalVideoPlayer::~CLcVtLocalVideoPlayer()
    {
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcVideoPlayer::TLcVideoPlayerState 
CLcVtLocalVideoPlayer::LcVideoPlayerState() const
    {
    TLcVideoPlayerState playerState;
    if ( ivtSession->LcVtStates().MediaState().IsVideo() )
        {
        playerState = MLcVideoPlayer::EPlaying;
        }
    else
        {
        playerState = MLcVideoPlayer::EPaused;
        }
    __VTPRINT2( DEBUG_GEN, "CLcVtLocalVideoPlayer.LcVideoPlayerState = %d", playerState )
    return playerState;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
TBool CLcVtLocalVideoPlayer::LcIsPlayingL()
    {         
    TBool result = (LcVideoPlayerState() == MLcVideoPlayer::EPlaying);
    __VTPRINT2( DEBUG_GEN, "CLcVtLocalVideoPlayer.LcIsPlayingL = %d", result )
    return result;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
void CLcVtLocalVideoPlayer::LcPlayL(TLcVideoPlayerSource /*aSource*/)
    {
    __VTPRINT( DEBUG_GEN, "CLcVtLocalVideoPlayer.LcPlayL" )
    ivtSession->HandleCommandL(EPluginCmdEnableVideo);
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
void CLcVtLocalVideoPlayer::LcPauseL()
    {
    __VTPRINT( DEBUG_GEN, "CLcVtLocalVideoPlayer.LcPauseL" )
    ivtSession->HandleCommandL(EPluginCmdDisableVideo);
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcCameraControl* CLcVtLocalVideoPlayer::LcCameraControl()
    {
    return &iCameraHandler;
    }


// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcZoomControl* CLcVtLocalVideoPlayer::LcZoomControl()
    {
    return &iCameraHandler;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcBrightnessControl* CLcVtLocalVideoPlayer::LcBrightnessControl()
    {
    return &iCameraHandler;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtLocalVideoPlayer::SetLcWindowRectL( TRect aRect )
    {
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.RMRect.iTl.iX: %d", aRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.RMRect.iTl.iY: %d", aRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.RMRect.iWidth: %d", aRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    LocalVideoPlayer.RMRect.iHeight: %d", aRect.Height() ) 
    
    CLcVtVideoPlayerBase::SetLcWindowRectL( aRect );    
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtLocalVideoPlayer::SetLcWindowOrientationL( TLcWindowOrientation aOrientation )
    {    
    __VTPRINT2( DEBUG_MEDIA , "CLcVtLocalVideoPlayer.SetLcWindowOrientationL: %d", aOrientation ) 
    iOrientationChanged = EFalse;    
    if( aOrientation != iWindowOrientation )
        iOrientationChanged = ETrue;    
    CLcVtVideoPlayerBase::SetLcWindowOrientationL( aOrientation );    
    }

TBool CLcVtLocalVideoPlayer::IsOrientationChanged()
    {
    return iOrientationChanged;
    }
