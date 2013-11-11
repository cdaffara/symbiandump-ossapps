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
#include <ceunitalloctestcasedecorator.h>
#include "cphoneringingtone.h"
#include "cphoneringingtoneplayer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::CPhoneRingingtonePlayer
// -----------------------------------------------------------------------------
//
CPhoneRingingtonePlayer::CPhoneRingingtonePlayer(  )
    :CActive( CActive::EPriorityStandard )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructL(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::~CPhoneRingingtonePlayer
// -----------------------------------------------------------------------------
//
CPhoneRingingtonePlayer::~CPhoneRingingtonePlayer(  )
    {
    
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::NewL
// -----------------------------------------------------------------------------
//
CPhoneRingingtonePlayer * CPhoneRingingtonePlayer::NewL(  )
    {
    SMC_MOCK_NEWL0( CPhoneRingingtonePlayer * )

    CPhoneRingingtonePlayer* self = new( ELeave ) CPhoneRingingtonePlayer;
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayProfileBasedTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayProfileBasedTone( 
        CPhoneRingingTone * aAudioVideoRingingTone )
    {
    SMC_MOCK_METHOD1( void, CPhoneRingingTone *, aAudioVideoRingingTone )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayBeepOnce
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayBeepOnce( 
        TInt aVolume )
    {
    SMC_MOCK_METHOD1( void, TInt, aVolume )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlaySilentTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlaySilentTone(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayVideoRingingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayVideoRingingTone( 
        const CPhoneRingingTone & aRingingTone,
        TInt aVolume,
        TProfileRingingType aRingingType,
        TBool aPersonalTone )
    {
    SMC_MOCK_METHOD4( void, const CPhoneRingingTone &, aRingingTone, 
        TInt, aVolume, 
        TProfileRingingType, aRingingType, 
        TBool, aPersonalTone )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayDefaultTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayDefaultTone( 
        TInt aVolume,
        TProfileRingingType aRingingType )
    {
    SMC_MOCK_METHOD2( void, TInt, aVolume, 
        TProfileRingingType, aRingingType )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayBackupTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayBackupTone( 
        TInt aVolume,
        TProfileRingingType aRingingType )
    {
    SMC_MOCK_METHOD2( void, TInt, aVolume, 
        TProfileRingingType, aRingingType )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::StopPlaying
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::StopPlaying(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayer
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer * CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayer(  )
    {
    SMC_MOCK_METHOD0( CPhoneAudioPlayer * )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::MuteActiveAudioPlayer
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::MuteActiveAudioPlayer(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandleCenRepChangeL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleCenRepChangeL( 
        const TUid & aUid,
        const TUint aId )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD2( void, const TUid &, aUid, 
        const TUint, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::RunL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::RunL(  )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::DoCancel
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DoCancel(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandleAudioPlayerError
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleAudioPlayerError( 
        TPhoneAudioPlayerErrorEvent aEvent,
        TInt aError,
        TInt aId )
    {
    SMC_MOCK_METHOD3( void, TPhoneAudioPlayerErrorEvent, aEvent, 
        TInt, aError, 
        TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandleAudioPlayerInitComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleAudioPlayerInitComplete( 
        TInt aId )
    {
    SMC_MOCK_METHOD1( void, TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandlePlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandlePlayingComplete( 
        TInt aId )
    {
    SMC_MOCK_METHOD1( void, TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandleTimeOutL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleTimeOutL(  )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::DeletePlayerAsync
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DeletePlayerAsync( 
        TPlayerId aPlayer )
    {
    SMC_MOCK_METHOD1( void, TPlayerId, aPlayer )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructSequencePlayer
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructSequencePlayer( 
        TPlayerId aPlayer )
    {
    SMC_MOCK_METHOD1( void, TPlayerId, aPlayer )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructSequencePlayerL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructSequencePlayerL( 
        TPlayerId aPlayer )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD1( void, TPlayerId, aPlayer )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructBackupPlayer
// -----------------------------------------------------------------------------
//
TInt CPhoneRingingtonePlayer::ConstructBackupPlayer(  )
    {
    SMC_MOCK_METHOD0( TInt )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructBackupPlayerL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructBackupPlayerL(  )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructDefaultPlayerL
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructDefaultPlayerL(  )
    {
    EUNIT_LEAVE_POINT
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConstructTonePlayer
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer * CPhoneRingingtonePlayer::ConstructTonePlayer( 
        const CPhoneRingingTone & aRingingTone,
        TInt aId )
    {
    SMC_MOCK_METHOD2( CPhoneAudioPlayer *, const CPhoneRingingTone &, aRingingTone, 
        TInt, aId )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConvertRingingType
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::TRingingType CPhoneRingingtonePlayer::ConvertRingingType( 
        TProfileRingingType aRingingType )
    {
    SMC_MOCK_METHOD1( CPhoneAudioPlayer::TRingingType, TProfileRingingType, aRingingType )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::ConvertVideoRingingType
// -----------------------------------------------------------------------------
//
MPhoneVideoPlayer::TPlayMode CPhoneRingingtonePlayer::ConvertVideoRingingType( 
        TProfileRingingType aRingingType )
    {
    SMC_MOCK_METHOD1( MPhoneVideoPlayer::TPlayMode, TProfileRingingType, aRingingType )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::CleanupPlayers
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::CleanupPlayers(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::DoHandlePlayerError
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DoHandlePlayerError( 
        TBool aDelete,
        TBool aSync )
    {
    SMC_MOCK_METHOD2( void, TBool, aDelete, 
        TBool, aSync )
    }


