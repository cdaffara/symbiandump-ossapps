/* 
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <QDebug>
#include <smcmockclassincludes.h>
#include <hbwidget.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbWidget::HbWidget
// -----------------------------------------------------------------------------
//
HbWidget::HbWidget( 
        QGraphicsItem * parent,
        Qt::WindowFlags wFlags )
    //:
    //HbWidgetBase( /*parent, wFlags*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbWidget::Q_ENUMS
// -----------------------------------------------------------------------------
//
/*
void HbWidget::Q_ENUMS( 
        FocusDelegation FocusMode ) Q_PROPERTY ( QString toolTipText READ toolTip WRITE setToolTip ) Q_PROPERTY ( HbWidget::FocusDelegation focusDelegation READ focusDelegation WRITE setFocusDelegation ) Q_PROPERTY ( bool focusLooping READ hasFocusLooping WRITE setFocusLooping ) Q_PROPERTY ( HbWidget::FocusMode focusMode READ focusMode WRITE setFocusMode )
    {
    SMC_MOCK_METHOD1( void, FocusDelegation FocusMode ) Q_PROPERTY ( QString toolTipText READ toolTip WRITE setToolTip ) Q_PROPERTY ( HbWidget::FocusDelegation focusDelegation READ focusDelegation WRITE setFocusDelegation ) Q_PROPERTY ( bool focusLooping READ hasFocusLooping WRITE setFocusLooping ) Q_PROPERTY ( HbWidget::FocusMode focusMode READ focusMode WRITE, setFocusMode )
    }
*/

