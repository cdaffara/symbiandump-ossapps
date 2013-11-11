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

// System includes
#include <AudioOutput.h>
#include <eikdef.h>
#include <coemain.h>
#include <badesca.h>
#include <tzlocalizer.h>

// User includes
#include "cradioenginelogger.h"
#include "radiointernalcrkeys.h"
#include "cradioaudiorouter.h"
#include "cradioengineimp.h"
#include "mradioengineobserver.h"
#include "cradioregion.h"
#include "cradiosettings.h"
#include "mradioenginesettings.h"
#include "mradiosettingssetter.h"
#include "cradiordsreceiver.h"
#include "cradiosystemeventcollector.h"
#include "cradionetworkinfolistener.h"
#include "radioengine.hrh"
#include "cradioenginelogger.h"

// This has to be the last include.
#ifdef STUB_CONSTELLATION
#   include <RadioStubManager.h>
#endif //STUB_CONSTELLATION

// Constants

/** The limit of volume steps that must not be divided */
const TInt KRadioVolumeStepsDividinglimit = 20;
/** If CMMTunerUtility has 200 volume steps, AknVolume control has 20 steps */
const TInt KRadioVolumeStepsDivider = 500;
/** amount of volume steps used previously */
#if defined __WINS__
const TInt KRadioVolumeStepsWins = 20;
#endif // defined __WINS__

/** KRadioRadioSwitchDelay value must not be too small, otherwise problems with
 radioserver will occur, when swithing between valid and invalid presets */
const TInt KRadioRadioSwitchDelay = 300; // In milliseconds

const TInt KRadioThousand = 1000;
const TInt KRadioMillion = KRadioThousand * KRadioThousand;

