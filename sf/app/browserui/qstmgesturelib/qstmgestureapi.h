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

#ifndef QSTMGESTUREAPI_H__
#define QSTMGESTUREAPI_H__

#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>
#include <qstmuievent_if.h>
#include <QGestureRecognizer>
#include <qwindowdefs.h>

#if !defined(Q_WS_X11)
#define XEvent void
#endif

namespace qstmGesture
{
    class QStm_GestureRecogniserIf;
    class QStm_GestureEngine;
}


namespace qstmUiEventEngine
{
    class QStm_StateMachine;
}

class QStm_GestureContext;
class QGestureEvent;
class QSymbianEvent;
class QStm_Gesture;

class QStm_GestureListenerApiIf
{
public:
    enum QStm_ProcessingResult
        {
        // Normally listeners return this code
        EContinue   = 0,
        // Prevents any other listeners to get this event
        EConsume    = 1,
        // continue and disable recognizer till explicitly re-enabled
        ESuspend    = 1<<1,
        };

    enum QStm_GestureContextEvent
        {
        EContextActivated,
        EContextDeactivated
        };

    // Gesture handling callback.
    // @param aUid  Type of gesture (actually, UID of gesture recognizer implementation)
    // @param aGesture Geture data; if it's NULL then event is Exit (end of gesture)
    // @note Some gestures issue more then one calls and then, typically Exit call.
    virtual QStm_ProcessingResult handleGestureEvent(qstmGesture::QStm_GestureUid /*aUid*/, qstmGesture::QStm_GestureIf* /*aGesture*/)
    {
        return QStm_GestureListenerApiIf::EContinue;
    }

    // This method is called instead of HandleGestureEventL if prevous listener locked itself
    // onto gesture handling by returning TProcessingResult::EConsume.
    // @see HandleGestureEventL
    // Default implementation does nothing.
    virtual void handleGestureListenerSuppressed(qstmGesture::QStm_GestureUid /*aUid*/, qstmGesture::QStm_GestureIf* /*aGesture*/)
    {

    }

    // Called by framework when listener is removed from a listener set.
    // It can be used for application-specific action, i.e. desturction of the listener.
    // Default implementation does nothing.
    virtual void handleGestureContextEvent(QStm_GestureContextEvent /*aEvent*/)
    {

    }

    // Called by framework when listener is removed from a listener set.
    // It can be used for application-specific action, i.e. destruction of the listener.
    // Default implementation does nothing.
    virtual void handleGestureListenerRemoved()
    {

    }
};







//==================================================================

class QStm_GestureParametersApiIf
{
public:
    virtual qstmGesture::QStm_GestureArea* area(qstmGesture::QStm_GestureAreaSettings aArea) = 0;
    virtual int param(qstmGesture::QStm_GestureSettings aParam) const = 0;
    virtual uint enabled(qstmGesture::QStm_GestureUid gestureUid) const = 0;
};

//==================================================================

class QStm_GestureParameters: public QObject, public QStm_GestureParametersApiIf
{
public:

    QStm_GestureParameters();

    QStm_GestureParameters(const QStm_GestureParameters& aParams);

public: // getters from QStm_GestureParametersApiIf

    qstmGesture::QStm_GestureArea* area(qstmGesture::QStm_GestureAreaSettings aArea)
    {
        return &m_areaSettings[aArea];
    }

    int param(qstmGesture::QStm_GestureSettings param) const
    {
        return m_param[param];
    }

    uint enabled(qstmGesture::QStm_GestureUid gestureUid) const
        {
        return m_enabled[gestureUid] != 0;
        }

    void setParam(qstmGesture::QStm_GestureSettings param, int value)
    {
        m_param[param] = value;
    }

    void setEnabled(qstmGesture::QStm_GestureUid gestureUid, uint param)
    {
        m_enabled[gestureUid] = param;
    }

    int& operator[](qstmGesture::QStm_GestureSettings param)
    {
        return m_param[param];
    }

private:
    int m_param[qstmGesture::EGestureSettingsCount];
    qstmGesture::QStm_GestureArea m_areaSettings[qstmGesture::EGestureAreaSettingsCount];
    int m_modified;
    uint m_enabled[qstmGesture::EStmGestureUid_Count];
};

//==================================================================

class QStm_GestureEngineApi : public QObject
{
	Q_OBJECT
public:
    ~QStm_GestureEngineApi();

    QSTMGESTURELIB_EXPORT QStm_GestureContext* createContext(int aId);

    QStm_GestureContext* context(int aId);

    QSTMGESTURELIB_EXPORT QStm_GestureEngineApi();

    qstmGesture::QStm_GestureEngine* getGestureEngine() { return m_gestureEngine; }
    qstmUiEventEngine::QStm_StateMachine* getStateMachine() { return m_statemachine; }


private: // used by friend class QStm_GestureContext
    void init();
    void enableContext(QStm_GestureContext& ctx);

    void disableContext(QStm_GestureContext& ctx);

    template<class T>
    void initRecognizer(
        T*& gesture,
        QStm_GestureContext& ctx,
        bool isNewCtx);

