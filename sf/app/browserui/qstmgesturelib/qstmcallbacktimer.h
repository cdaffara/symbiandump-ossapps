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

#ifndef QSTMCALLBACKTIMER_H_
#define QSTMCALLBACKTIMER_H_



#include <QTimer>

namespace qstmUiEventEngine
{
/**
 * define the method to be called by the timer; this includes the pointer number
 * the timer is associated with (maybe some other solution could do this, but...)
 */
class QStm_StateMachine ;
typedef void (QStm_StateMachine::*CallbackFunction)(int );

#define INVOKE_CALLBACK(obj, funcptr) ((obj)->*(funcptr))
/**
 * CCallbackTimer to implement the timers.  This needs to be replaced
 * with something else if cross platform is needed.
 */
class QStm_CallbackTimer : public QObject
{
	Q_OBJECT
	
public:
    QStm_CallbackTimer(QStm_StateMachine* helper, CallbackFunction cbFunc,
                       int delay, int pointerNumber, bool isEnabled) :
                       m_helper(helper), m_callbackFunction(cbFunc),
                       m_delay(delay), m_isEnabled(isEnabled), m_pointerNumber(pointerNumber)
    {
        m_isTriggered = false;
        m_timer = new QTimer(this);
        m_timer->setSingleShot(true);
        connect(m_timer, SIGNAL(timeout()), this, SLOT(callback())); 
    }
    
    /*
    QStm_CallbackTimer(QStm_StateMachine* helper, const char*  slot,
                           int delay, int pointerNumber, bool isEnabled) :
                           m_helper(helper), m_delay(delay), m_isEnabled(isEnabled), 
                           m_pointerNumber(pointerNumber)
        {
            m_isTriggered = false;
            m_timer = new QTimer(this);
            connect(m_timer, SIGNAL(timeout()), m_helper, slot); 
        }    
    */
    
    /** Destructor */
    ~QStm_CallbackTimer()
    {
    	if (m_timer && m_timer->isActive())
    	{
            m_timer->stop();
            delete m_timer;
    	}
    }

    /** Set whether sending events is currently enabled */
    void setEnabled(bool enabled)
    {
        m_isEnabled = enabled;
        // cancel in case timer is already running
        m_timer->stop();
    }

    /** @return whether sending events is currently enabled */
    bool isEnabled() const
    {
        return m_isEnabled;
    }

    /** Start the timer. Calls callback upon completion.  It is possible to give temporary delay if needed,
     * otherwise use the defined delay.
     */
    void start(int newDelay = 0)
    {
        if (m_isEnabled)
        {
            m_timer->stop();
            m_timer->setSingleShot(true);
            if (newDelay != 0)
            {
            	m_timer->start(newDelay);
            }
            else
            {
            	m_timer->start(m_delay);
            }
        }
    }
    
    void setDelay(int delay) { m_delay = delay; }
    
    int getDelay() { return m_delay; }
    
    
    bool isTriggered() { return m_isTriggered; }
    
    bool isActive() { return m_timer->isActive(); }
    
    void stop() { m_timer->stop(); }

public slots: 
    void callback() 
    { 
    	
    	m_isTriggered = true;
    	INVOKE_CALLBACK(m_helper, m_callbackFunction)(m_pointerNumber);
    	//CallbackFunction f = m_callbackFunction;
    	//(m_helper->*f)(m_pointerNumber); 
    	m_isTriggered = false;
    	 
    }
    
private:
    QTimer*         m_timer;
    /// helper object that will be called back when timer is triggered
    QStm_StateMachine* m_helper;
    /// Function in the iHelper object call
    CallbackFunction  m_callbackFunction;
    /// How long a time to wait before calling back after Start()
    long m_delay;
    /// whether sending holding events is currently enabled
    bool m_isEnabled;
    /**
     * while we are inside RunL iIsTriggered = ETrue
     */
    bool m_isTriggered;
    /*
     * in multitouch now we need timers for each pointer
     * could this be handled in some other way?
     */
    int m_pointerNumber ;
};
} // namespace



#endif /* QSTMCALLBACKTIMER_H_ */
