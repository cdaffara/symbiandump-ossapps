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
#include "qgraphicslinearlayout.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::QGraphicsLinearLayout
// -----------------------------------------------------------------------------
//
QGraphicsLinearLayout::QGraphicsLinearLayout( 
        QGraphicsLayoutItem * parent )
    //:
    //QGraphicsLayout( /*parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::QGraphicsLinearLayout
// -----------------------------------------------------------------------------
//
QGraphicsLinearLayout::QGraphicsLinearLayout( 
        Qt::Orientation orientation,
        QGraphicsLayoutItem * parent )
    //:
    //QGraphicsLayout( /*orientation, parent*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::~QGraphicsLinearLayout
// -----------------------------------------------------------------------------
//
QGraphicsLinearLayout::~QGraphicsLinearLayout(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setOrientation
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setOrientation( 
        Qt::Orientation orientation )
    {
    SMC_MOCK_METHOD1( void, Qt::Orientation, orientation )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::orientation
// -----------------------------------------------------------------------------
//
Qt::Orientation QGraphicsLinearLayout::orientation(  ) const
    {
    SMC_MOCK_METHOD0( Qt::Orientation )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::insertItem
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::insertItem( 
        int index,
        QGraphicsLayoutItem * item )
    {
    SMC_MOCK_METHOD2( void, int, index, 
        QGraphicsLayoutItem *, item )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::insertStretch
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::insertStretch( 
        int index,
        int stretch )
    {
    SMC_MOCK_METHOD2( void, int, index, 
        int, stretch )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::removeItem
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::removeItem( 
        QGraphicsLayoutItem * item )
    {
    SMC_MOCK_METHOD1( void, QGraphicsLayoutItem *, item )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::removeAt
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::removeAt( 
        int index )
    {
    SMC_MOCK_METHOD1( void, int, index )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setSpacing
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setSpacing( 
        qreal spacing )
    {
    SMC_MOCK_METHOD1( void, qreal, spacing )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::spacing
// -----------------------------------------------------------------------------
//
qreal QGraphicsLinearLayout::spacing(  ) const
    {
    SMC_MOCK_METHOD0( qreal )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setItemSpacing
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setItemSpacing( 
        int index,
        qreal spacing )
    {
    SMC_MOCK_METHOD2( void, int, index, 
        qreal, spacing )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::itemSpacing
// -----------------------------------------------------------------------------
//
qreal QGraphicsLinearLayout::itemSpacing( 
        int index ) const
    {
    SMC_MOCK_METHOD1( qreal, int, index )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setStretchFactor
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setStretchFactor( 
        QGraphicsLayoutItem * item,
        int stretch )
    {
    SMC_MOCK_METHOD2( void, QGraphicsLayoutItem *, item, 
        int, stretch )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::stretchFactor
// -----------------------------------------------------------------------------
//
int QGraphicsLinearLayout::stretchFactor( 
        QGraphicsLayoutItem * item ) const
    {
    SMC_MOCK_METHOD1( int, QGraphicsLayoutItem *, item )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setAlignment
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setAlignment( 
        QGraphicsLayoutItem * item,
        Qt::Alignment alignment )
    {
//    SMC_MOCK_METHOD2( void, QGraphicsLayoutItem *, item, 
//        Qt::Alignment, alignment )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::alignment
// -----------------------------------------------------------------------------
//
Qt::Alignment QGraphicsLinearLayout::alignment( 
        QGraphicsLayoutItem * item ) const
    {
    //SMC_MOCK_METHOD1( Qt::Alignment, QGraphicsLayoutItem *, item )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::setGeometry
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::setGeometry( 
        const QRectF & rect )
    {
    SMC_MOCK_METHOD1( void, const QRectF &, rect )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::count
// -----------------------------------------------------------------------------
//
int QGraphicsLinearLayout::count(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::itemAt
// -----------------------------------------------------------------------------
//
QGraphicsLayoutItem * QGraphicsLinearLayout::itemAt( 
        int index ) const
    {
    SMC_MOCK_METHOD1( QGraphicsLayoutItem *, int, index )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::invalidate
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::invalidate(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF QGraphicsLinearLayout::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {
    SMC_MOCK_METHOD2( QSizeF, Qt::SizeHint, which, 
        const QSizeF &, constraint )
    }


// -----------------------------------------------------------------------------
// QGraphicsLinearLayout::dump
// -----------------------------------------------------------------------------
//
void QGraphicsLinearLayout::dump( 
        int indent ) const
    {
    SMC_MOCK_METHOD1( void, int, indent )
    }




