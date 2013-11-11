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

#include "qstmstateengine.h"
#include "qstmutils.h"
#include "qstmuievent.h"

#include "qstmfilelogger.h"

using namespace qstmUiEventEngine ;

/*!
  State definitions glue together the methods of the qstmstateengine.cpp
  so that it will behave as defined in the state machine specification.
  First define the separate elements for each possible event and then tie them together
  to create one state entry.  The state entries then are put to array
  where the index is at the same time also the state ID.

  STATE_ELEMENT arrays define the methods called when handling a message.
  \sa STATE_ELEMENT.
  Note that the last row of STATE_ELEMENT array must NOT have a ConditionFunction entry
  and it must have a NextState entry != Eignore.  Otherwise the state machine will
  not behave correctly.

 */
/*! Add macro with token pasting to make creation of the state machine tables easier
   and removing the necessity to write the classname twice.
   Maybe there would be some better way to do this using templates and typedefs?
 */
#define CND(x) isF<QStm_StateEngine,&QStm_StateEngine::x>
#define ACT(x) aF<QStm_StateEngine,&QStm_StateEngine::x>

/*********************************************************************************
 * empty statedef as a dummy entry
 *  */
const STATE_ELEMENT __errorEvent[1] = {
        0,
        ACT(errorEvent),
        EInit
};

const STATE Ignore__[1] = {
        EDown,          __errorEvent
} ;

/*!
  :INIT state and its event specific elements
  See the spec in http://wikis.in.nokia.com/Runtimes/NewGestureLibrary
  Down is only valid event in :INIT state
  The event is consumed immediately, so that the state machine will process only these
  methods when processing the message.
  If touch timer has been set, the next state is InTouchTime.
  If no touch timer, but if hold timer has been defined, the next state is InHoldTime_U
  If no touch or hold timer have been defined, but touch area has been defined, next state is InTouchArea.
  11-May-2009: addition: add another touch area: one for touch time and one for touch area after touch time
  has elapsed.  This allows "sloppy" touch to be handled properly without extra move if touchtimearea is larger,
  but after touch has been detected a smaller movement is allowed.
 */
const STATE_ELEMENT Init__Down[12] = {
        0,                  ACT(consumeEvent),    Eignore,
        0,                  ACT(setGestureStart), Eignore,
        0,                  ACT(setCurrentPos),   Eignore,
        CND(isTouchTimer),  ACT(initTouchTimer),  Eignore,
        CND(isHoldTimer),   ACT(initHoldTimer),   Eignore,
        CND(isTouchTimeArea),   ACT(prepareTouchTimeArea),Eignore,
        CND(isHoldArea),    ACT(prepareHoldArea) ,Eignore,
        CND(isTouchTimer),  0,                    EInTouchTime,
        0,                  ACT(produceTouch),    Eignore,
        CND(isHoldTimer),   0,                    EInHoldTime_U,
        CND(isTouchArea),   ACT(prepareTouchArea),EInTouchArea,
        0,                  0,                    EDispatch             // If nothing else happens, goto to Dispatch state
};
/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the Init state
 */
const STATE_ELEMENT Init__errorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInit
};

/*!
 * :INIT
 * note that only valid event is DOWN, all the rest can be handled as errors
 */
const STATE Init__[7] = {
        EDown,          Init__Down,
        EDrag,          Init__errorEvent,
        ECapacitiveUP,  Init__errorEvent,
        EResistiveUP,   Init__errorEvent,
        ETouchTimer,    Init__errorEvent,
        EHoldTimer,     Init__errorEvent,
        ESuppressTimer, Init__errorEvent
};

/*!
 *  :Dispatch state end its elements
 *  Here the valid events are DRAG and the UP events.
 */
const STATE_ELEMENT Dispatch__Drag[8] = {
        0,                  ACT(storeMovePos),    Eignore,
        CND(insideMoveThreshold), ACT(consumeEvent), EDispatch,
        0,                  ACT(consumeEvent),    Eignore,
        0,                  ACT(setCurrentPos),   Eignore,
        0,                  ACT(addDraggingPos),  Eignore,
        0,                  ACT(produceMove),     Eignore,
        CND(looksLikeHold), ACT(initHoldTimer),EInHoldTime_U,
        0,                  0,                    EDispatch
} ;

