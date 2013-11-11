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
 * Description:  CalenDayContentScrollArea class definition.
 *
 */

#ifndef CALENDAYCONTENTSCROLLAREA_H_
#define CALENDAYCONTENTSCROLLAREA_H_

// System includes
#include <HbScrollArea>

// User includes
#include "calendaycommonheaders.h"

/*!
 Value [degree] defines the max. angle of swipe gesture which should change day.
 */
const qreal KCalenSwipeAngle = 30;

#ifdef  CALENVIEWS_DLL
#define CALENDAYCONTENTSCROLLAREA_EXPORT Q_DECL_EXPORT
#else
#define CALENDAYCONTENTSCROLLAREA_EXPORT Q_DECL_IMPORT
#endif

// Class declaration
class CALENDAYCONTENTSCROLLAREA_EXPORT CalenDayContentScrollArea : public HbScrollArea
{
Q_OBJECT

public:
    CalenDayContentScrollArea(QGraphicsItem *parent = 0);
    virtual ~CalenDayContentScrollArea();
    
public:
    /*!
     \enum CalenPanDirection
     \brief Enumeration identifies pan gesture directions.
     */
    enum CalenPanDirection
    {
        ECalenPanNotSet = 0,
        ECalenPanVertical,
        ECalenPanHorizontal
    };
    
    CalenScrollDirection disallowedScrollDirection() const;
    void setDisallowedScrollDirection(const CalenScrollDirection direction); 

signals:
    void scrollAreaMoveStarted(CalenScrollDirection scrollTo);
    void scrollAreaMoveFinished(CalenScrollDirection scrollTo);

public slots:
    void scrollToMiddleWidget();

protected:
    bool scrollByAmount(const QPointF &delta);
    
    void gestureEvent(QGestureEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);
    bool event(QEvent *e);
    
private:	// private functions
    void checkPanDirection(QPanGesture *panGesture);
    void moveTo(const QPointF &newPosition, int time = 0);
    bool isHorizontalSwipe(qreal angle) const;
    
private slots:
    void moveFinished();
    void orientationChanged(Qt::Orientation orientation);

private:    // data
    QPointF mStartPosition;  //!< Start position of movement

    qreal mContentWidth; //!< Content width
    
    Qt::Orientation mOrientation;   //!< Stores current orientation

    /*!
     \brief Pan gesture direction indicator
     
     Member is used when switching widgets by panning gesture is enabled.
     */
    CalenPanDirection mPanDayDirection;
    
    bool mIsMoving; //!< Indicates if moving of scroll area is in progress
    
    /*!
     \brief Scroll area horizontal move direction
     */
    CalenScrollDirection mMoveDirection;
    
    /*!
     \brief Scroll area horizontal move disallowed direction
     */
    CalenScrollDirection mDisallowedDirection;
};

#endif /* CALENDAYCONTENTSCROLLAREA_H_ */
