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

#include "qstmpinchgesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include <qstmuievent_if.h>
#include <qstmfilelogger.h>
#include <qstmutils.h>


using namespace qstmGesture ;
using namespace qstmUiEventEngine;


QStm_PinchGestureRecogniser::QStm_PinchGestureRecogniser(QStm_GestureListenerIf* listener) : 
		                      QStm_GestureRecogniser(listener)
{
    m_pinching = false ;
    m_pinchingspeed = 3.5 ; // by default something suitable for capacitive
    m_holdseen = false ;
}

QStm_PinchGestureRecogniser::~QStm_PinchGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_PinchGestureRecogniser::recognise(int numOfActiveStreams,
                                QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    if (m_loggingenabled) {
        LOGARG("QStm_PinchGestureRecogniser: %d %d %d ", m_pinching, m_holdseen, numOfActiveStreams) ;
    }
#if !defined(ADVANCED_POINTER_EVENTS)
    // Look at the events to see if it looks like pinch in single touch
    // WARNING: this code is a hack : in single touch capacitive touch device (like Alvin with 52.50) it works so-and-so, 
    // because the pointer events were reported from the corners of the rectangle formed by two fingers pressing.
    // In resistive touch device like Tube or Ivalo the reported points are somewhere in the middle between the fingers
    // and jumping a lot, so it is very difficult to get it right.
    if (numOfActiveStreams == 1) {
        // Then look at the event stream, first we need to see a hold and then a fast jump
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code() ;

        if (countOfEvents > 0 ) {// how many events
            if (m_loggingenabled) {
                LOGARG("QStm_PinchGestureRecogniser: %d %d %d %d %d, m: %d b: %d", 
                		m_pinching, m_holdseen, numOfActiveStreams, countOfEvents, eventCode, int(m_m), int(m_b)) ;
            }

            if (m_pinching) {
                // We have entered pinching state, lets move one of the points unless it is a release
                if (eventCode == qstmUiEventEngine::ERelease) {
                    m_pinching = false ;
                    m_holdseen = false ;
                }
                else {
                    bool pointIgnored = true ;  // for logging purposes
                    int currentLength = m_loggingenabled ? QStm_Utils::distance(m_pinchstart, m_pinchend) : 0;

                    QPoint oStart(m_pinchstart) ;
                    QPoint oEnd(m_pinchend) ;
                    int difference = 0 ;
                    state = ELockToThisGesture ;
                    const QPoint& tp = puie->currentXY();
                    // calculate the distance of the new point from the stored vector
                    int d1 =  ((m_pinchstart.x() - tp.x()) * (m_pinchstart.x() - tp.x())) + 
                    		  ((m_pinchstart.y() - tp.y()) * (m_pinchstart.y() - tp.y())) ;
                    int d2 =  ((m_pinchend.x() - tp.x()) * (m_pinchend.x() - tp.x())) + 
                    		  ((m_pinchend.y() - tp.y()) * (m_pinchend.y() - tp.y())) ;
                    // check also if the Y coordinate happens to be near the hold point,
                    // this seems to be the case at least with alvin, we keep getting two points,
                    // where one is near the Y coordinate of the hold point
                    int diffY = ABS(tp.y() - m_pinchstart.y()) ;

                    if (d1 < d2 || diffY < 12) {
                        // the detected point is near the first point,
                        // or the detected point is about on the same horizontal line with the hold point
                        // do not do anything, but keep the gesture
                    }
                    else {
                        pointIgnored = false ;
                        // the detected point is close to the other end, then adjust the stored vector
                        int xd = m_pinchend.x() - tp.x() ;
                        int yd = m_pinchend.y() - tp.y() ;
                        if (xd < 0 ) xd = - xd ;
                        if (yd < 0 ) yd = - yd ;
                        // look which coordinate is closer to the original and use that
                        if (xd < yd) {
                            // calculate new point based on the X value
                            m_pinchend.setX(tp.x()) ;
                            m_pinchend.setY(m_m * m_pinchend.x() + m_b) ;
                            if (m_pinchend.y() < 0) m_pinchend.setY(0) ;
                        }
                        else {
                            if (m_m != 0) {
                                m_pinchend.setY(tp.y()) ;
                                m_pinchend.setX((m_pinchend.y() - m_b) / m_m) ;
                                if (m_pinchend.x() < 0 ) m_pinchend.setX(0) ;
                            }
                            else {
                                m_pinchend.setX(tp.x()) ;
                                m_pinchend.setY(m_m * m_pinchend.x() + m_b) ;
                                if (m_pinchend.y() < 0) m_pinchend.setY(0) ;
                            }
                        }
                        float newd = calculateDistance() ;
                        // check if the difference is too big and adjust accordingly
                        // the method also updates the m_ddistance
                        difference = adjustPinchMove(m_ddistance, newd) ;
                        // Now we have a pinch gesture with size as details
                        qstmGesture::QStm_TwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                        pgest.setLogging(m_loggingenabled);
                        pgest.setDetails(&difference) ;
                        pgest.setTarget(puie->target());
                        // inform the listener
                        m_listener->gestureEnter(pgest);
                    }
                    if (m_loggingenabled) {
                        int newLength = QStm_Utils::distance(m_pinchstart, m_pinchend);
                        float speed = puie->speed() ;

                        LOGARG("QStm_PinchGestureRecogniser: %d: o: %d, n: %d, d: %d (%d,%d) speed %f (%d,%d : %d,%d) (from: (%d,%d : %d,%d) (m: %f b: %f)",
                                pointIgnored,
                                currentLength, newLength, difference,
                                tp.x(), tp.y(), double(speed),
                                m_pinchstart.x(), m_pinchstart.y(), m_pinchend.x(), m_pinchend.y(),
                                oStart.x(), oStart.y(), oEnd.x(), oEnd.y(),
                                double(m_m), double(m_b)) ;
                    }

                }
            }
            else if (eventCode == qstmUiEventEngine::EMove) { // The last one is move and we were not pinching

                if (m_loggingenabled) {
                    LOGARG("QStm_PinchGestureRecogniser: %d: num %d code %d", m_pinching, countOfEvents, eventCode);
                }
                qstmUiEventEngine::QStm_UiEventIf* puieFirst = puie->previousEvent();

                // check if we have seen hold
                if (m_holdseen) {
                    const QPoint& tp1 = puie->currentXY() ;
                    float speed = puie->speed() ;
                    if (m_loggingenabled) {
                        LOGARG("QStm_PinchGestureRecogniser: tp1: %d %d hold %d %d, speed %f", 
                        		tp1.x(), tp1.x(), m_holdseenAtPos.x(), m_holdseenAtPos.y(), double(speed) );
                    }
                    // is the speed extremely high so that it looks like other finger pressing in different location?
                    if (speed > m_pinchingspeed) {
                        QTime tstamp = puie->timestamp() ;
                        
                        int tim = m_holdseenAtTime.msecsTo(tstamp);
          
                        m_pinching = true;
                        m_pinchstart = m_holdseenAtPos;
                        m_pinchend = tp1;
                        calculateZoomingLine();
                        m_ddistance = calculateDistance();
                        state = ELockToThisGesture ;    // NOTE: once pinch is started, it will stay until release
                        // create the first pich gesture which does not yet resize anything
                        qstmGesture::QStm_TwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                        pgest.setTarget(puie->target());
                        pgest.setLogging(m_loggingenabled);
                        pgest.setDetails(0) ;
                        // inform the listener
                        m_listener->gestureEnter(pgest);
                    }
                }
            }
        }
        if (!m_pinching) {
            if (m_loggingenabled) {
                LOGARG("QStm_PinchGestureRecogniser: not pinching %d", puie);
            }
            if (puie != NULL && puie->code() == qstmUiEventEngine::EHold) { // The last one is hold and we were not pinching

                m_holdseen = true;
                m_holdseenAtPos = puie->currentXY();
                m_holdseenAtTime = puie->timestamp() ;
                if (m_loggingenabled) {
                    LOGARG("QStm_PinchGestureRecogniser: hold seen at(%d, %d) at %s", 
                    		m_holdseenAtPos.x(), m_holdseenAtPos.y(), m_holdseenAtTime.toString("hh:mm:ss.zzz"));
                }
            }
        }
        if (puie && puie->code() == qstmUiEventEngine::ETouch) {// The last one is touch
            m_holdseen = false;
        }
        else if (puie && puie->code() == qstmUiEventEngine::ERelease) {// The last one is release
            m_holdseen = false;
        }
    }
#else
    // This is the multi touch case: two event streams needs to be there; this is the real pinch zoom
    if (numOfActiveStreams == 2) {
        const qstmUiEventEngine::QStm_UiEventIf* puie1 = pge->getUiEvents(0);
        const qstmUiEventEngine::QStm_UiEventIf* puie2 = pge->getUiEvents(1);
        qstmUiEventEngine::QStm_UiEventCode eventCode1 = puie1->code() ;
        qstmUiEventEngine::QStm_UiEventCode eventCode2 = puie2->code() ;

        if (m_loggingenabled) {
            QPoint p1 = puie1->currentXY() ;
            QPoint p2 = puie2->currentXY() ;
            LOGARG("QStm_PinchGestureRecogniser: two streams: %s at [%d,%d], %s at [%d,%d]",
                    qstmUiEventEngine::event_name(eventCode1), p1.x(), p1.y(),
                    qstmUiEventEngine::event_name(eventCode1), p2.x(), p2.y()
                    ) ;

        }


        if (!m_pinching) {
            // This means we start pinching, the events can be any combination of ETouch, EMove, EHold
            if ((eventCode1 == qstmUiEventEngine::ETouch || 
                 eventCode1 == qstmUiEventEngine::EMove || 
                 eventCode1 == qstmUiEventEngine::EHold) &&
                (eventCode2 == qstmUiEventEngine::ETouch || 
                 eventCode2 == qstmUiEventEngine::EMove  || 
                 eventCode2 == qstmUiEventEngine::EHold))   {
            	
                // This is valid pinching start
                m_pinching = true ;
                // get the start and end position for the picnhing vector
                m_pinchstart = puie1->currentXY() ;
                m_pinchend = puie2->currentXY() ;
                calculateZoomingLine();
                m_ddistance = calculateDistance();
                state = ELockToThisGesture ;    // NOTE: once pich is started, it will stay until release
                if (m_loggingenabled) {
                    LOGARG("QStm_PinchGestureRecogniser: pinch start: [%d,%d][%d,%d]",
                            m_pinchstart.x(), m_pinchstart.y(), m_pinchend.x(), m_pinchend.y()) ;

                }
                // create the first pich gesture which does not yet resize anything
                qstmGesture::QStm_TwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                pgest.setLogging(m_loggingenabled);
                pgest.setDetails(0) ;
                pgest.setTarget(puie->target());
                // inform the listener
                m_listener->gestureEnter(pgest);
            }
            else  {
            	// Not a valid pinching start, do nothing (maybe it were easier to just check if one of the events is ERelease)
            }
        }
        else  {
            // We have entered pinching state, lets move one of the points unless it is a release
            if (eventCode1 == qstmUiEventEngine::ERelease || eventCode2 == qstmUiEventEngine::ERelease) {
                m_pinching = false ;
            }
            else  {
                state = ELockToThisGesture ;

                // get the start and end position for the picnhing vector
                m_pinchstart = puie1->currentXY() ;
                m_pinchend = puie2->currentXY() ;
                float newd = calculateDistance() ;
                // check if the difference is too big and adjust accordingly
                // the method also updates the m_ddistance
                int difference = adjustPinchMove(m_ddistance, newd) ;
                // Now we have a pinch gesture with size
                if (m_loggingenabled) {
                    LOGARG("QStm_PinchGestureRecogniser: pinch: [%d,%d][%d,%d], diff %d",
                            m_pinchstart.x(), m_pinchstart.y(), m_pinchend.x(), m_pinchend.y(), difference) ;

                }

                qstmGesture::QStm_TwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
                pgest.setLogging(m_loggingenabled);
                pgest.setDetails(difference) ;
                pgest.setTarget(puie->target());
                // inform the listener
                m_listener->gestureEnter(pgest);
            }
        }

    }
#endif

    if (state == ENotMyGesture) {
        if (m_loggingenabled) {
            LOGARG("QStm_PinchGestureRecogniser: NotMyGesture %d %d %d ", m_pinching, m_holdseen, numOfActiveStreams) ;
        }
        // if it was not our gesture, then the state can not be pinching...
        m_pinching = false ;
    }
    m_state = state;
    
    return state;
}