// Country order from timezones.rss. Default = 0, Japan = 1, America = 2.
const TUint KCityGroupRegions[] =
    {0,0,0,0,0,0,2,0,0,0,0,2,0,2,0,2,0,2,0,2,2,0,2,0,0,2,2,2,0,2,0,0, //32
     0,0,0,0,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0,2,1,0,0,0,0,0,0,0,0,0,0,0, //64
     0,0,0,0,0,2,0,0,0,2,0,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //96
     0,0,0,2,0,0,0,0,2,2,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //128
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0, //160
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,2,2,0,0,0,0,0,0,0,0,0,0, //192
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0, //224
     2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineImp::CRadioEngineImp( CRadioAudioRouter* aAudioRouter )
    : CRadioEngine( aAudioRouter )
    , iAntennaAttached( ETrue )
    , iFreqEventReason( RadioEngine::ERadioFrequencyEventReasonUnknown )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineImp* CRadioEngineImp::NewL()
    {
    LEVEL3( LOG_METHOD_AUTO );
    
    // audiorouter is required for constructing engine so we need to create it and
    // radio system event collector as observer of audiorouter before creating the
    // actual engine.
    CRadioSystemEventCollector *eventCollector = CRadioSystemEventCollector::NewL();
    CleanupStack::PushL( eventCollector );
    
    MRadioAudioRoutingObserver* observer = 0;
    eventCollector->AsObserver( observer );
    User::LeaveIfNull( observer );
    
    CRadioAudioRouter* audioRouter = CRadioAudioRouter::NewL( *( observer ) );
    CleanupStack::PushL( audioRouter );
    
    CRadioEngineImp* self = new (ELeave) CRadioEngineImp( audioRouter );
    
    CleanupStack::Pop( audioRouter );
    CleanupStack::Pop( eventCollector );  
    
    self->iSystemEventCollector = eventCollector;
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::ConstructL()
    {
    LOG_METHOD_AUTO;

    if ( !iAudioRouter || !iSystemEventCollector )
        {
        User::Leave( KErrNotReady );
        }

    iSystemEventCollector->AddObserverL( this );

    iSettings = CRadioSettings::NewL();
    iSettings->RadioSetter().SetObserver( this );
    
    // Initial default values to be over by init
    User::LeaveIfError( iSettings->RadioSetter().SetPowerOn( EFalse ) );

    // The output source needs to be in headset when starting the radio.
    // But if the headset is not connected, audio cannot be heard if it is routed
    // to headset
    RadioEngine::TRadioAudioRoute route = iSystemEventCollector->IsHeadsetConnectedL() ?
                                RadioEngine::ERadioHeadset : RadioEngine::ERadioSpeaker;
    User::LeaveIfError( iSettings->RadioSetter().SetAudioRoute( route ) );

    iRdsReceiver = CRadioRdsReceiver::NewL( iSettings->EngineSettings() );
    iNetworkInfoListener = CRadioNetworkInfoListener::NewL( iSettings->RadioSetter(), NULL );

    iRdsReceiver->AddObserverL( this );

    // Create timer that is used when polling for radio restart.
    iRadioTimer = CPeriodic::NewL( CActive::EPriorityHigh );
    
    InitRadioL( DetermineRegion() );
    EnableAudio( ETrue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioEngineImp::~CRadioEngineImp()
    {
    LEVEL3( LOG_METHOD_AUTO );

    delete iNetworkInfoListener;

    PowerOff();

    delete iRadioTimer;

    DeleteAudioOutput();

    if ( iTunerUtility )
        {
        iTunerUtility->Close();
        }

    if ( iPlayerUtility )
        {
        iPlayerUtility->Close();
        }

    if ( iRdsReceiver )
        {
        iRdsReceiver->RemoveObserver( this );
        }

    delete iRdsReceiver;
    delete iRadioUtility;

    iObservers.Close();

    if ( iSystemEventCollector )
        {
        iSystemEventCollector->RemoveObserver( this );
        }
    delete iSystemEventCollector;

    if ( iSettings )
        {
        iSettings->RadioSetter().SetObserver( NULL );
        }
    delete iSettings;

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioAudioRouter& CRadioEngineImp::AudioRouter() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iAudioRouter;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSystemEventCollector& CRadioEngineImp::SystemEventCollector() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iSystemEventCollector;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CRadioSettings& CRadioEngineImp::Settings() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iSettings;
    }

// ---------------------------------------------------------------------------
// Determines radio region
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::DetermineRegion()
    {
    LEVEL3( LOG_METHOD_AUTO );
    TRadioRegion region = ERadioRegionNone;

    //TODO: Reimplement in QT side and remove the mobile network functionality
    MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
    if ( iSystemEventCollector->IsMobileNetworkCoverage() )
        {
        region = RegionFromMobileNetwork();
        }
    else
        {
        region = RegionFromTimezone();
        }

    if ( !iSettings->IsRegionAllowed( region ) )
        {
        region = ERadioRegionNone;
        }

    // Region not found, try to use the previously set region
    if ( region == ERadioRegionNone && engineSettings.RegionId() != ERadioRegionNone )
        {
        region = engineSettings.RegionId();
        }

    // All regions have been searched and no direct match found, use default one
    if ( region == ERadioRegionNone )
        {
        region = engineSettings.DefaultRegion();
        }

    return region;
    }

// ---------------------------------------------------------------------------
// Initializes / reinitializes the radio. If this is not called
// the radio is not functional
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::InitRadioL( TInt aRegionId )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "Region: %d", aRegionId );

    iRadioInitializationState = ERadioNotInitialized;

    iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonUnknown;
    if ( iSettings->EngineSettings().RegionId() != aRegionId )
        {
        // Change only when necessary as it changes also the default tuned frequency
        User::LeaveIfError( iSettings->RadioSetter().SetRegionId( aRegionId ) );
        }

    if ( !iRadioUtility )
        {
        iRadioUtility = CRadioUtility::NewL( ETrue );
        }

    if ( !iPlayerUtility )
        {
        iPlayerUtility = &iRadioUtility->RadioPlayerUtilityL( *this );
        SetAudioOutput( CAudioOutput::NewL( *iPlayerUtility ) );
        }

    if ( !iTunerUtility )
        {
        iTunerUtility = &iRadioUtility->RadioFmTunerUtilityL( *this );
        }

    // Utilities have been created now
    iRadioInitializationState = ERadioUtilitiesConstructed;

    RequestTunerControl();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::RadioInitialized() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRadioInitializationState == ERadioTunerControlGranted;
    }

// ---------------------------------------------------------------------------
// Sets the state for radio audio
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::EnableAudio( TBool aEnable, TBool aDelay )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aEnable %d ", aEnable );
    iRadioEnabled = aEnable;
    if ( aDelay )
        {
        SwitchPower( iRadioEnabled );
        }
    else if ( aEnable )
        {
        if ( !RadioInitialized() &&
             iRadioInitializationState == ERadioUtilitiesConstructed &&
             iRadioEnabled &&
             OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
            {
            RequestTunerControl();
            }
        else
            {
            PowerOn();
            }
        }
    else
        {
        PowerOff();
        }
    }

// ---------------------------------------------------------------------------
// Gets the state for radio audio.
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::RadioAudioEnabled() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iRadioEnabled;
    }

// ---------------------------------------------------------------------------
// Sets the state for audio overriding
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetAudioOverride( TBool aOverride )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aOverride %d ", aOverride ) );
    iOverrideAudioResources = aOverride;
    }

