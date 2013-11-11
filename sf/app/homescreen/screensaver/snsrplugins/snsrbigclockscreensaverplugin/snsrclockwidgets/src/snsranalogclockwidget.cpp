/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Analog Clock Widget.
*
*/

#include "snsranalogclockwidget.h"

#include <QGraphicsSvgItem>
#include <QTime>
#include <QDebug>
#include <HbExtendedLocale>
#include <HbStyleLoader>
#include <HbIconItem>
#include <hbevent.h>
#include <HbColorScheme>
#include <HbTextItem>


const char *gClockCssFilePath =":/style/snsranalogclockwidget.css";
const char *gClockWidgetMLFilePath = ":/style/snsranalogclockwidget.widgetml";

const QString amPmLabelTextColorRole("qtc_screensaver_analog_clock");

/*!
    \class SnsrAnalogClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver analog clock widget.
 */

/*!
    Constructs a new SnsrAnalogClockWidget.
    \param parent Parent object.
 */
SnsrAnalogClockWidget::SnsrAnalogClockWidget(QGraphicsItem* parent):
    HbWidget(parent),
    mClockBackground(0), mClockHourHand(0), 
    mClockMinuteHand(0), mClockSecondHand(0),
    mClockAmPmLabel(0)
{
    HbStyleLoader::registerFilePath(gClockCssFilePath);
    HbStyleLoader::registerFilePath(gClockWidgetMLFilePath);
    
    createPrimitives();
    updatePrimitives();
}

/*!
    Destructs the class.
 */
SnsrAnalogClockWidget::~SnsrAnalogClockWidget()
{    
    HbStyleLoader::unregisterFilePath( gClockCssFilePath );
    HbStyleLoader::unregisterFilePath( gClockWidgetMLFilePath );

}

void SnsrAnalogClockWidget::polish( HbStyleParameters& params )
{
    // TODO: updatePrimitives might not be needed to be called from here anymore (hb changes)?
    HbWidget::polish(params);
    updatePrimitives();
}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void SnsrAnalogClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();
}

/*!
    Creates all widget primitives.
 */
void SnsrAnalogClockWidget::createPrimitives()
{
    // TODO: when final graphics are available in platform (qtg_graf_screensaver_clock_xxx)
    // use them instead.
    if (!mClockBackground) {
        mClockBackground = new HbIconItem(QLatin1String("qtg_graf_clock_day_bg"), this);
        HbStyle::setItemName(mClockBackground, QLatin1String("analog_clock_background"));
    }
    if (!mClockHourHand) {
        mClockHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_hour"), this);
        HbStyle::setItemName(mClockHourHand, QLatin1String("analog_clock_hour_hand"));
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_min"), this);
        HbStyle::setItemName(mClockMinuteHand, QLatin1String("analog_clock_minute_hand"));
    }
    if (!mClockSecondHand) {
        mClockSecondHand = new HbIconItem(QLatin1String("qtg_graf_clock_day_sec"), this);
        HbStyle::setItemName(mClockSecondHand, QLatin1String("analog_clock_second_hand"));
    }
    if (!mClockAmPmLabel) {
        mClockAmPmLabel = new HbTextItem(this);
        HbStyle::setItemName(mClockAmPmLabel, QLatin1String("analog_clock_am_pm_label"));
        mClockAmPmLabel->setTextColor(HbColorScheme::color(amPmLabelTextColorRole));
        mClockAmPmLabel->setMinimumLines(1);
        mClockAmPmLabel->setMaximumLines(1);
        mClockAmPmLabel->setTextWrapping(Hb::TextNoWrap);
        HbFontSpec textFont(HbFontSpec::Primary);
        qreal textHeight = 0;
        style()->parameter(QLatin1String("hb-param-text-height-tiny"), textHeight);
        textFont.setTextHeight(textHeight);
        mClockAmPmLabel->setFontSpec(textFont);
    }
}

/*!
    @copydoc HbWidget::updatePrimitives()
 */
void SnsrAnalogClockWidget::updatePrimitives()
{
    if (!mClockHourHand || !mClockMinuteHand || !mClockSecondHand || !mClockAmPmLabel) {
        createPrimitives();
    }
    
    // Calculate angles for clock hands.
    QTime time = QTime::currentTime();
    qreal s = 6 * time.second();
    qreal m = 6 * (time.minute() + s/360);
    qreal h = 30 * ((time.hour() % 12) + m/360);
    
    int x = mClockHourHand->preferredSize().width()/2;
    int y = mClockHourHand->preferredSize().height()/2;
    mClockHourHand->setTransform(QTransform().translate(x, y).rotate(h).translate(-x, -y));

    x = mClockMinuteHand->preferredSize().width()/2;
    y = mClockMinuteHand->preferredSize().height()/2;
    mClockMinuteHand->setTransform(QTransform().translate(x, y).rotate(m).translate(-x, -y));

    x = mClockSecondHand->preferredSize().width()/2;
    y = mClockSecondHand->preferredSize().height()/2;
    mClockSecondHand->setTransform(QTransform().translate(x, y).rotate(s).translate(-x, -y));

    QString amPmString = (time.hour()<12) ? HbExtendedLocale().amText() : HbExtendedLocale().pmText();
    mClockAmPmLabel->setText( amPmString );
}

/*!
    Updates primitives when time change.
 */
void SnsrAnalogClockWidget::tick()
{
    updatePrimitives();
}

/*!
    \reimp
 */
void SnsrAnalogClockWidget::changeEvent(QEvent * event)
{
    if (event->type() == HbEvent::ThemeChanged) {
        mClockAmPmLabel->setTextColor(HbColorScheme::color(amPmLabelTextColorRole));
    }
    return HbWidget::changeEvent(event);
}
