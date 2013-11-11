/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/
#include <QDebug>
#include <smcmockclassincludes.h>
#include "qgesture.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QSwipeGesture::QSwipeGesture
// -----------------------------------------------------------------------------
//
QSwipeGesture::QSwipeGesture( 
        QObject * parent )
    //:
    //QGesture( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QSwipeGesture::horizontalDirection
// -----------------------------------------------------------------------------
//
QSwipeGesture::SwipeDirection QSwipeGesture::horizontalDirection(  ) const
    {
    SMC_MOCK_METHOD0( QSwipeGesture::SwipeDirection )
    }


// -----------------------------------------------------------------------------
// QSwipeGesture::verticalDirection
// -----------------------------------------------------------------------------
//
QSwipeGesture::SwipeDirection QSwipeGesture::verticalDirection(  ) const
    {
    SMC_MOCK_METHOD0( QSwipeGesture::SwipeDirection )
    }


// -----------------------------------------------------------------------------
// QSwipeGesture::swipeAngle
// -----------------------------------------------------------------------------
//
qreal QSwipeGesture::swipeAngle(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QSwipeGesture::setSwipeAngle
// -----------------------------------------------------------------------------
//
void QSwipeGesture::setSwipeAngle( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QTapAndHoldGesture::position
// -----------------------------------------------------------------------------
//
QPointF QTapAndHoldGesture::position(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QTapAndHoldGesture::setPosition
// -----------------------------------------------------------------------------
//
void QTapAndHoldGesture::setPosition( 
        const QPointF & pos )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// QTapAndHoldGesture::setTimeout
// -----------------------------------------------------------------------------
//
void QTapAndHoldGesture::setTimeout( 
        int msecs )
    {
    SMC_MOCK_METHOD1( void, int, msecs )
    }


// -----------------------------------------------------------------------------
// QTapAndHoldGesture::timeout
// -----------------------------------------------------------------------------
//
int QTapAndHoldGesture::timeout(  )
    {
    SMC_MOCK_METHOD0( int )
    }

// -----------------------------------------------------------------------------
// QPanGesture::lastOffset
// -----------------------------------------------------------------------------
//
QPointF QPanGesture::lastOffset(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPanGesture::offset
// -----------------------------------------------------------------------------
//
QPointF QPanGesture::offset(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPanGesture::delta
// -----------------------------------------------------------------------------
//
QPointF QPanGesture::delta(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPanGesture::acceleration
// -----------------------------------------------------------------------------
//
qreal QPanGesture::acceleration(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPanGesture::setLastOffset
// -----------------------------------------------------------------------------
//
void QPanGesture::setLastOffset( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QPanGesture::setOffset
// -----------------------------------------------------------------------------
//
void QPanGesture::setOffset( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QPanGesture::setAcceleration
// -----------------------------------------------------------------------------
//
void QPanGesture::setAcceleration( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QTapGesture::position
// -----------------------------------------------------------------------------
//
QPointF QTapGesture::position(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QTapGesture::setPosition
// -----------------------------------------------------------------------------
//
void QTapGesture::setPosition( 
        const QPointF & pos )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::QPinchGesture
// -----------------------------------------------------------------------------
//
QPinchGesture::QPinchGesture( 
        QObject * parent )
    //:
    //QGesture( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QPinchGesture::totalChangeFlags
// -----------------------------------------------------------------------------
//
QPinchGesture::ChangeFlags QPinchGesture::totalChangeFlags(  ) const
    {
    SMC_MOCK_METHOD0( QPinchGesture::ChangeFlags )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setTotalChangeFlags
// -----------------------------------------------------------------------------
//
void QPinchGesture::setTotalChangeFlags( 
        ChangeFlags value )
    {
    //SMC_MOCK_METHOD1( void, ChangeFlags, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::changeFlags
// -----------------------------------------------------------------------------
//
QPinchGesture::ChangeFlags QPinchGesture::changeFlags(  ) const
    {
    SMC_MOCK_METHOD0( QPinchGesture::ChangeFlags )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setChangeFlags
// -----------------------------------------------------------------------------
//
void QPinchGesture::setChangeFlags( 
        ChangeFlags value )
    {
    //SMC_MOCK_METHOD1( void, ChangeFlags, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::startCenterPoint
// -----------------------------------------------------------------------------
//
QPointF QPinchGesture::startCenterPoint(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::lastCenterPoint
// -----------------------------------------------------------------------------
//
QPointF QPinchGesture::lastCenterPoint(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::centerPoint
// -----------------------------------------------------------------------------
//
QPointF QPinchGesture::centerPoint(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setStartCenterPoint
// -----------------------------------------------------------------------------
//
void QPinchGesture::setStartCenterPoint( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setLastCenterPoint
// -----------------------------------------------------------------------------
//
void QPinchGesture::setLastCenterPoint( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setCenterPoint
// -----------------------------------------------------------------------------
//
void QPinchGesture::setCenterPoint( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::totalScaleFactor
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::totalScaleFactor(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::lastScaleFactor
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::lastScaleFactor(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::scaleFactor
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::scaleFactor(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setTotalScaleFactor
// -----------------------------------------------------------------------------
//
void QPinchGesture::setTotalScaleFactor( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setLastScaleFactor
// -----------------------------------------------------------------------------
//
void QPinchGesture::setLastScaleFactor( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setScaleFactor
// -----------------------------------------------------------------------------
//
void QPinchGesture::setScaleFactor( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::totalRotationAngle
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::totalRotationAngle(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::lastRotationAngle
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::lastRotationAngle(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::rotationAngle
// -----------------------------------------------------------------------------
//
qreal QPinchGesture::rotationAngle(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setTotalRotationAngle
// -----------------------------------------------------------------------------
//
void QPinchGesture::setTotalRotationAngle( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setLastRotationAngle
// -----------------------------------------------------------------------------
//
void QPinchGesture::setLastRotationAngle( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QPinchGesture::setRotationAngle
// -----------------------------------------------------------------------------
//
void QPinchGesture::setRotationAngle( 
        qreal value )
    {
    SMC_MOCK_METHOD1( void, qreal, value )
    }


// -----------------------------------------------------------------------------
// QGesture::QGesture
// -----------------------------------------------------------------------------
//
QGesture::QGesture( 
        QGesturePrivate & dd,
        QObject * parent )
    //:
    //QObject( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGesture::~QGesture
// -----------------------------------------------------------------------------
//
QGesture::~QGesture(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGesture::gestureType
// -----------------------------------------------------------------------------
//
Qt::GestureType QGesture::gestureType(  ) const
    {
    SMC_MOCK_METHOD0( Qt::GestureType )
    }


// -----------------------------------------------------------------------------
// QGesture::state
// -----------------------------------------------------------------------------
//
Qt::GestureState QGesture::state(  ) const
    {
    SMC_MOCK_METHOD0( Qt::GestureState )
    }


// -----------------------------------------------------------------------------
// QGesture::hotSpot
// -----------------------------------------------------------------------------
//
QPointF QGesture::hotSpot(  ) const
    {
    SMC_MOCK_METHOD0( QPointF )
    }


// -----------------------------------------------------------------------------
// QGesture::setHotSpot
// -----------------------------------------------------------------------------
//
void QGesture::setHotSpot( 
        const QPointF & value )
    {
    SMC_MOCK_METHOD1( void, const QPointF &, value )
    }


// -----------------------------------------------------------------------------
// QGesture::hasHotSpot
// -----------------------------------------------------------------------------
//
bool QGesture::hasHotSpot(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QGesture::unsetHotSpot
// -----------------------------------------------------------------------------
//
void QGesture::unsetHotSpot(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGesture::setGestureCancelPolicy
// -----------------------------------------------------------------------------
//
void QGesture::setGestureCancelPolicy( 
        GestureCancelPolicy policy )
    {
    //SMC_MOCK_METHOD1( void, GestureCancelPolicy, policy )
    }


// -----------------------------------------------------------------------------
// QGesture::gestureCancelPolicy
// -----------------------------------------------------------------------------
//
QGesture::GestureCancelPolicy QGesture::gestureCancelPolicy(  ) const
    {
    SMC_MOCK_METHOD0( QGesture::GestureCancelPolicy )
    }


