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
 * Description:  CalenDayHourScrollArea implementation.
 *
 */

// System includes
#include <QGraphicsLinearLayout>

// User includes
#include "calendayhourscrollarea.h"
#include "calendayhourelement.h"
#include "calendayutils.h"

/*!
 \class CalenDayHourScrollArea
 \brief Scrollable container class for hour elements.
 
 It handles vertical scrolling.
 */

/*!
 \brief Constructor
 
 Configures scroll area settings and resets internal stares of widget.
 Gets the width of device.
 
 \param parent The parent of scroll area widget
 */
CalenDayHourScrollArea::CalenDayHourScrollArea(QGraphicsItem *parent) :
    HbScrollArea(parent), mDateTime(QDateTime())
{
    // Grab pan gestures
    grabGesture(Qt::PanGesture);

    // Create widget for hour elements
    HbWidget *hourWidget = new HbWidget();

    // Create and insert hour elements into vertical layout
    QGraphicsLinearLayout* hourLayout = new QGraphicsLinearLayout(Qt::Vertical,
        NULL);
    for (int i = 0; i < KCalenHoursInDay; i++) {
        CalenDayHourElement* element = new CalenDayHourElement(QTime(i, 0),
            this);
        hourLayout->addItem(element);
        mHourElements.append(element);
    }
    hourLayout->setContentsMargins(0.0, 0.0, 0.0, 0.0);
    hourLayout->setSpacing(0.0);

    // Apply hour layout for new widget and set content widget to scroll area
    hourWidget->setLayout(hourLayout);
    setContentWidget(hourWidget);
}

/*!
 \brief Destructor
 */
CalenDayHourScrollArea::~CalenDayHourScrollArea()
{
    mHourElements.clear();
}

/*!
 \brief Sets date and time for this container.  
 
 \param dateTime new date and time
 */
void CalenDayHourScrollArea::setDateTime(const QDateTime &dateTime)
{
    mDateTime = dateTime;
}

/*!
 \brief Returns date and time assigned to current view.
*/
QDateTime CalenDayHourScrollArea::dateTime() const
{
    return mDateTime;
}

/*!
 \brief Scroll view to given hour.
 
 \param  An hour in 24 hour format (0 - 23)
*/
void CalenDayHourScrollArea::scrollToHour(int hour)
{
    Q_ASSERT((hour > -1) && (hour < KCalenHoursInDay));
    
    CalenDayHourElement *hourElement = mHourElements.at(hour); 
    QRectF hourElementRect = hourElement->rect();
    hourElementRect = hourElement->mapRectToParent(hourElementRect);
    QPointF newPos = hourElementRect.topLeft();
    
    // Ensure that we won't scroll out of bound
    CalenDayHourElement *lastElement = mHourElements.last();
    qreal bottomEdge = lastElement->mapRectToParent(lastElement->rect()).bottom();
    qreal viewHeight = rect().height();

    if (bottomEdge - newPos.y() < viewHeight) {
        newPos.setY(bottomEdge - viewHeight);
    }
    
    scrollContentsTo(newPos);
}

/*!
 \brief SLOT scrolls the view vertically to new position.
 
 \param newPosition New position
 */
void CalenDayHourScrollArea::scrollVertically(const QPointF &newPosition)
{
    QPointF currentPos = contentWidget()->pos();
    if (abs(newPosition.y()) != abs(currentPos.y())) {
        currentPos.setY(newPosition.y());
        scrollContentsTo(currentPos, 0);
    }
}

/*!
 \brief SLOT updates time/date representation when system locale are changed.
 */
void CalenDayHourScrollArea::localeChanged() 
{
    for (int i = 0; i < mHourElements.count(); i++) {
        mHourElements.at(i)->localeChanged();
        mHourElements.at(i)->update(mHourElements.at(i)->rect());
    }
}

/*!
 \brief SLOT refreshes time indicator if current day is displayed.
 
 Time indicator should also be updated if transition between prev/next
 day was done previously: failure ou1cimx1#565650 fixed
 */
void CalenDayHourScrollArea::updateTimeIndicator()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    
    bool updateNeeded = mDateTime.date() >= currentDateTime.date().addDays(-1);
    updateNeeded = updateNeeded && mDateTime.date() <= currentDateTime.date().addDays(1);
    
    if (updateNeeded) {
        for (int i = 0; i < mHourElements.count(); i++) {
            mHourElements.at(i)->update(mHourElements.at(i)->rect());
        }
    }
}

// End of File
