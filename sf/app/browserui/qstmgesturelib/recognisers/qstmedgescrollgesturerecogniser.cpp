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
#include "qstmedgescrollgesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"


using namespace qstmGesture ;

QStm_EdgeScrollGestureRecogniser::QStm_EdgeScrollGestureRecogniser(QStm_GestureListenerIf* listener) :
                                   QStm_GestureRecogniser(listener),
                                   m_listener(listener) , m_area()
{
    m_powner = listener->getOwner() ;
    m_gestureEnabled = true ;
    m_rangesizeInPixels = 10 ;  // by default 10 pixels from the edges is the area
}


QStm_EdgeScrollGestureRecogniser::~QStm_EdgeScrollGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_EdgeScrollGestureRecogniser::recognise(int numOfActiveStreams,
                                                                      QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like edge scroll with one pointer
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be EHold
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();

        if (m_loggingenabled) {
            LOGARG("QStm_EdgeScrollGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }

        if (eventCode == qstmUiEventEngine::EHold) { // The last one is EHold, look if it is near the area defined
            const QPoint& p = puie->currentXY();
            if (m_loggingenabled) {
                LOGARG("QStm_EdgeScrollGestureRecogniser: 0x%x EHold: num %d code %d, %d", 
                		this, countOfEvents, p.x(), p.y());
                LOGARG("QStm_EdgeScrollGestureRecogniser: area, %d,%d %d,%d, range: %d", 
                		m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height(), 
                		m_rangesizeInPixels);
            }
            // check where the point is inside the area defined but outside of the area shrunk by m_rangesizeInPixels.
            QRect rcInner(QPoint(0,0), m_area.size() - QSize(m_rangesizeInPixels, m_rangesizeInPixels));
            rcInner.moveCenter(m_area.center()) ;
            if (m_area.contains(p) && !rcInner.contains(p))
            {
                if (m_loggingenabled) {
                    LOGARG("QStm_EdgeScrollGestureRecogniser: HIT, (%d,%d) in %d,%d %d,%d, range: %d", 
                    		p.x(), p.y(), m_area.x(), m_area.y(), 
                    		m_area.x() + m_area.width(), m_area.y() + m_area.height(),
                    		m_rangesizeInPixels);
                }

                state = EGestureActive ;
                // this is edge scroll, check where it is about...
                QStm_EdgeScroll scrolltype = EEdgeScrollUnknown ;
                if (p.y() < m_area.y() + m_rangesizeInPixels)
                    scrolltype = EEdgeScrollUp ;   // if Y is small enough, it is always up
                else if (p.y() > m_area.y() + m_area.height() - m_rangesizeInPixels)
                    scrolltype = EEdgeScrollDown ; // if Y is big enough, it is always down
                else if (p.x() < m_area.x() + m_rangesizeInPixels)
                    scrolltype = EEdgeScrollLeft ; // if X is small enough, it is always left
                else  if (p.x() > m_area.x() + m_area.width() - m_rangesizeInPixels)
                    scrolltype = EEdgeScrollRight ;   // if X is big enough, it is always right
                // issue the edge scroll gesture
                qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, scrolltype, puie) ;
                pgest.setTarget(puie->target());
                // Call the listener to inform that a Tap has occurred...
                m_listener->gestureEnter(pgest) ;
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_EdgeScrollGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_EdgeScrollGestureRecogniser: 0x%x release", this);
    }
    m_state = m_state = ENotMyGesture;
}


void QStm_EdgeScrollGestureRecogniser::enableLogging(bool loggingOn)
{
    m_loggingenabled = loggingOn;
}

void QStm_EdgeScrollGestureRecogniser::enable(bool enabled)
{
    m_gestureEnabled = enabled ;
}

bool QStm_EdgeScrollGestureRecogniser::isEnabled()
{
    return m_gestureEnabled ;
}

void QStm_EdgeScrollGestureRecogniser::setOwner(void* owner)
{
    m_powner = owner;
}

void QStm_EdgeScrollGestureRecogniser::setScrollRange(int rangeInPixels)
{
    m_rangesizeInPixels = rangeInPixels ;
}

void QStm_EdgeScrollGestureRecogniser::setArea(const QRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled)
    {
        LOGARG("QStm_EdgeScrollGestureRecogniser: set area, %d,%d %d,%d", 
        		m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
    }
}
