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
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <smcmockclassincludes.h>
#include "hbframeitem.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbFrameItem::HbFrameItem
// -----------------------------------------------------------------------------
//
HbFrameItem::HbFrameItem( 
        QGraphicsItem * parent )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameItem::HbFrameItem
// -----------------------------------------------------------------------------
//
HbFrameItem::HbFrameItem( 
        HbFrameDrawer * drawer,
        QGraphicsItem * parent )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameItem::HbFrameItem
// -----------------------------------------------------------------------------
//
HbFrameItem::HbFrameItem( 
        const QString & frameGraphicsName,
        HbFrameDrawer::FrameType frameGraphicsType,
        QGraphicsItem * parent )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameItem::~HbFrameItem
// -----------------------------------------------------------------------------
//
HbFrameItem::~HbFrameItem(  )
    {
    }


// -----------------------------------------------------------------------------
// HbFrameItem::frameDrawer
// -----------------------------------------------------------------------------
//
HbFrameDrawer & HbFrameItem::frameDrawer(  ) const
    {
    SMC_MOCK_METHOD0( HbFrameDrawer & )
    }


// -----------------------------------------------------------------------------
// HbFrameItem::setFrameDrawer
// -----------------------------------------------------------------------------
//
void HbFrameItem::setFrameDrawer( 
        HbFrameDrawer * drawer )
    {
    SMC_MOCK_METHOD1( void, HbFrameDrawer *, drawer )
    }


// -----------------------------------------------------------------------------
// HbFrameItem::paint
// -----------------------------------------------------------------------------
//
void HbFrameItem::paint( 
        QPainter * painter,
        const QStyleOptionGraphicsItem * option,
        QWidget * widget )
    {
    SMC_MOCK_METHOD3( void, QPainter *, painter, 
        const QStyleOptionGraphicsItem *, option, 
        QWidget *, widget )
    }


// -----------------------------------------------------------------------------
// HbFrameItem::changeEvent
// -----------------------------------------------------------------------------
//
void HbFrameItem::changeEvent( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( void, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbFrameItem::sizeHint
// -----------------------------------------------------------------------------
//
QSizeF HbFrameItem::sizeHint( 
        Qt::SizeHint which,
        const QSizeF & constraint ) const
    {
    SMC_MOCK_METHOD2( QSizeF, Qt::SizeHint, which, 
        const QSizeF &, constraint )
    }


