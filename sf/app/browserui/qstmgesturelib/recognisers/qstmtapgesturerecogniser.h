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

#ifndef QSTMTAPGESTURERECOGNISER_H_
#define QSTMTAPGESTURERECOGNISER_H_

#include <qstmgestureengine_if.h>
#include <qstmgesture_if.h>
#include <qstmgesturelistener_if.h>

namespace qstmGesture
{

/*!
 * QStm_TapGestureRecogniser handles both tap and double tap recognition.
 * Doubletap needs to work even if it crosses window boundaries, so
 * QStm_TapGestureRecogniser is a "global" recogniser.  The application
 * creates only one instance of it (TODO later: how to use Singleton
 * pattern in Symbian, seems to be tricky since we would need to have
 * writable static variable to hold the one instance, so currently
 * we need well-behaving app to handle this: the application must known
 * the nature of this gesture recogniser).
 * The different windows can add their callbacks, and when recognising tap
 * the target window is stored so that proper callback can be called.
 * Doubletap is reported either to the target of second tap, or if that windows parent
 * has a doubletap listener, to that.
 * Use separate listeners for tap and doubletap.
 */
class QStm_TapGestureRecogniser : public QStm_GestureRecogniser
{
	Q_OBJECT
public:
    static const QStm_GestureUid KUid = EGestureUidTap;

    virtual ~QStm_TapGestureRecogniser();

    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) ;
    virtual void release(QStm_GestureEngineIf* ge) ;
    virtual void enable(bool enabled) ;
    virtual bool isEnabled() ;
    virtual void setOwner(void* owner) ;

    virtual QStm_GestureUid gestureUid() const { return KUid; }

    /*!
     * Additional methods to set up tap gesture recogniser:
     * define the double tap timeout in microseconds.
     * \param timeoutInMicroseconds
     */
    void setDoubleTapTimeout(int timeoutInMicroseconds) ;

    /*!
     * Additional methods to set up tap gesture recogniser:
     * define how close the two taps need to be to be recognised
     * as a double tap.
     * \param rangeInMillimetres
     */
    void setDoubleTapRange(int rangeInMillimetres) ;
    /*!
     * Additional methods to set up tap gesture recogniser:
     * Produce two separate taps or just ignore the first one
     * if the second tap is outside range.
     */
    void ignoreFirstTap(bool ignore) ;

    /*!
     * Method to add a listener to tap gestures
     */
    void addTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;
    /*!
     * Method to remove a listener from tap gestures
     */
    void removeTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;
    /*!
     * Method to add a listener to doubletap gestures
     */
    void addDoubleTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;
    /*!
     * Method to remove a listener from doubletap gestures
     */
    void removeDoubleTapListener(QStm_GestureListenerIf* listener, void* listenerOwner) ;

    /*!
     * for testing purposes we need to log the stuff somewhere...
     */

    virtual void enableLogging(bool loggingOn) ;

    /*!
     * The timer function to handle timeout for tap
     */

    QStm_TapGestureRecogniser(QStm_GestureListenerIf* listener) ;
    
public slots:
    void timeoutCallback();
    
private:
    bool m_loggingenabled ;
    bool isPointClose(const QPoint& firstPoint, const QPoint& secondPoint) ;
private:
    void* m_powner ; // The owning control for this gesture
    bool m_waitingforsecondtap ;
    QPoint m_firstTapXY ;
    void* m_firstTapTarget ;
    float m_firstTapSpeed ;
    int m_doubleTapTimeout ;
    bool m_gestureEnabled ;
    bool m_ignorefirst ;
    int  m_rangesizeInPixels ;
    // use simple arrays to store the listeners and corresponding windows
    QList<QStm_GestureListenerIf*>  m_tapListeners ;
    QList<void*>                    m_tapListenerWindows ;
    QList<QStm_GestureListenerIf*>  m_doubleTapListeners ;
    QList<void*>                    m_doubleTapListenerWindows ;
    QTimer        m_timer;
};

} // namespace

#endif /* QSTMTAPGESTURERECOGNISER_H_ */
