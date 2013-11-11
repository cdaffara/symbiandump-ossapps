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
#ifndef QSTMGESTUREEVENT_H_
#define QSTMGESTUREEVENT_H_

#include <QEvent>
#include <QPoint>
#include <QString>
#include <QVarLengthArray>
#include <QMouseEvent>
#include <QGesture>
#include <QWidget>

#include "qstmgesturedefs.h"
#include "qstmgesture_if.h"

#define QStmGestureEventType QEvent::User + 1 
#define QStmGestureType Qt::CustomGesture


enum QStm_GestureType
	{
	    QStmUknownGestureType = 0,
	    QStmTouchGestureType,
	    QStmTapGestureType,
	    QStmDoubleTapGestureType,
	    QStmLongPressGestureType,
	    QStmHoverGestureType,
	    QStmPanGestureType,
	    QStmReleaseGestureType,
	    
	    QStmLeftRightGestureType,
	    QStmUpDownGestureType,
	    QStmFlickGestureType,     
	    QStmEdgeScrollGestureType,
	    
	    QStmPinchGestureType, 
	    QStmCornerZoomGestureType
	};



class QSTMGESTURELIB_EXPORT QStm_Gesture : public QGesture
{
	Q_OBJECT
	
public:	

	QStm_Gesture(QObject* parent = 0);
    
    virtual ~QStm_Gesture() {}
    
    QStm_Gesture& operator=(QStm_Gesture& other);
    QPoint  position() const { return m_pos; }
    void setPosition(const QPoint &pos) { m_pos = pos; /*setHotSpot(QPointF(pos)); */}
    
    void setGestureStmType(QStm_GestureType type) { m_gstType = type; }    
    QStm_GestureType  getGestureStmType() const { return m_gstType; }
    
    void setGestureState(Qt::GestureState state) { m_state = state; }
    Qt::GestureState gestureState() { return m_state; }
    
    void setGestureSubType(int type) { m_gstSubType = type; }
    int  getGestureSubType() { return m_gstSubType; }    
    
    qreal getSpeed() { return m_speed; }
    void setSpeed(qreal speed) { m_speed = speed; }
    
    QPointF getSpeedVec() { return m_speedVec; }
    void setSpeedVec(QPointF speedVec) { m_speedVec = speedVec; }
    
    QPoint getLengthAndDirection() { return m_vector; }
    void setLengthAndDirection(QPoint vec) { m_vector = vec; }
    
    int getDirection() { return m_direction; }
    void setDirection(int dir) { m_direction = dir; }
    QString  gestureName();
    
    void* getDetails() { return m_details; }
    void  setDetails(void* details ) { m_details = details; }
    
    bool sendEvent(QEvent* event);
    QList<QMouseEvent*>   getMouseEvents();
    bool sendMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier = Qt::NoModifier);
    bool sendMouseEvents(Qt::KeyboardModifier modifier = Qt::NoModifier);
    bool postMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier = Qt::NoModifier);

    void setTarget(QWidget* target) { m_target = target; }
    QWidget* target() { return m_target; }
    
    static void setAssignedGestureType(Qt::GestureType type) { s_assignedType = type; }
    static Qt::GestureType assignedType() { return s_assignedType; } 
    
    

private:
    bool sendOrPostMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier, bool send);
    void gestureTypeToMouseTypes(QVarLengthArray<int, 4>& types);     
    
    
    qreal             m_speed;    
    int               m_direction;
    QPoint            m_vector;
    QStm_GestureType  m_gstType;
    Qt::GestureState  m_state;
    int               m_gstSubType;
    QPoint            m_pos;
    void*             m_details;
    QPointF           m_speedVec;
    static Qt::GestureType   s_assignedType;
    QWidget*          m_target;
};


QSTMGESTURELIB_EXPORT QStm_Gesture* getQStmGesture(QEvent* event);



class QSTMGESTURELIB_EXPORT QStm_GestureEvent : public QEvent
{
public:	
	QStm_GestureEvent();
	virtual ~QStm_GestureEvent();
	static QEvent::Type stmGestureEventType() { return static_cast<QEvent::Type>(QStmGestureEventType); }
};


#endif /* QSTMGESTUREEVENT_H_ */
