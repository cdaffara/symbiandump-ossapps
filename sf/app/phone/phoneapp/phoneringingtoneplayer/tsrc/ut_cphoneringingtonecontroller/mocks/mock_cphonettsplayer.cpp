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
* Description:
*
*/

#include <smcmockclassincludes.h>
#include "cphonettsplayer.h"
#include "cphoneringingtone.h"
#include "cphoneringingtoneplayer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::CPhoneTTSPlayer
// -----------------------------------------------------------------------------
//
CPhoneTTSPlayer::CPhoneTTSPlayer( 
        CPhoneRingingtonePlayer * aRingingtonePlayer )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::ConstructL(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::~CPhoneTTSPlayer
// -----------------------------------------------------------------------------
//
CPhoneTTSPlayer::~CPhoneTTSPlayer(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::NewL
// -----------------------------------------------------------------------------
//
CPhoneTTSPlayer * CPhoneTTSPlayer::NewL( 
        CPhoneRingingtonePlayer * aRingingtonePlayer )
    {
    SMC_MOCK_NEWL1( CPhoneTTSPlayer *, CPhoneRingingtonePlayer *, aRingingtonePlayer )

    CPhoneTTSPlayer* self = new( ELeave ) CPhoneTTSPlayer(aRingingtonePlayer);
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::PlayTtsTone
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::PlayTtsTone( 
        const TDesC & aTextToSay,
        CPhoneRingingTone * aAudioVideoRingingTone )
    {
    SMC_MOCK_METHOD2( void, const TDesC &, aTextToSay, 
        CPhoneRingingTone *, aAudioVideoRingingTone )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::StopPlaying
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::StopPlaying(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::AddTtsPlaybackIfNeeded
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::AddTtsPlaybackIfNeeded(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::HandleAudioPlayerError
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandleAudioPlayerError( 
        TPhoneAudioPlayerErrorEvent aEvent,
        TInt aError,
        TInt aId )
    {
    SMC_MOCK_METHOD3( void, TPhoneAudioPlayerErrorEvent, aEvent, 
        TInt, aError, 
        TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::HandleAudioPlayerInitComplete
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandleAudioPlayerInitComplete( 
        TInt aId )
    {
    SMC_MOCK_METHOD1( void, TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::HandlePlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandlePlayingComplete( 
        TInt aId )
    {
    SMC_MOCK_METHOD1( void, TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::HandleTtsDelayTimeout
// -----------------------------------------------------------------------------
//
TInt CPhoneTTSPlayer::HandleTtsDelayTimeout( 
        TAny * object )
    {
    //SMC_MOCK_METHOD1( TInt, TAny *, object )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::DoHandleTtsDelayTimeout
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::DoHandleTtsDelayTimeout(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::GetCurrentlyActiveAudioPlayerWithTTs
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer * CPhoneTTSPlayer::GetCurrentlyActiveAudioPlayerWithTTs(  )
    {
    SMC_MOCK_METHOD0( CPhoneAudioPlayer * )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::SolveNewVolumeAndRamptime
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::SolveNewVolumeAndRamptime( 
        TTtsStatus aStatus )
    {
    SMC_MOCK_METHOD1( void, TTtsStatus, aStatus )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::ReStartRingingTonePlayer
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::ReStartRingingTonePlayer(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::ConvertRingingType
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::TRingingType CPhoneTTSPlayer::ConvertRingingType( 
        TProfileRingingType aRingingType )
    {
    SMC_MOCK_METHOD1( CPhoneAudioPlayer::TRingingType, TProfileRingingType, aRingingType )
    }


