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
 * Description:  CalenDayContentScrollArea class definition.
 *
 */

#ifndef CALENDAYCONTENTSCROLLAREA_H_
#define CALENDAYCONTENTSCROLLAREA_H_

// System includes
#include <hbscrollarea.h>

#include "calendaycommonheaders.h"

extern CalenScrollDirection gDisallowedDirection;

// Class declaration
class CalenDayContentScrollArea : public HbScrollArea
{
Q_OBJECT

public:
    CalenDayContentScrollArea(QGraphicsItem *parent = 0)
    {
        Q_UNUSED(parent);
    }
    
    virtual ~CalenDayContentScrollArea()
    {
    }
    
    CalenScrollDirection disallowedScrollDirection() const
    {
        return gDisallowedDirection;
    }

    void setDisallowedScrollDirection(const CalenScrollDirection direction)
    {
        gDisallowedDirection = direction;
    }

signals:
    void scrollAreaMoveStarted(CalenScrollDirection scrollTo);
    void scrollAreaMoveFinished(CalenScrollDirection scrollTo);

public slots:
    void scrollToMiddleWidget()
    {
    }

protected:
    bool scrollByAmount(const QPointF &delta)
    {
        Q_UNUSED(delta);
        return false;
    }

    void gestureEvent(QGestureEvent *event)
    {
        Q_UNUSED(event);
    }
    bool eventFilter(QObject *obj, QEvent *event)
    {
        Q_UNUSED(obj);
        Q_UNUSED(event);
        return false;
    }
    bool event(QEvent *e)
    {
        Q_UNUSED(e);
        return false;
    }
};

#endif /* CALENDAYCONTENTSCROLLAREA_H_ */
