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

#include "GestureRecognizer.h"

#include "GestureListener.h"
#include "GestureRecognizer_p.h"

#include <QtGui>

static const int DoubleClickFilterDuration = 300;
static const int LongClickFilterDuration = 1000;
static const int MinTimeHoldForClick = 50;
static const int ThresholdForMove = 30;

namespace GVA {

GestureRecognizer::GestureRecognizer(GestureListener* gestureListener)
    : d_ptr(new GestureRecognizerPrivate(gestureListener))
{
    Q_D(GestureRecognizer);
    d->q_ptr = this;
}

GestureRecognizer::~GestureRecognizer()
{ }

bool GestureRecognizer::mouseEventFilter(QGraphicsSceneMouseEvent* event)
{
    Q_D(GestureRecognizer);
    bool handled = false;

    switch(event->type()) {
    case QEvent::GraphicsSceneMouseDoubleClick:
        handled = d->mouseDoubleClickEvent(event);
        break;
    case QEvent::GraphicsSceneMouseMove:
        handled = d->mouseMoveEvent(event);
        break;
    case QEvent::GraphicsSceneMousePress:
        handled = d->mousePressEvent(event);
        break;
    case QEvent::GraphicsSceneMouseRelease:
        handled = d->mouseReleaseEvent(event);
        break;
    case QEvent::GraphicsSceneContextMenu:
        //Swallow context menu event.
        //Since we have own way of handling it
        handled = true;
    default:
        break;
    }
    return handled;
}

qreal GestureRecognizer::dragInertia() const
{
    Q_D(const GestureRecognizer);
    return d->m_dragInertia;
}

void GestureRecognizer::setDragInertia(qreal inertia)
{
    Q_D(GestureRecognizer);
    d->m_dragInertia = inertia;
}

int GestureRecognizer::directionErrorMargin() const
{
    Q_D(const GestureRecognizer);
    return d->m_directionErrorMargin;
}

void GestureRecognizer::setDirectionErrorMargin(int errorMargin)
{
    Q_D(GestureRecognizer);
    d->m_directionErrorMargin = errorMargin;
}

qreal GestureRecognizer::axisLockThreshold() const
{
    Q_D(const GestureRecognizer);
    return d->m_axisLockThreshold;
}

void GestureRecognizer::setAxisLockThreshold(qreal threshold)
{
    Q_D(GestureRecognizer);
    d->m_axisLockThreshold = threshold;
}

qreal GestureRecognizer::maximumVelocity() const
{
    Q_D(const GestureRecognizer);
    return d->m_maxVelocity;
}

void GestureRecognizer::setMaximumVelocity(qreal v)
{
    Q_D(GestureRecognizer);
    d->m_maxVelocity = v;
}

qreal GestureRecognizer::minimumVelocity() const
{
    Q_D(const GestureRecognizer);
    return d->m_minVelocity;
}

void GestureRecognizer::setMinimumVelocity(qreal v)
{
    Q_D(GestureRecognizer);
    d->m_minVelocity = v;
}

int GestureRecognizer::panningThreshold() const
{
    Q_D(const GestureRecognizer);
    return d->m_panningThreshold;
}

void GestureRecognizer::setPanningThreshold(int threshold)
{
    Q_D(GestureRecognizer);
    d->m_panningThreshold = threshold;
}

qreal GestureRecognizer::fastVelocityFactor() const
{
    Q_D(const GestureRecognizer);
    return d->m_fastVelocityFactor;
}

void GestureRecognizer::setFastVelocityFactor(qreal v)
{
    Q_D(GestureRecognizer);
    d->m_fastVelocityFactor = v;
}

int GestureRecognizer::scrollsPerSecond() const
{
    Q_D(const GestureRecognizer);
    return d->m_fastVelocityFactor;
}

void GestureRecognizer::setScrollsPerSecond(int sps)
{
    Q_D(GestureRecognizer);
    d->m_scrollsPerSecond = sps;
}

//
//GestureRecognizerPrivate DIFINITION
//
GestureRecognizerPrivate::GestureRecognizerPrivate(GestureListener* gestureListener)
    : m_gestureListener(gestureListener)
    , m_state(GestureRecognizerPrivate::Inactive)
    , m_dragInertia(0.85)
    , m_directionErrorMargin(10)
    , m_axisLockThreshold(0)
    , m_maxVelocity(3500)
    , m_minVelocity(10)
    , m_panningThreshold(25)
    , m_fastVelocityFactor(0.01)
    , m_scrollsPerSecond(20)
    , m_velocity(QPointF(0, 0))
    , m_position(QPointF(-1, -1))
    , m_initialPos(QPointF(-1, -1))

{ }

GestureRecognizerPrivate::~GestureRecognizerPrivate()
{ }

bool GestureRecognizerPrivate::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return false;

    if (m_state == GestureRecognizerPrivate::Inactive) {
        //First mouse press.
        m_position = event->scenePos();
        m_initialPos = m_position;

        changeState(GestureRecognizerPrivate::Press);

        GestureEvent gesture = gestureEvent(m_initialPos, GestureEvent::Touch);
        m_gestureListener->handleGesture(&gesture);

        m_lastTime.start();
        m_delayedPressMoment.start();
        m_timer.start(LongClickFilterDuration, this);
    } else if(m_state == GestureRecognizerPrivate::Release) {
        //This press is for double tap.
        changeState(GestureRecognizerPrivate::DoublePress);
    }
    event->setAccepted(true);
    return true;
}

