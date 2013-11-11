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
* Description:  LiveComm Videotelephony's Remte Video Player
*
*/

#include "clcvtremotevideoplayer.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtRemoteVideoPlayer* CLcVtRemoteVideoPlayer::NewL(
    CLcVtSession* avtSession,
    MLcAudioControl& aLcAudioControl )
    {
    CLcVtRemoteVideoPlayer* self = 
        new( ELeave ) CLcVtRemoteVideoPlayer( avtSession, aLcAudioControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtRemoteVideoPlayer::CLcVtRemoteVideoPlayer( 
    CLcVtSession* avtSession,
    MLcAudioControl& aLcAudioControl ) :
    CLcVtVideoPlayerBase( avtSession, aLcAudioControl )
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CLcVtRemoteVideoPlayer::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtRemoteVideoPlayer::~CLcVtRemoteVideoPlayer()
    {
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcVideoPlayer::TLcVideoPlayerState 
CLcVtRemoteVideoPlayer::LcVideoPlayerState() const
    {
    TLcVideoPlayerState playerState = MLcVideoPlayer::EPlaying;
    return playerState;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
TBool CLcVtRemoteVideoPlayer::LcIsPlayingL()
    {    
    return ETrue;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtRemoteVideoPlayer::SetLcWindowRectL( TRect aRect )
    {    
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iX: %d", aRect.iTl.iX )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iTl.iY: %d", aRect.iTl.iY )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iWidth: %d", aRect.Width() )
    __VTPRINT2( DEBUG_MEDIA , "    RemoteVideoPlayer.RMRect.iHeight: %d", aRect.Height() )    
    
    CLcVtVideoPlayerBase::SetLcWindowRectL( aRect );    
    }

