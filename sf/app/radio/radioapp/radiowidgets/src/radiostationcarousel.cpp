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
#include <QGraphicsLinearLayout>
#include <QGraphicsSceneResizeEvent>
#include <QTimer>
#include <HbPanGesture>
#include <HbSwipeGesture>
#include <HbFontSpec>
#include <HbMenu>
#include <QPainter>

// User includes
#include "radiostationcarousel.h"
#include "radiocarouselanimator.h"
#include "radiouiloader.h"
#include "radiocarouselitem.h"
#include "radiostation.h"
#include "radiouiengine.h"
#include "radiostationmodel.h"
#include "radiofadinglabel.h"
#include "radiologger.h"
#include "radioutil.h"
#include "radio_global.h"

// Constants
const int RTPLUS_CHECK_TIMEOUT = 700;
const int SET_FREQUENCY_TIMEOUT = 500;

// Matti testing constants
const QLatin1String LEFT_ITEM_NAME      ( "carousel_left" );
const QLatin1String CENTER_ITEM_NAME    ( "carousel_center" );
const QLatin1String RIGHT_ITEM_NAME     ( "carousel_right" );

#ifdef BUILD_WIN32
#   define SCROLLBAR_POLICY ScrollBarAlwaysOn
#else
#   define SCROLLBAR_POLICY ScrollBarAlwaysOff
#endif // BUILD_WIN32

#define CALL_TO_ALL_ITEMS( expr ) \
    mItems[LeftItem]->expr; \
    mItems[CenterItem]->expr; \
    mItems[RightItem]->expr;

/*!
 *
 */
RadioStationCarousel::RadioStationCarousel( QGraphicsItem* parent ) :
    HbScrollArea( parent ),
    mUiEngine( NULL ),
    mAutoScrollTime( 300 ),
    mGenericTimer( new QTimer( this ) ),
    mTimerMode( NoTimer ),
    mInfoText( NULL ),
    mRadiotextPopup( NULL ),
    mContainer( new HbWidget( this ) ),
    mMidScrollPos( 0 ),
    mMaxScrollPos( 0 ),
    mCurrentIndex( 0 ),
    mTargetIndex( -1 ),
    mCurrentFrequency( 0 ),
    mIsCustomFreq( false ),
    mInfoTextType( CarouselInfoText::None ),
    mModel( NULL ),
    mPosAdjustmentDisabled( false ),
    mScrollDirection( Scroll::Shortest ),
    mManualSeekMode( false ),
    mOrientation( false ),
    mScrollIgnore( false )
#ifdef USE_DEBUGGING_CONTROLS
    ,mRdsLabel( new RadioFadingLabel( this ) )
#endif // USE_DEBUGGING_CONTROLS
{
}

/*!
 * Property
 *
 */
void RadioStationCarousel::setFavoriteIcon( const HbIcon& favoriteIcon )
{
    mFavoriteIcon = favoriteIcon;
}

/*!
 * Property
 *
 */
HbIcon RadioStationCarousel::favoriteIcon() const
{
    return mFavoriteIcon;
}

/*!
 * Property
 *
 */
void RadioStationCarousel::setNonFavoriteIcon( const HbIcon& nonFavoriteIcon )
{
    mNonFavoriteIcon = nonFavoriteIcon;
}

/*!
 * Property
 *
 */
HbIcon RadioStationCarousel::nonFavoriteIcon() const
{
    return mNonFavoriteIcon;
}

/*!
 *
 */
void RadioStationCarousel::setAutoScrollTime( const int time )
{
    mAutoScrollTime = time;
}

/*!
 *
 */
int RadioStationCarousel::autoScrollTime() const
{
    return mAutoScrollTime;
}

/*!
 *
 */
