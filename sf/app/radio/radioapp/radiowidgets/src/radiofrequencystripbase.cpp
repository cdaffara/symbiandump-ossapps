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
#include <HbDocumentLoader>
#include <HbEffect>
#include <QPainter>
#include <QStringListModel>
#include <QGraphicsSceneResizeEvent>
#include <QPen>
#include <HbPushButton>
#include <QTimer>
#include <HbColorScheme>
#include <HbEvent>
#include <HbSwipeGesture>
#include <HbDeviceProfile>
#include <QDebug>

// User includes
#include "radiofrequencystripbase.h"
#include "radiofrequencystripdef.h"
#include "radiofrequencyitem.h"

/*!
 * Convenience function to check signal-slot connections
 */
//#define CHECK_SIGNAL_CONNECTIONS
static bool connectAndCheck( const QObject* sender, const char* signal,
                             const QObject* receiver, const char* member,
                             Qt::ConnectionType type = Qt::AutoConnection )
{
    bool connected = QObject::connect( sender, signal, receiver, member, type );

    #ifdef CHECK_SIGNAL_CONNECTIONS

    if ( !connected ) {
        qDebug() << "Failed to make a signal-slot connection!";
        qDebug() << "sender: " << sender->metaObject()->className();
        qDebug() << "signal: " << signal;
        qDebug() << "receiver: " << receiver->metaObject()->className();
        qDebug() << "slot/signal: " << signal;

        Q_ASSERT( false );

        // ----------------------------------------------------------------
        // SIGNAL-SLOT CONNECT FAILED!
        // ----------------------------------------------------------------
    }

    #endif // CHECK_SIGNAL_CONNECTIONS

    return connected;
}

/*!
 *
 */
RadioFrequencyStripBase::RadioFrequencyStripBase( QGraphicsItem* parent ) :
    RadioStripBase( parent ),
    mItemHeight( 8 ),
    mMinFrequency( 87500000 ),
    mMaxFrequency( 108000000 ),
    mFrequencyStepSize( 100000 ),
    mFrequency( 87500000 ),
    mSelectorImage( new QGraphicsPixmapItem( this ) ),
    mSeparatorPos( 0.0 ),
    mMaxWidth( 0 ),
    mSelectorPos( 0.0 ),
    mLeftButton( 0 ),
    mRightButton( 0 ),
    mManualSeekTimer( NULL ),
    mManualSeekMode( false ),
    mLastReportedFrequency( 0 ),
    mManualSeekTimerId( 0 ),
    mForegroundColor( Qt::white ),//HbColorScheme::color( FrequencyStrip::TEXT_COLOR_ATTRIBUTE ) )
    mIgnoreScrollingEnd( false )
{
}

/*!
 * Property
 */
void RadioFrequencyStripBase::setItemHeight( int itemHeight )
{
    mItemHeight = itemHeight;
}

/*!
 * Property
 */
int RadioFrequencyStripBase::itemHeight() const
{
    return mItemHeight;
}

/*!
 *
 */
void RadioFrequencyStripBase::initbase( uint minFrequency, uint maxFrequency, uint stepSize, uint frequency )
{
    mMinFrequency       = minFrequency;
    mMaxFrequency       = maxFrequency;
    mFrequencyStepSize  = stepSize;
    mFrequency          = frequency;

    setScrollingStyle( HbScrollArea::PanWithFollowOn );

    HbDeviceProfile deviceProfile;
    mItemHeight = static_cast<int>( mItemHeight * deviceProfile.unitValue() );
    setItemSize( QSizeF( FrequencyStrip::ITEM_WIDTH + FrequencyStrip::PIXMAP_OVERLAP / 2, mItemHeight ) );

    setOverlap( FrequencyStrip::PIXMAP_OVERLAP / 2 );

    initModel();
    initEmptyItems();
    initPositions();

    mManualSeekTimer = new QTimer( this );
    mManualSeekTimer->setInterval( FrequencyStrip::MANUALSEEK_START_TIMEOUT );
    mManualSeekTimer->setSingleShot( true );
    connectAndCheck( mManualSeekTimer,   SIGNAL(timeout()),
                     this,               SLOT(toggleManualSeek()) );

    connectAndCheck( this,           SIGNAL(scrollingEnded()),
                     this,           SLOT(handleScrollingEnd()) );

    grabGesture( Qt::SwipeGesture );

    //TODO: Remove. Stepsize temporarily hardcoded to 100 Khz in europe region
    if ( mFrequencyStepSize < FrequencyStrip::ONE_HUNDRED_KHZ ) {
        mFrequencyStepSize = FrequencyStrip::ONE_HUNDRED_KHZ;
    }

    updateAllItems();

    showButtons();
}

