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
#include <QTimer>
#include <QSettings>

// User includes
#include "radioenginewrapper_win32_p.h"
#include "radiosettings.h"
#include "radiosettings_p.h"
#include "radiologger.h"
#include "radio_global.h"
#include "radiostationhandlerif.h"
#include "radioenginewrapperobserver.h"
//#include "t_radiodataparser.h"

static RadioEngineWrapperPrivate* theInstance = 0;

const QString KKeyFrequency = "CurrentFreq";
const QString KKeyOffline = "Offline";

const uint KScanFrequencies[] = {
    87500000,
    88100000,
    89400000,
    96000000,
    97600000,
    100600000,
    101300000,
    102600000,
    103500000,
    104100000,
    105500000,
    107500000
};

const int KScanFrequencyCount = sizeof( KScanFrequencies ) / sizeof( KScanFrequencies[0] );

/*!
 *
 */
RadioEngineWrapperPrivate::RadioEngineWrapperPrivate( RadioEngineWrapper* wrapper,
                                                      RadioStationHandlerIf& stationHandler ) :
    q_ptr( wrapper ),
    mStationHandler( stationHandler ),
    mTuneTimer( new QTimer( this ) ),
    mTuneReason( 0 ),
    mUseLoudspeaker( false ),
    mAntennaAttached( true ),
    mFrequency( 0 ),
    mNextFrequency( 0 ),
    mVolume( 5 ),
    mMaxVolume( 10000 ),
    mFrequencyStepSize( 50000 ),
    mRegionId( RadioRegion::Default ),
    mMinFrequency( 87500000 ),
    mMaxFrequency( 108000000 ),
    mManualSeekMode( false )
{
    ::theInstance = this;
    mEngineSettings.reset( new QSettings( "Nokia", "QtFmRadio" ) );
    mFrequency = mEngineSettings->value( KKeyFrequency ).toUInt();
    if ( mFrequency == 0 ) {
        mFrequency = mMinFrequency;
    }

    Radio::connect( mTuneTimer, SIGNAL(timeout()), this, SLOT(frequencyEvent()) );
    mTuneTimer->setSingleShot( true );    
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
 *
 */
RadioEngineWrapperPrivate* RadioEngineWrapperPrivate::instance()
{
    return ::theInstance;
}

/*!
 * Initializes the private implementation
 */
bool RadioEngineWrapperPrivate::init()
{
    mUseLoudspeaker = false;
    if ( !mUseLoudspeaker ) {
        RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( false ) );
    }

    parseData();
    return true;
}

/*!
 * Returns the settings handler owned by the engine
 */
RadioSettingsIf& RadioEngineWrapperPrivate::settings()
{
    if ( !mSettings ) {
        mSettings.reset( new RadioSettings() );
//        mSettings->d_func()->init( &mEngineHandler->ApplicationSettings() );
    }
    return *mSettings;
}

/*!
 * Tunes to the given frequency
 */
