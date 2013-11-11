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
#include "radiofrequencystrip.h"
#include "radiouiengine.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radioutil.h"
#include "radiouiloader.h"
#include "radiologger.h"

RadioFrequencyStrip::RadioFrequencyStrip() :
    RadioFrequencyStripBase(),
    mUiEngine( NULL )
{
    RadioUtil::setFrequencyStrip( this );
}

/*!
 *
 */
void RadioFrequencyStrip::init( RadioUiEngine* engine, RadioUiLoader& uiLoader )
{
    mUiEngine = engine;
    initbase( mUiEngine->minFrequency(), mUiEngine->maxFrequency(),
              mUiEngine->frequencyStepSize(), RadioUiEngine::lastTunedFrequency() );

    createButtonsFromDocml( uiLoader, DOCML::MV_NAME_PREV_BUTTON, DOCML::MV_NAME_NEXT_BUTTON );

    RadioStationModel* stationModel = &mUiEngine->stationModel();
    Radio::connect( stationModel,   SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(updateStation(QModelIndex,int,int)) );
    Radio::connect( stationModel,   SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                    this,           SLOT(removeStation(QModelIndex,int,int)) );
    Radio::connect( stationModel,   SIGNAL(modelReset()),
                    this,           SLOT(initEmptyItems()) );
}

/*!
 *
 */
void RadioFrequencyStrip::setFrequency( const uint newFrequency, int reason, Scroll::Direction direction )
{
    RadioFrequencyStripBase::setFrequency( newFrequency, reason, direction );
}

/*!
 * \reimp
 */
bool RadioFrequencyStrip::isInitialized() const
{
    return mUiEngine != NULL;
}

/*!
 * \reimp
 */
QList<FrequencyStrip::StationMarker> RadioFrequencyStrip::stationsInRange( uint minFrequency, uint maxFrequency ) const
{
    QList<RadioStation> stations = mUiEngine->stationsInRange( minFrequency, maxFrequency );
    QList<FrequencyStrip::StationMarker> markers;
    foreach ( const RadioStation& station, stations ) {
        markers.append( FrequencyStrip::StationMarker( station.frequency(), station.isFavorite() ) );
    }
    return markers;
}

/*!
 * \reimp
 */
void RadioFrequencyStrip::emitFrequencyChanged( uint frequency, int reason, int direction )
{
    int tuneReason = TuneReason::Unspecified;
    if ( reason == FrequencyStrip::ManualSeekUpdate ) {
        tuneReason = TuneReason::ManualSeekUpdate;
    } else if ( reason == FrequencyStrip::ManualSeekTune ) {
        tuneReason = TuneReason::ManualSeekTune;
    }

    emit frequencyChanged( frequency, tuneReason, direction );
}

/*!
 *
 */
void RadioFrequencyStrip::skipToPrevious()
{
    emit skipRequested( StationSkip::Previous );
}

/*!
 *
 */
void RadioFrequencyStrip::skipToPreviousFavorite()
{
    emit skipRequested( StationSkip::PreviousFavorite );
}

/*!
 *
 */
void RadioFrequencyStrip::skipToNext()
{
    emit skipRequested( StationSkip::Next );
}

/*!
 *
 */
void RadioFrequencyStrip::skipToNextFavorite()
{
    emit skipRequested( StationSkip::NextFavorite );
}

/*!
 *
 */
void RadioFrequencyStrip::seekDown()
{
    emit seekRequested( Seek::Down );
}


/*!
 *
 */
void RadioFrequencyStrip::seekUp()
{
    emit seekRequested( Seek::Up );
}

/*!
 * \reimp
 */
bool RadioFrequencyStrip::isScanning() const
{
    return RadioUtil::scanStatus() == Scan::ScanningInMainView;
}

/*!
 * \reimp
 */
uint RadioFrequencyStrip::frequencyAtIndex( int index ) const
{
    RadioStationModel& model = mUiEngine->stationModel();
    return model.data( model.index( index, 0, QModelIndex() ),
                       RadioRole::RadioStationRole ).value<RadioStation>().frequency();
}
