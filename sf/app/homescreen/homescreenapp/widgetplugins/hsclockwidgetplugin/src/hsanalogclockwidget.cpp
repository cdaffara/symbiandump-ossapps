/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Analog clock widget
*
*/

#include <QGraphicsSceneMouseEvent>
#include <QTime>

#include <HbStyleLoader>
#include <HbIconItem>
#include <HbTapGesture>
#include <HbInstantFeedback>

#include "hsconfiguration.h"
#include "hsanalogclockwidget.h"

/*!
    \class HsAnalogClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Homescreen themable analog clock widget.
*/

/*!
    Constructs widget.
*/
HsAnalogClockWidget::HsAnalogClockWidget(QGraphicsItem *parent)
    : HbWidget(parent),
      mBackground(0), mHourHand(0), mMinuteHand(0), 
      mSecondHand(0)
{
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsanalogclockwidget.css");

    createPrimitives();
    grabGesture(Qt::TapGesture);
}

/*!
    Destructor.
*/
HsAnalogClockWidget::~HsAnalogClockWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsanalogclockwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsanalogclockwidget.css");
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsAnalogClockWidget::gestureEvent(QGestureEvent *event)
{
    HbTapGesture *gesture = qobject_cast<HbTapGesture *>(event->gesture(Qt::TapGesture));
    if (gesture) {
        if (gesture->state() == Qt::GestureFinished) {
            if (gesture->tapStyleHint() == HbTapGesture::Tap) {
                    HbInstantFeedback::play(HSCONFIGURATION_GET(clockWidgetTapFeedbackEffect));
                    emit clockTapped();                
            }
        }
    }
}
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT


/*!
    Return bounding rect
*/
QRectF HsAnalogClockWidget::boundingRect() const
{   
    QRectF currRect = rect();
    currRect.setHeight(mBackground->iconItemSize().height());
    currRect.setWidth(mBackground->iconItemSize().width());
    return currRect;
}

/*!
    Return shape
*/
QPainterPath HsAnalogClockWidget::shape() const
{   
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

/*!
    Updates clock visualization according to current time
 */
void HsAnalogClockWidget::tick()
{
    updatePrimitives();
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void HsAnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();   
}

/*!
    Creates all widget primitives.
 */
void HsAnalogClockWidget::createPrimitives()
{
    mBackground = new HbIconItem(QLatin1String("qtg_graf_clock_day_bg"), this);
    HbStyle::setItemName(mBackground, QLatin1String("background"));

    mHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_hour"), this);
    HbStyle::setItemName(mHourHand, QLatin1String("hour_hand"));

    mMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_min"), this);
    HbStyle::setItemName(mMinuteHand, QLatin1String("minute_hand"));

    mSecondHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_sec"), this);
    HbStyle::setItemName(mSecondHand, QLatin1String("second_hand"));
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void HsAnalogClockWidget::updatePrimitives()
{
    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);

    int x = mBackground->iconItemSize().width()/2;
    int y = mBackground->iconItemSize().height()/2;
    QPointF originPoint = QPointF(x, y);

    mHourHand->setTransformOriginPoint(originPoint);
    mHourHand->setRotation(h);

    mMinuteHand->setTransformOriginPoint(originPoint);
    mMinuteHand->setRotation(m);

    mSecondHand->setTransformOriginPoint(originPoint);
    mSecondHand->setRotation(s);
}
