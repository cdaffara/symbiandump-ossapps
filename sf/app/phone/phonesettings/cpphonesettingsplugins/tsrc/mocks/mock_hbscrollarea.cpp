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
#include <hbscrollarea.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbScrollArea::HbScrollArea
// -----------------------------------------------------------------------------
//
HbScrollArea::HbScrollArea( 
        HbScrollAreaPrivate & dd,
        QGraphicsItem * parent )
    //:
    //HbWidget( /*dd, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbScrollArea::Q_PROPERTY
// -----------------------------------------------------------------------------
//
/*
void HbScrollArea::Q_PROPERTY( 
        Qt::Orientations scrollDirections READ scrollDirections WRITE setScrollDirections ) Q_PROPERTY ( ClampingStyle clampingStyle READ clampingStyle WRITE setClampingStyle ) Q_PROPERTY ( ScrollingStyle scrollingStyle READ scrollingStyle WRITE setScrollingStyle ) Q_PROPERTY ( bool frictionEnabled READ frictionEnabled WRITE setFrictionEnabled ) Q_PROPERTY ( bool longPressEnabled READ longPressEnabled WRITE setLongPressEnabled ) Q_PROPERTY ( ScrollBarPolicy verticalScrollBarPolicy READ verticalScrollBarPolicy WRITE setVerticalScrollBarPolicy ) Q_PROPERTY ( ScrollBarPolicy horizontalScrollBarPolicy READ horizontalScrollBarPolicy WRITE setHorizontalScrollBarPolicy ) Q_PROPERTY ( Qt::Alignment alignment READ alignment WRITE setAlignment ) Q_PROPERTY ( bool continuationIndicators READ continuationIndicators WRITE setContinuationIndicators ) Q_ENUMS ( ClampingStyle ) Q_ENUMS ( ScrollingStyle ) Q_ENUMS ( ScrollBarPolicy ):HbScrollArea ( QGraphicsItem * parent )
    {
    SMC_MOCK_METHOD1( void, Qt::Orientations scrollDirections READ scrollDirections WRITE setScrollDirections ) Q_PROPERTY ( ClampingStyle clampingStyle READ clampingStyle WRITE setClampingStyle ) Q_PROPERTY ( ScrollingStyle scrollingStyle READ scrollingStyle WRITE setScrollingStyle ) Q_PROPERTY ( bool frictionEnabled READ frictionEnabled WRITE setFrictionEnabled ) Q_PROPERTY ( bool longPressEnabled READ longPressEnabled WRITE setLongPressEnabled ) Q_PROPERTY ( ScrollBarPolicy verticalScrollBarPolicy READ verticalScrollBarPolicy WRITE setVerticalScrollBarPolicy ) Q_PROPERTY ( ScrollBarPolicy horizontalScrollBarPolicy READ horizontalScrollBarPolicy WRITE setHorizontalScrollBarPolicy ) Q_PROPERTY ( Qt::Alignment alignment READ alignment WRITE setAlignment ) Q_PROPERTY ( bool continuationIndicators READ continuationIndicators WRITE setContinuationIndicators ) Q_ENUMS ( ClampingStyle ) Q_ENUMS ( ScrollingStyle ) Q_ENUMS ( ScrollBarPolicy ):HbScrollArea ( QGraphicsItem *, parent )
    }*/


