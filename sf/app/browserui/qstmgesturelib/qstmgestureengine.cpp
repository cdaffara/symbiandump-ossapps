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

#include "qstmgestureengine.h"
#include "qstmfilelogger.h"
#include "qstmutils.h"


using namespace qstmGesture ;

QStm_GestureEngine::QStm_GestureEngine()
{
	m_loggingEnabled = 0;
    m_numOfActiveStreams = 0 ;
    m_currentGestureOwner = -1 ;
    m_currentLockedGesture = -1 ;
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++) {
        m_uiEventStream[i] = NULL ;
    }
}

QStm_GestureEngine::~QStm_GestureEngine()
{
    m_gestures.clear() ;
}

bool QStm_GestureEngine::addGesture(const QStm_GestureRecogniserIf* newGesture)
{
    // Add the new gesture recogniser to our list of recognisers
	if (newGesture) {
		QStm_GestureRecogniserIf* p = const_cast<QStm_GestureRecogniserIf*>(newGesture);
		m_gestures.append(p);
	}
    return true;
}

bool QStm_GestureEngine::insertGesture(const QStm_GestureRecogniserIf* newGesture, int startPos)
{
	QStm_GestureRecogniserIf* p = const_cast<QStm_GestureRecogniserIf*>(newGesture);
    m_gestures.insert(startPos, p);
    return true;
}

int QStm_GestureEngine::findGesture(const QStm_GestureRecogniserIf* gesture, int startPos) const
{
    for(int i = MAX(startPos, 0); i < m_gestures.count(); ++i) {
        if(m_gestures[i] == gesture) {
            return i;
        }
    }
    return -1;
}

int QStm_GestureEngine::findGestureReverse(const QStm_GestureRecogniserIf* gesture, int startPos) const
{
    for(int i = MIN(startPos, m_gestures.count() - 1); i >=0; --i) {
        if(m_gestures[i] == gesture) {
            return i;
            }
        }
    return -1;
}

int QStm_GestureEngine::findGesture(QStm_GestureUid uid, int startPos) const
{
    for(int i = MAX(startPos, 0); i < m_gestures.count(); ++i) {
        if(m_gestures[i]->gestureUid() == uid) {
            return i;
        }
    }
    return -1;
}

int QStm_GestureEngine::findGestureReverse(QStm_GestureUid uid, int startPos) const
{
    for(int i = MIN(startPos, m_gestures.count() - 1); i >=0; --i) {
        if(m_gestures[i]->gestureUid() == uid) {
            return i;
        }
    }
    return -1;
}

int QStm_GestureEngine::gestureCount() const
{
    return m_gestures.count();
}

bool QStm_GestureEngine::removeGesture(const QStm_GestureRecogniserIf* oldGesture)
{
    // If gestures are removed, there cannot be current gesture owner...
    if (m_currentGestureOwner != -1)
    {
        QStm_GestureRecogniserIf* pgrif = m_gestures[m_currentGestureOwner] ;
        pgrif->release(this) ;
        m_currentGestureOwner = -1 ;    // no more gesture owners...
    }
    QStm_GestureRecogniserIf* p = const_cast<QStm_GestureRecogniserIf*>(oldGesture);
    int ix = m_gestures.indexOf(p) ;
    bool found = (ix != -1);
    if (found) {
        m_gestures.removeAt(ix) ;
    }
    return found ;
}

int QStm_GestureEngine::activeStreamCount() const
{
    return m_numOfActiveStreams ;
}

const qstmUiEventEngine::QStm_UiEventIf* QStm_GestureEngine::getUiEvents(int indexOfActiveStream) const
{
//#if defined(ADVANCED_POINTER_EVENTS)
    // create temporary array of active event streams and initialize with zero
    const qstmUiEventEngine::QStm_UiEventIf* activeEventPointers[qstmUiEventEngine::KMaxNumberOfPointers] ;
    for (int x = 0; x < qstmUiEventEngine::KMaxNumberOfPointers; activeEventPointers[x++] = 0);
    
    // then fill from currently active event streams
    int indextoactiveEventPointers = 0 ;
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++) {
        if (m_uiEventStream[i] != NULL) {
            activeEventPointers[indextoactiveEventPointers++] = m_uiEventStream[i] ;
        }
    }
    // then return the active event stream asked
    return activeEventPointers[indexOfActiveStream] ;
//#else
    // in single touch it is enough to return the only possible pointer
//    return m_uiEventStream[indexOfActiveStream] ;
//#endif
}

/*!
 * Process the UI events
 */
void QStm_GestureEngine::handleUiEvent(const qstmUiEventEngine::QStm_UiEventIf& event )
{
    // process one incoming UI event
    storeUiEvent(event) ;  // store the event to the "stream" based on the index of pointer
    walkTroughGestures() ;  // and walk trough the gestures to process the UI event
    updateUiEvents() ;
    // If it was last release event, make sure no-one has the gestures locked
    m_numOfActiveStreams = 0 ;
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++) {
        if (m_uiEventStream[i] != NULL) m_numOfActiveStreams++ ;
    }
    
    if (m_numOfActiveStreams == 0)
    {
        if (m_currentLockedGesture != -1)
        {
            QStm_GestureRecogniserIf* pgrif = m_gestures[m_currentLockedGesture] ;
            pgrif->release(this) ;
        }
        m_currentLockedGesture = -1 ;
    }
}

