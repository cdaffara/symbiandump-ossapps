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
#include "qstmgenericsimplegesture.h"
#include "qstmreleasegesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_ReleaseGestureRecogniser::QStm_ReleaseGestureRecogniser(QStm_GestureListenerIf* listener) :
                                 QStm_GestureRecogniser(listener)
{
}

QStm_ReleaseGestureRecogniser::~QStm_ReleaseGestureRecogniser()
{
}

/*!
 * Release gesture recogniser.  Note that this one never owns the gesture, it just calls
 * the callback if it detects ERelease inside the area being watched.
 * There could be also check for the target window?
 */
QStm_GestureRecognitionState QStm_ReleaseGestureRecogniser::recognise(int numOfActiveStreams,
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
            LOGARG("QStm_ReleaseGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        if (eventCode == qstmUiEventEngine::ERelease) {
            if (m_loggingenabled) {
                LOGARG("QStm_ReleaseGestureRecogniser: 0x%x ERelease: num %d code %d, %d", 
                		this, countOfEvents, puie->currentXY().x(), puie->currentXY().y());
                LOGARG("QStm_ReleaseGestureRecogniser: area, %d,%d %d,%d", 
                		m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
            }
            bool produceGesture ;
            if(!m_area.isEmpty())  {
                produceGesture = m_area.contains(puie->currentXY()) ;
                if(produceGesture && m_loggingenabled) {
                    LOGARG("QStm_ReleaseGestureRecogniser: HIT area (%d,%d) in %d,%d %d,%d", 
                    		puie->currentXY().x(), puie->currentXY().y(), 
                    	    m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
                }
            }
            else {
                produceGesture = (m_powner == puie->target()) ;  // no area defined, touch detected in the window
            }
            if (produceGesture) {
                //state = EGestureActive ;
                // issue the release gesture using the GenericSimpleGesture
                qstmGesture::QStm_GenericSimpleGesture pgest(KUid, puie->currentXY());
                // Give the gesture a name
                pgest.setName(QString("Release")) ;
                pgest.setTarget(puie->target());
                // Call the listener to inform that a release has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_ReleaseGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
        LOGARG("QStm_ReleaseGestureRecogniser: 0x%x release", this);
    }
    m_state = ENotMyGesture;
}

void QStm_ReleaseGestureRecogniser::setArea(const QRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled) {
        LOGARG("QStm_ReleaseGestureRecogniser: area, %d,%d %d,%d", 
        		m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
    }
}