void RadioStationCarousel::init( RadioUiLoader& uiLoader, RadioUiEngine* uiEngine )
{
    mUiEngine = uiEngine;
    RadioUtil::setCarousel( this );

    mItems[CenterItem] = new RadioCarouselItem( *this, this, true );
    mItems[LeftItem] = new RadioCarouselItem( *this, this );
    mItems[RightItem] = new RadioCarouselItem( *this, this );

    // Matti testing needs the objects to have names
    mItems[LeftItem]->setObjectName( LEFT_ITEM_NAME );
    mItems[CenterItem]->setObjectName( CENTER_ITEM_NAME );
    mItems[RightItem]->setObjectName( RIGHT_ITEM_NAME );

    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout( Qt::Horizontal );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );
    layout->addItem( mItems[LeftItem] );
    layout->addItem( mItems[CenterItem] );
    layout->addItem( mItems[RightItem] );
    mContainer->setLayout( layout );
    setContentWidget( mContainer );

    setClampingStyle( HbScrollArea::NoClamping );
    setScrollDirections( Qt::Horizontal );

    setFrictionEnabled( true );
    setHorizontalScrollBarPolicy( HbScrollArea::SCROLLBAR_POLICY );
    setVerticalScrollBarPolicy( HbScrollArea::ScrollBarAlwaysOff );

    mInfoText = uiLoader.findWidget<HbLabel>( DOCML::MV_NAME_INFO_TEXT );
    mInfoText->setTextWrapping( Hb::TextWordWrap );

    mRadiotextPopup = uiLoader.findObject<HbMenu>( DOCML::MV_NAME_CAROUSEL_RT_MENU );

#ifdef BUILD_WIN32
    HbFontSpec spec = mInfoText->fontSpec();
    spec.setRole( HbFontSpec::Secondary );
    mInfoText->setFontSpec( spec );
#endif

    setScrollDirections( Qt::Horizontal );

    Radio::connect( this,           SIGNAL(scrollingEnded()),
                    this,           SLOT(adjustAfterScroll()) );

    mModel = &mUiEngine->stationModel();
    Radio::connect( mModel,         SIGNAL(favoriteChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)) );
    Radio::connect( mModel,         SIGNAL(stationDataChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));
    Radio::connect( mModel,         SIGNAL(radioTextReceived(RadioStation)),
                    this,           SLOT(updateRadioText(RadioStation)));
    Radio::connect( mModel,         SIGNAL(dynamicPsChanged(RadioStation)),
                    this,           SLOT(update(RadioStation)));

    mGenericTimer->setSingleShot( true );
    Radio::connect( mGenericTimer,  SIGNAL(timeout()),
                    this,           SLOT(timerFired()));

    Radio::connect( mModel,         SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this,           SLOT(updateStations()) );
    Radio::connect( mModel,         SIGNAL(modelReset()),
                    this,           SLOT(updateStations()) );
    Radio::connect( mModel,         SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this,           SLOT(updateStations()) );

    setFrequency( mUiEngine->currentFrequency(), TuneReason::Unspecified );

#ifdef USE_DEBUGGING_CONTROLS
    mRdsLabel->setPos( QPoint( 300, 10 ) );
    mRdsLabel->setText( "RDS" );
    mRdsLabel->setElideMode( Qt::ElideNone );
    HbFontSpec spec = mRdsLabel->fontSpec();
    spec.setTextPaneHeight( 10 );
    spec.setRole( HbFontSpec::Secondary );
    mRdsLabel->setFontSpec( spec );
    mRdsLabel->setTextColor( Qt::gray );
    if ( mUiEngine ) {
        Radio::connect( mUiEngine,      SIGNAL(rdsAvailabilityChanged(bool)),
                        this,           SLOT(setRdsAvailable(bool)) );
    }
#endif // USE_DEBUGGING_CONTROLS
}

/*!
 *
 */