const STATE_ELEMENT Dispatch__CapacitiveUp[3] = {
        0,                  ACT(consumeEvent),    Eignore,
        0,                  ACT(setCurrentPos),   Eignore,
        0,                  ACT(produceRelease),  EInit
} ;

const STATE_ELEMENT Dispatch__ResistiveUp[4] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(setCurrentPos),   Eignore,
        CND(isSuppressTimer),ACT(initMoveSuppressTimer),  ESuppress_D,
        0,                   ACT(produceRelease),     EInit
} ;

/*!
 * All the rest of the events are errors so produce error entry to log and
 * stay in the Dispatch state
 * (TODO: note that in the future we may further
 * define the error cases so that they may change state; )
 */
const STATE_ELEMENT DispatcherrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EDispatch
};

const STATE Dispatch__[7] = {
        EDown,          DispatcherrorEvent,
        EDrag,          Dispatch__Drag,
        ECapacitiveUP,  Dispatch__CapacitiveUp,
        EResistiveUP,   Dispatch__ResistiveUp,
        ETouchTimer,    DispatcherrorEvent,
        EHoldTimer,     DispatcherrorEvent,
        ESuppressTimer, DispatcherrorEvent
};

/*!
 *  :InTouchTime state end its elements
 *  Here the valid events are DRAG and the UP events and the TouchTimer
 */
const STATE_ELEMENT InTouchTime__Drag[6] = {
        0,                  ACT(storeMovePos),    Eignore,
        CND(insideTouchTimeArea), ACT(consumeEvent),      Eignore,
        CND(insideTouchTimeArea), ACT(addToTouch),        EInTouchTime,     // Calculate touch XY as average of the touches
        0,                    ACT(clearTouchTimer),   Eignore,          // These lines are done only if insideTouchArea returns false
        0,                    ACT(clearHoldTimer),    Eignore,
        0,                    ACT(produceTouch),      EDispatch

} ;
/**
 * Note that consumeEvent is missing so after doing this the state engine will do EDispatch
 */
const STATE_ELEMENT InTouchTime__CapacitiveUp[4] = {
        0,                  ACT(setCurrentPos),       Eignore,
        0,                  ACT(clearTouchTimer),     Eignore,
        0,                  ACT(clearHoldTimer),      Eignore,
        0,                  ACT(produceTouch),        EDispatch
} ;
/**
 * Note that consumeEvent is not called if isHoldTimer returns false, so the Dispatch will be done
 * by the state machine.
 */
const STATE_ELEMENT InTouchTime__ResistiveUp[5] = {
        0,                   ACT(setCurrentPos),   Eignore,
        0,                   ACT(clearTouchTimer),    Eignore,
        0,                   ACT(produceTouch),       Eignore,
        CND(isHoldTimer),    0, /*ACT(consumeEvent),*/EInHoldTime_U,    // Note that otherwise immediate UP is handled improperly
        0,                   0,                       EDispatch
} ;

const STATE_ELEMENT InTouchTime__TouchTimer[6] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(clearTouchTimer),    Eignore,
        0,                   ACT(produceTouch),       Eignore,
        CND(isTouchArea),    ACT(prepareTouchArea),Eignore,         // prepare the other touch area
        CND(isHoldTimer),    0,                       EInHoldTime_U,
        0,                   0,                     EInTouchArea
} ;


/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the InTouchTime state
 */
const STATE_ELEMENT InTouchTimeerrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInTouchTime
};

const STATE InTouchTime__[7] = {
        EDown,          InTouchTimeerrorEvent,
        EDrag,          InTouchTime__Drag,
        ECapacitiveUP,  InTouchTime__CapacitiveUp,
        EResistiveUP,   InTouchTime__ResistiveUp,
        ETouchTimer,    InTouchTime__TouchTimer,
        EHoldTimer,     InTouchTimeerrorEvent,
        ESuppressTimer, InTouchTimeerrorEvent
};

/*!
 *  :InHoldTime_U state end its elements
 *  Here only touch timer event is invalid
 */

const STATE_ELEMENT InHoldTime_U__Down[1] = {
        0,                    0,      EInHoldTime_D  // Note that consumeEvent is not called
} ;

const STATE_ELEMENT InHoldTime_U__Drag[3] = {
        0,                  ACT(storeMovePos),    Eignore,
        CND(insideHoldArea), ACT(consumeEvent),       EInHoldTime_U,
        0,                    ACT(clearHoldTimer),    EDispatch     // Note that in this case consumeEvent is not called
} ;
/**
 * Note that consumeEvent is missing so after doing this the state engine will do EDispatch
 */
