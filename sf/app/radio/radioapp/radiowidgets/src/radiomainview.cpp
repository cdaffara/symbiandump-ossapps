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
#include <HbPushButton>
#include <HbAction>
#include <HbMenu>
#include <HbApplication>
#include <afactivitystorage.h>
#include <QPixmap>
#include <HbLabel>
#include <HbFrameItem>          // Temporarily here until docml supports frame items

// User includes
#include "radiowindow.h"
#include "radiomainview.h"
#include "radiofrequencystrip.h"
#include "radiouiengine.h"
#include "radiologger.h"
#include "radiostationcarousel.h"
#include "radiouiloader.h"
#include "radioutil.h"
#include "radiostationmodel.h"
#include "radiofrequencyscanner.h"

// Constants
const QLatin1String RADIO_MAINVIEW_ACTIVITY_ID( "FMRadioMainView" );

/*!
 * Temporary convenience function to set frame background until DOCML supports frame items
 */
static void initFrameBackground( RadioUiLoader& uiLoader,
                                 const QString backgroundName,
                                 HbFrameDrawer::FrameType frameType )
{
    if ( HbLabel* backgroundLabel = uiLoader.findObject<HbLabel>( backgroundName ) ) {
        QString backgroundName = backgroundLabel->icon().iconName();
        HbFrameItem* frameItem = new HbFrameItem( backgroundName, frameType, backgroundLabel );
        backgroundLabel->setBackgroundItem( frameItem );
    }
}


/*!
 *
 */
RadioMainView::RadioMainView() :
    RadioViewBase( false ),
    mCarousel( NULL ),
    mFrequencyStrip( NULL )
{
}

/*!
 *
 */
RadioMainView::~RadioMainView()
{
}

/*!
 * \reimp
 *
 */