bool GestureRecognizerPrivate::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
        return true;

    if (m_state == GestureRecognizerPrivate::Inactive) {
        //Release event cannot be generated in gesture recognizer is in active!
        event->setAccepted(true);
        resetTouchPositions();
        return true;
    }

    if (m_state == GestureRecognizerPrivate::Press) {
        if (m_delayedPressMoment.elapsed() > MinTimeHoldForClick) {
            //Waiting for MinTimeHoldForClick to make sure use has actually pressed.
            //Removes accidentall press
            changeState(GestureRecognizerPrivate::Release);
            m_timer.start(DoubleClickFilterDuration, this);
        } else
            resetTouchPositions();
    } else if (m_state == GestureRecognizerPrivate::Move) {
        if (qAbs(m_velocity.x()) > m_minVelocity
            || qAbs(m_velocity.y()) > m_minVelocity) {
            GestureEvent gesture = gestureEvent(m_position, GestureEvent::Flick);
            gesture.setVelocity(m_velocity);
            m_gestureListener->handleGesture(&gesture);
        }
        m_velocity = QPointF(0,0);
        resetTouchPositions();
    } else if (m_state == GestureRecognizerPrivate::DoublePress) {
        //Stop double tap timer
        m_timer.stop();
        GestureEvent gesture = gestureEvent(m_initialPos, GestureEvent::DoubleTap);
        m_gestureListener->handleGesture(&gesture);
        resetTouchPositions();
    }
    event->setAccepted(true);
    return true;
}

bool GestureRecognizerPrivate::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPoint delta;
    if (m_state == GestureRecognizerPrivate::Press) {
        delta = (event->scenePos() - m_initialPos).toPoint();
        QPoint absDelta;
        absDelta.setX(qAbs(delta.x()));
        absDelta.setY(qAbs(delta.y()));

        if ((absDelta.x() > ThresholdForMove) || (absDelta.y() > ThresholdForMove)) {
            //Stop long tap timer
            m_timer.stop();

            changeState(GestureRecognizerPrivate::Move);
            m_position = event->scenePos();
        } else {
            //Ignore until user has actually moved
            return true;
        }
    } else if (m_state == GestureRecognizerPrivate::Move) {
        delta = (event->scenePos() - m_position).toPoint();
        m_position = event->scenePos();
    } else {
        resetTouchPositions();
        return true;
    }

    //Aplly if axis stickiness is specified.
    if (m_axisLockThreshold) {
        int dx = qAbs(delta.x());
        int dy = qAbs(delta.y());

        if (dx || dy) {
            bool vertical = (dy > dx);
            qreal alpha = qreal(vertical ? dx : dy) / qreal(vertical ? dy : dx);
            if (alpha <= m_axisLockThreshold) {
                if (vertical)
                    delta.setX(0);
                else
                    delta.setY(0);
            }
        }
    }

    m_velocity = calculateVelocity(delta, m_lastTime.elapsed());

    //Send pan gesture
    GestureEvent gesture = gestureEvent(m_position, GestureEvent::Pan);
    gesture.setDelta(delta);
    m_gestureListener->handleGesture(&gesture);
    m_lastTime.restart();

    event->setAccepted(true);
    return true;
}

bool GestureRecognizerPrivate::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (m_state == GestureRecognizerPrivate::Release) {
        m_timer.stop();
        GestureEvent gesture = gestureEvent(m_initialPos, GestureEvent::DoubleTap);
        m_gestureListener->handleGesture(&gesture);
    }
    resetTouchPositions();
    event->setAccepted(true);
    return true;
}

void GestureRecognizerPrivate::changeState(GestureRecognizerPrivate::State state)
{
    m_state = state;
}

GestureEvent GestureRecognizerPrivate::gestureEvent(const QPointF& pos, GestureEvent::Type type)
{
    GestureEvent gestureEvent;
    gestureEvent.setType(type);
    gestureEvent.setPosition(pos);
    return gestureEvent;
}

void GestureRecognizerPrivate::timerEvent(QTimerEvent* event)
{
    if (event->timerId() != m_timer.timerId())
        return;

    m_timer.stop();

    QPointF position;
    GestureEvent::Type gestureType;

    if (m_state == GestureRecognizerPrivate::Press) {
        //Long press
        position = m_position;
        gestureType = GestureEvent::LongTap;
    } else if (m_state == GestureRecognizerPrivate::Release) {
        //Actual release event
        position = m_initialPos;
        gestureType = GestureEvent::Release;
    } else {
        resetTouchPositions();
        changeState(GestureRecognizerPrivate::Inactive);
        return;
    }

    resetTouchPositions();
    GestureEvent gesture = gestureEvent(position, gestureType);
    m_gestureListener->handleGesture(&gesture);
    return;
}

void GestureRecognizerPrivate::resetTouchPositions()
{
    m_position = QPointF(-1, -1);
    m_initialPos = m_position;
    changeState(GestureRecognizerPrivate::Inactive);
}

QPointF GestureRecognizerPrivate::calculateVelocity(const QPointF& delta, int time)
{
    QPointF newVelocity = m_velocity;

    if ((delta / qreal(time)).manhattanLength() < 25) {
        QPointF rawVelocity = delta / qreal(time) * qreal(1000) / qreal(m_scrollsPerSecond);
        newVelocity = newVelocity * (qreal(1) - m_dragInertia) + rawVelocity * m_dragInertia;
    }

    newVelocity.setX(delta.x() ? qBound(-m_maxVelocity, newVelocity.x(), m_maxVelocity) : m_velocity.x());
    newVelocity.setY(delta.y() ? qBound(-m_maxVelocity, newVelocity.y(), m_maxVelocity) : m_velocity.y());
    return newVelocity;
}

} // namespace GVA

