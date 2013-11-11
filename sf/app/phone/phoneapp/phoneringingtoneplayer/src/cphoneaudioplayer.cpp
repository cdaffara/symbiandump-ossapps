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
#include <e32std.h>
#include <eikenv.h>
#include <apgcli.h>
#include <apmrec.h>
#include <AudioPreference.h>
#include <utf.h>    // for converting data for tts
#include <c3dringingtoneinterface.h>
#include <AudioOutput.h>

#include "cphoneaudioplayer.h"
#include "cphoneringingtone.h"
#include "mphoneaudioplayerobserver.h"
#include "phoneconstants.h"
#include "phonelogger.h"
#include "phoneui.pan"


// CONSTANTS


_LIT( KFileListRngMimeType, "application/vnd.nokia.ringing-tone" );
//Min volume level
const TInt KMinVolumeLevel = 1;
//Max volume level
const TInt KMaxVolumeLevel = 10;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::CPhoneAudioPlayer
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::CPhoneAudioPlayer( TAudioDataFormat aDataFormat,
    MPhoneAudioPlayerObserver& aObserver, 
    TInt aPriority, 
    TUint aPreference,
    TInt aId )
    : iFormat( aDataFormat ),
    iObserver( aObserver ),
    iPriority( aPriority ),
    iPreference( aPreference ),
    iId( aId )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::ConstructL
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::ConstructL(
    const TDesC& aFileName,
    CMdaServer* aMdaServer )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::ConstructL()" );
    if ( iFormat == EFormatTone )
        {
        iTonePlayer = CMdaAudioToneUtility::NewL( *this, aMdaServer );
        iTonePlayer->PrepareToPlayFileSequence( aFileName );
        iPlayerStatus = EToneLoading;
        }
    else
        {
        //audio sample player
        __PHONELOG3( 
            EBasic, 
            EPhoneControl, 
            "CPhoneAudioPlayer::ConstructL - aFileName(%S), iPriority(%d), iPreference(%d)",
            &aFileName, 
            iPriority, 
            iPreference );

        iSamplePlayer = CMdaAudioPlayerUtility::NewFilePlayerL( 
            aFileName,
            *this, 
            iPriority,
            static_cast< TMdaPriorityPreference >( iPreference ),
            aMdaServer );            
        iPlayerStatus = EToneLoading;
        
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::ConstructSeqL
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::ConstructSeqL( 
    const TDesC8& aSequence,
        CMdaServer* aMdaServer )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::ConstructSeqL()" );
    iTonePlayer = CMdaAudioToneUtility::NewL( *this, aMdaServer );
    iSequence = aSequence.AllocL();
    iTonePlayer->PrepareToPlayDesSequence( *iSequence );
    iPlayerStatus = EToneLoading;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::ConstructTtsL
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::ConstructTtsL( 
    const TDesC& aTtsText, 
    TInt aPriority,
    TUint aPreference )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::ConstructTtsL()" );
    delete iTtsPlayer; // if any
    iTtsPlayer = NULL;
    iTtsPlayer = 
        CMdaAudioPlayerUtility::NewL( 
            *this, aPriority, 
            static_cast< TMdaPriorityPreference >( aPreference ) );
    
    delete iTtsText;  // if any
    iTtsText = NULL;
    // UTF-8 strings can take up to 4 bytes per character
    iTtsText = HBufC8::NewL( aTtsText.Length() << KTimesToMultiply ); 
    
    TPtr8 refText = iTtsText->Des(); 
    User::LeaveIfError( 
        CnvUtfConverter::ConvertFromUnicodeToUtf8( refText, aTtsText ) );

    // UTF-8 chars can be up to 4 bytes long, but usually 
    // take 1-2 bytes, 3 for asian chars.
    HBufC8* oldText = iTtsText;
    iTtsText = iTtsText->ReAlloc( iTtsText->Length() );
    if ( !iTtsText )
        {
        // ReAlloc failed, set back to original.
        iTtsText = oldText;
        }
    
    iTtsPlayer->OpenDesL( *iTtsText );
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::New
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneAudioPlayer::New( 
    const CPhoneRingingTone& aRingingTone, 
    TInt aPriority, 
    TUint aPreference,
    MPhoneAudioPlayerObserver& aObserver, 
    TInt aId,
    CMdaServer* aMdaServer,
    TBool aExtSecNeeded )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::New()" );
    CPhoneAudioPlayer* self = NULL;
    TRAPD( err, self = CPhoneAudioPlayer::NewL( 
        aRingingTone,
        aPriority,
        aPreference,
        aObserver,
        aId,
        aMdaServer,
        aExtSecNeeded ) );
    if ( err != KErrNone )
        {
        return NULL;
        }
    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::NewL
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneAudioPlayer::NewL( 
    const CPhoneRingingTone& aRingingTone, 
    TInt aPriority,
    TUint aPreference,
    MPhoneAudioPlayerObserver& aObserver,
    TInt aId,
    CMdaServer* aMdaServer,
    TBool aExtSecNeeded )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::NewL()" );
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneAudioPlayer::NewL - aExtSecNeeded(%d)",
        aExtSecNeeded );
    
    // Check the file DRM property if extend secure is needed.
    if ( aExtSecNeeded )
        {
        if ( !aRingingTone.IsFileInRom() &&
             !aRingingTone.IsFileDrmProtected() )
            {
            __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::NewL - DRM extend security check permission denied" );
            User::Leave( KErrPermissionDenied );
            }
        __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::NewL - DRM extend security check ok" );
        }
    // RNG file types have to be played with CMdaAudioToneUtility player.
    // Otherwise use sample player
    TAudioDataFormat format = 
    ( aRingingTone.MimeType().CompareF( KFileListRngMimeType ) == KErrNone ) 
    ? EFormatTone :EFormatSample;

    CPhoneAudioPlayer* self = 
        new (ELeave) CPhoneAudioPlayer( 
            format,
            aObserver, 
            aPriority,
            aPreference,
            aId );
    
    CleanupStack::PushL( self );
    self->ConstructL( aRingingTone.FileName(), aMdaServer );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::NewL
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneAudioPlayer::NewL( 
    const TDesC& aFileName, 
    TInt aPriority,
    TUint aPreference,
    MPhoneAudioPlayerObserver& aObserver,
    TInt aId,
    CMdaServer* aMdaServer,
    TBool aExtSecNeeded )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::NewL()" );
    CPhoneRingingTone* tone = CPhoneRingingTone::NewL( aFileName );
    CleanupStack::PushL( tone );

    CPhoneAudioPlayer* player = CPhoneAudioPlayer::NewL( *tone,
                                                         aPriority,
                                                         aPreference,
                                                         aObserver,
                                                         aId,
                                                         aMdaServer,
                                                         aExtSecNeeded );

    CleanupStack::PopAndDestroy( tone );
    
    return player;        
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::NewSeqL
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneAudioPlayer::NewSeqL(
    const TDesC8& aSequence,
    TInt aPriority,
    TUint aPreference,
    MPhoneAudioPlayerObserver& aObserver, 
    TInt aId,
    CMdaServer* aMdaServer )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::NewSeqL()" );
    CPhoneAudioPlayer* self = 
        new (ELeave) CPhoneAudioPlayer( 
            EFormatTone, 
            aObserver, 
            aPriority,
            aPreference,
            aId );
    
    CleanupStack::PushL( self );
    self->ConstructSeqL( aSequence, aMdaServer  );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::NewTtsL
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer* CPhoneAudioPlayer::NewTtsL(
    const TDesC& aText,
    TInt aPriority,
    TUint aPreference,
    MPhoneAudioPlayerObserver& aObserver, 
    TInt aId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::NewTtsL()" );
    CPhoneAudioPlayer* self = 
        new (ELeave) CPhoneAudioPlayer( 
            EFormatTts, 
            aObserver, 
            aPriority,
            aPreference,
            aId );
    
    CleanupStack::PushL( self );
    self->ConstructTtsL( aText, aPriority, aPreference );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::~CPhoneAudioPlayer
// -----------------------------------------------------------------------------
//
CPhoneAudioPlayer::~CPhoneAudioPlayer()
    {
    
    if ( iAudioOutput )
        {
        delete iAudioOutput;    
        }  
    if ( iTonePlayer )  
        {
        delete iTonePlayer;  
        } 
    if ( i3DPlugin )
       {
        i3DPlugin->Stop();
        delete i3DPlugin;
        }
    // ECom cleanup
    REComSession::FinalClose();
    if ( iSamplePlayer )    
        {
        delete iSamplePlayer; 
        }
    if ( iTtsPlayer )
        {
        delete iTtsPlayer;   
        }
    if ( iSequence )
        {
        delete iSequence; 
        }
    if ( iTtsText )  
        {
        delete iTtsText;
        }   
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::Play
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::Play( TRingingType aRingType, TInt aVolume, TInt aTTsToBePlayed )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::Play()" );
    __PHONELOG2( 
        EBasic, 
        EPhoneControl, 
        "CPhoneAudioPlayer::Play - iFormat(%d), iPlayerStatus(%d)",
        iFormat, 
        iPlayerStatus );
    __PHONELOG3( 
        EBasic, 
        EPhoneControl, 
        "CPhoneAudioPlayer::Play - aRingType(%d), aVolume(%d), aTTsToBePlayed(%d)",
        aRingType, 
        aVolume,
        aTTsToBePlayed );
    iRingType = aRingType;
    iVolume = aVolume;
    iTTsToBePlayed = aTTsToBePlayed;
    
    if ( iFormat == EFormatTone )
        {
        switch ( iPlayerStatus )
            {
            case ETonePlaying:
                iTonePlayer->CancelPlay();
                DoPlay();
                break;

            case EToneReady:
                iPlayerStatus = ETonePlaying;
                DoPlay();
                break;

            case EToneLoading:
                iToBePlaying = ETrue;
                break;

            default:
                __ASSERT_DEBUG( EFalse, 

                      Panic( EPhoneViewCaseNotHandled ) );
            }
        }
    else if ( iFormat == EFormatTts )
        {
        switch ( iPlayerStatus )
            {
            case ETonePlaying:
                //stop previous playing
                iTtsPlayer->Stop();
                DoPlay();
                break;

            case EToneReady:
                iPlayerStatus = ETonePlaying;
                DoPlay();
                break;

            case EToneLoading:
                iToBePlaying = ETrue;
                break;

            default:
                __ASSERT_DEBUG( EFalse, 
                        Panic( EPhoneViewCaseNotHandled ) );
            }
        }
    else
        {
        switch ( iPlayerStatus )
            {
            case ETonePlaying:
                //stop previous playing
                StopPlaying();
                iPlayerStatus = ETonePlaying;
                DoPlay();
                break;

            case EToneReady:
                iPlayerStatus = ETonePlaying;
                DoPlay();
                break;

            case EToneLoading:
                iToBePlaying = ETrue;
                break;

            default:
                __ASSERT_DEBUG( EFalse, 

                        Panic( EPhoneViewCaseNotHandled ) );
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::ReStartPlaying
// -----------------------------------------------------------------------------
//    
void CPhoneAudioPlayer::ReStartPlaying()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::ReStartPlaying()" );
    __PHONELOG3( 
        EBasic, 
        EPhoneControl, 
        "CPhoneAudioPlayer::ReStartPlaying - iFormat(%d), iRingType(%d), iVolume(%d)",
        iFormat, 
        iRingType, 
        iVolume );
        
    if ( iFormat == EFormatTone )
        {
        iTonePlayer->Play();
        iPlayerStatus = ETonePlaying;
        }
    else if ( iFormat == EFormatTts )
        {
        iTtsPlayer->Play();
        iPlayerStatus = ETonePlaying;
        }        
    else
        {
        iSamplePlayer->Play();
        iPlayerStatus = ETonePlaying;
        }    
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::StopPlaying
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::StopPlaying()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::StopPlaying()" );
    iToBePlaying = EFalse;
    
    if ( iFormat == EFormatTone )
        {
        if ( EMdaAudioToneUtilityPrepared == iTonePlayer->State() )
            {
            iTonePlayer->CancelPrepare();
            }
        else if ( EMdaAudioToneUtilityPlaying == iTonePlayer->State() )
            {
            iTonePlayer->CancelPlay();
            }
        }
    else if ( iFormat == EFormatTts )
        {
        iTtsPlayer->Stop();
        }
    else
        {
        if ( i3DPlugin )
            {
            i3DPlugin->Stop();
            delete i3DPlugin;
            i3DPlugin = NULL;
            }
        else
            {
            iSamplePlayer->Stop();        
            }
        }
    
    iPlayerStatus = EToneReady;
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::MapcInitComplete
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::MapcInitComplete(
    TInt aError, 
    const TTimeIntervalMicroSeconds& /*aDuration*/ )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::MapcInitComplete()" );
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneAudioPlayer::MapcInitComplete - aError(%d)",
        aError );
    __ASSERT_DEBUG( ( ( iFormat == EFormatSample ) || ( iFormat == EFormatTts ) ) && 
        ( iPlayerStatus == EToneLoading ), Panic( EPhoneViewGeneralError ) );
    if ( aError == KErrNone )
        {
        if ( iToBePlaying )
            {
            iPlayerStatus = ETonePlaying;
            iToBePlaying = EFalse;
            DoPlay();
            }
        else
            {
            iPlayerStatus = EToneReady;
            }
        iObserver.HandleAudioPlayerInitComplete( iId );
        }
    else
        {
        iObserver.HandleAudioPlayerError( 
            MPhoneAudioPlayerObserver::EAudioPlayerInitializingFailure, 
            aError, iId );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::MapcPlayComplete
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::MapcPlayComplete( TInt aError )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::MapcPlayComplete()" );
    __PHONELOG2( EBasic, EPhoneControl, "CPhoneAudioPlayer::MapcPlayComplete - iFormat(%d), aError(%d)",
        iFormat, aError );
    iPlayerStatus = EToneReady;

    //Destruct 3D plugin before player itself gets destructed.
    //In case of continuous ringing type, stopplaying() handles
    //3d plugin destruction
    if ( i3DPlugin )
        {
        i3DPlugin->Stop();
        delete i3DPlugin;
        i3DPlugin = NULL;            
        }

    if ( aError != KErrNone )
        {
        iObserver.HandleAudioPlayerError( 
            MPhoneAudioPlayerObserver::EAudioPlayerPlayingFailure, 
            aError, iId );
        }
    else
        {
        iObserver.HandlePlayingComplete( iId );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::MatoPrepareComplete
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::MatoPrepareComplete( TInt aError )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::MatoPrepareComplete()" );
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneAudioPlayer::MatoPrepareComplete - aError(%d)",
        aError );
        
    __ASSERT_DEBUG( ( iFormat == EFormatTone ) && ( iPlayerStatus == EToneLoading ), 
        Panic( EPhoneViewGeneralError ) );
    if ( aError == KErrNone )
        {
        if ( iToBePlaying )
            {
            iPlayerStatus = ETonePlaying;
            iToBePlaying = EFalse;
            DoPlay();
            }
        else
            {
            iPlayerStatus = EToneReady;
            }
        iObserver.HandleAudioPlayerInitComplete( iId );
        }
    else
        {
        iObserver.HandleAudioPlayerError( 
            MPhoneAudioPlayerObserver::EAudioPlayerInitializingFailure, 
            aError, iId );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::MatoPlayComplete
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::MatoPlayComplete(TInt aError)
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::MatoPlayComplete()" );
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneAudioPlayer::MatoPlayComplete - aError(%d)",
        aError );
    if ( aError != KErrNone )
        {
        iObserver.HandleAudioPlayerError( 
            MPhoneAudioPlayerObserver::EAudioPlayerPlayingFailure, 
            aError, iId );
        }
    else
        {
        iObserver.HandlePlayingComplete( iId );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::DoPlay
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::DoPlay()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::DoPlay()" );
    __ASSERT_DEBUG( iPlayerStatus == ETonePlaying, 
        Panic( EPhoneViewGeneralError ) );
        
    SetRingingTypeProperties();
    
    TInt err(KErrNone);
    if ( iFormat == EFormatTone )
        {
        iTonePlayer->Play();
        }
    else if ( iFormat == EFormatTts )
        {
        iTtsPlayer->Play();
        }        
    else
        {
        TRAP( err,i3DPlugin = C3DRingingToneInterface::NewL( KNullUid ) );
        if ( !err && i3DPlugin )
            {
            i3DPlugin->SetAttr( E3DRTIAttrAudioPlayerUtility, iSamplePlayer );
            TRAP( err,i3DPlugin->PlayL() )
            }
        if ( err || !i3DPlugin )
            {
            if ( !iAudioOutput )
                 {
                 __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::DoPlay c aud ");
                 TRAP (err, iAudioOutput = CAudioOutput::NewL( *iSamplePlayer ) );
                if ( err )
                    {
                    __PHONELOG1(EBasic, EPhoneControl, 
                        "P.AudPlayer.DoPlay.iAudioOutput.err ", err );
                    }                               
               }
            if ( !err && iAudioOutput )
                 {
                 __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::DoPlay EALL ");
                 TRAP (err, iAudioOutput->SetAudioOutputL( CAudioOutput::EAll ) );
                 if ( err )
                     {
                     __PHONELOG1( EBasic, EPhoneControl, 
                         "P.AudPlayer.DoPlay.SetAudioOutputL.err ", err );   
                     }

                 }
            iSamplePlayer->Play();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::SetRingingTypeProperties
// -----------------------------------------------------------------------------
//
void CPhoneAudioPlayer::SetRingingTypeProperties()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::SetRingingTypeProperties()" );
    __PHONELOG2( EBasic, EPhoneControl, "CPhoneAudioPlayer::SetRingingTypeProperties - iFormat(%d), iRingType(%d)",
        iFormat, iRingType );
    TInt rampTime( 0 );     
    if ( iFormat == EFormatTone )
        {
        switch ( iRingType )
            {
            case ETypeRinging:
                //If we have TTS activated and ringingtype is ringing:
                //We need to play TTS sequence again when ringingtone restarts.
                //Thats why we need to set ringingtype to ETypeRingingOnce because  
                //it is the only way of knowing when ringingtone is completed. 
                //Then we can restart it with new TTS iterations.
                if ( iTTsToBePlayed )
                    {
                     //play only once
                    iTonePlayer->SetRepeats( 0, 
                        TTimeIntervalMicroSeconds(
                        KPhoneRingingRepeatsTrailPause ) ); 
                    iTonePlayer->SetVolume( ConvertVolume( iVolume ) );
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ringing tone with TTS, Set volume(%d) and ring once",
                        iVolume );
                    }
                else
                    {
                    iTonePlayer->SetRepeats( KPhoneMdaAudioToneRepeatForever,
                        TTimeIntervalMicroSeconds( 
                         KPhoneRingingRepeatsTrailPause ) );
                    iTonePlayer->SetVolume( ConvertVolume( iVolume ) );
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ringing tone, Set volume(%d)",
                        iVolume );
                    }
                break;

            case ETypeRingingOnce:
                //play only once
                iTonePlayer->SetRepeats( 0, 
                    TTimeIntervalMicroSeconds(
                    KPhoneRingingRepeatsTrailPause ) ); 
                iTonePlayer->SetVolume( ConvertVolume( iVolume ) );
                __PHONELOG1( 
                    EBasic, 
                    EPhoneControl, 
                    "CPhoneAudioPlayer::SetRingingTypeProperties - Ring once tone, set volume(%d)",
                    iVolume );
                break;

            case ETypeAscending:
                iTonePlayer->SetRepeats( KPhoneMdaAudioToneRepeatForever,
                    TTimeIntervalMicroSeconds(
                    KPhoneRingingRepeatsTrailPause ) );

                //Special case for TTs + ascending profile. Volume 
                //and ramptime needs to be different as usual
                if ( iTTsToBePlayed )
                    {
                    //rampTime in this case is only 3 seconds and volume is 1.
                    rampTime = KPhoneAudioAscendingRampDuration;
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ascending tone with TTS, Start from lowest volume, rampTime(%d)",
                        rampTime );
                    iTonePlayer->SetVolumeRamp( 
                        TTimeIntervalMicroSeconds( rampTime ) ); 

                    iCurrentVolume = 1; //we start from the lowest volume
                    iTonePlayer->SetVolume( ConvertVolume( iCurrentVolume ) );
                    }
                else
                    {
                    //rampTime: time for one step * volume level
                    rampTime = KPhoneAudioAscendingRampDuration*iVolume;
                    __PHONELOG2( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ascending tone, Set volume(%d), rampTime(%d)",
                        iVolume, rampTime );
                    iTonePlayer->SetVolumeRamp( 
                        TTimeIntervalMicroSeconds( rampTime ) );
                        
                    iCurrentVolume = 1; //we start from the lowest volume
                    iTonePlayer->SetVolume( ConvertVolume( iVolume ) );       
                    }
                break;

            default:
                Panic( EPhoneViewGeneralError );
                break;
            }

        __PHONELOG3( 
            EBasic, 
            EPhoneControl, 
            "CPhoneAudioPlayer::SetRingingTypeProperties - Tone, iPriority(%d), iPreference(%d), iVolume(%d)",
            iPriority, iPreference, iVolume );

        //Set priority and preference
        iTonePlayer->SetPriority( iPriority, 
            static_cast< TMdaPriorityPreference >( iPreference ) );
        }
    else if ( iFormat == EFormatTts )
        {
        switch ( iRingType )
            {
            case ETypeRinging:
            case ETypeAscending:
                // Not possible iRingType for TTS.
                break;

            case ETypeRingingOnce:
                iTtsPlayer->SetRepeats( 0, // play only once
                    TTimeIntervalMicroSeconds( 
                    KPhoneRingingRepeatsTrailPause ) );
                iTtsPlayer->SetVolume( ConvertVolume( iVolume ) );
                __PHONELOG1( 
                    EBasic, 
                    EPhoneControl, 
                    "CPhoneAudioPlayer::SetRingingTypeProperties - TTS, Set volume(%d)",
                    iVolume );
                break;
            
            default:
                __ASSERT_DEBUG( EFalse, 
                        Panic( EPhoneViewCaseNotHandled ) );
            }
        __PHONELOG( 
            EBasic, 
            EPhoneControl, 
            "CPhoneAudioPlayer::SetRingingTypeProperties - TTS" );
        }
    else
        {
        switch ( iRingType )
            {
            case ETypeRinging:
                //If we have TTS activated and ringingtype is ringing:
                //We need to play TTS sequence again when ringingtone restarts.
                //Thats why we need to set ringingtype to ETypeRingingOnce because  
                //it is theonly way of knowing when ringingtone is completed. 
                //Then we can restartit with new TTS iterations.
                if ( iTTsToBePlayed )
                    {
                     //play only once
                    iSamplePlayer->SetRepeats( 0, 
                        TTimeIntervalMicroSeconds(
                        KPhoneRingingRepeatsTrailPause ) ); 
                    iSamplePlayer->SetVolume( ConvertVolume( iVolume ) );
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ringing sample with TTS, Ring only once, Set volume(%d)",
                        iVolume );
                    }
                else
                    {
                    iSamplePlayer->SetRepeats( KPhoneMdaAudioToneRepeatForever,
                        TTimeIntervalMicroSeconds( 
                        KPhoneRingingRepeatsTrailPause ) );
                    iSamplePlayer->SetVolume( ConvertVolume( iVolume ) );
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ringing sample, Set volume(%d)",
                        iVolume );
                    }
                break;

            case ETypeRingingOnce:
                iSamplePlayer->SetRepeats( 0,//play only once
                    TTimeIntervalMicroSeconds( 
                    KPhoneRingingRepeatsTrailPause ) ); 
                iSamplePlayer->SetVolume( ConvertVolume( iVolume ) );
                __PHONELOG1( 
                    EBasic, 
                    EPhoneControl, 
                    "CPhoneAudioPlayer::SetRingingTypeProperties - Ring once sample, set volume(%d)",
                    iVolume );
                break;

            case ETypeAscending:
                iSamplePlayer->SetRepeats( KPhoneMdaAudioToneRepeatForever,
                    TTimeIntervalMicroSeconds(
                    KPhoneRingingRepeatsTrailPause ) );

                //Special case for TTs + ascending profile. Volume 
                //and ramptime needs to be different as usual
                if ( iTTsToBePlayed )
                    {
                    //rampTime in this case is only 3 seconds and volume is 1.
                    rampTime = KPhoneAudioAscendingRampDuration;
                    __PHONELOG1( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ascending sample with TTS, Start from lowest volume, rampTime(%d)",
                        rampTime );
                    iSamplePlayer->SetVolumeRamp( 
                        TTimeIntervalMicroSeconds( rampTime ) ); 

                    iCurrentVolume = 1; //we start from the lowest volume
                    iSamplePlayer->SetVolume( ConvertVolume( iCurrentVolume ) );
                    }
                else
                    {
                    //rampTime: time for one step * volume level
                    rampTime = KPhoneAudioAscendingRampDuration*iVolume;
                    __PHONELOG2( 
                        EBasic, 
                        EPhoneControl, 
                        "CPhoneAudioPlayer::SetRingingTypeProperties - Ascending sample, Set volume(%d), rampTime(%d)",
                        iVolume, rampTime );
                    iSamplePlayer->SetVolumeRamp( 
                        TTimeIntervalMicroSeconds( rampTime ) );
                        
                    iCurrentVolume = 1; //we start from the lowest volume
                    iSamplePlayer->SetVolume( ConvertVolume( iVolume ) );
                    }
                break;

            default:
                Panic( EPhoneViewGeneralError );
                break;
            }

        __PHONELOG( 
            EBasic, 
            EPhoneControl, 
            "CPhoneAudioPlayer::SetRingingTypeProperties - Sample" );
        }
    }

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::SetNewVolumeAndRamptime
// -----------------------------------------------------------------------------
//    
void CPhoneAudioPlayer::SetNewVolumeAndRamptime( TInt aVolume, TInt aRamptime )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneAudioPlayer::SetNewVolumeAndRamptime()" );
   
    //Check that volume is in valid range.
   TInt volume = aVolume<1 ? 1:aVolume;
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneAudioPlayer::SetNewVolumeAndRamptime - aVolume(%d)",
        aVolume ); 
            
    if ( iFormat == EFormatTone && iTonePlayer )
        {
        iTonePlayer->SetVolumeRamp(TTimeIntervalMicroSeconds( aRamptime ) );
        iTonePlayer->SetVolume( ConvertVolume( volume ) );
        __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::SetNewVolumeAndRamptime - Tone player" );
        }
    else if ( iFormat == EFormatTts && iTtsPlayer )
        {
        iTtsPlayer->SetVolumeRamp(TTimeIntervalMicroSeconds( aRamptime ) );
        iTtsPlayer->SetVolume( ConvertVolume( volume ) );
        __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::SetNewVolumeAndRamptime - TTS player" );
        }
    else 
        {
        iSamplePlayer->SetVolumeRamp(TTimeIntervalMicroSeconds( aRamptime ) );
        iSamplePlayer->SetVolume( ConvertVolume( volume ) );
        __PHONELOG( EBasic, EPhoneControl, "CPhoneAudioPlayer::SetNewVolumeAndRamptime - Sample player" );
        }
   
    }
   

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::ConvertVolume
// -----------------------------------------------------------------------------
//  
TInt CPhoneAudioPlayer::ConvertVolume( TInt aVolume )
    {
    //_DDPRINT( 5, "P.AudPlayer.ConvertVolume.aVolume", aVolume );

    TInt result( 0 );    
    
    if ( iFormat == EFormatTone && iTonePlayer)
        {
        result = iTonePlayer->MaxVolume() * aVolume / KMaxVolumeLevel;
        }
    else if ( iFormat == EFormatTts && iTtsPlayer )
        {
        result = iTtsPlayer->MaxVolume() * aVolume / KMaxVolumeLevel;    
        }
    else
        {
        result = iSamplePlayer->MaxVolume() * aVolume / KMaxVolumeLevel;       
        }
        
    // if user has selected minimum volume level
    // set HW volume 1
    if ( aVolume == KMinVolumeLevel && result == 0 )
        {
        result = 1; 
        }
       
    //_DDPRINT( 5, "P.AudPlayer.ConvertVolume.result", result );       
    return result;    
    }

       
// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::SetTTsToBePlayed
// -----------------------------------------------------------------------------
//  
void CPhoneAudioPlayer::SetTTsToBePlayed( TBool aTTsToBePlayed )
    {
    iTTsToBePlayed = aTTsToBePlayed;
    }  

// -----------------------------------------------------------------------------
// CPhoneAudioPlayer::MutePlaying
// -----------------------------------------------------------------------------
//  
void CPhoneAudioPlayer::MutePlaying()
    {
    //_DPRINT( 5, "P.Aud.Mute" );
    if ( iPlayerStatus == ETonePlaying )
        {
        if ( iFormat == EFormatTone )
            {
            iTonePlayer->SetVolume(0);
            }
        else if ( iFormat == EFormatTts )
            {
            iTtsPlayer->SetVolume(0);
            }
        else // EFormatSample
            {
            iSamplePlayer->SetVolume(0);        
            }
        }
    else
        {
        // Mute called during EToneLoading state.
        iToBePlaying = EFalse;    
        }                   
    }
   
//  End of File  
