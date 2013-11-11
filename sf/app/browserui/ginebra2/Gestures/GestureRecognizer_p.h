/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, version 2.1 of the License.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not,
* see "http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html/".
*
* Description:
*
*/

/*
 W A R N I N G
 -------------
 THIS IS A TEMPORARY GESTURE CODE. WOULD BE REPLACED WHEN BROWSER HAS ITS OWN GESTURE FRAMEWORK
 */

#ifndef GestureRecognizer_p_h
#define GestureRecognizer_p_h

#include "GestureEvent.h"

#include <QBasicTimer>
#include <QObject>
#include <QTime>

class QGraphicsSceneMouseEvent;
class QPoint;
class QPointF;

namespace GVA {

class GestureListener;
class GestureRecognizer;

class GestureRecognizerPrivate : public QObject {
    Q_OBJECT
    Q_DECLARE_PUBLIC(GestureRecognizer)

public:
    //States of gesture recognizer
    enum State {
        Inactive,
        Press,
        Release,
        Move,
        DoublePress,
        LongPress
    };

    GestureRecognizerPrivate(GestureListener*);
    ~GestureRecognizerPrivate();

    bool mousePressEvent(QGraphicsSceneMouseEvent* event);
    bool mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    bool mouseMoveEvent(QGraphicsSceneMouseEvent* event);
    bool mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

protected:
    void changeState(State state);
    GestureEvent gestureEvent(const QPointF& pos, GestureEvent::Type type);
    void timerEvent(QTimerEvent* event);
    void resetTouchPositions();
    QPointF calculateVelocity(const QPointF& delta, int time);

private:
    GestureRecognizer* q_ptr;
    GestureListener* m_gestureListener;

    // state
    State m_state;

    //last moved time
    QTime m_lastTime;
    //time elapsed since last press
    QTime m_delayedPressMoment;
    //common timer for long tap or double tap recognition
    QBasicTimer m_timer;

    //Velocity factors
    qreal m_dragInertia;
    int m_directionErrorMargin;
    qreal m_axisLockThreshold;
    qreal m_maxVelocity;
    qreal m_minVelocity;
    int m_panningThreshold;
    qreal m_fastVelocityFactor;
    int m_scrollsPerSecond;

    QPointF m_velocity;

    //Touch positions
    QPointF m_position;
    QPointF m_initialPos;

    QGraphicsSceneMouseEvent* m_delayedReleaseEvent;
}; //GestureRecognizerPrivate

} //namespace GVA
#endif //GestureRecognizer_p_h
