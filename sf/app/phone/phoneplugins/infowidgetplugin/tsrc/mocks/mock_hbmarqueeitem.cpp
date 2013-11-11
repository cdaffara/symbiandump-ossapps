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
#include <hbmarqueeitem.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// QGraphicsWidget::adjustSize
// -----------------------------------------------------------------------------
//
void QGraphicsWidget::adjustSize() 
{
    SMC_MOCK_METHOD0( void )
}

// -----------------------------------------------------------------------------
// HbMarqueeItem::HbMarqueeItem
// -----------------------------------------------------------------------------
//
HbMarqueeItem::HbMarqueeItem( 
        QGraphicsItem * parent )
    {
    Q_UNUSED(parent)
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::HbMarqueeItem
// -----------------------------------------------------------------------------
//
HbMarqueeItem::HbMarqueeItem( 
        const QString & text,
        QGraphicsItem * parent )
    {
    Q_UNUSED(text)
    Q_UNUSED(parent)
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::~HbMarqueeItem
// -----------------------------------------------------------------------------
//
HbMarqueeItem::~HbMarqueeItem(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::text
// -----------------------------------------------------------------------------
//
QString HbMarqueeItem::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::setTextColor
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::setTextColor( 
        const QColor & color )
    {
    SMC_MOCK_METHOD1( void, const QColor &, color )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::textColor
// -----------------------------------------------------------------------------
//
QColor HbMarqueeItem::textColor(  ) const
    {
    SMC_MOCK_METHOD0( QColor )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::isAnimating
// -----------------------------------------------------------------------------
//
bool HbMarqueeItem::isAnimating(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::loopCount
// -----------------------------------------------------------------------------
//
int HbMarqueeItem::loopCount(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::setLoopCount
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::setLoopCount( 
        int count )
    {
    SMC_MOCK_METHOD1( void, int, count )
    }

// -----------------------------------------------------------------------------
// HbMarqueeItem::setText
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::startAnimation
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::startAnimation(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::stopAnimation
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::stopAnimation(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::animationStarted
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::animationStarted(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::animationStopped
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::animationStopped(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF HbMarqueeItem::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {
    SMC_MOCK_METHOD2( QSizeF, Qt::SizeHint, which, 
        const QSizeF &, constraint )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::changeEvent
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::changeEvent( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::resizeEvent
// -----------------------------------------------------------------------------
//
void HbMarqueeItem::resizeEvent( 
        QGraphicsSceneResizeEvent *  )
    {
    //SMC_MOCK_METHOD1( void, QGraphicsSceneResizeEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbMarqueeItem::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
    SMC_MOCK_METHOD2( QVariant, GraphicsItemChange, change, 
        const QVariant &, value )
    }


// -----------------------------------------------------------------------------
// HbMarqueeItem::HbMarqueeItem
// -----------------------------------------------------------------------------
//
HbMarqueeItem::HbMarqueeItem( 
        HbMarqueeItemPrivate & dd,
        QGraphicsItem * parent )
    {
    Q_UNUSED(dd)
    Q_UNUSED(parent)
    }