// -----------------------------------------------------------------------------
// HbScrollArea::~HbScrollArea
// -----------------------------------------------------------------------------
//
HbScrollArea::~HbScrollArea(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbScrollArea::contentWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbScrollArea::contentWidget(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setContentWidget
// -----------------------------------------------------------------------------
//
void HbScrollArea::setContentWidget( 
        QGraphicsWidget * widget )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::takeContentWidget
// -----------------------------------------------------------------------------
//
QGraphicsWidget * HbScrollArea::takeContentWidget(  )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::clampingStyle
// -----------------------------------------------------------------------------
//
HbScrollArea::ClampingStyle HbScrollArea::clampingStyle(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setClampingStyle
// -----------------------------------------------------------------------------
//
void HbScrollArea::setClampingStyle( 
        ClampingStyle value )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollingStyle
// -----------------------------------------------------------------------------
//
HbScrollArea::ScrollingStyle HbScrollArea::scrollingStyle(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setScrollingStyle
// -----------------------------------------------------------------------------
//
void HbScrollArea::setScrollingStyle( 
        ScrollingStyle value )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollDirections
// -----------------------------------------------------------------------------
//
Qt::Orientations HbScrollArea::scrollDirections(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setScrollDirections
// -----------------------------------------------------------------------------
//
void HbScrollArea::setScrollDirections( 
        Qt::Orientations value )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::frictionEnabled
// -----------------------------------------------------------------------------
//
bool HbScrollArea::frictionEnabled(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::setFrictionEnabled
// -----------------------------------------------------------------------------
//
void HbScrollArea::setFrictionEnabled( 
        bool value )
    {
    SMC_MOCK_METHOD1( void, bool, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::longPressEnabled
// -----------------------------------------------------------------------------
//
bool HbScrollArea::longPressEnabled(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::setLongPressEnabled
// -----------------------------------------------------------------------------
//
void HbScrollArea::setLongPressEnabled( 
        bool value )
    {
    SMC_MOCK_METHOD1( void, bool, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::verticalScrollBarPolicy
// -----------------------------------------------------------------------------
//
HbScrollArea::ScrollBarPolicy HbScrollArea::verticalScrollBarPolicy(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setVerticalScrollBarPolicy
// -----------------------------------------------------------------------------
//
void HbScrollArea::setVerticalScrollBarPolicy( 
        HbScrollArea::ScrollBarPolicy policy )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::verticalScrollBar
// -----------------------------------------------------------------------------
//
HbScrollBar * HbScrollArea::verticalScrollBar(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setVerticalScrollBar
// -----------------------------------------------------------------------------
//
void HbScrollArea::setVerticalScrollBar( 
        HbScrollBar * scrollBar )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::horizontalScrollBarPolicy
// -----------------------------------------------------------------------------
//
HbScrollArea::ScrollBarPolicy HbScrollArea::horizontalScrollBarPolicy(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setHorizontalScrollBarPolicy
// -----------------------------------------------------------------------------
//
void HbScrollArea::setHorizontalScrollBarPolicy( 
        HbScrollArea::ScrollBarPolicy policy )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::horizontalScrollBar
// -----------------------------------------------------------------------------
//
HbScrollBar * HbScrollArea::horizontalScrollBar(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setHorizontalScrollBar
// -----------------------------------------------------------------------------
//
void HbScrollArea::setHorizontalScrollBar( 
        HbScrollBar * scrollBar )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::alignment
// -----------------------------------------------------------------------------
//
Qt::Alignment HbScrollArea::alignment(  ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::setAlignment
// -----------------------------------------------------------------------------
//
void HbScrollArea::setAlignment( 
        Qt::Alignment alignment )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::continuationIndicators
// -----------------------------------------------------------------------------
//
bool HbScrollArea::continuationIndicators(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::setContinuationIndicators
// -----------------------------------------------------------------------------
//
void HbScrollArea::setContinuationIndicators( 
        bool indication )
    {
    SMC_MOCK_METHOD1( void, bool, indication )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::isScrolling
// -----------------------------------------------------------------------------
//
bool HbScrollArea::isScrolling(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::isDragging
// -----------------------------------------------------------------------------
//
bool HbScrollArea::isDragging(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::ensureVisible
// -----------------------------------------------------------------------------
//
void HbScrollArea::ensureVisible( 
        const QPointF & position,
        qreal xMargin,
        qreal yMargin )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollContentsTo
// -----------------------------------------------------------------------------
//
void HbScrollArea::scrollContentsTo( 
        const QPointF & newPosition,
        int time )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::mousePressEvent
// -----------------------------------------------------------------------------
//
void HbScrollArea::mousePressEvent( 
        QGraphicsSceneMouseEvent * event )
    {

    }




// -----------------------------------------------------------------------------
// HbScrollArea::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF HbScrollArea::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbScrollArea::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::event
// -----------------------------------------------------------------------------
//
bool HbScrollArea::event( 
        QEvent * event )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::focusOutEvent
// -----------------------------------------------------------------------------
//
void HbScrollArea::focusOutEvent( 
        QFocusEvent * event )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollByAmount
// -----------------------------------------------------------------------------
//
bool HbScrollArea::scrollByAmount( 
        const QPointF & delta )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::polish
// -----------------------------------------------------------------------------
//
void HbScrollArea::polish( 
        HbStyleParameters & params )
    {

    }


// -----------------------------------------------------------------------------
// HbScrollArea::eventFilter
// -----------------------------------------------------------------------------
//
bool HbScrollArea::eventFilter( 
        QObject * obj,
        QEvent * event )
    {
  
    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollingStarted
// -----------------------------------------------------------------------------
//
void HbScrollArea::scrollingStarted(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollingEnded
// -----------------------------------------------------------------------------
//
void HbScrollArea::scrollingEnded(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::scrollDirectionsChanged
// -----------------------------------------------------------------------------
//
void HbScrollArea::scrollDirectionsChanged( 
        Qt::Orientations newValue )
    {

    }





// -----------------------------------------------------------------------------
// HbScrollArea::scrollPositionChanged
// -----------------------------------------------------------------------------
//
void HbScrollArea::scrollPositionChanged( 
        const QPointF & newPosition )
    {
    
    }


// -----------------------------------------------------------------------------
// HbScrollArea::upGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::upGesture( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::downGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::downGesture( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::leftGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::leftGesture( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::rightGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::rightGesture( 
        int value )
    {
    SMC_MOCK_METHOD1( void, int, value )
    }


// -----------------------------------------------------------------------------
// HbScrollArea::panGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::panGesture( 
        const QPointF & point )
    {
    
    }


// -----------------------------------------------------------------------------
// HbScrollArea::longPressGesture
// -----------------------------------------------------------------------------
//
void HbScrollArea::longPressGesture( 
        const QPointF & point )
    {
    
    }


