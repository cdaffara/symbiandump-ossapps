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
#include "qstmstatemachine.h"
#include "qstmstateengine.h"
#include "qstmuievent_if.h"
#include "qstmcallbacktimer.h"
#include "qstmstateengineconfig.h"

#include "qstmfilelogger.h"
#include "qstmutils.h"
#include <QtGui>

#ifdef Q_OS_SYMBIAN
#include <w32std.h>
#include <coecntrl.h>
#endif

#if defined(Q_WS_X11)
#include <X11/Xlib.h>
#endif

using namespace qstmUiEventEngine ;

//int pointBufferTimerCB(TAny* prt);

extern const char* stateNames[8] ;

const char* const ttypeNames[] = {  // for debugging purposes define the names of the pointer events
            "EButton1Down         ",
            "EButton1Up           ",
            "EButton2Down         ",
            "EButton2Up           ",
            "EButton3Down         ",
            "EButton3Up           ",
            "EDrag                ",
            "EMove                ",
            "EButtonRepeat        ",
            "ESwitchOn            ",
            "EOutOfRange          ",
            "EEnterCloseProximity ",
            "EExitCloseProximity  ",
            "EEnterHighPressure   ",
            "EExitHighPressure    "
            };

QStm_StateMachine::QStm_StateMachine()
{
    m_WasMessageFiltered = false ;
    m_wseventmonitoringenabled = false ; // NB: enabled only if really used by application
    m_loggingenabled = false ;
    m_capacitiveup = false ;
    m_adjustYposition = false ;
    //m_pointBuffer = NULL;
    init();
}

QStm_StateMachine::~QStm_StateMachine()
{
    for (int i = 0; i < KMaxNumberOfPointers; i++)
    {
        delete m_holdTimer[i] ;
        delete m_touchTimer[i] ;
        delete m_suppressTimer[i] ;
        delete m_impl[i] ;
    }
    delete m_config ;
/*    
    if (m_pointBufferTimer && m_pointBufferTimer->IsActive())
    {
        m_pointBufferTimer->Cancel();
    }
    delete m_pointBufferTimer;
    m_pointBufferTimer = NULL;
*/    
}



void QStm_StateMachine::init()
{
    m_config = new QStm_StateEngineConfiguration() ;

    for (int i = 0; i < KMaxNumberOfPointers; i++)
    {
        m_impl[i] = new QStm_StateEngine(m_config, this, i) ;
        
        m_holdTimer[i] = new QStm_CallbackTimer(this, &qstmUiEventEngine::QStm_StateMachine::handleholdTimer, 0, i, true);
        m_touchTimer[i] = new QStm_CallbackTimer(this, &qstmUiEventEngine::QStm_StateMachine::handletouchTimer, 0, i, true);
        m_suppressTimer[i] = new QStm_CallbackTimer(this, &qstmUiEventEngine::QStm_StateMachine::handlesuppressTimer, 0, i, true);
        /*
        m_holdTimer[i] = new QStm_CallbackTimer(this, SLOT(handleholdTimer(int)), 0, i, true);
        m_touchTimer[i] = new QStm_CallbackTimer(this, SLOT(handletouchTimer(int)), 0, i, true);
        m_suppressTimer[i] = new QStm_CallbackTimer(this, SLOT(handlesuppressTimer(int)), 0, i, true);
        */
    }
//    m_coeEnv = CCoeEnv::Static();

    m_3mminpixels = QStm_Utils::mm2Pixels(3) ;
}


bool QStm_StateMachine::handleStateEvent(const QStm_PlatformPointerEvent& platPointerEvent)
{
    int index = pointerIndex(platPointerEvent);
    QStm_StateEngine* engine = m_impl[index];
    createHwEvent(engine->initEvent(), platPointerEvent, platPointerEvent.m_target, platPointerEvent.m_time) ;
    if (m_loggingenabled)
    {
        LOGARG("handleStateEvent: ptr %d", index) ;
    }
    m_WasMessageFiltered = engine->handleStateEvent() ;
    return m_WasMessageFiltered ;
}


bool QStm_StateMachine::wasLastMessageFiltered(int aPointerNumber)
{
    return m_impl[aPointerNumber]->wasLastMessageFiltered() ;
}


QRect QStm_StateMachine::getTouchArea(int pointerNumber)
{
    return m_impl[pointerNumber]->getTouchArea() ;
}

void QStm_StateMachine::setTouchTimeArea(long fingersize_mm)
{
    m_config->setTouchTimeArea(fingersize_mm) ;
}

void QStm_StateMachine::setTouchArea(long fingersize_mm)
{
    m_config->setTouchArea(fingersize_mm) ;
}

void QStm_StateMachine::setMoveTolerance(long fingersize_mm)
{
    m_config->setMoveTolerance(fingersize_mm);    
}