    void setupRecognizers(QStm_GestureContext& ctx, bool isNewCtx);

private:
    friend class QStm_GestureContext;

    // Registered/owned contexts
    QList<QStm_GestureContext*> m_contexts;
    // Active set of contexts; latest added defines parameters
    QList<QStm_GestureContext*> m_ctxtStack;

    // Global/default parameters
    QStm_GestureParameters* m_config;
    bool m_dtorPhase;
    
    qstmGesture::QStm_GestureEngine* m_gestureEngine;
    qstmUiEventEngine::QStm_StateMachine* m_statemachine;
};


// Gesture context represents a certain configuration of gesture parameters
// (including what gestures are enabled) and gesture listeners.
// Context can be created beforehand and reused later.
// The assumed use of a context is to have rough correspondence between
// UI view and enabled gestures.
//
// @note Gesture contexts are created by an instance of QStm_GestureEngineApi.
class QStm_GestureContext: public QObject, 
                           public qstmGesture::QStm_GestureListenerIf,
                           public qstmUiEventEngine::QStm_UiEventObserverIf
{
    Q_OBJECT
public:
    QSTMGESTURELIB_EXPORT QStm_GestureContext(QStm_GestureEngineApi& engine);

    QSTMGESTURELIB_EXPORT ~QStm_GestureContext();

public: // Gesture listeners

    // Add listener to the gesture processing context.
    // @param aListener Listener object
    // @param aPos Position of inserted listener which determines order
    //     in which it receives gesture events (0 == first, before existing)
    QSTMGESTURELIB_EXPORT void addListener(QStm_GestureListenerApiIf* listener, int pos = 0);

    QSTMGESTURELIB_EXPORT int removeListener(QStm_GestureListenerApiIf* listener);

    const QList<QStm_GestureListenerApiIf*>& listeners() const { return m_listeners; }

    QSTMGESTURELIB_EXPORT QStm_GestureParameters& config() { return *m_config; }

    QSTMGESTURELIB_EXPORT void setContext(void* control);

    QSTMGESTURELIB_EXPORT void setLogging(int enabled) { m_logging = enabled; }

    QSTMGESTURELIB_EXPORT void* contextControl() { return m_ownerControl; }

    QSTMGESTURELIB_EXPORT void activate();
    
    QSTMGESTURELIB_EXPORT void activate(void* control);

    QSTMGESTURELIB_EXPORT bool isActive() { return m_isActivated; }

    QSTMGESTURELIB_EXPORT void deactivate();

    QSTMGESTURELIB_EXPORT int id() const { return m_ctxtId; }

    // TODO: hide implementation, check boundaries
    QSTMGESTURELIB_EXPORT qstmGesture::QStm_GestureRecogniserIf* recognizer(qstmGesture::QStm_GestureUid aUid)
    {
        return m_recognizers[aUid];
    }

// from qstmGesture::QStm_GestureListenerIf

    void gestureEnter(qstmGesture::QStm_GestureIf& aGesture);

    void gestureExit(qstmGesture::QStm_GestureIf& aGesture);

    QSTMGESTURELIB_EXPORT void* getOwner();

    
//from qstmUiEventEngine::QStm_UiEventObserverIf    
    QSTMGESTURELIB_EXPORT void handleUiEvent( const qstmUiEventEngine::QStm_UiEventIf& event );
    
    QSTMGESTURELIB_EXPORT bool handleSymbianPlatformEvent(const QSymbianEvent* platEvent); 
    
    QSTMGESTURELIB_EXPORT bool handleX11PlatformEvent(const XEvent* platEvent);
    
    
signals:
    void uiEvent(const qstmUiEventEngine::QStm_UiEventIf& event);
    
private:
    friend class QStm_GestureEngineApi;

    QStm_Gesture* createQStmGesture(qstmGesture::QStm_GestureUid uid,
                                    qstmGesture::QStm_GestureIf* gesture);
    void init();

    void dispatchGestureEvent(qstmGesture::QStm_GestureUid aUid, qstmGesture::QStm_GestureIf* aGesture);
    
    void suspendRecognizer(qstmGesture::QStm_GestureUid aUid);
    void enableRecognizers();
    void enableRecognizer(qstmGesture::QStm_GestureUid aUid);

private:
    QStm_GestureEngineApi& m_engine;
    // Application-specific ID for finding context
    int    m_ctxtId;
    bool   m_isActivated;
    QStm_GestureParameters* m_config;
    void*  m_ownerControl;

    //TBool iPinchUsed;
    QList<QStm_GestureListenerApiIf*> m_listeners;
    // for simplicity of addressing recognizers, we'll use fixed array
    // with UID-based index (range 0..N is assumed for UIDs)
    // @note Tap recognizer actually produces Tap and DoubleTap gesture,
    //       so should be handled carefully -- only Tap position is set
    qstmGesture::QStm_GestureRecogniserIf* m_recognizers[qstmGesture::EStmGestureUid_Count];
    int m_logging;
};


#endif  // QSTMGESTUREAPI_H__
