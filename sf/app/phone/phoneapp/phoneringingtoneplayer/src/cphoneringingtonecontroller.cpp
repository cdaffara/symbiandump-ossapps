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

// INCLUDE FILES
#include <telephonyvariant.hrh>
#include "cphoneringingtonecontroller.h"
#include "phonelogger.h"
#include "cphoneringingtone.h"
#include "tphonecmdparamringtone.h"
#include "cphonecenrepproxy.h"
#include "cphoneringingtoneplayer.h"
#include "cphonettsplayer.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneRingingToneController* CPhoneRingingToneController::NewL()
    {
    CPhoneRingingToneController* self = 
        new ( ELeave ) CPhoneRingingToneController();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::CPhoneRingingToneController
// -----------------------------------------------------------------------------
//
CPhoneRingingToneController::CPhoneRingingToneController()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::ConstructL()" );
    //iMediatorSender = CPhoneMediatorSender::NewL();
    //iMediatorSender->AttachCoverUiObserverL( this );
    
    iRingingtonePlayer = CPhoneRingingtonePlayer::NewL(); 

    // Check video player configuration.
    iArbitraryVideoScaling = 
    CPhoneCenRepProxy::Instance()->IsTelephonyFeatureSupported( 
            KTelephonyLVFlagArbitraryVideoScaling );

    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::~CPhoneRingingToneController
// -----------------------------------------------------------------------------
//
CPhoneRingingToneController::~CPhoneRingingToneController()
    {
    /*if ( iMediatorSender )
        {
        iMediatorSender->DetachCoverUiObserver( this );
        }*/

    //delete iMediatorSender;
    //iMediatorSender = NULL
    
    
    delete iAudioVideoRingingTone;
    iAudioVideoRingingTone = NULL;
    delete iTTSPlayer;
    iTTSPlayer = NULL;
    delete iRingingtonePlayer;
    iRingingtonePlayer = NULL;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayRingToneL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::PlayRingToneL( 
                                         TPhoneCommandParam* aCommandParam )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::PlayRingToneL()" );

    if ( aCommandParam->ParamId() == TPhoneCommandParam::EPhoneParamIdRingTone )
        {
        TPhoneCmdParamRingTone* ringToneParam = 
            static_cast<TPhoneCmdParamRingTone*>( aCommandParam );

        const TInt volume = ringToneParam->Volume();
        TProfileRingingType ringingType = 
        static_cast<TProfileRingingType>( ringToneParam->RingingType() );
        
        if ( !iAudioVideoRingingTone )
            {
            iAudioVideoRingingTone = CPhoneRingingTone::NewL(
                ringToneParam->RingTone() );
            }

        // Store pofile based parameters.
        iAudioVideoRingingTone->SetFileName( ringToneParam->RingTone() );
        iAudioVideoRingingTone->SetVolume( volume );
        iAudioVideoRingingTone->SetRingingType ( ringingType );
        iAudioVideoRingingTone->SetTtsToneToBePlayed(
                ringToneParam->TextToSay().Length()? ETrue : EFalse);

        
        // Caller contact text and image and image has
        // higher priority than video ringing tone set for caller.
        if ( iAudioVideoRingingTone->IsVideoRingingTone() )
            {
            if ( ringToneParam->IsCallerImage() )
                {
                // Play only audio from video ringingtone
                PlayAudioRingTone(
                        volume,
                        ringingType );
                }
            else
                {
                // Play video ring tone
                const TBool ringToneIsPersonal = ringToneParam->Type() ==
                    EPhoneRingTonePersonal;
                PlayVideoRingingTone( 
                    *iAudioVideoRingingTone, 
                    volume, 
                    ringingType,
                    ringToneIsPersonal );
                }
            }
        else
            {
            // Play text to speech, if available
            if ( ringToneParam->TextToSay().Length() )
                {
                if (iTTSPlayer) 
                    {
                    delete iTTSPlayer;
                    iTTSPlayer = NULL;
                    }
                iTTSPlayer = CPhoneTTSPlayer::NewL(iRingingtonePlayer); 
                iTTSPlayer->AddTtsPlaybackIfNeeded();
                iTTSPlayer->PlayTtsTone(
                    ringToneParam->TextToSay(),
                    iAudioVideoRingingTone );
                }
            // Play audio ring tone
            PlayAudioRingTone( 
                volume, 
                ringingType );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayAudioRingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayAudioRingTone( 
    TInt aVolume, 
    TProfileRingingType aRingingType )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::PlayAudioRingTone()" );
    __PHONELOG2( 
        EBasic,
        EPhoneControl, 
        "CPhoneRingingToneController::PlayAudioRingTone - aVolume(%d), aRingingType(%d)",
        aVolume,
        aRingingType );
 
    if ( !iAudioVideoRingingTone ||
         !iAudioVideoRingingTone->CheckAndHandleToneSizeLimit() )
        {
        iRingingtonePlayer->PlayDefaultTone( aVolume, aRingingType );
        }
    else if ( aRingingType == EProfileRingingTypeSilent )
        {
        iRingingtonePlayer->PlaySilentTone();
        }
    else if ( aRingingType == EProfileRingingTypeBeepOnce )
        {
        iRingingtonePlayer->PlayBeepOnce( aVolume );
        }
    else
        {
        iRingingtonePlayer->PlayProfileBasedTone(iAudioVideoRingingTone);
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::MuteRingingToneOnAnswer
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::MuteRingingToneOnAnswer()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::MuteRingingToneOnAnswer()" );
    
  /*  if ( iVideoPlayer && iTonePlayingStatus == EVideoTonePlaying ||
         iTonePlayingStatus == EPersonalVideoTonePlaying )
        {
        // Mute the video audio
        iVideoPlayer->MuteVideoRingTone();
        iTonePlayingStatus = ESilentVideoTonePlaying;
        iVolume = 0;
        }
    else
        {*/
        DoMuteRingingTone();
        //}
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::MuteRingingTone
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::MuteRingingTone()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::MuteRingingTone()" );
    
 /*   if ( iVideoPlayer && iTonePlayingStatus == EVideoTonePlaying ||
         iTonePlayingStatus == EPersonalVideoTonePlaying )
        {
        // Mute the video audio
        iVideoPlayer->MuteVideoRingTone();
        iTonePlayingStatus = ESilentVideoTonePlaying;
        iVolume = 0;
        }
    else
        {*/
        StopPlaying();
        //}
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::StopPlaying
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneRingingToneController::StopPlaying()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::StopPlaying()" );
    if ( iTTSPlayer )
        {
        iTTSPlayer->StopPlaying();
        delete iTTSPlayer;
        iTTSPlayer = NULL;
        }
    iRingingtonePlayer->StopPlaying();
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::DoMuteRingingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::DoMuteRingingTone()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::DoMuteRingingTone()" );
    
  /*  switch( iTonePlayingStatus )
        {
        case EVideoTonePlaying: // video ringing tone, fall through
        case EPersonalVideoTonePlaying: // fall through
        case ESilentVideoTonePlaying:
            if ( iVideoPlayer )
                {
                iVideoPlayer->MuteVideoRingTone();
                return;                 
                }
            break;
            
        default:
            break;
        }*/
    
    iRingingtonePlayer->MuteActiveAudioPlayer();
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ConvertVideoRingingType
// -----------------------------------------------------------------------------
//
MPhoneVideoPlayer::TPlayMode CPhoneRingingToneController::ConvertVideoRingingType(
    TProfileRingingType aRingingType )
    {
    MPhoneVideoPlayer::TPlayMode playMode;
        
    switch ( aRingingType )
        {
        case EProfileRingingTypeAscending:
            playMode = MPhoneVideoPlayer::EPlayAscending;
            break;
        case EProfileRingingTypeRingingOnce:
            playMode = MPhoneVideoPlayer::EPlayOnce;
            break;
        case EProfileRingingTypeRinging:
        default:
            playMode = MPhoneVideoPlayer::EPlayInLoop;
            break;
        }
    
    return playMode;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::SetVideoPlayer
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::SetVideoPlayer( 
    MPhoneVideoPlayer* aVideoPlayer )
    {
    iVideoPlayer = aVideoPlayer;    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayVideoRingingTone
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayVideoRingingTone( 
    const CPhoneRingingTone& /*aRingingTone*/, 
    TInt /*aVolume*/, 
    TProfileRingingType /*aRingingType*/,
    TBool /*aPersonalTone*/ )
    {
    /*
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::PlayVideoRingingTone()" );
    
    __ASSERT_DEBUG( iVideoPlayer, Panic( EPhoneViewGeneralError ) );
    
    iVolume = aVolume;
    iRingingType = aRingingType;
    TBool startTimer( ETrue );
    
     // Extended security check
    if ( ExtendedSecurity() )
        {
        if ( !aRingingTone.IsFileInRom() &&
             !aRingingTone.IsFileDrmProtected() )
            {
            __PHONELOG( EBasic, 
                        EPhoneControl, 
                        "CPhoneRingingToneController::HandleVideoPlayerError - PermissionDenied" );
            iRingingtonePlayer->PlayDefaultTone( iVolume, iRingingType );
            return;
            }
        __PHONELOG( EBasic, 
                    EPhoneControl, 
                    "CPhoneRingingToneController::HandleVideoPlayerError - ExtSecChk ok" );            
            }
         
    // If flip is closed then show video on Cover UI
    TBool videoPlaySentToCoverUi( EFalse );
    TInt leaveCode( KErrNone );
    if ( FeatureManager::FeatureSupported( KFeatureIdCoverDisplay ) )
        {
        const TInt flipOpen = CPhonePubSubProxy::Instance()->Value(
            KPSUidUikon, KUikFlipStatus );
        if( !flipOpen ) 
            {
            TRAP( leaveCode, videoPlaySentToCoverUi = CoverUIPlayVideoRingingToneL( 
                aRingingTone ) );
            }
        }
    // Play ringing tone here if video play sending did leave or
    // video playing wasn't delegated to Cover UI   
    if( !leaveCode && videoPlaySentToCoverUi )
        {
        iTimer->After( KPhoneMaxRingingWaiting, this );
        return;
        }
 
    __PHONELOG( EBasic, EPhoneControl, "CPhoneRingingToneController::PlayVideoRingingTone - play" );
    switch( aRingingType )
        {
        case EProfileRingingTypeSilent:
            iVideoPlayer->PlayVideoRingTone( 
                aRingingTone.FileName(),
                ConvertVideoRingingType( aRingingType ),
                0,
                iArbitraryVideoScaling,
                this );
            iRingingtonePlayer->PlaySilentTone();
            iVolume = 0; // for repeat
            startTimer = EFalse; // no need for fallback
            break;
        
        case EProfileRingingTypeBeepOnce:
            iVideoPlayer->PlayVideoRingTone( 
                aRingingTone.FileName(),
                ConvertVideoRingingType( aRingingType ),
                0,
                iArbitraryVideoScaling,
                this );
            iRingingtonePlayer->BeepOnce( aVolume );
            iVolume = 0; // for repeat
            startTimer = EFalse; // no need for fallback
            break;
        default:
            iVideoPlayer->PlayVideoRingTone( 
                aRingingTone.FileName(),
                ConvertVideoRingingType( aRingingType ),
                iVolume,
                iArbitraryVideoScaling,
                this );
            break;                
        }

    if ( aPersonalTone )
        {
        iTonePlayingStatus = EPersonalVideoTonePlaying;
        }
    else
        {
        iTonePlayingStatus = EVideoTonePlaying;
        }

    if ( startTimer && !iTimer->IsActive() )
        {
        // Start timer to guard video opening
        iTimer->After( KPhoneMaxRingingWaiting, this );
        }
    */
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerError
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerError( 
    TPhoneVideoPlayerErrorEvent /*aEvent*/,
    TInt /*aError*/ )
    {
   /* __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::HandleVideoPlayerError()" );

    if ( aError )
        {
        __PHONELOG1( EBasic, 
                     EPhoneControl, 
                     "CPhoneRingingToneController::HandleVideoPlayerError - error (%d)",
                     aError );        
        }
  
    // cancel guarding timer
    iTimer->Cancel();

    // to remove video window
    iVideoPlayer->CancelVideoRingTone();

    if ( iTonePlayingStatus == EPersonalVideoTonePlaying )
        {
        // Play default tone (active profile tone).
        if ( iAudioVideoRingingTone &&
             !iAudioVideoRingingTone->IsVideoRingingTone() )
            {
            PlayAudioRingTone( iVolume, iRingingType );
            }
        else // audio/video tone is video
            {
            PlayDefaultVideoAsync();
            }
        }
    else
        {
        // Play backup tone
        iRingingtonePlayer->PlayDefaultTone( iVolume, iRingingType );
        }
*/
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerInitComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerInitComplete()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::HandleVideoPlayerInitComplete()" );
    
    // cancel guarding timer
    //iTimer->Cancel();
    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::HandleVideoPlayerPlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::HandleVideoPlayerPlayingComplete()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::HandleVideoPlayerPlayingComplete()" );
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::PlayDefaultVideoAsync
// -----------------------------------------------------------------------------
//
void CPhoneRingingToneController::PlayDefaultVideoAsync()
    {
  /*  __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::PlayDefaultVideoAsync()" );    
     
    Cancel();

    iState = EPlayingDefaultVideo; 

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();*/
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ExtendedSecurity
// -----------------------------------------------------------------------------
//    
TBool CPhoneRingingToneController::ExtendedSecurity() const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::ExtendedSecurity()" );
    return EFalse;
    //return iExtSecNeeded;        
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::CoverUIPlayVideoRingingToneL
// -----------------------------------------------------------------------------
//   
TBool CPhoneRingingToneController::CoverUIPlayVideoRingingToneL( 
    const CPhoneRingingTone& aRingingTone )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::CoverUIPlayVideoRingingToneL()" );

    TBool showVideo( EFalse );

    RBuf8 data;
    CleanupClosePushL( data );
    data.CreateL( aRingingTone.FileName().Length() );
    data.Copy( aRingingTone.FileName() );
    /*TInt err = iMediatorSender->IssueCommand( 
        EPhoneCmdCoverUiShowMultimediaRingingTone, 
        data );
    if ( err == KErrNone )
        {
        showVideo = ETrue;
        iActiveCoverUICommand = ETrue;
        }*/
    CleanupStack::PopAndDestroy(); //data 

    return showVideo;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingToneController::ShowMultimediaRingingToneResponseL
// -----------------------------------------------------------------------------
//   
void CPhoneRingingToneController::ShowMultimediaRingingToneResponseL( 
    TInt aStatus )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingToneController::ShowMultimediaRingingToneResponseL()" );

    //iTimer->Cancel();
    if( aStatus != KErrNone && iActiveCoverUICommand )
        {
        iRingingtonePlayer->PlayDefaultTone( iVolume, iRingingType );           
        }
    iActiveCoverUICommand = EFalse;
    }


    
//  End of File  
