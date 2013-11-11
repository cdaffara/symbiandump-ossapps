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
#ifndef CRADIOENGINEIMP_H
#define CRADIOENGINEIMP_H

// System includes
#include <RadioFmTunerUtility.h>
#include <RadioPlayerUtility.h>
#include <RadioUtility.h>

// User includes
#include "cradioengine.h"
#include "mradiosettingsobserver.h"
#include "mradiordsdataobserver.h"
#include "mradiosystemeventobserver.h"
#include "radioenginedef.h"

// Forward declarations
class CRadioRdsReceiverBase;
class CRadioNetworkInfoListener;
class MRadioEngineSettings;
class MRadioSettingsSetter;
class MRadioEngineObserver;

/**
 * Radio class takes care of the FM-radio side.
 */
NONSHARABLE_CLASS( CRadioEngineImp ) : public CRadioEngine
                                     , public MRadioSystemEventObserver
                                     , public MRadioFmTunerObserver
                                     , public MRadioPlayerObserver
                                     , public MRadioSettingsObserver
                                     , public MRadioRdsDataObserver
    {
    friend class CRadioEngine;

public:

    static CRadioEngineImp* NewL();
    
    ~CRadioEngineImp();

private:

    CRadioEngineImp( CRadioAudioRouter* aAudioRouter );

// from base class CRadioEngine

    CRadioAudioRouter& AudioRouter() const;
    CRadioSystemEventCollector& SystemEventCollector() const;
    CRadioSettings& Settings() const;
    TRadioRegion DetermineRegion();
    void InitRadioL( TInt aRegionId );
    TBool RadioInitialized() const;
    void EnableAudio( TBool aEnable, TBool aDelay = ETrue );
    TBool RadioAudioEnabled() const;
    void SetAudioOverride( TBool aOverride );
    void AddObserverL( MRadioEngineObserver* aObserver );
    void RemoveObserver( MRadioEngineObserver* aObserver );
    void SetAudioMode( TInt aAudioMode );
    TBool IsFrequencyValid( TUint32 aFrequency = 0 ) const;
    void SetManualSeekMode( TBool aManualSeekActive );
    TBool IsInManualSeekMode() const;
    void SetFrequency( TUint32 aFrequency,
            RadioEngine::TRadioFrequencyEventReason aReason = RadioEngine::ERadioFrequencyEventReasonUnknown );
    void Seek( RadioEngine::TRadioTuneDirection aDirection );
    void CancelSeek();
    RadioEngine::TRadioSeeking Seeking() const;
    void AdjustVolume( RadioEngine::TRadioVolumeSetDirection aDirection );
    void SetVolume( TInt aVolume );
    void SetVolumeMuted( TBool aMute, TBool aUpdateSettings = ETrue );
    TBool IsAntennaAttached();
    TBool IsFmTransmitterActive() const;
    void SetAntennaAttached( TBool aAntennaAttached );
    TInt MaxVolumeLevel() const;
    TBool FrequencySetByRdsAf() const;
    MRadioRdsReceiver& RdsReceiver();
    TBool IsAudioRoutingPossible() const;
    TBool OkToPlay( TUint32 aFrequency ) const;

private:

    /** Radio event notifications */
    enum TRadioEventNotification
        {
        ERadioEventPower = 1,
        ERadioEventFrequency,
        ERadioEventVolume,
        ERadioEventMute,
        ERadioEventAudioMode,
        ERadioEventAntenna,
        ERadioEventAudioRouting,
        ERadioEventSeeking,
        ERadioEventRegion,
        ERadioEventFmTransmitter
        };

    enum TRadioInitialisationState
        {
        ERadioNotInitialized,
        ERadioUtilitiesConstructed,
        ERadioTunerControlRequested,
        ERadioTunerControlGranted
        };

    void ConstructL();
    

    /**
     * Switches power on/off after a delay
     *
     * @param aPowerOn ETrue if power is to be switched on,
     *                 EFalse if power is to be switched off
     */
    void SwitchPower( TBool aPowerOn );

    /**
     * Callback for switching the radio power on
     *
     * @param aSelfPtr A pointer to CRadioEngineImp instance
     * @return KErrNone
     */
    static TInt StaticPowerOnCallback( TAny* aSelfPtr );

    /**
     * Callback for switching the radio power off
     *
     * @param aSelfPtr A pointer to CRadioEngineImp instance
     * @return KErrNone
     */
    static TInt StaticPowerOffCallback( TAny* aSelfPtr );

    /**
     * Switches radio power ON.
     */
    void PowerOn();

    /**
     * Switches radio power OFF.
     */
    void PowerOff();

    /**
     * Requests tuner control from tuner utility
     */
    void RequestTunerControl();

    /**
     * Converts region code used internally by Visual Radio
     * in to a frequency range by tuner ( TFmRadioFrequencyRange )
     *
     * @param aRegionId VR region ID
     * @return Frequency range
     */
    TFmRadioFrequencyRange TunerFrequencyRangeForRegionId( TInt aRegionId ) const;

    /**
     * Non-leaving version of DoNotifyRadioEventL.
     * @param aRadioEvent event notification ( identification )
     * @param aErrorCode error code related to state change
     */
    void NotifyRadioEvent( TInt aRadioEvent, TInt aErrorCode = KErrNone );

    /**
     * Leaving version of DoNotifyRadioEvent.
     * @param aRadioEvent event notification ( identification )
     * @param aErrorCode error code related to state change
     */
    void DoNotifyRadioEventL( TInt aRadioEvent, TInt aErrorCode = KErrNone );

    /**
     * Handles the change in audio routing
     *
     * @param aDestination The new routing destination
     */
    void HandleAudioRoutingEvent( RadioEngine::TRadioAudioRoute aDestination );

    /**
     * Handles the change in power state
     *
     * @param aPowerOn The power state
     * @param aErrorCode Error code related to state change
     */
    void HandlePowerEvent( TBool aPowerOn, TInt aErrorCode = KErrNone );

    /**
     * Handles the change in power state
     *
     * @param aFrequency The current frequency
     * @param aErrorCode Error code related to state change
     */
    void HandleFrequencyEvent( TUint32 aFrequency, TInt aErrorCode = KErrNone );

    /**
     * Converts the volume used by Visual Radio to volume used by Radio Utility
     *
     * @param aUiVolume The Volume used by Visual Radio
     * @return The volume used by Radio Utility
     */
    TInt TunerVolumeForUiVolume( TInt aUiVolume );

// from base class MRadioSystemEventObserver

    void HandleSystemEventL( TRadioSystemEventType aEventType );

// from base class MRadioFmTunerObserver

    void MrftoRequestTunerControlComplete( TInt aError );
    void MrftoSetFrequencyRangeComplete( TInt aError );
    void MrftoSetFrequencyComplete( TInt aError );
    void MrftoStationSeekComplete( TInt aError, TInt aFrequency );
    void MrftoFmTransmitterStatusChange( TBool aActive );
    void MrftoAntennaStatusChange( TBool aAttached );
    void MrftoOfflineModeStatusChange( TBool aOfflineMode );
    void MrftoFrequencyRangeChange( TFmRadioFrequencyRange aBand );
    void MrftoFrequencyChange( TInt aNewFrequency );
    void MrftoForcedMonoChange( TBool aForcedMono );
    void MrftoSquelchChange( TBool aSquelch );

// from base class MRadioPlayerObserver

    void MrpoStateChange( TPlayerState aState, TInt aError );
    void MrpoVolumeChange( TInt aVolume );
    void MrpoMuteChange( TBool aMute );
    void MrpoBalanceChange( TInt aLeftPercentage, TInt aRightPercentage );

// from base class MRadioSettingsObserver

    void RdsAfSearchSettingChangedL( TBool aEnabled );
    void RegionSettingChangedL( TInt aRegion );

// from base class MRadioRdsDataObserver

    void RdsAvailable( TUint32 /*aFrequency*/, TBool /*aAvailable*/ ) {}
    void RdsAfSearchBegin();
    void RdsAfSearchEnd( TUint32 aFrequency, TInt aError );
    void RdsAfSearchStateChange( TBool /*aEnabled*/ ) {}
    void RdsDataProgrammeService( TUint32 /*aFrequency*/, const TDesC& /*aProgramService*/ ) {}
    void RdsDataRadioText( TUint32 /*aFrequency*/, const TDesC& /*aRadioText*/ ) {}
    void RdsDataRadioTextPlus( TUint32 /*aFrequency*/, const TInt /*aRadioTextPlusClass*/, const TDesC& /*aRadioText*/ ) {}
    void RdsDataGenre( TUint32 /*aFrequency*/, const TInt /*aGenre*/ ) {}
    void RdsDataPiCode( TUint32 /*aFrequency*/, const TInt /*aPiCode*/ ) {}

// New functions

    /**
     * Tries to figure the region out based on current mobile network id
     */
    TRadioRegion RegionFromMobileNetwork() const;

    /**
     * Tries to figure the region out based on timezone selection
     */
    TRadioRegion RegionFromTimezone() const;

    /**
     * Performs the timezone-based check
     */
    TRadioRegion DoRegionFromTimezoneL() const;

private: // data

    /**
     * Array of state change observers.
     */
    RPointerArray<MRadioEngineObserver>         iObservers;

    /**
     * A pointer to system event collector
     * Own.
     */
    CRadioSystemEventCollector*                 iSystemEventCollector;

    /**
     * Radio settings
     * Own.
     */
    CRadioSettings*                             iSettings;

    /**
     * Network info listener
     * Own.
     */
    CRadioNetworkInfoListener*                  iNetworkInfoListener;

    /**
     * Controls the radio hardware
     */
    CRadioUtility*                              iRadioUtility;

    /**
     * Controls FM radio tuner. Not owned.
     */
    CRadioFmTunerUtility*                       iTunerUtility;

    /**
     * Controls the radio player. Not owned.
     */
    CRadioPlayerUtility*                        iPlayerUtility;

    /**
     * Receives and notifies of the RDS data
     */
    CRadioRdsReceiverBase*                      iRdsReceiver;

    /**
     * State for radio initialization
     */
    TRadioInitialisationState                   iRadioInitializationState;

    /**
     * Radio on/off timer.
     */
    CPeriodic*                                  iRadioTimer;

    /**
    * If this is true, audio resource availability is ignored on next radio power on
    */
    TBool                                       iOverrideAudioResources;

    /**
     * Direction of the previous seek request, needed for canceling
     */
    RadioEngine::TRadioSeeking                  iSeekingState;

    /**
     * Antenna state.
     */
    TBool                                       iAntennaAttached;

    /**
     * Radio enabling state
     */
    TBool                                       iRadioEnabled;

    /**
     * ETrue if frequency was set by RDS AF
     */
    TBool                                       iFrequencySetByRdsAf;

    /**
     * The state of the fm transmitter
     */
    TBool                                       iFmTransmitterActive;

    /**
     * Cause for the ongoing frequency change operation.
     */
    RadioEngine::TRadioFrequencyEventReason     iFreqEventReason;

    /**
     * The previously scanned frequency, or <code>KErrNotFound</code> if none.
     */
    TUint32                                     iPreviousScannedFrequency;

    /**
     * The state of mute before scan has been started.
     */
    TBool                                       iPreviousMuteState;

    /**
     * Manual seek mode flag
     */
    TBool                                       iManualSeekMode;

    };

#endif  // CRADIOENGINEIMP_H