const STATE_ELEMENT InHoldTime_U__CapacitiveUp[1] = {
        0,                  ACT(clearHoldTimer),      EDispatch     // Note that consumeEvent not called
} ;
/**
 *
 */
const STATE_ELEMENT InHoldTime_U__ResistiveUp[5] = {
        0,                   ACT(consumeEvent),  Eignore,
        0,                   ACT(setCurrentPos),   Eignore,
        CND(isSuppressTimer),ACT(initTouchSuppressTimer),  EInHoldTime_D,   // If suppression, start timer and wait for down or timer
        0,                   ACT(clearHoldTimer), Eignore,  // remember to do this
        0,                   ACT(produceRelease), EInit     // No suppression, then this is immediate release
} ;

const STATE_ELEMENT InHoldTime_U__HoldTimer[3] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(produceHold),        Eignore,
        0,                   ACT(restartHoldTimer),   EInHoldTime_U,
} ;

const STATE_ELEMENT InHoldTime_U__SuppressTimer[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInHoldTime_U
} ;


/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the InHoldTime_U state
 */
const STATE_ELEMENT InHoldTime_UerrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInHoldTime_U
};

const STATE InHoldTime_U__[7] = {
        EDown,          InHoldTime_U__Down,
        EDrag,          InHoldTime_U__Drag,
        ECapacitiveUP,  InHoldTime_U__CapacitiveUp,
        EResistiveUP,   InHoldTime_U__ResistiveUp,
        ETouchTimer,    InHoldTime_UerrorEvent,
        EHoldTimer,     InHoldTime_U__HoldTimer,
        ESuppressTimer, InHoldTime_U__SuppressTimer
};


/*!
 *  :InHoldTime_D state end its elements
 *  Here drag, touch timer and suppress timer events are invalid
 */

const STATE_ELEMENT InHoldTime_D__Down[5] = {
        0,                    ACT(clearSuppressTimer),    Eignore,
        0,                    ACT(consumeEvent),      Eignore,
        CND(insideHoldArea),  0,                      EInHoldTime_U,
        0,                    ACT(clearHoldTimer),    Eignore,
        0,                    ACT(produceMove),       EDispatch
} ;

/**
 * Note that consumeEvent is missing so after doing this the state engine will do InHoldTime_U
 */
const STATE_ELEMENT InHoldTime_D__CapacitiveUp[1] = {
        0,                  0,    EInHoldTime_U
} ;
/**
 * Note that consumeEvent is missing so after doing this the state engine will do InHoldTime_U
 */
const STATE_ELEMENT InHoldTime_D__ResistiveUp[1] = {
        0,                  0,    EInHoldTime_U     // InHoldTime_U initialises timers etc. if needed
} ;
/*!
 * In case of hold timer has been elapsed stop the timers, generate Release UI event.
 */
const STATE_ELEMENT InHoldTime_D__HoldTimer[4] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(clearSuppressTimer), Eignore,
        0,                   ACT(clearHoldTimer),     Eignore,
        0,                   ACT(produceRelease),     EInit,
} ;
/*!
 * If suppress timer hits, stop the timers and generate Release UI event.
 */
const STATE_ELEMENT InHoldTime_D__SuppressTimer[4] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(clearSuppressTimer), Eignore,
        0,                   ACT(clearHoldTimer),     Eignore,
        0,                   ACT(produceRelease),     EInit,
} ;

/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the InHoldTime_D state
 */
const STATE_ELEMENT InHoldTime_DerrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInHoldTime_D
};

const STATE InHoldTime_D__[7] = {
        EDown,          InHoldTime_D__Down,
        EDrag,          InHoldTime_DerrorEvent,
        ECapacitiveUP,  InHoldTime_D__CapacitiveUp,
        EResistiveUP,   InHoldTime_D__ResistiveUp,
        ETouchTimer,    InHoldTime_DerrorEvent,
        EHoldTimer,     InHoldTime_D__HoldTimer,
        ESuppressTimer, InHoldTime_D__SuppressTimer
};


/*!
 *  :InTouchArea state end its elements
 *  Here Drag and Up events are valid.
 *  If drag is inside touch are it is ignored, otherwise
 *  the Dispatch state will handle the event.
 */

