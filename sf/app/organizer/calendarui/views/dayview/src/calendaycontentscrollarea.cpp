/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description:  CalenDayContentScrollArea implementation.
 *
 */

// System includes
#include <QGraphicsLinearLayout>
#include <QGesture>

#include <HbInstance>
#include <HbSwipeGesture>

// User includes
#include "calendaycontentscrollarea.h"
#include "calendayutils.h"

// Constants
/*!
 Default timeout for scrolling between days [ms]
 */
const int KCalenScrollDaysTimeout = 600;

/*!
 Value [%] defines how long (depending on content area width) should horizontal
 pan gesture be to change day to previous/next.
 If the gesture is shorter - current view is not changed.
 */
const int KCalenHScrollMoveParam = 30;  //!< Percentage


/*!
 \class CalenDayContentScrollArea
 \brief Scrollable container class for content widgets.
 
 It handles horizontal scrolling and swipe or pan gestures.
 */

/*!
 \brief Constructor
 
 Configures scroll area settings and resets internal stares of widget.
 Gets the width of device.
 
 \param parent The parent of scroll area widget
 */
CalenDayContentScrollArea::CalenDayContentScrollArea(QGraphicsItem *parent) :
    HbScrollArea(parent), mPanDayDirection(ECalenPanNotSet), mIsMoving(false),
    mMoveDirection(ECalenScrollNoDayChange), mDisallowedDirection(ECalenScrollNoDayChange)
{ 
#ifdef CALENDAYVIEW_PANNING_ENABLED
    grabGesture(Qt::PanGesture);
    ungrabGesture(Qt::SwipeGesture);
#else
    grabGesture(Qt::SwipeGesture);
    ungrabGesture(Qt::PanGesture);
#endif

    // Get the width of content area and orientation of screen
    mContentWidth = CalenDayUtils::instance()->contentWidth();
    mOrientation = CalenDayUtils::instance()->orientation();
    
    // Fix the width of scroll area
    setMinimumWidth(mContentWidth);
    setMaximumWidth(mContentWidth);

    // Connect to main window's orientationChanged SIGNAL to handle orientation
    // switching
    connect(CalenDayUtils::instance()->mainWindow(), 
        SIGNAL(orientationChanged(Qt::Orientation)), this,
        SLOT(orientationChanged(Qt::Orientation)));
}

/*!
 \brief Destructor
 */
CalenDayContentScrollArea::~CalenDayContentScrollArea()
{
}

/*!
 \brief Returns disallowed scroll direction (if defined)
 
 \return Disallowed scroll direction
 */
CalenScrollDirection CalenDayContentScrollArea::disallowedScrollDirection() const
{
    return mDisallowedDirection;
}

/*!
 \brief Sets disallowed scroll direction
 
 \param direction Disallowed scroll direction to be set
 */
void CalenDayContentScrollArea::setDisallowedScrollDirection(
    const CalenScrollDirection direction)
{
    mDisallowedDirection = direction;
}

/*!
 \brief Scrolls to middle widget.
 
 Scrolling to middle widget is done if needed.
 Resets internal pan direction flag.
 */
void CalenDayContentScrollArea::scrollToMiddleWidget()
{
    QPointF currentPosition = contentWidget()->pos();
    QPointF destPosition = QPointF(mContentWidth, currentPosition.y());

    // Scroll only when x position is wrong
    if (currentPosition.x() != destPosition.x()) {
        scrollContentsTo(QPointF(mContentWidth, currentPosition.y()), 0);
    }

    // Reset pan direction flag and scrolling flag
    mPanDayDirection = ECalenPanNotSet;
}

/*!
 \brief Scrolls the view by the amount indicated by "delta".
 
 Checks the direction of pan gesture and promotes leading direction.
 
 \param delta Move offset
 \return Returns TRUE if the view was able to scroll, FALSE otherwise
 */
bool CalenDayContentScrollArea::scrollByAmount(const QPointF &delta)
{
    QPointF newDelta(delta);
    if (mPanDayDirection == ECalenPanVertical) {
        newDelta.setX(0);
    }
    else
        if (mPanDayDirection == ECalenPanHorizontal) {
            newDelta.setY(0);
        }
        else {
            // Pan direction not set
        }

    return HbScrollArea::scrollByAmount(newDelta);
}

/*!
 \brief Handles pan gesture event (horizontal) or swipe gesture. 
 
 Ignores vertical pan gestures.
 
 \param event Gesture event to be handled
 */
