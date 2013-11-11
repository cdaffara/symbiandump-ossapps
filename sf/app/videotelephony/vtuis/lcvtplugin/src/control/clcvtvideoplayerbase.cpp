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
* Description:  LiveComm Videotelephony's Video Player Base Class
*
*/

#include "clcvtvideoplayerbase.h"
#include "clcvtsession.h"


CLcVtVideoPlayerBase::CLcVtVideoPlayerBase( 
    CLcVtSession* avtSession,
    MLcAudioControl& aLcAudioControl ) :
    ivtSession( avtSession ),
    iLcAudioControl( aLcAudioControl )
    {
    iIsWindowEnabled = ETrue;
    iWindowOrientation = MLcWindow::EPortrait;
    }


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CLcVtVideoPlayerBase::~CLcVtVideoPlayerBase()
    {
    }


// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
#if 0
MLcVideoPlayer::TLcVideoPlayerState 
CLcVtVideoPlayerBase::LcVideoPlayerState() const
    {
    return MLcVideoPlayer::EUnavailable;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
TBool CLcVtVideoPlayerBase::LcIsPlayingL()
    {
    return EFalse;
    }
#endif
// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
void CLcVtVideoPlayerBase::LcPlayL( TLcVideoPlayerSource /*aSource*/ )
    {
    // NOP
    __VTPRINT( DEBUG_GEN, "CLcVtVideoPlayerBase.LcPlayL" )
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
void CLcVtVideoPlayerBase::LcPauseL()
    {
    // NOP
    __VTPRINT( DEBUG_GEN, "CLcVtVideoPlayerBase.LcPauseL" )
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcWindow* CLcVtVideoPlayerBase::LcWindow()
    {
    return this;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcCameraControl* CLcVtVideoPlayerBase::LcCameraControl()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcSourceFileControl* CLcVtVideoPlayerBase::LcSourceFileControl()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcDestinationFileControl* CLcVtVideoPlayerBase::LcDestinationFileControl()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcAudioControl* CLcVtVideoPlayerBase::LcAudioControl()
    {
    return &iLcAudioControl;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcZoomControl* CLcVtVideoPlayerBase::LcZoomControl()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// From MLcVideoPlayer
// -----------------------------------------------------------------------------
//
MLcBrightnessControl* CLcVtVideoPlayerBase::LcBrightnessControl()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtVideoPlayerBase::EnableLcWindowL( TBool aEnable )
    {    
    iIsWindowEnabled = aEnable;    
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
TBool CLcVtVideoPlayerBase::IsLcWindowEnabled()
    {
    return iIsWindowEnabled;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtVideoPlayerBase::SetLcWindowRectL( TRect aRect )
    {
    iWindowRect = aRect;    
    }


// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
TRect CLcVtVideoPlayerBase::LcWindowRect()
    {
    return iWindowRect;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
void CLcVtVideoPlayerBase::SetLcWindowOrientationL( 
    TLcWindowOrientation aOrientation )
    {    
    iWindowOrientation = aOrientation;
    }

// -----------------------------------------------------------------------------
// From MLcWindow
// -----------------------------------------------------------------------------
//
MLcWindow::TLcWindowOrientation CLcVtVideoPlayerBase::LcWindowOrientationL()
    {
    return iWindowOrientation;
    }
