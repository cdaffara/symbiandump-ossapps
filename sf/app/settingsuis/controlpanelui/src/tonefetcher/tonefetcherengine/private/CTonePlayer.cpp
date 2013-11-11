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
 *     The source file for tone playing.
 *     
 */
#include "CTonePlayer.h"
#include "tonefetcherutils.h"
#include <AudioPreference.h>             // KAudioPriorityPreview
#include <c3dringingtoneinterface.h>     // C3DRingingToneInterface
#include <ctsydomainpskeys.h>            // for phone call states
#include <MProfileEngine.h>
#include <MProfile.h>
#include <MProfileTones.h>
#include "TProfileToneSettings.h"
#include <MProfileExtraSettings.h>
#include <MProfile3DToneSettings.h>
#include <ProfileInternal.hrh>
#include <ProfileEngineDomainCRKeys.h>   // KProEngDefaultRingingTone
#include "MTonePlayingWatcher.h"


CMFPreviewHandlerBase::CMFPreviewHandlerBase()
    {
    iMediaType = KErrNotFound;
    iRingingVolume = KErrNotFound;
    iRingingType = KErrNotFound;
    iVibra = KErrNotFound;
    i3DEffect = KErrNotFound;
    i3DEcho = KErrNotFound;
    iFileSize = KErrNotFound;    
    iFullName = 0;    
    iActiveProfileRead = EFalse;
    iPlayerStatus = EPlayerNotCreated;

    }

void CMFPreviewHandlerBase::ConstructL()
    {
    // To allow/not allow screensaver
    // Errors ignored, no actions needed if API is not available
    //iProperty.Attach( KPSUidScreenSaver, KScreenSaverAllowScreenSaver );    
    TRAP_IGNORE(User::LeaveIfError( iApaSession.Connect() ) );
    
    TRAP_IGNORE( ReadDefaultToneL() );
    // To keep backlight on while a video is being previewed
    iBacklightTimer = CPeriodic::NewL( EPriorityLow );
    }

CMFPreviewHandlerBase::~CMFPreviewHandlerBase()
    {
    delete iFullName;
    iProperty.Close();
    
    iApaSession.Close();
    }

void CMFPreviewHandlerBase::SetAttrL( const TDesC& aFileName )
    {
    if ( aFileName.Length() )
        {        
        delete iFullName;
        iFullName = 0;
        iFullName = aFileName.AllocL();      
        }
    }

TInt CMFPreviewHandlerBase::RingingVolume()
    {
    const TInt KDefaultVolumeLevel = 7; // see profile.hrh for volume levels
    
    if ( iRingingVolume != KErrNotFound )
        {
        return iRingingVolume;
        }
        
    if ( iActiveProfileRead )
        {
        return iActiveProfileRingingVolume;
        }

    return KDefaultVolumeLevel;
    }

TInt CMFPreviewHandlerBase::RingingType()
    {
    if ( iRingingType != KErrNotFound )
        {
        return iRingingType;
        }
        
    if ( iActiveProfileRead )
        {
        return iActiveProfileRingingType;
        }

    return ERingingTypeRinging;
    }

TInt CMFPreviewHandlerBase::Vibra()
    {
    if ( iVibra != KErrNotFound )
        {
        return iVibra;
        }
        
    if ( iActiveProfileRead )
        {
        return iActiveProfileVibra;
        }
    
    return 0;  // in case of error vibra is off
    }

TInt CMFPreviewHandlerBase::Echo3D()
    {
    if ( i3DEcho != KErrNotFound )
        {
        return i3DEcho;
        }
    
    if ( iActiveProfileRead )
        {
        return iActiveProfile3DEcho;
        }

    return EProfile3DEchoOff;  // from ProfileInternal.hrh
    }

TInt CMFPreviewHandlerBase::Effect3D()
    {
    if ( i3DEffect != KErrNotFound )
        {
        return i3DEffect;
        }
    
    if ( iActiveProfileRead )
        {
        return iActiveProfile3DEffect;
        }

    return EProfile3DEffectOff;
    }

TInt CMFPreviewHandlerBase::ConvertVolume( TInt aVolume, TInt aMaxVolume )
    {
    const TInt KMinVolumeLevel = 1;
    const TInt KMaxVolumeLevel = 10;
        
    TInt result = aMaxVolume * aVolume / KMaxVolumeLevel;
    
    // if user has selected minimum volume level set HW volume 1
    if ( aVolume == KMinVolumeLevel && result == 0 )
        {
        result = 1;
        }

    return result;
    }
  
