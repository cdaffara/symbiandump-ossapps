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

#ifndef KineticScroller_h
#define KineticScroller_h

#include "KineticScrollable.h"

#include <QObject>

namespace GVA {

class KineticScroller : public QObject {
    Q_OBJECT

public :
    enum OvershootPolicy {
        OvershootWhenScrollable,
        OvershootAlwaysOff,
        OvershootAlwaysOn,
    };

    enum Mode {
        AutoMode,
        PushMode
    };

    enum {
        MotionEventsPerSecond =  25
    };

    KineticScroller(KineticScrollable* scrollableView);
    ~KineticScroller();

    OvershootPolicy overshootPolicy() const;
    void setOvershootPolicy(OvershootPolicy policy);

    qreal decelerationFactor() const;
    void setDecelerationFactor(qreal f);

    int scrollsPerSecond() const;
    void setScrollsPerSecond(int sps);

    void reset();
    void doPan(QPoint delta);
    void doFlick(QPointF velocity);
    void stop();

protected:
    void changeState(KineticScrollable::State newState);
    void setScrollPositionHelper(const QPoint& point);

    void handleScrollTimer();
    void handleIdleTimer();
    void timerEvent(QTimerEvent* ev);

private:
    KineticScrollable* m_scrollableView;

    Mode m_mode;
    KineticScrollable::State m_state;

    QPoint m_motion;
    bool m_move;

    // overshoot
    OvershootPolicy m_overshootPolicy;
    int m_bounceSteps;
    QPoint m_maxOvershoot;
    int m_vmaxOvershoot;
    QPoint m_overshootDist;
    int m_overshooting; // the overshooting time in idleTimer steps

    // velocity
    QPointF m_velocity;
    qreal m_minVelocity;
    qreal m_maxVelocity;
    qreal m_deceleration;
    int m_scrollsPerSecond;

    // timer
    int m_idleTimerId;
    int m_scrollTimerId;
    int m_inactivityTimerId;
};//KineticScroller

}//namespace GVA

#endif //KineticScroller_h
