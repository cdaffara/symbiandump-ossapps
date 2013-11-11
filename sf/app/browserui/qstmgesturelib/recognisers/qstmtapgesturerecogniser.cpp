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
#include "qstmtapgesturerecogniser.h"
#include "qstmuievent_if.h"
#include "qstmutils.h"
#include "qstmfilelogger.h"

using namespace qstmGesture ;
using namespace qstmUiEventEngine;

QStm_TapGestureRecogniser::QStm_TapGestureRecogniser(QStm_GestureListenerIf* listener) :
                                        QStm_GestureRecogniser(listener)
{
    m_powner = listener->getOwner() ;
    // if a listener is given here, then it is both tap and doubletap listener
    if (listener) {
        addTapListener(listener, m_powner) ;
        addDoubleTapListener(listener, m_powner) ;
    }
    m_waitingforsecondtap = false ;
    m_gestureEnabled = true ;
    m_ignorefirst = true ;  // by default ignore the first tap
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeoutCallback()));
}

QStm_TapGestureRecogniser::~QStm_TapGestureRecogniser()
{
    m_timer.stop();
    m_tapListeners.clear() ;
    m_tapListenerWindows.clear() ;
    m_doubleTapListeners.clear() ;
    m_doubleTapListenerWindows.clear() ;

}

QStm_GestureRecognitionState QStm_TapGestureRecogniser::recognise(int numOfActiveStreams,
                              QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = 
                            (m_waitingforsecondtap) ? EGestureActive :ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like a tap or double tap
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, it has to be tap and release
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;

        if (m_loggingenabled) {
            LOGARG("QStm_TapGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);
        }
        void* target = puie->target();
        
        if (countOfEvents == 2) { // Do we have touch and release in the stream, check if there are two events

            // Then look at the events to see if they are suitable for us
            if (target && eventCode == qstmUiEventEngine::ERelease) {// The last one is release

                qstmUiEventEngine::QStm_UiEventIf* puieFirst = puie->previousEvent();

                if (puieFirst != NULL && 
                    (eventCode = puieFirst->code()) == qstmUiEventEngine::ETouch && // is the first one ETouch
                    isPointClose(puie->currentXY(), puie->previousXY())) { // if system failed to deliver move event between 
                	                                                       // down and up we can get tap with two points
                	                                                       // too far from each other, so check for it

                    if (m_loggingenabled) {
                        LOGARG("QStm_TapGestureRecogniser: 0x%x TAP: num %d code %d", this, countOfEvents, eventCode);
                    }
                    // It is tap gesture in our window, handle it
                    state = EGestureActive;

                    

                    if (m_waitingforsecondtap) {
                        m_waitingforsecondtap = false ;
                        if (m_loggingenabled) {
                            LOGARG("QStm_TapGestureRecogniser: 0x%x second tap: num %d code %d", this, countOfEvents, eventCode);
                        }
                        
                        m_timer.stop();  // The timer

                        const QPoint& secondPoint = puieFirst->currentXY() ;
                        if (isPointClose(m_firstTapXY, secondPoint)) {
                            // Taps were close enough together, so issue a doubletap

                            // Call the listener of the current window to inform that a doubletap has occurred...
                            int inx = m_doubleTapListenerWindows.indexOf(target) ;
                            if (inx == -1) {
                                // the second tap hit a window with no listener,
                                // check if the first one has a listener
                                inx = m_doubleTapListenerWindows.indexOf(m_firstTapTarget) ;
                            }
                            // not found, check if the parent is in the listener list
                            if (inx == -1)
                            {
                                QObject* pc = static_cast<QObject*>(target) ;
                                while (pc) {
                                    pc = pc->parent() ;
                                    inx = m_doubleTapListenerWindows.indexOf(pc) ;
                                    if (inx != -1) break ;
                                }
                            }
                            if (inx != -1) {
                                // Tap gesture
                                qstmGesture::QStm_GenericSimpleGesture pgest(
                                    qstmGesture::EGestureUidDoubleTap, 
                                    secondPoint, 
                                    qstmGesture::ETapTypeDouble, 
                                    puie) ;
                                pgest.setTarget(puie->target());
                                QStm_GestureListenerIf* plistener = m_doubleTapListeners[inx] ;
                                plistener->gestureEnter(pgest) ;
                            }
                        }
                        else {
                            // Second tap is too far away, generate just tap
                            // and if configured, also the fist tap is generated
                            if (!m_ignorefirst) {
                                // do not ignore the first tap, so issue it now using the stored location
                                // Call the listener to inform that a Tap has occurred, if there was a listener in that window
                                int inx = m_tapListenerWindows.indexOf(m_firstTapTarget) ;
                                if (inx != -1) {   // check if the listener exists

                                    qstmGesture::QStm_GenericSimpleGesture pgest(
                                        qstmGesture::EGestureUidTap, 
                                        m_firstTapXY, 
                                        qstmGesture::ETapTypeSingle, 
                                        puieFirst) ; // TODO: speed is 0?
                                    pgest.setTarget(puie->target());
                                    QStm_GestureListenerIf* plistener = m_tapListeners[inx] ;
                                    plistener->gestureEnter(pgest) ;
                                }
                            }
                            // generate a tap at the current location, if there is a listener for it
                            int inx = m_tapListenerWindows.indexOf(target) ;
                            if (inx != -1) {
                                qstmGesture::QStm_GenericSimpleGesture pgest(
                                    qstmGesture::EGestureUidTap, 
                                    puie->currentXY(), 
                                    qstmGesture::ETapTypeSingle, puie) ; // TODO: speed is 0?
                                pgest.setTarget(puie->target());
                                QStm_GestureListenerIf* plistener = m_tapListeners[inx] ;
                                plistener->gestureEnter(pgest) ;
                            }
                        }
                    }
                    else {
                        m_firstTapXY = puieFirst->currentXY() ;
                        m_firstTapTarget = target ;
                        m_firstTapSpeed = puie->speed() ;
                        // This was the first tap, start the timer...
                        m_waitingforsecondtap = true ;
                        if (m_loggingenabled) {
                            LOGARG("QStm_TapGestureRecogniser: 0x%x first tap: num %d code %d", this, countOfEvents, eventCode);
                        }
                        m_timer.stop();
                        m_timer.setSingleShot(true);
                        m_timer.start(m_doubleTapTimeout) ;
                    }

                }
            }
            else if (eventCode == qstmUiEventEngine::EMove) {
                if (m_timer.isActive()) {
                    if (m_loggingenabled) {
                        LOGARG("QStm_TapGestureRecogniser: 0x%x: num %d code %d, Got EMove c- cancel timer.", this, countOfEvents, eventCode);
                    }
                    m_timer.stop();
                }
            }
        }
    }
    m_state = state;
    return state;
}