const STATE_ELEMENT InTouchArea__Drag[3] = {
        0,                  ACT(storeMovePos),    Eignore,
        CND(insideTouchArea), ACT(consumeEvent),     EInTouchArea,
        0,                    0,                     EDispatch  // Note that in this case consumeEvent has not been called so Dispatch state processes the message
} ;

/**
 * Note that consumeEvent is missing so after doing this the state engine will do Dispatch
 */
const STATE_ELEMENT InTouchArea__CapacitiveUp[1] = {
        0,                  0,    EDispatch
} ;
/**
 * Note that consumeEvent is missing so after doing this the state engine will do Dispatch
 */
const STATE_ELEMENT InTouchArea__ResistiveUp[1] = {
        0,                  0,    EDispatch
} ;

/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the InTouchArea state
 */
const STATE_ELEMENT InTouchAreaerrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      EInTouchArea
};

const STATE InTouchArea__[7] = {
        EDown,          InTouchAreaerrorEvent,
        EDrag,          InTouchArea__Drag,
        ECapacitiveUP,  InTouchArea__CapacitiveUp,
        EResistiveUP,   InTouchArea__ResistiveUp,
        ETouchTimer,    InTouchAreaerrorEvent,
        EHoldTimer,     InTouchAreaerrorEvent,
        ESuppressTimer, InTouchAreaerrorEvent
};


/*!
 *  :Suppress_D state end its elements
 *  Here Down and suppress timers are OK.
 */

/*!
 * Down will be handled as a Drag event in the Dispatch state.
 */
const STATE_ELEMENT Suppress_D__Down[2] = {
        0,           ACT(clearSuppressTimer),    Eignore,
        0,           ACT(renameToDrag),          EDispatch
} ;
/*!
 * Suppress timer will generate Release UI event.
 */
const STATE_ELEMENT Suppress_D__SuppressTimer[3] = {
        0,                   ACT(consumeEvent),       Eignore,
        0,                   ACT(clearSuppressTimer), Eignore,
        0,                   ACT(produceRelease),     EInit,
} ;

/**
 * All the rest of the events are errors so produce error entry to log and
 * stay in the Suppress_D state
 */
const STATE_ELEMENT Suppress_DerrorEvent[2] = {
        0,              ACT(consumeEvent),    Eignore,  // remember to consume event, otherwise state machine will loop...
        0,              ACT(errorEvent),      ESuppress_D
};

const STATE Suppress_D__[7] = {
        EDown,          Suppress_D__Down,
        EDrag,          Suppress_DerrorEvent,
        ECapacitiveUP,  Suppress_DerrorEvent,
        EResistiveUP,   Suppress_DerrorEvent,
        ETouchTimer,    Suppress_DerrorEvent,
        EHoldTimer,     Suppress_DerrorEvent,
        ESuppressTimer, Suppress_D__SuppressTimer
};
/*!
 * The allStates array contains all the possible states of the state machine.
 */
const STATE* const allStates[8] =
{
        Ignore__, Init__, Dispatch__, InTouchTime__, InHoldTime_U__, InHoldTime_D__, InTouchArea__, Suppress_D__
};
/*!
 * stateNames are used in the logging
 */
const char* const stateNames[8] =
{
        "Ignore", "Init", "Dispatch", "InTouchTime", "InHoldTime_U", "InHoldTime_D", "InTouchArea", "Suppress"
};

// event names are also used in logging
const char* const hweventNames[] = {
        "EDown",
        "EMove",
        "ECapacitiveUP",
        "EResistiveUP",
        "ETouchTimer",
        "EHoldTimer",
        "ESuppressTimer"
} ;


QStm_StateEngine::QStm_StateEngine(QStm_StateEngineConfiguration* config, QStm_TimerInterfaceIf* timerif, int index)
{
    m_config = config ;
    m_timerif = timerif ;
    m_currentState = EInit ;
    m_index = index ;
    m_lastMessageTime.start();
}

QStm_StateEngine::~QStm_StateEngine()
{
    m_touchPoints.clear() ;
    qDeleteAll(m_dragPoints.begin(), m_dragPoints.end());
    m_dragPoints.clear() ;
}

void QStm_StateEngine::consumeEvent()
{
    m_eventConsumed = true ;
}


bool QStm_StateEngine::isTouchTimer()
{
    bool isit =  (m_config->m_touchTimerLimit > 0) ;

    return isit ;
}