// ---------------------------------------------------------------------------
// Adds an observer for the radio state changes notifications.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::AddObserverL( MRadioEngineObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt index = iObservers.FindInAddressOrder( aObserver );
    if ( index == KErrNotFound )
        {
        iObservers.InsertInAddressOrderL( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// Removes an observer from the list of obsevers
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RemoveObserver( MRadioEngineObserver* aObserver )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt index = iObservers.FindInAddressOrder( aObserver );

    if ( index >= 0 )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TFmRadioFrequencyRange CRadioEngineImp::TunerFrequencyRangeForRegionId( TInt aRegionId ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TFmRadioFrequencyRange result = EFmRangeEuroAmerica;
    switch ( aRegionId )
        {
        case ERadioRegionDefault:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        case ERadioRegionJapan:
            {
            result = EFmRangeJapan;
            break;
            }
        case ERadioRegionAmerica:
            {
            result = EFmRangeEuroAmerica;
            break;
            }
        default:
            break;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// Sets radio mode ERadioStereo or ERadioMono
// ---------------------------------------------------------------------------

void CRadioEngineImp::SetAudioMode( TInt aAudioMode )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LOG_FORMAT( "aAudioMode: %d", aAudioMode );
    TInt err = KErrNone;
    if ( !RadioInitialized() )
        {
        TInt err = iSettings->RadioSetter().SetOutputMode( aAudioMode );
        NotifyRadioEvent( ERadioEventAudioMode, err );
        }
    else
        {
        err = iTunerUtility->ForceMonoReception( aAudioMode == RadioEngine::ERadioMono );

        if ( err )
            {
            NotifyRadioEvent( ERadioEventAudioMode, err );
            }
        }
    }

// ---------------------------------------------------------------------------
// Switches power on/off after a delay
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SwitchPower( TBool aPowerOn )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LOG_FORMAT( "aPowerOn %d", aPowerOn );
    if ( RadioInitialized() )
        {
        if ( !aPowerOn || OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
            {
            iRadioTimer->Cancel();
            if ( aPowerOn )
                {
                iRadioTimer->Start( TTimeIntervalMicroSeconds32( KRadioRadioSwitchDelay * KRadioThousand ),
                        TTimeIntervalMicroSeconds32( 0 ),
                        TCallBack( StaticPowerOnCallback, this ) );
                }
            else
                {
                iRadioTimer->Start( TTimeIntervalMicroSeconds32( KRadioRadioSwitchDelay * KRadioThousand ),
                        TTimeIntervalMicroSeconds32( 0 ),
                        TCallBack( StaticPowerOffCallback, this ) );
                }
            }
        }
    else if ( aPowerOn &&
            iRadioInitializationState == ERadioUtilitiesConstructed &&
            iRadioEnabled &&
            OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
        {
        RequestTunerControl();
        }
    else
        {
        LOG( "CRadioEngineImp::SwitchPower - Unhandled case" );
        LOG_FORMAT( "PowerOn: %d, InitializationState: %d, Enabled: %d, Frequency: %d",
                aPowerOn, iRadioInitializationState, iRadioEnabled, iSettings->EngineSettings().TunedFrequency() );
        }
    }

// ---------------------------------------------------------------------------
// Executes the power switch
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::StaticPowerOnCallback( TAny* aSelfPtr )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioEngineImp* self = reinterpret_cast<CRadioEngineImp*>( aSelfPtr );

    if ( self )
        {
        self->iRadioTimer->Cancel(); // prevents the further calls.

        if ( !self->iSettings->EngineSettings().IsPowerOn() )
            {
            self->PowerOn();
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Executes the power switch
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::StaticPowerOffCallback( TAny* aSelfPtr )
    {
    LEVEL3( LOG_METHOD_AUTO );
    CRadioEngineImp* self = reinterpret_cast<CRadioEngineImp*>( aSelfPtr );

    if ( self )
        {
        self->iRadioTimer->Cancel(); // prevents the further calls.

        if ( self->iSettings->EngineSettings().IsPowerOn() )
            {
            self->PowerOff();
            }
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::PowerOn()
    {
    LOG_METHOD_AUTO;

    if ( RadioInitialized() &&
        iRadioEnabled &&
        OkToPlay( iSettings->EngineSettings().TunedFrequency() ) )
        {
        SetAudioMode( iSettings->EngineSettings().OutputMode() );
        iPlayerUtility->SetVolumeRamp( TTimeIntervalMicroSeconds( MAKE_TINT64( 0, KRadioMillion ) ) );
        iPlayerUtility->SetVolume( TunerVolumeForUiVolume( iSettings->EngineSettings().Volume() ) );

        iPlayerUtility->Mute( iSettings->EngineSettings().IsVolMuted() );
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;

        TRAP_IGNORE( iAudioRouter->SetAudioRouteL(
                RadioEngine::TRadioAudioRoute( iSettings->EngineSettings().AudioRoute() ) ) )

        iPlayerUtility->Play();
        }
    else
        {
        HandlePowerEvent( EFalse, KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Radio power off
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::PowerOff()
    {
    LOG_METHOD_AUTO;

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        if ( RadioInitialized() )
            {
            iPlayerUtility->Stop();
            }
        else // This shouldn't occur normally, just a recovery action
            {
            HandlePowerEvent( EFalse, KErrNone );
            }
        }
    }

// ---------------------------------------------------------------------------
// Requests tuner control from tuner utility
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RequestTunerControl()
    {
    LOG_METHOD_AUTO;

    if ( iRadioInitializationState < ERadioTunerControlRequested )
        {
        LOG( "Requesting tuner control" );
        // Before first RequestTunerControl() say that it is ok to enable offline mode without checking capabilities
        iTunerUtility->EnableTunerInOfflineMode( ETrue );
        iTunerUtility->RequestTunerControl();

        iRadioInitializationState = ERadioTunerControlRequested;
        }
    else
        {
        LOG( "Tuner control already requested" );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::OkToPlay( TUint32 aFrequency ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TBool audioResourcesAvailable = iSystemEventCollector->IsAudioResourcesAvailable();
    TBool okToPlay = iAntennaAttached &&
                     !iFmTransmitterActive &&
                     ( audioResourcesAvailable || iOverrideAudioResources ) &&
#ifdef COMPILE_IN_IVALO
                     IsFrequencyValid( aFrequency )
                     && !iSystemEventCollector->IsCallActive();
#else
                     IsFrequencyValid( aFrequency );
#endif //COMPILE_IN_IVALO

    LEVEL2( LOG_FORMAT( "returning okToPlay %d ", okToPlay ) );
    return okToPlay;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsFrequencyValid( TUint32 aFrequency ) const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TBool ret( EFalse );
    if ( !aFrequency )
        {
        aFrequency = iSettings->EngineSettings().TunedFrequency();
        }
    if ( aFrequency >= iSettings->EngineSettings().MinFrequency() && aFrequency <= iSettings->EngineSettings().MaxFrequency() )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetManualSeekMode( TBool aManualSeekActive )
    {
    iManualSeekMode = aManualSeekActive;
    if ( iManualSeekMode )
        {
        CancelSeek();
        iRdsReceiver->StopReceiver();
        }
    else
        {
        iRdsReceiver->StartReceiver();
        TInt frequency = 0;
        iTunerUtility->GetFrequency( frequency );
        HandleFrequencyEvent( frequency );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsInManualSeekMode() const
    {
    return iManualSeekMode;
    }

// ---------------------------------------------------------------------------
// Radio tuning
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetFrequency( TUint32 aFrequency, RadioEngine::TRadioFrequencyEventReason aReason )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL2( LOG_FORMAT( "freq: %u, Initialized: %d, Enabled: %d",
            aFrequency, RadioInitialized(), iRadioEnabled ) );

    iFreqEventReason = aReason;
    if ( iManualSeekMode )
        {
        iTunerUtility->SetFrequency( aFrequency );
        }
    else
        {
        iFrequencySetByRdsAf = EFalse;

        TInt frequency = 0;
        if ( iTunerUtility )
            {
            iTunerUtility->GetFrequency( frequency );
            }
        CancelSeek();

        if ( aFrequency == frequency ) //radio has already the frequency to be set.
            {
            LOG( "CRadioEngineImp::SetFrequency: Already at the requested frequency" );
            HandleFrequencyEvent( aFrequency );
            }
        else
            {
            iRadioTimer->Cancel();
            if ( RadioInitialized() && iRadioEnabled && OkToPlay( aFrequency ) )
                {
                LOG( "CRadioEngineImp::SetFrequency: Tuning to frequency" );
                iTunerUtility->SetFrequency( aFrequency );
                }
            else
                {
                HandleFrequencyEvent( aFrequency );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Radio seek
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::Seek( RadioEngine::TRadioTuneDirection aDirection )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aDirection: %d",aDirection );

    iFrequencySetByRdsAf = EFalse;

    // Check if audio playing parameters ( other than frequency ) are OK
    if ( iRadioEnabled &&
        OkToPlay( iSettings->EngineSettings().MinFrequency() ) &&
        iSeekingState == RadioEngine::ERadioNotSeeking )
        {
        if ( aDirection == RadioEngine::ERadioDown )
            {
            iSeekingState = RadioEngine::ERadioSeekingDown;
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonSeekDown;
            }
        else
            {
            iSeekingState = RadioEngine::ERadioSeekingUp;
            iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonSeekUp;
            }

        NotifyRadioEvent( ERadioEventSeeking );

        if ( IsFrequencyValid() )
            {
            if ( iSettings->EngineSettings().IsPowerOn() )
                {
                iTunerUtility->StationSeek( aDirection == RadioEngine::ERadioUp ? ETrue : EFalse );
                }
            else
                {
                // Try to switch power on ( reinitialization )
                SwitchPower( ETrue );
                }
            }
        else
            {
            // Tune first to valid frequency, start seeking after radio power is on
            SetFrequency( iSettings->EngineSettings().MinFrequency(), RadioEngine::ERadioFrequencyEventReasonImplicit );
            }
        }
    else
        {
        NotifyRadioEvent( ERadioEventSeeking, KErrGeneral );
        }
    }

// ---------------------------------------------------------------------------
// Cancels seek up/down request
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::CancelSeek()
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "seeking state was:%d", iSeekingState );

    if ( iSeekingState != RadioEngine::ERadioNotSeeking )
        {
        iSeekingState = RadioEngine::ERadioNotSeeking;
        iTunerUtility->CancelStationSeek();
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
        NotifyRadioEvent( ERadioEventSeeking, KErrCancel );
        NotifyRadioEvent( ERadioEventFrequency, KErrNone ); // Notify the observers even if the frequency remains the same.
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
RadioEngine::TRadioSeeking CRadioEngineImp::Seeking() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iSeekingState;
    }

// ---------------------------------------------------------------------------
// Sets volume level up/down one step.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::AdjustVolume( RadioEngine::TRadioVolumeSetDirection aDirection )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aDirection: %d ", aDirection );

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        TInt volume = iSettings->EngineSettings().Volume();
        LOG_FORMAT( "volume = ( %d )", volume );

        if ( aDirection == RadioEngine::ERadioDecVolume )
            {
            TInt min = iSettings->EngineSettings().DefaultMinVolumeLevel();
            if ( --volume < min )
                {
                volume = 0;
                }
            }
        else if ( aDirection == RadioEngine::ERadioIncVolume )
            {
            TInt max = MaxVolumeLevel();

            if ( ++volume > max )
                {
                volume = max;
                }
            }
        else
            {
            LOG( "Unhandled case" );
            }
        SetVolume( volume );
        }
    }

// ---------------------------------------------------------------------------
// Sets audio volume level
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetVolume( TInt aVolume )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aVolume: %d", aVolume );

    if ( iSettings->EngineSettings().IsPowerOn() && RadioInitialized() )
        {
        LOG( "Setting volume to player utility" );

        if ( aVolume == 0 )
            {
            iSettings->RadioSetter().SetVolume( aVolume );
            NotifyRadioEvent( ERadioEventVolume, KErrNone );
            SetVolumeMuted( ETrue );
            }
        else
            {
            if ( iPlayerUtility->SetVolume( TunerVolumeForUiVolume( aVolume ) ) == KErrNone )
                {
                iSettings->RadioSetter().SetVolume( aVolume );
                NotifyRadioEvent( ERadioEventVolume, KErrNone );
                SetVolumeMuted( EFalse );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Set radio audio muted/unmuted
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetVolumeMuted( TBool aMuteState, TBool aUpdateSettings )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "MuteState = %d", aMuteState );

    if ( iSettings->EngineSettings().IsPowerOn() )
        {
        TInt err = KErrNone;
        if ( RadioInitialized() )
            {
            err = iPlayerUtility->Mute( aMuteState );
            }

        if ( aUpdateSettings )
            {
            err = iSettings->RadioSetter().SetVolMuted( aMuteState );
            }
        NotifyRadioEvent( ERadioEventMute, err );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsAntennaAttached()
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "returning iAntennaAttached: %d", iAntennaAttached ) );
    return iAntennaAttached;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsFmTransmitterActive() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "returning iFmTransmitterActive: %d", iFmTransmitterActive ) );
    return iFmTransmitterActive;
    }

// ---------------------------------------------------------------------------
// This function is usable only in WINS emulator.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::SetAntennaAttached( TBool aAntennaAttached )
    {
    LOG_METHOD_AUTO;
#ifdef __WINS__
    MrftoAntennaStatusChange( aAntennaAttached );
#endif
    }

// ---------------------------------------------------------------------------
// Determine the meximum volume level by dividing the maximum volume setting
// received from player utility to get the desired 20 volume steps
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::MaxVolumeLevel() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt maxLevel = 0;

#if defined __WINS__
    maxLevel = KRadioVolumeStepsWins;
#else
    if ( RadioInitialized() )
        {
        iPlayerUtility->GetMaxVolume( maxLevel );
        }
#endif //defined __WINS__

    if ( maxLevel > KRadioVolumeStepsDividinglimit )
        {
        maxLevel = maxLevel / KRadioVolumeStepsDivider;
        }

    return maxLevel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::FrequencySetByRdsAf() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    return iFrequencySetByRdsAf;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
MRadioRdsReceiver& CRadioEngineImp::RdsReceiver()
    {
    LEVEL3( LOG_METHOD_AUTO );
    return *iRdsReceiver;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CRadioEngineImp::TunerVolumeForUiVolume( TInt aUiVolume )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TInt vol = aUiVolume * KRadioVolumeStepsDivider;

    return vol;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::NotifyRadioEvent( TInt aRadioEvent, TInt aErrorCode )
    {
    LEVEL3( LOG_METHOD_AUTO );
    TRAP_IGNORE( DoNotifyRadioEventL( aRadioEvent, aErrorCode ) )
    }

// ---------------------------------------------------------------------------
// Notifies all the registered observers of radio events
// by sending notifications to observers.
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::DoNotifyRadioEventL( TInt aRadioEvent, TInt aErrorCode )
    {
    LOG_METHOD_AUTO;
    TInt count = iObservers.Count();

    for ( TInt i = 0; i < count; ++i )
        {
        MRadioEngineObserver* observer = iObservers[i];

        switch ( aRadioEvent )
            {
            case ERadioEventPower:
                {
                observer->PowerEventL( iSettings->EngineSettings().IsPowerOn(), aErrorCode );
                break;
                }
            case ERadioEventFrequency:
                {
                observer->FrequencyEventL( iSettings->EngineSettings().TunedFrequency(),
                                           iFreqEventReason, aErrorCode );
                break;
                }
            case ERadioEventVolume:
                {
                observer->VolumeEventL( iSettings->EngineSettings().Volume(), aErrorCode );
                break;
                }
            case ERadioEventMute:
                {
                observer->MuteEventL( iSettings->EngineSettings().IsVolMuted(), aErrorCode );
                break;
                }
            case ERadioEventAudioMode:
                {
                observer->AudioModeEventL( iSettings->EngineSettings().OutputMode(), aErrorCode );
                break;
                }
            case ERadioEventAntenna:
                {
                observer->AntennaEventL( iAntennaAttached, aErrorCode );
                break;
                }
            case ERadioEventAudioRouting:
                {
                observer->AudioRoutingEventL( iSettings->EngineSettings().AudioRoute(), aErrorCode );
                break;
                }
            case ERadioEventSeeking:
                {
                observer->SeekingEventL( iSeekingState, aErrorCode );
                break;
                }
            case ERadioEventRegion:
                {
                observer->RegionEventL( iSettings->EngineSettings().RegionId(), aErrorCode );
                break;
                }
            case ERadioEventFmTransmitter:
                {
                observer->FmTransmitterEventL( iFmTransmitterActive );
                break;
                }
            default:
                {
                break;
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Handles some system events
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleSystemEventL( TRadioSystemEventType aEventType )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aEventType = %d", aEventType );

    switch ( aEventType )
        {
        case ERadioAudioResourcesAvailable:
            {
            LOG( "CRadioEngineImp::HandleSystemEventL, Audio resources available" );
            SwitchPower( ETrue );
            }
            break;

        case ERadioAudioAutoResumeForbidden:
            {
            EnableAudio( EFalse );
            }
            break;

        case ERadioAudioRouteHeadset:
            {
            HandleAudioRoutingEvent( RadioEngine::ERadioHeadset );
            }
            break;

        case ERadioAudioRouteSpeaker:
            {
            HandleAudioRoutingEvent( RadioEngine::ERadioSpeaker );
            }
            break;

        case ERadioCallActivated:
            {
            CancelSeek();
            }
            break;

        case ERadioCallDeactivated:
            {
            iPlayerUtility->Play();
            SwitchPower( ETrue );
            }
            break;

        case ERadioHeadsetConnected:
            {
            // Explicitly set the audio routing to headset. Because system
            // forces the routing to headset anyway, and without our knowledge.
            MRadioSettingsSetter& setter = iSettings->RadioSetter();
            setter.SetAudioRoute( RadioEngine::ERadioHeadset );
            iAudioRouter->SetAudioRouteL( RadioEngine::ERadioHeadset );
            }
            break;

        case ERadioHeadsetDisconnected:
            {
            // Explicitly set the audio routing to speaker. Because system
            // forces the routing to speaker anyway, if radio stays on.
            MRadioSettingsSetter& setter = iSettings->RadioSetter();
            setter.SetAudioRoute( RadioEngine::ERadioSpeaker );
            iAudioRouter->SetAudioRouteL( RadioEngine::ERadioSpeaker );
            }
            break;
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoRequestTunerControlComplete( TInt aError )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aError: %d", aError );

    if ( aError == KErrNone || aError == KErrAlreadyExists || aError == -12056 ) // Tuner activated now or was already active
        {
        iRadioInitializationState = ERadioTunerControlGranted;

        TFmTunerCapabilities tunerCaps;
        tunerCaps.iTunerBands = 0;
        tunerCaps.iTunerFunctions = 0;
        tunerCaps.iAdditionalFunctions1 = 0;
        tunerCaps.iAdditionalFunctions2 = 0;
        iTunerUtility->GetCapabilities( tunerCaps );

#ifdef LOGGING_ENABLED
        TBuf<50> tunerCapsBuf;
        if ( tunerCaps.ETunerAvailableInOfflineMode )
            {
            tunerCapsBuf.Append( _L("OfflineMode ") );
            }
        if ( tunerCaps.ETunerRdsSupport )
            {
            tunerCapsBuf.Append( _L("RDS ") );
            }
        if ( tunerCaps.ETunerDualTunerSupport )
            {
            tunerCapsBuf.Append( _L("DualTuner ") );
            }
        LOG_FORMAT( "Radio tuner capabilities: %S", &tunerCapsBuf );

#endif // LOGGING_ENABLED

        TBool offlineAvailable( tunerCaps.iTunerFunctions &
                                TFmTunerCapabilities::ETunerAvailableInOfflineMode );
        iTunerUtility->EnableTunerInOfflineMode( offlineAvailable );

        if ( tunerCaps.iTunerFunctions & TFmTunerCapabilities::ETunerRdsSupport )
            {
            TRAP_IGNORE( iRdsReceiver->InitL( *iRadioUtility ) )
            }

        iTunerUtility->SetFrequencyRange( TunerFrequencyRangeForRegionId(
                                          iSettings->EngineSettings().RegionId() ) );
        // To prevent false frequency changes
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        SwitchPower( ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSetFrequencyRangeComplete( TInt aError )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aError: %d", aError ) );
    if ( aError )
        {
        NotifyRadioEvent( ERadioEventRegion, aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSetFrequencyComplete( TInt aError )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aError: %d", aError );

    if ( aError && !iManualSeekMode )
        {
        if ( aError == KErrNotReady )
            {
            iRadioInitializationState = ERadioUtilitiesConstructed;
            }
        HandleFrequencyEvent( iSettings->EngineSettings().TunedFrequency(), aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoStationSeekComplete( TInt aError, TInt aFrequency )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aError = %d, aFrequency = %d", aError, aFrequency );
    // Seeking has ended, error code tells if it was successful

    if ( aError == KFmRadioErrAntennaNotConnected )
        {
        iAntennaAttached = EFalse;
        }
    else if ( aError == KErrNotReady )
        {
        iRadioInitializationState = ERadioUtilitiesConstructed;
        }

    iSeekingState = RadioEngine::ERadioNotSeeking;

    // -12051 comes from adaptation when no stations found in scanning
    if ( aError == -12051 || aFrequency == 0 )
        {
        NotifyRadioEvent( ERadioEventFrequency, aError );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFmTransmitterStatusChange( TBool aActive )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aActive: %d", aActive );
    iFmTransmitterActive = aActive;

    if ( !iFmTransmitterActive )
        {
        SwitchPower( ETrue );
        }

    NotifyRadioEvent( ERadioEventFmTransmitter );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoAntennaStatusChange( TBool aAttached )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aAttached: %d", aAttached );
    iAntennaAttached = aAttached;
    if ( iAntennaAttached )
        {
        SwitchPower( ETrue );
        NotifyRadioEvent( ERadioEventAntenna );
        }
    else
        {
        NotifyRadioEvent( ERadioEventAntenna, KErrDisconnected );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------

//
void CRadioEngineImp::MrftoOfflineModeStatusChange( TBool DEBUGVAR3( aOfflineMode ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aOfflineMode: %d", aOfflineMode ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFrequencyRangeChange( TFmRadioFrequencyRange DEBUGVAR3( aBand ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aBand: %d", aBand ) );
    if ( RadioInitialized() )
        {
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
        }
    NotifyRadioEvent( ERadioEventRegion );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoFrequencyChange( TInt aNewFrequency )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aNewFrequency = %u", aNewFrequency ) );

    // There may be frequency changed events when radio is not initialized ( because
    // of SetFrequency or Seek returns with KErrNotReady ).
    if ( !iManualSeekMode && RadioInitialized() )
        {
        HandleFrequencyEvent( aNewFrequency );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoForcedMonoChange( TBool aForcedMono )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aForcedMono = %d", aForcedMono ) );

    iSettings->RadioSetter().SetOutputMode( aForcedMono ? RadioEngine::ERadioMono : RadioEngine::ERadioStereo );
    NotifyRadioEvent( ERadioEventAudioMode );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrftoSquelchChange( TBool /*aSquelch*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoStateChange( TPlayerState aState, TInt aError )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aState = %d, aError = %d", aState, aError ) );

    if ( aError == KFmRadioErrAntennaNotConnected )
        {
        iAntennaAttached = EFalse;
        }
    HandlePowerEvent( aState == ERadioPlayerPlaying, aError );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoVolumeChange( TInt aVolume )
    {
    LEVEL3( LOG_METHOD_AUTO );
	aVolume = aVolume / KRadioVolumeStepsDivider;
    if ( aVolume != iSettings->EngineSettings().Volume() )
        {
        iSettings->RadioSetter().SetVolume( aVolume );
        NotifyRadioEvent( ERadioEventVolume );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoMuteChange( TBool /*aMute*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::MrpoBalanceChange( TInt /*aLeftPercentage*/, TInt /*aRightPercentage*/ )
    {
    LEVEL3( LOG_METHOD_AUTO );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchSettingChangedL( TBool aEnabled )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aEnabled: %d", aEnabled ) );
    iRdsReceiver->SetAutomaticSwitchingL( aEnabled );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
//TODO: Check if this can be removed. Radio doesn't currently support changing regions on the fly
void CRadioEngineImp::RegionSettingChangedL( TInt DEBUGVAR3( aRegion ) )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aRegion: %d", aRegion ) );

    if ( RadioInitialized() )
        {
        iTunerUtility->SetFrequencyRange( TunerFrequencyRangeForRegionId( iSettings->EngineSettings().RegionId() ) );
        iTunerUtility->SetFrequency( iSettings->EngineSettings().TunedFrequency() );
        iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonImplicit;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchBegin()
    {
    LEVEL3( LOG_METHOD_AUTO );
    iFrequencySetByRdsAf = ETrue;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::RdsAfSearchEnd( TUint32 DEBUGVAR3( aFrequency ), TInt aError )
    {
    LEVEL3( LOG_METHOD_AUTO );
    LEVEL3( LOG_FORMAT( "aFrequency: %d, aError: %d )", aFrequency, aError ) );
    if ( aError != KErrNone )
        {
        iFrequencySetByRdsAf = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// Tries to figure the region out based on current mobile network id
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::RegionFromMobileNetwork() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TRadioRegion region = ERadioRegionNone;

    // Choose the frequency range according to country code
    MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
    TPtrC countryCode = engineSettings.CountryCode();
    const TInt regionCount = engineSettings.CountRegions();
    TBool matchFound = EFalse;
    for ( TInt i = 0; i < regionCount && !matchFound; ++i )
        {
        const RRadioCountryCodeArray& regionCountryCodes =
                engineSettings.Region( i ).CountryCodes();

        const TInt countryCodeCount = regionCountryCodes.Count();
        for ( TInt j = 0; j < countryCodeCount && !matchFound; ++j )
            {
            if ( countryCode == *regionCountryCodes[j] )
                {
                // We have a match
                matchFound = ETrue;
                region = engineSettings.Region( i ).Id();
                }
            }
        }

    return region;
    }

// ---------------------------------------------------------------------------
// Tries to figure the region out based on timezone selection
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::RegionFromTimezone() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TRadioRegion region = ERadioRegionNone;
    TRAP_IGNORE( region = DoRegionFromTimezoneL() );
    return region;
    }

// ---------------------------------------------------------------------------
// Performs the timezone-based check
// ---------------------------------------------------------------------------
//
TRadioRegion CRadioEngineImp::DoRegionFromTimezoneL() const
    {
    LOG_METHOD_AUTO;
    CTzLocalizer* timezoneLocalizer = CTzLocalizer::NewL();
    CleanupStack::PushL( timezoneLocalizer );

    CTzLocalizedCityGroupArray* cityGroups = timezoneLocalizer->GetAllCityGroupsL(
                                            CTzLocalizer::ETzAlphaNameAscending );
    CleanupStack::PushL( cityGroups );

    // We get the ownership so we must delete
    CTzLocalizedCity* city = timezoneLocalizer->GetFrequentlyUsedZoneCityL( CTzLocalizedTimeZone::ECurrentZone );
    const TUint8 cityId = city->GroupId();
    delete city;
    city = NULL;
    LOG_FORMAT( "group id: %d", cityId );

    TRadioRegion region = ERadioRegionNone;
    const TInt cityGroupCount = cityGroups->Count();
    TBool found = EFalse;
    for ( TInt i = 0; i < cityGroupCount && !found; ++i )
        {
        if ( cityId == cityGroups->At( i ).Id() )
            {
            region = static_cast<TRadioRegion>( KCityGroupRegions[ cityId - 1 ] );
            found = ETrue;
            }
        }

    CleanupStack::PopAndDestroy( cityGroups );
    CleanupStack::PopAndDestroy( timezoneLocalizer );

    LOG_ASSERT( found, LOG_FORMAT( "City not found: %d", cityId ) );

    return region;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleAudioRoutingEvent( RadioEngine::TRadioAudioRoute aDestination )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aDestination: %d", aDestination );

    // Make modifications to volume ONLY if new audio source state
    // differs from settings. If they don't differ, this state
    // change is a part of the radio initialization.
    MRadioSettingsSetter& setter = iSettings->RadioSetter();
    if ( aDestination != iSettings->EngineSettings().AudioRoute() )
        {
        setter.SetAudioRoute( aDestination );

        // If audio muted, change it to minimum volume
        MRadioEngineSettings& engineSettings = iSettings->EngineSettings();
        TInt vol = engineSettings.IsVolMuted() ? engineSettings.DefaultMinVolumeLevel()
                                               : engineSettings.Volume();

        if ( RadioInitialized() )
            {
            TInt err = iPlayerUtility->Mute( EFalse );
            if ( !err )
                {
                setter.SetVolMuted( EFalse );
                err = iPlayerUtility->SetVolume( TunerVolumeForUiVolume( vol ));

                if ( !err )
                    {
                    setter.SetVolume( vol );
                    }
                }
            }
        else
            {
            setter.SetVolMuted( EFalse );
            setter.SetVolume( vol );
            }
        NotifyRadioEvent( ERadioEventAudioRouting );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandlePowerEvent( TBool aPowerOn, TInt aErrorCode )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aPowerOn: %d, aErrorCode: %d )", aPowerOn, aErrorCode );

    const TBool powerState = iSettings->EngineSettings().IsPowerOn();
    if ( !powerState != !aPowerOn )
        {
        iSettings->RadioSetter().SetPowerOn( aPowerOn );

        if ( aPowerOn )
            {
            iRdsReceiver->StartReceiver();
            }
        else
            {
            iRdsReceiver->StopReceiver();
            }
        }

    if ( !aPowerOn )
        {
        CancelSeek();
        }

    // If we are seeking, power event starts seeking
    if ( iSeekingState != RadioEngine::ERadioNotSeeking && aPowerOn )
        {
        // Reset seeking state to enable seeking start
        LOG( "PowerOn event in seekingstate. Restart seeking" );
        RadioEngine::TRadioSeeking oldSeeking = iSeekingState;
        iSeekingState = RadioEngine::ERadioNotSeeking;
        Seek( oldSeeking == RadioEngine::ERadioSeekingUp ? RadioEngine::ERadioUp : RadioEngine::ERadioDown );
        }

    if ( !powerState != !aPowerOn || aErrorCode )
        {
        NotifyRadioEvent( ERadioEventPower, aErrorCode );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CRadioEngineImp::HandleFrequencyEvent( TUint32 aFrequency, TInt aErrorCode )
    {
    LOG_METHOD_AUTO;
    LOG_FORMAT( "aFrequency:  %d, aErrorCode: %d )", aFrequency, aErrorCode );

    if ( iSettings->EngineSettings().TunedFrequency() != aFrequency )
        {
        if ( iSettings->EngineSettings().IsPowerOn() )
            {
            if ( !OkToPlay( aFrequency ) )
                {
                // Radio is going to be powered off, stop rds receiver immediately because
                // some rds data from previous channel might come before power off event.
                iRdsReceiver->StopReceiver();
                }
            else
                {
                // Normal frequency change, make sure that rds receiver is started
                iRdsReceiver->StartReceiver();
                }
            }
        if ( !iFrequencySetByRdsAf )
            {
            iRdsReceiver->ClearRdsInformation();
            }
        }
    else if ( iFrequencySetByRdsAf )
        {
        // frequency didn't change, so AF search didn't complete successfully
        iFrequencySetByRdsAf = EFalse;
        }

    iSettings->RadioSetter().SetTunedFrequency( aFrequency );

    NotifyRadioEvent( ERadioEventFrequency, aErrorCode );
    iFreqEventReason = RadioEngine::ERadioFrequencyEventReasonUnknown;

    if ( aErrorCode == KErrNone )
        {
        SwitchPower( iRadioEnabled && OkToPlay( aFrequency ) );
        }
    }

// ---------------------------------------------------------------------------
// Routing is not possible when headset is not available, power is off or
// audio routing is not supported.
// ---------------------------------------------------------------------------
//
TBool CRadioEngineImp::IsAudioRoutingPossible() const
    {
    LEVEL3( LOG_METHOD_AUTO );
    TBool headsetConnected = EFalse;
    TRAP_IGNORE( ( headsetConnected = iSystemEventCollector->IsHeadsetConnectedL() ) )

    TBool powerOn = iSettings->EngineSettings().IsPowerOn();

    TBool isAudioRoutingPossible = EFalse;
    if ( headsetConnected && powerOn && AudioRoutingSupported() )
        {
        isAudioRoutingPossible = ETrue;
        }

    return isAudioRoutingPossible;
    }

