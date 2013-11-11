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

#ifndef QSTMSTATEMACHINE_H_
#define QSTMSTATEMACHINE_H_




#include <qstmuievent_if.h>
#include "qstmtimerinterface.h"
#include "qstmstatemachine_v2.h"

#include <qwindowdefs.h>

class QSymbianEvent;

#if !defined(Q_WS_X11)
#define XEvent void
#endif

namespace qstmUiEventEngine
{

class QStm_CallbackTimer;
class QStm_StateEngine;
class QStm_HwEvent;
class QStm_StateEngineConfiguration;


class QStm_StateMachine : public QObject, public QStm_TimerInterfaceIf, public QStm_StateMachineIf
{
	Q_OBJECT
public:
    virtual QRect getTouchArea(int pointerNumber) ;
    virtual void setTouchTimeArea(long fingersize_mm) ;
    virtual void setTouchArea(long fingersize_mm) ;
    virtual QStm_AreaShape getTouchAreaShape() ;
    virtual void setTouchAreaShape(const QStm_AreaShape shape) ;
    virtual unsigned int getTouchTimeout() ;
    virtual void setTouchTimeout(unsigned int) ;
    virtual QRect getHoldArea(int pointerNumber) ;
    virtual void setHoldArea(long fingersize_mm) ;
    virtual QStm_AreaShape getHoldAreaShape() ;
    virtual void setHoldAreaShape(const QStm_AreaShape shape) ;
    virtual unsigned int getHoldTimeout();
    virtual void setHoldTimeout(unsigned int a);
    virtual unsigned int getTouchSuppressTimeout() ;
    virtual void setTouchSuppressTimeout(unsigned int a) ;
    virtual unsigned int getMoveSuppressTimeout() ;
    virtual void setMoveSuppressTimeout(unsigned int a) ;
    virtual bool addUiEventObserver(QStm_UiEventObserverIf* observer) ;
    virtual bool removeUiEventObserver(QStm_UiEventObserverIf* observer) ;
    virtual bool wasLastMessageFiltered(int pointerNumber) ;
    virtual void enableCapacitiveUp(bool enable) ;
    virtual void enableLogging(bool aEnable) ;
    virtual void enableYadjustment(bool aEnable) ;
    virtual int getNumberOfPointers() ; 
    
    /*
    virtual void setPointerBuffer(TPoint* aBufferPtr, int aBufSize) { m_pointBuffer = aBufferPtr; m_pointBufferSize = aBufSize; }
    
    virtual TPoint* getPointerBuffer() { return m_pointBuffer; }
    
    virtual int getPointerBufferSize() { return m_pointBufferSize; }
    */
    virtual void setMoveTolerance(long fingersize_mm);
    
    QPoint getMoveTolerance();


    bool handleSymbianPlatformEvent(const QSymbianEvent* platEvent);     

    bool handleX11PlatformEvent(const XEvent* platEvent);

    bool handleStateEvent(const QStm_PlatformPointerEvent& platPointerEvent) ;
    /*!
     * Setting the Y adjustment useful in capacitive touch
     * Note that there are problems with the adjustment if done at this level,
     * the most proper place would be the window server.
     */

    /**
     * MTimerInterface methods, these are internal and do not need to be exported
     */
    virtual void startTouchTimer(int delay, int pointerNumber) ;
    virtual void cancelTouchTimer(int pointerNumber)  ;
    virtual void startHoldTimer(int delay, int pointerNumber)  ;
    virtual void cancelHoldTimer(int pointerNumber)  ;
    virtual void startSuppressTimer(int delay, int pointerNumber) ;
    virtual void cancelSuppressTimer(int pointerNumber)  ;

    
    //void  dragEventFromPointBuffer();
    
public:
    ~QStm_StateMachine();
     QStm_StateMachine();
private:

    void init();

    inline /*static*/ int pointerIndex(const QStm_PlatformPointerEvent& platPointerEvent);

    /**
     * Convert pointer event into our own event format (including timestamp)
     */
    void createHwEvent(QStm_HwEvent& event, const QStm_PlatformPointerEvent& platPointerEvent, void* target, const QTime& aTime) ;
    void createTimerEvent(QStm_HwEvent& event, QStm_StateMachineEvent eventCode) ;

    
    //int  retrieveMovePointsBuffer(CCoeControl* aDestination);
    
    //void stopPointBuffering();
    
    /**
     * the actual state machines implementing the transitions etc.
     */
    QStm_StateEngine* m_impl[KMaxNumberOfPointers] ;
    /*
     * The configuration for state machines
     */
    QStm_StateEngineConfiguration* m_config ;
    /*!
     * convert window coordinates to screen coordinates.
     */
    QPoint screenCoordinates(const QPoint& pos, void* gestureTarget) ;

//    CCoeEnv* m_coeEnv;

    bool m_WasMessageFiltered ;
    bool m_wseventmonitoringenabled ;
    bool m_loggingenabled ;
    bool m_capacitiveup ;
    bool m_pointerBufferSupported;
    

    //TPoint*   m_pointBuffer;
    //int       m_pointBufferSize;
    //int       m_pointBufferRetrieved;
    //int       m_currentPointBuffIdx;
    //CCoeControl*  m_destination;
    //CPeriodic*  m_pointBufferTimer;
    //RArray<TPoint> m_pointBufferPoints;
    
    int m_3mminpixels ;
    bool m_adjustYposition ;
    // Use same naming scheme with the timers, and variables and methods
    // using macro expansion tricks (with multitouch support starts to look quite ugly):
#define DECLARE_TIMER(x) \
            void start##x##Timer(int aPointerNumber) ;\
            QStm_CallbackTimer* m_##x##Timer[KMaxNumberOfPointers] ;\
            void handle##x##Timer(int aPointerNumber) ;\
            void cancel##x##Timer(int aPointerNumber)

//public slots:    
    DECLARE_TIMER(touch) ;
    DECLARE_TIMER(hold) ;
    DECLARE_TIMER(suppress) ;
    
};
}   // namespace


#endif /* QSTMSTATEMACHINE_H_ */
