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

#ifndef QSTMUIEVENT_IF_H_
#define QSTMUIEVENT_IF_H_

#include <QtCore>
#include "qstmgesturedefs.h"

namespace qstmUiEventEngine
{
static const int KMaxNumberOfPointers(5) ;  // How many of these should we have in multi-touch case

enum QStm_UiEventCode
{
    ETouch  = 0x01,
    EHold   = 0x02,
    EMove   = 0x03,
    ERelease= 0x04,
    ENull   = 0x05
};

enum QStm_AreaShape
{
    ERectangle = 1,
    ECircle,
    EEllipse
};

// for testingg/debugging purposes - string name og the code
const char* event_name(QStm_UiEventCode aCode);


struct QStm_PlatformPointerEvent
{
		enum PEType
			{
			EButton1Down,
			EButton1Up,
			EButton2Down,
			EButton2Up,
			EButton3Down,
			EButton3Up,
			EDrag,
			EMove,
			EButtonRepeat,
			ESwitchOn,
			};
		PEType          m_type;
		unsigned int    m_modifiers;
		QPoint          m_position;
		int             m_pointerNumber;
		void*           m_target;
		QTime           m_time;
};

class QStm_UiEventSpeedIf
{
public:
	QStm_UiEventSpeedIf() {}
    virtual float speed() const /*__SOFTFP*/ = 0;
    virtual QPointF speedVec() const = 0;
};

/*!
 * Utility class to wrap number for (already evaluated) speed values.
 */
class QStm_UiEventSpeed : public QStm_UiEventSpeedIf
{
public:
	QStm_UiEventSpeed(): m_speed(0), m_speedVec(QPointF(0.0, 0.0)) {}
    QStm_UiEventSpeed(float speed): m_speed(speed), m_speedVec(QPointF(0.0, 0.0)) {}
    QStm_UiEventSpeed(float speed, QPointF speedVec): m_speed(speed), m_speedVec(speedVec) {}
    virtual float speed() const /*__SOFTFP*/ { return m_speed; }
    virtual void setSpeed(float speed) { m_speed = speed; }

    virtual QPointF speedVec() const { return m_speedVec; }
    virtual void setSpeedVec(QPointF speedVec) { m_speedVec = speedVec; }
    
    float m_speed;
    QPointF m_speedVec;
};

/*!
 * The UI event interface, UI events are touch, hold, move and release.
 * Note that currently the interface is not OS agnostic enough.  It is using
 * TPoint, TTimeIntervalMicroSeconds etc. types which should be replaced
 * with some standard types/classes.
 */
class QStm_UiEventIf : public QStm_UiEventSpeed
{
public:
	QStm_UiEventIf() : QStm_UiEventSpeed() {}
	
    /*!
     * The starting position of the gesture in _screen_ coordinates
     */
    virtual const QPoint& startPos() const = 0;
    /*!
     * Current position in _screen_ coordinates
     */
    virtual const QPoint& currentXY() const = 0 ;
    /*!
     * Previous position in _screen_ coordinates
     */
    virtual const QPoint& previousXY() const = 0 ;
    /*!
     * Time difference between this and previous UI event
     */
    virtual long stateTransition() const = 0 ;
    /*!
     * true, if the UI event was generated because of timer expiration
     */
    virtual bool timerExpired() const = 0;
    /*!
     * The UI event code
     */
    virtual QStm_UiEventCode code() const = 0 ;
    /*!
     * Target identifier (in practice the CCoeControl* of the window)
     */
    virtual void* target() const = 0 ;
    /*!
     * The index of the UI event.  In single touch this is always 0
     */
    virtual int index() const = 0 ;
    /*!
     * Next event in the gesture (with the same index)
     */
    virtual QStm_UiEventIf* previousEvent() const = 0 ;
    /*!
     * Count of events in gesture
     */
    virtual int countOfEvents() const = 0 ;
    /*!
     * Timestamp
     */
    virtual QTime  timestamp() const = 0 ;
    /*!
     * Speed.  Speed is calculated based on the previous event.
     */
    virtual float speed() const /*__SOFTFP */ = 0 ;
    
    virtual void setSpeed(float speed) = 0;
    