/*!
 *
 */
void RadioFrequencyStripBase::createButtonsFromDocml( HbDocumentLoader& uiLoader, const QString& leftButtonName, const QString& rightButtonName )
{
    Q_ASSERT( !mLeftButton && !mRightButton );
    mLeftButton = qobject_cast<HbPushButton*>( uiLoader.findWidget( leftButtonName ) );
    mRightButton = qobject_cast<HbPushButton*>( uiLoader.findWidget( rightButtonName ) );

    initButtons();
}

/*!
 *
 */
void RadioFrequencyStripBase::createButtons()
{
    Q_ASSERT( !mLeftButton && !mRightButton );
    mLeftButton = new HbPushButton( this );
    mRightButton = new HbPushButton( this );

    mLeftButton->setIcon( HbIcon( "qtg_mono_previous" ) );
    mRightButton->setIcon( HbIcon( "qtg_mono_next" ) );

    initButtons();
}

/*!
 *
 */
void RadioFrequencyStripBase::setFrequency( const uint frequency, int reason, Scroll::Direction direction )
{
    Q_UNUSED( reason );

    if ( mFrequencies.contains( frequency ) ) {
        scrollToFrequency( frequency, direction, mAutoScrollTime );
    }
}

/*!
 *
 */
uint RadioFrequencyStripBase::frequency() const
{
    return mFrequency;
}

/*!
 *
 */
bool RadioFrequencyStripBase::isInManualSeekMode() const
{
    return mManualSeekMode;
}

/*!
 *
 */
void RadioFrequencyStripBase::cancelManualSeek()
{
    mManualSeekTimer->stop();
    if ( mManualSeekMode ) {
        toggleManualSeek();
    }
}

/*!
 *
 */
void RadioFrequencyStripBase::addScannedStation( uint frequency )
{
    FrequencyPos pos = mFrequencies.value( frequency );
    updateItem( pos.mItem, frequency );
}

/*!
 *
 */
void RadioFrequencyStripBase::updateFavorite( uint frequency )
{
    FrequencyPos pos = mFrequencies.value( frequency );
    updateItem( pos.mItem );
}

/*!
 * Public slot
 *
 */
