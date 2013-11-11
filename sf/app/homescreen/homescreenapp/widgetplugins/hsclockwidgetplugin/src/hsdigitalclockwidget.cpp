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
* Description:  Digital clock widget
*
*/

#include <QGraphicsSceneMouseEvent>
#include <QTime>

#include <HbStyleLoader>
#include <HbIconItem>
#include <HbFrameItem>
#include <HbFrameDrawer>
#include <HbTapGesture>
#include <HbInstantFeedback>

#include "hsdigitalclockwidget.h"
#include "hsconfiguration.h"

/*!
    \class HsDigitalClockWidget
    \ingroup group_hsclockwidgetplugin
    \brief Homescreen themable digital clock widget.
*/

/*!
    Constructs widget.
*/
HsDigitalClockWidget::HsDigitalClockWidget(bool useAmPm, QGraphicsItem *parent)
  : HbWidget(parent),
    mBackground(0),
    mDigit1(0), mDigit2(0), mDigit3(0),
    mDigit4(0), mDigit5(0), mDigit6(0),
    mAmPm(0), mUseAmPm(useAmPm)
{
    HbStyleLoader::registerFilePath(":/hsdigitalclockwidget.widgetml");
    HbStyleLoader::registerFilePath(":/hsdigitalclockwidget.css");
/*
    mDigitMap.insert(QChar('0'), QString("clock_widget_zero"));
    mDigitMap.insert(QChar('1'), QString("clock_widget_one"));
    mDigitMap.insert(QChar('2'), QString("clock_widget_two"));
    mDigitMap.insert(QChar('3'), QString("clock_widget_three"));
    mDigitMap.insert(QChar('4'), QString("clock_widget_four"));
    mDigitMap.insert(QChar('5'), QString("clock_widget_five"));
    mDigitMap.insert(QChar('6'), QString("clock_widget_six"));
    mDigitMap.insert(QChar('7'), QString("clock_widget_seven"));
    mDigitMap.insert(QChar('8'), QString("clock_widget_eight"));
    mDigitMap.insert(QChar('9'), QString("clock_widget_nine"));
*/
    mDigitMap.insert(QChar('0'), QString(":/clock_widget_zero.svg"));
    mDigitMap.insert(QChar('1'), QString(":/clock_widget_one.svg"));
    mDigitMap.insert(QChar('2'), QString(":/clock_widget_two.svg"));
    mDigitMap.insert(QChar('3'), QString(":/clock_widget_three.svg"));
    mDigitMap.insert(QChar('4'), QString(":/clock_widget_four.svg"));
    mDigitMap.insert(QChar('5'), QString(":/clock_widget_five.svg"));
    mDigitMap.insert(QChar('6'), QString(":/clock_widget_six.svg"));
    mDigitMap.insert(QChar('7'), QString(":/clock_widget_seven.svg"));
    mDigitMap.insert(QChar('8'), QString(":/clock_widget_eight.svg"));
    mDigitMap.insert(QChar('9'), QString(":/clock_widget_nine.svg"));

    createPrimitives();
    grabGesture(Qt::TapGesture);
}

/*!
    Destructor.
*/
HsDigitalClockWidget::~HsDigitalClockWidget()
{
    HbStyleLoader::unregisterFilePath(":/hsdigitalclockwidget.widgetml");
    HbStyleLoader::unregisterFilePath(":/hsdigitalclockwidget.css");
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void HsDigitalClockWidget::gestureEvent(QGestureEvent *event)
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
    Return shape
*/
QPainterPath HsDigitalClockWidget::shape() const
{   
    QPainterPath path;
    path.addRect(mBackground->rect());
    return path;
}

/*!
    Updates clock visualization according to current time
 */
void HsDigitalClockWidget::tick()
{
    updatePrimitives();
}

/*!
    Toggles time format
 */
void HsDigitalClockWidget::setAmPm(bool useAmPm)
{
    bool old = mUseAmPm;
    mUseAmPm = useAmPm;
    
    if (mUseAmPm != old) {
        updatePrimitives();
    }
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void HsDigitalClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();   
}

/*!
    Creates all widget primitives.
 */
void HsDigitalClockWidget::createPrimitives()
{
/*
    HbFrameDrawer *drawer = new HbFrameDrawer(
        QLatin1String("clock_widget"), HbFrameDrawer::NinePieces);
    mBackground = new HbFrameItem(drawer, this);
    HbStyle::setItemName(mBackground, QLatin1String("background"));

    mDigit1 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit1, QLatin1String("digit1"));

    mDigit2 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit2, QLatin1String("digit2"));

    mDigit3 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit3, QLatin1String("digit3"));

    mDigit4 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit4, QLatin1String("digit4"));

    mDigit5 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit5, QLatin1String("digit5"));

    mDigit6 = new HbIconItem(QLatin1String("clock_widget_zero"), this);
    HbStyle::setItemName(mDigit6, QLatin1String("digit6"));

    if (mUseAmPm) {
        mAmPm = new HbIconItem(QLatin1String("clock_widget_am"), this);
        HbStyle::setItemName(mAmPm, QLatin1String("ampm"));
    }
*/
    HbFrameDrawer *drawer = new HbFrameDrawer(
        QLatin1String(":/clock_widget.svg"), HbFrameDrawer::NinePieces);
    mBackground = new HbFrameItem(drawer, this);
    HbStyle::setItemName(mBackground, QLatin1String("background"));

    mDigit1 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit1, QLatin1String("digit1"));

    mDigit2 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit2, QLatin1String("digit2"));

    mDigit3 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit3, QLatin1String("digit3"));

    mDigit4 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit4, QLatin1String("digit4"));

    mDigit5 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit5, QLatin1String("digit5"));

    mDigit6 = new HbIconItem(QLatin1String(":/clock_widget_zero.svg"), this);
    HbStyle::setItemName(mDigit6, QLatin1String("digit6"));

    if (mUseAmPm) {
        mAmPm = new HbIconItem(QLatin1String(":/clock_widget_am.svg"), this);
        HbStyle::setItemName(mAmPm, QLatin1String("ampm"));
    }
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void HsDigitalClockWidget::updatePrimitives()
{
    QTime time = QTime::currentTime();
    QString timeString;

    if (mUseAmPm) {
        if (!mAmPm) {
            //mAmPm = new HbIconItem(QLatin1String("clock_widget_am"), this);
            mAmPm = new HbIconItem(QLatin1String(":/clock_widget_am.svg"), this);
            HbStyle::setItemName(mAmPm, QLatin1String("ampm"));
        }
        timeString = time.toString("hh:mm:ss ap");
    } else {
        if (mAmPm) {
            delete mAmPm;
            mAmPm = 0;
        }
        timeString = time.toString("hh:mm:ss");
    }

    mDigit1->setIconName(mDigitMap[timeString[0]]);
    mDigit2->setIconName(mDigitMap[timeString[1]]);
    mDigit3->setIconName(mDigitMap[timeString[3]]);
    mDigit4->setIconName(mDigitMap[timeString[4]]);
    mDigit5->setIconName(mDigitMap[timeString[6]]);
    mDigit6->setIconName(mDigitMap[timeString[7]]);

    if (mUseAmPm) {
        if (timeString[9] == QChar('a')) {
            //mAmPm->setIconName("clock_widget_am");
            mAmPm->setIconName(":/clock_widget_am.svg");
        } else {
            //mAmPm->setIconName("clock_widget_pm");
            mAmPm->setIconName(":/clock_widget_pm.svg");
        }
    }
}