bool QStm_StateEngine::isHoldTimer()
{
    bool isit =  (m_config->m_holdTimerLimit > 0) ;

    return isit ;
}


bool QStm_StateEngine::isSuppressTimer()
{
    bool isit =  (m_config->m_suppressTimerLimit > 0) ;

    return isit ;
}

bool QStm_StateEngine::isTouchTimeArea()
{
    bool isit = (m_config->m_touchTimeTolerance.x() > 0) ;
    return isit  ;
}


bool QStm_StateEngine::isTouchArea()
{
    bool isit = (m_config->m_touchTolerance.x() > 0) ;
    return isit  ;
}


bool QStm_StateEngine::isHoldArea()
{
    bool isit = (m_config->m_holdTolerance.x() > 0) ;
    return isit  ;
}

bool QStm_StateEngine::insideArea(const QPoint& point, const QRect& rect, QStm_AreaShape shape, const QPoint& tolerance)
{
    bool isit;
    switch(shape)
    {
    default:    // pass trough
    case ERectangle:
    {
    	
        isit = rect.contains(m_hwe.m_position) ;
        break ;
    }
    case ECircle:
    {
        QPoint delta = m_hwe.m_position - point;
        long circlepoint = delta.x() * delta.x() + delta.y() * delta.y();
        isit = (circlepoint < tolerance.x() * tolerance.x());
        break ;
    }
    case EEllipse:
    {
        int asquare = tolerance.x() * tolerance.x() ;
        int bsquare = tolerance.y() * tolerance.y() ;
        QPoint delta = m_hwe.m_position - point;
        int result = (delta.x() * delta.x()) * bsquare + (delta.y() * delta.y()) * asquare;

        isit = (result < asquare * bsquare);
        break ;
    }
    }
    return isit ;
}

bool QStm_StateEngine::insideTouchTimeArea()
{
    return insideArea(m_touchCentre, m_touchRect, m_config->m_touchAreaShape, m_config->m_touchTimeTolerance);
}


bool QStm_StateEngine::insideTouchArea()
{
    return insideArea(m_touchCentre, m_touchRect, m_config->m_touchAreaShape, m_config->m_touchTolerance);
}


bool QStm_StateEngine::insideHoldArea()
{
    return insideArea(m_holdCentre, m_holdRect, m_config->m_holdAreaShape, m_config->m_holdTolerance);
}


bool QStm_StateEngine::insideMoveThreshold()
{
    bool inside = false;
    if ( m_dragPoints.count() > 0 )
    {
        QStm_HwEvent* hwe = m_dragPoints[m_dragPoints.count() - 1];
        QPoint delta = m_hwe.m_position - hwe->m_position; 
        inside = (ABS(delta.x()) <= m_config->m_moveTolerance.x() / 2) &&
                 (ABS(delta.y()) <= m_config->m_moveTolerance.y() / 2);
        if (m_config->m_enableLogging) 
        {
            LOGARG("insideMoveThreshold: delta(%d, %d), inside == %d", delta.x(), delta.y(), inside); LOGFLUSH;
        }
    }
    return inside;
}

bool QStm_StateEngine::looksLikeHold()
{
    bool isit = isNewHoldingPoint() ;
    return isit ;
}

void QStm_StateEngine::errorEvent()
{
    if (m_config->m_enableLogging)
    {
        LOGARG("errorEvent: %s %s", stateNames[m_currentState], hweventNames[m_hwe.m_type]) ;
    }
}


void QStm_StateEngine::initTouchTimer()
{
    m_touchRect = toleranceRect(m_hwe.m_position, m_config->m_touchTolerance) ;
    m_touchCentre = m_hwe.m_position ;
    m_timerif->startTouchTimer(m_config->m_touchTimerLimit, m_index) ;
}


void QStm_StateEngine::initHoldTimer()
{
    m_holdRect = toleranceRect(m_hwe.m_position, m_config->m_holdTolerance) ;
    m_holdCentre = m_hwe.m_position ;
    m_timerif->startHoldTimer(m_config->m_holdTimerLimit, m_index) ;
}


void QStm_StateEngine::restartHoldTimer()
{
    m_timerif->startHoldTimer(m_config->m_holdTimerLimit, m_index) ;
}


void QStm_StateEngine::initTouchSuppressTimer()
{
    m_timerif->startSuppressTimer(m_config->m_suppressTimerLimit, m_index) ;
}


