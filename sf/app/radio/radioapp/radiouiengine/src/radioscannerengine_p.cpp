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
#include "radioscannerengine_p.h"
#include "radioscannerengine.h"
#include "radiouiengine_p.h"
#include "radiouiengine.h"
#include "radiostationhandlerif.h"
#include "radioenginewrapper.h"
#include "radiostation.h"
#include "radiostationmodel.h"

/*!
 *
 * @param scanner
 * @param uiEngine
 */
RadioScannerEnginePrivate::RadioScannerEnginePrivate( RadioScannerEngine* scanner, RadioUiEnginePrivate& uiEngine ) :
    q_ptr( scanner ),
    mUiEngine( uiEngine ),
    mLastFoundFrequency( 0 ),
    mMutedByScanner( false ),
    mIsScanning( false )
{
    mUiEngine.wrapper().addObserver( this );
}

/*!
 *
 */
RadioScannerEnginePrivate::~RadioScannerEnginePrivate()
{
    mUiEngine.wrapper().removeObserver( this );
}

/*!
 * \reimp
 */
void RadioScannerEnginePrivate::tunedToFrequency( uint frequency, int reason )
{
    if ( !mIsScanning ) {
        return;
    }

    Q_Q( RadioScannerEngine );
    if ( reason == TuneReason::StationScanInitialization ) {
        mUiEngine.wrapper().startSeeking( Seek::Up, TuneReason::StationScan );
    } else if ( reason == TuneReason::StationScan ) {
        if ( frequency == mLastFoundFrequency ) {
            // Stop scanning
            q->emitStationFound( RadioStation() );
        } else if ( frequency > mLastFoundFrequency ) {
            // Station has been found normally
            mLastFoundFrequency = frequency;
            addFrequencyAndReport( frequency );
        } else if ( frequency == mUiEngine.api().minFrequency() ) {
            // Special case. A station has been found in the mininmum frequency
            addFrequencyAndReport( frequency );
        } else {
            // Seeking looped around the frequency band. Send invalid station as indicator that the scanning should stop
            q->emitStationFound( RadioStation() );
        }
    } else {
        q->emitStationFound( RadioStation() );
    }
}

/*!
 *
 */
void RadioScannerEnginePrivate::addFrequencyAndReport( const uint frequency )
{
    RadioStationModel& stationModel = mUiEngine.api().stationModel();
    stationModel.stationHandlerIf().addScannedFrequency( frequency );

    // Return value of findFrequency() is intentionally ignored. The station was just added
    // to the model in the previous line so it should be found and if it isn't then an
    // empty station is sent with the signal and scanner will stop the scanning process.
    RadioStation station;
    stationModel.findFrequency( frequency, station );

    Q_Q( RadioScannerEngine );
    q->emitStationFound( station );
}