void QStm_TapGestureRecogniser::release(QStm_GestureEngineIf* /*ge*/)
{
    m_timer.stop() ;  // some other gesture took hold of the thing, do not send tap gesture
    m_waitingforsecondtap = false ;
    if (m_loggingenabled) {
        LOGARG("QStm_TapGestureRecogniser: 0x%x release, %d %d", this, m_firstTapXY.x(), m_firstTapXY.y());
    }
    m_state = ENotMyGesture;
}

void QStm_TapGestureRecogniser::timeoutCallback()
{
    m_waitingforsecondtap = false ;
    m_timer.stop();
    if (m_loggingenabled) {
        LOGARG("QStm_TapGestureRecogniser: 0x%x timer, %d %d", this, m_firstTapXY.x(), m_firstTapXY.y());
    }
    // Double tap timer has been elapsed without new Touch/Release, generate the tap if there is a listener
    int inx = m_tapListenerWindows.indexOf(m_firstTapTarget) ;
    if (inx != -1) {
        using qstmUiEventEngine::QStm_UiEventSpeed;

        QStm_UiEventSpeed speedIf(m_firstTapSpeed);

        qstmGesture::QStm_GenericSimpleGesture pgest(
                qstmGesture::EGestureUidTap,
                m_firstTapXY,
                qstmGesture::ETapTypeSingle,
                &speedIf) ;
        pgest.setTarget(m_firstTapTarget);

        QStm_GestureListenerIf* plistener = m_tapListeners[inx] ;
        plistener->gestureEnter(pgest) ;
    }
    m_state = ENotMyGesture;
}

void QStm_TapGestureRecogniser::enableLogging(bool loggingOn)
{
    m_loggingenabled = loggingOn;
}

void QStm_TapGestureRecogniser::setOwner(void* owner)
{
    m_powner = owner;
}

void QStm_TapGestureRecogniser::setDoubleTapTimeout(int newtimeout)
{
    m_doubleTapTimeout  = newtimeout;
}

void QStm_TapGestureRecogniser::enable(bool enabled)
{
    m_gestureEnabled = enabled ;
}

bool QStm_TapGestureRecogniser::isEnabled()
{
    return m_gestureEnabled ;
}

void QStm_TapGestureRecogniser::setDoubleTapRange(int rangeInMillimetres)
{
    m_rangesizeInPixels = QStm_Utils::mm2Pixels(rangeInMillimetres) ;
}

void QStm_TapGestureRecogniser::ignoreFirstTap(bool ignore)
{
    m_ignorefirst = ignore ;
}

/*!
 * Check whether the two taps are close enough to each other
 */
bool QStm_TapGestureRecogniser::isPointClose(const QPoint&  firstPoint, const QPoint& secondPoint)
{
    QRect tolerance = QStm_Utils::toleranceRectPx(secondPoint, m_rangesizeInPixels) ;
    bool aretheyclose = tolerance.contains(firstPoint);
    return aretheyclose ;
}

void QStm_TapGestureRecogniser::addTapListener(QStm_GestureListenerIf* listener, void* listenerOwner)
{
    m_tapListeners.append(listener) ;
    m_tapListenerWindows.append(listenerOwner) ;
}

void QStm_TapGestureRecogniser::removeTapListener(QStm_GestureListenerIf* listener, void* /*listenerOwner*/)
{
    int inx = m_tapListeners.indexOf(listener) ;
    if(inx != -1) {
        m_tapListeners.removeAt(inx) ;
        m_tapListenerWindows.removeAt(inx) ;
    }
}

void QStm_TapGestureRecogniser::addDoubleTapListener(QStm_GestureListenerIf* listener, void* listenerOwner)
{
    m_doubleTapListeners.append(listener) ;
    m_doubleTapListenerWindows.append(listenerOwner) ;
}

void QStm_TapGestureRecogniser::removeDoubleTapListener(QStm_GestureListenerIf* listener, void* /*listenerOwner*/)
{
    int inx = m_doubleTapListeners.indexOf(listener) ;
    if(inx != -1) {
        m_doubleTapListeners.removeAt(inx) ;
        m_doubleTapListenerWindows.removeAt(inx) ;
    }
}