void QStm_StateEngine::initMoveSuppressTimer()
{
    m_timerif->startSuppressTimer(m_config->m_moveSuppressTimerLimit, m_index) ;
}


void QStm_StateEngine::clearTouchTimer()
{
    m_timerif->cancelTouchTimer(m_index) ;
}


void QStm_StateEngine::clearHoldTimer()
{
    m_timerif->cancelHoldTimer(m_index) ;
}


void QStm_StateEngine::clearSuppressTimer()
{
    m_timerif->cancelSuppressTimer(m_index) ;
}


QStm_UiEvent* QStm_StateEngine::createUIEvent(QStm_UiEventCode code, const QPoint& pos)
{
    // Check if this is EMove; then check if we might have a jump speed because of filtered events
    // before generating this one; use the saved position and time from the previous driver event.
    if ( code == qstmUiEventEngine::EMove &&
         (  m_previousUiGenerated == qstmUiEventEngine::ETouch ||
            m_previousUiGenerated == qstmUiEventEngine::EHold    ))
    {
        if (m_config->m_enableLogging)
        {
            LOGARG("changed prev XY: from (%d, %d) to (%d, %d)",m_previousXY.x(), m_previousXY.y(),
            		m_lastFilteredPosition.x(), m_lastFilteredPosition.y()  ) ;
        }
        m_lastMessageTime = m_lastFilteredMessageTime;
        
        m_previousXY = m_lastFilteredPosition ;
    }
    m_previousUiGenerated = code ;
 
    QStm_UiEvent* ue = new QStm_UiEvent(code, m_gestureStartXY, pos, getPreviousXY(pos),
                                       isTimerMessage(), m_hwe.m_target, getInterval(), m_index, 
                                       m_hwe.m_time) ;
    return ue;
}

QPoint QStm_StateEngine::getPreviousXY(const QPoint& currentXY)
{
    QPoint p = m_previousXY ;
    m_previousXY = currentXY ;
    return p ;
}


bool QStm_StateEngine::isTimerMessage()
{
    return (m_hwe.m_type >= ETouchTimer); // NOTE: if new events are added at the end of the list this needs to be changed
}

void QStm_StateEngine::produceTouch()
{
    m_wasFiltered = false ;
    QStm_UiEvent* cue ;
    getInterval() ; // dummy call to initialize the variable....
    if (m_touchPoints.count()>0)
    {
        // calculate average of the touch points
        m_currentTouchXY = calculateTouchAverageFromPoints() ;
        cue = createUIEvent(qstmUiEventEngine::ETouch, m_currentTouchXY) ;
    }
    else
    {
        cue = createUIEvent(qstmUiEventEngine::ETouch, m_uiEventXY) ;
    }
    m_config->m_uiEventSender->addEvent(cue) ;
}

void QStm_StateEngine::produceMove()
{
    m_wasFiltered = false ;
    QStm_UiEvent* cue = createUIEvent(qstmUiEventEngine::EMove, m_uiEventXY) ;
    m_config->m_uiEventSender->addEvent(cue) ;
}


void QStm_StateEngine::produceRelease()
{
    m_wasFiltered = false ;
    QStm_UiEvent* cue = createUIEvent(qstmUiEventEngine::ERelease, m_uiEventXY) ;
    m_config->m_uiEventSender->addEvent(cue) ;
    if (m_config->m_enableLogging)
    {
        LOGFLUSH ;
    }
}


void QStm_StateEngine::produceHold()
{
    m_wasFiltered = false ;
    QStm_UiEvent* cue = createUIEvent(qstmUiEventEngine::EHold, m_holdCentre) ;
    m_config->m_uiEventSender->addEvent(cue) ;
}

void QStm_StateEngine::renameToDrag()
{
    m_hwe.m_type = qstmUiEventEngine::EDrag ;
}


void QStm_StateEngine::prepareTouchTimeArea()
{
    if (m_touchPoints.count()>0) m_touchPoints.clear() ;
    m_touchRect = toleranceRect(m_hwe.m_position, m_config->m_touchTimeTolerance) ;
}


void QStm_StateEngine::prepareTouchArea()
{
    if (m_touchPoints.count()>0) m_touchPoints.clear() ;
    m_touchRect = toleranceRect(m_hwe.m_position, m_config->m_touchTolerance) ;
}


void QStm_StateEngine::prepareHoldArea()
{
    m_holdRect = toleranceRect(m_hwe.m_position, m_config->m_holdTolerance) ;
}


