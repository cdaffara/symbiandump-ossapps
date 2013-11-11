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
#include <hbabstractitemview.h>
#include <hbscrollarea.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HbAbstractItemView::HbAbstractItemView
// -----------------------------------------------------------------------------
//
HbAbstractItemView::HbAbstractItemView( 
        HbAbstractItemViewPrivate & /*dd*/,
        HbAbstractItemContainer * /*container*/,
        HbModelIterator *,
        QGraphicsItem * parent )
    : HbScrollArea(parent)
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::~HbAbstractItemView
// -----------------------------------------------------------------------------
//
HbAbstractItemView::~HbAbstractItemView(  )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setModel
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setModel( 
        QAbstractItemModel * ,
        HbAbstractViewItem *  )
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::model
// -----------------------------------------------------------------------------
//
QAbstractItemModel * HbAbstractItemView::model(  ) const
    {
  
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setItemPrototype
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setItemPrototype( 
        HbAbstractViewItem *  )
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setItemPrototypes
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setItemPrototypes( 
        const QList<HbAbstractViewItem *> &  )
    {
  
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::itemPrototypes
// -----------------------------------------------------------------------------
//
QList <HbAbstractViewItem * > HbAbstractItemView::itemPrototypes(  ) const
    {
    return QList <HbAbstractViewItem * >();
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setSelectionModel
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setSelectionModel( 
        QItemSelectionModel *  )
    {
  
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::selectionModel
// -----------------------------------------------------------------------------
//
QItemSelectionModel * HbAbstractItemView::selectionModel(  ) const
    {
    return 0;
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::currentIndex
// -----------------------------------------------------------------------------
//
QModelIndex HbAbstractItemView::currentIndex(  ) const
    {
    return QModelIndex();
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::rootIndex
// -----------------------------------------------------------------------------
//
QModelIndex HbAbstractItemView::rootIndex(  ) const
    {
    return QModelIndex();
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setSelectionMode
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setSelectionMode( 
        SelectionMode )
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::selectionMode
// -----------------------------------------------------------------------------
//
HbAbstractItemView::SelectionMode HbAbstractItemView::selectionMode(  ) const
    {
    return NoSelection;
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setItemRecycling
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setItemRecycling( 
        bool enabled )
    {
    SMC_MOCK_METHOD1( void, bool, enabled )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::itemRecycling
// -----------------------------------------------------------------------------
//
bool HbAbstractItemView::itemRecycling(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::isVisible
// -----------------------------------------------------------------------------
//
bool HbAbstractItemView::isVisible( 
        const QModelIndex & index ) const
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::isVisible
// -----------------------------------------------------------------------------
//
/*bool HbAbstractItemView::isVisible( 
        const QModelIndex & ) const
    {
    return false;
    }
*/

// -----------------------------------------------------------------------------
// HbAbstractItemView::currentViewItem
// -----------------------------------------------------------------------------
//
HbAbstractViewItem * HbAbstractItemView::currentViewItem(  ) const
    {
    SMC_MOCK_METHOD0( HbAbstractViewItem * )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::itemByIndex
// -----------------------------------------------------------------------------
//
HbAbstractViewItem * HbAbstractItemView::itemByIndex( 
        const QModelIndex & index ) const
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::scrollTo
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::scrollTo( 
        const QModelIndex & index,
        ScrollHint hint )
    {

    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::visibleItems
// -----------------------------------------------------------------------------
//
QList <HbAbstractViewItem * > HbAbstractItemView::visibleItems(  ) const
    {
 
    }

// -----------------------------------------------------------------------------
// HbAbstractItemView::layoutName
// -----------------------------------------------------------------------------
//
QString HbAbstractItemView::layoutName(  ) const
    {
 
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setLayoutName
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setLayoutName( 
        const QString & layoutName )
    {
   
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setUniformItemSizes
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setUniformItemSizes( 
        bool enable )
    {
    SMC_MOCK_METHOD1( void, bool, enable )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::uniformItemSizes
// -----------------------------------------------------------------------------
//
bool HbAbstractItemView::uniformItemSizes(  ) const
    {
    SMC_MOCK_METHOD0( bool )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setCurrentIndex
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setCurrentIndex( 
        const QModelIndex & index,
        QItemSelectionModel::SelectionFlags selectionFlag )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::setRootIndex
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::setRootIndex( 
        const QModelIndex & index )
    {
   
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::reset
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::reset(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::selectAll
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::selectAll(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::clearSelection
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::clearSelection(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::pressed
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::pressed( 
        const QModelIndex & index )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::released
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::released( 
        const QModelIndex & index )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::activated
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::activated( 
        const QModelIndex & index )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::longPressed
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::longPressed( 
        HbAbstractViewItem * item,
        const QPointF & coords )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::selectionCommand
// -----------------------------------------------------------------------------
//
QItemSelectionModel::SelectionFlags HbAbstractItemView::selectionCommand( 
        const HbAbstractViewItem *item, 
        const QEvent *event )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::event
// -----------------------------------------------------------------------------
//
bool HbAbstractItemView::event( 
        QEvent * e )
    {
    
    }



// -----------------------------------------------------------------------------
// HbAbstractItemView::emitActivated
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::emitActivated( 
        const QModelIndex & modelIndex )
    {
   
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::emitPressed
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::emitPressed( 
        const QModelIndex & modelIndex )
    {
   
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::emitReleased
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::emitReleased( 
        const QModelIndex & modelIndex )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::scrollByAmount
// -----------------------------------------------------------------------------
//
bool HbAbstractItemView::scrollByAmount( 
        const QPointF & delta )
    {
   
    }

// -----------------------------------------------------------------------------
// HbAbstractItemView::itemChange
// -----------------------------------------------------------------------------
//
QVariant HbAbstractItemView::itemChange( 
        GraphicsItemChange change,
        const QVariant & value )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::dataChanged
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::dataChanged( 
        const QModelIndex & topLeft,
        const QModelIndex & bottomRight )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::orientationChanged
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::orientationChanged( 
        Qt::Orientation newOrientation )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::orientationAboutToBeChanged
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::orientationAboutToBeChanged(  )
    {
    SMC_MOCK_METHOD0( void )
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::currentIndexChanged
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::currentIndexChanged( 
        const QModelIndex & current,
        const QModelIndex & previous )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::currentSelectionChanged
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::currentSelectionChanged( 
        const QItemSelection & selected,
        const QItemSelection & deselected )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::rowsAboutToBeInserted
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::rowsAboutToBeInserted( 
        const QModelIndex & index,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::rowsInserted
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::rowsInserted( 
        const QModelIndex & parent,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::rowsAboutToBeRemoved
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::rowsAboutToBeRemoved( 
        const QModelIndex & index,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::rowsRemoved
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::rowsRemoved( 
        const QModelIndex & parent,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::columnsAboutToBeInserted
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::columnsAboutToBeInserted( 
        const QModelIndex & index,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::columnsInserted
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::columnsInserted( 
        const QModelIndex & parent,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::columnsAboutToBeRemoved
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::columnsAboutToBeRemoved( 
        const QModelIndex & index,
        int start,
        int end )
    {
    
    }


// -----------------------------------------------------------------------------
// HbAbstractItemView::columnsRemoved
// -----------------------------------------------------------------------------
//
void HbAbstractItemView::columnsRemoved( 
        const QModelIndex & parent,
        int start,
        int end )
    {
    
    }