void CMFPreviewHandlerBase::ReadActiveProfileL()
    {
    iActiveProfileRead = EFalse;
    
    MProfileEngine* profileEngine = CreateProfileEngineL();
    CleanupReleasePushL( *profileEngine );
    
    MProfile* activeProfile  = profileEngine->ActiveProfileL();
    CleanupReleasePushL( *activeProfile );
        
    const MProfileTones& profileTones = activeProfile->ProfileTones();
     
    const TProfileToneSettings& toneSettings = profileTones.ToneSettings();
    iActiveProfileVibra = toneSettings.iVibratingAlert;
    iActiveProfileRingingVolume = toneSettings.iRingingVolume;
    iActiveProfileRingingType = toneSettings.iRingingType;
          
    const MProfileExtraSettings& extra = activeProfile->ProfileExtraSettings();
    const MProfile3DToneSettings& threeD = extra.Profile3DToneSettings();
    
    iActiveProfile3DEffect = threeD.Effect();
    iActiveProfile3DEcho = threeD.Echo();
   
    CleanupStack::PopAndDestroy( activeProfile ); 
    CleanupStack::PopAndDestroy( profileEngine );
    
    iActiveProfileRead = ETrue;
    }
 
void CMFPreviewHandlerBase::ReadDefaultToneL()
    {
    CRepository* cenrep = CRepository::NewLC( KCRUidProfileEngine );

    User::LeaveIfError( cenrep->Get( KProEngDefaultRingingTone, iDefaultTone ) );
    CleanupStack::PopAndDestroy( cenrep );
    }

TInt CMFPreviewHandlerBase::GetDataType( const TDesC& aFileName, TDataType& aDataType )
    {
    TUid dummyUid( KNullUid );
    return iApaSession.AppForDocument( aFileName, dummyUid, aDataType );
    }

TInt CMFPreviewHandlerBase::DoResetInactivityTimer( TAny* /*aObject*/ )
    {
    User::ResetInactivityTime();
    return KErrNone;
    }

void CMFPreviewHandlerBase::DisableBackLight()
    {
        const TInt KResetInactivityTimerDelay = 2000000;
    iBacklightTimer->Cancel(); // Just in case
    // Disable backlight turn off during video preview
    iBacklightTimer->Start( KResetInactivityTimerDelay,
                            KResetInactivityTimerDelay,
                            TCallBack( DoResetInactivityTimer, 0 ) );
    
    }

CTonePlayer* CTonePlayer::NewL( MTonePlayingWatcher *aWatcher )
    {
    CTonePlayer* self = CTonePlayer::NewLC( aWatcher );
    CleanupStack::Pop();
    return self;
    }

