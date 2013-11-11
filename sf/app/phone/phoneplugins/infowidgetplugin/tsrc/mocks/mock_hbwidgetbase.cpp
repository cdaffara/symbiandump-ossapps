/** Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

#include <HbStylePrimitiveData>

// NOTE! The following header include requires
//       INCLUDEPATH += /sf/mw/hb/include/hbcore/private
#include <hbstyleoption_p.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbWidgetBase::HbWidgetBase
// -----------------------------------------------------------------------------
//
HbWidgetBase::HbWidgetBase( 
        QGraphicsItem * parent,
        Qt::WindowFlags wFlags )
    :d_ptr(0)
    //QGraphicsWidget( /*parent, wFlags*/ )
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
// HbWidgetBase::effectiveFontSpec
// -----------------------------------------------------------------------------
//
HbFontSpec HbWidgetBase::effectiveFontSpec(  ) const
    {
    SMC_MOCK_METHOD0( HbFontSpec )
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
// HbWidgetBase::HbWidgetBase
// -----------------------------------------------------------------------------
//
HbWidgetBase::HbWidgetBase( 
        HbWidgetBasePrivate & dd,
        QGraphicsItem * parent,
        Qt::WindowFlags wFlags )
    :d_ptr(0)
    //QGraphicsWidget( /*dd, parent, wFlags*/ )
    {
    
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


// -----------------------------------------------------------------------------
// HbWidgetBase::sceneEvent
// -----------------------------------------------------------------------------
//
bool HbWidgetBase::sceneEvent( 
        QEvent * event )
    {
    SMC_MOCK_METHOD1( bool, QEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::gestureEvent
// -----------------------------------------------------------------------------
//
void HbWidgetBase::gestureEvent( 
        QGestureEvent * event )
    {
    SMC_MOCK_METHOD1( void, QGestureEvent *, event )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::updateGeometry
// -----------------------------------------------------------------------------
//
void HbWidgetBase::updateGeometry(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::initStyleOption
// -----------------------------------------------------------------------------
//
void HbWidgetBase::initStyleOption( 
        HbStyleOption * option ) const
    {
    SMC_MOCK_METHOD1( void, HbStyleOption *, option )
    }


// -----------------------------------------------------------------------------
// HbWidgetBase::initPrimitiveData
// -----------------------------------------------------------------------------
//
void HbWidgetBase::initPrimitiveData( 
        HbStylePrimitiveData * primitiveData,
        const QGraphicsObject * primitive )
    {
    SMC_MOCK_METHOD2( void, HbStylePrimitiveData *, primitiveData, 
        const QGraphicsObject *, primitive )
    }


