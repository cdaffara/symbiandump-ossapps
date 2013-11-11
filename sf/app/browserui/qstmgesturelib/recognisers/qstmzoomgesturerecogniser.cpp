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
#include <qstmuievent_if.h>
#include <qstmfilelogger.h>
#include "qstmzoomgesturerecogniser.h"
#include "qstmgenericsimplegesture.h"
#include "qstmutils.h"

using namespace qstmGesture ;
using namespace qstmUiEventEngine;

QStm_ZoomGestureRecogniser::QStm_ZoomGestureRecogniser(QStm_GestureListenerIf* listener) :
                             QStm_GestureRecogniser(listener), m_area(QRect())
{
    m_rangesizeInPixels = 20 ;  // by default 20 pixels from the edges is the area
    m_zooming = false ;
}

QStm_ZoomGestureRecogniser::~QStm_ZoomGestureRecogniser()
{
}

QStm_GestureRecognitionState QStm_ZoomGestureRecogniser::recognise(int numOfActiveStreams,
                                            QStm_GestureEngineIf* pge)
{
    QStm_GestureRecognitionState state = m_state = ENotMyGesture;
    // Check if we are enabled or not
    if (!m_gestureEnabled) return state ;

    // Look at the events to see if it looks like zoom with one pointer
    if (numOfActiveStreams == 1) {
        const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        int countOfEvents = puie->countOfEvents();
        qstmUiEventEngine::QStm_UiEventCode eventCode = puie->code();

        if (m_loggingenabled) {
            LOGARG("QStm_ZoomGestureRecogniser: %d num %d code %d", eventCode, countOfEvents, eventCode);

        }
        if (!m_zooming) {
            // We are not yet zoomin, check if we should start, i.e. whether we are
            // inside the zoom trigger areas near the corners when touch is done...
            if (eventCode == qstmUiEventEngine::ETouch) { // It must be ETouch inside the corner

                if (m_loggingenabled)  {
                    LOGARG("QStm_ZoomGestureRecogniser: 0x%x ETouch: num %d code %d, %d", 
                    		this, countOfEvents, puie->currentXY().x(), puie->currentXY().y());
                    LOGARG("QStm_ZoomGestureRecogniser: area, %d,%d %d,%d, range: %d", 
                    		m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height(), 
                    		m_rangesizeInPixels);
                }

                // the lower left rectangle is m_rangesizeIPixels from the corner
                QRect lowerleft(m_area.x(), m_area.y() + m_area.height() - m_rangesizeInPixels,
                                m_area.x() + m_rangesizeInPixels, m_area.y() + m_area.height()) ;
                QRect upperright(m_area.x() + m_area.width() - m_rangesizeInPixels, m_area.y(),
                                 m_area.x() + m_area.width(), m_area.y() + m_rangesizeInPixels) ;

                m_previoustouch = m_startingtouch = puie->currentXY() ;
                m_delta = 0 ;
                if (lowerleft.contains(m_startingtouch) || upperright.contains(m_startingtouch)) {
                    if (m_loggingenabled) {
                        LOGARG("QStm_ZoomGestureRecogniser: ZOOM IN/OUT, (%d,%d) in %d,%d %d,%d, range: %d",
                            m_startingtouch.x(), m_startingtouch.y(),
                            m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height(),
                            m_rangesizeInPixels);
                    }
                    m_zooming = true ;
                    if (lowerleft.contains(m_startingtouch)) {
                        m_zoomtype = EZoomIn ;
                    }
                    else {
                        m_zoomtype = EZoomOut ;
                    }
                    state = ELockToThisGesture ;    // keep zooming until release
                    QStm_TwoPointGesture pgest(KUid, m_startingtouch, m_startingtouch);
                    pgest.setLogging(m_loggingenabled) ;
                    pgest.setDetails(&m_delta) ;
                    pgest.setName(m_zoomtype == EZoomIn ? QString("ZoomIn") : QString("ZoomOut")) ;
                    pgest.setType(m_zoomtype);
                    pgest.setTarget(puie->target());

                    // Call the listener to inform that a gesture has happened
                    m_listener->gestureEnter(pgest) ;
                }
                else {
                    // It is not our gesture so do nothing...
                }
            }
            else {
                // It is not touch, so not our initiating UI event.. do nothing
            }
        }
        else {
            // We are already zooming, calculate the changes in zooming factor if it looks we are still zooming
            if (eventCode == qstmUiEventEngine::ERelease) { // ERelease stops zooming
                // We were zooming, but if there are multiple touches we are not any more
            	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
            	QStm_TwoPointGesture pgest = QStm_TwoPointGesture(KUid, puie->currentXY(), m_startingtouch);
                m_listener->gestureExit(pgest) ; // should we call this or not?
            }
            else {   // all other UI events will keep on zooming
                state = ELockToThisGesture ;    // Keep the gesture
                const QPoint& p = puie->currentXY() ;
                float newdist = calculateDistance(p) ;
                float olddist = calculateDistance(m_previoustouch) ;
                m_previoustouch = p ;
                m_delta = adjustZoom(olddist, newdist) ;
                state = EGestureActive ;

                // Inform listener only if there is something to say
                if (m_delta != 0) {
                    QStm_TwoPointGesture pgest = QStm_TwoPointGesture(KUid, p, m_startingtouch);
                    pgest.setLogging(m_loggingenabled) ;
                    pgest.setDetails(&m_delta) ;
                    pgest.setName(m_zoomtype == EZoomIn ? QString("ZoomIn") : QString("ZoomOut")) ;
                    pgest.setType(m_zoomtype);
                    pgest.setTarget(puie->target());
                    // Call the listener to inform that a gesture has happened
                    m_listener->gestureEnter(pgest) ;
                }
            }
        }
    }
    else {
        if (m_zooming) {
            // We were zooming, but if there are multiple touches we are not any more
        	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
        	QStm_TwoPointGesture pgest = QStm_TwoPointGesture(KUid, puie->currentXY(), m_startingtouch);
        	pgest.setTarget(puie->target());
            m_listener->gestureExit(pgest) ; // should we call this or not?
        }
        m_zooming = false ;
    }
    m_state = state;
    return state;
}

