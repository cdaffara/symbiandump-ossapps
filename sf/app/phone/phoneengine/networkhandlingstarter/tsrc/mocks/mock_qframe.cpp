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
#include <qframe.h>
#include <QWidget>


QWidget::QWidget(QWidget* parent, Qt::WindowFlags f)
    {};

QWidget::~QWidget()
    {};

void QWidget::setVisible(bool visible)
    {};

inline bool QWidget::testAttribute_helper(Qt::WidgetAttribute attribute) const
{
    return true;
}
    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QFrame::QFrame
// -----------------------------------------------------------------------------
//
QFrame::QFrame( 
        QWidget * parent,
        Qt::WindowFlags f )
    {
    
    }


// -----------------------------------------------------------------------------
// QFrame::~QFrame
// -----------------------------------------------------------------------------
//
QFrame::~QFrame(  )
    {
    
    }


// -----------------------------------------------------------------------------
// QFrame::frameStyle
// -----------------------------------------------------------------------------
//
int QFrame::frameStyle(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFrame::setFrameStyle
// -----------------------------------------------------------------------------
//
void QFrame::setFrameStyle( int )
    {
    }


// -----------------------------------------------------------------------------
// QFrame::frameWidth
// -----------------------------------------------------------------------------
//
int QFrame::frameWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFrame::sizeHint
// -----------------------------------------------------------------------------
//
QSize QFrame::sizeHint(  ) const
    {
    SMC_MOCK_METHOD0( QSize )
    }


// -----------------------------------------------------------------------------
// QFrame::frameShape
// -----------------------------------------------------------------------------
//
QFrame::Shape QFrame::frameShape(  ) const
    {
    }


// -----------------------------------------------------------------------------
// QFrame::setFrameShape
// -----------------------------------------------------------------------------
//
void QFrame::setFrameShape( Shape )
    {
    }


// -----------------------------------------------------------------------------
// QFrame::frameShadow
// -----------------------------------------------------------------------------
//
QFrame::Shadow QFrame::frameShadow(  ) const
    {
    }


// -----------------------------------------------------------------------------
// QFrame::setFrameShadow
// -----------------------------------------------------------------------------
//
void QFrame::setFrameShadow( Shadow )
    {
    }


// -----------------------------------------------------------------------------
// QFrame::lineWidth
// -----------------------------------------------------------------------------
//
int QFrame::lineWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFrame::setLineWidth
// -----------------------------------------------------------------------------
//
void QFrame::setLineWidth( int )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QFrame::midLineWidth
// -----------------------------------------------------------------------------
//
int QFrame::midLineWidth(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// QFrame::setMidLineWidth
// -----------------------------------------------------------------------------
//
void QFrame::setMidLineWidth( int )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// QFrame::frameRect
// -----------------------------------------------------------------------------
//
QRect QFrame::frameRect(  ) const
    {
    SMC_MOCK_METHOD0( QRect )
    }


// -----------------------------------------------------------------------------
// QFrame::setFrameRect
// -----------------------------------------------------------------------------
//
void QFrame::setFrameRect( 
        const QRect & )
    {
    //SMC_MOCK_METHOD1( void, const QRect, & )
    }


// -----------------------------------------------------------------------------
// QFrame::event
// -----------------------------------------------------------------------------
//
bool QFrame::event( 
        QEvent * e )
    {
    //SMC_MOCK_METHOD1( bool, QEvent *, e )
    }


// -----------------------------------------------------------------------------
// QFrame::paintEvent
// -----------------------------------------------------------------------------
//
void QFrame::paintEvent( 
        QPaintEvent * )
    {
    //SMC_MOCK_METHOD1( void, QPaintEvent, * )
    }


// -----------------------------------------------------------------------------
// QFrame::changeEvent
// -----------------------------------------------------------------------------
//
void QFrame::changeEvent( 
        QEvent * )
    {
    //SMC_MOCK_METHOD1( void, QEvent, * )
    }


// -----------------------------------------------------------------------------
// QFrame::drawFrame
// -----------------------------------------------------------------------------
//
void QFrame::drawFrame( 
        QPainter * )
    {
    //SMC_MOCK_METHOD1( void, QPainter, * )
    }


// -----------------------------------------------------------------------------
// QFrame::QFrame
// -----------------------------------------------------------------------------
//
QFrame::QFrame( 
        QFramePrivate & dd,
        QWidget * parent,
        Qt::WindowFlags f )
    //:
    //QWidget( /*dd, parent, f*/ )
    {
    
    }


