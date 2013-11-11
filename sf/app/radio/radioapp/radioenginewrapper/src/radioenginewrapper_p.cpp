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

// User includes
#include "radioenginewrapper_p.h"
#include "radiosettings.h"
#include "radiosettings_p.h"
#include "radiologger.h"
#include "radio_global.h"
#include "radioenginehandler.h"
#include "radiostationhandlerif.h"
#include "radiocontroleventlistener.h"
#include "radiordslistener.h"
#include "radioenginewrapperobserver.h"

// Constants

/*!
 *
 */
RadioEngineWrapperPrivate::RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                                                      RadioStationHandlerIf& stationHandler ) :
    q_ptr( wrapper ),
    mStationHandler( stationHandler ),
    mEngineHandler( new RadioEngineHandler( *this ) ),
    mControlEventListener( new RadioControlEventListener( *this ) ),
    mRdsListener ( new RadioRdsListener( mStationHandler, *this ) ),
    mTuneReason( TuneReason::Unspecified ),
    mUseLoudspeaker( false )
{
}

/*!
 *
 */
RadioEngineWrapperPrivate::~RadioEngineWrapperPrivate()
{
    // Destructor needs to be defined because some member variables that are forward declared
    // in the header are managed by QT's smart pointers and they require that the owning class
    // has a non-inlined destructor. Compiler generates an inlined destructor if it isn't defined.
}

/*!
 * Initializes the private implementation
 */
bool RadioEngineWrapperPrivate::init()
{
    if ( !mEngineHandler->constructEngine() ) {
        LOG( "RadioEngineWrapperPrivate::init, EngineHandler construct failed" );
        mEngineHandler.reset();
        return false;
    }

    mEngineHandler->setRdsObserver( mRdsListener.data() );
    mControlEventListener->init();

    mUseLoudspeaker = mEngineHandler->isAudioRoutedToLoudspeaker();
    if ( !mUseLoudspeaker ) {
        RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( false ) );
    }

    return true;
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettingsIf& RadioEngineWrapperPrivate::settings()
{
    if ( !mSettings ) {
        mSettings.reset( new RadioSettings() );
        mSettings->d_func()->init( &mEngineHandler->applicationSettings() );
    }
    return *mSettings;
}

/*!
 * Returns the enginehandler owned by the engine
 */
RadioEngineHandler& RadioEngineWrapperPrivate::radioEnginehandler()
{
    return *mEngineHandler;
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapperPrivate::setFrequency( uint frequency, const int reason )
{
    if ( mEngineHandler->currentFrequency() != frequency ) {
        mTuneReason = reason;
        mEngineHandler->setFrequency( frequency );
    }
}

/*!
 *
 */
ObserverList& RadioEngineWrapperPrivate::observers()
{
    return mObservers;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::startSeeking( Seek::Direction direction, const int reason )
{
    mTuneReason = reason;
    mEngineHandler->seek( direction );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::PowerEventL( TBool aPowerState, TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::PowerEventL, PowerState: %d, Error: %d", aPowerState, aError );
    RUN_NOTIFY_LOOP( mObservers, radioStatusChanged( aPowerState ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::FrequencyEventL( TUint32 aFrequency,
                                                 RadioEngine::TRadioFrequencyEventReason aReason,
                                                 TInt aError )
{
    Q_UNUSED( aReason );
    LOG_FORMAT( "RadioEngineWrapperPrivate::FrequencyEventL - Freq: %d, TuneReason: %d, Err: %d", aFrequency, mTuneReason, aError );

    if ( !aError ) {
        const uint frequency = static_cast<uint>( aFrequency );
        RUN_NOTIFY_LOOP( mObservers, tunedToFrequency( frequency, mTuneReason ) );
    } else {
        RUN_NOTIFY_LOOP( mObservers, tunedToFrequency( mEngineHandler->minFrequency(), TuneReason::StationScanNoStationsFound ) ); // no frequencies found
    }
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::VolumeEventL( TInt aVolume, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, volumeChanged( aVolume ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::MuteEventL( TBool aMuteState, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, muteChanged( aMuteState ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioModeEventL( TInt DEBUGVAR( aAudioMode ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::AudioModeEventL, AudioMode: %d, Error: %d", aAudioMode, aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AntennaEventL( TBool aAntennaAttached, TInt aError )
{
    Q_UNUSED( aError );
    RUN_NOTIFY_LOOP( mObservers, antennaStatusChanged( aAntennaAttached ) );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::AudioRoutingEventL( TInt aAudioDestination, TInt aError )
{
    if( !aError )
    {
        mUseLoudspeaker = aAudioDestination == RadioEngine::ERadioSpeaker;
        RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( mUseLoudspeaker ) );
    }
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::SeekingEventL( TInt DEBUGVAR( aSeekingState ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::SeekingEventL, aSeekingState: %d, Error: %d", aSeekingState, aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::RegionEventL( TInt DEBUGVAR( aRegion ), TInt DEBUGVAR( aError ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::RegionEventL, aRegion: %d, Error: %d", aRegion, aError );
}

/*!
 * \reimp
 */
void RadioEngineWrapperPrivate::HandleSystemEventL( TRadioSystemEventType DEBUGVAR( aEventType ) )
{
    LOG_FORMAT( "RadioEngineWrapperPrivate::HandleSystemEventL, Event: %d", aEventType );
//    ERadioHeadsetConnected,         ///< Headset was connected
//    ERadioHeadsetDisconnected,      ///< Headset was disconnected
//    ERadioNetworkCoverageUp,        ///< Network coverage detected
//    ERadioNetworkCoverageDown,      ///< Network coverage lost
//    ERadioCallActivated,            ///< Call activated or ringing
//    ERadioCallDeactivated,          ///< Call disconnected
//    ERadioEmergencyCallActivated,   ///< Call activated or ringing
//    ERadioEmergencyCallDeactivated, ///< Call disconnected
//    ERadioLowDiskSpace,             ///< Low disk space
//    ERadioAudioRoutingHeadset,      ///< Audio routed through headset
//    ERadioAudioRoutingSpeaker,      ///< Audio routed through speaker ( IHF )
//    ERadioAudioResourcesAvailable,  ///< Audio resources have become available
//    ERadioAudioAutoResumeForbidden  ///< Audio auto resuming is forbidden
}