void RadioStationCarousel::setFrequency( uint frequency, int reason, Scroll::Direction direction )
{
    if ( mModel ) {
        if ( !mManualSeekMode ) {

            if ( mModel->rowCount() <= 1 ) {
                mItems[LeftItem]->setStation( RadioStation() );
                mItems[RightItem]->setStation( RadioStation() );
            }

            mIsCustomFreq = false;
            if ( reason == TuneReason::Skip || reason == TuneReason::SkipFromEngine || reason == TuneReason::StationScanFinalize ) {
                const int newIndex = mModel->indexFromFrequency( frequency );
                scrollToIndex( newIndex, direction, NoSignal );
                mCurrentIndex = newIndex;
            } else {
                if ( mModel->contains( frequency ) ) {
                    mCurrentIndex = mModel->indexFromFrequency( frequency );
                } else {
                    const RadioStation prevStation = mModel->findClosest( frequency, StationSkip::Previous );
                    if ( prevStation.isValid() ) {
                        mCurrentIndex = mModel->indexFromFrequency( prevStation.frequency() );
                    } else {
                        mCurrentIndex = -1;
                    }

                    mIsCustomFreq = true;
                }

                mItems[CenterItem]->setFrequency( frequency );
                mTimerMode = SetFrequency;
                mGenericTimer->stop();
                mGenericTimer->start( SET_FREQUENCY_TIMEOUT );
            }
        } else {
            mItems[CenterItem]->setFrequency( frequency );
            mCurrentFrequency = frequency;
        }
    }
}

/*!
 *
 */
RadioUiEngine* RadioStationCarousel::uiEngine()
{
    return mUiEngine;
}

/*!
 *
 */
bool RadioStationCarousel::isAntennaAttached() const
{
    return mUiEngine->isAntennaAttached();
}

/*!
 *
 */
void RadioStationCarousel::setScanningMode( bool scanning )
{
    CALL_TO_ALL_ITEMS( setSeekLayout( scanning ) );

    if ( scanning ) {
        setInfoText( CarouselInfoText::Scanning );
        if ( !mAnimator ) {
            mAnimator = new RadioCarouselAnimator( *this );
        }
        mAnimator.data()->startFlashingText();
    } else {
        if ( mAnimator ) {
            mAnimator.data()->stopFlashingText();
        }
        clearInfoText();
        setCenterIndex( 0 );
    }

    setEnabled( !scanning );
}

/*!
 *
 */
bool RadioStationCarousel::isInScanningMode() const
{
    return RadioUtil::scanStatus() == Scan::ScanningInMainView;
}

/*!
 *
 */
void RadioStationCarousel::cleanRdsData()
{
    mItems[CenterItem]->cleanRdsData();
}

/*!
 *
 */
void RadioStationCarousel::animateNewStation( const RadioStation& station )
{
    if ( mAnimator && mUiEngine ) {
        const uint previousFrequency = mItems[CenterItem]->frequency();

        mItems[RightItem]->setFrequency( previousFrequency );
        mCurrentIndex = mModel->indexFromFrequency( station.frequency() );

        mAnimator.data()->startNumberScroll( previousFrequency, station.frequency() );
    }
}

/*!
 *
 */
void RadioStationCarousel::cancelAnimation()
{
    if ( mAnimator ) {
        mAnimator.data()->stopAll();
    }
}

/*!
 *
 */
void RadioStationCarousel::setInfoText( CarouselInfoText::Type type )
{
    mInfoTextType = type;

    if ( type == CarouselInfoText::ConnectAntenna ) {
        cleanRdsData();
        mInfoText->setPlainText( hbTrId( "txt_rad_info_connect_wired_headset1" ) );
    } else if ( type == CarouselInfoText::Seeking ) {
        cleanRdsData();
        mInfoText->setPlainText( hbTrId( "txt_rad_list_seeking" ) );
    } else if ( type == CarouselInfoText::Scanning ) {
        cleanRdsData();
        mInfoText->setPlainText( hbTrId( "txt_rad_list_searching_all_available_stations_ple" ) );
    }

    mInfoText->setVisible( true );
}

/*!
 *
 */
void RadioStationCarousel::clearInfoText()
{
    if ( mInfoTextType != CarouselInfoText::None ) {
        if ( mAnimator ) {
            mAnimator.data()->stopFlashingIcon();
        }

        mGenericTimer->stop();
        mInfoTextType = CarouselInfoText::None;
        mInfoText->setVisible( false );
        mInfoText->clear();
        mItems[CenterItem]->setItemVisibility( RadioCarouselItem::AllVisible );
    }
}

/*!
 *
 */
