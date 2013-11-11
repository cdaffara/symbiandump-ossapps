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
 * Description: Mocked class CalenDayHourElement
 *
 */

#ifndef CALENDAYHOURELEMENT_H
#define CALENDAYHOURELEMENT_H

// System includes
#include <QTime>
#include <HbWidget>

// Test variables
extern bool gTestLocaleChanged;
extern bool gTestUpdatePerformed;

// Mocked class
class CalenDayHourElement : public HbWidget
{
Q_OBJECT

public:

    CalenDayHourElement(const QTime &time, QGraphicsItem *parent = 0) : HbWidget(parent) {
        mHour = time;
    }
    virtual ~CalenDayHourElement() {
        
    }

    void paint(
        QPainter * painter,
        const QStyleOptionGraphicsItem * option,
        QWidget * widget) {
        Q_UNUSED(painter)
        Q_UNUSED(option)
        Q_UNUSED(widget)
    }

    void setTime(const QTime &time) {
        mHour = time;
    }
    
    QTime time() const {
        return mHour;
    }
    
    void update(const QRectF &rect = QRectF()) {
        Q_UNUSED(rect)
        gTestUpdatePerformed = true;
    }
    
public slots:
    void localeChanged() {
        gTestLocaleChanged = true;
    }

private:
    QTime mHour;
};

#endif // CALENDAYHOURELEMENT_H
