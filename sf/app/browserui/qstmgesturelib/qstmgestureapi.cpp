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
*  Gesture UI Engine
*/


// Gesture Library: Framework
#include <qstmgestureengine.h>
#include <qstmstatemachine.h>
// Gesture Library: Recognizers
#include <qstmtapgesturerecogniser.h>
#include <qstmpangesturerecogniser.h>
#include <qstmhoveringgesturerecogniser.h>
#include <qstmedgescrollgesturerecogniser.h>
#include <qstmlongpressgesturerecogniser.h>
#include <qstmpinchgesturerecogniser.h>
#include <qstmtouchgesturerecogniser.h>
#include <qstmreleasegesturerecogniser.h>
#include <qstmleftrightgesturerecogniser.h>
#include <qstmupdowngesturerecogniser.h>
#include <qstmflickgesturerecogniser.h>
#include <qstmunknowngesturerecogniser.h>
#include <qstmzoomgesturerecogniser.h>

#include <qstmgestureapi.h>
#include <qstmgestureevent.h>
#include <QtGui>

using namespace qstmGesture;
using namespace qstmUiEventEngine;

QStm_GestureParameters::QStm_GestureParameters()
{
}

QStm_GestureParameters::QStm_GestureParameters(const QStm_GestureParameters& params )
{
    memcpy(&m_param[0], &params.m_param[0], sizeof(m_param));
    memcpy(&m_areaSettings[0], &params.m_areaSettings[0], sizeof(m_areaSettings));
    memcpy(&m_enabled[0], &params.m_enabled[0], sizeof(m_enabled));
}


QStm_GestureEngineApi::QStm_GestureEngineApi()
{
	init();
}

void QStm_GestureEngineApi::init()
{
    //m_gf = new QStm_GestureFramework();
	m_gestureEngine = new QStm_GestureEngine();
	m_statemachine = new QStm_StateMachine();
	m_statemachine->addUiEventObserver(m_gestureEngine);
	m_config = new QStm_GestureParameters();    
}

QStm_GestureEngineApi::~QStm_GestureEngineApi()
{
    // Prevent reactivation of contexts as they remove themselves
    m_dtorPhase = true;

    qDeleteAll(m_contexts.begin(), m_contexts.end());
    m_contexts.clear();
    m_ctxtStack.clear();
    delete m_config;
    delete m_gestureEngine;
    delete m_statemachine;
}

QStm_GestureContext* QStm_GestureEngineApi::createContext(int /*aId*/ )
{
    QStm_GestureContext* ctxt = new QStm_GestureContext(*this);
    return ctxt;
}

QStm_GestureContext* QStm_GestureEngineApi::context(int /*aId*/ )
{
    return NULL;
}

void QStm_GestureEngineApi::enableContext(QStm_GestureContext& context )
{
    // NB: enabling context puts it on top of the Context Stack
    int idx = m_ctxtStack.indexOf(&context);
    if(idx >= 0) {
        m_ctxtStack.removeAt(idx);
    }

    setupRecognizers(context, true);

    m_ctxtStack.append(&context);
}

void QStm_GestureEngineApi::disableContext(QStm_GestureContext& context )
{
    int idx = m_ctxtStack.indexOf(&context);
    
    if(idx == -1) {
        return;
    }

    m_ctxtStack.removeAt(idx);

    for(int i = 0; i < qstmGesture::EStmGestureUid_Count; ++i) {
    	qstmGesture::QStm_GestureRecogniserIf* rec = context.m_recognizers[i];
        if(rec) {
        	m_gestureEngine->removeGesture(rec);
        }
    }
/* OK: we need to decide semantics of the GestureContext concept:
   - is only one is active at a time?
   - is there a stack of them
   OR
   - application activates and deactivates them as needed
   - GestureEngine only ensures that proper order of recognizers is used

   Disabled code below, because it doesn't allow porting of the stmProto demo,
   where two TestContainer are used, each with own gesture context, simultaneously.

   If app uses only one context, then this change doesn't break anything

    // re-activate previous (new top-most) context
    // NB: if deleted context is not top-most (active) one, no updates done
    TInt count = m_ctxtStack.Count();
    if(idx == count && count > 0 && !m_dtorPhase)
        {
        QStm_GestureContext* previous = m_ctxtStack[count - 1];
        setupRecognizers(*previous, EFalse);
        }
*/
}

