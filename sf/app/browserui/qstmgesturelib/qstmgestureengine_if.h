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
#ifndef QSTMGESTURELISTENER_IF_H_
#define QSTMGESTURELISTENER_IF_H_


#include "qstmgesturelistener_if.h"
#include "qstmgesture_if.h"

namespace qstmUiEventEngine
{
    class QStm_UiEventIf ;
}

namespace qstmGesture
{

// Forward declarations
class QStm_GestureRecogniserIf ;

/*!
 *  QStm_GestureRecogniserIf defines the methods for the gesture recognisers
 *  to be added to the gesture engine.
 *  TODO: add rule based settings to position the gesture recognisers to the list.
 *  The gesture recogniser order rules would be defined how?
 *  - defining the order inside the application
 *  - defining the order in some configuration file (maybe not)
 *
 *  This requires:
 *  - the gesture recognisers need to have an ID
 *  - a type can also be defined if the rules can be set based on types.
 *
 *  The gesture recognisers are arranged in a list so that the correct gesture recogniser
 *  gets the opportunity to detect the gesture in time.  An axample of a set of gesture recognisers
 *  could be e.g.
 *
 *  |---------------------------------------------|
 *  |  scrolling stop by tap -gesture recogniser  | A is enabled only when needed, when enabled must precede all others
 *  |                                             |   (note that locking partly can be used to achieve the same)
 *  |---------------------------------------------|
 *  | location specific: zoom + button tap gesture| B these must detect the location first
 *  |---------------------------------------------|
 *  | location specific: zoom - button tap gesture| B these must detect the location first
 *  |---------------------------------------------|
 *  | location specific: edge scroll gesture      | B these must detect the location first
 *  |---------------------------------------------|
 *  | pinch zoom gesture                          |   standard gesture, but application wide (spans windows)
 *  |                                             | C another requirement: disable all but me/restore state
 *  |                                             |   so that other gestures will not meddle in while pinching
 *  |---------------------------------------------|
 *  | hover gesture                               | C standard gesture, needs to detect the EMove speed
 *  |---------------------------------------------|
 *  | pan gesture                                 | C standard gesture, but must be after hover
 *  |---------------------------------------------|
 *  | tap/doubletap gesture                       | C standard gesture, order not that important, since
 *  |                                             |   needs just to see certain events in history: touch/release
 *  |---------------------------------------------|
 *
 *  The order of gesture recognisers determine how the system behaves, so there must be an easy way
 *  to define the order.  The order could be defined either by specifying the IDs or just their types, if then the
 *  order is not that important.  TODO to study whether it would be a good idea to implement this so that
 *  there are separate lists for the three different classes of gesture recognisers A, B and C; this might
 *  make the implementation simpler.
 */
class QStm_GestureEngineIf
{
public:
	QStm_GestureEngineIf() {}
    /*!
     *  add gesture to the end of the list of gestures
     */
    virtual bool addGesture(const QStm_GestureRecogniserIf* newGesture) = 0 ;
    /*!
     * inset a gesture to specific position
     */
    virtual bool insertGesture(const QStm_GestureRecogniserIf* newGesture, int position) = 0 ;

    virtual int findGesture(const QStm_GestureRecogniserIf* newGesture, int startPos = 0) const = 0;
    virtual int findGestureReverse(const QStm_GestureRecogniserIf* gesture, int astartPos) const = 0;
    virtual int findGesture(QStm_GestureUid uid, int startPos = 0) const = 0;
    virtual int findGestureReverse(QStm_GestureUid uid, int startPos) const = 0;
    virtual int gestureCount() const = 0;
    virtual QStm_GestureRecogniserIf* gestureAt(int idx) = 0;

