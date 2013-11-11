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
#include <HbStyleLoader>
#include <HbTextItem>
#include <HbRichTextItem>
#include <HbIconItem>
#include <HbTouchArea>
#include <HbTapGesture>
#include <QPainter>

// User includes
#include "radiocarouselitem.h"
#include "radiocarouselitemobserver.h"
#include "radiostation.h"
#include "radiologger.h"

const QLatin1String FILE_PATH_WIDGETML   ( ":/layout/radiocarouselitem.widgetml" );
const QLatin1String FILE_PATH_CSS        ( ":/layout/radiocarouselitem.css" );
const QLatin1String ICON_BUTTON          ( "star_button" );
const QLatin1String GENRE_LABEL          ( "genre_label" );
const QLatin1String FAVORITE_TOUCH_AREA  ( "favorite_touch_area" );
const QLatin1String NAME_LABEL           ( "name_label" );
const QLatin1String RT_LABEL             ( "rt_label" );
const QLatin1String URL_LABEL            ( "url_label" );

const QLatin1String SEEKING_TEXT         ( "txt_rad_list_tuning" );
const QLatin1String CONNECT_HEADSET_TEXT ( "txt_rad_list_connect_wireless_antenna_headset_with" );
const int PS_NAME_MAX_VISIBILE_LENGTH    = 11;

/*!
 *
 */
static void registerAndCheck( const QString& file ) {
    bool registered = HbStyleLoader::registerFilePath( file );
    Q_ASSERT_X( registered, "RadioCarouselItem", "Failed to register CSS or WIDGETML!" );
}

/*!
 *
 */
RadioCarouselItem::RadioCarouselItem( RadioCarouselItemObserver& observer, QGraphicsItem* parent, bool registerCss ) :
    HbWidget( parent ),
    mObserver( observer ),
    mFavoriteItem( NULL ),
    mGenreItem( NULL ),
    mFavoriteTouchArea( NULL ),
    mNameItem( NULL ),
    mRadiotextItem( NULL ),
    mUrlItem( NULL ),
    mAppearance( Default ),
    mOwnsCss( registerCss ),
    mLandscapeMode( false ),
    mFlags( DefaultFlags )
{
    mStation.reset( new RadioStation() );

    if ( mOwnsCss ) {
        registerAndCheck( FILE_PATH_CSS );
        registerAndCheck( FILE_PATH_WIDGETML );
    }

    createPrimitives();
    updatePrimitives();

    updateFavoriteIcon( false );

    updateVisibilities();

    grabGesture( Qt::TapGesture );
}

/*!
 *
 */
RadioCarouselItem::~RadioCarouselItem()
{
    if ( mOwnsCss ) {
        HbStyleLoader::unregisterFilePath( FILE_PATH_CSS );
        HbStyleLoader::unregisterFilePath( FILE_PATH_WIDGETML );
    }
}

/*!
 *
 */
void RadioCarouselItem::createPrimitives()
{
    mFavoriteItem = new HbIconItem( this );
    HbStyle::setItemName( mFavoriteItem, ICON_BUTTON );

    mGenreItem = new HbTextItem( this );
    HbStyle::setItemName( mGenreItem, GENRE_LABEL );

    mNameItem = new HbTextItem( this );
    HbStyle::setItemName( mNameItem, NAME_LABEL );

    mRadiotextItem = new HbRichTextItem( this );
    HbStyle::setItemName( mRadiotextItem, RT_LABEL );
	
    // TODO: Proper text wrapping mode can be set in style sheet once 
    // TextWrapAtWordBoundaryOrAnywhere counterpart gets available
    mRadiotextItem->setTextWrapping( Hb::TextWrapAtWordBoundaryOrAnywhere );
    
    mUrlItem = new HbTextItem( this );
    HbStyle::setItemName( mUrlItem, URL_LABEL );

    mFavoriteTouchArea = new HbTouchArea( this );
    HbStyle::setItemName( mFavoriteTouchArea, FAVORITE_TOUCH_AREA );

    // Matti testing needs these
    mFavoriteItem->setObjectName( ICON_BUTTON );
    mGenreItem->setObjectName( GENRE_LABEL );
    mNameItem->setObjectName( NAME_LABEL );
    mRadiotextItem->setObjectName( RT_LABEL );
    mUrlItem->setObjectName( URL_LABEL );
    mFavoriteTouchArea->setObjectName( FAVORITE_TOUCH_AREA );
}

