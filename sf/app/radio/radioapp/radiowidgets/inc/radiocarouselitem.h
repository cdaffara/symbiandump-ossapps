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

#ifndef RADIOCAROUSELITEM_H
#define RADIOCAROUSELITEM_H

// System includes
#include <HbWidget>
#include <QScopedPointer>

// User includes

// Forward declarations
class RadioStation;
class HbIconItem;
class HbTextItem;
class HbRichTextItem;
class HbTouchArea;
class RadioCarouselItemObserver;

// Class declaration
class RadioCarouselItem : public HbWidget
{
    Q_OBJECT
    Q_PROPERTY(Appearance appearance READ appearance WRITE setAppearance)
    Q_PROPERTY(bool landscape READ landscape WRITE setLandscape)
    Q_ENUMS(Appearance)

    friend class RadioStationCarousel;

public:

    enum ItemFlag
    {
        NameVisible         = 1 << 0,
        FavoriteVisible     = 1 << 1,
        FavoriteTouchable   = 1 << 2,
        GenreVisible        = 1 << 3,
        RadiotextVisible    = 1 << 4,
        RadiotextTouchable  = 1 << 5,
        UrlVisible          = 1 << 6,
        UrlTouchable        = 1 << 7,

        LastFlagMarker      = 1 << 8,       // Keep this as the last flag

        DefaultFlags = NameVisible | FavoriteVisible | FavoriteTouchable | GenreVisible | RadiotextVisible,
        ManualSeekFlags = NameVisible
    };
    Q_DECLARE_FLAGS( CarouselItemFlags, ItemFlag )

    enum Appearance { Default, Full, ManualSeek };

    RadioCarouselItem( RadioCarouselItemObserver& observer, QGraphicsItem* parent, bool registerCss = false );
    ~RadioCarouselItem();

// New functions

    void setAppearance( Appearance appearance );
    Appearance appearance() const;

    void setLandscape( bool landscape );
    bool landscape() const;

    void setSeekLayout( bool seekLayout );

    void setStation( const RadioStation& station );

    uint frequency() const;
    void update( const RadioStation* station = NULL );
    void setFrequency( uint frequency );
    void cleanRdsData();
    void setRadioText( const QString& text );

    enum ItemVisibility{ AllVisible, AllHidden, IconVisible };
    void setItemVisibility( ItemVisibility visibility );

    void setIconOpacity( qreal opacity );

    void createPrimitives();

    void drawOffScreen( QPainter& painter );

public slots:

    void updatePrimitives();

private:

    void gestureEvent( QGestureEvent* event );

    void setFlags( CarouselItemFlags flags );
    void clearFlags( CarouselItemFlags flags );

    void updateVisibilities();

    void updateFavoriteIcon( bool isFavorite );

    void updateLayout();

private: // data

    RadioCarouselItemObserver&          mObserver;

    QScopedPointer<RadioStation>        mStation;

    HbIconItem*                         mFavoriteItem;
    HbTextItem*                         mGenreItem;
    HbTouchArea*                        mFavoriteTouchArea;
    HbTextItem*                         mNameItem;
    HbRichTextItem*                     mRadiotextItem;
    HbTextItem*                         mUrlItem;

    Appearance                          mAppearance;
    bool                                mOwnsCss;
    bool                                mLandscapeMode;
    CarouselItemFlags                   mFlags;

};

Q_DECLARE_OPERATORS_FOR_FLAGS( RadioCarouselItem::CarouselItemFlags )

#endif // RADIOCAROUSELITEM_H
