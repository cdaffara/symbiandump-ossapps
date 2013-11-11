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
#include "qstmpangesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmuievent_if.h"

#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_PanGestureRecogniser::QStm_PanGestureRecogniser(QStm_GestureListenerIf* listener) : 
		                         QStm_GestureRecogniser(listener)
{
}

QStm_PanGestureRecogniser::~QStm_PanGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_PanGestureRecogniser::recognise(int numOfActiveStreams,
                                    QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    
    if (numOfActiveStreams == 1) {   
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;
        
        if (countOfEvents > 1) { // do we have more than one event in the stream?
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then we pan if the speed is OK
            if (m_loggingenabled) {
            	LOGARG("QStm_PanGestureRecogniser: Got: numer of events %d, event code %d", countOfEvents, eventCode);
            }
        	
            if (puie->target() == m_powner &&
                eventCode == qstmUiEventEngine::EMove) { // The last one is move in our window

                float speed = puie->speed();
                if (m_loggingenabled) {
                    LOGARG("QStm_PanGestureRecogniser: Pan: num %d code %d, speed %f, pos(%d, %d), prev pos(%d, %d)", 
                            countOfEvents, eventCode, speed, puie->currentXY().x(), puie->currentXY().y(), 
                            puie->previousXY().x(), puie->previousXY().y());
                }
                
                // It is pan gesture in our window, handle it, if the speed is inside limits
                if (speed > m_panningspeedlow && speed < m_panningspeedhigh) {
                    using qstmUiEventEngine::QStm_UiEventSpeed;

                    state = EGestureActive;
                    QStm_UiEventSpeed speedIf(speed, puie->speedVec());
                    // Panning gesture
                    qstmGesture::QStm_DirectionalGesture pgest(
                                    KUid,
                                    puie->currentXY(),
                                    puie->previousXY(),
                                    &speedIf,
                                    m_loggingenabled);
                    pgest.setTarget(puie->target());

                    // Call the listener to inform that a Pan has occurred...
                    m_listener->gestureEnter(pgest);
                }
            }
            else if (eventCode == qstmUiEventEngine::ERelease) {
            	LOGARG("QStm_PanGestureRecogniser::recognise: (0x%x) eventCode == ERelease", this);
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_PanGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
	if (m_loggingenabled) {
	    LOGARG("QStm_PanGestureRecogniser: release (0x%x)", this);	
	}
	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    using qstmUiEventEngine::QStm_UiEventSpeed;
    QStm_UiEventSpeed speedIf(puie->speed(), puie->speedVec());
    qstmGesture::QStm_DirectionalGesture pgest(
                    KUid,
                    puie->currentXY(),
                    puie->previousXY(),
                    &speedIf,
                    m_loggingenabled);
	pgest.setTarget(puie->target());
    m_listener->gestureExit(pgest) ;
    m_state = ENotMyGesture;
}

void QStm_PanGestureRecogniser::setPanningSpeedLow(float aSpeed)/* __SOFTFP */
{
    m_panningspeedlow = aSpeed ;
}

void QStm_PanGestureRecogniser::setPanningSpeedHigh(float aSpeed) /*__SOFTFP */
{
    m_panningspeedhigh = aSpeed ;
}

