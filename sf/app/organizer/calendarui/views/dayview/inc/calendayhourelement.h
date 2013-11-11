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

#ifndef CALENDAYHOURELEMENT_H
#define CALENDAYHOURELEMENT_H

// System includes
#include <QTime>
#include <QPen>
#include <HbWidget>
#include <HbTextItem>

// User includes


// Forward declarations
class CalenDayHourScrollArea;

class CalenDayHourElement : public HbWidget
{
Q_OBJECT

public:

    CalenDayHourElement(const QTime &time, QGraphicsItem *parent = 0);
    virtual ~CalenDayHourElement();

    void paint(
        QPainter * painter,
        const QStyleOptionGraphicsItem * option,
        QWidget * widget);

    void setTime(const QTime &time);
    QTime time() const;
    
public slots:
    void localeChanged();

private:
    void setupWithLocale();

private:
    const CalenDayHourScrollArea *mContainer;
    QTime mHour;
    QColor mHourLineColor;
    qreal mUnitInPixels;
    
    HbTextItem *mTimeTextItem;  //!< Text item for displaying time
    HbTextItem *mAmpmTextItem;  //!< Test item for displaying am/pm
};

#endif // CALENDAYHOURELEMENT_H