QPoint QStm_StateMachine::getMoveTolerance()
{ 
    return m_config->getMoveTolerance(); 
}


QStm_AreaShape QStm_StateMachine::getTouchAreaShape()
{
    return m_config->getTouchAreaShape() ;
}


void QStm_StateMachine::setTouchAreaShape(const QStm_AreaShape aShape)
{
    m_config->setTouchAreaShape(aShape) ;
}


unsigned int QStm_StateMachine::getTouchTimeout()
{
    return m_config->getTouchTimeout() ;
}


void QStm_StateMachine::setTouchTimeout(unsigned int aDelay)
{
    m_config->setTouchTimeout(aDelay) ;
}


QRect QStm_StateMachine::getHoldArea(int pointerNumber)
{
    return m_impl[pointerNumber]->getHoldArea() ;
}


void QStm_StateMachine::setHoldArea(long fingersize_mm)
{
    m_config->setHoldArea(fingersize_mm) ;
}

QStm_AreaShape QStm_StateMachine::getHoldAreaShape()
{
    return m_config->getHoldAreaShape() ;
}

void QStm_StateMachine::setHoldAreaShape(const QStm_AreaShape aShape)
{
    m_config->setHoldAreaShape(aShape) ;
}

unsigned int QStm_StateMachine::getHoldTimeout()
{
    return m_config->getHoldTimeout() ;
}

void QStm_StateMachine::setHoldTimeout(unsigned int a)
{
    m_config->setHoldTimeout(a) ;
}

unsigned int QStm_StateMachine::getTouchSuppressTimeout()
{
    return m_config->getTouchSuppressTimeout() ;
}

void QStm_StateMachine::setTouchSuppressTimeout(unsigned int a)
{
    m_config->setTouchSuppressTimeout(a) ;
}

unsigned int QStm_StateMachine::getMoveSuppressTimeout()
{
    return m_config->getMoveSuppressTimeout() ;
}

void QStm_StateMachine::setMoveSuppressTimeout(unsigned int a)
{
    m_config->setMoveSuppressTimeout(a) ;
}

bool QStm_StateMachine::addUiEventObserver(QStm_UiEventObserverIf* observer)
{
    return m_config->addUiEventObserver(observer) ;
}


bool QStm_StateMachine::removeUiEventObserver(QStm_UiEventObserverIf* observer)
{
    return m_config->removeUiEventObserver(observer) ;
}

void QStm_StateMachine::enableLogging(bool aEnable)
{
    m_loggingenabled = aEnable ;
    m_config->enableLogging(aEnable) ;
}

int QStm_StateMachine::pointerIndex(const QStm_PlatformPointerEvent& platPointerEvent)
{
    int index = 0;
/*    
#if defined(ADVANCED_POINTER_EVENTS)
    if (aPointerEvent.IsAdvancedPointerEvent())
    {
        const TAdvancedPointerEvent* tadvp = aPointerEvent.AdvancedPointerEvent() ;
        index = tadvp->PointerNumber() ;
    }
#endif
*/
    index = platPointerEvent.m_pointerNumber;
    return index;
}

void QStm_StateMachine::createHwEvent(QStm_HwEvent& event, const QStm_PlatformPointerEvent& platPointerEvent, void* target, const QTime& time)
{
/*  should be set by this moment by CStateEngine with that index in CStateEngine::initEvent()
    aEvent.iPointerNumber = PointerIndex(aPointerEvent);
*/
    event.m_target = target ;
    event.m_time = time;
    // Change to screen coordinates here while the window is still existing....
    event.m_position = screenCoordinates(platPointerEvent.m_position, target) ;
    switch (platPointerEvent.m_type)
    {
    case QStm_PlatformPointerEvent::EButton1Down:
    case QStm_PlatformPointerEvent::EButton2Down:
    case QStm_PlatformPointerEvent::EButton3Down:
    {
        event.m_type = qstmUiEventEngine::EDown ;
        break ;
    }
    case QStm_PlatformPointerEvent::EButton1Up:
    case QStm_PlatformPointerEvent::EButton2Up:
    case QStm_PlatformPointerEvent::EButton3Up:
    {
        if (m_capacitiveup)
        {
            event.m_type = qstmUiEventEngine::ECapacitiveUP ;   // How could this be checked automatically?
        }
        else
        {
            event.m_type = qstmUiEventEngine::EResistiveUP ;    // How could this be checked automatically?
        }

        break ;
    }
    case QStm_PlatformPointerEvent::EDrag:
    {
        event.m_type = qstmUiEventEngine::EDrag ;
        break ;
    }
    }
}


