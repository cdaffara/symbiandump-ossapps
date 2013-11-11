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

#include "KineticScroller.h"

#include <QTimerEvent>

namespace GVA {

KineticScroller::KineticScroller(KineticScrollable* scrollableView)
    : m_scrollableView(scrollableView)
    , m_mode(KineticScroller::AutoMode)
    , m_state(KineticScrollable::Inactive)
    , m_overshootPolicy(KineticScroller::OvershootWhenScrollable)
    , m_motion(QPoint(0,0))
    , m_move(false)
    , m_bounceSteps(3)
    , m_maxOvershoot(150, 150)
    , m_vmaxOvershoot(130)
    , m_overshootDist(0, 0)
    , m_overshooting(0)
    , m_minVelocity(10)
    , m_maxVelocity(3500)
    , m_deceleration(0.85)
    , m_scrollsPerSecond(20)
    , m_idleTimerId(0)
    , m_scrollTimerId(0)
    , m_inactivityTimerId(0)
{}

KineticScroller::~KineticScroller()
{}

KineticScroller::OvershootPolicy KineticScroller::overshootPolicy() const
{
    return m_overshootPolicy;
}

void KineticScroller::setOvershootPolicy(KineticScroller::OvershootPolicy policy)
{
    m_overshootPolicy  = policy;
}

qreal KineticScroller::decelerationFactor() const
{
    return m_deceleration;
}

void KineticScroller::setDecelerationFactor(qreal f)
{
    m_deceleration = f;
}

int KineticScroller::scrollsPerSecond() const
{
    return m_scrollsPerSecond;
}

void KineticScroller::setScrollsPerSecond(int sps)
{
    m_scrollsPerSecond = qBound(1, sps, 100);
}

void KineticScroller::doPan(QPoint delta)
{
    //stop inactivity timer 
    if (m_inactivityTimerId) {
        killTimer(m_inactivityTimerId);
        m_inactivityTimerId = 0;
    }

    //overshoot only if OvershootAlwaysOn is set
    QPoint maxPos = m_scrollableView->maximumScrollPosition();
    bool alwaysOvershoot = (overshootPolicy() == KineticScroller::OvershootAlwaysOn);

    if (!maxPos.x() && !alwaysOvershoot) {
        delta.setX(0);
    }
    if (!maxPos.y() && !alwaysOvershoot) {
        delta.setY(0);
    }

    if (m_scrollTimerId)
        m_motion += delta;
    else {
        m_mode = KineticScroller::PushMode;
        changeState(KineticScrollable::Pushing);
        // we do not delay the first event but the next ones
        setScrollPositionHelper(m_scrollableView->scrollPosition() - m_overshootDist - delta);
        m_motion = QPoint(0, 0);
        m_scrollTimerId = startTimer(1000 / KineticScroller::MotionEventsPerSecond);
    }
}

void KineticScroller::doFlick(QPointF velocity)
{
    //stop inactivity timer 
    if (m_inactivityTimerId) {
        killTimer(m_inactivityTimerId);
        m_inactivityTimerId = 0;
    }

    m_motion = QPoint(0,0);
    m_mode = KineticScroller::AutoMode;
    changeState(KineticScrollable::AutoScrolling);
    m_velocity = velocity;
    m_idleTimerId = startTimer(1000 / m_scrollsPerSecond);
}

void KineticScroller::stop()
{
    if (m_inactivityTimerId) {
        killTimer(m_inactivityTimerId);
        m_inactivityTimerId = 0;
    }

    if (m_scrollTimerId) {
        killTimer(m_scrollTimerId);
        m_scrollTimerId = 0;
        setScrollPositionHelper(m_scrollableView->scrollPosition() - m_overshootDist - m_motion);
    }

    if (m_idleTimerId) {
        killTimer(m_idleTimerId);
        m_idleTimerId = 0;
    }
    
    if (m_overshootDist.x()) {
        m_overshooting = m_bounceSteps; 
        m_velocity.setX(-m_overshootDist.x() * qreal(0.8));
    }
    if (m_overshootDist.y()) {
        m_overshooting = m_bounceSteps; 
        m_velocity.setY(-m_overshootDist.y() * qreal(0.8));
    }

    //if page is overshoot doFlick() to switch back
    if (!m_overshootDist.isNull()) {    
        doFlick(m_velocity);
        return;
    }
    
    m_motion = QPoint(0, 0);
    m_velocity = QPointF(0,0);
    m_mode = KineticScroller::AutoMode;
    changeState(KineticScrollable::Inactive);
}

void KineticScroller::changeState(KineticScrollable::State newState)
{
    if (newState != m_state) {
        KineticScrollable::State oldState = m_state;
        m_state = newState;
        m_scrollableView->stateChanged(oldState, newState);
    }
}

void KineticScroller::reset()
{
    changeState(KineticScrollable::Inactive);

    if (m_idleTimerId)
        killTimer(m_idleTimerId);

    m_idleTimerId = 0;

    if (m_scrollTimerId)
        killTimer(m_scrollTimerId);

    m_scrollTimerId = 0;

    if (m_inactivityTimerId)
        killTimer(m_inactivityTimerId);
    m_inactivityTimerId = 0;

    m_velocity = QPointF(0, 0);
    m_overshootDist = QPoint(0, 0);
    m_overshooting = 0;
}

void KineticScroller::setScrollPositionHelper(const QPoint &pos)
{
    QPoint maxPos = m_scrollableView->maximumScrollPosition();

    QPoint clampedPos;
    clampedPos.setX(qBound(0, pos.x(), maxPos.x()));
    clampedPos.setY(qBound(0, pos.y(), maxPos.y()));

    bool alwaysOvershoot = (m_overshootPolicy == KineticScroller::OvershootAlwaysOn);
    int overshootX = (maxPos.x() || alwaysOvershoot) ? clampedPos.x() - pos.x() : 0;
    int overshootY = (maxPos.y() || alwaysOvershoot) ? clampedPos.y() - pos.y() : 0;

    m_overshootDist.setX(qBound(-m_maxOvershoot.x(), overshootX, m_maxOvershoot.x()));
    m_overshootDist.setY(qBound(-m_maxOvershoot.y(), overshootY, m_maxOvershoot.y()));

    m_scrollableView->setScrollPosition(clampedPos
                                        , m_overshootPolicy == KineticScroller::OvershootAlwaysOff
                                                               ? QPoint()
                                                               : m_overshootDist);
}

void KineticScroller::handleScrollTimer()
{
    if (!m_motion.isNull())
        setScrollPositionHelper(m_scrollableView->scrollPosition() - m_overshootDist - m_motion);

    killTimer(m_scrollTimerId);
    m_scrollTimerId = 0;

    m_inactivityTimerId = startTimer(300);
}

void KineticScroller::handleIdleTimer()
{
    if (m_mode == KineticScroller::PushMode && m_overshootDist.isNull()) {
        stop();
        return;
    }
 
    setScrollPositionHelper(m_scrollableView->scrollPosition() - m_overshootDist - m_velocity.toPoint());

    if (!m_overshootDist.isNull()) {
        m_overshooting++;
       
        /* When the overshoot has started we continue for
         * PROP_BOUNCE_STEPS more steps into the overshoot before we
         * reverse direction. The deceleration factor is calculated
         * based on the percentage distance from the first item with
         * each iteration, therefore always returning us to the
         * top/bottom most element
         */
        if (m_overshooting < m_bounceSteps) {
            m_velocity.setX( m_overshootDist.x() / m_maxOvershoot.x() * m_velocity.x() );
            m_velocity.setY( m_overshootDist.y() / m_maxOvershoot.y() * m_velocity.y() );
        } else {
            m_velocity.setX( -m_overshootDist.x() * 0.8 );
            m_velocity.setY( -m_overshootDist.y() * 0.8 );

            // ensure a minimum speed when scrolling back or else we might never return
            if (m_velocity.x() > -1.0 && m_velocity.x() < 0.0)
                m_velocity.setX(-1.0);
            if (m_velocity.x() <  1.0 && m_velocity.x() > 0.0)
                m_velocity.setX( 1.0);
            if (m_velocity.y() > -1.0 && m_velocity.y() < 0.0)
                m_velocity.setY(-1.0);
            if (m_velocity.y() <  1.0 && m_velocity.y() > 0.0)
                m_velocity.setY( 1.0);
        }

        m_velocity.setX( qBound((qreal)-m_vmaxOvershoot, m_velocity.x(), (qreal)m_vmaxOvershoot));
        m_velocity.setY( qBound((qreal)-m_vmaxOvershoot, m_velocity.y(), (qreal)m_vmaxOvershoot));

    } else if (m_state == KineticScrollable::AutoScrolling) {
        // Decelerate gradually when pointer is raised
        m_overshooting = 0;

        if (qAbs(m_velocity.x()) < qreal(0.8) * m_maxVelocity)
            m_velocity.rx() *= m_deceleration;
        if (qAbs(m_velocity.y()) < qreal(0.8) * m_maxVelocity)
            m_velocity.ry() *= m_deceleration;

        if ((qAbs(m_velocity.x()) < qreal(1.0)) && (qAbs(m_velocity.y()) < qreal(1.0))) 
            stop();
      
    } else if (m_mode == KineticScroller::AutoMode) 
          stop();
}

void KineticScroller::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == m_idleTimerId)
        handleIdleTimer();
    else if (ev->timerId() == m_scrollTimerId)
        handleScrollTimer();
    else if (ev->timerId() == m_inactivityTimerId) {
        //Do not move to inactive state if page is still moving  
        if (!m_idleTimerId && !m_scrollTimerId)
            stop();
    }
}

}//namespace GVA
