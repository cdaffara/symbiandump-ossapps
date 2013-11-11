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

#include <QGesture>

#include "hspagetoucharea.h"
#include "hsscene.h"

HsPageTouchArea::HsPageTouchArea(QGraphicsItem *parent)
  : HbTouchArea(parent)
{
    grabGesture(Qt::TapGesture);
    grabGesture(Qt::TapAndHoldGesture);
    grabGesture(Qt::PanGesture);
}
  
HsPageTouchArea::~HsPageTouchArea()
{
}

void HsPageTouchArea::gestureEvent(QGestureEvent *event)
{
    HsScene *scene = HsScene::instance();
    
    // Tap gesture.
    QGesture *tapGesture = event->gesture(Qt::TapGesture);
    if (tapGesture) {
        switch (tapGesture->state()) {
            case Qt::GestureFinished:
                emit scene->pageTapFinished(event);
                break;
            default:
                break;
        }
    } 
    
    // Tap-and-hold gesture.
    QGesture *gesture = event->gesture(Qt::TapAndHoldGesture);
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureFinished:
                emit scene->pageTapAndHoldFinished(event);
                break;
            default:
                break;
        }
    } 

    // Pan gesture.
    gesture = event->gesture(Qt::PanGesture);
    if (gesture) {
        switch (gesture->state()) {
            case Qt::GestureStarted:
                emit scene->pagePanStarted(event);
                break;
            case Qt::GestureUpdated:
                emit scene->pagePanUpdated(event);
                break;
            case Qt::GestureFinished:
            case Qt::GestureCanceled:
                emit scene->pagePanFinished(event);
                break;
            default:
                break;
        }
    }
}
