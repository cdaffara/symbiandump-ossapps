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

#ifndef CRADIOENGINE_H_
#define CRADIOENGINE_H_

// User includes
#include "cradioroutableaudio.h"
#include "radioenginedef.h"
#include "radioengine.hrh"

// Forward declarations
class CRadioAudioRouter;
class CRadioSettings;
class MRadioApplicationSettings;
class MRadioEngineSettings;
class CRadioSystemEventCollector;
class CRadioRepositoryManager;
class MRadioPresetSettings;
class MRadioSettingsSetter;
class MRadioEngineObserver;
class MRadioRdsReceiver;

/**
 * Convenience class to handle engine startup and the ownership of the related classes
 */
NONSHARABLE_CLASS( CRadioEngine ) : public CRadioRoutableAudio
    {
public:

    /**
     * Starts the radio engine initialization.
     * Asks the given initializer to create the member variables
     */
    IMPORT_C static CRadioEngine* NewL();

    IMPORT_C ~CRadioEngine();

    /**
     * Returns a reference to the audio router
     *
     * @return  Reference to the audio router
     */
    virtual CRadioAudioRouter& AudioRouter() const = 0;

    /**
     * Returns the system event collector
     *
     * @return  The system event collector
     */
    virtual CRadioSystemEventCollector& SystemEventCollector() const = 0;

    /**
     * Returns a reference to the application settings
     *
     * @return  Reference to the application settings
     */
    virtual CRadioSettings& Settings() const = 0;

    /**
     * Determines radio region
     */
    virtual TRadioRegion DetermineRegion() = 0;

    /**
     * ( Re )initializes the radio.
     *
     * @param aRegionId Region id
     */
    virtual void InitRadioL( TInt aRegionId ) = 0;

    /**
     * Has the radio been initialized
     *
     * @return ETrue if radio is initialized, otherwise EFalse
     */
    virtual TBool RadioInitialized() const = 0;

    /**
     * Sets the state for radio audio
     * This is the public interface to manage radio power state
     * This method should be called before radio can turn itself on
     *
     * @param aEnable ETrue if radio audio can be played,
     *                EFalse if audio should be disabled
     * @param aDelay  ETrue if radio is played/disabled with delay
     *                EFalse if radio is played/disabled without delay
     */
    virtual void EnableAudio( TBool aEnable, TBool aDelay = ETrue ) = 0;

    /**
     * Gets the state for radio audio.
     *
     * This returns the information if radio could be played when
     * other conditions are valid. This should be false only when
     * some other application has cause disabling of auto resume.
     *
     * @return ETrue if radio audio could be played,
     *         EFalse if radio audio couldn't be played
     */
    virtual TBool RadioAudioEnabled() const = 0;

    /**
     * Sets the audio overriding flag
     *
     * @param aOverride ETrue if other audio resources are tried to be overriden.
     */
    virtual void SetAudioOverride( TBool aOverride ) = 0;

    /**
     * Adds an observer wich will be notified of the radio state changes
     * If observer already exists, it is not added
     *
     * @param aObserver pointer of the observer instance.
     */
    virtual void AddObserverL( MRadioEngineObserver* aObserver ) = 0;

    /**
     * Removes a radio state change observer.
     */
    virtual void RemoveObserver( MRadioEngineObserver* aObserver ) = 0;

    /**
     * Sets audio mode ( Stereo/Mono )
     *
     * @param aAudioMode actual new radio mode
     */
    virtual void SetAudioMode( TInt aAudioMode ) = 0;

    /**
     * Compares if frequency is within limits range.
     *
     * @param aFrequency The frequency that is checked.
     *                   If omitted, current frequency is checked.
     * @return ETrue, if frequency is within radio range.
     */
    virtual TBool IsFrequencyValid( TUint32 aFrequency = 0 ) const = 0;

    /**
     * Sets or unsets the manual seek mode
     *
     * @param aManualSeekActive ETrue if active EFalse if not
     */
    virtual void SetManualSeekMode( TBool aManualSeekActive ) = 0;

    /**
     * Returns the manual seek mode status
     */
    virtual TBool IsInManualSeekMode() const = 0;

    /**
     * Tunes to frequency
     * If radio is not initialized by InitRadioL, frequency is just
     * set to settings.
     *
     * @param aFrequency frequency to tune to
     * @param aReason reason for the frequency setting
     */
    virtual void SetFrequency( TUint32 aFrequency,
            RadioEngine::TRadioFrequencyEventReason aReason = RadioEngine::ERadioFrequencyEventReasonUnknown ) = 0;

    /**
     * Performs seeking operation.
     *
     * @param aDirection direction of seeking operation.
     */
    virtual void Seek( RadioEngine::TRadioTuneDirection aDirection ) = 0;

    /**
     * Cancels an ongoing request to seek up/down.
     */
    virtual void CancelSeek() = 0;

    /**
     * Is the radio seeking up/down, or at all.
     *
     * @return Seeking state.
     */
    virtual RadioEngine::TRadioSeeking Seeking() const = 0;

    /**
     * Changes volume by one level.
     *
     * @param aDirection Direction of the volume change.
     */
    virtual void AdjustVolume( RadioEngine::TRadioVolumeSetDirection aDirection ) = 0;

    /**
     * Sets volume level
     *
     * @param aVolume new volume level.
     */
    virtual void SetVolume( TInt aVolume ) = 0;

    /**
     * Sets volume to be muted.
     *
     * @param aMute mute status.
     */
    virtual void SetVolumeMuted( TBool aMute, TBool aUpdateSettings = ETrue ) = 0;

    /**
     * Retrieves current antenna state.
     *
     * @return ETrue if antenna is attached, EFalse otherwise.
     */
    virtual TBool IsAntennaAttached() = 0;

    /**
     * Retrieves current state of fm transmitter.
     *
     * @return ETrue if fm transmitter is active, EFalse otherwise.
     */
    virtual TBool IsFmTransmitterActive() const = 0;

    /**
     * Changes the antenna state.
     * This should only be used when faking the antenna
     * state in emulator environment.
     * Thus this should work only in WINS.
     *
     * @param aAntennaAttached The new antenna state.
     */
    virtual void SetAntennaAttached( TBool aAntennaAttached ) = 0;

    /**
     * Returns the maximum volume level
     *
     * @return maximum volume level
     */
    virtual TInt MaxVolumeLevel() const = 0;

    /**
     * Has the frequency been set by RDS AF search or not.
     *
     * @return ETrue if frequency was set by RDS AF, otherwise EFalse
     */
    virtual TBool FrequencySetByRdsAf() const = 0;

    /**
     * Getter for RDS receiver
     *
     * @return Reference to CRadioRdsReceiver
     */
     virtual MRadioRdsReceiver& RdsReceiver() = 0;

    /**
     * Is routing between loudspeaker and headset possible
     *
     * @return ETrue, if routing is possible
     */
     virtual TBool IsAudioRoutingPossible() const = 0;

    /**
     * Checks if the radio playing parameters are OK
     *
     * @param aFrequency The frequency that is wanted to play.
     * @return ETrue if audio can be played, otherwise EFalse
     */
    virtual TBool OkToPlay( TUint32 aFrequency ) const = 0;

protected:

    CRadioEngine( CRadioAudioRouter* aAudioRouter );

    };

#endif // CRADIOENGINE_H_
