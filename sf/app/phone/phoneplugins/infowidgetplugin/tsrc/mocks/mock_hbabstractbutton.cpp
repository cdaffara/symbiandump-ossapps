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
#include "hbabstractbutton.h"
#include <HbStylePrimitiveData>

// NOTE! The following header include requires
//       INCLUDEPATH += /sf/mw/hb/include/hbcore/private
#include <hbstyleoption_p.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbAbstractButton::HbAbstractButton
// -----------------------------------------------------------------------------
//
HbAbstractButton::HbAbstractButton( 
        QGraphicsItem * parent )
    //:
    //HbWidget( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::~HbAbstractButton
// -----------------------------------------------------------------------------
//
HbAbstractButton::~HbAbstractButton(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setCheckable
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setCheckable( bool checkable )
    {
    SMC_MOCK_METHOD1( void, bool, checkable )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::isCheckable
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::isCheckable(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::isChecked
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::isChecked(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setDown
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setDown( bool set )
    {
    SMC_MOCK_METHOD1( void, bool, set )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::isDown
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::isDown(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setAutoRepeat
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setAutoRepeat( bool set )
    {
    SMC_MOCK_METHOD1( void, bool, set )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::autoRepeat
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::autoRepeat(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setAutoRepeatDelay
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setAutoRepeatDelay( int delay )
    {
    SMC_MOCK_METHOD1( void, int, delay )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::autoRepeatDelay
// -----------------------------------------------------------------------------
//
int HbAbstractButton::autoRepeatDelay(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setAutoRepeatInterval
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setAutoRepeatInterval( int interval )
    {
    SMC_MOCK_METHOD1( void, int, interval )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::autoRepeatInterval
// -----------------------------------------------------------------------------
//
int HbAbstractButton::autoRepeatInterval(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setAutoExclusive
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setAutoExclusive( bool exclusive )
    {
    SMC_MOCK_METHOD1( void, bool, exclusive )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::autoExclusive
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::autoExclusive(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::animateClick
// -----------------------------------------------------------------------------
//
void HbAbstractButton::animateClick( 
        int msec )
    {
    SMC_MOCK_METHOD1( void, int, msec )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::click
// -----------------------------------------------------------------------------
//
void HbAbstractButton::click(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::toggle
// -----------------------------------------------------------------------------
//
void HbAbstractButton::toggle(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::setChecked
// -----------------------------------------------------------------------------
//
void HbAbstractButton::setChecked( bool checked )
    {
    SMC_MOCK_METHOD1( void, bool, checked )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::pressed
// -----------------------------------------------------------------------------
//
/*void HbAbstractButton::pressed(  )
    {
    SMC_MOCK_METHOD0( void )
    }*/


// -----------------------------------------------------------------------------
// HbAbstractButton::released
// -----------------------------------------------------------------------------
//
/*void HbAbstractButton::released(  )
    {
    SMC_MOCK_METHOD0( void )
    }*/


// -----------------------------------------------------------------------------
// HbAbstractButton::clicked
// -----------------------------------------------------------------------------
//
/*void HbAbstractButton::clicked( 
        bool checked )
    {
    SMC_MOCK_METHOD1( void, bool, checked )
    }*/


// -----------------------------------------------------------------------------
// HbAbstractButton::toggled
// -----------------------------------------------------------------------------
//
/*void HbAbstractButton::toggled( 
        bool checked )
    {
    SMC_MOCK_METHOD1( void, bool, checked )
    }*/


// -----------------------------------------------------------------------------
// HbAbstractButton::HbAbstractButton
// -----------------------------------------------------------------------------
//
HbAbstractButton::HbAbstractButton( 
        HbAbstractButtonPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbWidget( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::initStyleOption
// -----------------------------------------------------------------------------
//
void HbAbstractButton::initStyleOption( 
        HbStyleOption * option ) const
    {
    SMC_MOCK_METHOD1( void, HbStyleOption *, option )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::initPrimitiveData
// -----------------------------------------------------------------------------
//
void HbAbstractButton::initPrimitiveData( 
        HbStylePrimitiveData * primitiveData,
        const QGraphicsObject * primitive )
    {
    SMC_MOCK_METHOD2( void, HbStylePrimitiveData *, primitiveData, 
        const QGraphicsObject *, primitive )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::hitButton
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::hitButton( 
        const QPointF & pos ) const
    {
    SMC_MOCK_METHOD1( bool, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::checkStateSet
// -----------------------------------------------------------------------------
//
void HbAbstractButton::checkStateSet(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::nextCheckState
// -----------------------------------------------------------------------------
//
void HbAbstractButton::nextCheckState(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::event
// -----------------------------------------------------------------------------
//
bool HbAbstractButton::event( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::keyPressEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::keyPressEvent( 
        QKeyEvent * event )
    {
    SMC_MOCK_METHOD1( void, QKeyEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::keyReleaseEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::keyReleaseEvent( 
        QKeyEvent * event )
    {
    SMC_MOCK_METHOD1( void, QKeyEvent *, event )
    }


#ifndef HB_GESTURE_FW
// -----------------------------------------------------------------------------
// HbAbstractButton::mousePressEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::mousePressEvent( 
        QGraphicsSceneMouseEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::mouseReleaseEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::mouseReleaseEvent( 
        QGraphicsSceneMouseEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::mouseMoveEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::mouseMoveEvent( 
        QGraphicsSceneMouseEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }
#endif

// -----------------------------------------------------------------------------
// HbAbstractButton::focusInEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::focusInEvent( 
        QFocusEvent * event )
    {
    SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::changeEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::changeEvent( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::timerEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::timerEvent( 
        QTimerEvent * event )
    {
    SMC_MOCK_METHOD1( void, QTimerEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF HbAbstractButton::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {
    SMC_MOCK_METHOD2( QSizeF, Qt::SizeHint, which, 
        const QSizeF &, constraint )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::polish
// -----------------------------------------------------------------------------
//
void HbAbstractButton::polish( 
        HbStyleParameters & params )
    {
    SMC_MOCK_METHOD1( void, HbStyleParameters &, params )
    }


// -----------------------------------------------------------------------------
// HbAbstractButton::gestureEvent
// -----------------------------------------------------------------------------
//
void HbAbstractButton::gestureEvent( 
        QGestureEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGestureEvent *, event )
    }


