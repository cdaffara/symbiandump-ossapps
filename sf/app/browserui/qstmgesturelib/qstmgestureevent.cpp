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

#include <QWidget>
#include <QApplication>
#include "qstmgestureevent.h"
#include "qstmuievent_if.h"

using namespace qstmUiEventEngine;

Qt::GestureType QStm_Gesture::s_assignedType = Qt::CustomGesture;

QStm_GestureEvent::QStm_GestureEvent():
                   QEvent(QStm_GestureEvent::stmGestureEventType())

{
}

QStm_GestureEvent::~QStm_GestureEvent()
{
}





QStm_Gesture::QStm_Gesture(QObject* parent) : QGesture(parent)
{
	    m_speed = 0.0;    
	    m_direction = -1;
	    m_vector = QPoint(0,0);
	    m_gstType = QStmUknownGestureType; 
	    m_state = Qt::NoGesture;
	    m_gstSubType = 0;
	    m_pos = QPoint(INT_MIN,INT_MIN);
	    m_details = NULL;
	    m_speedVec = QPointF(0.0, 0.0);
	    m_target = NULL;
};


QStm_Gesture& QStm_Gesture::operator=(QStm_Gesture& other)
{
    m_speed = other.getSpeed();    
    m_direction = other.getDirection();
    m_vector = other.getLengthAndDirection();
    m_gstType = other.getGestureStmType(); 
    m_state = other.gestureState();
    m_gstSubType = other.getGestureSubType();
    m_pos = other.position();
    m_details = other.getDetails();
    m_speedVec = other.getSpeedVec();
    m_target = other.m_target;
	return *this;
}


QString  QStm_Gesture::gestureName()
{
    switch (m_gstType) {
        case QStmUknownGestureType:
            return QString("QStmUknownGestureType");    
        case QStmTouchGestureType:
            return QString("QStmTouchGestureType");
        case QStmTapGestureType:
            return QString("QStmTapGestureType");
        case QStmDoubleTapGestureType:
            return QString("QStmDoubleTapGestureType");
        case QStmLongPressGestureType:
            return QString("QStmLongPressGestureType");
        case QStmHoverGestureType:
            return QString("QStmHoverGestureType");
        case QStmPanGestureType:
            return QString("QStmPanGestureType");
        case QStmReleaseGestureType:
            return QString("QStmReleaseGestureType");
        case QStmLeftRightGestureType:
            return QString("QStmLeftRightGestureType");
        case QStmUpDownGestureType:
            return QString("QString");
        case QStmFlickGestureType:
            return QString("QStmFlickGestureType");
        case QStmEdgeScrollGestureType:
            return QString("QStmEdgeScrollGestureType");
        case QStmPinchGestureType:
            return QString("QStmPinchGestureType");
        case QStmCornerZoomGestureType:
            return QString("QStmCornerZoomGestureType");
        default:
            return QString("XZ Gesture");
    }	
}

void QStm_Gesture::gestureTypeToMouseTypes(QVarLengthArray<int, 4>& types)
{
	switch (m_gstType) {
		case QStmTouchGestureType:
		{
			types.append(QEvent::MouseButtonPress);
			break;
		}
		case QStmDoubleTapGestureType:
		{
			types.append(QEvent::MouseButtonDblClick);
			break;
		}

		case QStmHoverGestureType:
		case QStmPanGestureType:
		case QStmLeftRightGestureType:
		case QStmUpDownGestureType:
		case QStmCornerZoomGestureType:
		{
			types.append(QEvent::MouseMove);
			break;
		}
		case QStmReleaseGestureType:
		{
			types.append(QEvent::MouseButtonRelease);
			break;
		}
		case QStmTapGestureType:
		{
			types.append(QEvent::MouseButtonPress);
			types.append(QEvent::MouseButtonRelease); 
			break;
		}
		case QStmUknownGestureType:
		{
		    QStm_UiEventIf* uiEvent = static_cast<QStm_UiEventIf*>(m_details);
		    if (uiEvent) {
		        QEvent::Type evType = uiEvent->mapToMouseEventType();
		        types.append(evType);
		    }
		    break;
		}
		case QStmFlickGestureType:
		case QStmEdgeScrollGestureType:
		case QStmPinchGestureType:
		case QStmLongPressGestureType:
		default:
			break;
	}
	return;
}