void RadioEngineWrapperPrivate::setFrequency( uint frequency, const int reason )
{
    mNextFrequency = frequency;
    mTuneReason = reason;

    mTuneTimer->stop();
    if ( !mManualSeekMode ) {
        mTuneTimer->start( 500 );
    } else {
        frequencyEvent();
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
    mNextFrequency = 0;

    // Find the previous and next favorite from current frequency
    uint previous = 0;
    uint next = 0;
    for( int i = 0; i < KScanFrequencyCount; ++i ) {
        const uint testFreq = KScanFrequencies[i];
        if ( KScanFrequencies[i] > mFrequency ) {
            next = testFreq;
            break;
        }
        previous = testFreq;
    }


    if ( direction == Seek::Up ) {
        if ( next == 0 ) {
            next = KScanFrequencies[0];
        }
        mNextFrequency = next;
    } else {
        if ( previous == 0 ) {
            previous = KScanFrequencies[KScanFrequencyCount - 1];
        }
        mNextFrequency = previous;
    }

    mTuneTimer->start( 1000 );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::cancelSeeking()
{
    mTuneTimer->stop();
    mNextFrequency = 0;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::toggleAudioRoute()
{
    mUseLoudspeaker = !mUseLoudspeaker;
    RUN_NOTIFY_LOOP( mObservers, audioRouteChanged( mUseLoudspeaker ) );
}

/*!
 *
 */
QString RadioEngineWrapperPrivate::dataParsingError() const
{
    return mParsingError;
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setHeadsetStatus( bool connected )
{
    mAntennaAttached = connected;
    RUN_NOTIFY_LOOP( mObservers, antennaStatusChanged( mAntennaAttached ) );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setVolume( int volume )
{
    mVolume = volume;
    RUN_NOTIFY_LOOP( mObservers, volumeChanged( mVolume ) );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::addSong( const QString& artist, const QString& title )
{
    QString radioText = QString( "Now Playing: %1 - %2" ).arg( artist ).arg( title );
    mArtist = artist;
    mTitle = title;

    const uint frequency = mStationHandler.currentFrequency();
    mStationHandler.setCurrentRadioText( frequency, radioText );

    QTimer::singleShot( 500, this, SLOT(addSongTags()) );
}

/*!
 *
 */
void RadioEngineWrapperPrivate::clearSong()
{
    mStationHandler.setCurrentRadioText( mStationHandler.currentFrequency(), "" );
}

/*!
 *
 */
bool RadioEngineWrapperPrivate::isOffline() const
{
    return mEngineSettings->value( KKeyOffline, false ).toBool();
}

/*!
 *
 */
void RadioEngineWrapperPrivate::setOffline( bool offline )
{
    mEngineSettings->setValue( KKeyOffline, offline );
}

/*!
 * Private slot
 */
void RadioEngineWrapperPrivate::frequencyEvent()
{
    if ( mNextFrequency > 0 ) {
        mFrequency = mNextFrequency;
        mEngineSettings->setValue( KKeyFrequency, mFrequency );
    }

    if ( !mManualSeekMode ) {
        RUN_NOTIFY_LOOP( mObservers, tunedToFrequency( mNextFrequency, mTuneReason ) );
    }
}

/*!
 * Private slot
 */
void RadioEngineWrapperPrivate::addSongTags()
{
    const uint frequency = mStationHandler.currentFrequency();
    mStationHandler.setCurrentRadioTextPlus( frequency, RtPlus::Artist, mArtist );
    mStationHandler.setCurrentRadioTextPlus( frequency, RtPlus::Title, mTitle );
    mArtist = "";
    mTitle = "";
}

/*!
 *
 */
void RadioEngineWrapperPrivate::parseData()
{
    /*
    mDataParser.reset( new T_RadioDataParser() );
    bool ok = mDataParser->parse();
    if ( !ok ) {
        mParsingError = mDataParser->errorString();
    } else {
        if ( mDataParser->mEngineSettings.mMaxVolume > 0 ) {
            mMaxVolume = mDataParser->mEngineSettings.mMaxVolume;
        }

        if ( mDataParser->mEngineSettings.mFrequencyStepSize > 0 ) {
            mFrequencyStepSize = mDataParser->mEngineSettings.mFrequencyStepSize;
        }

        if ( mDataParser->mEngineSettings.mRegionId >= 0 ) {
            mRegionId = static_cast<RadioRegion::Region>( mDataParser->mEngineSettings.mRegionId );
        }

        if ( mDataParser->mEngineSettings.mMinFrequency > 0 ) {
            mMinFrequency = mDataParser->mEngineSettings.mMinFrequency;
        }

        if ( mDataParser->mEngineSettings.mMaxFrequency > 0 ) {
            mMaxFrequency = mDataParser->mEngineSettings.mMaxFrequency;
        }
    }
*/
}
