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
 * Description:  Day view control of calendar
 *
 */

// System includes
#include <QGraphicsLinearLayout>
#include <QPainter>
#include <HbTextItem>
#include <HbStyle>
#include <HbColorScheme>
#include <HbFontSpec>
#include <HbExtendedLocale>

// User includes
#include "calendayhourelement.h"
#include "calendaycontainer.h"
#include "calendayhourscrollarea.h"

// Constants
/*!
 \brief Time format is: [0-9][0-9]:[0-9][0-9]
 */
const int KCalenTimeFormatLength = 5;

/*!
 \class CalenDayHourElement
 \brief Hour element widget for Calendar's Day View
 */

/*!
 \brief Constructor
 
 \param time Time assigned to widget
 \param parent Widget's parent
 */
CalenDayHourElement::CalenDayHourElement(
    const QTime &time,
    QGraphicsItem *parent) :
    HbWidget(parent), mHour(time), mTimeTextItem(NULL), mAmpmTextItem(NULL)
{
    // Necessary when widget implements own paint method
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    
    HbDeviceProfile deviceProfile;
    mUnitInPixels = deviceProfile.unitValue();
    mHour = time;

    // Initialize hour line color
    mHourLineColor = HbColorScheme::color(KCalenHourLineColor); 
    
    mTimeTextItem = new HbTextItem(this);
    mAmpmTextItem = new HbTextItem(this);
    HbStyle::setItemName(mTimeTextItem, QLatin1String("time"));
    HbStyle::setItemName(mAmpmTextItem, QLatin1String("ampm"));
    
    setupWithLocale();

    // Parent container is needed to update widget's time
    mContainer = static_cast<CalenDayHourScrollArea*> (parent);
}

/*!
 \brief Destructor
 */
CalenDayHourElement::~CalenDayHourElement()
{
}

/*!
 \brief Customized paint() function
 
 \param painter Painter
 \param option Style option
 \param widget
 */
void CalenDayHourElement::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *option,
    QWidget *widget)
{
    Q_UNUSED(widget);

    painter->save();
    
    QRectF drawArea = option->rect;

    // Draw full hour line
    QPen linePen = QPen(mHourLineColor, KCalenHourLineThickness * mUnitInPixels);
    painter->setPen(linePen);
    
    QLineF fullHourLine(drawArea.bottomLeft(), drawArea.bottomRight());
    painter->drawLine(fullHourLine);

    // Draw extra line on top for midnight
    if (mHour.hour() == 0) {
        fullHourLine = QLineF(drawArea.topLeft(), drawArea.topRight());
        painter->drawLine(fullHourLine);
    }

    QDateTime currentDateTime = QDateTime::currentDateTime();

    // Draw the time line in theme color
    if (mContainer) {
        QDateTime containersDateTime = mContainer->dateTime();

        if (currentDateTime.date() == containersDateTime.date()
            && currentDateTime.time().hour() == mHour.hour()) {

            qreal currentTimeY = drawArea.height()
                * currentDateTime.time().minute() / 60;

            QColor color = HbColorScheme::color(KCalenTimeLineColor);
            painter->setPen(QPen(color, KCalenCurrentTimeLineThickness
                * mUnitInPixels, Qt::SolidLine, Qt::FlatCap));
            QLineF currentTimeline(drawArea.left(), drawArea.top() + currentTimeY,
                    drawArea.right(), drawArea.top() + currentTimeY);
            painter->drawLine(currentTimeline);
        }
    }

    painter->restore();
}

/*!
 \brief Sets time for hour element.
 
 \param time Time to be set for hour element
 */
void CalenDayHourElement::setTime(const QTime &time)
{
    mHour = time;
}

/*!
 \brief Returns time of hour element.
 
 \return Time of hour element
 */
QTime CalenDayHourElement::time() const
{
    return mHour;
}

/*!
 \brief SLOT updates control when system locale changes
 */
void CalenDayHourElement::localeChanged()
{
    setupWithLocale();
    repolish();
}

/*!
 \brief Sets up internal data using current system locale.
 */
void CalenDayHourElement::setupWithLocale()
{
    HbDeviceProfile deviceProfile;

    // Create text items
    HbExtendedLocale systemLocale = HbExtendedLocale::system();

    // Get current time format and (if there's a need) separate time from am/pm text
    QChar timeSeparator = ' ';
    QStringList timeTextList = systemLocale.format(mHour, r_qtn_time_usual_with_zero).split(timeSeparator);

    // If needed, prepend '0' to get proper time format: [0-9][0-9]:[0-9][0-9]
    QString timeString = timeTextList[0];
    if (timeString.length() < KCalenTimeFormatLength) {
        timeString.prepend('0');
    }

    QString ampmString = "";
    if (timeTextList.count() > 1) {
        ampmString = timeTextList[1].toLower();
    }
    
    mTimeTextItem->setText(timeString);
    mAmpmTextItem->setText(ampmString);
}

// End of File
