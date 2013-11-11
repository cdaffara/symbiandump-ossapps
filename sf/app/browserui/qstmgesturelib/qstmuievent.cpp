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
#include <QtCore>
#include "qstmfilelogger.h"
#include "qstmstatemachine.h" // for qstmUiEventEngine::Distance(dx,dy)
#include "qstmuievent.h"
#include "qstmutils.h"

using namespace qstmUiEventEngine ;

static const char* const eventNames[] = {  // for debugging purposes define the names of the UI events
        "noEvent",
        "ETouch",
        "EHold",
        "EMove",
        "ERelease",
        "ENull"
} ;

const char* qstmUiEventEngine::event_name(qstmUiEventEngine::QStm_UiEventCode code)
{
    return eventNames[code];
}

QStm_UiEvent::QStm_UiEvent(
    QStm_UiEventCode code,
    const QPoint& start, const QPoint& xy, const QPoint& previousXY,
    bool timerExpired, void* target, long interval,
    int index, QTime timestamp) :
    m_code(code), m_start(start), m_XY(xy), m_previousXY(previousXY),
    m_statetransition(interval), m_target(target), m_timerExpired(timerExpired),
    m_index(index), m_previousEvent(NULL), m_timestamp(timestamp)
{
}



QStm_UiEvent::~QStm_UiEvent()
{
    delete m_previousEvent ;
    m_previousEvent = NULL ;
}

const QPoint& QStm_UiEvent::startPos() const
{
    return m_start ;
}

const QPoint& QStm_UiEvent::currentXY() const
{
    return m_XY ;
}

QStm_UiEventCode QStm_UiEvent::code()const
{
    return m_code ;
}

const QPoint& QStm_UiEvent::previousXY() const
{
    return m_previousXY ;
}

long QStm_UiEvent::stateTransition() const
{
    return m_statetransition ;
}

bool QStm_UiEvent::timerExpired() const
{
    return m_timerExpired ;
}

void* QStm_UiEvent::target() const
{
    return m_target ;
}

int QStm_UiEvent::index() const
{
    return m_index ;
}

void QStm_UiEvent::setPrevious(QStm_UiEvent* event)
{
    m_previousEvent = event ;
}

QStm_UiEventIf* QStm_UiEvent::previousEvent() const
{
    return m_previousEvent ;
}

int QStm_UiEvent::countOfEvents() const
{
    int count = 1 ;
    for(QStm_UiEvent* prev = m_previousEvent; prev; prev = prev->m_previousEvent)
        {
        ++count;
        }
    return count ;
}

QTime QStm_UiEvent::timestamp() const
{
    return m_timestamp ;
}

float QStm_UiEvent::speed() const /*__SOFTFP */
{
    int dist = QStm_Utils::distance(m_previousXY, m_XY);
    // now calculate speed
    int elapsed = m_statetransition ; // use ms as time unit
    float speed = float(dist)/(elapsed ? elapsed : 1) ;
    return speed ;
}

QPointF QStm_UiEvent::speedVec() const
{
    QPointF diff(m_XY - m_previousXY);
    
    diff = diff / (m_statetransition ? m_statetransition : 1) ;
    
    return diff;
}


void QStm_UiEvent::logSpeed() const
{
    int dist = QStm_Utils::distance(m_previousXY, m_XY);
    // now calculate speed
    int elapsed = m_statetransition ; // use ms as time unit
    double speed = double(dist)/(elapsed ? elapsed : 1) ;

    LOGARG("event: %s: (%d,%d), prev: (%d,%d), time %d dist %d speed %f", event_name(code()), m_XY.x(), m_XY.y(),
            m_previousXY.x(), m_previousXY.y(), elapsed, dist, speed);
}



QEvent::Type  QStm_UiEvent::mapToMouseEventType()
{
    QEvent::Type type = QEvent::None;
    switch (m_code) {
        case ETouch:
        case EHold:    
        {
            type = QEvent::MouseButtonPress;
            break;
        }
        case EMove:
        {
            type = QEvent::MouseMove;
            break;
        }
        case ERelease:
        {
            type = QEvent::MouseButtonRelease;
            break;
        }
    }
    return type;
}

QEvent::Type  QStm_UiEvent::mapToTouchEventType()
{
    QEvent::Type type = QEvent::None;
    switch (m_code) {
        case ETouch:
        case EHold:    
        {
            type = QEvent::TouchBegin;
            break;
        }
        case EMove:
        {
            type = QEvent::TouchUpdate;
            break;
        }
        case ERelease:
        {
            type = QEvent::TouchEnd;
            break;
        }
    }
    return type;
}

