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

#include "qstmleftrightgesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmuievent_if.h"
#include "qstmutils.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;

QStm_LeftrightGestureRecogniser::QStm_LeftrightGestureRecogniser(QStm_GestureListenerIf* listener) : 
		                         QStm_GestureRecogniser(listener)
{
}

QStm_LeftrightGestureRecogniser::~QStm_LeftrightGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_LeftrightGestureRecogniser::recognise(int numOfActiveStreams,
                                  QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like a tap or double tap
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be tap and release
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;
        if (countOfEvents > 1) { // do we have more than one event in the stream?
            // Then look at the events to see if they are suitable for us
            // should we check that all of the events are targeted to our window?
            // currently we only check if the last one is for us and is EMove, then check if |x| > |y|
            if (puie->target() == m_powner &&
                    eventCode == qstmUiEventEngine::EMove) {// The last one is move in our window

                if (m_loggingenabled) {
                    LOGARG("QStm_LeftrightGestureRecogniser: Leftright: num %d code %d", countOfEvents, eventCode);
                }
                // Is it leftright gesture in our window?
                const QPoint& p = puie->currentXY();
                QPoint dp = p - puie->previousXY();
                if (ABS(dp.x()) > ABS(dp.y())) {
                    state = EGestureActive;
                    qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, dp.x(), puie) ;
                    pgest.setTarget(puie->target());
                    pgest.setName(QString("Leftlight")) ;
                    // Call the listener to inform that a Leftright has occurred...
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_LeftrightGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    using qstmUiEventEngine::QStm_UiEventSpeed;
    const QPoint& p = puie->currentXY();
    QPoint dp = p - puie->previousXY();
    qstmGesture::QStm_GenericSimpleGesture pgest(KUid, p, dp.x(), puie) ;
    pgest.setTarget(puie->target());
    m_listener->gestureExit(pgest) ;
    m_state = ENotMyGesture;
}

