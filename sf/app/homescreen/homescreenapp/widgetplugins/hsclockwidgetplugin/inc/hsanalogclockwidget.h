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
* Description:  Clock widget
*
*/

#ifndef HSANALOGCLOCKWIDGET_H
#define HSANALOGCLOCKWIDGET_H

#include <HbWidget>

#include <hstest_global.h>

HOMESCREEN_TEST_CLASS(TestClockWidget)

class HbIconItem;
class QGestureEvent;

class HsAnalogClockWidget : public HbWidget
{
    Q_OBJECT

public:
    explicit HsAnalogClockWidget(QGraphicsItem *parent = 0);
    ~HsAnalogClockWidget();

    QRectF boundingRect() const;
    QPainterPath shape() const;

signals:
    void clockTapped();

public slots:
    void tick();

protected:
    void resizeEvent(QGraphicsSceneResizeEvent *event);
    void gestureEvent(QGestureEvent *event);

private:
    Q_DISABLE_COPY(HsAnalogClockWidget)
    void createPrimitives();
    void updatePrimitives();

private:
    HbIconItem *mBackground;
    HbIconItem *mHourHand;
    HbIconItem *mMinuteHand;
    HbIconItem *mSecondHand;

    HOMESCREEN_TEST_FRIEND_CLASS(TestClockWidget)
};

#endif