void CalenDayContentScrollArea::gestureEvent(QGestureEvent *event)
{
#ifdef CALENDAYVIEW_PANNING_ENABLED
    // Process a pan gesture event
    if (QPanGesture *panGesture = qobject_cast<QPanGesture*> (event->gesture(
        Qt::PanGesture))) {

        // Checks pan gesture direction
        checkPanDirection(panGesture);

        // Put the gesture forward before working with finished gesture
        HbScrollArea::gestureEvent(event);

        // If gesture is finished move the scroll area to next or previous 
        // widget or resume to gesture start point
        if (panGesture->state() == Qt::GestureFinished) {
            // Pan direction should be reseted when scrolling ends

            // Gets the offset of pan gesture.
            QPointF offset = panGesture->offset();

            // Note: in horizontal orientation x should is treaten as Y, y as X.
            QPointF movement;
            if (mOrientation == Qt::Vertical) {
                movement = offset;
            }
            else {
                movement.setX(offset.y());
                movement.setY(offset.x());
            }

            // Gesture was long enough for place movement
            if (qAbs(movement.x()) > (KCalenHScrollMoveParam * mContentWidth / 100)) {
                if (movement.x() < 0) {
                    mMoveDirection = ECalenScrollToNext;
                    moveTo(QPointF((-mStartPosition.x() + mContentWidth),
                        -mStartPosition.y()), KCalenScrollDaysTimeout);
                }
                else {
                    mMoveDirection = ECalenScrollToPrev;
                    moveTo(QPointF(-mStartPosition.x() - mContentWidth,
                        -mStartPosition.y()), KCalenScrollDaysTimeout);
                }
            }
            // Gesture was short one, reset to gesture start point
            else {
                qreal startPos = mStartPosition.x();
                bool isNegative = false;
                if (startPos < 0) {
                    isNegative = true;
                }
                startPos = qAbs(startPos);
                qreal normalizeValue = mContentWidth / 2;

                while (startPos > normalizeValue) {
                    normalizeValue += mContentWidth;
                }

                if (isNegative) {
                    mStartPosition.setX(-(normalizeValue - (mContentWidth / 2)));
                }
                else {
                    mStartPosition.setX(normalizeValue - (mContentWidth / 2));
                }

                mMoveDirection = ECalenScrollNoDayChange;
                moveTo(-mStartPosition, KCalenScrollDaysTimeout);
            }
        }
    }
    else {
        HbScrollArea::gestureEvent(event);
    }
#else
    // Let the content scroll area ignore pan gestures
    if (QPanGesture *panGesture = qobject_cast<QPanGesture *> (event->gesture(
                Qt::PanGesture))) {
        // do nothing with pan gesture
    }

    if (HbSwipeGesture *swipeGesture =
        qobject_cast<HbSwipeGesture *> (event->gesture(Qt::SwipeGesture))) {
        if (swipeGesture->state() == Qt::GestureStarted) {
            mStartPosition = contentWidget()->pos();
            
            qreal swipeAngle = swipeGesture->sceneSwipeAngle();
            if (isHorizontalSwipe(swipeAngle)) {
                if (QSwipeGesture::Left == 
                    swipeGesture->sceneHorizontalDirection()) {
                    mMoveDirection = ECalenScrollToNext;
                    moveTo(QPointF((-mStartPosition.x() + mContentWidth),
                            -mStartPosition.y()), KCalenScrollDaysTimeout);
                }
                else if (QSwipeGesture::Right == 
                    swipeGesture->sceneHorizontalDirection()) {
                    mMoveDirection = ECalenScrollToPrev;
                    moveTo(QPointF(-mStartPosition.x() - mContentWidth,
                            -mStartPosition.y()), KCalenScrollDaysTimeout);
                }
            }
        }
    }
#endif
}

/*!
 \brief Filters pan gesture events.
 
 Filters events if this object has been installed as an event filter for
 the watched object. Handles horizontal pan gestures (ignores vertical).
 When moving scroll area all gesture events are blocked.
 
 \param obj Watched object
 \param event Event to be filtered
 \return Returns TRUE if event was handled. FALSE otherwise.
 */
bool CalenDayContentScrollArea::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    bool handled = false;

    // Check if we get a gesture event
    if (event->type() == QEvent::Gesture) {

        // Blocks handling of gesture events if scrolling started by 
        // pan gesture is in progress
        if (mIsMoving) {
            handled = true;
        }
        else {
            QGestureEvent* gesture = static_cast<QGestureEvent*> (event);

            QPanGesture *panGesture = qobject_cast<QPanGesture*>(gesture->gesture(Qt::PanGesture));
            if (panGesture) {
                checkPanDirection(panGesture);
                if (mPanDayDirection == ECalenPanHorizontal) {
                    gestureEvent(gesture);
                    handled = true;
                }
            }else{
                QSwipeGesture *swipeGesture = qobject_cast<QSwipeGesture*>(gesture->gesture(Qt::SwipeGesture));
                if (swipeGesture) {
                    gestureEvent(gesture);
                    handled = true;
                }
            }
        }
    }

    return handled;
}

/*!
 \brief Overriden event handler.
 
 Handles events:
 - gesture/focus events blocked when horizontal scrolling is in progress
 - layout request event - scrolls to middle widget if current position is wrong
 
 \param e Event to be handled
 \return Returns TRUE if event was handled. FALSE otherwise.
 */
