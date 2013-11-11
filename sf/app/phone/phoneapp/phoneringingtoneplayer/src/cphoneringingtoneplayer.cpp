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

#include <cntdef.h>
#include <f32file.h>
#include <featmgr.h>
#include <utility.h>
#include <AudioPreference.h>
#include <ProfileEngineDomainCRKeys.h>
#include <DRMHelperServerInternalCRKeys.h>
#include <DRMHelper.h>
#include <data_caging_path_literals.hrh>
#include "cphoneringingtoneplayer.h"
#include "cphoneaudioplayer.h"
#include "cphoneringingtone.h"
#include "phonelogger.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"
#include "cphonetimer.h"
#include "phoneconstants.h"
#include "phoneui.pan"

CPhoneRingingtonePlayer::CPhoneRingingtonePlayer():
CActive( CActive::EPriorityStandard ), iAsyncDeletePlayers( EPlayerCount )
{
    CActiveScheduler::Add( this );
}

CPhoneRingingtonePlayer::~CPhoneRingingtonePlayer()
    {
    iAsyncDeletePlayers.ResetAndDestroy();
    iAsyncDeletePlayers.Close();
        
    delete iTimer;
    
    CleanupPlayers();
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::CleanupPlayers()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::CleanupPlayers()" );
    
    delete iAudioPlayer;
    iAudioPlayer = NULL;
    delete iDefaultPlayer;
    iDefaultPlayer = NULL;
    delete iBeepOncePlayer;
    iBeepOncePlayer = NULL;
    delete iSilentPlayer;
    iSilentPlayer = NULL;
    delete iBackupPlayer;
    iBackupPlayer = NULL;
    delete iMdaServer;
    iMdaServer = NULL;
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CPhoneRingingtonePlayer* CPhoneRingingtonePlayer::NewL()
    {
    CPhoneRingingtonePlayer* self = 
        new ( ELeave ) CPhoneRingingtonePlayer();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructL()
    {
    
    // Construct timer.
    iTimer = CPhoneTimer::NewL();
    
    for ( TInt i = EPlayerFirst; i <= EPlayerLast; i++ )
        {
        User::LeaveIfError( 
            iAsyncDeletePlayers.Append( NULL ) );
        }
    
    //disabling the DRM for the TB10.1wk46 release. Lets see if need to put it back and how.
    //iExtSecNeeded = ( ( err == KErrNone ) && extSecNeeded ) ? ETrue : EFalse;
    iExtSecNeeded = EFalse; 
        
    // Construct players.
    if ( !iMdaServer )
        {
        iMdaServer = CMdaServer::NewL();
        }
    
    // Backup player. 
    ConstructBackupPlayer();
    ConstructDefaultPlayerL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayProfileBasedTone( CPhoneRingingTone* aAudioVideoRingingTone )
    {
    iAudioVideoRingingTone = aAudioVideoRingingTone;
    // Construct player for line 1.
     if ( !iAudioPlayer )
         {
         iAudioPlayer = ConstructTonePlayer( 
             *iAudioVideoRingingTone, 
             EPlayerAudio );
         if ( !iTimer->IsActive() )
             {
             // Start timer, since the player is constructed.
             iTimer->After( KPhoneMaxRingingWaiting, this );
             }
         }

     // Start playing.
     if ( !iAudioPlayer )
         {
         iTimer->Cancel();
         PlayDefaultTone( aAudioVideoRingingTone->Volume(), 
                          aAudioVideoRingingTone->RingingType() );
         }
     else
         {
         __PHONELOG( 
             EBasic, 
             EPhoneControl, 
             "CPhoneRingingtonePlayer::PlayAudioRingTone - Start playing audio" );
         iVolume = aAudioVideoRingingTone->Volume();
         iRingingType = aAudioVideoRingingTone->RingingType();
         //AddTtsPlaybackIfNeeded( aRingingType );
         iTonePlayingStatus = EAudioTonePlaying;
         iAudioPlayer->Play( 
             ConvertRingingType( aAudioVideoRingingTone->RingingType() ),
             aAudioVideoRingingTone->Volume(),
             aAudioVideoRingingTone->TtsToneToBePlayed());
         }
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayBeepOnce( TInt aVolume )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::BeepOnce()" );
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::BeepOnce - aVolume(%d)",
        aVolume );

    if ( !iBeepOncePlayer )
        {
        ConstructSequencePlayer( EPlayerBeepOnce );
        }

    if ( iBeepOncePlayer )
        {
        __PHONELOG( 
            EBasic, 
            EPhoneControl, 
            "CPhoneRingingtonePlayer::BeepOnce - Play" );
        iTonePlayingStatus = EBeepOnce;
        iBeepOncePlayer->Play( CPhoneAudioPlayer::ETypeRingingOnce, 
            aVolume, EFalse );        
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlaySilentTone()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::PlaySilentTone()" );

    if ( !iSilentPlayer )
        {
        ConstructSequencePlayer( EPlayerSilent );
        }

    if ( iSilentPlayer )
        {
        __PHONELOG( 
            EBasic, 
            EPhoneControl, 
            "CPhoneRingingtonePlayer::PlaySilentTone - play" );
        iTonePlayingStatus = ESilentTonePlaying;
        iSilentPlayer->Play( CPhoneAudioPlayer::ETypeRinging, 0, EFalse );        
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayDefaultTone( 
    TInt aVolume, 
    TProfileRingingType aRingingType )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::PlayDefaultTone()" );

    if( aRingingType == EProfileRingingTypeSilent ||
        aRingingType == EProfileRingingTypeBeepOnce )
        {
        return;
        }
        
    if ( !iDefaultRingingTone )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::PlayDefaultTone - default tone does not exist, play backup..." );
        PlayBackupTone( aVolume, aRingingType );
        return;    
        }

    // Construct player.
    if ( !iDefaultPlayer )
        {
        iDefaultPlayer = ConstructTonePlayer( 
            *iDefaultRingingTone, 
            EPlayerDefault );
        if ( !iTimer->IsActive() )
            {
            iTimer->After( KPhoneMaxRingingWaiting, this );
            }
        }

    // Start playing backup.
    if ( !iDefaultPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::PlayDefaultTone - player not constructed, play backup..." );
        iTimer->Cancel();
        PlayBackupTone( aVolume, aRingingType );
        }
    else
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::PlayDefaultTone - play" );
        iVolume = aVolume;
        iRingingType = aRingingType;
        iTonePlayingStatus = EDefaultTonePlaying;
        iDefaultPlayer->Play( 
            ConvertRingingType( aRingingType ), aVolume,
            iAudioVideoRingingTone->TtsToneToBePlayed());
        }

    }

// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::PlayBackupTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::PlayBackupTone( 
    TInt aVolume, 
    TProfileRingingType aRingingType )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::PlayBackupTone()" );
    __ASSERT_DEBUG( iBackupPlayer, Panic( EPhoneViewGeneralError ) );

    if ( !iBackupPlayer )
        {
        ConstructBackupPlayer();
        }

    if ( iBackupPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::PlayBackupTone - play" );
        iTonePlayingStatus = EBackupTonePlaying;
/*
        // playing text-to-speech, if necessary
        iTtsToneToBePlayed = EFalse;
        iTTsTimeOutCounter = 0;
        iTtsDelayTimer->Cancel();
        if ( iTtsPlayer )
            {
            iTtsPlayer->StopPlaying();
            delete iTtsPlayer;
            iTtsPlayer = NULL;
            }*/

        iBackupPlayer->Play( 
            ConvertRingingType( aRingingType ), aVolume, EFalse );        
        }
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::StopPlaying()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::StopPlaying()" );

    iTimer->Cancel();

    switch( iTonePlayingStatus )
        {
        case EAudioTonePlaying:
            if( iAudioPlayer )
                {
                iAudioPlayer->StopPlaying();
                delete iAudioPlayer;
                iAudioPlayer = NULL;
                }
            break;
        case EDefaultTonePlaying:
            if ( iDefaultPlayer )
                {
                iDefaultPlayer->StopPlaying();
                delete iDefaultPlayer;
                iDefaultPlayer = NULL;
                }
            break;
        case EBeepOnce:
            if( iBeepOncePlayer )
                {
                iBeepOncePlayer->StopPlaying();
                delete iBeepOncePlayer;
                iBeepOncePlayer = NULL;
                }
            break;
        case EBackupTonePlaying:
            if( iBackupPlayer )
                {
                iBackupPlayer->StopPlaying();
                }
            break;
        case ESilentTonePlaying:
            if( iSilentPlayer )
                {
                iSilentPlayer->StopPlaying();
                delete iSilentPlayer;
                iSilentPlayer = NULL;
                }
            break;
        case EVideoTonePlaying: // video ringing tone
        case EPersonalVideoTonePlaying:
        case ESilentVideoTonePlaying:
            if ( iBeepOncePlayer )
                {
                iBeepOncePlayer->StopPlaying();
                delete iBeepOncePlayer;
                iBeepOncePlayer = NULL;                    
                }
                
            if ( iSilentPlayer )
                {
                iSilentPlayer->StopPlaying();
                delete iSilentPlayer;
                iSilentPlayer = NULL;                    
                }    
            /*if ( iVideoPlayer )
                {
                iVideoPlayer->StopVideoRingTone();
                }*/
            if ( iAudioVideoRingingTone )
                {
                delete iAudioVideoRingingTone;
                iAudioVideoRingingTone = NULL;    
                }
            break;
        case EIdle:
            // Do nothing here.
            return;
        default:
            Panic( EPhoneViewCaseNotHandled );
            break;
        }

    iTonePlayingStatus = EIdle;
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneRingingtonePlayer::ConstructTonePlayer( 
    const CPhoneRingingTone& aRingingTone,
    TInt aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConstructTonePlayer()" );
    return CPhoneAudioPlayer::New(
        aRingingTone, 
        KAudioPriorityPhoneCall, 
        KAudioPrefIncomingCall,
        *this, 
        aId,
        iMdaServer,
        aId == EPlayerDefault ? EFalse : iExtSecNeeded );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CPhoneRingingtonePlayer::ConstructBackupPlayer()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConstructBackupPlayer()" );
    TRAPD( err, ConstructBackupPlayerL() );
    return err;
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructBackupPlayerL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConstructBackupPlayerL()" );

    // Construct Backup player.
    if ( !iBackupPlayer )
        {
        TFileName defaultSoundFile( KDriveZ );
        defaultSoundFile.Append( KPhoneDefaultSoundFile );
        iBackupPlayer = CPhoneAudioPlayer::NewL( 
            defaultSoundFile,
            KAudioPriorityPhoneCall,
            KAudioPrefIncomingCall,
            *this,
            EPlayerBackup,
            iMdaServer 
            );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructDefaultPlayerL()
    {
    // Construct the default ringing tone
    TBuf<KMaxFileName> defaultTone;
    defaultTone = _L("z:\\data\\sounds\\digital\\Nokia tune.aac");        
    TInt err(KErrNone);
    
    if ( err == KErrNone )
        {
        iDefaultRingingTone = CPhoneRingingTone::NewL( defaultTone );
        }
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructSequencePlayer( TPlayerId aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConstructSequencePlayer()" );

    // Construct players.
    TRAPD( err, ConstructSequencePlayerL( aId ) );
    if ( err )
        {
        __PHONELOG1( EBasic, EPhoneControl, 
        "CPhoneRingingtonePlayer::ConstructSequencePlayer - err(%d)", err );    
        }
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::ConstructSequencePlayerL( TPlayerId aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConstructSequencePlayerL()" );
    // Construct beep once player
    if ( aId == EPlayerBeepOnce )
        {
#ifdef __WINS__    
        TParse* fp = new (ELeave) TParse();
        fp->Set( KPhoneBeepOnceSoundFile, &KDC_RESOURCE_FILES_DIR, NULL ); 
        TFileName fileName( fp->FullName() );
        delete fp;

        iBeepOncePlayer = CPhoneAudioPlayer::NewL(
            fileName, 
            KAudioPriorityPhoneCall,
            KAudioPrefIncomingCall,
            *this, 
            EPlayerBeepOnce );
#else
        iBeepOncePlayer = CPhoneAudioPlayer::NewSeqL(
            KPhoneBeepSequence(),
            KAudioPriorityPhoneCall,
            KAudioPrefIncomingCall,
            *this, 
            EPlayerBeepOnce );
#endif // __WINS__
        }

    // Construct Silent Player.
    else if ( aId == EPlayerSilent )
        {
#ifdef __WINS__
        TParse* fp = new (ELeave) TParse();
        fp->Set( KPhoneSilentSoundFile, &KDC_RESOURCE_FILES_DIR, NULL ); 
        TFileName fileName( fp->FullName() );
        delete fp;

        iSilentPlayer = CPhoneAudioPlayer::NewL( 
            fileName, 
            KAudioPriorityPhoneCall,
            KAudioPrefIncomingCall,
            *this, 
            EPlayerSilent );
#else
        iSilentPlayer = CPhoneAudioPlayer::NewSeqL(
            KPhoneNoSoundSequence(),
            KAudioPriorityPhoneCall,
            KAudioPrefIncomingCall,
            *this,
            EPlayerSilent );
#endif // __WINS__
        }        
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleAudioPlayerError( 
    TPhoneAudioPlayerErrorEvent /*aEvent*/, 
    TInt aError, 
    TInt /*aId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::HandleAudioPlayerError()" );
    // This is called from CPhoneAudioPlayer - we do not 
    // want to delete instances from which this method
    // is called.
    
     __PHONELOG1( EBasic, 
                 EPhoneControl, 
                 "CPhoneRingingtonePlayer::HandleAudioPlayerError - error (%d)",
                 aError ); 

    const TBool deleteAll = ( aError == KErrServerTerminated );

    if ( deleteAll )
        {
        // KErrServerTerminated is considered as fatal error. So we
        // need to delete everything.

        for ( TInt i = EPlayerFirst; i <= EPlayerLast; i++ )
            {
            DeletePlayerAsync( static_cast< TPlayerId >( i ) );
            }

        // Now all member audio players point to NULL, except
        // those players in iAsyncDeletePlayers. Recreate 
        // players.

        ConstructBackupPlayer(); // Ignore error code.
        }

    iTimer->Cancel();
    DoHandlePlayerError( !deleteAll, EFalse );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleAudioPlayerInitComplete( TInt aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::HandleAudioPlayerInitComplete()" );
    // This is called from CPhoneAudioPlayer - we do not 
    // want to delete instances from which this method
    // is called. 

    if ( iTonePlayingStatus == EAudioTonePlaying && aId == EPlayerAudio ||
        iTonePlayingStatus == EDefaultTonePlaying && aId == EPlayerDefault )
        {
        iTimer->Cancel();
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRingingtonePlayer::HandlePlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandlePlayingComplete( TInt /*aId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::HandlePlayingComplete()" );
    // This is called from CPhoneAudioPlayer - we do not 
    // want to delete instances from which this method
    // is called. 


    __PHONELOG1( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::HandlePlayingComplete - delete player - iTonePlayingStatus(%d)",
        iTonePlayingStatus ); 
/*    if (iAudioVideoRingingTone->TtsToneToBePlayed() &&
        iAudioVideoRingingTone->RingingType() == EProfileRingingTypeRinging )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::HandlePlayingComplete - return" ); 
        return;
        }*/
    
    switch( iTonePlayingStatus )
        {
        case EAudioTonePlaying:
            DeletePlayerAsync( EPlayerAudio );
            iAudioPlayer = NULL;
            break;
            
        case EDefaultTonePlaying:
            DeletePlayerAsync( EPlayerDefault );
            iDefaultPlayer = NULL;
            break;
            
        case ESilentTonePlaying:
            DeletePlayerAsync( EPlayerSilent );
            iSilentPlayer = NULL;
            break;
            
        case EBeepOnce:
            DeletePlayerAsync( EPlayerBeepOnce );
            iBeepOncePlayer = NULL;
            break;
            
        case EVideoTonePlaying:
        case EPersonalVideoTonePlaying:
            DeletePlayerAsync( EPlayerBeepOnce );
            iBeepOncePlayer = NULL;
            return; // beep once + video
            
        case EBackupTonePlaying:
            break;
            
        default:
            Panic( EPhoneViewCaseNotHandled );
            break;
        }

    iTonePlayingStatus = EIdle;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DeletePlayerAsync( TPlayerId aPlayer )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::DeletePlayerAsync()" );
    __ASSERT_DEBUG( aPlayer >= EPlayerFirst && aPlayer < iAsyncDeletePlayers.Count(),
        Panic( EPhoneViewInvariant ) );
        
    // Player is not deleted here, but in RunL.
    Cancel();

    if ( iAsyncDeletePlayers[ aPlayer ] ) 
        {
        delete iAsyncDeletePlayers[ aPlayer ];
        iAsyncDeletePlayers[ aPlayer ] = NULL;
        }

    CPhoneAudioPlayer** players[] =
        {
        &iAudioPlayer,
        &iBeepOncePlayer,
        &iSilentPlayer,
        &iDefaultPlayer,
        &iBackupPlayer,
        &iTtsPlayer
        };

    __ASSERT_DEBUG( iAsyncDeletePlayers.Count() > aPlayer, Panic( EPhoneUtilsIndexOutOfBounds ) );
    iAsyncDeletePlayers[ aPlayer ] = 
        *(players[ aPlayer ]);
    *(players[ aPlayer ]) = NULL;

    iState = EDeletingAudioPlayer;

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::RunL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::RunL()" );

    switch ( iState )
        {
        case EDeletingAudioPlayer:
            {
            for ( TInt i = EPlayerFirst; i <= EPlayerLast; i++ )
                {
                delete iAsyncDeletePlayers[ i ];
                iAsyncDeletePlayers[ i ] = NULL;
                }
            }
            break;

      case EPlayingDefaultVideo:
            {
            //PlayAudioRingTone( iVolume, iRingingType );
            }
            break;

        case EIdleState:  
        default:
            break;
        } // switch iState
    
    iState = EIdleState;
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DoCancel()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::DoCancel()" );
    // Request is completed immediately before SetActive.
    }


// -----------------------------------------------------------
// CPhoneRingingtonePlayer::HandleCenRepChangeL
// -----------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleCenRepChangeL( 
    const TUid& aUid,
    const TUint /*aId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::HandleCenRepChangeL()");
    
    if ( aUid == KCRUidDRMHelperServer )
        {
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::HandleTimeOutL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::HandleTimeOutL()" );
   /* if( iActiveCoverUICommand )
        {
        iMediatorSender->CancelCommand( 
            EPhoneCmdCoverUiShowMultimediaRingingTone );
        PlayDefaultTone( iVolume, iRingingType );
        iActiveCoverUICommand = EFalse;
        }*/

    // Guarding timer for startup has expired
    if ( iTonePlayingStatus == EVideoTonePlaying ||
         iTonePlayingStatus == ESilentVideoTonePlaying ||   
         iTonePlayingStatus == EPersonalVideoTonePlaying )
        {
        //iVideoPlayer->CancelVideoRingTone(); // close video ringtone display
        PlayDefaultTone( iVolume, iRingingType );
        }
    else
        {
        // Start new audio player synchronously.
        DoHandlePlayerError( ETrue, ETrue );
        }           
    
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::DoHandlePlayerError(TBool aDelete, TBool aSync )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::DoHandlePlayerError()" );
    switch ( iTonePlayingStatus )
        {
        case EAudioTonePlaying:
            if ( aDelete )
                {
                if ( aSync )
                    {
                    delete iAudioPlayer;
                    }
                else
                    {
                    DeletePlayerAsync( EPlayerAudio );
                    }

                iAudioPlayer = NULL;
                }

#ifdef __WINS__
            //PlayAudioRingTone( iVolume, iRingingType );
            iTonePlayingStatus = EDefaultTonePlaying;
#else
            PlayDefaultTone( iVolume, iRingingType );
#endif
            break;
        case EDefaultTonePlaying:
            if ( aDelete )
                {
                if ( aSync )
                    {
                    delete iDefaultPlayer;
                    }
                else
                    {
                    DeletePlayerAsync( EPlayerDefault );
                    }

                iDefaultPlayer = NULL;
                }

            PlayBackupTone( iVolume, iRingingType );
            break;
        case ESilentTonePlaying:
        case EBeepOnce:
        case EBackupTonePlaying:
            break;
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::TRingingType CPhoneRingingtonePlayer::ConvertRingingType(
    TProfileRingingType aRingingType )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ConvertRingingType()" );

    CPhoneAudioPlayer::TRingingType ringingType = 
        CPhoneAudioPlayer::ETypeRinging;
    switch ( aRingingType )
        {
        case EProfileRingingTypeRinging:
            ringingType = CPhoneAudioPlayer::ETypeRinging;
            break;

        case EProfileRingingTypeAscending:
            ringingType = CPhoneAudioPlayer::ETypeAscending;
            break;

        case EProfileRingingTypeRingingOnce:
            ringingType = CPhoneAudioPlayer::ETypeRingingOnce;
            break;

        default:
            break;
        }
    return ringingType;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
CPhoneAudioPlayer* 
CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayer()
    {    
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayerWithTTs()" );

    if( iAudioPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayerWithTTs - audio player" ); 
        return iAudioPlayer;         
        }
    else if ( iDefaultPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayerWithTTs - default player" ); 
        return iDefaultPlayer;     
        }
    else if( iBackupPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingtonePlayer::GetCurrentlyActiveAudioPlayerWithTTs - backup player" ); 
        return iBackupPlayer;
        }
    else
        {
        return NULL;
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingtonePlayer::MuteActiveAudioPlayer()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtonePlayer::ActiveAudioPlayer()" );

    switch( iTonePlayingStatus )
        {
        case EAudioTonePlaying:
            if( iAudioPlayer )
                {
                iAudioPlayer->MutePlaying();
                }
            break;
            
        case EDefaultTonePlaying:
            if ( iDefaultPlayer )
                {
                return iDefaultPlayer->MutePlaying();
                }
            break;
            
        case EBeepOnce:
            if( iBeepOncePlayer )
                {
                return iBeepOncePlayer->MutePlaying();
                }
            break;
            
        case EBackupTonePlaying:
            if( iBackupPlayer )
                {
                return iBackupPlayer->MutePlaying();
                }
            break;
            
        case ESilentTonePlaying:
            if( iSilentPlayer )
                {
                return iSilentPlayer->MutePlaying();
                }
            break;
            
        case EVideoTonePlaying: // video ringing tone
        case EPersonalVideoTonePlaying:
        case ESilentVideoTonePlaying:
            if ( iBeepOncePlayer )
                {
                return iBeepOncePlayer->MutePlaying();                    
                }    
            else if ( iSilentPlayer )
                {
                return iSilentPlayer->MutePlaying();                    
                }    
            break;
            
        default:
            break;
        }      
    }
