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
#include <HbProgressDialog>
#include <QTimer>
#include <HbLabel>
#include <HbPushButton>
#include <HbMessageBox>

// User includes
#include "radiofrequencyscanner.h"
#include "radioscannerengine.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationmodel.h"
#include "radiofrequencystrip.h"
#include "radiostationcarousel.h"
#include "radioutil.h"
#include "radiomainview.h"

// Constants
const int EXTRA_TO_PROGRESS_MAX_VALUE = 100000;
const int CAROUSEL_FINISH_SCROLL_TIME = 1000;
const int STRIP_FINISH_SCROLL_TIME = 1100;
const int CAROUSEL_SCROLL_TIME = 1000;
const int STRIP_SCROLL_TIME = 1100;
const int START_DELAY = 1000;

/*!
 *
 */
RadioFrequencyScanner::RadioFrequencyScanner( RadioUiEngine& uiEngine, QObject* parent ) :
    QObject( parent ),
    mUiEngine( uiEngine ),
    mInMainView( parent->metaObject()->className() == RadioMainView::staticMetaObject.className() ),
    mScannerEngine( mUiEngine.createScannerEngine() ),
    mStripScrollTime( 0 ),
    mCarouselScrollTime( 0 ),
    mIsAlive( false ),
    mUserCanceled( false )
{
    RadioUtil::setFrequencyScanner( this );

    Radio::connect( mScannerEngine.data(),  SIGNAL(stationFound(RadioStation)),
                    this,                   SLOT(updateScanProgress(RadioStation)) );
}

/*!
 *
 */
RadioFrequencyScanner::~RadioFrequencyScanner()
{
    RadioUtil::setScanStatus( Scan::NotScanning );
}

/*!
 *
 */
void RadioFrequencyScanner::startScanning()
{
    mIsAlive = true;
    RadioFrequencyStrip* frequencyStrip = RadioUtil::frequencyStrip();
    RadioStationCarousel* carousel = RadioUtil::carousel();

    if ( mInMainView ) {

        // If this is the first time start, bring application to foreground
        if ( mUiEngine.isFirstTimeStart() ) {
            static_cast<RadioMainView*>( parent() )->bringToForeground();
        }

        RadioUtil::setScanStatus( Scan::ScanningInMainView );
        mStripScrollTime = frequencyStrip->autoScrollTime();
        mCarouselScrollTime = carousel->autoScrollTime();

        carousel->setScanningMode( true );
        carousel->setAutoScrollTime( CAROUSEL_SCROLL_TIME );
        frequencyStrip->setAutoScrollTime( STRIP_SCROLL_TIME );

        Radio::connect( carousel,               SIGNAL(scanAnimationFinished()),
                        this,                   SLOT(continueScanning()) );

        static_cast<RadioMainView*>( parent() )->setScanningMode( true );
        frequencyStrip->setScanningMode( true );
    } else {
        RadioUtil::setScanStatus( Scan::ScanningInStationsView );
        mScanningProgressNote = new HbProgressDialog( HbProgressDialog::ProgressDialog );
        mScanningProgressNote.data()->setModal( true );
        mScanningProgressNote.data()->setAutoClose( true );

        // Add some extra to the maximum value to allow room for the station at the low band edge
        mScanningProgressNote.data()->setRange( mUiEngine.minFrequency(), mUiEngine.maxFrequency() + EXTRA_TO_PROGRESS_MAX_VALUE );
        mScanningProgressNote.data()->setProgressValue( mUiEngine.minFrequency() );
        mScanningProgressNote.data()->setText( hbTrId( "txt_rad_info_searching_local_stations_please_wait" ) );
        mScanningProgressNote.data()->setAttribute( Qt::WA_DeleteOnClose, true );
        mScanningProgressNote.data()->open();

        Radio::connect( mScanningProgressNote.data(),   SIGNAL(cancelled()),
                        this,                           SLOT(cancelScanning()) );
    }

    QTimer::singleShot( START_DELAY, this, SLOT(delayedStart()) );
}

/*!
 *
 */
bool RadioFrequencyScanner::isAlive() const
{
    return mIsAlive;
}

/*!
 * Public slot
 *
 */