bool CalenDayContentScrollArea::event(QEvent *e)
{
    bool result = false;

    // Blocks base class handler for certain events if scrolling started
    // by pan gesture is in progress
    if (mIsMoving && (e->type() == QEvent::Gesture || e->type()
        == QEvent::GestureOverride || e->type() == QEvent::FocusOut
        || e->type() == QEvent::FocusIn)) {
        result = true;
    }
    if (!result) {
        // Call base class handler
        result = HbScrollArea::event(e);
        
        // Scroll to middle widget when layout request
        if (e->type() == QEvent::LayoutRequest || e->type() == QEvent::Show) {
            scrollToMiddleWidget();
        }  
    }

    return result;
}

/*!
 \brief Checks the direction of pan gesture.
 
 Changes the scrolling style according to movement direction, 
 stores the orientation of the pan gesture.
 Function is used when switching widgets by panning gesture is enabled.
 
 \param panGesture Pan gesture event
 */
void CalenDayContentScrollArea::checkPanDirection(QPanGesture *panGesture)
{
    // Gets the offset of pan gesture.
    QPointF offset = panGesture->offset();

    // Note: in horizontal orientation x should is treaten as Y, y as X.
    QPointF movement;
    if (mOrientation == Qt::Vertical) {
        movement = offset;
    }
    else {
        movement.setX(offset.y());
        movement.setY(offset.x());
    }

    // If gesture is started check leading movement direction
    if (panGesture->state() == Qt::GestureStarted) {
        if (qAbs(movement.x()) < qAbs(movement.y())) {
            mPanDayDirection = ECalenPanVertical;
        }
        else {
            mStartPosition = contentWidget()->pos();
            mPanDayDirection = ECalenPanHorizontal;
        }
    }
}

/*!
 \brief Scrolls the contents to the newPosition in a given time.
 
 Sets the flag to indicate that scrolling is in progress. Use this function
 for scrolling with timeout > 0 to block gesture and focus events during
 scroll area movement.
 
 \param newPosition Destination position
 \param time Time of scroll movement
 */
void CalenDayContentScrollArea::moveTo(const QPointF &newPosition, int time)
{
    bool canMove(true);
    if (mDisallowedDirection != ECalenScrollNoDayChange) {
        canMove = (mMoveDirection != mDisallowedDirection);
    }
    
    if (canMove) {
        // Connect to scrollingEnded SIGNAL to get feedback when scrolling ends
        connect(this, SIGNAL(scrollingEnded()), this, SLOT(moveFinished()));
        
        // Scroll the content to new position and set isMoving flag
        scrollContentsTo(newPosition, time);
        mIsMoving = true;
        
        // Emit signal that moving has just started
        if (mMoveDirection != ECalenScrollNoDayChange) {
            emit scrollAreaMoveStarted(mMoveDirection);
        }
    }
}

/*!
 \brief isHorizontalSwipe
 
 \return TRUE if horizontal swipe was recognized (angle in specific range)
 */
bool CalenDayContentScrollArea::isHorizontalSwipe(qreal angle) const
{
    bool isHSwipe = false;
    if ((angle < KCalenSwipeAngle) || 
        ((angle > 180 - KCalenSwipeAngle) && (angle < 180 + KCalenSwipeAngle)) ||
        (angle > 360 - KCalenSwipeAngle)) {
        isHSwipe = true;
    }
    
    return isHSwipe;
}

/*!
 \brief Slot which is called when moving of scroll area is finished.
 
 Resets internal isMoving flag.
 */
void CalenDayContentScrollArea::moveFinished()
{
    // Disconnect from signal, move is finished now
    disconnect(this, SIGNAL(scrollingEnded()), this, SLOT(moveFinished()));
    mIsMoving = false;
    
    // Emit signal that moving has just finished and reset direction
    if (mMoveDirection != ECalenScrollNoDayChange) {
        emit scrollAreaMoveFinished(mMoveDirection);
        mMoveDirection = ECalenScrollNoDayChange;
    }
}

/*!
 \brief Slot which is called whenever the orientation of the device changes.
 
 Stores screen width and orientation in private members.
 
 \param orientation Current device orientation
 */
void CalenDayContentScrollArea::orientationChanged(Qt::Orientation orientation)
{
    // Update the width of content area
    mContentWidth = CalenDayUtils::instance()->contentWidth();
    mOrientation = orientation;

    // Fix the width of scroll area
    setMinimumWidth(mContentWidth);
    setMaximumWidth(mContentWidth);

    scrollToMiddleWidget();

    // Reset flag related to moving
    mPanDayDirection = ECalenPanNotSet;
    mMoveDirection = ECalenScrollNoDayChange;
    mIsMoving = false;
}

// End of File