void QStm_ZoomGestureRecogniser::release(QStm_GestureEngineIf* pge)
{
    if (m_zooming) {
        m_zooming = false ;
    	const qstmUiEventEngine::QStm_UiEventIf* puie = pge->getUiEvents(0);
    	QStm_TwoPointGesture pgest = QStm_TwoPointGesture(KUid, puie->currentXY(), m_startingtouch);
    	pgest.setTarget(puie->target());
        m_listener->gestureExit(pgest) ; // should we call this or not?
    }
    if (m_loggingenabled) {
        LOGARG("QStm_ZoomGestureRecogniser: 0x%x release", this);
    }
    m_state = ENotMyGesture;
}

void QStm_ZoomGestureRecogniser::setRange(int rangeInMm)
{
    m_rangesizeInPixels = QStm_Utils::mm2Pixels(rangeInMm) ;
}

void QStm_ZoomGestureRecogniser::setArea(const QRect& theArea)
{
    m_area = theArea ;
    if (m_loggingenabled) {
        LOGARG("QStm_ZoomGestureRecogniser: set area, %d,%d %d,%d", 
        	   m_area.x(), m_area.y(), m_area.x() + m_area.width(), m_area.y() + m_area.height());
    }
}

/*!
 * calculate the distance, return as as float
 */
float QStm_ZoomGestureRecogniser::calculateDistance(const QPoint& tp)
{
	const QPoint p = m_startingtouch;
	return QStm_Utils::distanceF(p, tp);
    /*
	double x = ((m_startingtouch.iX-tp.iX)*(m_startingtouch.iX-tp.iX))+((m_startingtouch.iY-tp.iY)*(m_startingtouch.iY-tp.iY)) ;
    double ddist ;
    Math::Sqrt(ddist, x) ;
    return ddist ;
    */
}

int QStm_ZoomGestureRecogniser::adjustZoom(float& previousDistance, float newDistance)
{
    float diff = newDistance - previousDistance ;
    float logdiff = diff ;
    if (diff < 0) diff = -diff ;
    float changePercentage = (diff/previousDistance)*100.f ;
    if (changePercentage > 10.f)
    {
        // change more than 10%, make at most 10%
        float newdiff = previousDistance*0.1f;
        if (previousDistance > newDistance) newdiff = -newdiff ;
        if (m_loggingenabled) {
            LOGARG("QStm_ZoomGestureRecogniser: adjust zoom from %f to %f : was, now %f %f",
                double(logdiff), double(newdiff), double(previousDistance), double(newDistance));
        }

        previousDistance = previousDistance + newdiff ;
        diff = newdiff ;

    }
    else {
        if (m_loggingenabled)  {
            LOGARG("QStm_ZoomGestureRecogniser: adjust zoom from %f to %f : was, now %f %f",
                double(logdiff), double(diff), double(previousDistance), double(newDistance));
        }
        previousDistance = newDistance ;  // accept the new value and update the new length
        diff = logdiff ;    // put the original back
    }
    return (int)diff ;
}