    virtual QPointF speedVec() const = 0;
    virtual void setSpeedVec(QPointF speedVec) = 0;
    virtual QEvent::Type  mapToMouseEventType() = 0;
    virtual QEvent::Type  mapToTouchEventType() = 0;

};

/**
 * Observer that will be notified when UI events have been recognised
 */
class QStm_UiEventObserverIf
{
public:
	QStm_UiEventObserverIf() {};
    /**
     * Handle the UI event
     * \param aEvent event describing the event
     */
    virtual void handleUiEvent( const QStm_UiEventIf& aEvent ) = 0;
};

/*! The state machine interface.
 *
 * To be OS agnostic TPointerEvent, TRect etc. should be replaced with
 * something else.
 */
class QStm_StateMachineIf
{
public:
    /*!
     * \return the rectangle containing the touch area.
     * The shape of the touch area can be either rectangle, circle or ellipse.
     * getTouchArea returns the current touch area, so it may be of zero size.
     * During touch timer the method will return the TouchTimeArea, after that it
     * will return the TouchArea.
     */
    virtual QRect getTouchArea(int pointerNumber = 0) = 0 ;
    /*!
     * \param fingersize_mm defines the width of the rectangle or the diameter of the circle/ellipse
     * used for the touch area during touch timer running.  If the initial touch is a "sloppy" one,
     * there is very easily an extra move event detected during touch time.  On the other hand
     * after touch has been detected, the touch area should not be too big, just something suitable to
     * filter minor movements out.  The proposed solution is to define two touch areas: one to be used
     * while touch timer is running, and another used after touch has been detected.
     * The TouchTimeArea can be a bit larger to allow sloppy touch, then the TouchArea can be smaller to
     * filter minor movements out.
     */
    virtual void setTouchTimeArea(long fingersize_mm) = 0 ;
    /*!
     * \param fingersize_mm defines the width of the rectangle or the diameter of the circle/ellipse
     * used for the touch area.
     */
    virtual void setTouchArea(long fingersize_mm) = 0 ;
    /*!
     * get the touch area shape, either rectangle, circle or ellipse
     */
    virtual QStm_AreaShape getTouchAreaShape() = 0 ;
    /*!
     * set the touch area shape, either rectangle, circle or ellipse.  This is the same for both of
     * the touch areas.
     */
    virtual void setTouchAreaShape(const QStm_AreaShape shape) = 0 ;
    /*!
     * get the touch timeout.  Touch timeout is the time after the first down event
     * until the Touch UI event is generated. Touch timeout makes it possible to
     * calculate an average of the first few points detected before generating the Touch UI event.
     */
    virtual unsigned int getTouchTimeout() = 0 ;
    /*!
     * Set the touch timeout.
     */
    virtual void setTouchTimeout(unsigned int) = 0 ;
    /*!
     * \return the rectangle containing the hold area.
     * The shape of the hold area can be either rectangle, circle or ellipse.
     * getholdArea returns the current hold area, so it may be of zero size.
     */
    virtual QRect getHoldArea(int pointerNumber = 0) = 0 ;
    /*!
     * \param fingersize_mm defines the width of the rectangle or the diameter of the circle/ellipse
     * used for the hold area.  Hold area defines an area so that if the touch coordinates stay
     * inside that area for the duration of hold timeout the Hold UI event is generated.
     */
    virtual void setHoldArea(long fingersize_mm) = 0 ;
    /*!
     * get the hold area shape, either rectangle, circle or ellipse
     */
    virtual QStm_AreaShape getHoldAreaShape() = 0 ;
    /*!
     * set the hold area shape, either rectangle, circle or ellipse
     */
    virtual void setHoldAreaShape(const QStm_AreaShape shape) = 0 ;
    /*!
     * get the hold timeout.  The timeout defines how long the touch coordinates need to stay
     * inside hold area before Hold UI event is generated.
     */
    virtual unsigned int getHoldTimeout() = 0 ;
    /*!
     * Set the hold timeout.
     */
    virtual void setHoldTimeout(unsigned int a) = 0 ;
    /*!
     * get the touch suppress timeout.  This timeout defines how long it will take to generate
     * the Release UI event after UP event during the touch timeout.  This timeout is rather short
     * but will cause the filtering of accidental UP/DOWN events during if they are close together.
     */
    virtual unsigned int getTouchSuppressTimeout() = 0 ;
    /*!
     * Set the touch suppress timeout.
     */
    virtual void setTouchSuppressTimeout(unsigned int a) = 0 ;
    /*!
     * get the move suppress timeout.  This timeout is used after Move UI event has been generated to
     * filter accidental UP/DOWN events.  Using light touch it is possible to cause accidental UP/DOWN
     * events with the timespan can be over 120 ms when the direction of movement changes.
     */
    virtual unsigned int getMoveSuppressTimeout() = 0 ;
    /*!
     * set the move suppress timeout.
     */
    virtual void setMoveSuppressTimeout(unsigned int a) = 0 ;
    /*!
     * add UI event observer. The generated UI events will be sent to the observers.
     * \return false, if the max number of observers (=5) has been reached.
     */
    virtual bool addUiEventObserver(QStm_UiEventObserverIf* observer) = 0 ;
    /*!
     * remove the UI event observer.
     */
    virtual bool removeUiEventObserver(QStm_UiEventObserverIf* observer) = 0 ;
    /*!
     * \return true, of the message being processed did not generate UI event
     */
    virtual bool wasLastMessageFiltered(int pointerNumber = 0) = 0 ;
    /*!
     * enable capacitive UP message.  If it is enabled, UP suppression is not used
     * but the UP event causes immediate Release UI event.
     */
    virtual void enableCapacitiveUp(bool enable) = 0 ;
    /*!
     * enable or disable debug logging of the state machine
     * \param aEnable : logging enabled
     */
    virtual void enableLogging(bool aEnable) = 0 ;
    /*!
     * add "window handles" to the list of targets which should be included
     * in the gesture recognition.  This way it is possible to drop the messges
     * which are not of interest from the gesture recognition point of view.
     * This is used when only the gesture recognition is used so that the UI events are
     * not passed to the application.
     */
    // virtual void addGestureTarget(void* aTarget) = 0 ;

     /*!
     * Setting the Y adjustment useful in capacitive touch
     * Note that there are problems with the adjustment if done at this level,
     * the most proper place would be the window server.
     */
    virtual void enableYadjustment(bool aEnable) = 0 ;

    // Get the number of supported touch pointers
    virtual int getNumberOfPointers() = 0;
    
    /*
    virtual void setPointerBuffer(TPoint* aBufferPtr, int aBufSize) = 0;
    
    virtual TPoint* getPointerBuffer() = 0;
    
    virtual int getPointerBufferSize() = 0;
    */
    virtual void setMoveTolerance(long fingersize_mm) = 0;

};

} // namespace

#endif /* QSTMUIEVENT_IF_H_ */