void RadioFrequencyScanner::cancelScanning()
{
    mUserCanceled = true;
    RadioUtil::carousel()->cancelAnimation();
    finishScanning();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::delayedStart()
{
    mScannerEngine->startScanning();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::updateScanProgress( const RadioStation& station )
{
    if ( !station.isValid() ) {
        finishScanning();
        return;
    }

    const uint frequency = station.frequency();
    LOG_FORMAT( "RadioFrequencyScanner::updateScanAndSaveProgress frequency: %d", frequency );

    if ( mInMainView ) {

        RadioUtil::frequencyStrip()->setFrequency( frequency, TuneReason::StationScan );
        RadioUtil::carousel()->animateNewStation( station );

    } else {
        // Check for special case that can happen during scanning.
        // If there is a valid radio station at the low frequency band edge it will be reported last after
        // all of the higher frequencies. We don't update the progress value here because the value would
        // be lower than the previous one. The progress value is set to maximum when the scanner finishes.
        if ( frequency != mUiEngine.minFrequency() ) {
            mScanningProgressNote.data()->setProgressValue( frequency );
        }

        mScannerEngine->continueScanning();
    }
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::continueScanning()
{
    if ( !mUserCanceled ) {
        RadioUtil::frequencyStrip()->addScannedStation( mUiEngine.stationModel().currentStation().frequency() );
        mScannerEngine->continueScanning();
    }
}

/*!
 * Private slot
 *
 */
void RadioFrequencyScanner::restoreUiControls()
{
    if ( mInMainView ) {
        disconnect( RadioUtil::carousel(), SIGNAL(scrollingEnded()), this, 0 );

        RadioUtil::frequencyStrip()->setScanningMode( false );
        static_cast<RadioMainView*>( parent() )->setScanningMode( false );
        RadioUtil::carousel()->setScanningMode( false );
    }

    deleteLater();
}

/*!
 *
 */
void RadioFrequencyScanner::finishScanning()
{
    mScannerEngine->cancel();
    RadioUtil::setScanStatus( Scan::NotScanning );
    RadioUtil::setFrequencyScanner( NULL );
    RadioFrequencyStrip* frequencyStrip = RadioUtil::frequencyStrip();
    RadioStationCarousel* carousel = RadioUtil::carousel();

    disconnect( mScannerEngine.data(), SIGNAL(stationFound(RadioStation)), this, 0 );

    RadioStationModel& model = mUiEngine.stationModel();
    const int stationCount = model.localCount();

    if ( mInMainView ) {

        disconnect( carousel, SIGNAL(scanAnimationFinished()), this, 0 );

        // Scroll the carousel and frequency strip through all of the scanned stations
        if ( stationCount > 1 ) {
            frequencyStrip->setAutoScrollTime( STRIP_FINISH_SCROLL_TIME );
            carousel->setAutoScrollTime( CAROUSEL_FINISH_SCROLL_TIME );

            Radio::connect( carousel,   SIGNAL(scrollingEnded()),
                            this,       SLOT(restoreUiControls()) );

            const uint frequency = model.stationAt( 0 ).frequency();

            mUiEngine.setFrequency( frequency, TuneReason::StationScanFinalize );
            frequencyStrip->setFrequency( frequency, TuneReason::StationScanFinalize, Scroll::Right );
            carousel->setFrequency( frequency, TuneReason::StationScanFinalize, Scroll::Right );

            frequencyStrip->setAutoScrollTime( mStripScrollTime );
            carousel->setAutoScrollTime( mCarouselScrollTime );
        } else {
            const uint frequency = mUiEngine.minFrequency();
            frequencyStrip->setFrequency( frequency, TuneReason::Unspecified );
            carousel->setFrequency( frequency, TuneReason::Unspecified );

            if ( !mUserCanceled ) {
                HbMessageBox* box = new HbMessageBox( HbMessageBox::MessageTypeInformation );
                box->setTimeout( HbPopup::NoTimeout );
                box->setText( hbTrId( "txt_rad_dpophead_no_stations_found_try_searching" ) );
                box->setDismissPolicy( HbPopup::NoDismiss );
                box->setAttribute( Qt::WA_DeleteOnClose, true );
                box->open();
            }

            restoreUiControls();
        }

    } else {
        if ( !mUserCanceled ) {
            if ( mScanningProgressNote ) {
                mScanningProgressNote.data()->setProgressValue( mScanningProgressNote.data()->maximum() );
                mScanningProgressNote.data()->close();
            }

            if ( stationCount == 0 ) {
                HbMessageBox* box = new HbMessageBox( HbMessageBox::MessageTypeInformation );
                box->setTimeout( HbPopup::NoTimeout );
                box->setText( hbTrId( "txt_rad_dpophead_no_stations_found_try_searching" ) );
                box->setDismissPolicy( HbPopup::NoDismiss );
                box->setAttribute( Qt::WA_DeleteOnClose, true );
                box->open();
            }
        }
        const uint frequency = model.stationAt( 0 ).frequency();
        mUiEngine.setFrequency( frequency, TuneReason::StationScanFinalize );
    }

    mIsAlive = false;
    emit frequencyScannerFinished();

    if ( !mInMainView ) {
        if ( mUserCanceled ) {
            deleteLater();
        } else {
            delete this; //TODO: Remove this weird hack once the problem with deleteLater is clear
        }
    }
}