    /*!
     * remove a gesture from the list
     */
    virtual bool removeGesture(const QStm_GestureRecogniserIf* oldGesture) = 0 ;
    /*!
     * get the number of non-empty event streams.
     * Event streams correspond UI events generated by one touch pointer (=finger).
     * The low level state machine handles the necessary filtering etc.
     * so that it is safe to remove the event stream after UI Release event has been processed.
     */
    virtual int activeStreamCount() const = 0 ;
    /*!
     * get the UI events of stream X
     * \param indexOfActiveStream defines which active stream is used.
     * Note that QStm_UiEventIf contains all the events from down up to the current event.
     * Assumption: the UI events contain the target "window handle", i.e.
     * the gesture recognition needs to be aware of all possible windows of the application.
     */
    virtual const qstmUiEventEngine::QStm_UiEventIf* getUiEvents(int indexOfActiveStream) const = 0 ;
    /*!
     * Enable/disable logging
     */
    virtual void enableLogging(bool aLoggingEnabled) = 0 ;
};

/*!
 * Enumerated values for the gesture recogniser
 */
enum QStm_GestureRecognitionState
{
    EGestureActive,     /*! < gesture recognised and gesture notification sent */
    ELockToThisGesture, /*! < gesture recognised, and only this recogniser called until release/this returns something else */
    ENotMyGesture       /*! < not this gesture, try the next one in the list  */
};

/*!
 * The types of gesture recognisers. TODO to implement rule based gesture engine
 * where the recognisers can be added in any order, and the rules define the order of them.
 * The rules may define
 */
enum QStm_GestureRecogniserType
{
    ECriticalGestureRecogniser,           /*! < This kind of gestures should be at the beginning of the list */
    ELocationSpecificGestureRecogniser,   /*! < This kind of gestures should be at the next in the list */
    EGenericGestureRecogniser             /*! < This kind of gestures should be at the next in the list */
};

/*!
 * QStm_GestureRecogniserIf defines the interface needed to be implemented by every gesture recognition element.
 * The gesture recogniser gets the UI events and needs to determine whether they define the expected gesture.
 *
 */
class QStm_GestureRecogniserIf
{
public:
	QStm_GestureRecogniserIf() {}
    /*!
     *  Virtual destuructor.
     */
    virtual ~QStm_GestureRecogniserIf() {}
    /*!
     * Process the UI event.
     * \param numOfActiveStreams : indicates how many pointers are currently active.
     * \param ge the gesture engine.  The Gesture recogniser uses this to get the current active events.
     *
     * \return EMaybeGesture    if gesture was not yet recognised but can be possible (e.g. after first touch everyone returns this)
     * \return EGestureActive   if gesture was recognised and a gesture notification was sent; this is then the active gesture
     * \return ENotMygesture    if gesture can not be this one, e.g. TAP gesture after receiving Move UI event.
     *
     * Note that a gesture may take control only if it sends out a gesture notification. (or is it so?)
     * Until no gesture recogniser has sent a gesture notification, all recognisers who have not returned
     * ENotMyGesture are eligible, based on the order they are in the list.
     *
     * The gesture engine may be either in dynamic gestures mode or fixed gestures mode.
     * In dynamic gestures mode each time an UI event is processed, engine will call each
     * gesture recogniser until one of the recognisers returns EGestureActive.
     *
     * In fixed gestures mode the index of the gesture recogniser first returning EGestureActive is stored
     * and the subsequent UI events are processed so that only that recogniser is allowed to send gesture
     * notification.  If it returns !=EGestureActive, it relinguishes the control.
     *
     */
    virtual QStm_GestureRecognitionState recognise(int numOfActiveStreams, QStm_GestureEngineIf* ge) = 0 ;
    /*!
     * In dynamic gestures mode the gesture engine will call release of the recogniser
     * who lost activation status because a preceding recogniser in the list took control.
     *
     * Note that the recogniser must then do whatever (if anything) is needed to cancel the gesture.
     */
    virtual void release(QStm_GestureEngineIf* ge) = 0  ;
    /*!
     * Enable or disable recogniser.  E.g. if two gestures are related so that
     * a gesture ending recogniser needs to be activated to stop a t5imer driven
     * behaviour it is useful to add the recogniser to the top of the list
     * and then disable it until needed.
     */
    virtual void enable(bool enableRecogniser) = 0 ;
    /*!
     * Get the state of the gesture recogniser, whether it is enabled or not
     * \return true, if the recogniser is enabled.
     */
    virtual bool isEnabled() = 0 ;
    /*!
     * Return the type of the gesture recogniser
     */
    virtual QStm_GestureUid gestureUid() const = 0;

    /*!
     * for testing purposes
     */
    virtual void enableLogging(bool loggingEnabled) = 0 ;

    virtual void setOwner(void* owner) = 0;
    
    virtual QStm_GestureRecognitionState state() = 0;
};

class QStm_GestureRecogniser : public QObject, public QStm_GestureRecogniserIf
{
public:
    virtual ~QStm_GestureRecogniser()
    {

    }
    /*!
     * MGestureRecogniserIf methods; partial implementation
     */
    virtual void enable(bool enabled)
    {
        m_gestureEnabled = enabled ;
    }
    
    virtual bool isEnabled()
    {
        return m_gestureEnabled ;
    }
    
    virtual void enableLogging(bool loggingOn)
    {
        m_loggingenabled = loggingOn;
    }
    
    virtual void setOwner(void* owner)
    {
        m_powner = owner;
    }
    
    virtual QStm_GestureRecognitionState state()
    {
        return m_state;
    }
    
protected:
    QStm_GestureRecogniser(QStm_GestureListenerIf* listener) : QStm_GestureRecogniserIf(),
                           m_powner(listener->getOwner()), 
                           m_listener(listener), 
                           m_gestureEnabled(false), 
                           m_loggingenabled(false),
                           m_state(ENotMyGesture)
    {

    }

protected:
    void*                    m_powner ; // The owning control for this gesture
    QStm_GestureListenerIf*  m_listener ;
    bool                     m_gestureEnabled ;
    bool                     m_loggingenabled ;
    QStm_GestureRecognitionState m_state;
};

} // namespace


#endif /* QSTMGESTURELISTENER_IF_H_ */
