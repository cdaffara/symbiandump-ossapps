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

#include "qstmhoveringgesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_HoveringGestureRecogniser::QStm_HoveringGestureRecogniser(QStm_GestureListenerIf* listener) :
                                QStm_GestureRecogniser(listener)
{
    m_hovering = false ;
    m_hoveringspeed = 0.9f ;
}

QStm_HoveringGestureRecogniser::~QStm_HoveringGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_HoveringGestureRecogniser::recognise(int numOfActiveStreams,
                                                    QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like hovering
    if (numOfActiveStreams == 1)  {
        // Then look at the event stream, it has to be tap and release
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;
        if (countOfEvents > 1) {// do we have more than one event in the stream?
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then we pan...
            if (puie->target() == m_powner &&
                    eventCode == qstmUiEventEngine::EMove) {// The last one is move in our window
                float speed = puie->speed() ;
                if (m_loggingenabled) {
                    LOGARG("QStm_HoveringGestureRecogniser: %d: num %d code %d, speed %f, limit %f",
                            m_hovering, countOfEvents, eventCode, double(speed), double(m_hoveringspeed));
                }
                // It might be hovering gesture in our window, handle it
                if (!m_hovering) {
                    // we are not yet hovering, so lets see if it is slow movement
                    // but it must be movement; if it is 0.0 do not hover
                    if (speed > 0.01f && speed < m_hoveringspeed) {
                        state = EGestureActive;
                        m_hovering = true;
                    }
                }
                if (m_hovering) {
                    // after we've started hovering, the speed could be increased a little before we loose hovering
                    // but this adjustment is not implemented now...
                    if (speed < m_hoveringspeed) {
                        using qstmUiEventEngine::QStm_UiEventSpeed;

                        state = EGestureActive;
                        QStm_UiEventSpeed speedIf(speed);
                        qstmGesture::QStm_DirectionalGesture pgest(
                                        KUid,
                                        puie->currentXY(),
                                        puie->previousXY(),
                                        &speedIf,
                                        m_loggingenabled);

                        pgest.setTarget(puie->target());                        // Call the listener to inform that a Hover has occurred...
                        m_listener->gestureEnter(pgest);
                    }
                }
            }
            else if (m_hovering) {
                if (eventCode == qstmUiEventEngine::ERelease) {  // The last one is release in any window
                    m_hovering = false ;
                    // release will handle informing of the listener
                }
            }
        }
        else  {
            // count of events == 1, lets see if it is EMove, then we take it and start hovering
            if (puie->target() == m_powner &&
                    eventCode == qstmUiEventEngine::EMove) {// The only one is move in our window
                if (m_loggingenabled) {
                    LOGARG("QStm_HoveringGestureRecogniser: move: num %d code %d", countOfEvents, eventCode);
                }
                state = EGestureActive;
                qstmGesture::QStm_DirectionalGesture pgest(
                                        KUid,
                                        puie->currentXY(),
                                        puie->previousXY(),
                                        puie,
                                        m_loggingenabled);
                pgest.setTarget(puie->target());

                // Call the listener to inform that a Hover has occurred...
                m_listener->gestureEnter(pgest);
            }
        }
    }
    if (state == ENotMyGesture) {
        // if it was not our gesture, then the state can not be hovering...
        m_hovering = false ;
    }
    m_state = state;
    return state;
}

void QStm_HoveringGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
    m_hovering = false ;
	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    using qstmUiEventEngine::QStm_UiEventSpeed;
    qstmGesture::QStm_DirectionalGesture pgest(
                    KUid,
                    puie->currentXY(),
                    puie->previousXY(),
                    puie,
                    m_loggingenabled);
    pgest.setTarget(puie->target());
	
    m_listener->gestureExit(pgest) ;
    m_state = ENotMyGesture;
}

void QStm_HoveringGestureRecogniser::setHoveringSpeed(float aSpeed) /*__SOFTFP */
{
    m_hoveringspeed = aSpeed ;
}

