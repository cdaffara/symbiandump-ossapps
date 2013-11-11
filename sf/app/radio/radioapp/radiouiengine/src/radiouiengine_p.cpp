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
#include <QApplication>
#include <QStringList>
#include <QDateTime>
#ifndef BUILD_WIN32
#   include <XQSettingsManager>
#   include <XQPublishAndSubscribeSettingsKey>
#   include <XQPublishAndSubscribeUtils>
#endif

// User includes
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationmodel.h"
#include "radiostationmodel_p.h"
#include "radiohistorymodel.h"
#include "radiopresetstorage.h"
#include "radiosettings.h"
#include "radiostation.h"
#include "radioscannerengine.h"
#include "radiostationhandlerif.h"
#include "radiocontrolservice.h"
#include "radiomonitorservice.h"
#include "radioservicedef.h"
#include "radiologger.h"

/*!
 *
 */
RadioUiEnginePrivate::RadioUiEnginePrivate( RadioUiEngine* engine ) :
    q_ptr( engine ),
    mPowerOffTimer( NULL )
{
}

/*!
 *
 */
RadioUiEnginePrivate::~RadioUiEnginePrivate()
{
#ifndef BUILD_WIN32
    XQSettingsManager settingsManager;
    XQPublishAndSubscribeUtils utils( settingsManager );
    XQPublishAndSubscribeSettingsKey radioStartupKey( KRadioPSUid, KRadioStartupKey );
    bool deleted = utils.deleteProperty( radioStartupKey );
    LOG_ASSERT( deleted, LOG( "RadioUiEnginePrivate::~RadioUiEnginePrivate(). Failed to remove P&S key" ) );
#endif
}

/*!
 *
 */
RadioUiEngine& RadioUiEnginePrivate::api()
{
    Q_Q( RadioUiEngine );
    return *q;
}

/*!
 *
 */
bool RadioUiEnginePrivate::init()
{
    mControlService.reset( new RadioControlService( *q_ptr ) );
    mMonitorService.reset( new RadioMonitorService( *this ) );
    mStationModel.reset( new RadioStationModel( *this ) );

    mEngineWrapper.reset( new RadioEngineWrapper( mStationModel->stationHandlerIf() ) );
    if ( !mEngineWrapper->init() ) {
        return false;
    }
    mEngineWrapper->addObserver( this );

    mPresetStorage.reset( new RadioPresetStorage() );
    mStationModel->initialize( mPresetStorage.data(), mEngineWrapper.data() );
    mHistoryModel.reset( new RadioHistoryModel( *q_ptr ) );

#ifndef BUILD_WIN32
    // Write the startup timestamp to P&S key for the homescreen widget
    XQSettingsManager settingsManager;
    XQPublishAndSubscribeUtils utils( settingsManager );
    XQPublishAndSubscribeSettingsKey radioStartupKey( KRadioPSUid, KRadioStartupKey );
    bool defined = utils.defineProperty( radioStartupKey, XQSettingsManager::TypeInt );
    if ( defined ) {
        settingsManager.writeItemValue( radioStartupKey, (int)QDateTime::currentDateTime().toTime_t() );
    }
#endif

    mMonitorService->init();

    return true;
}

/*!
 *
 */
void RadioUiEnginePrivate::cancelSeeking()
{
    mEngineWrapper->cancelSeeking();
}

/*!
 *
 */
RadioEngineWrapper& RadioUiEnginePrivate::wrapper()
{
    return *mEngineWrapper;
}

/*!
 *
 */
void RadioUiEnginePrivate::tunedToFrequency( uint frequency, int reason )
{
    Q_Q( RadioUiEngine );
    q->emitTunedToFrequency( frequency, reason );
}

/*!
 *
 */
void RadioUiEnginePrivate::radioStatusChanged( bool radioIsOn )
{
    Q_Q( RadioUiEngine );
    q->emitRadioStatusChanged( radioIsOn );
}

/*!
 *
 */
void RadioUiEnginePrivate::rdsAvailabilityChanged( bool available )
{
    Q_Q( RadioUiEngine );
    q->emitRdsAvailabilityChanged( available );
}

/*!
 *
 */
void RadioUiEnginePrivate::increaseVolume()
{
    Q_Q( RadioUiEngine );
    if( q->isScanning() ){
        // volume not changed while scanning
    } else {
        mEngineWrapper->increaseVolume();
    }
}

/*!
 *
 */
void RadioUiEnginePrivate::decreaseVolume()
{
    Q_Q( RadioUiEngine );
    if( q->isScanning() ) {
        // volume not changed while scanning
    } else {
        mEngineWrapper->decreaseVolume();
    }
}

/*!
 *
 */
void RadioUiEnginePrivate::volumeChanged( int volume )
{
    Q_Q( RadioUiEngine );
    q->emitVolumeChanged( volume );
}

/*!
 *
 */
void RadioUiEnginePrivate::muteChanged( bool muted )
{
    Q_Q( RadioUiEngine );
    q->emitMuteChanged( muted );
}

/*!
 *
 */
void RadioUiEnginePrivate::audioRouteChanged( bool loudspeaker )
{
    Q_Q( RadioUiEngine );
    q->emitAudioRouteChanged( loudspeaker );
}

/*!
 *
 */
void RadioUiEnginePrivate::antennaStatusChanged( bool connected )
{
    Q_Q( RadioUiEngine );
    q->emitAntennaStatusChanged( connected );
}

/*!
 *
 */
void RadioUiEnginePrivate::skipPrevious()
{
    skip( StationSkip::PreviousFavorite, 0, TuneReason::SkipFromEngine );
}

/*!
 *
 */
void RadioUiEnginePrivate::skipNext()
{
    skip( StationSkip::NextFavorite, 0, TuneReason::SkipFromEngine );
}

/*!
 * Tunes to next or previous station
 */
uint RadioUiEnginePrivate::skip( StationSkip::Mode mode, uint startFrequency, const int reason )
{
    LOG_FORMAT( "RadioUiEnginePrivate::skip: mode: %d", mode );
    if ( startFrequency == 0 ) {
        startFrequency = mEngineWrapper->currentFrequency();
    }

    const int favoriteCount = mStationModel->favoriteCount();
    if ( favoriteCount < 2 ) {
        if ( mode == StationSkip::NextFavorite ) {
            mode = StationSkip::Next;
        } else if ( mode == StationSkip::PreviousFavorite ) {
            mode = StationSkip::Previous;
        }
    }

    const RadioStation station = mStationModel->findClosest( startFrequency, mode );
    if ( station.isValid() ) {
        const uint newFrequency = station.frequency();

        LOG_FORMAT( "RadioUiEnginePrivate::skip. CurrentFreq: %u, tuning to: %u", startFrequency, newFrequency );
        mEngineWrapper->setFrequency( newFrequency, reason );
        return newFrequency;
    }
    return startFrequency;
}