template<class T>
void QStm_GestureEngineApi::initRecognizer(T*& gesture, QStm_GestureContext& context, bool isNewCtx )
{
    qstmGesture::QStm_GestureRecogniserIf*& recognizer = context.m_recognizers[T::KUid];
    gesture = static_cast<T*>(recognizer);
    if(!gesture && isNewCtx) {
        recognizer = gesture = new T(&context);
        recognizer->enableLogging((context.m_logging & (1<<T::KUid)) != 0);
    }

    // TODO: Add in right place according to gesture class
    // Find similar (by Uid gesture, insert before it), otherwise add as last or
    //      before any known gestures that should follow after it (NOT IMPLEMENTED)
    
    int pos = m_gestureEngine->findGesture(T::KUid);
    bool added = m_gestureEngine->insertGesture(gesture, pos >= 0 ? pos : m_gestureEngine->gestureCount());
    if(!added) {
    	// TODO: exception here?
    }
}

void QStm_GestureEngineApi::setupRecognizers(QStm_GestureContext& context, bool isNewCtx )
{
    // Order of recognizers in the Gesture Engine (upper ones receive input before lower ones)
    //
    // PINCH
    // CORNER_ZOOM
    // EDGE_SCROLL
    // LONGPRESS
    // TOUCH
    // UP_DOWN
    // LEFT_RIGHT
    // HOVER
    // PAN
    // TAP / DOUBLE_TAP
    // FLICK
    // RELEASE
    // UNKNOWN

    QStm_GestureParameters& conf = context.config();

    bool filter = conf.param(qstmGesture::EEnableFiltering) != 0;

    // TOUCH AREA
    qstmGesture::QStm_GestureArea& touchArea = *(conf.area(qstmGesture::ETouchArea));
    int touchAreaSizeInMm = touchArea.m_size.width();
    m_statemachine->setTouchAreaShape ( qstmUiEventEngine::QStm_AreaShape(touchArea.m_shape) );
    m_statemachine->setTouchArea      ( !filter ? 0 : touchAreaSizeInMm );
    m_statemachine->setTouchTimeout   ( !filter ? 0 : touchArea.m_timeout);
    int moveToleranceInMm = conf.param(qstmGesture::EMoveTolerance);
    m_statemachine->setMoveTolerance(moveToleranceInMm);
    
    QWidget* gestureContext = static_cast<QWidget*>(context.getOwner());
    QRect ctxtRect = gestureContext ? gestureContext->rect() : QRect();
    if(gestureContext) {
        QPoint p = gestureContext->mapToGlobal(QPoint(0,0)) ;
        ctxtRect.moveTo(p) ;
        }

    // ===================================================== PINCH
    using qstmGesture::QStm_PinchGestureRecogniser;

    if (conf.enabled(QStm_PinchGestureRecogniser::KUid)) {
        QStm_PinchGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            qreal pspeed = conf.param(qstmGesture::EPinchSpeed) / 100.f;
            gesture->setPinchingSpeed(pspeed);
        }
    }

    // ===================================================== CORNER ZOOM
    using qstmGesture::QStm_ZoomGestureRecogniser;

    if (conf.enabled(QStm_ZoomGestureRecogniser::KUid)) {
        QStm_ZoomGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            gesture->setArea(ctxtRect);
            gesture->setRange(conf.param(qstmGesture::EZoomCornerSize));
        }
    }

    // ===================================================== EDGE SCROLL
    using qstmGesture::QStm_EdgeScrollGestureRecogniser;

    if (conf.enabled(QStm_EdgeScrollGestureRecogniser::KUid)) {
        QStm_EdgeScrollGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            gesture->setArea(ctxtRect);
            gesture->setScrollRange(conf.param(qstmGesture::EEdgeScrollRange)); // range is 20 pixels from the edge TODO: add this to settings...
        }
    }

    // ===================================================== LONG PRESS
    using qstmGesture::QStm_LongPressGestureRecogniser;

    if (conf.enabled(QStm_LongPressGestureRecogniser::KUid)) {
        QStm_LongPressGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            gesture->setArea(ctxtRect);
        }
    }

    // ===================================================== TOUCH
    using qstmGesture::QStm_TouchGestureRecogniser;

    if (conf.enabled(QStm_TouchGestureRecogniser::KUid)) {
    	QStm_TouchGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            // define empty area so that touch is reported only inside
            // our window (touch recognizer handles either an area or the target window)
            gesture->setArea(QRect());
        }
    }

    // ===================================================== LEFT-RIGHT
    using qstmGesture::QStm_LeftrightGestureRecogniser;

    if (conf.enabled(QStm_LeftrightGestureRecogniser::KUid)) {
        QStm_LeftrightGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);
    }

    // ===================================================== UP-DOWN
    using qstmGesture::QStm_UpdownGestureRecogniser;

    if (conf.enabled(QStm_UpdownGestureRecogniser::KUid)) {
        QStm_UpdownGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);
    }

    // ===================================================== HOVER
    using qstmGesture::QStm_HoveringGestureRecogniser;

    if (conf.enabled(QStm_HoveringGestureRecogniser::KUid)) {
        QStm_HoveringGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);
        if(gesture) {
            gesture->setHoveringSpeed(conf.param(qstmGesture::EHoverSpeed) / 100.f);
        }
    }

    // ===================================================== PAN
    using qstmGesture::QStm_PanGestureRecogniser;

    if (conf.enabled(QStm_PanGestureRecogniser::KUid)) {
        QStm_PanGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            gesture->setPanningSpeedLow ( conf.param(qstmGesture::EPanSpeedLow)  / 100.f );
            gesture->setPanningSpeedHigh( conf.param(qstmGesture::EPanSpeedHigh) / 100.f );
        }
    }

    // ===================================================== RELEASE
    using qstmGesture::QStm_ReleaseGestureRecogniser;

    if (conf.enabled(QStm_ReleaseGestureRecogniser::KUid)) {
        QStm_ReleaseGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            gesture->setArea(QRect());
        }
    }


    // ===================================================== TAP / DOUBLE TAP
    // Add recognizer before any existing Flick, Release, Unknown
    // Add the gesture to the gesture engine
    using qstmGesture::QStm_TapGestureRecogniser;

    if (conf.enabled(QStm_TapGestureRecogniser::KUid)) {
        int pos = m_gestureEngine->findGesture(QStm_TapGestureRecogniser::KUid);
        QStm_TapGestureRecogniser* gesture = NULL;
        
        if (pos > -1) {
            gesture = static_cast<QStm_TapGestureRecogniser*>(m_gestureEngine->gestureAt(pos));
            gesture->addTapListener(&context, context.getOwner());
            gesture->addDoubleTapListener(&context, context.getOwner());
        }
        else {
            initRecognizer(gesture, context, isNewCtx);
        }

        if(gesture) {
            gesture->setDoubleTapTimeout(conf.param(qstmGesture::EDoubleTapTimeout));
            gesture->setDoubleTapRange( touchAreaSizeInMm );
            gesture->ignoreFirstTap(false);
        }
    }

    // ===================================================== FLICK
    using qstmGesture::QStm_FlickGestureRecogniser;

    if (conf.enabled(QStm_FlickGestureRecogniser::KUid)) {
        QStm_FlickGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);

        if(gesture) {
            qreal flickSpeed = conf.param(qstmGesture::EFlickSpeed) / 100.f;
            gesture->setFlickingSpeed(flickSpeed);
        }
    }

    // ===================================================== UNKNOWN
    using qstmGesture::QStm_UnknownGestureRecogniser;

    if (conf.enabled(QStm_UnknownGestureRecogniser::KUid)) {
    	QStm_UnknownGestureRecogniser* gesture = NULL;
        initRecognizer(gesture, context, isNewCtx);
    }

    // ===========================================================
    if(isNewCtx) {
        for(int i = 0; i < qstmGesture::EStmGestureUid_Count; ++i) {
        	qstmGesture::QStm_GestureRecogniserIf* rec = context.m_recognizers[i];
            if(rec) {
                rec->enableLogging(context.m_logging & (1<<i)) ;
            }
        }
    }


    // HOLD AREA
    qstmGesture::QStm_GestureArea& holdArea = *conf.area(qstmGesture::EHoldArea);
    int holdAreaSizeInMm = holdArea.m_size.width();
    m_statemachine->setHoldAreaShape  ( qstmUiEventEngine::QStm_AreaShape(holdArea.m_shape) );
    m_statemachine->setHoldArea       ( !filter ? 0 : holdAreaSizeInMm );
    m_statemachine->setHoldTimeout    ( !filter ? 0 : holdArea.m_timeout);

    // TOUCH-TIME AREA
    qstmGesture::QStm_GestureArea& tTimeArea = *conf.area(qstmGesture::ETouchTimeArea);
    int tTimeAreaSizeInMm = tTimeArea.m_size.width();
    // NB: shape <-- TouchAreaShape
    m_statemachine->setTouchTimeArea      ( !filter ? 0 : tTimeAreaSizeInMm );

    // Timeouts, Adjustments, etc.
    m_statemachine->setTouchSuppressTimeout(!filter ? 0 : conf.param(qstmGesture::ESuppressTimeout)*1000) ;
    m_statemachine->setMoveSuppressTimeout(!filter ? 0 : conf.param(qstmGesture::EMoveSuppressTimeout)*1000) ;
    m_statemachine->enableCapacitiveUp    (!filter ? 0 : conf.param(qstmGesture::ECapacitiveUpUsed));
    m_statemachine->enableYadjustment     (!filter ? 0 : conf.param(qstmGesture::EAdjustYPos));
    m_statemachine->enableLogging(context.m_logging);
}

