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
* Description:
*
*/

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGesture>
#include <QTouchEvent>

#include <HbTapGesture>
#include <HbPanGesture>

#include "hswidgettoucharea.h"
#include "hswidgethostvisual.h"
#include "hsscene.h"

HsWidgetTouchArea::HsWidgetTouchArea(HsWidgetHostVisual *visual)
  : HbTouchArea(visual),
    mWidgetHostVisual(visual)
{
    grabGesture(Qt::TapAndHoldGesture);
}
  
HsWidgetTouchArea::~HsWidgetTouchArea()
{
}

bool HsWidgetTouchArea::sceneEvent(QEvent *event)
{
    HsScene *scene = HsScene::instance();
    switch (event->type()) {
        case QEvent::TouchBegin:
            {
            QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
            QPointF scenePos;
            if (touchEvent && !touchEvent->touchPoints().isEmpty() ) {
                scenePos = static_cast<QTouchEvent *>(event)->touchPoints().first().scenePos();
                }
            emit scene->widgetTapStarted(scenePos, mWidgetHostVisual->visualModel());
            break;        
            }
        case QEvent::GraphicsSceneMousePress:
            {
            QPointF scenePos = static_cast<QGraphicsSceneMouseEvent *>(event)->lastScenePos();
            emit scene->widgetTapStarted(scenePos, mWidgetHostVisual->visualModel());
            break;        
            }
        case QEvent::TouchEnd:
            {
                ungrabGesture(Qt::PanGesture);
                ungrabMouse();
                QPointF scenePos = static_cast<QTouchEvent *>(event)->touchPoints().first().scenePos();
                emit scene->widgetMoveFinished(scenePos, mWidgetHostVisual->visualModel());
            }
            break;
        case QEvent::GraphicsSceneMouseRelease:
            {
                ungrabGesture(Qt::PanGesture);
                ungrabMouse();
                QPointF scenePos = static_cast<QGraphicsSceneMouseEvent *>(event)->scenePos();
                emit scene->widgetMoveFinished(scenePos, mWidgetHostVisual->visualModel());
            }
            break;    
        default:
            break;
    }

    return HbTouchArea::sceneEvent(event);
}

QPainterPath HsWidgetTouchArea::shape() const
{       
    return mWidgetHostVisual->shape();
}

void HsWidgetTouchArea::gestureEvent(QGestureEvent *event)
{
    HsScene *scene = HsScene::instance();
 
    // Tap-and-hold gesture.
    QGesture *gesture = event->gesture(Qt::TapAndHoldGesture);
    if (gesture) {
        if (gesture->state() == Qt::GestureFinished) {
            grabGesture(Qt::PanGesture);
            grabMouse();
            emit scene->widgetTapAndHoldFinished(event, mWidgetHostVisual->visualModel());
        }
        return;
    }

    // Pan gesture.
    gesture = event->gesture(Qt::PanGesture);
    if (gesture) {
        QPointF scenePos = event->mapToGraphicsScene(gesture->hotSpot());
        switch (gesture->state()) {
            case Qt::GestureStarted:
            case Qt::GestureUpdated:
                emit scene->widgetMoveUpdated(scenePos, mWidgetHostVisual->visualModel());
                break;
            case Qt::GestureCanceled:
            case Qt::GestureFinished:
                ungrabGesture(Qt::PanGesture);
                emit scene->widgetMoveFinished(scenePos, mWidgetHostVisual->visualModel());
                break;
            default:
                break;
        }
        return;
    }
}