/*!
 *
 */
void RadioCarouselItem::drawOffScreen( QPainter& painter )
{
    QStyleOptionGraphicsItem option;

    foreach ( QGraphicsItem* child, childItems() ) {
        QGraphicsWidget* childWidget = static_cast<QGraphicsWidget*>( child );
        option.exposedRect = childWidget->rect();
        painter.save();
        painter.translate( childWidget->pos() );
        childWidget->paint( &painter, &option, NULL );
        painter.restore();
    }
}

/*!
 *
 */
void RadioCarouselItem::updatePrimitives()
{
    update();
}

/*!
 * \reimp
 */
void RadioCarouselItem::gestureEvent( QGestureEvent* event )
{
    if ( HbTapGesture* gesture = qobject_cast<HbTapGesture*>( event->gesture( Qt::TapGesture ) ) ) {
        if ( gesture->state() == Qt::GestureFinished ) {
            const QPointF mappedHotSpot = event->mapToGraphicsScene( gesture->hotSpot() );

            if ( mFlags.testFlag( FavoriteTouchable ) &&
                    mFavoriteTouchArea->sceneBoundingRect().contains( mappedHotSpot ) ) {

                mObserver.handleIconClicked( *mStation );

            } else if ( mFlags.testFlag( RadiotextTouchable ) &&
                    mRadiotextItem->sceneBoundingRect().contains( mappedHotSpot ) ) {

                mObserver.handleRadiotextClicked( *mStation );

            } else if ( mFlags.testFlag( UrlTouchable ) &&
                    mUrlItem->sceneBoundingRect().contains( mappedHotSpot ) ) {

                mObserver.handleUrlClicked( *mStation );

            }
        }
    }
}

/*!
 *
 */
void RadioCarouselItem::setFlags( CarouselItemFlags flags )
{
    mFlags |= flags;
    updateVisibilities();
}

/*!
 *
 */
void RadioCarouselItem::clearFlags( CarouselItemFlags flags )
{
    for ( int i = 1; i < LastFlagMarker; i = i << 1 ) {
        if ( flags.testFlag( static_cast<ItemFlag>( i ) ) ) {
            mFlags &= ~i;
        }
    }
    updateVisibilities();
}

/*!
 *
 */
void RadioCarouselItem::updateVisibilities()
{
    mFavoriteItem->setVisible( mFlags.testFlag( FavoriteVisible ) );
    mGenreItem->setVisible( mFlags.testFlag( GenreVisible ) );
    mRadiotextItem->setVisible( mFlags.testFlag( RadiotextVisible ) );
    mUrlItem->setVisible( mFlags.testFlag( UrlVisible ) );
}

/*!
 *
 */
void RadioCarouselItem::setAppearance( Appearance appearance )
{
    mAppearance = appearance;

    if ( mAppearance == ManualSeek ) {
        mFlags = ManualSeekFlags;
        mGenreItem->setText( "" );
        mRadiotextItem->setText( "" );
        mUrlItem->setText( "" );
        mNameItem->setText( mStation->frequencyString() );
        updateFavoriteIcon( false );
    } else {
        mFlags = DefaultFlags;
    }

    updateVisibilities();

    repolish();
}

/*!
 *
 */
RadioCarouselItem::Appearance RadioCarouselItem::appearance() const
{
    return mAppearance;
}

/*!
 *
 */
void RadioCarouselItem::setLandscape( bool landscape )
{
    mLandscapeMode = landscape;
}

/*!
 *
 */
bool RadioCarouselItem::landscape() const
{
    return mLandscapeMode;
}

/*!
 *
 */
void RadioCarouselItem::setSeekLayout( bool seekLayout )
{
    if ( seekLayout ) {
        setAppearance( ManualSeek );
    } else {
        setAppearance( mStation->radioText().isEmpty() ? Default : Full );
    }
}