void RadioFrequencyStripBase::setScanningMode( bool isScanning )
{
    mManualSeekTimer->stop();
    if ( isScanning ) {
        initEmptyItems();
        hideButtons();
        scrollToFrequency( mMinFrequency, Scroll::Shortest, mAutoScrollTime );
    } else {
        showButtons();
    }
    setEnabled( !isScanning );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStripBase::initEmptyItems()
{
    //TODO: Remove the weird hack of checking the sender
    if ( !sender() || !isScanning() ) {
        QList<FrequencyStrip::StationMarker> emptyList;
        foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
            QPixmap pixmap = drawPixmap( item->frequency(), emptyList, item );
            item->setPixmap( pixmap );
        }
    }
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStripBase::removeStation( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    updateStationsInRange( first, last, true );
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStripBase::updateStation( const QModelIndex& parent, int first, int last )
{
    Q_UNUSED( parent );
    updateStationsInRange( first, last );
}

/*!
 * Private slot
 */
void RadioFrequencyStripBase::handleLeftButton()
{
    skipToPrevious();
}

/*!
 * Private slot
 */
void RadioFrequencyStripBase::handleRightButton()
{
    skipToNext();
}

/*!
 * Private slot
 */
void RadioFrequencyStripBase::handleLongLeftButton()
{
    seekDown();
}

/*!
 * Private slot
 */
void RadioFrequencyStripBase::handleLongRightButton()
{
    seekUp();
}

/*!
 * Private slot
 *
 */
void RadioFrequencyStripBase::toggleManualSeek()
{
    mManualSeekMode = !mManualSeekMode;
    emit manualSeekChanged( mManualSeekMode );

    if ( mManualSeekMode ) {
        grabMouse();
        hideButtons();
        mManualSeekTimerId = startTimer( FrequencyStrip::MANUALSEEK_SIGNAL_DELAY );
    } else {
        ungrabMouse();
        showButtons();
        killTimer( mManualSeekTimerId );
        mManualSeekTimerId = 0;
    }
}

/*!
 * Private slot
 */
void RadioFrequencyStripBase::handleScrollingEnd()
{
    if ( mIgnoreScrollingEnd ) {
        mIgnoreScrollingEnd = false;
        return;
    }

    // Check if the selector is in the invalid area where the strip loops around
    const int selectorPosition = selectorPos();
    if ( mManualSeekMode ) {
        if ( !mPositions.contains( selectorPosition ) ) {
            if ( selectorPosition < mMaxWidth - FrequencyStrip::ITEM_WIDTH + mSeparatorPos ) {
                scrollToFrequency( mMaxFrequency, Scroll::Shortest, 500 );
                emitFrequencyChanged( mMaxFrequency, FrequencyStrip::ManualSeekUpdate, Scroll::Shortest );
            } else {
                scrollToFrequency( mMinFrequency, Scroll::Shortest, 500 );
                emitFrequencyChanged( mMinFrequency, FrequencyStrip::ManualSeekUpdate, Scroll::Shortest );
            }
        }

        mManualSeekTimer->start( FrequencyStrip::MANUALSEEK_END_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex )
{
    QGraphicsPixmapItem* item = static_cast<QGraphicsPixmapItem*>( itemToUpdate );
    if ( itemIndex < mFrequencyItems.count() ) {
        item->setPixmap( mFrequencyItems.at( itemIndex )->updatePrimitive( item ) );
    }
}

/*!
 * \reimp
 */
QGraphicsItem* RadioFrequencyStripBase::createItemPrimitive( QGraphicsItem* parent )
{
    return new QGraphicsPixmapItem( parent );
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::scrollPosChanged()
{
    if ( mManualSeekMode ) {
        const int pos = selectorPos();
        const uint frequency = mPositions.value( pos );
        if ( frequency > 0 && mFrequency != frequency ) {
            mFrequency = frequency;
            emitFrequencyChanged( mFrequency, FrequencyStrip::ManualSeekUpdate, Scroll::Shortest );
        }
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::resizeEvent ( QGraphicsSceneResizeEvent* event )
{
    RadioStripBase::resizeEvent( event );

    initSelector();

    const qreal height = event->newSize().height();
    const qreal width = event->newSize().width();

    mSelectorPos = width / 2;
    mSelectorImage->setOffset( mSelectorPos - ( FrequencyStrip::INDICATOR_WIDTH / 2 ), 0.0 );

    if ( mLeftButton && mRightButton ) {
        mLeftButton->resize( height, height );
        mRightButton->resize( height, height );
        mRightButton->setPos( QPointF( width - height, 0 ) );
    }

    scrollToFrequency( mFrequency );
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::changeEvent( QEvent* event )
{
    if ( event->type() == HbEvent::ThemeChanged ) {
        // Update the foreground color and redraw each item
        mForegroundColor = Qt::white;// HbColorScheme::color( FrequencyStrip::TEXT_COLOR_ATTRIBUTE );
        updateAllItems();
    }

    return RadioStripBase::changeEvent(event);
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mousePressEvent( event );

    mManualSeekTimer->stop();
    if ( mManualSeekMode ) {
        const bool insideStrip = rect().contains( event->pos() );
        if ( !insideStrip ) {
            toggleManualSeek();
        }
    } else {
        mManualSeekTimer->start( FrequencyStrip::MANUALSEEK_START_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    RadioStripBase::mouseReleaseEvent( event );

    mManualSeekTimer->stop();
    if ( mManualSeekMode ) {
        mManualSeekTimer->start( FrequencyStrip::MANUALSEEK_END_TIMEOUT );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::gestureEvent( QGestureEvent* event )
{
    mIgnoreScrollingEnd = true;
    HbSwipeGesture* swipeGesture = static_cast<HbSwipeGesture*>( event->gesture( Qt::SwipeGesture ) );
    if ( swipeGesture && !mManualSeekMode ) {
        if ( swipeGesture->state() == Qt::GestureFinished ) {
            if ( swipeGesture->horizontalDirection() == QSwipeGesture::Left ) {
                skipToNext();
            } else if ( swipeGesture->horizontalDirection() == QSwipeGesture::Right ) {
                skipToPrevious();
            }
        }
    } else if ( mManualSeekMode ) {
        RadioStripBase::gestureEvent( event );
    }
}

/*!
 * \reimp
 */
void RadioFrequencyStripBase::timerEvent( QTimerEvent* event )
{
    Q_UNUSED( event );
    if ( mLastReportedFrequency != mFrequency ) {
        mLastReportedFrequency = mFrequency;
        emitFrequencyChanged( mFrequency, FrequencyStrip::ManualSeekTune, Scroll::Shortest );
    }
}

/*!
 *
 */
void RadioFrequencyStripBase::initModel()
{
    const uint minFreq = uint( qreal(mMinFrequency) / FrequencyStrip::ONE_HERTZ + FrequencyStrip::ROUNDER );
    const uint maxFreq = uint( qreal(mMaxFrequency) / FrequencyStrip::ONE_HERTZ + 0.9 ); // always round up

    QStringList list;
    for ( uint i = minFreq; i <= maxFreq; ++i ) {
        list.append( QString::number( i ) );
        mFrequencyItems.append( new RadioFrequencyItem( i ) );
    }
    mFrequencyItems.append( new RadioFrequencyItem( 0 ) );
    list.append( "" );

    mMaxWidth = list.count() * FrequencyStrip::ITEM_WIDTH;

    mSeparatorPos = qreal(FrequencyStrip::ITEM_WIDTH) / 2;
    const uint minDrawableFreq = minFreq * FrequencyStrip::ONE_HERTZ - FrequencyStrip::HALF_HERTZ;;
    const uint maxDrawableFreq = maxFreq * FrequencyStrip::ONE_HERTZ + FrequencyStrip::HALF_HERTZ;
    mSeparatorPos += qreal( ( mMinFrequency  - minDrawableFreq ) / 2 ) / FrequencyStrip::PIXEL_IN_HZ;
    mSeparatorPos -= qreal( ( maxDrawableFreq - mMaxFrequency ) / 2 ) / FrequencyStrip::PIXEL_IN_HZ;

    setModel( new QStringListModel( list, this ) );
}

/*!
 *
 */
void RadioFrequencyStripBase::initSelector()
{
    QPixmap selectorPixmap = QPixmap( QSize( FrequencyStrip::SELECTOR_WIDTH, (int)size().height() - FrequencyStrip::SELECTOR_MARGIN * 2 ) );
    selectorPixmap.fill( Qt::red );
    mSelectorImage->setPixmap( selectorPixmap );
    mSelectorImage->setZValue( FrequencyStrip::SELECTOR_Z_POS );
    mSelectorImage->setPos( 0, FrequencyStrip::SELECTOR_MARGIN );
}

/*!
 *
 */
void RadioFrequencyStripBase::initPositions()
{
    int prevPos = 0;
    int nextPos = 0;
    const int lastPos = mFrequencies.value( mMaxFrequency ).mPosition;
    for ( int i = mFrequencies.value( mMinFrequency ).mPosition; i < lastPos; ++i ) {
        if ( mPositions.contains( i ) ) {
            prevPos = i;
            const uint freq = mPositions.value( prevPos ) + mFrequencyStepSize;
            if ( mFrequencies.contains( freq ) ) {
                nextPos = mFrequencies.value( freq ).mPosition;
            } else {
                nextPos = prevPos;
            }
        } else {
            const int nearestHit = ( i - prevPos ) < ( nextPos - i ) ? prevPos : nextPos;
            mPositions.insert( i, mPositions.value( nearestHit ) );
        }
    }
}

/*!
 *
 */
void RadioFrequencyStripBase::initButtons()
{
    Q_ASSERT( mLeftButton && mRightButton );
    connectAndCheck( mLeftButton,    SIGNAL(clicked()),
                     this,           SLOT(handleLeftButton()) );
    connectAndCheck( mRightButton,   SIGNAL(clicked()),
                     this,           SLOT(handleRightButton()) );
    connectAndCheck( mLeftButton,    SIGNAL(longPress(QPointF)),
                     this,           SLOT(handleLongLeftButton()) );
    connectAndCheck( mRightButton,   SIGNAL(longPress(QPointF)),
                     this,           SLOT(handleLongRightButton()) );

    mLeftButton->setZValue( FrequencyStrip::SELECTOR_Z_POS );
    mLeftButton->setObjectName( FrequencyStrip::LEFT_BUTTON );
    mRightButton->setZValue( FrequencyStrip::SELECTOR_Z_POS );
    mRightButton->setObjectName( FrequencyStrip::RIGHT_BUTTON );

    const qreal height = size().height();
    const qreal width = size().width();
    mLeftButton->resize( height, height );
    mRightButton->resize( height, height );
    mRightButton->setPos( QPointF( width - height, 0 ) );

    // Add left button effects
    QStringList pathList;
    pathList << ":/effects/slide_to_left.fxml" << ":/effects/slide_from_left.fxml";
    QStringList eventList;
    eventList << FrequencyStrip::SLIDE_TO_LEFT << FrequencyStrip::SLIDE_FROM_LEFT;
    HbEffect::add( mLeftButton, pathList, eventList );

    // Add right button effects
    pathList.clear();
    pathList << ":/effects/slide_to_right.fxml" << ":/effects/slide_from_right.fxml";
    eventList.clear();
    eventList << FrequencyStrip::SLIDE_TO_RIGHT << FrequencyStrip::SLIDE_FROM_RIGHT;
    HbEffect::add( mRightButton, pathList, eventList );
}

/*!
 *
 */
void RadioFrequencyStripBase::addFrequencyPos( int pos, uint frequency, RadioFrequencyItem* item )
{
    mFrequencies.insert( frequency, FrequencyPos( pos, item ) );
    mPositions.insert( pos, frequency );
}

/*!
 *
 */
void RadioFrequencyStripBase::updateStationsInRange( int first, int last, bool stationRemoved )
{
    if ( !isScanning() ) {
        uint frequency = 0;
        for ( int i = first; i <= last; ++i ) {
            frequency = frequencyAtIndex( i );
            if ( mFrequencies.contains( frequency ) ) {
                FrequencyPos pos = mFrequencies.value( frequency );
                updateItem( pos.mItem, 0, stationRemoved ? frequency : 0 );
            }
        }
    }
}

/*!
 *
 */
void RadioFrequencyStripBase::updateItem( RadioFrequencyItem* item, uint upperRange, uint ignoredFrequency )
{
    if ( item ) {
        uint frequency = item->frequency();
        QList<FrequencyStrip::StationMarker> markers;

        if ( upperRange == 0 ) {
            upperRange = frequency + FrequencyStrip::HALF_HERTZ;
        }

        if ( isInitialized() ) {
            markers = stationsInRange( frequency - FrequencyStrip::HALF_HERTZ, upperRange );
            for ( int i = markers.count() - 1; ignoredFrequency > 0 && i >= 0; --i ) {
                if ( markers[i].mFrequency == ignoredFrequency ) {
                    markers.removeAt( i );
                    break;
                }
            }
        }

        QPixmap pixmap = drawPixmap( frequency, markers, item );
        item->setPixmap( pixmap );

        foreach ( const FrequencyStrip::StationMarker& marker, markers ) {
            FrequencyPos pos = mFrequencies.value( marker.mFrequency );
            mFrequencies.insert( marker.mFrequency, pos );
        }
    }
}

/*!
 *
 */
void RadioFrequencyStripBase::updateAllItems()
{
    foreach ( RadioFrequencyItem* item, mFrequencyItems ) {
        updateItem( item );
    }
}

/*!
 *
 */
QPixmap RadioFrequencyStripBase::drawPixmap( uint mainFrequency, QList<FrequencyStrip::StationMarker> stations, RadioFrequencyItem* item )
{
    QPixmap pixmap( FrequencyStrip::PIXMAP_WIDTH, mItemHeight );
    pixmap.fill( Qt::transparent );

    if ( mainFrequency == 0 ) {
        return pixmap;
    }

    QPainter painter( &pixmap );
    QPen normalPen = painter.pen();
    QPen favoritePen = normalPen;
    normalPen.setColor( mForegroundColor );
    painter.setPen( normalPen );

    const uint frequencyIncrement = qMin( mFrequencyStepSize, FrequencyStrip::ONE_HUNDRED_KHZ );
    const QString itemText = QString::number( mainFrequency / FrequencyStrip::ONE_HERTZ );
    const uint startFrequency = mainFrequency - FrequencyStrip::HALF_HERTZ + frequencyIncrement;
    const uint endFrequency = mainFrequency + FrequencyStrip::HALF_HERTZ;
    const uint  roundedMin = int( qreal(mMinFrequency) / FrequencyStrip::ONE_HERTZ + FrequencyStrip::ROUNDER );
    const uint freq = mainFrequency / FrequencyStrip::ONE_HERTZ;
    const int diff = freq - roundedMin;
    const qreal startPixel = diff * FrequencyStrip::ITEM_WIDTH;
    qreal pixels = 0.0;
    const qreal leftOverlap = FrequencyStrip::PIXMAP_OVERLAP / 2;

    for ( uint loopFrequency = startFrequency; loopFrequency <= endFrequency; loopFrequency += frequencyIncrement ) {

        pixels = qreal( loopFrequency - startFrequency ) / FrequencyStrip::PIXEL_IN_HZ;
        if ( loopFrequency % FrequencyStrip::ONE_HERTZ == 0 ) {

            // Draw the high frequency tab and the frequency text for the even number
            normalPen.setWidth( 3 );
            painter.setPen( normalPen );
            painter.drawLine( makeTab( pixels + leftOverlap - 1, FrequencyStrip::TAB_HEIGHT_BIG ) );
            normalPen.setWidth( 1 );
            painter.setPen( normalPen );

            // Draw the frequency text and its decimals
            painter.setFont( FrequencyStrip::DECIMAL_FONT );
            const int decimalWidth = painter.fontMetrics().width( FrequencyStrip::DECIMAL_TEXT );
            const int decimalSpace = painter.fontMetrics().leftBearing( '.' );
            painter.setFont( FrequencyStrip::FREQUENCY_FONT );
            const int frequencyWidth = painter.fontMetrics().width( itemText );
            const int textPosX = int( pixels + leftOverlap - ( frequencyWidth + decimalSpace + decimalWidth ) / 2 );
            painter.drawText( QPoint( textPosX, FrequencyStrip::FREQUENCY_TEXT_Y_POS ), itemText );
            painter.setFont( FrequencyStrip::DECIMAL_FONT );
            painter.drawText( QPoint( textPosX + frequencyWidth + decimalSpace, FrequencyStrip::FREQUENCY_TEXT_Y_POS ), FrequencyStrip::DECIMAL_TEXT );

        } else if ( loopFrequency % FrequencyStrip::ONE_TAB_IN_HZ == 0 ) {

            // Draw the low frequency tab for the uneven number
            painter.drawLine( makeTab( pixels + leftOverlap, FrequencyStrip::TAB_HEIGHT_SMALL ) );

        }

        if ( ( loopFrequency - mMinFrequency ) % mFrequencyStepSize == 0  && loopFrequency >= mMinFrequency && loopFrequency <= mMaxFrequency ) {
            addFrequencyPos( int( startPixel + pixels + FrequencyStrip::ROUNDER ), loopFrequency, item );
        }
    }

    // Draw favorites and local stations
    favoritePen.setColor( Qt::yellow );

    const int markerYPos = mItemHeight - 18;
    foreach ( const FrequencyStrip::StationMarker& station, stations ) {
        const uint frequency = station.mFrequency;
        pixels = qreal( frequency - startFrequency ) / FrequencyStrip::PIXEL_IN_HZ;

        if ( station.mIsFavorite ) {
            favoritePen.setWidth( FrequencyStrip::PEN_WIDTH_FAVORITE );
            painter.setPen( favoritePen );
            painter.drawEllipse( int( pixels + leftOverlap - 3 ), markerYPos - 3, 6, 6 );
//            painter.drawEllipse( int( pixels + leftOverlap - 3 ), FrequencyStrip::STATION_MARKER_Y_POS - 3, 6, 6 );
        } else {
            favoritePen.setWidth( 1 );
            painter.setPen( favoritePen );
            painter.drawEllipse( int( pixels + leftOverlap - 4 ), markerYPos - 4, 8, 8 );
//            painter.drawEllipse( int( pixels + leftOverlap - 4 ), FrequencyStrip::STATION_MARKER_Y_POS - 4, 8, 8 );
        }
    }

    return pixmap;
}

/*!
 *
 */
QLineF RadioFrequencyStripBase::makeTab( qreal pos, int height )
{
    return QLineF( pos, mItemHeight - height, pos, mItemHeight );
}

/*!
 *
 */
int RadioFrequencyStripBase::selectorPos() const
{
    const int pos = int( -contentWidget()->x() + mSelectorPos + FrequencyStrip::ROUNDER );
    return pos % mMaxWidth;
}

/*!
 *
 */
void RadioFrequencyStripBase::scrollToFrequency( uint frequency, Scroll::Direction direction, int time )
{
    Q_UNUSED( time );

    int targetFreqPos = mFrequencies.value( frequency ).mPosition;
    const int currentFreqPos = mFrequencies.value( mFrequency ).mPosition;
    const int currentPos = int( -contentWidget()->x() + mSelectorPos + FrequencyStrip::ROUNDER );
    const bool isInSecondHalf = currentPos >= mMaxWidth - FrequencyStrip::ITEM_WIDTH + mSeparatorPos;

    // Special case: When there is only one favorite and the user presses skip
    // the strip must scroll around to the same frequency.
    if ( mFrequency == frequency && direction != Scroll::Shortest ) {
        if ( direction == Scroll::Left ) {
            targetFreqPos += mMaxWidth;
        } else {
            targetFreqPos -= mMaxWidth;
        }
    }

    mFrequency = frequency;

    qreal newPos = targetFreqPos;
    if ( direction == Scroll::Left ) {

        if ( currentFreqPos > targetFreqPos ) {
            newPos += mMaxWidth;
        }

    } else if ( direction == Scroll::Right ) {

        if ( currentFreqPos < targetFreqPos ) {
            newPos -= mMaxWidth;
        }

    }

    if ( isInSecondHalf ) {
        newPos += mMaxWidth;
    }

    newPos -= mSelectorPos - FrequencyStrip::ROUNDER;

//    scrollContentsTo( QPointF( newPos, 0 ), time );
// TODO: Remove this and uncomment the above line. This is a temporary workaround to get the strip to move
    scrollContentsTo( QPointF( newPos, 0 ), 0 );
}

/*!
 *
 */
void RadioFrequencyStripBase::hideButtons()
{
    HbEffect::start( mLeftButton, FrequencyStrip::SLIDE_TO_LEFT );
    HbEffect::start( mRightButton, FrequencyStrip::SLIDE_TO_RIGHT );
}

/*!
 *
 */
void RadioFrequencyStripBase::showButtons()
{
    HbEffect::start( mLeftButton, FrequencyStrip::SLIDE_FROM_LEFT );
    HbEffect::start( mRightButton, FrequencyStrip::SLIDE_FROM_RIGHT );
}
