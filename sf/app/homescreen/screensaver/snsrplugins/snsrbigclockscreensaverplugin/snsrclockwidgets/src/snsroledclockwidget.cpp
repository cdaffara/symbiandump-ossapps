/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Oled Clock Widget.
*
*/

#include "snsroledclockwidget.h"
#include "snsrlabel.h"
#include "snsrcolors.h"

#include <QGraphicsSvgItem>
#include <QTime>
#include <QDebug>
#include <QLocale>
#include <HbExtendedLocale>
#include <HbStyleLoader>
#include <HbIconItem>
#include <HbTextItem>


const char *gOledClockCssFilePath = ":/style/snsroledclockwidget.css";
const char *gOledClockWidgetMLFilePath = ":/style/snsroledclockwidget.widgetml";

/*!
    \class SnsrOledClockWidget
    \ingroup group_snsrbigclockscreensaverprovider
    \brief Screensaver oled clock widget.
 */

/*!
    Constructs a new SnsrOledClockWidget.
    \param parent Parent object.
 */
SnsrOledClockWidget::SnsrOledClockWidget(QGraphicsItem* parent):
    HbWidget(parent),
    mClockBackground(0), mClockHourHand(0), mClockMinuteHand(0),
    mClockAmPmLabel(0)
{
    HbStyleLoader::registerFilePath(gOledClockCssFilePath);
    HbStyleLoader::registerFilePath(gOledClockWidgetMLFilePath);

    createPrimitives();
    updatePrimitives();
}

void SnsrOledClockWidget::polish( HbStyleParameters& params )
{
    // TODO: updatePrimitives might not be needed to be called from here anymore (hb changes)?
    HbWidget::polish(params);
    updatePrimitives();
}

/*!
    Destructs the class.
 */
SnsrOledClockWidget::~SnsrOledClockWidget()
{
    HbStyleLoader::unregisterFilePath( gOledClockCssFilePath );
    HbStyleLoader::unregisterFilePath( gOledClockWidgetMLFilePath );

}

/*!
    @copydoc HbWidget::resizeEvent()
 */
void SnsrOledClockWidget::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    HbWidget::resizeEvent(event);
    updatePrimitives();
}

/*!
    Creates all widget primitives.
 */
void SnsrOledClockWidget::createPrimitives()
{
    // TODO: final graphics will be named qtg_graf_screensaver_clock_oled_xxx.
    // not available in platform yet

    if (!mClockBackground) {
        mClockBackground = new HbIconItem(QLatin1String("qtg_graf_clock_night_bg"), this);
        HbStyle::setItemName(mClockBackground, QLatin1String("oled_clock_background"));
    }
    if (!mClockHourHand) {
        mClockHourHand = new HbIconItem(QLatin1String("qtg_graf_clock_night_hour"), this);
        HbStyle::setItemName(mClockHourHand, QLatin1String("oled_clock_hour_hand"));
    }
    if (!mClockMinuteHand) {
        mClockMinuteHand = new HbIconItem(QLatin1String("qtg_graf_clock_night_min"), this);
        HbStyle::setItemName(mClockMinuteHand, QLatin1String("oled_clock_minute_hand"));
    }
    if (!mClockAmPmLabel) {
        mClockAmPmLabel = new HbTextItem(this);
        HbStyle::setItemName(mClockAmPmLabel, QLatin1String("oled_clock_am_pm_label"));
        mClockAmPmLabel->setTextColor(SnsrColors::PowerSaveModeWidgetColor);
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
void SnsrOledClockWidget::updatePrimitives()
{
    if (!mClockHourHand || !mClockMinuteHand || !mClockAmPmLabel) {
        createPrimitives();
    }
    Q_ASSERT( mClockHourHand && mClockMinuteHand && mClockAmPmLabel );
    
    // Calculate angles for clock hands.
    // Use granularity of one minute so that minute hand is always exactly
    // on some minute and not between minutes. OLED clock is not updated more
    // frequently than once per minute and using finer granularity would cause
    // the minute hand to be always between even minutes.
    QTime time = QTime::currentTime();
    qreal m = 6 * time.minute();
    qreal h = 30 * ((time.hour() % 12) + m/360);
    
    int x = mClockHourHand->preferredSize().width()/2;
    int y = mClockHourHand->preferredSize().height()/2;
    mClockHourHand->setTransform(QTransform().translate(x, y).rotate(h).translate(-x, -y));

    x = mClockMinuteHand->preferredSize().width()/2;
    y = mClockMinuteHand->preferredSize().height()/2;
    mClockMinuteHand->setTransform(QTransform().translate(x, y).rotate(m).translate(-x, -y));

    QString amPmString = (time.hour()<12) ? HbExtendedLocale().amText() : HbExtendedLocale().pmText();
    mClockAmPmLabel->setText( amPmString );
}

/*!
    Updates primitives when time change.
 */
void SnsrOledClockWidget::tick()
{
    updatePrimitives();
}
