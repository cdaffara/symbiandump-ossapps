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
#include <hblistwidget.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbListWidget::HbListWidget
// -----------------------------------------------------------------------------
//
HbListWidget::HbListWidget( 
        HbListWidgetPrivate & dd,
        HbAbstractItemContainer * container,
        QGraphicsItem * parent )
    :
    HbListView( parent )
    {
    
    }
    
HbListWidget::HbListWidget(QGraphicsItem *parent)
    : HbListView( parent )
		{
		
		}

// -----------------------------------------------------------------------------
// HbListWidget::~HbListWidget
// -----------------------------------------------------------------------------
//
HbListWidget::~HbListWidget(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbListWidget::count
// -----------------------------------------------------------------------------
//
int HbListWidget::count(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbListWidget::currentRow
// -----------------------------------------------------------------------------
//
int HbListWidget::currentRow(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbListWidget::setCurrentRow
// -----------------------------------------------------------------------------
//
void HbListWidget::setCurrentRow( 
        int row )
    {
    SMC_MOCK_METHOD1( void, int, row )
    }


// -----------------------------------------------------------------------------
// HbListWidget::currentItem
// -----------------------------------------------------------------------------
//
HbListWidgetItem * HbListWidget::currentItem(  ) const
    {
  //  SMC_MOCK_METHOD0( HbListWidgetItem * )
    return NULL;
    }


// -----------------------------------------------------------------------------
// HbListWidget::setCurrentItem
// -----------------------------------------------------------------------------
//
void HbListWidget::setCurrentItem( 
        HbListWidgetItem * item )
    {
  //   SMC_MOCK_METHOD1( void, HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::item
// -----------------------------------------------------------------------------
//
HbListWidgetItem * HbListWidget::item( 
        int row ) const
    {
  //   SMC_MOCK_METHOD1( HbListWidgetItem *, int, row )
    }


// -----------------------------------------------------------------------------
// HbListWidget::row
// -----------------------------------------------------------------------------
//
int HbListWidget::row( 
        const HbListWidgetItem * item ) const
    {
   //  SMC_MOCK_METHOD1( int, const HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::addItem
// -----------------------------------------------------------------------------
//
void HbListWidget::addItem( 
        const HbIcon & icon,
        const QString & text )
    {
  //   SMC_MOCK_METHOD2( void, const HbIcon &, icon, 
   //      const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidget::addItem
// -----------------------------------------------------------------------------
//
void HbListWidget::addItem( 
        const QString & text )
    {
    SMC_MOCK_METHOD1( void, const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidget::addItem
// -----------------------------------------------------------------------------
//
void HbListWidget::addItem( 
        HbListWidgetItem * item )
    {
  //   SMC_MOCK_METHOD1( void, HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::insertItem
// -----------------------------------------------------------------------------
//
void HbListWidget::insertItem( 
        int row,
        const HbIcon & icon,
        const QString & text )
    {
 //    SMC_MOCK_METHOD3( void, int, row, 
 //        const HbIcon &, icon, 
 //        const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidget::insertItem
// -----------------------------------------------------------------------------
//
void HbListWidget::insertItem( 
        int row,
        const QString & text )
    {
    SMC_MOCK_METHOD2( void, int, row, 
        const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidget::insertItem
// -----------------------------------------------------------------------------
//
void HbListWidget::insertItem( 
        int row,
        HbListWidgetItem * item )
    {
  //   SMC_MOCK_METHOD2( void, int, row, 
  //       HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::setText
// -----------------------------------------------------------------------------
//
void HbListWidget::setText( 
        int row,
        const QString & text )
    {
    SMC_MOCK_METHOD2( void, int, row, 
        const QString &, text )
    }


// -----------------------------------------------------------------------------
// HbListWidget::setIcon
// -----------------------------------------------------------------------------
//
void HbListWidget::setIcon( 
        int row,
        const HbIcon & icon )
    {
 //    SMC_MOCK_METHOD2( void, int, row, 
  //       const HbIcon &, icon )
    }


// -----------------------------------------------------------------------------
// HbListWidget::takeItem
// -----------------------------------------------------------------------------
//
HbListWidgetItem * HbListWidget::takeItem( 
        int row )
    {
  //   SMC_MOCK_METHOD1( HbListWidgetItem *, int, row )
    }


// -----------------------------------------------------------------------------
// HbListWidget::setArrangeMode
// -----------------------------------------------------------------------------
//
bool HbListWidget::setArrangeMode( 
        bool arrangeMode )
    {
    SMC_MOCK_METHOD1( bool, bool, arrangeMode )
    }


// -----------------------------------------------------------------------------
// HbListWidget::clear
// -----------------------------------------------------------------------------
//
void HbListWidget::clear(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbListWidget::setModel
// -----------------------------------------------------------------------------
//
void HbListWidget::setModel( 
        QAbstractItemModel * model,
        HbAbstractViewItem * prototype )
    {
  //   SMC_MOCK_METHOD2( void, QAbstractItemModel *, model, 
 //        HbAbstractViewItem *, prototype )
    }


// -----------------------------------------------------------------------------
// HbListWidget::activated
// -----------------------------------------------------------------------------
//
void HbListWidget::activated( 
        HbListWidgetItem * item )
    {
 //    SMC_MOCK_METHOD1( void, HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::pressed
// -----------------------------------------------------------------------------
//
void HbListWidget::pressed( 
        HbListWidgetItem * item )
    {
  //   SMC_MOCK_METHOD1( void, HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::released
// -----------------------------------------------------------------------------
//
void HbListWidget::released( 
        HbListWidgetItem * item )
    {
  //   SMC_MOCK_METHOD1( void, HbListWidgetItem *, item )
    }


// -----------------------------------------------------------------------------
// HbListWidget::longPressed
// -----------------------------------------------------------------------------
//
void HbListWidget::longPressed( 
        HbListWidgetItem * item,
        const QPointF & coords )
    {
  //   SMC_MOCK_METHOD2( void, HbListWidgetItem *, item, 
  //       const QPointF &, coords )
    }


// -----------------------------------------------------------------------------
// HbListWidget::move
// -----------------------------------------------------------------------------
//
void HbListWidget::move( 
        const QModelIndex & from,
        const QModelIndex & to )
    {
  //   SMC_MOCK_METHOD2( void, const QModelIndex &, from, 
   //      const QModelIndex &, to )
    }