void QStm_PinchGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
    m_pinching = false ;
    const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    qstmGesture::QStm_TwoPointGesture pgest(KUid, m_pinchstart, m_pinchend);
    pgest.setDetails(0) ;
    pgest.setTarget(puie->target());
    m_listener->gestureExit(pgest) ;
    m_state = ENotMyGesture;
}


/*!
 * Now that we know the two points where the zooming started, we move those points only along
 * the same line y = mx + b, so lets calculate m and b.
 */
void QStm_PinchGestureRecogniser::calculateZoomingLine()
{
    int sX = m_pinchstart.x() ;
    int sY = m_pinchstart.y() ;
    int eX = m_pinchend.x() ;
    int eY = m_pinchend.y() ;

    if (eX == sX) {
        m_m = 0.f ;
    }
    else {
        m_m = float(eY-sY)/(eX-sX) ;
    }
    m_b = sY-(m_m*sX) ;
}

/*!
 * calculate the distance, return as float
 */
float QStm_PinchGestureRecogniser::calculateDistance()
{
	/*
    double x = ((m_pinchstart.x() - m_pinchend.x()) * (m_pinchstart.x() - m_pinchend.x())) + 
    		   ((m_pinchstart.y() - m_pinchend.y()) * (m_pinchstart.y() - m_pinchend.y())) ;
    double ddist; 
    SQRT(ddist, x) ;
    return float(ddist) ;
    */
	return QStm_Utils::distanceF(m_pinchstart, m_pinchend);
}

