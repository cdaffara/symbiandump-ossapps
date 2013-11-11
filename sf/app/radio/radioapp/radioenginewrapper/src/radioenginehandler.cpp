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

// User includes
#include "cradioengine.h"
#include "cradioaudiorouter.h"
#include "cradiosystemeventcollector.h"
#include "cradiosettings.h"
#include "mradioenginesettings.h"
#include "mradiordsreceiver.h"
#include "radioenginehandler.h"
#include "mradioenginehandlerobserver.h"
#include "radio_global.h"
#include "radiologger.h"
#include "radioenummapper.h"

/*!
 * Map to translate seek direction enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KSeekDirectionMap )
    ENUM_MAP_ITEM( Seek::Down,              RadioEngine::ERadioDown ),
    ENUM_MAP_ITEM( Seek::Up,                RadioEngine::ERadioUp )
END_ENUM_MAP( KSeekDirectionMap )

/*!
 * Convenience macro to do the mapping of seek directions
 */
#define MAP_FROM_UI_DIRECTION(ui_enum) MAP_FROM_UI_ENUM( RadioEngine::TRadioTuneDirection, ui_enum, KSeekDirectionMap )

/*!
 * Map to translate radio region enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KRegionMap )
    ENUM_MAP_ITEM( RadioRegion::None,       ERadioRegionNone ),
    ENUM_MAP_ITEM( RadioRegion::Default,    ERadioRegionDefault ),
    ENUM_MAP_ITEM( RadioRegion::Japan,      ERadioRegionJapan ),
    ENUM_MAP_ITEM( RadioRegion::America,    ERadioRegionAmerica ),
    ENUM_MAP_ITEM( RadioRegion::Poland,     ERadioRegionPoland ),
END_ENUM_MAP( KRegionMap )

/*!
 * Convenience macros to do the mapping of radio regions
 */
#define MAP_FROM_UI_REGION(ui_enum) MAP_FROM_UI_ENUM( TRadioRegion, ui_enum, KRegionMap )
#define MAP_TO_UI_REGION(engine_enum) MAP_TO_UI_ENUM( RadioRegion::Region, engine_enum, KRegionMap )

/*!
 * Map to translate seeking state enum from its definition in the engine to
 * its definition in the ui and vice versa
 */
BEGIN_ENUM_MAP( KSeekingStateMap )
    ENUM_MAP_ITEM( Seek::NotSeeking,        RadioEngine::ERadioNotSeeking ),
    ENUM_MAP_ITEM( Seek::SeekingUp,         RadioEngine::ERadioSeekingUp ),
    ENUM_MAP_ITEM( Seek::SeekingDown,       RadioEngine::ERadioSeekingDown )
END_ENUM_MAP( KSeekingStateMap )

/*!
 * Convenience macro to do the mapping of seeking states
 */
#define MAP_TO_UI_SEEKING_STATE(ui_enum) MAP_TO_UI_ENUM( Seek::State, ui_enum, KSeekingStateMap )

/*!
 *
 */
RadioEngineHandler::RadioEngineHandler( MRadioEngineHandlerObserver& observer )
    : mObserver( observer )
{
}

/*!
 *
 */
RadioEngineHandler::~RadioEngineHandler()
{
}

/*!
 * Attempts to construct the radio engine
 */
bool RadioEngineHandler::constructEngine()
{
    LOG_METHOD;

    CRadioEngine* engine = NULL;
    TRAPD( err, engine = CRadioEngine::NewL() );
    if ( err ) {
        return false;
    }

    mEngine.reset( engine );
    TRAP( err,
        mEngine->SystemEventCollector().AddObserverL( &mObserver );
        mEngine->AddObserverL( &mObserver );
    );
    if ( err ) {
        return false;
    }

    mRegion = MAP_TO_UI_REGION( mEngine->Settings().EngineSettings().RegionId() );
    return true;
}

/*!
 * Sets the rds data observer
 */
void RadioEngineHandler::setRdsObserver( MRadioRdsDataObserver* observer )
{
    TRAP_IGNORE( mEngine->RdsReceiver().AddObserverL( observer ) );
}

/*!
 * Starts or stops receiving RDS data
 */
void RadioEngineHandler::setRdsEnabled( bool rdsEnabled )
{
    if ( rdsEnabled ) {
        mEngine->RdsReceiver().StartReceiver();
    } else {
        mEngine->RdsReceiver().StopReceiver();
    }
}

/*!
 * Returns the radio status.
 */
bool RadioEngineHandler::isRadioOn()
{
    return mEngine->Settings().EngineSettings().IsPowerOn();
}

/*!
 * Sets the manual seek status
 */
