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

// User includes
#include "radioscannerengine.h"
#include "radioscannerengine_p.h"
#include "radiouiengine.h"
#include "radiouiengine_p.h"
#include "radioenginewrapper.h"
#include "radiostationhandlerif.h"
#include "radiostationmodel.h"
#include "radiostation.h"
#include "radiologger.h"

/*!
 *
 */
RadioScannerEngine::RadioScannerEngine( RadioUiEnginePrivate& uiEngine ) :
    QObject( &uiEngine.api() ),
    d_ptr( new RadioScannerEnginePrivate( this, uiEngine ) )
{
}

/*!
 *
 */
RadioScannerEngine::~RadioScannerEngine()
{
    cancel();
}

/*!
 * Starts the scanning from minimum frequency
 */
void RadioScannerEngine::startScanning()
{
    Q_D( RadioScannerEngine );
    d->mUiEngine.cancelSeeking();

//    d->mUiEngine.wrapper().setRdsEnabled( false );

    d->mIsScanning = true;

    if ( !d->mUiEngine.api().isMuted() ) {
        d->mUiEngine.api().setMute( true );
        d->mMutedByScanner = true;
    }

    d->mUiEngine.api().emitSeekingStarted( Seek::Up );

    d->mUiEngine.api().stationModel().removeAll( RadioStationModel::RemoveLocalStations );
    d->mLastFoundFrequency = d->mUiEngine.api().minFrequency();

    if ( d->mUiEngine.wrapper().currentFrequency() == d->mLastFoundFrequency ) {
        // Engine was already at the minimun frequency so start scanning
        d->mUiEngine.wrapper().startSeeking( Seek::Up, TuneReason::StationScan );
    } else {
        // Engine must be initialized to minimum frequency before scanning can start
        d->mUiEngine.wrapper().setFrequency( d->mLastFoundFrequency, TuneReason::StationScanInitialization );
    }
}

/*!
 * Continues the scanning upwards from current frequency
 */
void RadioScannerEngine::continueScanning()
{
    Q_D( RadioScannerEngine );
    d->mUiEngine.wrapper().startSeeking( Seek::Up, TuneReason::StationScan );
}

/*!
 * Checks if the scanning is ongoing
 */
bool RadioScannerEngine::isScanning() const
{
    Q_D( const RadioScannerEngine );
    return d->mIsScanning;
}

/*!
 * Cancels the scanning process
 */
void RadioScannerEngine::cancel()
{
    Q_D( RadioScannerEngine );
    if ( isScanning() ) {
        d->mIsScanning = false;
        d->mUiEngine.cancelSeeking();
    }

    if ( d->mMutedByScanner ) {
        d->mUiEngine.api().setMute( false );
        d->mMutedByScanner = false;
    }

//    d->mUiEngine.wrapper().setRdsEnabled( true );
}

/*!
 * Adds a new station that was found
 */
void RadioScannerEngine::addScannedFrequency( const uint frequency )
{
    Q_D( RadioScannerEngine );
    if ( frequency > d->mLastFoundFrequency ) {
        // Station has been found normally
        d->mLastFoundFrequency = frequency;
        d->addFrequencyAndReport( frequency );
    } else if ( frequency == d->mUiEngine.api().minFrequency() ) {
        // Special case. A station has been found in the mininmum frequency
        d->addFrequencyAndReport( frequency );
    } else {
        // Seeking looped around the frequency band. Send invalid station as indicator that the scanning should stop
        emit stationFound( RadioStation() );
    }
}

/*!
 *
 */
void RadioScannerEngine::emitStationFound( const RadioStation& station )
{
    emit stationFound( station );
}
