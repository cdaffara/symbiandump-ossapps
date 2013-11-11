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
#include <hblistwidgetitem.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbListWidgetItem::HbListWidgetItem
// -----------------------------------------------------------------------------
//
HbListWidgetItem::HbListWidgetItem( 
        int type )
    {
    
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::~HbListWidgetItem
// -----------------------------------------------------------------------------
//
HbListWidgetItem::~HbListWidgetItem(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::data
// -----------------------------------------------------------------------------
//
QVariant HbListWidgetItem::data( 
        int role ) const
    {
 //   SMC_MOCK_METHOD1( QVariant, int, role )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::setData
// -----------------------------------------------------------------------------
//
void HbListWidgetItem::setData( 
        const QVariant & value,
        int role )
    {
//    SMC_MOCK_METHOD2( void, const QVariant &, value, 
//        int, role )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::text
// -----------------------------------------------------------------------------
//
QString HbListWidgetItem::text(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::setText
// -----------------------------------------------------------------------------
//
void HbListWidgetItem::setText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::secondaryText
// -----------------------------------------------------------------------------
//
QString HbListWidgetItem::secondaryText(  ) const
    {
    SMC_MOCK_METHOD0( QString )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::setSecondaryText
// -----------------------------------------------------------------------------
//
void HbListWidgetItem::setSecondaryText( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::setEnabled
// -----------------------------------------------------------------------------
//
void HbListWidgetItem::setEnabled( 
        bool enabled )
    {
    SMC_MOCK_METHOD1( void, bool, enabled )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::isEnabled
// -----------------------------------------------------------------------------
//
bool HbListWidgetItem::isEnabled(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbListWidgetItem::type
// -----------------------------------------------------------------------------
//
int HbListWidgetItem::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


