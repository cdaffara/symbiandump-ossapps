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
#include "qstmunknowngesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_UnknownGestureRecogniser::QStm_UnknownGestureRecogniser(QStm_GestureListenerIf* listener) :
                                QStm_GestureRecogniser(listener)
{
}


QStm_UnknownGestureRecogniser::~QStm_UnknownGestureRecogniser()
{
}


QStm_GestureRecognitionState QStm_UnknownGestureRecogniser::recognise(int numOfActiveStreams,
                                                      QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;


    // Look at the events to see if it looks like long press with one pointer
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be EHold
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();

        if (m_loggingenabled) {
            LOGARG("QStm_UnknownGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
//        if (puie->target() == m_powner && eventCode == qstmUiEventEngine::ERelease) {// The last one is ERelease

            const QPoint& currentXY = puie->currentXY() ;
            if (m_loggingenabled) {
                LOGARG("QStm_UnknownGestureRecogniser: (%d, %d) ", currentXY.x(), currentXY.y()) ;
            }
            //state = EGestureActive ;
            // issue the gesture
            qstmGesture::QStm_GenericSimpleGesture pgest(KUid, currentXY, 0, puie) ;
            pgest.setTarget(puie->target());
            if (puie->target() != m_powner) {
                pgest.setDetails((void*)puie);
            }
            // Call the listener to inform that a gesture has occurred...
            m_listener->gestureEnter(pgest) ;
//        }
    }
    m_state = state;
    return state;
}

void QStm_UnknownGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    if (m_loggingenabled) {
    	LOGARG("QStm_UnknownGestureRecogniser: 0x%x release", this);
    }
    m_state = ENotMyGesture;
}

