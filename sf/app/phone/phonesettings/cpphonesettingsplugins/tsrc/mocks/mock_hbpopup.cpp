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
#include <hbpopup.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbPopup::HbPopup
// -----------------------------------------------------------------------------
//
HbPopup::HbPopup( 
        QGraphicsItem * parent )
    //:
    //HbWidget( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbPopup::Q_ENUMS
// -----------------------------------------------------------------------------
//
/*
void HbPopup::Q_ENUMS( 
        DefaultTimeout DismissPolicy FrameType ) Q_PROPERTY ( int timeout READ timeout WRITE setTimeout ) Q_PROPERTY ( bool modal READ isModal WRITE setModal ) Q_PROPERTY ( bool backgroundFaded READ isBackgroundFaded WRITE setBackgroundFaded ) Q_PROPERTY ( DismissPolicy dismissPolicy READ dismissPolicy WRITE setDismissPolicy ) Q_PROPERTY ( FrameType frameType READ frameType WRITE setFrameType )
    {
    SMC_MOCK_METHOD1( void, DefaultTimeout DismissPolicy FrameType ) Q_PROPERTY ( int timeout READ timeout WRITE setTimeout ) Q_PROPERTY ( bool modal READ isModal WRITE setModal ) Q_PROPERTY ( bool backgroundFaded READ isBackgroundFaded WRITE setBackgroundFaded ) Q_PROPERTY ( DismissPolicy dismissPolicy READ dismissPolicy WRITE setDismissPolicy ) Q_PROPERTY ( FrameType frameType READ frameType WRITE, setFrameType )
    }
*/

// -----------------------------------------------------------------------------
// HbPopup::~HbPopup
// -----------------------------------------------------------------------------
//
HbPopup::~HbPopup(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbPopup::timeout
// -----------------------------------------------------------------------------
//
int HbPopup::timeout(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbPopup::setTimeout
// -----------------------------------------------------------------------------
//
void HbPopup::setTimeout( 
        int timeout )
    {
    SMC_MOCK_METHOD1( void, int, timeout )
    }


// -----------------------------------------------------------------------------
// HbPopup::setTimeout
// -----------------------------------------------------------------------------
//
void HbPopup::setTimeout( 
        HbPopup::DefaultTimeout timeout )
    {
    SMC_MOCK_METHOD1( void, HbPopup::DefaultTimeout, timeout )
    }


// -----------------------------------------------------------------------------
// HbPopup::isModal
// -----------------------------------------------------------------------------
//
bool HbPopup::isModal(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbPopup::setModal
// -----------------------------------------------------------------------------
//
void HbPopup::setModal( 
        bool enabled )
    {
    SMC_MOCK_METHOD1( void, bool, enabled )
    }


// -----------------------------------------------------------------------------
// HbPopup::setBackgroundFaded
// -----------------------------------------------------------------------------
//
void HbPopup::setBackgroundFaded( 
        bool fadeBackground )
    {
    SMC_MOCK_METHOD1( void, bool, fadeBackground )
    }


// -----------------------------------------------------------------------------
// HbPopup::isBackgroundFaded
// -----------------------------------------------------------------------------
//
bool HbPopup::isBackgroundFaded(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbPopup::dismissPolicy
// -----------------------------------------------------------------------------
//
HbPopup::DismissPolicy HbPopup::dismissPolicy(  ) const
    {
    SMC_MOCK_METHOD0( DismissPolicy )
    }


// -----------------------------------------------------------------------------
// HbPopup::setDismissPolicy
// -----------------------------------------------------------------------------
//
void HbPopup::setDismissPolicy( 
        HbPopup::DismissPolicy dismissPolicy )
    {
    SMC_MOCK_METHOD1( void, HbPopup::DismissPolicy, dismissPolicy )
    }


// -----------------------------------------------------------------------------
// HbPopup::frameType
// -----------------------------------------------------------------------------
//
HbPopup::FrameType HbPopup::frameType(  ) const
    {
    SMC_MOCK_METHOD0( FrameType )
    }


// -----------------------------------------------------------------------------
// HbPopup::setFrameType
// -----------------------------------------------------------------------------
//
void HbPopup::setFrameType( 
        HbPopup::FrameType frameType )
    {
    SMC_MOCK_METHOD1( void, HbPopup::FrameType, frameType )
    }


// -----------------------------------------------------------------------------
// HbPopup::type
// -----------------------------------------------------------------------------
//
/*
int HbPopup::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }
*/

// -----------------------------------------------------------------------------
// HbPopup::setPreferredPos
// -----------------------------------------------------------------------------
//
void HbPopup::setPreferredPos( 
        const QPointF & position,
        HbPopup::Placement placement )
    {
    //SMC_MOCK_METHOD2( void, const QPointF &, position, 
      //  HbPopup::Placement, placement )
    }


// -----------------------------------------------------------------------------
// HbPopup::aboutToShow
// -----------------------------------------------------------------------------
//
void HbPopup::aboutToShow(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbPopup::aboutToHide
// -----------------------------------------------------------------------------
//
void HbPopup::aboutToHide(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbPopup::aboutToClose
// -----------------------------------------------------------------------------
//
void HbPopup::aboutToClose(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbPopup::HbPopup
// -----------------------------------------------------------------------------
//
HbPopup::HbPopup( 
        HbPopupPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbWidget( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbPopup::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbPopup::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
  //  SMC_MOCK_METHOD2( QVariant, GraphicsItemChange, change, 
    //    const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// HbPopup::mousePressEvent
// -----------------------------------------------------------------------------
//
void HbPopup::mousePressEvent( 
        QGraphicsSceneMouseEvent * event )
    {
 //   SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbPopup::mouseReleaseEvent
// -----------------------------------------------------------------------------
//
void HbPopup::mouseReleaseEvent( 
        QGraphicsSceneMouseEvent * event )
    {
   // SMC_MOCK_METHOD1( void, QGraphicsSceneMouseEvent *, event )
    }




// -----------------------------------------------------------------------------
// HbPopup::showEvent
// -----------------------------------------------------------------------------
//
void HbPopup::showEvent( 
        QShowEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QShowEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbPopup::hideEvent
// -----------------------------------------------------------------------------
//
void HbPopup::hideEvent( 
        QHideEvent * event )
    {
    //SMC_MOCK_METHOD1( void, QHideEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbPopup::resizeEvent
// -----------------------------------------------------------------------------
//
void HbPopup::resizeEvent( 
        QGraphicsSceneResizeEvent * event )
    {
  //  SMC_MOCK_METHOD1( void, QGraphicsSceneResizeEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbPopup::closeEvent
// -----------------------------------------------------------------------------
//
void HbPopup::closeEvent( 
        QCloseEvent * event )
    {
  //  SMC_MOCK_METHOD1( void, QCloseEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbPopup::event
// -----------------------------------------------------------------------------
//
bool HbPopup::event( 
        QEvent * event )
    {
  //  SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


