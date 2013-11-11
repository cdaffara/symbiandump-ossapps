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
#include <QTimeLine>
#include <HbLabel>

// User includes
#include "radiocarouselanimator.h"
#include "radiostationcarousel.h"
#include "radiocarouselitem.h"
#include "radiostation.h"
#include "radiostationmodel.h"
#include "radiologger.h"

// Constants
const int FREQUENCY_SCROLL_DIVIDER = 100000;
const int NUMBER_SCROLL_TIMELINE_DURATION = 1000;
const int FLASH_TIMER_INTERVAL = 400;
const qreal FLASHING_ITEM_DIM_OPACITY = 0.3;
const qreal FLASHING_ITEM_NORMAL_OPACITY = 1.0;

/*!
 *
 */
RadioCarouselAnimator::RadioCarouselAnimator( RadioStationCarousel& carousel ) :
    mCarousel( carousel ),
    mAnimationTimeLine( new QTimeLine( NUMBER_SCROLL_TIMELINE_DURATION, this ) ),
    mTimer( new QTimer( this ) ),
    mCanceled( false ),
    mMode( NoAnimation ),
    mStationIsFavorite( false ),
    mFlashingToggle( false )
{
    mAnimationTimeLine->setCurveShape( QTimeLine::EaseInCurve );
    Radio::connect( mAnimationTimeLine, SIGNAL(finished()),
                    this,               SLOT(finalizeNumberScroll()) );
    Radio::connect( mAnimationTimeLine, SIGNAL(frameChanged(int)),
                    this,               SLOT(updateNumberScroll(int)) );

    mTimer->setInterval( FLASH_TIMER_INTERVAL );
    mTimer->setSingleShot( false );
    Radio::connect( mTimer,     SIGNAL(timeout()),
                    this,       SLOT(handleTimeout()) );
}

/*!
 *
 */
RadioCarouselAnimator::~RadioCarouselAnimator()
{
    stopAll();
}

/*!
 *
 */
void RadioCarouselAnimator::startNumberScroll( const uint startFrequency, const uint stopFrequency )
{
    addMode( ScrollingNumbers );

    //TODO: Take italy case into account
    mAnimationTimeLine->setDuration( NUMBER_SCROLL_TIMELINE_DURATION );
    mAnimationTimeLine->setFrameRange( startFrequency / FREQUENCY_SCROLL_DIVIDER,
                                       stopFrequency / FREQUENCY_SCROLL_DIVIDER );
    mAnimationTimeLine->start();

    mCarousel.scrollContentsTo( QPointF( -mCarousel.mMaxScrollPos, 0 ), mCarousel.mAutoScrollTime );
}

/*!
 *
 */
void RadioCarouselAnimator::startFlashingIcon()
{
    const uint frequency = mCarousel.mItems[RadioStationCarousel::CenterItem]->frequency();
    RadioStation station;
    mCarousel.mModel->findFrequency( frequency, station );
    mStationIsFavorite = station.isFavorite();

    mCarousel.mItems[RadioStationCarousel::CenterItem]->setIconOpacity( FLASHING_ITEM_DIM_OPACITY );

    addMode( FlashingIcon );
}

/*!
 *
 */
void RadioCarouselAnimator::stopFlashingIcon()
{
    mCarousel.mItems[RadioStationCarousel::CenterItem]->setIconOpacity( FLASHING_ITEM_NORMAL_OPACITY );
    removeMode( FlashingIcon );
}

/*!
 *
 */
void RadioCarouselAnimator::startFlashingText()
{
    mCarousel.mInfoText->setOpacity( FLASHING_ITEM_DIM_OPACITY );
    addMode( FlashingText );
}

/*!
 *
 */
void RadioCarouselAnimator::stopFlashingText()
{
    mCarousel.mInfoText->setOpacity( FLASHING_ITEM_NORMAL_OPACITY );
    removeMode( FlashingText );
}

/*!
 *
 */
void RadioCarouselAnimator::stopAll()
{
    mCanceled = true;
    mAnimationTimeLine->stop();
    mTimer->stop();

    stopFlashingText();
    stopFlashingIcon();

    mMode = NoAnimation;
    deleteLater();
}

/*!
 * Private slot
 *
 */
void RadioCarouselAnimator::handleTimeout()
{
    mFlashingToggle = !mFlashingToggle;
    if ( mMode.testFlag( IconItem ) ) {
        if ( mFlashingToggle ) {
            mCarousel.mItems[RadioStationCarousel::CenterItem]->setIconOpacity( FLASHING_ITEM_NORMAL_OPACITY );
        } else {
            mCarousel.mItems[RadioStationCarousel::CenterItem]->setIconOpacity( FLASHING_ITEM_DIM_OPACITY );
        }
    }

    if ( mMode.testFlag( TextItem ) ) {
        if ( mFlashingToggle ) {
            mCarousel.mInfoText->setOpacity( FLASHING_ITEM_NORMAL_OPACITY );
        } else {
            mCarousel.mInfoText->setOpacity( FLASHING_ITEM_DIM_OPACITY );
        }
    }
}

/*!
 * Private slot
 *
 */
void RadioCarouselAnimator::updateNumberScroll( int value )
{
    mCarousel.mItems[RadioStationCarousel::RightItem]->setFrequency( value * FREQUENCY_SCROLL_DIVIDER );
}

/*!
 * Private slot
 *
 */
void RadioCarouselAnimator::finalizeNumberScroll()
{
    if ( !mCanceled ) {
        mCarousel.mItems[RadioStationCarousel::CenterItem]->setFrequency(
                mCarousel.mItems[RadioStationCarousel::RightItem]->frequency() );
        mCarousel.scrollContentsTo( QPointF( -mCarousel.mMidScrollPos, 0 ), 0 );
        emit mCarousel.scanAnimationFinished();
    }
    removeMode( ScrollingNumbers );
}

/*!
 *
 */
void RadioCarouselAnimator::addMode( AnimationMode mode )
{
    mMode |= mode;
    if ( mMode.testFlag( TimerItem ) && !mTimer->isActive() ) {
        mTimer->start();
    }
}

/*!
 *
 */
void RadioCarouselAnimator::removeMode( AnimationMode mode )
{
    mMode &= ~mode;
    if ( !mMode.testFlag( TimerItem ) ) {
        mTimer->stop();
    }

    if ( mMode == NoAnimation ) {
        deleteLater();
    }

}
