/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Swipe Widget.
*
*/

#include <QDebug>
#include <HbStyleLoader>
#include <hbeffect.h>
#include <hblabel.h>
#include <HbIconItem>
#include <HbIconAnimationManager>

#include <hbiconitem.h>
#include <hbiconanimator.h>

#include "snsrbigclockcontainer.h"
#include "snsranalogclockcontainer.h"
#include "snsrswipewidget.h"

const char *gSwipeCssFilePath =":/style/snsrswipewidget.css";
const char *gSwipeWidgetMLFilePath = ":/style/snsrswipewidget.widgetml";
const char *gSwipeColorCssFilePath = ":/style/snsrswipewidget_color.css";
const char *gSwipeIcon = ":/xml/qtg_anim_swipe.axml";
const qreal gSwipeDownAngle = 270;
const qreal gLandscapeSwipeDownAngle = 180;
const qreal gSwipeAngleTolerance = 25;

/*!
    \class SnsrSwipeWidget
    \ingroup group_snsrbigclockscreensaverplugins
    \brief Screensaver swipe widget.
 */

/*!
    Constructs a new SnsrSwipeWidget.
    \param parent Parent object.
 */
SnsrSwipeWidget::SnsrSwipeWidget(QGraphicsItem* parent):
    HbWidget(parent),
    mSlideLabel(0), mIconItem(0),mCurrentOrientation(-1)
{
    HbStyleLoader::registerFilePath(gSwipeCssFilePath);
    HbStyleLoader::registerFilePath(gSwipeWidgetMLFilePath);
    HbStyleLoader::registerFilePath(gSwipeColorCssFilePath);
    
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);

    createPrimitives();

    grabGesture(Qt::SwipeGesture);
}

/*!
    Destructs the class.
 */
SnsrSwipeWidget::~SnsrSwipeWidget()
{
    HbStyleLoader::unregisterFilePath( gSwipeCssFilePath );
    HbStyleLoader::unregisterFilePath( gSwipeWidgetMLFilePath );
    HbStyleLoader::unregisterFilePath( gSwipeColorCssFilePath );

}

/*!
    Creates all widget primitives.
 */
void SnsrSwipeWidget::createPrimitives()
{
    if (!mSlideLabel) {
        mSlideLabel = new HbLabel(this);
        mSlideLabel->setPlainText(hbTrId("txt_screensaver_swipe"));
        mSlideLabel->setAlignment(Qt::AlignCenter);
        HbStyle::setItemName(mSlideLabel, QLatin1String("slideLabel"));
    }
    if (!mIconItem) {
        HbIconAnimationManager::global()->addDefinitionFile(gSwipeIcon);
        mIconItem = new HbIconItem(QString("qtg_anim_swipe"),this);
        HbStyle::setItemName(mIconItem, QLatin1String("slideIcon"));
        mIconItem->show();
        mIconItem->animator().startAnimation();
        mIconItem->setAlignment(Qt::AlignCenter);
    }
}

/*!
    \reimp
 */

void SnsrSwipeWidget::gestureEvent(QGestureEvent *event)
{
    SCREENSAVER_TEST_FUNC_ENTRY("SnsrSwipeWidget::gestureEvent")

    QGesture *gesture = event->gesture(Qt::SwipeGesture);
    QSwipeGesture *swipe = static_cast<QSwipeGesture *>(gesture);

    if ( swipe && swipe->state() == Qt::GestureStarted ) {
        event->accept(Qt::SwipeGesture);
        mIconItem->animator().startAnimation();
    }
    else if ( swipe && swipe->state() == Qt::GestureFinished ) {
        // unclock with downward swipe
        qreal downAngle = (mCurrentOrientation == Qt::Vertical) ? 
            gSwipeDownAngle : gLandscapeSwipeDownAngle;
        qreal swipeAngle = swipe->swipeAngle();
        if ( abs(swipeAngle - downAngle) < gSwipeAngleTolerance ) {
            emit swipeDownDetected();
            event->accept(Qt::SwipeGesture);
        }
    }
    else { 
        event->ignore(Qt::SwipeGesture);
    }

    SCREENSAVER_TEST_FUNC_EXIT("SnsrSwipeWidget::gestureEvent")
}

void SnsrSwipeWidget::setCurrentOrientation(int orientation)
{
    mCurrentOrientation = orientation;
}