void QStm_StateEngine::storeMovePos()
{
    if (m_config->m_enableLogging)
    {
        LOGARG("store move pos from (%d, %d) to (%d, %d)",m_lastFilteredPosition.x(), m_lastFilteredPosition.y(),
        		m_hwe.m_position.x(), m_hwe.m_position.y()  ) ;
    }
    m_lastFilteredPosition = m_hwe.m_position ;
    m_lastFilteredMessageTime = m_hwe.m_time ;

}


void QStm_StateEngine::setCurrentPos()
{
    m_uiEventXY = m_hwe.m_position ;
}


void QStm_StateEngine::setGestureStart()
{
    m_gestureStartXY = m_hwe.m_position ;
    m_previousXY = m_hwe.m_position ;
    m_gestureTarget = m_hwe.m_target ;
    qDeleteAll(m_dragPoints.begin(), m_dragPoints.end());
    m_dragPoints.clear() ;
    m_touchPoints.clear() ;
}


void QStm_StateEngine::addToTouch()
{
    m_touchPoints.append(QStm_HwEvent(m_hwe.m_type, m_hwe.m_position, m_hwe.m_time, m_hwe.m_target, m_index)) ;
    // calculate the average of touch points and move the touch area accordingly
    // this allows slight movement of the figertip while inside touch time
    if (m_touchPoints.count() > 2)
    {
        QPoint newtp = calculateTouchAverageFromPoints() ;
        m_touchRect = toleranceRect(newtp, m_config->m_touchTolerance) ;
        m_holdRect = toleranceRect(newtp, m_config->m_holdTolerance) ;
    }
}


void QStm_StateEngine::addDraggingPos()
{
    m_dragPoints.append(new QStm_HwEvent(m_hwe.m_type, m_hwe.m_position, m_hwe.m_time, m_hwe.m_target, m_index)) ;
}


bool QStm_StateEngine::handleStateEvent()
{
    // We get an event into m_hwe by this moment, lets kick the state machine
    m_wasFiltered = true ;

    calculateDelta() ;
    turnStateMachine() ;

    m_previousPointerEventPosition = m_hwe.m_position ;
    return m_wasFiltered ;
}


QRect QStm_StateEngine::getTouchArea()
{
    return m_touchRect ;
}


QRect QStm_StateEngine::getHoldArea()
{
    return m_holdRect ;
}

bool QStm_StateEngine::wasLastMessageFiltered()
{
    return m_wasFiltered ;
}

bool QStm_StateEngine::isNewHoldingPoint()
{
    int x = m_dragPoints.count();
    if (x > 2)    // are there any points to be checked?
    {
        QStm_HwEvent* phwe = m_dragPoints[x-1] ;
        QStm_HwEvent* phweinsidehold = phwe ;
        QRect recth = toleranceRect(phwe->m_position, m_config->m_holdTolerance) ;
        // Look backwards from the last point to see if there are enought points (enough in time) to look like a hold
        x -= 2 ;
        while (x > 0 && recth.contains(m_dragPoints[x]->m_position))
        {
            phweinsidehold = m_dragPoints[x];
            --x;
        }
        
        int tival = phweinsidehold->m_time.msecsTo(phwe->m_time) ;

        /**
         * remove the extra points from the list if they are outside of holding area
         */
        while (x > 0)
        {
            QStm_HwEvent* p = m_dragPoints[x] ;
            delete p ;
            m_dragPoints.removeAt(x) ;
            --x ;
        }

        // See the time difference of the two points which still are inside the hold area
        int limit = m_config->m_holdTimerLimit/2 ;
        if (tival > limit)
        {
            if (m_config->m_enableLogging)
            {
                LOGARG("isNewHoldingPoint: %s, dragpoints count %d", stateNames[m_currentState], m_dragPoints.count()) ;
            }
            return true ;
        }
    }
    else
    {
        // one or 0 points does not look like hold

    }
    return false ;
}


void QStm_StateEngine::calculateTouchAverage()
{
    m_uiEventXY.setX((m_uiEventXY.x() + m_hwe.m_position.x())/2) ;
    m_uiEventXY.setY((m_uiEventXY.y() + m_hwe.m_position.y())/2) ;
}

void QStm_StateEngine::calculateDelta()
{
    m_deltaVector.setX(m_hwe.m_position.x() - m_previousPointerEventPosition.x()) ;
    m_deltaVector.setY(m_hwe.m_position.y() - m_previousPointerEventPosition.y()) ;
}

