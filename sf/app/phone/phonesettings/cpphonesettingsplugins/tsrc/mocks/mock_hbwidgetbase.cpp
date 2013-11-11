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
#include "hbwidgetbase.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbWidgetBase::HbWidgetBase
// -----------------------------------------------------------------------------
//
HbWidgetBase::HbWidgetBase( 
        HbWidgetBasePrivate & dd,
        QGraphicsItem * parent,
        Qt::WindowFlags wFlags ) :d_ptr()
    //:
    //HbAbstractItem( /*dd, parent, wFlags*/ )
    {
    
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::~HbWidgetBase
// -----------------------------------------------------------------------------
//
HbWidgetBase::~HbWidgetBase(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::event
// -----------------------------------------------------------------------------
//
bool HbWidgetBase::event( 
        QEvent * e )
    {
    SMC_MOCK_METHOD1( bool, QEvent *, e )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::fontSpec
// -----------------------------------------------------------------------------
//
HbFontSpec HbWidgetBase::fontSpec(  ) const
    {
    SMC_MOCK_METHOD0( HbFontSpec )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::setFontSpec
// -----------------------------------------------------------------------------
//
void HbWidgetBase::setFontSpec( 
        const HbFontSpec & fontSpec )
    {
    SMC_MOCK_METHOD1( void, const HbFontSpec &, fontSpec )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::setAttribute
// -----------------------------------------------------------------------------
//
void HbWidgetBase::setAttribute( 
        Qt::WidgetAttribute att,
        bool on )
    {
    SMC_MOCK_METHOD2( void, Qt::WidgetAttribute, att, 
        bool, on )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::testAttribute
// -----------------------------------------------------------------------------
//
bool HbWidgetBase::testAttribute( 
        Qt::WidgetAttribute att ) const
    {
    SMC_MOCK_METHOD1( bool, Qt::WidgetAttribute, att )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::setAttribute
// -----------------------------------------------------------------------------
//
void HbWidgetBase::setAttribute( 
        Hb::WidgetAttribute att,
        bool on )
    {
    SMC_MOCK_METHOD2( void, Hb::WidgetAttribute, att, 
        bool, on )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::testAttribute
// -----------------------------------------------------------------------------
//
bool HbWidgetBase::testAttribute( 
        Hb::WidgetAttribute att ) const
    {
    SMC_MOCK_METHOD1( bool, Hb::WidgetAttribute, att )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbWidgetBase::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
    SMC_MOCK_METHOD2( QVariant, GraphicsItemChange, change, 
        const QVariant &, value )
    }