// -----------------------------------------------------------------------------
// HbWidget::~HbWidget
// -----------------------------------------------------------------------------
//
HbWidget::~HbWidget(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbWidget::event
// -----------------------------------------------------------------------------
//
bool HbWidget::event( 
        QEvent * e )
    {
  //  SMC_MOCK_METHOD1( bool, QEvent *, e )
    }


// -----------------------------------------------------------------------------
// HbWidget::setStyle
// -----------------------------------------------------------------------------
//
void HbWidget::setStyle( 
        HbStyle * style )
    {
 //   SMC_MOCK_METHOD1( void, HbStyle *, style )
    }


// -----------------------------------------------------------------------------
// HbWidget::style
// -----------------------------------------------------------------------------
//
HbStyle * HbWidget::style(  ) const
    {
   // SMC_MOCK_METHOD0( HbStyle * )
    }


// -----------------------------------------------------------------------------
// HbWidget::clearActions
// -----------------------------------------------------------------------------
//
void HbWidget::clearActions(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidget::type
// -----------------------------------------------------------------------------
/*
int HbWidget::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }
*/

// -----------------------------------------------------------------------------
// HbWidget::pluginBaseId
// -----------------------------------------------------------------------------
//
/*int HbWidget::pluginBaseId(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }*/


// -----------------------------------------------------------------------------
// HbWidget::mainWindow
// -----------------------------------------------------------------------------
//
HbMainWindow * HbWidget::mainWindow(  ) const
    {
   // SMC_MOCK_METHOD0( HbMainWindow * )
    return 0;
    }


// -----------------------------------------------------------------------------
// HbWidget::primitive
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbWidget::primitive( 
        HbStyle::Primitive  ) const
    {
  //  SMC_MOCK_METHOD1( QGraphicsItem *, HbStyle::Primitive, primitive )
    return 0;
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusOrientation
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusOrientation( 
        Qt::Orientations ,
        Qt::Orientations  )
    {
   // SMC_MOCK_METHOD2( void, Qt::Orientations, previous, 
     //   Qt::Orientations, next )
    }


// -----------------------------------------------------------------------------
// HbWidget::keyForFocusNextChild
// -----------------------------------------------------------------------------
//
QKeySequence HbWidget::keyForFocusNextChild(  ) const
    {
   // SMC_MOCK_METHOD0( QKeySequence )
    return QKeySequence();
    }


// -----------------------------------------------------------------------------
// HbWidget::keyForFocusPreviousChild
// -----------------------------------------------------------------------------
//
QKeySequence HbWidget::keyForFocusPreviousChild(  ) const
    {
  //  SMC_MOCK_METHOD0( QKeySequence )
    return QKeySequence();
    }


// -----------------------------------------------------------------------------
// HbWidget::setKeyFocusMode
// -----------------------------------------------------------------------------
//
void HbWidget::setKeyFocusMode( 
        const QKeySequence &  )
    {
//    SMC_MOCK_METHOD1( void, const QKeySequence &, key )
    }


// -----------------------------------------------------------------------------
// HbWidget::keyFocusMode
// -----------------------------------------------------------------------------
//
QKeySequence HbWidget::keyFocusMode(  ) const
    {
//    SMC_MOCK_METHOD0( QKeySequence )
    return QKeySequence();
    }


// -----------------------------------------------------------------------------
// HbWidget::lastFocusedChild
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbWidget::lastFocusedChild(  ) const
    {
 //   SMC_MOCK_METHOD0( QGraphicsItem * )
    return 0;
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusOrder
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusOrder( 
        QGraphicsItem * ,
        QGraphicsItem *  )
    {
   // SMC_MOCK_METHOD2( void, QGraphicsItem *, first, 
     //   QGraphicsItem *, second )
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusDelegation
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusDelegation( 
        HbWidget::FocusDelegation  )
    {
 //   SMC_MOCK_METHOD1( void, HbWidget::FocusDelegation, focusDelegation )
    }


// -----------------------------------------------------------------------------
// HbWidget::focusDelegation
// -----------------------------------------------------------------------------
//
HbWidget::FocusDelegation HbWidget::focusDelegation(  ) const
    {
  //  SMC_MOCK_METHOD0( HbWidget::FocusDelegation )
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusLooping
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusLooping( 
        bool enable )
    {
    SMC_MOCK_METHOD1( void, bool, enable )
    }


// -----------------------------------------------------------------------------
// HbWidget::hasFocusLooping
// -----------------------------------------------------------------------------
//
bool HbWidget::hasFocusLooping(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusMode
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusMode( 
        HbWidget:: FocusMode )
    {
   // SMC_MOCK_METHOD1( void, HbWidget::, FocusMode )
    }


// -----------------------------------------------------------------------------
// HbWidget::focusMode
// -----------------------------------------------------------------------------
//
HbWidget::FocusMode HbWidget::focusMode(  ) const
    {
  //  SMC_MOCK_METHOD0( HbWidget::FocusMode )
    }


// -----------------------------------------------------------------------------
// HbWidget::setFocusHighlight
// -----------------------------------------------------------------------------
//
void HbWidget::setFocusHighlight( 
        HbStyle::Primitive ,
        HbWidget::FocusHighlight  )
    {
  //  SMC_MOCK_METHOD2( void, HbStyle::Primitive, type, 
    //    HbWidget::FocusHighlight, focusHighlight )
    }


// -----------------------------------------------------------------------------
// HbWidget::focusHighlight
// -----------------------------------------------------------------------------
//
HbStyle::Primitive HbWidget::focusHighlight( 
        HbWidget::FocusHighlight  )
    {
 //   SMC_MOCK_METHOD1( HbStyle::Primitive, HbWidget::FocusHighlight, focusHighlight )
    }


// -----------------------------------------------------------------------------
// HbWidget::setBackgroundItem
// -----------------------------------------------------------------------------
//
void HbWidget::setBackgroundItem( 
        QGraphicsItem * ,
        int  )
    {
   // SMC_MOCK_METHOD2( void, QGraphicsItem *, item, 
     //   int, zValue )
    }


// -----------------------------------------------------------------------------
// HbWidget::backgroundItem
// -----------------------------------------------------------------------------
//
QGraphicsItem * HbWidget::backgroundItem(  ) const
    {
   // SMC_MOCK_METHOD0( QGraphicsItem * )
    }


// -----------------------------------------------------------------------------
// HbWidget::recreatePrimitives
// -----------------------------------------------------------------------------
//
void HbWidget::recreatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidget::updatePrimitives
// -----------------------------------------------------------------------------
//
void HbWidget::updatePrimitives(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidget::focusInEvent
// -----------------------------------------------------------------------------
//
void HbWidget::focusInEvent( 
        QFocusEvent * event )
    {
 //   SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbWidget::focusOutEvent
// -----------------------------------------------------------------------------
//
void HbWidget::focusOutEvent( 
        QFocusEvent * event )
    {
  //  SMC_MOCK_METHOD1( void, QFocusEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbWidget::resizeEvent
// -----------------------------------------------------------------------------
//
void HbWidget::resizeEvent( 
        QGraphicsSceneResizeEvent * event )
    {
  //  SMC_MOCK_METHOD1( void, QGraphicsSceneResizeEvent *, event )
    }



// -----------------------------------------------------------------------------
// HbWidget::changeEvent
// -----------------------------------------------------------------------------
//
void HbWidget::changeEvent( 
        QEvent * event )
    {
   // SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbWidget::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbWidget::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
 //   SMC_MOCK_METHOD2( QVariant, GraphicsItemChange, change, 
   //     const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// HbWidget::polishEvent
// -----------------------------------------------------------------------------
//
void HbWidget::polishEvent(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidget::polish
// -----------------------------------------------------------------------------
//
void HbWidget::polish( 
        HbStyleParameters & params )
    {
 //   SMC_MOCK_METHOD1( void, HbStyleParameters &, params )
    }


// -----------------------------------------------------------------------------
// HbWidget::repolish
// -----------------------------------------------------------------------------
//
void HbWidget::repolish(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidget::setPluginBaseId
// -----------------------------------------------------------------------------
//
/*void HbWidget::setPluginBaseId( 
        int baseId )
    {
    SMC_MOCK_METHOD1( void, int, baseId )
    }*/




// -----------------------------------------------------------------------------
// HbWidget::HbWidget
// -----------------------------------------------------------------------------
//
HbWidget::HbWidget( 
        HbWidgetPrivate & /*dd*/,
        QGraphicsItem * /*parent*/,
        Qt::WindowFlags /*wFlags*/ )
    //:
    //HbWidgetBase( /*dd, parent, wFlags*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbWidget::hideFocusHighlight
// -----------------------------------------------------------------------------
//
void HbWidget::hideFocusHighlight()
    {
    SMC_MOCK_METHOD0( void )
    }

 