void RadioStationCarousel::setManualSeekMode( bool manualSeekActive )
{
    mManualSeekMode = manualSeekActive;
    setEnabled( !manualSeekActive );

    mItems[CenterItem]->setSeekLayout( manualSeekActive );
    if ( !manualSeekActive ) {
        clearInfoText();
        setFrequency( mCurrentFrequency, TuneReason::Unspecified );
    }
}

/*!
 *
 */
void RadioStationCarousel::drawOffScreen( QPainter& painter )
{
    mItems[CenterItem]->drawOffScreen( painter );
}

/*!
 *
 */
void RadioStationCarousel::setLandscape( bool landscape )
{
    CALL_TO_ALL_ITEMS( setLandscape( landscape ) );
    mOrientation = landscape;
}

/*!
 * Private slot
 *
 */
void RadioStationCarousel::scrollPosChanged( const QPointF& newPosition )
{
    Q_UNUSED( newPosition );
//    const int xPos = static_cast<int>( newPosition.x() );
//    mItems[CenterItem]->setPos( xPos - mMidScrollPos, 0 );
}

/*!
 * Private slot
 ''
 */
void RadioStationCarousel::adjustAfterScroll()
{
    if ( isInScanningMode() ) {
        return;
    }

    if ( mScrollIgnore ) {
        mScrollIgnore = false; // next scrollingEnded() is handled
        return;
    }
    if ( mTargetIndex != -1 ) {
        setCenterIndex( mTargetIndex );
    }
}

/*!
 * Private slot
 *
 */