void RadioMainView::setScanningMode( bool scanning )
{
    if ( scanning ) {
        loadSection( DOCML::FILE_MAINVIEW, DOCML::MV_SECTION_SCANNING );
    } else {
        loadSection( DOCML::FILE_MAINVIEW, DOCML::MV_SECTION_NORMAL );
        updateFavoriteButton();
        mFrequencyScanner.take();

        const bool firsTimeStart = mUiEngine->isFirstTimeStart();
        const int rowCount = mUiEngine->stationModel().rowCount();
        if ( firsTimeStart && rowCount != 0 ) {
            mUiEngine->setFirstTimeStartPerformed( true );
        }
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::preLazyLoadInit()
{
    initFrameBackground( *mUiLoader, DOCML::MV_NAME_FREQUENCY_BACKGROUND, HbFrameDrawer::NinePieces );

    initFrameBackground( *mUiLoader, DOCML::MV_NAME_CAROUSEL_BACKGROUND, HbFrameDrawer::NinePieces );
}

/*!
 * \reimp
 *
 */
void RadioMainView::init()
{
    LOG_METHOD;
    loadSection( DOCML::FILE_MAINVIEW, DOCML::SECTION_LAZY_LOAD );
    initFrameBackground( *mUiLoader, DOCML::MV_NAME_CAROUSEL_OVERLAY, HbFrameDrawer::NinePieces );

    mCarousel = mUiLoader->findObject<RadioStationCarousel>( DOCML::MV_NAME_STATION_CAROUSEL );
    mCarousel->init( *mUiLoader, mUiEngine.data() );
    mCarousel->setLandscape( mMainWindow->orientation() == Qt::Horizontal );

    mFrequencyStrip = mUiLoader->findObject<RadioFrequencyStrip>( DOCML::MV_NAME_FREQUENCY_STRIP );
    mFrequencyStrip->init( mUiEngine.data(), *mUiLoader );

    RadioStationModel* stationModel = &mUiEngine->stationModel();

    Radio::connect( mFrequencyStrip,            SIGNAL(frequencyChanged(uint,int,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int,int)) );
    Radio::connect( mCarousel,                  SIGNAL(frequencyChanged(uint,int,int)),
                    this,                       SLOT(setFrequencyFromWidget(uint,int,int)) );
    Radio::connect( mUiEngine.data(),           SIGNAL(tunedToFrequency(uint,int)),
                    this,                       SLOT(setFrequencyFromEngine(uint,int)) );
    Radio::connect( mFrequencyStrip,            SIGNAL(manualSeekChanged(bool)),
                    this,                       SLOT(setManualSeekMode(bool)) );

    Radio::connect( mFrequencyStrip,            SIGNAL(skipRequested(int)),
                    this,                       SLOT(skip(int)) );
    Radio::connect( mCarousel,                  SIGNAL(skipRequested(int)),
                    this,                       SLOT(skip(int)) );
    Radio::connect( mFrequencyStrip,            SIGNAL(seekRequested(int)),
                    mUiEngine.data(),           SLOT(seekStation(int)) );

    Radio::connect( mUiEngine.data(),           SIGNAL(seekingStarted(int)),
                    this,                       SLOT(seekingStarted()) );
    Radio::connect( mUiEngine.data(),           SIGNAL(antennaStatusChanged(bool)),
                    this,                       SLOT(updateAntennaStatus(bool)) );
    Radio::connect( mUiEngine.data(),           SIGNAL(audioRouteChanged(bool)),
                    this,                       SLOT(updateAudioRoute(bool)) );

    Radio::connect( stationModel,               SIGNAL(favoriteChanged(RadioStation)),
                    this,                       SLOT(handleFavoriteChange(RadioStation)) );

    connectXmlElement( DOCML::MV_NAME_STATIONS_BUTTON,  SIGNAL(clicked()),
                       mMainWindow,                     SLOT(activateStationsView()) );

    connectXmlElement( DOCML::MV_NAME_FAVORITE_BUTTON,  SIGNAL(clicked()),
                       this,                            SLOT(toggleFavorite()) );

    connectXmlElement( DOCML::MV_NAME_SPEAKER_BUTTON,   SIGNAL(clicked()),
                       mUiEngine.data(),                SLOT(toggleAudioRoute()) );

    connectXmlElement( DOCML::MV_NAME_SCAN_ACTION,      SIGNAL(triggered()),
                       this,                            SLOT(toggleScanning()) );

    updateFavoriteButton();

    connectCommonMenuItem( MenuItem::Exit );

    // "Play history" menu item
    connectViewChangeMenuItem( DOCML::MV_NAME_HISTORYVIEW_ACTION, SLOT(activateHistoryView()) );

    updateAudioRoute( mUiEngine->isUsingLoudspeaker() );

    // Add "back" navigation action to put the application to background
    HbAction* backAction = new HbAction( Hb::BackNaviAction, this );
#ifdef BUILD_WIN32
    Radio::connect( backAction,     SIGNAL(triggered()),
                    this,           SLOT(quit()) );
#else
    Radio::connect( backAction,     SIGNAL(triggered()),
                    mMainWindow,    SLOT(lower()) );
#endif // BUILD_WIN32
    setNavigationAction( backAction );

    const bool firsTimeStart = mUiEngine->isFirstTimeStart();
    const int rowCount = mUiEngine->stationModel().rowCount();

   if ( firsTimeStart && rowCount == 0 ){
            QTimer::singleShot( 4500, this, SLOT(startFirstTimeScanning()) );
        }


    Radio::connect( static_cast<HbApplication*>( qApp ),    SIGNAL(aboutToQuit()),
                    this,                                   SLOT(saveActivity()) );

    emit applicationReady();

    QScopedPointer<AfActivityStorage> storage( new AfActivityStorage() );
    bool ok = storage->removeActivity( RADIO_MAINVIEW_ACTIVITY_ID );
    LOG_ASSERT( ok, LOG( "Failed to remove old activity from Activity Storage!" ) );
}

/*!
 * \reimp
 *
 */
void RadioMainView::setOrientation()
{
    loadSection( DOCML::FILE_MAINVIEW, mMainWindow->orientationSection() );
    if ( mCarousel && mFrequencyStrip ) {
        mCarousel->setFrequency( mFrequencyStrip->frequency(), TuneReason::Unspecified );
        mCarousel->setLandscape( mMainWindow->orientation() == Qt::Horizontal );
    }
}

/*!
 * \reimp
 *
 */
void RadioMainView::userAccepted()
{
    mFrequencyScanner.reset( new RadioFrequencyScanner( *mUiEngine, this ) );
    mFrequencyScanner->startScanning();
}

/*!
 * \reimp
 *
 */
bool RadioMainView::eventFilter( QObject* watched, QEvent* event )
{
    if ( event->type() == QEvent::ApplicationDeactivate ) {
        mFrequencyStrip->cancelManualSeek();
    }

    return RadioViewBase::eventFilter( watched, event );
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromWidget( uint frequency, int reason, int direction )
{
//    LOG_FORMAT( "RadioMainView::setFrequencyFromWidget: %u, reason = %d", frequency, reason );
    if ( !RadioUtil::isScannerAlive() ) {
        if ( reason == TuneReason::FrequencyStrip ) {
//            mCarousel->setFrequency( frequency, reason, direction );
//            mUiEngine->tuneWithDelay( frequency, reason );
        } else if ( reason == TuneReason::ManualSeekUpdate ) {
            mCarousel->setFrequency( frequency, reason, RadioUtil::scrollDirection( direction ) );
        } else if ( reason == TuneReason::ManualSeekTune ) {
            LOG_FORMAT( "--------Manual seek tune: %u", frequency );
            mUiEngine->setFrequency( frequency, reason );
        } else if ( reason == TuneReason::StationCarousel ) {
            mFrequencyStrip->setFrequency( frequency, reason, RadioUtil::scrollDirection( direction ) );
            mUiEngine->setFrequency( frequency, reason );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::setFrequencyFromEngine( uint frequency, int reason )
{
    LOG_FORMAT( "RadioMainView::setFrequencyFromEngine reason: %d", reason );
    if ( RadioUtil::scanStatus() != Scan::ScanningInMainView && !mFrequencyStrip->isInManualSeekMode() ) {
        mCarousel->clearInfoText();
        updateFavoriteButton();
        if ( reason != TuneReason::FrequencyStrip &&
             reason != TuneReason::StationCarousel &&
             reason != TuneReason::Skip &&
             reason != TuneReason::ManualSeekTune ) {
            mCarousel->setFrequency( frequency, reason, Scroll::Shortest );
            mFrequencyStrip->setFrequency( frequency, reason, Scroll::Shortest );
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::skip( int skipMode )
{
    LOG_FORMAT( "RadioMainView::skip skipMode: %d", skipMode );

    const uint currentFrequency = mFrequencyStrip->frequency();
    RadioStation station;
    mUiEngine->stationModel().findFrequency( currentFrequency, station );

    const uint frequency = mUiEngine->skipStation( static_cast<StationSkip::Mode>( skipMode ),
                                                            currentFrequency);

    if ( currentFrequency != frequency || station.isFavorite() ) {
        const Scroll::Direction direction = RadioUtil::scrollDirectionFromSkipMode( skipMode );
        mCarousel->setFrequency( frequency, TuneReason::Skip, direction );
        mFrequencyStrip->setFrequency( frequency, TuneReason::Skip, direction );
    }
}

/*!
 * Private slot
 */
void RadioMainView::openStationsView()
{
    mFrequencyStrip->cancelManualSeek();
    mMainWindow->activateStationsView();
}

/*!
 * Private slot
 */
void RadioMainView::startFirstTimeScanning()
{
    if ( mUiEngine->isAntennaAttached() ){
        QTimer::singleShot( 100, this, SLOT(toggleScanning()) );
    }
}

/*!
 * Private slot
 */
void RadioMainView::toggleScanning()
{
    mFrequencyStrip->cancelManualSeek();
    if ( mFrequencyScanner ) {
        mFrequencyScanner->cancelScanning();
    } else {
        const int rowCount =  mUiEngine->stationModel().rowCount();
        if ( rowCount > 0 ) {
            askQuestion( hbTrId( "txt_rad_info_all_stations_in_stations_list_will_be" ) );
        } else {
            userAccepted();
        }
    }
}

/*!
 * Private slot
 */
void RadioMainView::toggleFavorite()
{
    if ( RadioUtil::isScannerAlive() ) {
        toggleScanning();
    } else {
        mUiEngine->stationModel().setData( QModelIndex(), mFrequencyStrip->frequency(), RadioRole::ToggleFavoriteRole );
    }
}

/*!
 * Private slot
 */
void RadioMainView::seekingStarted()
{
    if ( !RadioUtil::isScannerAlive() ) {
        mCarousel->setInfoText( CarouselInfoText::Seeking );
    }
}

/*!
 * Private slot
 */
void RadioMainView::updateAntennaStatus( bool connected )
{
    if ( !connected ) {
        mFrequencyStrip->cancelManualSeek();
    }

    HbAction* scanAction = mUiLoader->findObject<HbAction>( DOCML::MV_NAME_SCAN_ACTION );
    scanAction->setEnabled( connected );
    mCarousel->updateAntennaStatus( connected );
    const bool firsTimeStart = mUiEngine->isFirstTimeStart();
    const int rowCount = mUiEngine->stationModel().rowCount();
    if ( firsTimeStart && rowCount == 0 && connected ){
            QTimer::singleShot( 100, this, SLOT(toggleScanning()) );
        }
}

/*!
 * Private slot
 */
void RadioMainView::updateAudioRoute( bool loudspeaker )
{
    HbPushButton* loudspeakerButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_SPEAKER_BUTTON );
    if ( loudspeaker ) {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker_off" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_deactivate_loudspeaker" ) );
    } else {
        loudspeakerButton->setIcon( HbIcon( "qtg_mono_speaker" ) );
        loudspeakerButton->setText( hbTrId( "txt_rad_button_activate_loudspeaker" ) );
    }
}

/*!
 * Private slot
 */
void RadioMainView::setManualSeekMode( bool manualSeekActive )
{
    if ( manualSeekActive ) {
        qApp->installEventFilter( this );

        mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_FAVORITE_BUTTON )->setText( hbTrId( "txt_rad_button_add_to_favourites" ) );
    } else {
        qApp->removeEventFilter( this );

        updateFavoriteButton();
    }

    mUiEngine->setManualSeekMode( manualSeekActive );

    mCarousel->setManualSeekMode( manualSeekActive );
}

/*!
 * Private slot
 */
void RadioMainView::handleFavoriteChange( const RadioStation& station )
{
    mFrequencyStrip->updateFavorite( station.frequency() );
    updateFavoriteButton();
}

/*!
 * Private slot
 */
void RadioMainView::saveActivity()
{
    // Get a screenshot for saving to the activity manager
    QSize screenShotSize = mCarousel->size().toSize();
    QPixmap screenShot( screenShotSize );
    QPainter painter( &screenShot );

    // Draw the background and overlay
    // TODO: Uncomment when Orbit fixes the crash caused calling the pixmap() function.
//    HbLabel* backgroundLabel = mUiLoader->findWidget<HbLabel>( DOCML::MV_NAME_CAROUSEL_BACKGROUND );
//    painter.drawPixmap( 0, 0, backgroundLabel->icon().pixmap().scaled( screenShotSize ) );
//    backgroundLabel = mUiLoader->findWidget<HbLabel>( DOCML::MV_NAME_CAROUSEL_OVERLAY );
//    painter.drawPixmap( 0, 0, backgroundLabel->icon().pixmap().scaled( screenShotSize ) );

    mCarousel->drawOffScreen( painter );

    QVariantHash metadata;
    metadata.insert( "screenshot", screenShot );

    #ifdef __WINS__
        screenShot.save( "c:\\radio.bmp" );
    #elif defined BUILD_WIN32
        screenShot.save( "radio.bmp" );
    #endif

    // Update the activity to the activity manager
    QScopedPointer<AfActivityStorage> storage( new AfActivityStorage() );
    bool ok = storage->saveActivity( RADIO_MAINVIEW_ACTIVITY_ID, QVariant(), metadata );
    LOG_ASSERT( ok, LOG( "Failed to update activity to Activity Storage!" ) );
}

/*!
 *
 */
void RadioMainView::updateFavoriteButton()
{
    RadioStation station;
    RadioStationModel& model = mUiEngine->stationModel();
    model.findFrequency( mUiEngine->currentFrequency(), station );
    HbPushButton* favoriteButton = mUiLoader->findWidget<HbPushButton>( DOCML::MV_NAME_FAVORITE_BUTTON );
    if ( station.isFavorite() ) {
        favoriteButton->setText( hbTrId( "txt_fmradio_button_remove_from_favourites" ) );
        favoriteButton->setIcon( HbIcon( "qtg_mono_favourites_remove" ) );
    } else {
        favoriteButton->setText( hbTrId( "txt_rad_button_add_to_favourites" ) );
        favoriteButton->setIcon( HbIcon( "qtg_mono_add_to_favourites" ) );
    }
}