/*!
 *
 */
void RadioCarouselItem::setStation( const RadioStation& station )
{
    *mStation = station;

    updateLayout();

    update();
}

/*!
 *
 */
uint RadioCarouselItem::frequency() const
{
    return mStation->frequency();
}

/*!
 *
 */
void RadioCarouselItem::update( const RadioStation* station )
{
    if ( station ) {
        *mStation = *station;
        updateLayout();
    }

    if ( mStation->isValid() ) {
        mGenreItem->setText( mObserver.localizeGenre( mStation->genre() ) );

        const bool hasName = mStation->hasName();
        if ( hasName ) {
            if ( mStation->name().length() > PS_NAME_MAX_VISIBILE_LENGTH ) {
                mNameItem->setAlignment( Qt::AlignLeft );
            } else {
                mNameItem->setAlignment( Qt::AlignHCenter );
            }
            mNameItem->setText( mStation->name() );

        } else {
            mNameItem->setAlignment( Qt::AlignHCenter );
            mNameItem->setText( mStation->frequencyString() );
        }

        if ( mStation->hasRadiotext() ) {
            mRadiotextItem->setText( mStation->radioText() );
        } else {
            if ( mStation->hasDynamicPs() ) {
                mRadiotextItem->setText( mStation->dynamicPsText() );
            } else if ( hasName ) {
                const QString loc = hbTrId( "txt_rad_list_l1_mhz_small" );
                mRadiotextItem->setText( loc.arg( mStation->frequencyString() ) );
            } else {
                mRadiotextItem->setText( "" );
            }
        }

        mUrlItem->setText( mStation->url() );
        if ( mStation->hasUrl() ) {
            HbStyle::setItemName( mUrlItem, URL_LABEL );
            setFlags( UrlVisible | UrlTouchable );
        } else {
            HbStyle::setItemName( mUrlItem, "" ); // Clear the name so the item disappears from layout
            clearFlags( UrlVisible | UrlTouchable );
        }

        updateFavoriteIcon( mStation->isFavorite() );
    } else {
        cleanRdsData();
    }
}

/*!
 *
 */
void RadioCarouselItem::setFrequency( uint frequency )
{
    LOG_FORMAT( "RadioCarouselItem::setFrequency: %d", frequency );

    if ( mStation->hasName() && mStation->frequency() == frequency ) {
        mNameItem->setText( mStation->name() );
    } else {
        mNameItem->setText( RadioStation::parseFrequency( frequency ) );
    }


    if ( !mObserver.isInManualSeek() ) {
        *mStation = mObserver.findStation( frequency );
    }
}

/*!
 *
 */
void RadioCarouselItem::cleanRdsData()
{
    mGenreItem->setText( "" );
    mRadiotextItem->setText( "" );
    mUrlItem->setText( "" );
}

/*!
 *
 */
void RadioCarouselItem::setRadioText( const QString& text )
{
    mRadiotextItem->setText( text );
}

/*!
 *
 */
void RadioCarouselItem::setItemVisibility( ItemVisibility visibility )
{
    CarouselItemFlags flags = 0;
    if ( visibility == AllVisible ) {
        flags = DefaultFlags;
    } else if ( visibility == AllHidden ) {

    } else if ( visibility == IconVisible ) {
        flags = FavoriteVisible;
    }

    setFlags( flags );
}

/*!
 *
 */
void RadioCarouselItem::setIconOpacity( qreal opacity )
{
    mFavoriteItem->setOpacity( opacity );
}

/*!
 *
 */
void RadioCarouselItem::updateFavoriteIcon( bool isFavorite )
{
    if ( isFavorite ) {
        mFavoriteItem->setIcon( mObserver.favoriteIcon() );
    } else {
        mFavoriteItem->setIcon( mObserver.nonFavoriteIcon() );
    }
}

/*!
 *
 */
void RadioCarouselItem::updateLayout()
{
    setAppearance( mStation->hasSentRds() ? Full : Default );
}