void RadioStationCarousel::update( const RadioStation& station )
{
    if ( !mManualSeekMode && !isInScanningMode() ) {
        for ( int i = LeftItem; i <= RightItem; ++i ) {
            if ( mItems[i]->frequency() == station.frequency() ) {
                mItems[i]->update( &station );
            }
        }
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateRadioText( const RadioStation& station )
{
    if ( isAntennaAttached() && !isInScanningMode() ) {
        if ( station.radioText().isEmpty() ) {
            mItems[CenterItem]->setRadioText( "" );
        } else {
            mRadioTextHolder = station.radioText();
            mTimerMode = RtPlusCheck;
            mGenericTimer->stop();
            mGenericTimer->setInterval( RTPLUS_CHECK_TIMEOUT );
            mGenericTimer->start();
        }
    }
}

/*!
 * Private slot
 */
void RadioStationCarousel::updateStations()
{
    if ( isInScanningMode() ) {
        return;
    }

    setFrequency( mUiEngine->currentFrequency(), TuneReason::Unspecified );
}

/*!
 * Private slot
 */
void RadioStationCarousel::timerFired()
{
    if ( mTimerMode == SetFrequency ) {
        setCenterIndex( mCurrentIndex, NoSignal | IgnoreCenter );
        mTimerMode = NoTimer;
    } else if ( mTimerMode == RtPlusCheck ) {
        //mItems[CenterItem]->mRadiotextLabel->setText( mRadioTextHolder );
        mRadioTextHolder = "";
        mTimerMode = NoTimer;
    } else if ( mTimerMode == InfoText ) {
        clearInfoText();
        mTimerMode = NoTimer;
    }
}

#ifdef USE_DEBUGGING_CONTROLS
/*!
 * Public slot
 */
void RadioStationCarousel::setRdsAvailable( bool available )
{
    QColor color = Qt::green;
    if ( !available && mUiEngine ) {
        LOG_FORMAT( "No RDS signal: Station has sent RDS earlier: %d", mUiEngine.model().currentStation().hasSentRds() );
        color = mUiEngine.model().currentStation().hasSentRds() ? Qt::yellow : Qt::gray;
        mRdsLabel->setText( "RDS" );
    } else {
        mRdsLabel->setText( "-RDS-" );
    }
    mRdsLabel->setTextColor( color );
}
#endif // USE_DEBUGGING_CONTROLS

/*!
 * Public slot
 */
void RadioStationCarousel::updateAntennaStatus( bool connected )
{
    mGenericTimer->stop();

    if ( !connected ) {
        setInfoText( CarouselInfoText::ConnectAntenna );
    } else {
        clearInfoText();
    }
}

/*!
 * \reimp
 */
void RadioStationCarousel::resizeEvent( QGraphicsSceneResizeEvent* event )
{
    HbScrollArea::resizeEvent( event );

    const int width = (int)event->newSize().width();

    mMidScrollPos = -width;
    mMaxScrollPos = mMidScrollPos * 2;

    if ( isInitialized() ) {
        mItems[LeftItem]->setMinimumWidth( width );
        mItems[CenterItem]->setMinimumWidth( width );
        mItems[RightItem]->setMinimumWidth( width );
    }
}

/*!
 * \reimp
 */
void RadioStationCarousel::showEvent( QShowEvent* event )
{
    HbScrollArea::showEvent( event );
//    mContainer->setPos( mMidScrollPos, 0 );
}

/*!
 * \reimp
 */
void RadioStationCarousel::gestureEvent( QGestureEvent* event )
{
//    if ( HbSwipeGesture* swipeGesture = static_cast<HbSwipeGesture*>( event->gesture( Qt::SwipeGesture ) ) ) {
//        if ( swipeGesture->state() == Qt::GestureFinished ) {
//            if ( swipeGesture->horizontalDirection() == QSwipeGesture::Left ) {
//                emit skipRequested( StationSkip::Next );
//            } else if ( swipeGesture->horizontalDirection() == QSwipeGesture::Right ) {
//                emit skipRequested( StationSkip::Previous );
//            }
//            mIsCustomFreq = false;
//        }
//        return;
//    }

    HbScrollArea::gestureEvent( event );

    if ( mOrientation ) {
        if ( HbPanGesture* gesture = qobject_cast<HbPanGesture*>( event->gesture( Qt::PanGesture ) ) ) {
            if ( gesture->state() == Qt::GestureFinished ) {
                mScrollIgnore = true; // next scrollingEnded() signal needs to be ignored
                adjustPos( (int)gesture->offset().y() );
            }
        }
    } else {
        if ( HbPanGesture* gesture = qobject_cast<HbPanGesture*>( event->gesture( Qt::PanGesture ) ) ) {
            if ( gesture->state() == Qt::GestureFinished ) {
                mScrollIgnore = true; // next scrollingEnded() signal needs to be ignored
                adjustPos( (int)gesture->offset().x() );
            }
        }
    }
}

/*!
 * \reimp
 */
void RadioStationCarousel::handleIconClicked( const RadioStation& station )
{
    if ( mModel ) {
        mModel->setData( QModelIndex(), station.frequency(), RadioRole::ToggleFavoriteRole );
    }
}

/*!
 * \reimp
 */
void RadioStationCarousel::handleRadiotextClicked( const RadioStation& station )
{
    Q_UNUSED( station );
    mRadiotextPopup->show();
}

/*!
 * \reimp
 */
void RadioStationCarousel::handleUrlClicked( const RadioStation& station )
{
    mUiEngine->launchBrowser( station.url() );
}

/*!
 * \reimp
 */
QString RadioStationCarousel::localizeGenre( int genre )
{
    return mUiEngine->genreToString( genre, GenreTarget::Carousel );
}

/*!
 * \reimp
 */
bool RadioStationCarousel::isInManualSeek() const
{
    return mManualSeekMode;
}

/*!
 *
 */
RadioStation RadioStationCarousel::findStation( uint frequency )
{
    return mModel->findStation( frequency, FindCriteria::IncludeManualStation );
}

/*!
 *
 */
bool RadioStationCarousel::isInitialized() const
{
    return mUiEngine != NULL;
}

/*!
 *
 */
void RadioStationCarousel::setCenterIndex( int index, ScrollMode mode )
{
    Q_UNUSED( mode );
    if ( mModel ) {
        const int newIndex = trimIndex( index );
        mCurrentIndex = newIndex;
        mTargetIndex = -1;

        if ( !mIsCustomFreq ) {
            mItems[CenterItem]->setStation( mModel->stationAt( mCurrentIndex ) );
        }

        if ( mModel->rowCount() > 1 ) {
            const int leftIndex = prevIndex( mCurrentIndex );
            const int rightIndex = nextIndex( mCurrentIndex );
            mItems[LeftItem]->setStation( mModel->stationAt( leftIndex ) );
            mItems[RightItem]->setStation( mModel->stationAt( rightIndex ) );
        } else {

            if ( mIsCustomFreq ) {
                const uint frequency = mItems[CenterItem]->frequency();
                mItems[LeftItem]->setStation( mModel->findClosest( frequency, StationSkip::Previous ) );
                mItems[RightItem]->setStation( mModel->findClosest( frequency, StationSkip::Next ) );
            } else {
                mItems[LeftItem]->setStation( RadioStation() );
                mItems[RightItem]->setStation( RadioStation() );
            }
        }

        scrollContentsTo( QPointF( -mMidScrollPos /* + delta */, 0 ), 0 );

//        if ( !mode.testFlag( NoSignal ) ) {
//            uint frequency = mModel->stationAt( mCurrentIndex ).frequency();
//            emit frequencyChanged( frequency, TuneReason::StationCarousel, mScrollDirection );
//            mScrollDirection = Scroll::Shortest;
//        }
    }
}

/*!
 *
 */
void RadioStationCarousel::scrollToIndex( int index, Scroll::Direction direction, ScrollMode mode )
{
    if ( mModel && index >= 0  ) {
        mTargetIndex = index;
        const int difference = calculateDifference( index, direction );
        int scrollTime = mAutoScrollTime;

        int posX = direction == Scroll::Left ? -mMaxScrollPos : 0;
        if ( difference == 1 ) {
            if ( direction == Scroll::Right ) {
                posX = 0;
            } else if ( direction == Scroll::Left ) {
                posX = -mMaxScrollPos;
            }
        } else {
            if ( direction == Scroll::Right ) {
                // Item where the scrolling starts
                mItems[RightItem]->setStation( mModel->stationAt( mCurrentIndex ) );

                // Item that is skipped over
                const uint centerFreq = mModel->stationAt( nextIndex( index ) ).frequency();
                mItems[CenterItem]->setFrequency( centerFreq );

                // Item where the scrolling ends
                const RadioStation station = mModel->stationAt( index );
                mItems[LeftItem]->setStation( station );

                mContainer->setPos( mMaxScrollPos, 0 );
                posX = 0;
            } else if ( direction == Scroll::Left ) {
                // Item where the scrolling starts
                mItems[LeftItem]->setStation( mModel->stationAt( mCurrentIndex ) );

                // Item that is skipped over
                const uint centerFreq = mModel->stationAt( prevIndex( index ) ).frequency();
                mItems[CenterItem]->setFrequency( centerFreq );

                // Item where the scrolling ends
                const RadioStation station = mModel->stationAt( index );
                mItems[RightItem]->setStation( station );

                mContainer->setPos( 0, 0 );
                posX = -mMaxScrollPos;
            }
        }

        if ( mode.testFlag( UpdateItem ) ) {
            //item->update();
        }

        if ( mode.testFlag( NoAnim ) ) {
            scrollTime = 0;
        } else if ( mode.testFlag( FromPanGesture ) ) {
            scrollTime = 500;
        } else if ( mode.testFlag( FromSwipeGesture ) ) {
            scrollTime = 100;
        }

        scrollContentsTo( QPointF( posX, 0 ), scrollTime );
    }
}

/*!
 *
 */
int RadioStationCarousel::calculateDifference( int targetIndex, Scroll::Direction& direction )
{
    int difference = 0;
    const int rowCount = mModel->rowCount();

    int diffToLeft = 0;
    int diffToRight = 0;
    if ( targetIndex > mCurrentIndex ) {
        const int loopedDiff = mCurrentIndex + rowCount - targetIndex;
        const int directDiff = targetIndex - mCurrentIndex;
        diffToLeft = loopedDiff;
        diffToRight = directDiff;
    } else {
        const int loopedDiff = targetIndex + rowCount - mCurrentIndex;
        const int directDiff = mIsCustomFreq ? 1 : mCurrentIndex - targetIndex;
        diffToLeft = directDiff;
        diffToRight = loopedDiff;
    }

    if ( direction == Scroll::Right ) {
        difference = diffToLeft;
    } else if ( direction == Scroll::Left ) {
        difference = diffToRight;
    } else {
        if ( diffToLeft < diffToRight ) {
            difference = diffToLeft;
            direction = Scroll::Right;
        } else {
            difference = diffToRight;
            direction = Scroll::Left;
        }
    }

    return difference;
}

/*!
 *
 */
bool RadioStationCarousel::isScrollingAllowed() const
{
    const int rowCount = mModel->rowCount();
    return rowCount > 1 || ( rowCount == 1 && mIsCustomFreq );
}

/*!
 *
 */
void RadioStationCarousel::adjustPos( int offset )
{
    int newPos = mMidScrollPos;
    const int threshold = (int)size().width() / 5;
    int newIndex = mCurrentIndex;
    bool needsToScroll = false;

        if ( isScrollingAllowed() && abs( offset ) >= threshold ) {
        needsToScroll = true;
        if ( offset > 0 ) {
            newPos = 0;
            mScrollDirection = Scroll::Right;
            if ( !mIsCustomFreq ) {
                if ( mModel->favoriteCount() > 1 ){
                    const uint newFreq = mModel->findClosest( mItems[CenterItem]->frequency(), StationSkip::PreviousFavorite ).frequency();
                    if ( newFreq > 0 ) {
                        newIndex = mModel->indexFromFrequency( newFreq );
                    } else {
                        needsToScroll = false;
                        newPos = mMidScrollPos;
                    }
                } else {
                    const uint newFreq = mModel->findClosest( mItems[CenterItem]->frequency(), StationSkip::Previous ).frequency();
                    if ( newFreq > 0 ) {
                        newIndex = mModel->indexFromFrequency( newFreq );
                    } else {
                        needsToScroll = false;
                        newPos = mMidScrollPos;
                    }
                }
            }
        } else {
            mScrollDirection = Scroll::Left;
            newPos = mMaxScrollPos;

            if ( mModel->favoriteCount() > 1 ){
                const uint newFreq = mModel->findClosest( mItems[CenterItem]->frequency(), StationSkip::NextFavorite ).frequency();
                if ( newFreq > 0 ) {
                    newIndex = mModel->indexFromFrequency( newFreq );
                } else {
                    needsToScroll = false;
                    newPos = mMidScrollPos;
                }
            } else {
                const uint newFreq = mModel->findClosest( mItems[CenterItem]->frequency(), StationSkip::Next ).frequency();
                if ( newFreq > 0 ) {
                    newIndex = mModel->indexFromFrequency( newFreq );
                } else {
                    needsToScroll = false;
                    newPos = mMidScrollPos;
                }
            }
        }
    }

    newIndex = trimIndex( newIndex );
    if ( needsToScroll ) {
        const uint frequency = mModel->stationAt( newIndex ).frequency();
        emit frequencyChanged( frequency, TuneReason::StationCarousel, mScrollDirection );
        scrollToIndex( newIndex, mScrollDirection, RadioStationCarousel::FromPanGesture );
        mIsCustomFreq = false;
    } else {
        scrollContentsTo( QPointF( -newPos, 0 ), 300 );
    }
}

/*!
 *
 */
int RadioStationCarousel::trimIndex( int index )
{
    const int count = mModel ? mModel->rowCount() : 0;

    if ( count == 0 ) {
        return -1;
    }

    if ( index < 0 ) {
        index = count - 1;
    }
    index %= count;
    return index;
}

/*!
 *
 */
int RadioStationCarousel::prevIndex( int index )
{
    if ( !mIsCustomFreq ) {
        --index;
    }
    return trimIndex( index );
}

/*!
 *
 */
int RadioStationCarousel::nextIndex( int index )
{
    return trimIndex( index + 1 );
}

/*!
 *
 */
void RadioStationCarousel::skip( StationSkip::Mode mode )
{
    if ( mModel ) {
        const uint frequency = mModel->findClosest( mItems[CenterItem]->frequency(), mode ).frequency();
        const int index = mModel->indexFromFrequency( frequency );
        const Scroll::Direction direction = RadioUtil::scrollDirectionFromSkipMode( mode );
        scrollToIndex( index, direction, RadioStationCarousel::NoSignal );
    }
}