// =============================================================

QStm_GestureContext::QStm_GestureContext(QStm_GestureEngineApi& engine ) : 
		             m_engine(engine),
		             m_isActivated(false),
		             m_ownerControl(NULL),
		             m_config(NULL)
		            		 
{
	init();
}

void QStm_GestureContext::init()
{
    m_engine.m_contexts.append(this);
    m_config = new QStm_GestureParameters();
    m_engine.getStateMachine()->addUiEventObserver(this);
    
    for(int i = 0; i < qstmGesture::EStmGestureUid_Count; ++i) {
        m_recognizers[i] = NULL;
    }
    
}

QStm_GestureContext::~QStm_GestureContext()
{
    // Remove all gesture listeners
    for(int i = m_listeners.count() - 1; i >= 0; --i) {
    	QStm_GestureListenerApiIf* listener = m_listeners[i];
        m_listeners.removeAt(i);
        listener->handleGestureListenerRemoved();
    }

    m_listeners.clear();

    // Remove all context's recognizers from the Gesture Engine
    deactivate();

    // Destroy all recognizers
    for(int r = 0; r < qstmGesture::EStmGestureUid_Count; ++r) {
    	qstmGesture::QStm_GestureRecogniserIf*& rec = m_recognizers[r];
        if(rec) {
            delete rec;
            rec = NULL;
        }
    }

    // Remove context from the Gesture Engine
    int idx = m_engine.m_contexts.indexOf(this);
    if(idx != -1) {
        m_engine.m_contexts.removeAt(idx);
    }

    delete m_config;
}

