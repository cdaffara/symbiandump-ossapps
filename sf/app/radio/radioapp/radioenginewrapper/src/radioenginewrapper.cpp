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
#include "radioenginewrapper.h"
#include "radioenginewrapperobserver.h"
#include "radioenginewrapper_p.h"
#include "radioenginehandler.h"

/*!
 * Constructor
 */
RadioEngineWrapper::RadioEngineWrapper( RadioStationHandlerIf& stationHandler ) :
    d_ptr( new RadioEngineWrapperPrivate( this, stationHandler ) )
{
}

/*!
 * Destructor
 */
RadioEngineWrapper::~RadioEngineWrapper()
{
}

/*!
 *
 */
bool RadioEngineWrapper::init()
{
    Q_D( RadioEngineWrapper );
    return d->init();
}

/*!
 *
 */
void RadioEngineWrapper::addObserver( RadioEngineWrapperObserver* observer )
{
    Q_D( RadioEngineWrapper );
    d->mObservers.prepend( observer );
}

/*!
 *
 */
void RadioEngineWrapper::removeObserver( RadioEngineWrapperObserver* observer )
{
    Q_D( RadioEngineWrapper );
    d->mObservers.removeAll( observer );
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettingsIf& RadioEngineWrapper::settings()
{
    Q_D( RadioEngineWrapper );
    return d->settings();
}

/*!
 * Returns the selected radio region
 */
RadioRegion::Region RadioEngineWrapper::region() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->region();
}

/*!
 * Returns the minimum frequency
 */
uint RadioEngineWrapper::minFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->minFrequency();
}

/*!
 * Returns the maximum frequency
 */
uint RadioEngineWrapper::maxFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->maxFrequency();
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioEngineWrapper::frequencyStepSize() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->frequencyStepSize();
}

/*!
 * Returns true if frequency is valid, otherwise false
 */
bool RadioEngineWrapper::isFrequencyValid( uint frequency ) const
{
    return frequency >= minFrequency() && frequency <= maxFrequency();
}

/*!
 * Checks if the radio engine is on
 */
bool RadioEngineWrapper::isRadioOn() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->isRadioOn();
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioEngineWrapper::currentFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->currentFrequency();
}

/*!
 * Returns the mute status
 */
bool RadioEngineWrapper::isMuted() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->isMuted();
}

/*!
 * Returns the antenna connection status
 */
bool RadioEngineWrapper::isAntennaAttached() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->isAntennaAttached();
}

/*!
 * Returns the "use loudspeaker" status
 */
bool RadioEngineWrapper::isUsingLoudspeaker() const
{
    Q_D( const RadioEngineWrapper );
    return d->mUseLoudspeaker;
}

/*!
 * Sets or unsets the engine to manual seek mode
 */
void RadioEngineWrapper::setManualSeekMode( bool manualSeek )
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->setManualSeekMode( manualSeek );
    if ( !manualSeek ) {
        RUN_NOTIFY_LOOP( d->observers(), tunedToFrequency( currentFrequency(), TuneReason::ManualSeekTune ) );
    }
}

/*!
 * Checks if the engine is in manual seek mode
 */
bool RadioEngineWrapper::isInManualSeekMode() const
{
    Q_D( const RadioEngineWrapper );
    return d->mEngineHandler->isInManualSeekMode();
}

/*!
 *
 */
void RadioEngineWrapper::setRdsEnabled( bool rdsEnabled )
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->setRdsEnabled( rdsEnabled );
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapper::setFrequency( uint frequency, const int reason )
{
    Q_D( RadioEngineWrapper );
    d->setFrequency( frequency, reason );
}

/*!
 * volume increase command for the engine
 */
void RadioEngineWrapper::increaseVolume()
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->increaseVolume();
}

/*!
 * volume decrease command for the engine
 */
void RadioEngineWrapper::decreaseVolume()
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->decreaseVolume();
}

/*!
 * volume update command for the engine
 */
void RadioEngineWrapper::setVolume( int volume )
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->setVolume( volume );
}

/*!
 *
 */
void RadioEngineWrapper::setMute( bool muted, bool updateSettings )
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->setMute( muted, updateSettings );
}

/*!
 *
 */
void RadioEngineWrapper::toggleAudioRoute()
{
    Q_D( RadioEngineWrapper );
    d->mUseLoudspeaker = !d->mUseLoudspeaker;
    d->mEngineHandler->setAudioRouteToLoudspeaker( d->mUseLoudspeaker );
}

/*!
 *
 */
void RadioEngineWrapper::startSeeking( Seek::Direction direction, const int reason )
{
    Q_D( RadioEngineWrapper );
    d->startSeeking( direction, reason );
}

/*!
 *
 */
void RadioEngineWrapper::cancelSeeking()
{
    Q_D( RadioEngineWrapper );
    d->mEngineHandler->cancelSeek();
}
