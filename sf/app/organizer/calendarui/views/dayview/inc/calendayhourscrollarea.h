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
#include <QDateTime>
#include <HbScrollArea>

// User includes
#include "calendaycommonheaders.h"

// Forward declarations
class CalenDayHourElement;

#ifdef CALENVIEWS_DLL
#define CALENDAYHOURSCROLLAREA_EXPORT Q_DECL_EXPORT
#else
#define CALENDAYHOURSCROLLAREA_EXPORT Q_DECL_IMPORT
#endif

// Class declaration
class CALENDAYHOURSCROLLAREA_EXPORT CalenDayHourScrollArea : public HbScrollArea
{
    Q_OBJECT

public:
    CalenDayHourScrollArea(QGraphicsItem *parent = 0);
    virtual ~CalenDayHourScrollArea();
    
    void setDateTime(const QDateTime &dateTime);
    QDateTime dateTime() const;
    
    void scrollToHour(int hour);
    
public slots:
    void scrollVertically(const QPointF &newPosition);
    void localeChanged();
    void updateTimeIndicator();
    
private:
    QDateTime mDateTime;
    QList<CalenDayHourElement*> mHourElements;
};

#endif /* CALENDAYHOURSCROLLAREA_H_ */