void QStm_GestureContext::addListener(QStm_GestureListenerApiIf* listener, int pos)
{
    m_listeners.insert(pos, listener);
}

int QStm_GestureContext::removeListener(QStm_GestureListenerApiIf* listener )
{
    int ind = m_listeners.indexOf(listener);
    if(ind >= 0) {
        m_listeners.removeAt(ind);
    }
    return ind;
}

void QStm_GestureContext::setContext(void* control)
{
    m_ownerControl = control;

    for(int i = 0; i < qstmGesture::EStmGestureUid_Count; ++i) {
    	QStm_GestureRecogniserIf* rec = m_recognizers[i];
        if(rec) {
            rec->setOwner(control);
        }
    }
}

void QStm_GestureContext::activate(void* control)
{
	setContext(control);
	activate();
}

void QStm_GestureContext::activate()
{
    if(isActive()) {
        // re-enable all recognizers
        enableRecognizers();
        return;
    }

    // 1. Tell Engine to deactivate active context

    // 2. Setup all gesture recognizers
    m_engine.enableContext(*this);

    // 3. Notify listeners
    enableRecognizers();

    //
    m_isActivated = true;
}

void QStm_GestureContext::deactivate()
{
    m_engine.disableContext(*this);
    m_isActivated = false;    
}

void QStm_GestureContext::gestureEnter(QStm_GestureIf& gesture )
{
	dispatchGestureEvent(gesture.gestureUid(), &gesture);
}

void QStm_GestureContext::gestureExit(QStm_GestureIf& gesture )
{
    dispatchGestureEvent(gesture.gestureUid(), NULL);
}

void* QStm_GestureContext::getOwner()
{
    return m_ownerControl;
}

