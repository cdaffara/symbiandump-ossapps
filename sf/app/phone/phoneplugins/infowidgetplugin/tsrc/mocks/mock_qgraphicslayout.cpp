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
#include "qgraphicslayout.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QGraphicsLayout::QGraphicsLayout
// -----------------------------------------------------------------------------
//
QGraphicsLayout::QGraphicsLayout( 
        QGraphicsLayoutItem * parent )
    //:
    //QGraphicsLayoutItem( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::~QGraphicsLayout
// -----------------------------------------------------------------------------
//
QGraphicsLayout::~QGraphicsLayout(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::setContentsMargins
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::setContentsMargins( 
        qreal left,
        qreal top,
        qreal right,
        qreal bottom )
    {
    SMC_MOCK_METHOD4( void, qreal, left, 
        qreal, top, 
        qreal, right, 
        qreal, bottom )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::getContentsMargins
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::getContentsMargins( 
        qreal * left,
        qreal * top,
        qreal * right,
        qreal * bottom ) const
    {
    SMC_MOCK_METHOD4( void, qreal *, left, 
        qreal *, top, 
        qreal *, right, 
        qreal *, bottom )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::activate
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::activate(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::isActivated
// -----------------------------------------------------------------------------
//
bool QGraphicsLayout::isActivated(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::invalidate
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::invalidate(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::updateGeometry
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::updateGeometry(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::widgetEvent
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::widgetEvent( 
        QEvent * e )
    {
    SMC_MOCK_METHOD1( void, QEvent *, e )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::count
// -----------------------------------------------------------------------------
//
int QGraphicsLayout::count(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::itemAt
// -----------------------------------------------------------------------------
//
QGraphicsLayoutItem * QGraphicsLayout::itemAt( 
        int i ) const
    {
    SMC_MOCK_METHOD1( QGraphicsLayoutItem *, int, i )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::removeAt
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::removeAt( 
        int index )
    {
    SMC_MOCK_METHOD1( void, int, index )
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::QGraphicsLayout
// -----------------------------------------------------------------------------
//
QGraphicsLayout::QGraphicsLayout( 
        QGraphicsLayoutPrivate &,
        QGraphicsLayoutItem * )
    //:
    //QGraphicsLayoutItem( /*&, **/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLayout::addChildLayoutItem
// -----------------------------------------------------------------------------
//
void QGraphicsLayout::addChildLayoutItem( 
        QGraphicsLayoutItem * layoutItem )
    {
    SMC_MOCK_METHOD1( void, QGraphicsLayoutItem *, layoutItem )
    }