void QStm_StateMachine::startholdTimer(int pointerNumber)
{
    if (!m_holdTimer[pointerNumber]->isActive())
    {
        m_holdTimer[pointerNumber]->start();
    }
}


void QStm_StateMachine::handleholdTimer(int pointerNumber)
{
    QStm_StateEngine* engine = m_impl[pointerNumber];
    createTimerEvent(engine->initEvent(), qstmUiEventEngine::EHoldTimer) ;
    engine->handleStateEvent() ;
}


void QStm_StateMachine::cancelholdTimer(int pointerNumber)
{
    m_holdTimer[pointerNumber]->stop();
}


void QStm_StateMachine::startsuppressTimer(int pointerNumber)
{
    m_suppressTimer[pointerNumber]->start();
}

void QStm_StateMachine::handlesuppressTimer(int pointerNumber)
{
    QStm_StateEngine* engine = m_impl[pointerNumber];
    createTimerEvent(engine->initEvent(), qstmUiEventEngine::ESuppressTimer) ;
    engine->handleStateEvent() ;
}


void QStm_StateMachine::cancelsuppressTimer(int pointerNumber)
{
    m_suppressTimer[pointerNumber]->stop();
}


void QStm_StateMachine::starttouchTimer(int pointerNumber)
{
    if (!m_touchTimer[pointerNumber]->isActive())
    {
        m_touchTimer[pointerNumber]->start();
    }
}

void QStm_StateMachine::handletouchTimer(int pointerNumber)
{
    QStm_StateEngine* engine = m_impl[pointerNumber];
    createTimerEvent(engine->initEvent(), qstmUiEventEngine::ETouchTimer) ;
    engine->handleStateEvent() ;
}

void QStm_StateMachine::canceltouchTimer(int pointerNumber)
{
    if (m_touchTimer[pointerNumber]->isActive())
    {
        m_touchTimer[pointerNumber]->stop();
    }
}

void QStm_StateMachine::createTimerEvent(QStm_HwEvent& event, QStm_StateMachineEvent eventCode)
{
    event.m_type = eventCode ;
    // m_hwe.iPosition = TPos(0, 0) ;       // should we just leave the previous pos
    event.m_time = QTime::currentTime() ;
}


bool QStm_StateMachine::handleX11PlatformEvent(const XEvent* platEvent)
{
    bool isPointerEvent = false;
#if defined(Q_WS_X11)
    QStm_PlatformPointerEvent platPointerEvent;
    switch (platEvent->type) {
        case ButtonPress:
        {
            switch (platEvent->xbutton.button) {
                case Button1:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton1Down;
                    break;
                }
                case Button2:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton2Down;
                    break;
                }
                case Button3:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton3Down;
                    break;
                }
            }
            isPointerEvent = true;
            break;
        }
        case ButtonRelease:
        {
            switch (platEvent->xbutton.button) {
                case Button1:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton1Up;
                    break;
                }
                case Button2:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton2Up;
                    break;
                }
                case Button3:
                {
                    platPointerEvent.m_type = QStm_PlatformPointerEvent::EButton3Up;
                    break;
                }
            }
            isPointerEvent = true;
            break;
        }
        case MotionNotify:
        {
            platPointerEvent.m_type = QStm_PlatformPointerEvent::EMove;
            isPointerEvent = true;
            break;
        }
    }

    if (isPointerEvent) {
        QWidget* widget = QWidget::find((WId)platEvent->xany.window);

        platPointerEvent.m_target = widget;
        int mds = platEvent->xbutton.time;
        int msec = mds % 1000;
        int sec =  (mds / 1000) % 60;
        int hr = mds / (1000 * 3600);
        int min = (mds % (1000 * 3600)) / 60000;

        platPointerEvent.m_time = QTime(hr, min, sec, msec);
        platPointerEvent.m_pointerNumber = 0;
        platPointerEvent.m_position = QPoint(platEvent->xbutton.x,
                                             platEvent->xbutton.y);

        handleStateEvent(platPointerEvent);
    }
#endif // Q_WS_X11
    return isPointerEvent;
}