QStm_Gesture* QStm_GestureContext::createQStmGesture(QStm_GestureUid uid,
                                                           QStm_GestureIf* gesture)
{
    QStm_Gesture* gest = new QStm_Gesture();
    int stmGestType = gesture ? gesture->getType() : -1;
    
    switch(uid) {
        case qstmGesture::EGestureUidTap:
        {
            qstmGesture::QStm_TapType type = qstmGesture::QStm_TapType(stmGestType);
            if (type == qstmGesture::ETapTypeDouble) {
                gest->setGestureStmType(QStmDoubleTapGestureType);
            }
            else  {
                gest->setGestureStmType(QStmTapGestureType);
            }
            break;    
        }
        
        case qstmGesture::EGestureUidTouch:
        {
            gest->setGestureStmType(QStmTouchGestureType);
            break;    
        }
        
        case qstmGesture::EGestureUidRelease:
        {
            gest->setGestureStmType(QStmReleaseGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidFlick:
        {
            gest->setGestureStmType(QStmFlickGestureType);
            break;    
        }
        
        case qstmGesture::EGestureUidLeftRight:
        {
            gest->setGestureStmType(QStmLeftRightGestureType);
            break;    
        }
        
        case qstmGesture::EGestureUidUpDown:
        {
            gest->setGestureStmType(QStmUpDownGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidPan:
        {
            gest->setGestureStmType(QStmPanGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidHover:
        {
            gest->setGestureStmType(QStmHoverGestureType);
            break;
        }

        case qstmGesture::EGestureUidLongPress:
        {
            gest->setGestureStmType(QStmLongPressGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidEdgeScroll:
        {
            gest->setGestureStmType(QStmEdgeScrollGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidCornerZoom:
        {
            gest->setGestureStmType(QStmCornerZoomGestureType);
            break;
        }
        
        case qstmGesture::EGestureUidPinch:
        {
            gest->setGestureStmType(QStmPinchGestureType);
            break;
        }
    }
    
    if (gesture) { //gesture enter
        gest->setGestureSubType(gesture->getType());
        gest->setDirection(gesture->getDirection());
        QPoint vec = gesture->getLengthAndDirection();
        gest->setLengthAndDirection(vec);
        gest->setSpeed(gesture->getSpeed());
        QPoint pos = gesture->getLocation();
        gest->setPosition(pos);
        gest->setDetails(gesture->getDetails());
        gest->setSpeedVec(gesture->getSpeedVec());
        QWidget* w = static_cast<QWidget*>(gesture->target());
        gest->setTarget(w);
    }
    else {
    	gest->setGestureState(Qt::GestureFinished);
    }
    
    return gest;
}

void QStm_GestureContext::dispatchGestureEvent(qstmGesture::QStm_GestureUid uid,
                                               qstmGesture::QStm_GestureIf* gesture )
{
    bool consumed = false;
    bool suspended = false;
    
    
    
    for(int i = 0; i < m_listeners.count(); ++i) {
    	QStm_GestureListenerApiIf* listener = m_listeners[i];
        if(consumed) {
            listener->handleGestureListenerSuppressed(uid, gesture);
        }
        else {
        	QStm_GestureListenerApiIf::QStm_ProcessingResult res = listener->handleGestureEvent(uid, gesture);
            if(res & QStm_GestureListenerApiIf::EConsume) {
                consumed = true; // TODO: maybe we should notify all previous listeners as well...
            }

            suspended |= (res & QStm_GestureListenerApiIf::ESuspend) != 0; // TODO: notify listeners?
        }
    }
    if(suspended) {
        suspendRecognizer(uid);
    }
}

void QStm_GestureContext::suspendRecognizer(qstmGesture::QStm_GestureUid uid)
{
	qstmGesture::QStm_GestureRecogniserIf* rec = m_recognizers[uid];
    if(rec && !rec->isEnabled()) {
        rec->enable(false);
        // TODO: Notify listener
    }
}

void QStm_GestureContext::enableRecognizers()
{
    for(int i = 0; i < qstmGesture::EStmGestureUid_Count; ++i) {
        enableRecognizer(qstmGesture::QStm_GestureUid(i));
    }
}

void QStm_GestureContext::enableRecognizer(qstmGesture::QStm_GestureUid uid )
{
	qstmGesture::QStm_GestureRecogniserIf* rec = m_recognizers[uid];
    if(rec && !rec->isEnabled()) {
        rec->enable(true);
        // TODO: Notify listener
    }
}


void QStm_GestureContext::handleUiEvent( const qstmUiEventEngine::QStm_UiEventIf& event )
{
    emit(uiEvent(event));	
}


bool QStm_GestureContext::handleSymbianPlatformEvent(const QSymbianEvent* platEvent) 
{ 
    return	m_engine.getStateMachine()->handleSymbianPlatformEvent(platEvent); 
}


bool QStm_GestureContext::handleX11PlatformEvent(const XEvent* platEvent)
{
	return	m_engine.getStateMachine()->handleX11PlatformEvent(platEvent);
}



