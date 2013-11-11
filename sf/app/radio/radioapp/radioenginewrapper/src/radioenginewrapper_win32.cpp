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
#include "radioenginewrapper_win32_p.h"
#include "radioenginewrapperobserver.h"

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
    d->mObservers.append( observer );
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
    return d->mRegionId;
}

/*!
 * Returns the minimum frequency
 */
uint RadioEngineWrapper::minFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mMinFrequency;
}

/*!
 * Returns the maximum frequency
 */
uint RadioEngineWrapper::maxFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mMaxFrequency;
}

/*!
 * Returns the frequency step size from the selected region
 */
uint RadioEngineWrapper::frequencyStepSize() const
{
    Q_D( const RadioEngineWrapper );
    return d->mFrequencyStepSize;
}

/*!
 * Returns the frequency step size from the selected region
 */
bool RadioEngineWrapper::isFrequencyValid( uint frequency ) const
{
    Q_UNUSED( frequency );
    return frequency >= minFrequency() && frequency <= maxFrequency() && frequency % frequencyStepSize() == 0;
}

/*!
 * Checks if the radio engine is on
 */
bool RadioEngineWrapper::isRadioOn() const
{
    return true;
}

/*!
 * Returns the currently tuned frequency
 */
uint RadioEngineWrapper::currentFrequency() const
{
    Q_D( const RadioEngineWrapper );
    return d->mFrequency;
}

/*!
 * Returns the mute status
 */
bool RadioEngineWrapper::isMuted() const
{
    return false;
}

/*!
 * Returns the antenna connection status
 */
bool RadioEngineWrapper::isAntennaAttached() const
{
    Q_D( const RadioEngineWrapper );
    return d->mAntennaAttached;
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
    d->mManualSeekMode = manualSeek;

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
    return d->mManualSeekMode;
}

/*!
 *
 */
void RadioEngineWrapper::setRdsEnabled( bool rdsEnabled )
{
    Q_UNUSED( rdsEnabled );
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
 *
 */
void RadioEngineWrapper::increaseVolume()
{
}

/*!
 *
 */
void RadioEngineWrapper::decreaseVolume()
{
}

/*!
 * volume update command for the engine
 */
void RadioEngineWrapper::setVolume( int volume )
{
    Q_D( RadioEngineWrapper );
    d->setVolume( volume );
}

/*!
 *
 */
void RadioEngineWrapper::setMute( bool muted, bool updateSettings )
{
    Q_UNUSED( muted );
    Q_UNUSED( updateSettings );
}

/*!
 *
 */
void RadioEngineWrapper::toggleAudioRoute()
{
    Q_D( RadioEngineWrapper );
    d->toggleAudioRoute();
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
    d->cancelSeeking();
}
