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

#ifndef QSTMUIEVENT_H_
#define QSTMUIEVENT_H_

#include "qstmuievent_if.h"


namespace qstmUiEventEngine
{

class QStm_UiEvent : public QStm_UiEventIf
{
public:
    virtual const QPoint& startPos() const ;
    virtual const QPoint& currentXY() const ; // current yx-coordinate
    virtual const QPoint& previousXY() const ;    // Past yx coordinate
    virtual long stateTransition() const ;    // Time taken for state transition
    virtual bool timerExpired() const;   // if timer expired
    virtual QStm_UiEventCode code()const ;
    virtual void* target() const ;
    virtual int index() const ;
    QStm_UiEventIf* previousEvent() const ;
    virtual int countOfEvents() const  ;
    virtual QTime timestamp() const ;
    virtual float speed() const /* __SOFTFP */; // pixels / millisecond
    virtual void setSpeed(float speed) { m_speed = speed; }		
    
    virtual QPointF speedVec() const;
    virtual void  setSpeedVec(QPointF speedVec) { m_speedVec = speedVec; }
    virtual QEvent::Type  mapToMouseEventType();
    virtual QEvent::Type  mapToTouchEventType();

    
    // for testing...
    virtual void logSpeed() const ;

    QStm_UiEvent(
        QStm_UiEventCode code,
        const QPoint& start, const QPoint& xy, const QPoint& previousXY,
        bool timerExpired, void* target, long interval,
        int index, QTime timestamp) ;

    ~QStm_UiEvent() ;
    /*!
     * Chain the UI events ; the whole chain is deleted after release has been handled
     * in UI sender
     */
    virtual void setPrevious(QStm_UiEvent* aEvent) ;
private:

    QStm_UiEventCode m_code ;
    QPoint m_start ;
    QPoint m_XY ;
    QPoint m_previousXY ;
    int    m_statetransition ;
    void*  m_target ;
    bool   m_timerExpired ;
    int    m_index ;
    QStm_UiEvent* m_previousEvent ;
    QTime  m_timestamp ;
};
}


#endif /* QSTMUIEVENT_H_ */
