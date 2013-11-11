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
#include <hbcheckbox.h>
#include <QGraphicsSceneResizeEvent>

// NOTE! The following header include requires
//       INCLUDEPATH += /sf/mw/hb/include/hbcore/private
#include <hbstyleoptioncheckbox_p.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbCheckBox::HbCheckBox
// -----------------------------------------------------------------------------
//
HbCheckBox::HbCheckBox( 
        QGraphicsItem * parent )
    //:
    //HbAbstractButton( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbCheckBox::HbCheckBox
// -----------------------------------------------------------------------------
//
HbCheckBox::HbCheckBox( 
        const QString & text,
        QGraphicsItem * parent )
    //:
    //HbAbstractButton( /*text, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbCheckBox::~HbCheckBox
// -----------------------------------------------------------------------------
//
HbCheckBox::~HbCheckBox(  )
    {
    
    }

// -----------------------------------------------------------------------------
// HbCheckBox::setText
// -----------------------------------------------------------------------------
//
void HbCheckBox::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::text
// -----------------------------------------------------------------------------
//
QString HbCheckBox::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::setTristate
// -----------------------------------------------------------------------------
//
void HbCheckBox::setTristate( 
        bool isTristate )
    {
    SMC_MOCK_METHOD1( void, bool, isTristate )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::isTristate
// -----------------------------------------------------------------------------
//
bool HbCheckBox::isTristate(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::checkState
// -----------------------------------------------------------------------------
//
Qt::CheckState HbCheckBox::checkState(  ) const
    {
    SMC_MOCK_METHOD0( Qt::CheckState )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::primitive
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbCheckBox::primitive( 
        HbStyle::Primitive primitive ) const
    {
    SMC_MOCK_METHOD1( QGraphicsItem *, HbStyle::Primitive, primitive )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::setCheckState
// -----------------------------------------------------------------------------
//
void HbCheckBox::setCheckState( 
        Qt::CheckState state )
    {
    SMC_MOCK_METHOD1( void, Qt::CheckState, state )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::updatePrimitives
// -----------------------------------------------------------------------------
//
void HbCheckBox::updatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::initStyleOption
// -----------------------------------------------------------------------------
//
void HbCheckBox::initStyleOption( 
        HbStyleOptionCheckBox * option ) const
    {
    SMC_MOCK_METHOD1( void, HbStyleOptionCheckBox *, option )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::resizeEvent
// -----------------------------------------------------------------------------
//
void HbCheckBox::resizeEvent( 
        QGraphicsSceneResizeEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneResizeEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::hitButton
// -----------------------------------------------------------------------------
//
bool HbCheckBox::hitButton( 
        const QPointF & pos ) const
    {
    SMC_MOCK_METHOD1( bool, const QPointF &, pos )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::checkStateSet
// -----------------------------------------------------------------------------
//
void HbCheckBox::checkStateSet(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::nextCheckState
// -----------------------------------------------------------------------------
//
void HbCheckBox::nextCheckState(  )
    {
    SMC_MOCK_METHOD0( void )
    }


#ifndef HB_GESTURE_FW
// -----------------------------------------------------------------------------
// HbCheckBox::mouseReleaseEvent
// -----------------------------------------------------------------------------
//
void HbCheckBox::mouseReleaseEvent( 
        QGraphicsSceneMouseEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::mouseMoveEvent
// -----------------------------------------------------------------------------
//
void HbCheckBox::mouseMoveEvent( 
        QGraphicsSceneMouseEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }
#endif

// -----------------------------------------------------------------------------
// HbCheckBox::gestureEvent
// -----------------------------------------------------------------------------
//
void HbCheckBox::gestureEvent( 
        QGestureEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGestureEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::keyPressEvent
// -----------------------------------------------------------------------------
//
void HbCheckBox::keyPressEvent( 
        QKeyEvent * keyEvent )
    {
    SMC_MOCK_METHOD1( void, QKeyEvent *, keyEvent )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbCheckBox::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
    SMC_MOCK_METHOD2( QVariant, GraphicsItemChange, change, 
        const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// HbCheckBox::stateChanged
// -----------------------------------------------------------------------------
//
void HbCheckBox::stateChanged( 
        int state )
    {
    SMC_MOCK_METHOD1( void, int, state )
    }


