/*
* Copyright (c) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test code
*
*/



#include <QtTest/QtTest>

#include <QGraphicsSceneResizeEvent>
#include <HbStyleParameters>
#include <HbIconItem>

#include "t_hsclockwidget.h"
#include "hsanalogclockwidget.h"


void TestClockWidget::createAnalogClockWidget()
{
    HsAnalogClockWidget *clock = new HsAnalogClockWidget();
    QVERIFY(clock);

    QVERIFY(clock->mBackground);
    QVERIFY(clock->mHourHand);
    QVERIFY(clock->mMinuteHand);
    QVERIFY(clock->mSecondHand);

    QGraphicsSceneResizeEvent event;
    clock->resizeEvent(&event);

    delete clock;
}

void TestClockWidget::analogClockWidgetTick()
{
    HsAnalogClockWidget *clock = new HsAnalogClockWidget();
    clock->tick();
    delete clock;
}

void TestClockWidget::analogClockWidgetPolish()
{
    HsAnalogClockWidget *clock = new HsAnalogClockWidget();
    HbStyleParameters params;
    clock->polish(params);
    HbIconItem *iconItem=static_cast<HbIconItem*>(clock->mHourHand);
    QVERIFY(iconItem);
    delete clock;
}

void TestClockWidget::analogClockWidgetShape()
{
    HsAnalogClockWidget *clock = new HsAnalogClockWidget();
    QPainterPath path = clock->shape();
    delete clock;
}

// end of file
