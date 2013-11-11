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
#include <hblistview.h>
#include <hbabstractitemview.h>

HbAbstractItemViewPrivate *pointerHbAbstractItemViewPrivate(NULL);
HbAbstractItemContainer *pointerHbAbstractItemContainer(NULL);

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbListView::HbListView
// -----------------------------------------------------------------------------
//
HbListView::HbListView( 
        HbListViewPrivate & dd,
        HbAbstractItemContainer * container,
        QGraphicsItem * parent )
    : HbAbstractItemView( *pointerHbAbstractItemViewPrivate, pointerHbAbstractItemContainer, 0, parent )
    {
    
    }

HbListView::HbListView(QGraphicsItem *parent)
    : HbAbstractItemView( *pointerHbAbstractItemViewPrivate, pointerHbAbstractItemContainer, 0, parent )
		{
		
		}

// -----------------------------------------------------------------------------
// HbListView::~HbListView
// -----------------------------------------------------------------------------
//
HbListView::~HbListView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbListView::type
// -----------------------------------------------------------------------------
//

int HbListView::type(  ) const
    {
    SMC_MOCK_METHOD0( int )
    }


// -----------------------------------------------------------------------------
// HbListView::listItemPrototype
// -----------------------------------------------------------------------------
//
HbListViewItem * HbListView::listItemPrototype(  ) const
    {
   //  SMC_MOCK_METHOD0( HbListViewItem * )
    }


// -----------------------------------------------------------------------------
// HbListView::arrangeMode
// -----------------------------------------------------------------------------
//
bool HbListView::arrangeMode(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbListView::setArrangeMode
// -----------------------------------------------------------------------------
//
bool HbListView::setArrangeMode( 
        bool arrangeMode )
    {
    SMC_MOCK_METHOD1( bool, bool, arrangeMode )
    }


// -----------------------------------------------------------------------------
// HbListView::viewItem
// -----------------------------------------------------------------------------
//
HbAbstractViewItem * HbListView::viewItem( 
        int row ) const
    {
  //   SMC_MOCK_METHOD1( HbAbstractViewItem *, int, row )
    }


// -----------------------------------------------------------------------------
// HbListView::scrollTo
// -----------------------------------------------------------------------------
//
void HbListView::scrollTo( 
        const QModelIndex & index,
        ScrollHint hint )
    {
   //  SMC_MOCK_METHOD2( void, const QModelIndex &, index, 
  //       ScrollHint, hint )
    }


// -----------------------------------------------------------------------------
// HbListView::rowsInserted
// -----------------------------------------------------------------------------
//
void HbListView::rowsInserted( 
        const QModelIndex & parent,
        int start,
        int end )
    {
  //   SMC_MOCK_METHOD3( void, const QModelIndex &, parent, 
  //       int, start, 
  //       int, end )
    }


// -----------------------------------------------------------------------------
// HbListView::rowsAboutToBeRemoved
// -----------------------------------------------------------------------------
//
void HbListView::rowsAboutToBeRemoved( 
        const QModelIndex & parent,
        int start,
        int end )
    {
  //   SMC_MOCK_METHOD3( void, const QModelIndex &, parent, 
 //        int, start, 
 //        int, end )
    }


// -----------------------------------------------------------------------------
// HbListView::rowsRemoved
// -----------------------------------------------------------------------------
//
void HbListView::rowsRemoved( 
        const QModelIndex & parent,
        int start,
        int end )
    {
 //    SMC_MOCK_METHOD3( void, const QModelIndex &, parent, 
 //        int, start, 
  //       int, end )
    }


// -----------------------------------------------------------------------------
// HbListView::dataChanged
// -----------------------------------------------------------------------------
//
void HbListView::dataChanged( 
        const QModelIndex & topLeft,
        const QModelIndex & bottomRight )
    {
 //    SMC_MOCK_METHOD2( void, const QModelIndex &, topLeft, 
 //        const QModelIndex &, bottomRight )
    }


// -----------------------------------------------------------------------------
// HbListView::orientationChanged
// -----------------------------------------------------------------------------
//
void HbListView::orientationChanged( 
        Qt::Orientation newOrientation )
    {
  //   SMC_MOCK_METHOD1( void, Qt::Orientation, newOrientation )
    }



// -----------------------------------------------------------------------------
// HbListView::move
// -----------------------------------------------------------------------------
//
void HbListView::move( 
        const QModelIndex & from,
        const QModelIndex & to )
    {
   //  SMC_MOCK_METHOD2( void, const QModelIndex &, from, 
  //       const QModelIndex &, to )
    }


// -----------------------------------------------------------------------------
// HbListView::scrolling
// -----------------------------------------------------------------------------
//
void HbListView::scrolling( 
        QPointF newPosition )
    {
   //  SMC_MOCK_METHOD1( void, QPointF, newPosition )
    }