/*!
 * Store the UI event.  There are max X "streams" of events, one for each
 * pointer.  The streams are actually just pointers to the latest event, since the
 * MUiEvent interface has methods to walk trough the chain of events.
 */
void QStm_GestureEngine::storeUiEvent(const qstmUiEventEngine::QStm_UiEventIf& event)
{
    m_uiEventStream[event.index()] = &event ;
    m_numOfActiveStreams = 0 ;
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++) {
        if (m_uiEventStream[i] != NULL) m_numOfActiveStreams++ ;
    }
}

/*!
 *  Call each gesture handler in turn until one claims to be in control of the gesture.
 */
void QStm_GestureEngine::walkTroughGestures()
{
    int newowner = -1 ;
    int newlocker =  -1; //m_currentLockedGesture ;
    // check if someone has locked the gesture
    QStm_GestureRecognitionState thestate = ENotMyGesture ;
    if (m_currentLockedGesture != -1) {
        QStm_GestureRecogniserIf* pgrif = m_gestures[m_currentLockedGesture] ;
        if (pgrif != NULL) {
            if (m_loggingEnabled) {
                // log entry about locked gesture (hmm.. should have added names to the MGestureRecogniserIf
                LOGARG("locked gesture recognizer %d (addr %d), active streams %d", 
                		m_currentLockedGesture, pgrif, m_numOfActiveStreams);
            }

            thestate = pgrif->recognise(m_numOfActiveStreams, this) ;
            switch (thestate) {
				case EGestureActive:
				{
					// This gesture recogniser owns the gesture, so release the lock
					newlocker = -1 ;
					newowner = m_currentLockedGesture ;
					if (m_loggingEnabled) {
						LOGARG("new owner %d lock release", m_currentLockedGesture);
					}
					break;
				}
				case ELockToThisGesture:
				{
					// this gesture recogniser wants to keep the lock
					newowner = m_currentLockedGesture ;
					newlocker = m_currentLockedGesture ;
					if (m_loggingEnabled) {
						LOGARG("new owner %d keep lock", m_currentLockedGesture);
					}
					break;
	
				}
				case ENotMyGesture:
				{
					break;
				}
            }
        }
        else {
            if (m_loggingEnabled) {
                LOGARG("NULL recogniser for %d", m_currentLockedGesture);
            }
        }
    }
    QStm_GestureRecognitionState currentState = ENotMyGesture;
    if (thestate == ENotMyGesture) {
        if (m_loggingEnabled) {
            LOGARG("walk trough recognizers active streams %d", m_numOfActiveStreams);
        }
        // No locking gesture, walk trough the list until someone handles this
        int gcount = m_gestures.count();
        if (m_currentGestureOwner > -1) {
            QStm_GestureRecogniserIf* gestureOwner = m_gestures[m_currentGestureOwner] ;
            currentState = gestureOwner->state();
        }
        else {
            currentState = ENotMyGesture;
        }
        
        for (int i = 0; i < gcount; i++) {
            bool controlObtained = false;
            QStm_GestureRecogniserIf* pgrif = m_gestures[i];
            
            
            if (pgrif != NULL) {
                switch (pgrif->recognise(m_numOfActiveStreams, this)) {
					case EGestureActive:
					{
						// This gesture recogniser owns the gesture, stop looping...
						controlObtained = true;
						newowner = i;
						break;
					}
					case ELockToThisGesture:
					{
						// this gesture recogniser wants to take ownership
						controlObtained = true;
						newowner = i;
						newlocker = i;
						break;
	
					}
					case ENotMyGesture:
					{
						break;
					}
                }
                LOGARG("walkTroughGestures: 0x%x, recognizers count %d, newowner %d, m_currentGestureOwner %d", 
                		pgrif, gcount, newowner, m_currentGestureOwner);
            }
            if (controlObtained)  {
                break; // do not process rest of the gestures
            }

        }
    }
    if (newowner != m_currentGestureOwner) {
        if (m_currentGestureOwner != -1 ) {
            QStm_GestureRecogniserIf* pgrif = m_gestures[m_currentGestureOwner] ;
            if ((currentState == EGestureActive || currentState == ELockToThisGesture) &&
                 pgrif->state() == ENotMyGesture) {
                
                pgrif->release(this) ;
            }
        }
        m_currentGestureOwner = newowner ;
    }
    m_currentLockedGesture = newlocker ;    // if someone locked it or released the lock
}

void QStm_GestureEngine::updateUiEvents()
{
    for (int i = 0; i < qstmUiEventEngine::KMaxNumberOfPointers; i++) {
        if (m_uiEventStream[i] != NULL) {
            if (m_uiEventStream[i]->code() == qstmUiEventEngine::ERelease) {
                // Event can be removed since Release is the last event
                // note that it is the lower layer event engine
                // which actually deletes the object
                m_uiEventStream[i] = NULL ;
            }
        }
    }
}


