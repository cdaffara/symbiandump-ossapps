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
#include "qstmuievent.h"

#include "qstmfilelogger.h"

using namespace qstmUiEventEngine;


QStm_UiEventSender::QStm_UiEventSender()
{
    m_loggingenabled = false ;
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        m_events[i] = NULL ;
    }
}

QStm_UiEventSender::~QStm_UiEventSender()
{
    // remove the possible events from the buffers if release was missed
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++)
    {
        if (m_events[i] != NULL) delete m_events[i] ;
    }
    m_observers.clear();
}

/*!
 * Add new UI event to the list or send it directly to the observers
 * depending on the m_directsending flag.
 * \param aUiEvent the new UI event to be sent to the observers.
 */
int QStm_UiEventSender::addEvent(QStm_UiEvent* uiEvent)
{
    int pointerIndex = uiEvent->index() ;
    // Store the new UI event.  Check what kind of event it is and compress the set of events
    // stored so far if possible
    compressStack(uiEvent) ;
    uiEvent->setPrevious(m_events[pointerIndex]) ;
    m_events[pointerIndex] = uiEvent ;  // Store the new event
    // TESTING: logging the speed calculations
    if (m_loggingenabled)
    {
        uiEvent->logSpeed() ;
    }
    emitEvent(*uiEvent) ;

    if (m_loggingenabled)
    {
        LOGARG("Sent event: %s: (ptr %d) (%d,%d)", qstmUiEventEngine::event_name(uiEvent->code()), pointerIndex, 
        		uiEvent->currentXY().x(), uiEvent->currentXY().y());
    }
    // If this was release event, then the chain can be removed
    if (uiEvent->code() == qstmUiEventEngine::ERelease)
    {
        delete uiEvent;    // This will delete the whole chain
        m_events[pointerIndex] = NULL ;
    }
    return 0;
}
/*!
 * Call each observer with the event
 */
void QStm_UiEventSender::emitEvent(const QStm_UiEvent& event)
{
	int count = m_observers.count();
    for (int i = 0; i < count; i++)
    {
        m_observers[i]->handleUiEvent(event);
    }
}

/*!
 * Add a new observer.  Note that current implementation is very rude:
 * max 5 observers in a simple array.
 */
bool QStm_UiEventSender::addObserver(QStm_UiEventObserverIf* observer )
{
    m_observers.append(observer) ;
    return true ;
}

// Check if the parameter refers to one of our observers
bool QStm_UiEventSender::isObserver(void* aObserver )
{
    QObject* pccToCheck = (QObject*) aObserver ;
    int count = m_observers.count();
    for (int i = 0; i < count; i++)
    {
        try
        {
            QObject* pcc = dynamic_cast<QObject*>(m_observers[i]) ;
            if (pcc == pccToCheck)
                return true ;
        }
        catch(...)
        {

        }
    }
    return false ; // Could not find observer
}

/*
 * remove observer from list
 */
bool QStm_UiEventSender::removeObserver(QStm_UiEventObserverIf* observer )
{
    int i = m_observers.indexOf(observer) ;
    if (i != -1)
    {
        m_observers.removeAt(i) ;
        return true ;

    }
    return false ; // Could not find observer
}


void QStm_UiEventSender::compressStack(QStm_UiEvent* uiEvent)
{
    int pointerIndex = uiEvent->index() ;
    QStm_UiEvent*& top = m_events[pointerIndex] ;
    if(!top)
    {
        return;
    }
    if (uiEvent->code() == qstmUiEventEngine::EHold)
    {
        // assumption: in case of hold, we can discard all previous messages
        delete top ;
        top = NULL ;
    }
    else
    {
        // Check if there would too many moves
        QStm_UiEvent* next = dynamic_cast<QStm_UiEvent*>(top->previousEvent()) ;
        if (next != 0 && next->code() == qstmUiEventEngine::EMove)
        {
            // leave only the topmost to the stack
            top->setPrevious(0) ;
            delete next ;
        }
    }
}

