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
#include "t_hsclockwidget.h"
#include "hsdigitalclockwidget.h"
#include "hsclockwidgettimer.h"



void TestClockWidget::createDigitalClockWidget()
{
    HsDigitalClockWidget *clock = new HsDigitalClockWidget();
    QVERIFY(clock);

    QVERIFY(clock->mBackground);
    QVERIFY(clock->mDigit1);
    QVERIFY(clock->mDigit2);
    QVERIFY(clock->mDigit3);
    QVERIFY(clock->mDigit4);
    QVERIFY(clock->mDigit5);
    QVERIFY(clock->mDigit6);

    QGraphicsSceneResizeEvent event;
    clock->resizeEvent(&event);

    delete clock;
}

void TestClockWidget::digitalClockWidgetTick()
{
    HsDigitalClockWidget *clock = new HsDigitalClockWidget(false);
    clock->tick();
    clock->setAmPm(true);
    clock->setAmPm(false);
    clock->setAmPm(false);
    delete clock;
}

void TestClockWidget::digitalClockWidgetPolish()
{
    HsDigitalClockWidget *clock = new HsDigitalClockWidget();
    HbStyleParameters params;
    clock->polish(params);
    HbIconItem *iconItem=static_cast<HbIconItem*>(clock->mDigit2);
    QVERIFY(iconItem);
    delete clock;
}

void TestClockWidget::digitalClockWidgetShape()
{
    HsDigitalClockWidget *clock = new HsDigitalClockWidget();
    QPainterPath path = clock->shape();
    delete clock;
}

void TestClockWidget::testHsClockWidgetTimer()
{
    HsClockWidgetTimer timer;
}
