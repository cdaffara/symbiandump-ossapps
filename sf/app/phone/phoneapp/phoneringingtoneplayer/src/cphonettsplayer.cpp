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

#include "cphonettsplayer.h"
#include "cphoneringingtone.h"
#include "cphoneringingtoneplayer.h"
#include "phonelogger.h"
#include <AudioPreference.h>

CPhoneTTSPlayer::CPhoneTTSPlayer(CPhoneRingingtonePlayer* aRingingtonePlayer):
iRingingtonePlayer (aRingingtonePlayer)
    {
    

    }

CPhoneTTSPlayer::~CPhoneTTSPlayer()
    {
    delete iTtsDelayTimer;
    delete iTtsPlayer;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CPhoneTTSPlayer* CPhoneTTSPlayer::NewL(CPhoneRingingtonePlayer* aRingingtonePlayer)
    {
    CPhoneTTSPlayer* self = 
        new ( ELeave ) CPhoneTTSPlayer(aRingingtonePlayer);
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::ConstructL()
    {
    iTtsDelayTimer = CPhoneTimer::NewL();
    }
      
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::PlayTtsTone( 
    const TDesC& aTextToSay, 
    CPhoneRingingTone* aAudioVideoRingingTone )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::PlayTtsTone()" );
    //__PHONELOG2( EBasic, EPhoneControl, "CPhoneTTSPlayer::PlayTtsTone - aVolume(%d), aRingingType(%d)",
    //    aVolume, aRingingType );

    if ( iTtsPlayer )
        {
        delete iTtsPlayer;
        iTtsPlayer = NULL;
        }

    //Set ringingtype for Tts.
    iTtsRingingType = aAudioVideoRingingTone->RingingType();
    iTtsVolume = aAudioVideoRingingTone->Volume();

    // TTS should be a bit louder, than main component.
    // No need to check for overflows, they are handled in SET_PHONE_VOLUME macro.
    
    TPtrC ttsHeader( KPhoneTtsHeader );
    HBufC* preparedString = 
        HBufC::New( aTextToSay.Length() + ttsHeader.Length() );

    if ( preparedString )
        {
        TPtr pPreparedString = preparedString->Des();
        pPreparedString.Append( ttsHeader );
        pPreparedString.Append( aTextToSay );
           
        __PHONELOG1( 
            EBasic, 
            EPhoneControl, 
            "CPhoneTTSPlayer::PlayTtsTone - about to say %S",
            &pPreparedString );

        TRAPD( 
            error, 
            iTtsPlayer = CPhoneAudioPlayer::NewTtsL( 
               *preparedString, 
               KAudioPriorityPhoneCall, 
               KAudioPrefTextToSpeechCallHardCoded,
               *this, 
               CPhoneRingingtonePlayer::EPlayerTts ) );
        if( error == KErrNone ) 
            {
            // To be played when default or personal tone will be played
            iTtsToneToBePlayed = ETrue;
            }
        __PHONELOG2(
            EBasic,
            EPhoneControl,
            "CPhoneTTSPlayer::PlayTtsTone - error(%d), iTtsPhoneToBePlayed(%d)",
            error, 
            iTtsToneToBePlayed );

        delete preparedString;  
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::StopPlaying()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::StopPlaying()" );

    iTtsToneToBePlayed = EFalse;
    iTTsTimeOutCounter = 0;
    iTtsDelayTimer->Cancel();
    if ( iTtsPlayer )
        {
        iTtsPlayer->StopPlaying();
        delete iTtsPlayer;
        iTtsPlayer = NULL;
        }
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandleAudioPlayerError( 
    TPhoneAudioPlayerErrorEvent /*aEvent*/, 
    TInt /*aError*/, 
    TInt /*aId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::HandleAudioPlayerError()" );

    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandleAudioPlayerInitComplete( TInt /*aId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::HandleAudioPlayerInitComplete()" );
    // This is called from CPhoneAudioPlayer - we do not 
    // want to delete instances from which this method
    // is called. 

    if( iTtsToneToBePlayed )  
        {
        // Start TTS timer just before "normal ringtone component" starts 
        // playing, i.e. after init completed.
        if ( iTtsDelayIndex < iTtsDelaysCount )  
            {
            if ( !iTtsDelayTimer->IsActive() )
                {
                // There are TTS iterations to be played yet.
                iTtsDelayTimer->After( 
                    KPhoneTtsDelays[iTtsDelayIndex], 
                    TCallBack( HandleTtsDelayTimeout, this ) );
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::HandlePlayingComplete
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::HandlePlayingComplete( TInt aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete()" );
    // This is called from CPhoneAudioPlayer - we do not 
    // want to delete instances from which this method
    // is called. 

  /*  __PHONELOG2(
        EBasic, 
        EPhoneControl,
        "CPhoneTTSPlayer::HandlePlayingComplete - aId(%d), iRingingType(%d)",
        aId,
        iRingingType );*/
    __PHONELOG2(
        EBasic, 
        EPhoneControl,
        "CPhoneTTSPlayer::HandlePlayingComplete - iTtsToneToBePlayed(%d), iTtsDelayIndex(%d)",
        iTtsToneToBePlayed,
        iTtsDelayIndex );

    if ( aId == CPhoneRingingtonePlayer::EPlayerTts ) 
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - resuming after TTS" );

        // TTS playing complete, normal tone player should volume up.
        if ( ( iTtsToneToBePlayed ) && ( ++iTtsDelayIndex < iTtsDelaysCount ) )  
            {
            __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - resuming after TTS first time" );
            //Callers name is said once. increase tone player volume.    
            SolveNewVolumeAndRamptime( ESaidOnce );
            
            if ( !iTtsDelayTimer->IsActive() )
                {
                // There are TTS iterations to be played yet.
                iTtsDelayTimer->After( KPhoneTtsDelays[iTtsDelayIndex], 
                    TCallBack( HandleTtsDelayTimeout,this ) );
                }
            }
        else
            {
            __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - resuming after TTS second time" );
            SolveNewVolumeAndRamptime( ESaidTwice );
            }
        
        // No further processing. All TTS player-related events are not 
        // "real" ringtone playing completions.
        return;
        }
    //Case: RingintonePlayer has completed playing with following set: ring once and
    // TTS is activated   
   if ( iTtsRingingType == EProfileRingingTypeRingingOnce && 
        iTtsToneToBePlayed )   
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - ring once and TTS" );
         if ( iTtsDelayIndex < iTtsDelaysCount )
            {
            ReStartRingingTonePlayer();
            //Do not destroy player yet. Return.
            return;            
            }
        }
    //Case: RingintonePlayer has completed playing with following set: ringing 
    //and TTS is activated. We need  to restart ringintoneplayer and new TTS
    //iterations are required.
    else if ( iTtsRingingType == EProfileRingingTypeRinging && iTtsToneToBePlayed )
        {  
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - ringing and TTS" );
        if ( iTtsDelayIndex == iTtsDelaysCount )
            {
            //Case: Ringingtype is EProfileRingingTypeRinging. New TTS
            //iterations are required.    
            __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::HandlePlayingComplete - need to restart TTS sequence" );
            //TTS has completed its iterations,set index to zero. 
            iTtsDelayIndex = 0;
            if ( !iTtsDelayTimer->IsActive() )
                {
                //Restart TTS sequence
                iTtsDelayTimer->After( 
                    KPhoneTtsDelays[iTtsDelayIndex], 
                    TCallBack( HandleTtsDelayTimeout, this ) ); 
                }
            }
        
        ReStartRingingTonePlayer();
        //Do not destroy player yet. Return
        return;  
        }
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CPhoneTTSPlayer::HandleTtsDelayTimeout( TAny* object )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::HandleTtsDelayTimeout()" );
    static_cast<CPhoneTTSPlayer*>( object )->
        DoHandleTtsDelayTimeout();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::DoHandleTtsDelayTimeout()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::DoHandleTtsDelayTimeout()" );
    __PHONELOG2( EBasic, EPhoneControl, "CPhoneTTSPlayer::DoHandleTtsDelayTimeout - iTtsRingingType(%d), iTTsTimeOutCounter(%d)",
        iTtsRingingType, iTTsTimeOutCounter );

    CPhoneAudioPlayer* currPlayer = NULL;
    currPlayer = GetCurrentlyActiveAudioPlayerWithTTs();        
    
    if ( !currPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::DoHandleTtsDelayTimeout - null current player" );
        return;
        }
    TInt volume( 0 );
     //Ascending case. TTs player needs to be ascending.
    if ( iTtsRingingType == EProfileRingingTypeAscending )
        {
        //Volume needs to be different in different ascending steps
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::DoHandleTtsDelayTimeout - play ascending" ); 
        if ( !iTTsTimeOutCounter )
            {
            //We are saying callers name for the first time in ascending mode.
            //Say callers name by volume level KTtsVolumeMin.             
            volume = KTtsVolumeMin;
                
            iTtsPlayer->Play(
                ConvertRingingType( EProfileRingingTypeRingingOnce ),
                volume,
                iTtsToneToBePlayed );
            }
        else
            {
            //Checks the case that ringingtone is very quiet. Then do not play
            //TTS too loud
            volume = iTtsVolume < KTtsVolumeMin ? KTtsVolumeMin : KTtsVolumeAscendingRepeat;     
                           
            //We are saying callers name for the second time in ascending mode.
            //Say callers name by volume level KTtsVolumeAscendingRepeat and decrease current players volume
            //to KPlayerVolumeAscendingRepeat. RampTime is zero
            currPlayer->SetNewVolumeAndRamptime( KPlayerVolumeAscendingRepeat, 0 );
            iTtsPlayer->Play(
                ConvertRingingType( EProfileRingingTypeRingingOnce ),
                volume,
                iTtsToneToBePlayed );
            }
       
        }
    else //Normal ringing case.
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::DoHandleTtsDelayTimeout - play normal" ); 
        currPlayer->SetNewVolumeAndRamptime( iTtsVolume-5, 0 );                       
        iTtsPlayer->Play(
            ConvertRingingType( EProfileRingingTypeRingingOnce ),
            iTtsVolume,
            iTtsToneToBePlayed );
        }
      
     iTTsTimeOutCounter++;
    }

// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::AddTtsPlaybackIfNeeded
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::AddTtsPlaybackIfNeeded()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::AddTtsPlaybackIfNeeded()" );
    if ( iTtsToneToBePlayed )   
        {
        iTtsDelayIndex = 0;
        iTtsDelaysCount = KPhoneTtsDelaysCount;
        }
    }

// -----------------------------------------------------------------------------
// CPhoneTTSPlayer::SolveNewVolumeAndRamptime
// -----------------------------------------------------------------------------
//
void CPhoneTTSPlayer::SolveNewVolumeAndRamptime( TTtsStatus aStatus )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime()" );

    CPhoneAudioPlayer* currPlayer = NULL;
    currPlayer = GetCurrentlyActiveAudioPlayerWithTTs();
    if ( !currPlayer )
        {
        __PHONELOG( EBasic, EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime - null current player" );
        return;
        }
    TInt ramptime( 0 );
    //Here is handled the cases when Callers name is said. Sequence is:
    //3 sec. tone + name + 4 sec. tone + name +the tone until the end. 
    switch ( aStatus )
        {
        case ESaidOnce:
            if ( iTtsRingingType == EProfileRingingTypeAscending )
                {
                //The ramptime could be ( 4 )* KPhoneTtsAscendingStep but now
                //we are setting this to 0 because when Say callers name is said
                //for first time: play ringing tone on level 3 for four secs.
                ramptime = 0;
                __PHONELOG1( EBasic, EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime - said once ascending - ramptime(%d)",
                    ramptime ); 
                currPlayer->SetNewVolumeAndRamptime( KPlayerVolumeAscendingRepeat, ramptime );
                }
            else
                {
                 //Normal ringingtone case. Adjust volume back to profile level. 
                __PHONELOG1( EBasic, EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime - said once normal - ramptime(%d)",
                    ramptime ); 
                currPlayer->SetNewVolumeAndRamptime( iTtsVolume, 0 );    
                }
             break;
        
        case ESaidTwice:
            if ( iTtsRingingType == EProfileRingingTypeAscending )
                {
                TInt vol = iTtsVolume;
                if ( vol > KPlayerVolumeAscendingRepeat )
                    {
                    vol = vol - KTtsVolumeAscendingDecrease;
                    }
                          
                //TTS playing complete for second time. increase tone player volume.
                ramptime = ( vol )*KPhoneTtsAscendingStep;
                __PHONELOG1( EBasic, EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime - said twice ascending - ramptime(%d)",
                    ramptime ); 
                currPlayer->SetNewVolumeAndRamptime( iTtsVolume, ramptime );
                }
            else
                {
                 //Normal ringingtone case. Adjust volume back to profile level. 
                __PHONELOG1( EBasic, EPhoneControl, "CPhoneTTSPlayer::SolveNewVolumeAndRamptime - said twice normal - ramptime(%d)",
                    ramptime ); 
                currPlayer->SetNewVolumeAndRamptime( iTtsVolume, 0 );
                }
            break;
        
        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
CPhoneAudioPlayer* 
CPhoneTTSPlayer::GetCurrentlyActiveAudioPlayerWithTTs()
    {    
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::GetCurrentlyActiveAudioPlayerWithTTs()" );

    return iRingingtonePlayer->GetCurrentlyActiveAudioPlayer();
    }
    
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//      
void CPhoneTTSPlayer::ReStartRingingTonePlayer()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::ReStartRingingTonePlayer()" );

    //First get currently active player which plays among TTS player.
    CPhoneAudioPlayer* currPlayer = NULL;
    currPlayer = GetCurrentlyActiveAudioPlayerWithTTs();   
    if ( currPlayer )
        {
        currPlayer->ReStartPlaying();
        }      
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::TRingingType CPhoneTTSPlayer::ConvertRingingType(
    TProfileRingingType aRingingType )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneTTSPlayer::ConvertRingingType()" );

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

 