bool QStm_StateMachine::handleSymbianPlatformEvent(const QSymbianEvent* platEvent)
{
    bool ret = false;
#ifdef Q_OS_SYMBIAN
	const TWsEvent* wse = platEvent->windowServerEvent();

	if (wse) {
		CCoeControl* ctrl = REINTERPRET_CAST(CCoeControl*, wse->Handle());
		
		QStm_PlatformPointerEvent platPointerEvent;
		
		if (wse->Type() == EEventPointer) {
		    QWidget* widget = QWidget::find(ctrl);
		    ret = (widget != NULL);
			TPointerEvent* tpe = wse->Pointer(); 
			 // For Symbian it's one-to-one correspondence
			platPointerEvent.m_type = static_cast<QStm_PlatformPointerEvent::PEType>(tpe->iType); 
			platPointerEvent.m_modifiers = tpe->iModifiers;
			platPointerEvent.m_position = QPoint(tpe->iPosition.iX, tpe->iPosition.iY); 
			platPointerEvent.m_pointerNumber = 0;
			
#if defined(ADVANCED_POINTER_EVENTS)
		    if (tpe->IsAdvancedPointerEvent()) {
		        const TAdvancedPointerEvent* tadvp = tpe->AdvancedPointerEvent() ;
		        platPointerEvent.m_pointerNumber = tadvp->PointerNumber() ;
		    }
#endif
		    platPointerEvent.m_target = widget;
		    int h = wse->Time().DateTime().Hour();
		    int m = wse->Time().DateTime().Minute();
		    int s = wse->Time().DateTime().Second();
		    int ms = wse->Time().DateTime().MicroSecond() / 1000;
		    QTime time(h, m, s, ms);
		    
		    platPointerEvent.m_time = time;
		    handleStateEvent(platPointerEvent);
		}
	}
#endif
	return ret;
}




void QStm_StateMachine::startTouchTimer(int delay, int pointerNumber)
{
    m_touchTimer[pointerNumber]->setDelay(delay) ;
    starttouchTimer(pointerNumber) ;
}

void QStm_StateMachine::cancelTouchTimer(int pointerNumber)
{
    canceltouchTimer(pointerNumber) ;
}

void QStm_StateMachine::startHoldTimer(int delay, int pointerNumber)
{
    m_holdTimer[pointerNumber]->setDelay(delay) ;
    startholdTimer(pointerNumber) ;
}

void QStm_StateMachine::cancelHoldTimer(int pointerNumber)
{
    cancelholdTimer(pointerNumber) ;
}

void QStm_StateMachine::startSuppressTimer(int delay, int pointerNumber)
{
    m_suppressTimer[pointerNumber]->setDelay(delay) ;
    startsuppressTimer(pointerNumber) ;
}

void QStm_StateMachine::cancelSuppressTimer(int pointerNumber)
{
    cancelsuppressTimer(pointerNumber) ;
}


QPoint QStm_StateMachine::screenCoordinates(const QPoint& pos, void* gestureTarget)
{
    QPoint newPos = pos ;
    QWidget* widget = static_cast<QWidget*>(gestureTarget);
    
    if (!widget) {
        widget = QApplication::activeModalWidget();
    }
    
    if (!widget) {
        widget = QApplication::activePopupWidget();
    }
    
    if (widget) {    
        if (widget->isWindow()) {
            QPoint tp = widget->mapToGlobal(QPoint(0, 0)) ;
        	newPos += tp ;
            if (m_adjustYposition) {
                QSize sz = widget->size() ;

                // If we are running in capacitive touch device,
                // adjust the point up about 3 mm unless we are
                // near top or bottom of the window

                // Y position in the window
                int wY = newPos.y() - tp.y();
                int edge = 2*m_3mminpixels;

                if (wY >= 0 && wY <= (edge - 1)) {
                    // close to the top we adjust suitably so that immediately at the top adjust is 0
                    int adjust = wY / 2 ;
                    newPos.setY(newPos.y() - adjust) ;
                    if (m_loggingenabled) {
                        LOGARG("adjustment: nY %d tY %d [3mm: %d  adj: %d]", newPos.y(), tp.y(), m_3mminpixels, adjust) ;
                    }
                }
                else if (wY >= edge && wY <= (sz.height() - edge)) {
                    int from = newPos.y() ;
                    newPos.setY(newPos.y() - m_3mminpixels) ;
                    if (m_loggingenabled) {
                        LOGARG("adjustment: %d to %d [3mm: %d  middle]", from, newPos.y(), m_3mminpixels) ;
                    }
                }
                else {
                    // similarly at the bottom we adjust less the closer we get to the edge
                    int adjust = (sz.height() - wY) / 2 ;
                    newPos.setY(newPos.y() - adjust) ;
                    if (m_loggingenabled) {
                        LOGARG("adjustment: nY %d tY %d  sH %d [3mm: %d  adj: %d]",
                                newPos.y(), tp.y(), sz.height(), m_3mminpixels, adjust) ;
                    }

                }
            }
        }
        else  {
            // if the target does not own a window how can we adjust to the screen?
        }
    }
    return newPos ;
}

void QStm_StateMachine::enableCapacitiveUp(bool enable)
{
    m_capacitiveup = enable ;
}

void QStm_StateMachine::enableYadjustment(bool enable)
{
    m_adjustYposition = enable ;
}

int QStm_StateMachine::getNumberOfPointers()
{
    return KMaxNumberOfPointers ;
}


