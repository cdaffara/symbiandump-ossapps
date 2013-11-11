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

#ifndef QSTMSTATEENGINE_H_
#define QSTMSTATEENGINE_H_

#include "qstmuievent_if.h"
#include "qstmtimerinterface.h"
#include "qstmstatemachine_v2.h"
#include "qstmstateengineconfig.h"
#include "qstmuievent.h"

#include <QtCore>


namespace qstmUiEventEngine
{

class QStm_UiEvent;
/*!
 * Define THwEvent since we need the point, the type and timestamp to be stored.
 * The HW event is used also to handle the timer events so that the wrapper class calling the
 * state machine will handle the starting, canceling etc. of the timers and the state machine
 * sees the timers as messages.  This hopefully makes it easier to keep the core state machine as
 * OS agnostic as possible.
 */
class QStm_HwEvent
{
public:
    QStm_HwEvent() {} ;
    QStm_HwEvent(const QStm_HwEvent& ev):
        m_type(ev.m_type), m_position(ev.m_position),
        m_time(ev.m_time), m_target(ev.m_target), m_pointerNumber(ev.m_pointerNumber) {}
    QStm_HwEvent(QStm_StateMachineEvent code, const QPoint& pos, QTime time, void* target, int pointerNr):
        m_type(code), m_position(pos),
        m_time(time), m_target(target), m_pointerNumber(pointerNr) {}

    QStm_StateMachineEvent m_type ;
    QPoint  m_position ;
    QTime   m_time ;
    void*   m_target ;
	int	    m_pointerNumber ;
};


class QStm_StateEngine : public QObject
{
public:
    /**
     * constructor and destructor
     */
	QStm_StateEngine(QStm_StateEngineConfiguration* config, QStm_TimerInterfaceIf* timerif, int index) ;

    ~QStm_StateEngine() ;
    /**
     *  The methods needed for state machine
     */
    void consumeEvent() ;
    bool isTouchTimer() ;
    bool isHoldTimer() ;
    bool isSuppressTimer() ;
    bool isTouchTimeArea() ;
    bool isTouchArea() ;
    bool isHoldArea() ;
    bool insideTouchTimeArea() ;
    bool insideTouchArea() ;
    bool insideHoldArea() ;
    bool looksLikeHold() ;
    void errorEvent() ;
    void initTouchTimer() ;
    void initHoldTimer() ;
    void restartHoldTimer() ;
    void initTouchSuppressTimer() ;
    void initMoveSuppressTimer() ;
    void clearTouchTimer() ;
    void clearHoldTimer() ;
    void clearSuppressTimer() ;
    void produceTouch() ;
    void produceMove() ;
    void produceRelease() ;
    void produceHold() ;
    void renameToDrag() ;
    void prepareTouchTimeArea() ;
    void prepareTouchArea() ;
    void prepareHoldArea() ;
    void setCurrentPos() ;
    void setGestureStart() ;
    void addToTouch() ;
    void addDraggingPos() ;
    void storeMovePos() ;
    bool insideMoveThreshold();

    /**
     * The event interface
     */

    QStm_HwEvent& initEvent()
    {
        m_hwe.m_pointerNumber = m_index;
        return m_hwe;
    }

    // event returned by initEvent() must initialized first
	bool handleStateEvent()  ;

	QRect getTouchArea() ;
	QRect getHoldArea() ;
	bool  wasLastMessageFiltered() ;

private:
    bool m_eventConsumed ;
    QStm_HwEvent m_hwe ;

    bool isNewHoldingPoint() ;
    QList<QStm_HwEvent*>  m_dragPoints;        // LATER: change this into std::vector
    QStm_TimerInterfaceIf*  m_timerif ;                //

    QList<QStm_HwEvent>   m_touchPoints;      // LATER: change this into std::vector

    // Variables for running the state machine
    QStm_StateMachineState m_currentState ;
    bool m_isTimerEvent ;                   // We need to separate the pointer events and the timer events
    QPoint m_currentTouchXY ;               // This is the touching point
    QPoint m_gestureStartXY ;               // gesture started at this point
    QPoint m_uiEventXY ;                    // The UI event XY point.
    QPoint m_previousPointerEventPosition ;
    QPoint m_deltaVector ;
    QRect m_touchRect ;
    QRect m_holdRect ;

    bool m_wasFiltered ;
    void calculateDelta() ;
    void calculateTouchAverage() ;
    void calculateCurrentVector() ;
    QPoint calculateTouchAverageFromPoints() ;
    bool insideArea(const QPoint& point, const QRect& rect, QStm_AreaShape shape, const QPoint& tolerance);
    void setTolerance(long fingersize_mm, QPoint& tolerance, QStm_AreaShape shape) ;

    void debugPrintState(QStm_StateMachineState nextstate) ;
    inline static QRect toleranceRect( const QPoint& aCenterPoint, const QPoint& tolerance) ;
    void turnStateMachine() ;

    QPoint     m_touchCentre ;
    QPoint     m_holdCentre ;

    void*       m_gestureTarget ;
    long        getInterval() ;
    QTime       m_lastMessageTime ;
    QStm_UiEvent* createUIEvent(QStm_UiEventCode code, const QPoint& aPos) ;
    QPoint getPreviousXY(const QPoint& aXY) ;
    QPoint m_previousXY ;
    bool isTimerMessage() ;
    int  m_index ;
    // TPoint screenCoordinates(TPoint aPos, void* aGestureTarget) ;
    QStm_UiEventCode m_previousUiGenerated ;
    QPoint m_lastFilteredPosition ;
    QTime m_lastFilteredMessageTime ;

	QStm_StateEngineConfiguration* m_config ;

};

class QStm_UiEventSender: public QObject
{
public:
    enum EventSenderState
        {
        ENoEvents,
        EEventsReady,
        EBusy
        };

    QStm_UiEventSender();
    ~QStm_UiEventSender();

    int addEvent(QStm_UiEvent* gestureEvent);
    bool addObserver(QStm_UiEventObserverIf* observer );
    bool removeObserver(QStm_UiEventObserverIf* observer );
    bool isObserver(void* observer );
    void setLogging(bool a) {m_loggingenabled = a;} ;

private:
    void emitEvent( const QStm_UiEvent& gestureEvent );

    /*!
     * Should we be prepared for more than 5 pointers ?  Jos tulee Torvisen voittaja
     */
    QStm_UiEvent* m_events[qstmUiEventEngine::KMaxNumberOfPointers];
    void compressStack(QStm_UiEvent* uiEvent) ;

    /*
     * Let's be prepared for more than one observer,
     * although usually there is only the gesture engine
     */
    QList<QStm_UiEventObserverIf*> m_observers;
    bool m_loggingenabled ;

};
}   // namespace

#endif /* QSTMSTATEENGINE_H_ */
