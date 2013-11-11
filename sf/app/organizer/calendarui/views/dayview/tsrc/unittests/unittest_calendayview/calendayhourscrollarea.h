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
 * Description:  CalenDayHourScrollArea class definition.
 *
 */

#ifndef CALENDAYHOURSCROLLAREA_H_
#define CALENDAYHOURSCROLLAREA_H_

// System includes
#include <hbscrollarea.h>
#include <QDateTime>

// User includes
#include "calendaycommonheaders.h"

extern  qint32 LOCALE_CHANGED;

// Class declaration
class CalenDayHourScrollArea : public HbScrollArea
{
Q_OBJECT

public:
    CalenDayHourScrollArea(QGraphicsItem *parent = 0)
    {
        Q_UNUSED(parent);
    }
    virtual ~CalenDayHourScrollArea()
    {
    }
    void setDateTime(const QDateTime &dateTime)
    {
        Q_UNUSED(dateTime);
    }
    QDateTime dateTime()
    {
        return QDateTime();
    }

    void scrollToHour(int hour)
    {
        Q_UNUSED(hour);
    }

public slots:
    void scrollVertically(const QPointF &newPosition)
    {
        Q_UNUSED(newPosition);
    }
    
    void localeChanged()
    {
        LOCALE_CHANGED++;
    }
    
    void updateTimeIndicator()
    {

    }
   
};

#endif /* CALENDAYHOURSCROLLAREA_H_ */
