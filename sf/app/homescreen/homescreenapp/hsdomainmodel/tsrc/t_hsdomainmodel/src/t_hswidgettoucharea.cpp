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
* Description:
*
*/

#include <HbTapAndHoldGesture>
#include <HbPanGesture>

#include "t_hsdomainmodel.h"
#include "hswidgethost.h"
#include "hswidgettoucharea.h"
#include "hsscene.h"



void TestHsDomainModel::testWidgetTouchAreaConstruction()
{
    HsWidgetHost *widget = new HsWidgetHost(-1);
    HsWidgetTouchArea *widgetTouchArea = new HsWidgetTouchArea(widget->visual());
    QVERIFY(widgetTouchArea);
    delete widgetTouchArea;   
}

void TestHsDomainModel::testWidgetTouchAreaSceneEvent()
{
    HsWidgetHost *widget = new HsWidgetHost(-1);
    HsWidgetTouchArea *widgetTouchArea = new HsWidgetTouchArea(widget->visual());
    HsScene *scene = HsScene::instance();

    // test shape fetch
    QPainterPath *path = new QPainterPath(widgetTouchArea->shape());
    QVERIFY(path);
    delete path;

    QSignalSpy widgetTapStartedSpy(scene, SIGNAL(widgetTapStarted(QPointF,HsWidgetHost*)));
    QSignalSpy widgetMoveFinishedSpy(scene, SIGNAL(widgetMoveFinished(const QPointF &, HsWidgetHost*)));

    // test touch begin
    QTouchEvent::TouchPoint touchPoint;
    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(touchPoint);
    QTouchEvent *touchEvent = new QTouchEvent(QEvent::TouchBegin);
    touchEvent->setTouchPoints(touchPoints);
    widgetTouchArea->sceneEvent(touchEvent);
    QCOMPARE(widgetTapStartedSpy.count(), 1);

    delete widgetTouchArea;    
    delete widget;  
}


void TestHsDomainModel::testWidgetTouchAreaGestureEvent()
{
    HsWidgetHost *widget = new HsWidgetHost(-1);
    HsWidgetTouchArea *widgetTouchArea = new HsWidgetTouchArea(widget->visual());
    QList<QGesture*> gestureList;

    // test empty gesture list
    widgetTouchArea->gestureEvent(new QGestureEvent(gestureList));

    // test tapAndHold (not real test as gestures need to be created using qesturerecognizer
    HbTapAndHoldGesture *tapAndHoldGesture = new HbTapAndHoldGesture();
    gestureList << tapAndHoldGesture;
    QGestureEvent *gestureEvent = new QGestureEvent(gestureList);
    widgetTouchArea->gestureEvent(gestureEvent);
    
    gestureList.clear();

    // test pan
    HbPanGesture *panGesture = new HbPanGesture();
    gestureList << panGesture;
    gestureEvent = new QGestureEvent(gestureList);
    widgetTouchArea->gestureEvent(gestureEvent);

    delete tapAndHoldGesture;
    delete panGesture;
    delete widgetTouchArea;
}
