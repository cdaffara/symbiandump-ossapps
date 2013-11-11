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

#ifndef RADIOCAROUSELANIMATOR_H
#define RADIOCAROUSELANIMATOR_H

// System includes
#include <QObject>

// User includes

// Forward declarations
class RadioStationCarousel;
class QTimeLine;
class QTimer;

// Class declaration
class RadioCarouselAnimator : public QObject
{
    Q_OBJECT

    enum AnimationFlag
    {
        NoAnimation         = 0,
        ScrollingNumbers    = 1 << 0,
        TimerItem           = 1 << 1,
        IconItem            = 1 << 2,
        TextItem            = 1 << 3,

        FlashingIcon        = TimerItem | IconItem,
        FlashingText        = TimerItem | TextItem
    };
    Q_DECLARE_FLAGS( AnimationMode, AnimationFlag )

public:

    RadioCarouselAnimator( RadioStationCarousel& carousel );
    ~RadioCarouselAnimator();

    void startNumberScroll( const uint startFrequency, const uint stopFrequency );

    void startFlashingIcon();
    void stopFlashingIcon();

    void startFlashingText();
    void stopFlashingText();

    void stopAll();

private slots:

    void handleTimeout();
    void updateNumberScroll( int value );
    void finalizeNumberScroll();

private:

    void addMode( AnimationMode mode );
    void removeMode( AnimationMode mode );

private:

    RadioStationCarousel&   mCarousel;
    QTimeLine*              mAnimationTimeLine;
    QTimer*                 mTimer;
    bool                    mCanceled;
    AnimationMode           mMode;
    bool                    mStationIsFavorite;
    bool                    mFlashingToggle;

};

#endif // RADIOCAROUSELANIMATOR_H