/*!
 * Set the pinching speed as pixels / ms (meaning that in case of singletouch device
 * the other finger looks like the EMove UI event suddenly jumps to new location;
 * in resistive the new location is somewhere in the middle of the touches, in capacitive
 * the driver seems to report three or four points:
 * original (x,y), new (a,b) and also (a,y), sometimes (x,b)
 */
void QStm_PinchGestureRecogniser::setPinchingSpeed(float speed) /* __SOFTFP */
{
    m_pinchingspeed = speed ;
}

/*!
 * Adjust the pinch move so that it will not be too jumpy
 */
int QStm_PinchGestureRecogniser::adjustPinchMove(float& previousDistance, float newDistance)
{
    float diff = newDistance - previousDistance ;
    float logdiff = diff ;
    if (diff < 0) diff = -diff ;	// Note that the next calculations need the positive diff value, but keep the original in logdiff 
    float changePercentage = (diff/previousDistance)*100.f ;
    if (changePercentage > 10.f) {
        // change more than 10%, make at most 10%
        float newdiff = previousDistance*0.1f ;
        if (previousDistance > newDistance) newdiff = -newdiff ;
        if (m_loggingenabled) {
            LOGARG("QStm_PinchGestureRecogniser: adjustPinchMove from %f to %f : was, now %f %f",
                double(logdiff), double(newdiff), double(previousDistance), double(newDistance));
        }

        previousDistance = previousDistance + newdiff ;
        diff = newdiff ;
    }
    else {
        if (m_loggingenabled) {
            LOGARG("QStm_PinchGestureRecogniser: adjustPinchMove from %f to %f : was, now %f %f",
                double(logdiff), double(diff), double(previousDistance), double(newDistance));
        }
        previousDistance = newDistance ;  // accept the new value and update the new length
        diff = logdiff ;    // put the original back (this is why the logdiff can not be ABS(diff)!
    }
    return (int)diff ;
}