CTonePlayer* CTonePlayer::NewLC( MTonePlayingWatcher *aWatcher )
    {
    CTonePlayer* self = new ( ELeave ) CTonePlayer( aWatcher );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

void CTonePlayer::ConstructL()
    {
    iAudioPlayerStatus = EPlayerNotCreated;
    CMFPreviewHandlerBase::ConstructL();
    iTonePlayerStatus = EPlayerNotCreated;
    CCoeEnv* coeEnv = CCoeEnv::Static();
    coeEnv->AddForegroundObserverL( *this );
    }

CTonePlayer::CTonePlayer( MTonePlayingWatcher *aWatcher ) : iTonePlayWatcher( aWatcher )
    {    
    }

CTonePlayer::~CTonePlayer()
    {
    Cancel();
    
    delete iAudioPlayer;
    delete iTonePlayer;
    delete i3dRingingTonePlugin;
    }

TBool CTonePlayer::IsPlaying()
    {
    if ( iAudioPlayerStatus != EPlayerNotCreated )
        {
        return ETrue;
        }
    
    if ( iTonePlayerStatus != EPlayerNotCreated )
        {
        return ETrue;
        }

    return EFalse;
    }

void CTonePlayer::PlayL()
    {    
    //sequence for playing a beep once sound
    _LIT8( KFileListBeepSequence, "\x00\x11\x06\x0A\x08\x73\x0A\x40\x28\x0A\xF7\
    \x05\xFC\x40\x64\x0A\x08\x40\x32\x0A\xF7\x06\x0B" ); 

    // rng mime type
    _LIT( KFileListRngMimeType, "application/vnd.nokia.ringing-tone" );
    
    Cancel(); // stop previous play
    
    if ( !iFullName || iFullName->Des().Length() == 0 )
        {
        User::Leave( KErrNotFound );
        }

    TRAP_IGNORE( ReadActiveProfileL() );

    TPtrC fileName( iFullName->Des() );
    TDataType dataType;
    TInt err = GetDataType( fileName, dataType );
    if ( err == KErrNotFound )
        {
        fileName.Set( iDefaultTone );
        if ( fileName.Length() == 0 )
             {
             User::Leave( KErrNotFound );
             }
        }
    else if ( err != KErrNone )
        {
        User::Leave( err );
        }
        
    TBool mimeTypeRng = EFalse;
    
    if ( err == KErrNone )
        {
        if( dataType.Des().CompareF( KFileListRngMimeType ) == 0 )
            {
            mimeTypeRng = ETrue;
            }
        }

    TInt ringingType = RingingType();
    if ( ringingType == ERingingTypeBeepOnce )
        {
        // Active profile ringing tone is set to Beep Once
        // Don't initialize a FileSequence but use DesSequence instead
        iTonePlayer = CMdaAudioToneUtility::NewL( *this );
        iTonePlayer->PrepareToPlayDesSequence( KFileListBeepSequence() );
        iTonePlayerStatus = EPlayerInitializing;
        }
    else
        {
        if( mimeTypeRng )
            {
            //Ringingtone is a RNG-file
            iTonePlayer = CMdaAudioToneUtility::NewL( *this );
            iTonePlayer->PrepareToPlayFileSequence( fileName );
            iTonePlayerStatus = EPlayerInitializing;
            }
        else
            {
            delete iAudioPlayer;
            iAudioPlayer = 0;

            iAudioPlayer = CDrmPlayerUtility::NewFilePlayerL(
                fileName, *this, KAudioPriorityRingingTonePreview,
                ( TMdaPriorityPreference )KAudioPrefRingFilePreview );
    
            iAudioPlayerStatus = EPlayerInitializing;
            }
        }
    DisableBackLight();
    }

void CTonePlayer::Stop()
    {
    Cancel();
    }

TInt CTonePlayer::ConvertVolume( TInt aVolume )
    {
    TInt result = 0;
    if ( iAudioPlayer )
        {
        result = CMFPreviewHandlerBase::ConvertVolume( aVolume, iAudioPlayer->MaxVolume() );
        }
    else if ( iTonePlayer )
        {
        result = CMFPreviewHandlerBase::ConvertVolume( aVolume, iTonePlayer->MaxVolume() );
        }
        
    //if user has selected silent ringing type, set volume off
    TInt ringingType = RingingType();
    if( ringingType == ERingingTypeSilent )
        {
        result = 0;
        }

    return result;
    }

void CTonePlayer::SetToneRingingType( TInt aRingingType )
    {
    const TInt KToneInterval = 1000000; // 1 second pause between tones
    const TInt KAscendingVolumeInterval = 3000000; // 3 seconds
    
    if ( !iTonePlayer )
        {
        return;
        }
    TInt ringingVolume = RingingVolume();
    
    switch( aRingingType )
        {
        case ERingingTypeRinging:
        case ERingingTypeSilent:
            {
            iTonePlayer->SetRepeats( KMdaAudioToneRepeatForever,
                                     TTimeIntervalMicroSeconds( KToneInterval ) );
            break;
            }
        case ERingingTypeAscending:
            {
            iTonePlayer->SetRepeats( KMdaAudioToneRepeatForever,
                                     TTimeIntervalMicroSeconds( KToneInterval ) );
            
            TInt volRamp = KAscendingVolumeInterval * ringingVolume;
            iTonePlayer->SetVolumeRamp( TTimeIntervalMicroSeconds( volRamp ) );
            break;
            }
        case ERingingTypeRingOnce:
        case ERingingTypeBeepOnce:
            {
            iTonePlayer->SetRepeats( 0, TTimeIntervalMicroSeconds( KToneInterval ) );
            break;
            }
        default:
            {
            break;
            }
        }
    }

void CTonePlayer::SetAudioRingingType( TInt aRingingType )
    {
    const TInt KToneInterval = 1000000; // 1 second pause between tones
    const TInt KAscendingVolumeInterval = 3000000; // 3 seconds
    
    if ( !iAudioPlayer )
        {
        return;
        }

    TInt ringingVolume = RingingVolume();
        
    switch( aRingingType )
        {
        case ERingingTypeRinging:
        case ERingingTypeSilent:
            {
            iAudioPlayer->SetRepeats( KMdaAudioToneRepeatForever,
                                      TTimeIntervalMicroSeconds( KToneInterval ) );
            break;
            }
        case ERingingTypeAscending:
            {
            iAudioPlayer->SetRepeats( KMdaAudioToneRepeatForever,
                                      TTimeIntervalMicroSeconds( KToneInterval ) );
            TInt volRamp = KAscendingVolumeInterval * ringingVolume;
            iAudioPlayer->SetVolumeRamp( TTimeIntervalMicroSeconds( volRamp ) );
            break;
            }
        case ERingingTypeRingOnce:
            {
            iAudioPlayer->SetRepeats( 0, TTimeIntervalMicroSeconds( KToneInterval ) );
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CTonePlayer::Cancel()
    {
    TBool isPlaying = EFalse;
    
    if ( iAudioPlayer )
        {
        isPlaying = ETrue;
        if ( iAudioPlayerStatus == EPlayerPlayingWith3DEffect )
            {
            i3dRingingTonePlugin->Stop();
            // plugin calls AudioPlayer->Stop()
            iAudioPlayer->Close();
            }
        if ( iAudioPlayerStatus == EPlayerPlaying )
            {
            iAudioPlayer->Stop();
            iAudioPlayer->Close();
            }
        
        delete iAudioPlayer;
        iAudioPlayer = 0;
        iAudioPlayerStatus = EPlayerNotCreated;
        }
    
    if ( iTonePlayer )
        {
        isPlaying = ETrue;
        if ( iTonePlayerStatus == EPlayerPlaying )
            {
            iTonePlayer->CancelPlay();
            }
        
        delete iTonePlayer;
        iTonePlayer = 0;
        iTonePlayerStatus = EPlayerNotCreated;
        }
        

    if ( isPlaying )
        {
        //User::InfoPrint(_L("cancel"));
//        EnableScreenSaver( ETrue );
        iBacklightTimer->Cancel();
        }
    }

void CTonePlayer::MatoPlayComplete( TInt aError )
    {
    Cancel();
    iTonePlayWatcher->HandlePreviewEvent( aError );
    }

void CTonePlayer::MatoPrepareComplete( TInt aError )
    {
    if ( aError != KErrNone )
        {
        Cancel();
        
        iTonePlayWatcher->HandlePreviewEvent( aError );
        return;
        }

    TInt ringingVolume = RingingVolume();
    TInt ringingType = RingingType();
    TInt vibra = Vibra();
       
    iTonePlayerStatus = EPlayerInitialized;
    SetToneRingingType( ringingType );
    iTonePlayer->SetVolume( ConvertVolume( ringingVolume ) );
    
    TMdaPriorityPreference pref = (TMdaPriorityPreference) KAudioPrefRingFilePreview;               
    if ( vibra )
        {
        pref = (TMdaPriorityPreference) KAudioPrefRingFilePreviewVibra;
        }
    iTonePlayer->SetPriority( KAudioPriorityPreview, pref );
                                             
    iTonePlayer->Play();
    iTonePlayerStatus = EPlayerPlaying;
    }

void CTonePlayer::MdapcInitComplete( TInt aError, 
                        const TTimeIntervalMicroSeconds& /* aDuration */ )
    {
    if ( aError != KErrNone )
        {
        Cancel();
        iTonePlayWatcher->HandlePreviewEvent( aError );
        return;
        }
        

    TInt ringingVolume = RingingVolume();
    TInt ringingType = RingingType();
    TInt vibra = Vibra();
    TInt echo3D = Echo3D();
    TInt effect3D = Effect3D();

    

    iAudioPlayerStatus = EPlayerInitialized;
    SetAudioRingingType( ringingType );
    iAudioPlayer->SetVolume( ConvertVolume( ringingVolume ) );
    
    TMdaPriorityPreference pref = (TMdaPriorityPreference) KAudioPrefRingFilePreview;               
    if ( vibra )
        {
        pref = (TMdaPriorityPreference) KAudioPrefRingFilePreviewVibra;
        }
    iAudioPlayer->SetPriority( KAudioPriorityPreview, pref );
     
    iAudioPlayerStatus = EPlayerPlaying;
    
    if ( effect3D == EProfile3DEffectOff )
        {
        iAudioPlayer->Play();  // 3D not used
        return;
        }

    if ( !i3dRingingTonePlugin )
        {
        TUid emptyUid = { 0 };
        TRAPD( err, i3dRingingTonePlugin = C3DRingingToneInterface::NewL( emptyUid ) );
        if ( err != KErrNone || !i3dRingingTonePlugin )
            {
            iAudioPlayer->Play();
            return;
            }
        }       
   
    i3dRingingTonePlugin->SetAttr( E3DRTIAttr3DEffect, effect3D );
    i3dRingingTonePlugin->SetAttr( E3DRTIAttr3DEcho, echo3D );    
    i3dRingingTonePlugin->SetAttr( E3DRTIAttrDrmPlayerUtility, iAudioPlayer );
    TRAP_IGNORE( i3dRingingTonePlugin->PlayL() );

    iAudioPlayerStatus = EPlayerPlayingWith3DEffect;
    }

void CTonePlayer::MdapcPlayComplete( TInt aError )
    {  
    Cancel();
    iTonePlayWatcher->HandlePreviewEvent( aError );
    }

void CTonePlayer::HandleLosingForeground()
    {
    if ( IsPlaying() )
        {
        Stop();
        }
    }
void CTonePlayer::HandleGainingForeground()
    {
    
    }