void RadioEngineHandler::setManualSeekMode( bool manualSeek )
{
    mEngine->SetManualSeekMode( manualSeek );
}

/*!
 * Returns the manual seek status
 */
bool RadioEngineHandler::isInManualSeekMode() const
{
    return mEngine->IsInManualSeekMode();
}

/*!
 * Tune to the specified frequency
 */
void RadioEngineHandler::setFrequency( uint frequency )
{
    mEngine->SetFrequency( frequency );
}

/*!
 * Sets the audio mute state
 */
void RadioEngineHandler::setMute( const bool muted, const bool updateSettings )
{
    mEngine->SetVolumeMuted( muted, updateSettings );
}

/*!
 * Gets the audio mute state
 */
bool RadioEngineHandler::isMuted() const
{
    return mEngine->Settings().EngineSettings().IsVolMuted();
}

/*!
 * Sets the volume level of the FM radio
 */
void RadioEngineHandler::setVolume( int newVolume )
{
    if ( volume() != newVolume ) {
        if ( newVolume > 0 ) {
            mEngine->SetVolumeMuted( EFalse );
        }

        mEngine->SetVolume( newVolume );
    }
}

/*!
 * Gets the volumelevel.
 */
int RadioEngineHandler::volume() const
{
    return mEngine->Settings().EngineSettings().Volume();
}

/*!
 * Gets the max volumelevel.
 */
int RadioEngineHandler::maxVolume() const
{
    return mEngine->MaxVolumeLevel();
}

/*!
 * Increases the volume by one increment
 */
void RadioEngineHandler::increaseVolume()
{
    mEngine->AdjustVolume( RadioEngine::ERadioIncVolume );
}

/*!
 * Decreases the volume by one increment
 */
void RadioEngineHandler::decreaseVolume()
{
    mEngine->AdjustVolume( RadioEngine::ERadioDecVolume );
}


/*!
 * Checks if the antenna is attached
 */
bool RadioEngineHandler::isAntennaAttached() const
{
    return mEngine->IsAntennaAttached();
}

/*!
 * Retrieves the current frequency.
 */
uint RadioEngineHandler::currentFrequency() const
{
    return mEngine->Settings().EngineSettings().TunedFrequency();
}

/*!
 * Returns the minimum allowed frequency in the current region
 */
uint RadioEngineHandler::minFrequency() const
{
    return mEngine->Settings().EngineSettings().MinFrequency();
}

/*!
 * Returns the maximum allowed frequency in the current region
 */
uint RadioEngineHandler::maxFrequency() const
{
    return mEngine->Settings().EngineSettings().MaxFrequency();
}

/*!
 * Checks if the given frequency is valid in the current region
 */
bool RadioEngineHandler::isFrequencyValid( uint frequency ) const
{
    return mEngine->IsFrequencyValid( frequency );
}

/*!
 * Scan up to the next available frequency
 */
void RadioEngineHandler::seek( Seek::Direction direction )
{
    LOG_TIMESTAMP( "Seek" );
    mEngine->Seek( MAP_FROM_UI_DIRECTION( direction ) );
}

/*!
 * Cancel previously requested scan, and return to the already tuned frequency
 */
void RadioEngineHandler::cancelSeek()
{
    mEngine->CancelSeek();
}

/*!
 * Returns the engine seeking state
 */
Seek::State RadioEngineHandler::seekingState() const
{
    return MAP_TO_UI_SEEKING_STATE( mEngine->Seeking() );
}

/*!
 * return step size for tuning.
 */
uint RadioEngineHandler::frequencyStepSize() const
{
    return mEngine->Settings().EngineSettings().FrequencyStepSize();
}

/*!
 * Returns the selected radio region
 */
RadioRegion::Region RadioEngineHandler::region() const
{
    return mRegion;
}

/*!
 * Sets whether or not audio should be routed to loudspeaker
 */
void RadioEngineHandler::setAudioRouteToLoudspeaker( bool loudspeaker )
{
    TRAPD( err, mEngine->AudioRouter().SetAudioRouteL( loudspeaker ? RadioEngine::ERadioSpeaker
                                                       : RadioEngine::ERadioHeadset ) );
    if ( err ) {
        LOG_FORMAT( "Failed to set audioroute: UseLoudspeadker: %d", loudspeaker );
    }
}

/*!
 * Checks if audio is routed to loudspeaker
 */
bool RadioEngineHandler::isAudioRoutedToLoudspeaker() const
{
    return mEngine->Settings().EngineSettings().AudioRoute() == RadioEngine::ERadioSpeaker;
}

/*!
 * Returns the repository manager.
 */
MRadioApplicationSettings& RadioEngineHandler::applicationSettings() const
{
    return mEngine->Settings().ApplicationSettings();
}

