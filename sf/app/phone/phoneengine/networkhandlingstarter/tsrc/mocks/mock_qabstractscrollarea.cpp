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
#include <qabstractscrollarea.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QAbstractScrollArea::QAbstractScrollArea
// -----------------------------------------------------------------------------
//
QAbstractScrollArea::QAbstractScrollArea( 
        QAbstractScrollAreaPrivate & dd,
        QWidget * parent )
    //:
    //QFrame( /*dd, parent*/ )
    {
    
    }

QAbstractScrollArea::QAbstractScrollArea(QWidget *parent)
{

}

// -----------------------------------------------------------------------------
// QAbstractScrollArea::~QAbstractScrollArea
// -----------------------------------------------------------------------------
//
QAbstractScrollArea::~QAbstractScrollArea(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::verticalScrollBarPolicy
// -----------------------------------------------------------------------------
//
Qt::ScrollBarPolicy QAbstractScrollArea::verticalScrollBarPolicy(  ) const
    {
    SMC_MOCK_METHOD0( Qt::ScrollBarPolicy )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setVerticalScrollBarPolicy
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setVerticalScrollBarPolicy( 
        Qt::ScrollBarPolicy )
    {
    // SMC_MOCK_METHOD1( void, Qt::, ScrollBarPolicy )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::verticalScrollBar
// -----------------------------------------------------------------------------
//
QScrollBar * QAbstractScrollArea::verticalScrollBar(  ) const
    {
    SMC_MOCK_METHOD0( QScrollBar * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setVerticalScrollBar
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setVerticalScrollBar( 
        QScrollBar * scrollbar )
    {
    //SMC_MOCK_METHOD1( void, QScrollBar *, scrollbar )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::horizontalScrollBarPolicy
// -----------------------------------------------------------------------------
//
Qt::ScrollBarPolicy QAbstractScrollArea::horizontalScrollBarPolicy(  ) const
    {
    SMC_MOCK_METHOD0( Qt::ScrollBarPolicy )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setHorizontalScrollBarPolicy
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setHorizontalScrollBarPolicy( 
        Qt:: ScrollBarPolicy )
    {
    //SMC_MOCK_METHOD1( void, Qt::, ScrollBarPolicy )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::horizontalScrollBar
// -----------------------------------------------------------------------------
//
QScrollBar * QAbstractScrollArea::horizontalScrollBar(  ) const
    {
    SMC_MOCK_METHOD0( QScrollBar * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setHorizontalScrollBar
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setHorizontalScrollBar( 
        QScrollBar * scrollbar )
    {
    //SMC_MOCK_METHOD1( void, QScrollBar *, scrollbar )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::cornerWidget
// -----------------------------------------------------------------------------
//
QWidget * QAbstractScrollArea::cornerWidget(  ) const
    {
    SMC_MOCK_METHOD0( QWidget * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setCornerWidget
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setCornerWidget( 
        QWidget * widget )
    {
    //SMC_MOCK_METHOD1( void, QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::addScrollBarWidget
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::addScrollBarWidget( 
        QWidget * widget,
        Qt::Alignment alignment )
    {
    //SMC_MOCK_METHOD2( void, QWidget *, widget, 
    //    Qt::Alignment, alignment )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::scrollBarWidgets
// -----------------------------------------------------------------------------
//
QWidgetList QAbstractScrollArea::scrollBarWidgets( 
        Qt::Alignment alignment )
    {
    //SMC_MOCK_METHOD1( QWidgetList, Qt::Alignment, alignment )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::viewport
// -----------------------------------------------------------------------------
//
QWidget * QAbstractScrollArea::viewport(  ) const
    {
    SMC_MOCK_METHOD0( QWidget * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setViewport
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setViewport( 
        QWidget * widget )
    {
    //SMC_MOCK_METHOD1( void, QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::maximumViewportSize
// -----------------------------------------------------------------------------
//
QSize QAbstractScrollArea::maximumViewportSize(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::minimumSizeHint
// -----------------------------------------------------------------------------
//
QSize QAbstractScrollArea::minimumSizeHint(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::sizeHint
// -----------------------------------------------------------------------------
//
QSize QAbstractScrollArea::sizeHint(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setupViewport
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setupViewport( 
        QWidget * viewport )
    {
    //SMC_MOCK_METHOD1( Q_SLOTS : void, QWidget *, viewport )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setViewportMargins
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setViewportMargins( 
        int left,
        int top,
        int right,
        int bottom )
    {
    //SMC_MOCK_METHOD4( void, int, left, 
    //    int, top, 
    //    int, right, 
    //    int, bottom )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::setViewportMargins
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::setViewportMargins( 
        const QMargins & margins )
    {
    //SMC_MOCK_METHOD1( void, const QMargins &, margins )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::event
// -----------------------------------------------------------------------------
//
bool QAbstractScrollArea::event( 
        QEvent * )
    {
    //SMC_MOCK_METHOD1( bool, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::viewportEvent
// -----------------------------------------------------------------------------
//
bool QAbstractScrollArea::viewportEvent( 
        QEvent * )
    {
    //SMC_MOCK_METHOD1( bool, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::resizeEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::resizeEvent( 
        QResizeEvent * )
    {
    //SMC_MOCK_METHOD1( void, QResizeEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::paintEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::paintEvent( 
        QPaintEvent * )
    {
    //SMC_MOCK_METHOD1( void, QPaintEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::mousePressEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::mousePressEvent( 
        QMouseEvent * )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::mouseReleaseEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::mouseReleaseEvent( 
        QMouseEvent * )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::mouseDoubleClickEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::mouseDoubleClickEvent( 
        QMouseEvent * )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::mouseMoveEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::mouseMoveEvent( 
        QMouseEvent * )
    {
    //SMC_MOCK_METHOD1( void, QMouseEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::wheelEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::wheelEvent( 
        QWheelEvent * )
    {
    //SMC_MOCK_METHOD1( void, QWheelEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::contextMenuEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::contextMenuEvent( 
        QContextMenuEvent * )
    {
    //SMC_MOCK_METHOD1( void, QContextMenuEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::dragEnterEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::dragEnterEvent( 
        QDragEnterEvent * )
    {
    //SMC_MOCK_METHOD1( void, QDragEnterEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::dragMoveEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::dragMoveEvent( 
        QDragMoveEvent * )
    {
    //SMC_MOCK_METHOD1( void, QDragMoveEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::dragLeaveEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::dragLeaveEvent( 
        QDragLeaveEvent * )
    {
    //SMC_MOCK_METHOD1( void, QDragLeaveEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::dropEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::dropEvent( 
        QDropEvent * )
    {
    //SMC_MOCK_METHOD1( void, QDropEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::keyPressEvent
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::keyPressEvent( 
        QKeyEvent * )
    {
    //SMC_MOCK_METHOD1( void, QKeyEvent, * )
    }


// -----------------------------------------------------------------------------
// QAbstractScrollArea::scrollContentsBy
// -----------------------------------------------------------------------------
//
void QAbstractScrollArea::scrollContentsBy( 
        int dx,
        int dy )
    {
    SMC_MOCK_METHOD2( void, int, dx, 
        int, dy )
    }


