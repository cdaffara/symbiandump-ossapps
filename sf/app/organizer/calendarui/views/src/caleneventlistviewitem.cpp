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
* Description:  CalenEventListViewItem class implementation
*
*/

// System includes
#include <hbextendedlocale.h>

// User includes
#include "caleneventlistviewitem.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "caleneventlistviewitemTraces.h"
#endif

// ----------------------------------------------------------------------------
// CalenEventListViewItem::CalenEventListViewItem
// constructor
// ----------------------------------------------------------------------------
// 
CalenEventListViewItem::CalenEventListViewItem(QGraphicsItem *parent) :
HbListViewItem(parent)
{
    OstTraceFunctionEntry0( CALENEVENTLISTVIEWITEM_CALENEVENTLISTVIEWITEM_ENTRY );
    
    OstTraceFunctionExit0( CALENEVENTLISTVIEWITEM_CALENEVENTLISTVIEWITEM_EXIT );
}

// ----------------------------------------------------------------------------
// CalenEventListViewItem::~CalenEventListViewItem
// constructor
// ----------------------------------------------------------------------------
// 
CalenEventListViewItem::~CalenEventListViewItem()
{
    OstTraceFunctionEntry0( DUP1_CALENEVENTLISTVIEWITEM_CALENEVENTLISTVIEWITEM_ENTRY );
    
    // No implementation yet
    OstTraceFunctionExit0( DUP1_CALENEVENTLISTVIEWITEM_CALENEVENTLISTVIEWITEM_EXIT );
}

// ----------------------------------------------------------------------------
// CalenEventListViewItem::createItem
// creates the instance of class
// ----------------------------------------------------------------------------
// 
HbAbstractViewItem* CalenEventListViewItem::createItem()
{
    OstTraceFunctionEntry0( CALENEVENTLISTVIEWITEM_CREATEITEM_ENTRY );
    
    OstTraceFunctionExit0( CALENEVENTLISTVIEWITEM_CREATEITEM_EXIT );
    return new CalenEventListViewItem(*this);
}

// ----------------------------------------------------------------------------
// CalenEventListViewItem::updateChildItems
// creates the child items
// ----------------------------------------------------------------------------
// 
void CalenEventListViewItem::updateChildItems()
{
    OstTraceFunctionEntry0( CALENEVENTLISTVIEWITEM_UPDATECHILDITEMS_ENTRY );
    HbListViewItem::updateChildItems();
    
    OstTraceFunctionExit0( CALENEVENTLISTVIEWITEM_UPDATECHILDITEMS_EXIT );
}


// End of file