void QStm_StateEngine::debugPrintState(QStm_StateMachineState anextstate)
{
    if (m_config->m_enableLogging)
    {
    LOGARG("%s: cuiev(%d,%d) cTxy ((%d,%d)(%d,%d)) cHxy ((%d,%d)(%d,%d)) gsXY(%d,%d) dV(%d,%d) EVNT(%d,%d (%s)) going to %s",
            stateNames[m_currentState],
            m_uiEventXY.x(), m_uiEventXY.y(),
            m_touchRect.x(), m_touchRect.y(),
            m_touchRect.x() +  + m_touchRect.width(), m_touchRect.y() + m_touchRect.height(),
            m_holdRect.x(), m_holdRect.y(), 
            m_holdRect.x() + m_holdRect.width(), m_holdRect.y() + m_holdRect.height(),
            m_gestureStartXY.x(), m_gestureStartXY.x(),
            m_deltaVector.x(), m_deltaVector.x(),
            m_hwe.m_position.x(), m_hwe.m_position.y(), hweventNames[m_hwe.m_type],
            stateNames[anextstate]
            );
    }
}

QRect QStm_StateEngine::toleranceRect(const QPoint& aCenterPoint, const QPoint& tolerance)
{
    // grow by the tolerance length, while keeping the center point
    QRect toleranceRect(
        aCenterPoint - tolerance,
        aCenterPoint + tolerance);
    return toleranceRect;
}

/*!
 * turnStateMachine.  Go trough the state elements found for the current event
 * until the event has been consumed.
 *
 * \pre m_currentState defines the current state and the index to the allStates array.
 * \pre m_hwe is the message being handled.  The corresponding STATE_ELEMENT array must be found and processed.
 *
 */
void QStm_StateEngine::turnStateMachine()
{

    const STATE_ELEMENT* pelement ;
    m_eventConsumed = false ;   // run the loop until the event has been consumed
    // Now run trough the motions of the state elements, and prepare to change to next state while doing so.
    // If the state elements set the m_eventConsumed then all is done
    while (!m_eventConsumed) {
        int i = 0 ;
        const STATE* const pcurrentstate = allStates[m_currentState] ;
        // Since each state definition must contain entries for all possible events the following loop cannot fail ;-)
        while (pcurrentstate[i].theEvent != m_hwe.m_type ) ++i ;
        pelement = pcurrentstate[i].stateElements ;
        QStm_StateMachineState      nextState = Eignore ;
        /*
         * Handle the individual state elements.  If there is a condition function,
         * call the function and if it returns true, handle the action function and possible next state
         * if the condition returns false, continue to next element
         * if there is no condition, run the action function if it exists.
         * if the next state is defined (i.e it is != Eignore), go to that state
         */
        while (nextState == Eignore) {
            condition_t cndfunc = pelement->conditionFunction ;
            action_t    actfunc = pelement->actionFunction ;
            if (cndfunc != 0) {
                /*
                 * There was a condition function, call it to see whether the action needs to performed and/or the next satte defined
                 */
                if (cndfunc(this)) {
                    // Condition was true, handle it
                    // call the action if it exists
                    if (actfunc != 0) actfunc(this) ;
                    // and now get to the next state
                    nextState   = pelement->nextState ; // Note that while this remains Eignore there are elements to be run
                }
            }
            else  {
                /**
                 * No condition function, call the possible action function and get the next state
                 */
                if (actfunc != 0) actfunc(this) ;
                nextState   = pelement->nextState ;     // Note that while this remains Eignore there are elements to be run
            }
            ++pelement ;    // next entry in the elements
        }
        if (m_config->m_enableLogging) debugPrintState(nextState) ;
        m_currentState = nextState ;    // Change to the next state
    }
}

long QStm_StateEngine::getInterval()
{
	
	int interval = m_lastMessageTime.elapsed();
	
	m_lastMessageTime.restart();
    return  interval;
}

QPoint QStm_StateEngine::calculateTouchAverageFromPoints()
{
    QPoint tp ;
    int count = m_touchPoints.count() ;
    for (int i = 0; i < count; i++) {
        tp += m_touchPoints[i].m_position;
    }
    if(count) {
        tp.setX(tp.x() / count) ;
        tp.setY(tp.y() / count) ;
    }
    return tp ;
}