bool QStm_Gesture::sendOrPostMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier, bool send)
{
    bool ret = false;
    QPoint pos = position();
    QPoint gpos = pos;
    QWidget* w = NULL;
    Qt::MouseButtons buttons = Qt::LeftButton;
    
    
    buttons &= ~Qt::RightButton;
    buttons &= ~Qt::MidButton; 
    buttons &= Qt::MouseButtonMask;
    
    if (receiver->isWidgetType()) {
    	QWidget* w = static_cast<QWidget*>(receiver);
    	pos = w->mapFromGlobal(gpos);
    }    
    
    QVarLengthArray<int, 4> mouseTypes;
    gestureTypeToMouseTypes(mouseTypes);   
    
    for (int i = 0; i < mouseTypes.size(); i++) {
    	QEvent::Type mtype = static_cast<QEvent::Type>(mouseTypes[i]);
    	
    	if (mtype == QEvent::MouseButtonRelease) {
    		buttons = 0;
    	}
    	
    	if (send) {
    	    QMouseEvent evt(mtype, pos, gpos, Qt::LeftButton, buttons, modifier);
    	    if (w) w->grabMouse();
    	    QApplication::sendEvent(receiver, &evt);
    	    if (w) w->releaseMouse();
    	}
    	else {
    		QMouseEvent* evt = new QMouseEvent(mtype, pos, gpos, Qt::LeftButton, buttons, modifier);
    		QApplication::postEvent(receiver, evt);
    	}
    }
    
    return ret;
}


bool QStm_Gesture::sendEvent(QEvent* event)
{
    QWidget* target = static_cast<QWidget*>(m_target);
    QWidget* w = NULL;
    
    if (target) {
        QPoint pos = target->mapFromGlobal(m_pos);
        w = target->childAt(pos);
        if (!w) {
            w = target;
        }
    }
    
    if (w) {
        if (event->type() == QStm_GestureEvent::stmGestureEventType() && 
            m_gstType == QStmUknownGestureType) {
            QStm_UiEventIf* uiEvent = static_cast<QStm_UiEventIf*>(m_details);
            QWidget* modal = QApplication::activeModalWidget();
            if (uiEvent && m_target == modal) {
            //re-generate mouse events
                sendMouseEvents(w);
            }
        }        
        else {
            QApplication::sendEvent(w, event);
        }
    }
}


bool QStm_Gesture::sendMouseEvents(Qt::KeyboardModifier modifier)
{
    Q_ASSERT(m_target);
    QWidget* target = static_cast<QWidget*>(m_target);
    QPoint pos = target->mapFromGlobal(m_pos);
    QWidget* w = target->childAt(pos);
    if (!w) {
        w = target;
    }
    
    return sendMouseEvents(w);
}

bool QStm_Gesture::sendMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier)
{
	return sendOrPostMouseEvents(receiver, modifier, true);
}

bool QStm_Gesture::postMouseEvents(QObject* receiver, Qt::KeyboardModifier modifier)
{
	return sendOrPostMouseEvents(receiver, modifier, false);
}

QList<QMouseEvent*>   QStm_Gesture::getMouseEvents()
{
	QList<QMouseEvent*> mouseEventsList;
    QPoint pos = position(); 
    QPoint gpos = pos;
    Qt::MouseButtons buttons = Qt::LeftButton;        
    buttons &= ~Qt::RightButton;
    buttons &= ~Qt::MidButton; 
    buttons &= Qt::MouseButtonMask;
        
    
    
    QVarLengthArray<int, 4> mouseTypes;
    gestureTypeToMouseTypes(mouseTypes);
    
    for (int i = 0; i < mouseTypes.size(); i++) {
        QMouseEvent* evt = new QMouseEvent(static_cast<QEvent::Type>(mouseTypes[i]), 
        		               pos, gpos, Qt::LeftButton, buttons, Qt::NoModifier);
        mouseEventsList.append(evt);
    }
	return mouseEventsList;
}


QSTMGESTURELIB_EXPORT QStm_Gesture*  getQStmGesture(QEvent* event)
{	
	QStm_Gesture* gesture = NULL;
	if (event->type() == QEvent::Gesture) {
	    QGestureEvent* ge = static_cast<QGestureEvent*>(event);
	    QList<QGesture*> l = ge->gestures();
	    QGesture* g = !l.isEmpty() ? l.at(0) : NULL;
	    if (g && g->gestureType() == QStm_Gesture::assignedType()) {
	        gesture = static_cast<QStm_Gesture*>(g);
	    }
	}
	return gesture;
}

