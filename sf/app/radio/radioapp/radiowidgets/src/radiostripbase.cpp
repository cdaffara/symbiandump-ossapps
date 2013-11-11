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

// System includes
#include <QGraphicsSceneEvent>

// User includes
#include "radiostripbase.h"

// Constants

/*!
 *
 */
RadioStripBase::RadioStripBase( QGraphicsItem* parent ) :
    HbScrollArea( parent ),
    mAutoScrollTime( 0 ),
    mStripContainer( new HbWidget( this ) ),
    mModel( 0 ),
    mIsCyclic( true ),
    mAutoCenter( false ),
    mOverlap( 0 ),
    mItemPoolParent( new QGraphicsWidget( NULL ) ),
    mCurrentIndex( 0 ),
    mPressedIndex( 0 ),
    mStripLength( 0 ),
    mContentsLength( 0 )
{
    setClampingStyle( HbScrollArea::NoClamping );
    setScrollDirections( Qt::Horizontal );
    setFlag( QGraphicsItem::ItemClipsChildrenToShape, true );
    setContentWidget( mStripContainer );
    setFrictionEnabled( true );
    setHorizontalScrollBarPolicy( HbScrollArea::ScrollBarAlwaysOff );
    setVerticalScrollBarPolicy( HbScrollArea::ScrollBarAlwaysOff );

    // mItemParent is used to hold the unused QGraphicsItem's in the pool.  It's visibility is set to false
    // so the visibility of the items doesn't need to be modified.
    mItemPoolParent->setVisible( false );
}

/*!
 *
 */
RadioStripBase::~RadioStripBase()
{
}

/*!
 *
 */
void RadioStripBase::setAutoScrollTime( const int time )
{
    mAutoScrollTime = time;
}

/*!
 *
 */
int RadioStripBase::autoScrollTime() const
{
    return mAutoScrollTime;
}

/*!
 *
 */
void RadioStripBase::setModel( QAbstractItemModel* model )
{
    if ( mModel != model )
    {
    //    if ( mModel )
    //    {
    //        disconnectDataModel();
    //    }

        mModel = model;

        if ( mModel )
        {
    //        connectDataModel();

            mCurrentIndex = 0;
            populateAndLayout();
        }
    }
}

/*!
 *
 */
QAbstractItemModel* RadioStripBase::model() const
{
    return mModel;
}

/*!
 *
 */
void RadioStripBase::setCyclic( bool isCyclic )
{
    mIsCyclic = isCyclic;
}

/*!
 *
 */
void RadioStripBase::setOverlap( qreal overlap )
{
    mOverlap = overlap;
}

/*!
 *
 */
void RadioStripBase::setAutoCenter( bool autoCenter )
{
    mAutoCenter = autoCenter;
}

/*!
 *
 */
void RadioStripBase::setItemSize( const QSizeF& size )
{
    if ( mItemSize != size ) {
        mItemSize = size;

        prepareGeometryChange();

        populateAndLayout();

        update();
        updateGeometry();
    }
}

/*!
 *
 */
void RadioStripBase::setIndex( int index, bool animateToCenter )
{
    Q_UNUSED( animateToCenter )
    // Sanity checks
    if ( !mModel || ( !mIsCyclic && ( index < 0 || index >= mModel->rowCount() ) ) ) {
        return;
    }

    const int oldIndex = mCurrentIndex;
    if ( mIsCyclic )
    {
        int numRows = mModel->rowCount();
        index = (index + numRows) % numRows;
    }
    Q_ASSERT( index >= 0 );

    mCurrentIndex = index;

    updateItemWithIndex( mCurrentIndex );
    updateItemWithIndex( oldIndex );
}

/*!
 * \reimp
 */
void RadioStripBase::resizeEvent( QGraphicsSceneResizeEvent* event )
{
    HbScrollArea::resizeEvent( event );
    populateAndLayout();
}

/*!
 * \reimp
 */
void RadioStripBase::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    if ( event->button() != Qt::LeftButton || !mModel || !mModel->rowCount() )
    {
        event->ignore();
        return;
    }

    HbScrollArea::mousePressEvent( event );
}

/*!
 * \reimp
 */
void RadioStripBase::mouseReleaseEvent( QGraphicsSceneMouseEvent* event )
{
    if ( event->button() != Qt::LeftButton )
    {
        event->ignore();
        return;
    }

    HbScrollArea::mouseReleaseEvent( event );
}

/*!
 *
 */
void RadioStripBase::moveAllItemsToPool()
{
    // set parent of all items to pool
    foreach( QGraphicsItem* item, mItemAtSlot )
    {
        item->setParentItem( mItemPoolParent );
    }

    // move all items to pool
    mItemPool += mItemAtSlot;
    mItemAtSlot.clear();
    mIndexAtSlot.clear();
}

/*!
 *
 */
void RadioStripBase::populateAndLayout()
{
    moveAllItemsToPool();

    if ( !mModel || mModel->rowCount() == 0 )
    {
        return;
    }

    mStripLength = boundingRect().width();
    qreal itemSize = mItemSize.width();
    mContentsLength = mModel->rowCount() * (itemSize - mOverlap);

    if ( mIsCyclic )
    {
        // if treating the items cyclically, double the content area so it can
        // be shifted back and forth as you scroll
        mContentsLength *= 2;
    }

    qreal currPos = -mOverlap;
    for ( int i = 0; i < mModel->rowCount(); ++i ) {
        if ( currPos > mStripLength )
        {
            break;
        }

        QGraphicsItem* item = constructItem( i, true );
        if ( item )
        {
            item->setPos( QPointF( currPos, 0 ) );
            currPos += itemSize - mOverlap;
        }
    }

    mStripContainer->setPreferredSize( mContentsLength, mItemSize.height() );

    if ( mCurrentIndex >= 0 )
    {
        setIndex( mCurrentIndex, false );
    }
}

/*!
 *
 */
QGraphicsItem* RadioStripBase::constructItem( int index, bool append )
{
    QGraphicsItem* item = getFromPool();

    if ( mIsCyclic )
    {
        Q_ASSERT( index >= 0 && index < 2 * mModel->rowCount() );

        updateItemPrimitive( item, index % mModel->rowCount() );
    }
    else
    {
        Q_ASSERT( index >= 0 && index < mModel->rowCount() );

        updateItemPrimitive( item, index );
    }

    item->setParentItem( mStripContainer );

    if ( append )
    {
        mItemAtSlot.append( item );
        mIndexAtSlot.append( index );
    }
    else
    {
        mItemAtSlot.prepend( item );
        mIndexAtSlot.prepend( index );
    }

    return item;
}

/*!
 *
 */
QGraphicsItem* RadioStripBase::getFromPool()
{
    QGraphicsItem* item = 0;

    if ( mItemPool.isEmpty() )
    {
        item = createItemPrimitive( this );
    }
    else
    {
        item = mItemPool.takeFirst();
    }

    return item;
}

/*!
 *
 */
void RadioStripBase::returnToPool( QGraphicsItem* item )
{
    // Unparent the item so it doesn't get deleted
    item->setParentItem( mItemPoolParent );
    mItemPool.append( item );
}

/*!
 * Returns starting coordinate of the item with the specified index
 */
qreal RadioStripBase::indexToOffset( int index )
{
    return index * ( mItemSize.width() - mOverlap ) - mOverlap;
}

/*!
 * Returns item index for specified offset amount into the content
 */
int RadioStripBase::offsetToIndex( qreal offset )
{
    const int rows = mModel->rowCount();
    int index = (int)( offset / ( mItemSize.width() - mOverlap ) );

    if ( mIsCyclic )
    {
       return qBound( 0, index, 2 * rows - 1 );
    }

    return qBound( 0, index, rows - 1 );
}

/*!
 * updates items with specified index value
 */
void RadioStripBase::updateItemWithIndex( int index )
{
    if( index >= 0 )
    {
        QList<QGraphicsItem *>::const_iterator item = mItemAtSlot.constBegin();
        QList<QGraphicsItem *>::const_iterator itemsEnd = mItemAtSlot.constEnd();
        QList<int>::const_iterator itemIndex = mIndexAtSlot.constBegin();
        const int rowCount = mModel->rowCount();

        // Find all items with this index (can be 2 in special cycling case)
        for( ; item != itemsEnd; ++item, ++itemIndex )
        {
            if( index == *itemIndex || index == *itemIndex - rowCount )
            {
                // update those items
                updateItemPrimitive( *item, index );
            }
        }
    }
}

/*!
 * Updates items during scrolling: removing invisible items and adding items that became visible
 */
void RadioStripBase::adjustItems()
{
    qreal contentPos = mStripContainer->pos().x();

    if ( mIsCyclic )
    {
        if ( -contentPos < 0 )
        {
            // trying  to display off the left end of the strip, so
            // shift the strip one length to the left
            contentPos -= mContentsLength * 0.5;
            mStripContainer->setPos( QPointF ( contentPos, mStripContainer->pos().y() ) );
        } else if (-contentPos > mContentsLength * 0.5) {
            // trying to display off the right end of the strip, so
            // shift the strip one length to the right
            contentPos += mContentsLength * 0.5;
            mStripContainer->setPos( QPointF ( contentPos, mStripContainer->pos().y() ) );
        }
    }

    // find the first and last indices of the visible items
    int firstVisibleIndex = offsetToIndex( -contentPos );
    int lastVisibleIndex = offsetToIndex( -contentPos + mStripLength );

    // remove items at the start that are no longer visible
    while ( !mIndexAtSlot.isEmpty() )
    {
        int firstSlotIndex = mIndexAtSlot.first();
        if ( firstVisibleIndex <= firstSlotIndex )
        {
            break;
        }

        returnToPool( mItemAtSlot.first() );
        mItemAtSlot.removeFirst();
        mIndexAtSlot.removeFirst();
    }

    // remove items at the end that are no longer visible
    while ( !mIndexAtSlot.isEmpty() )
    {
        int lastSlotIndex = mIndexAtSlot.last();
        if ( lastVisibleIndex >= lastSlotIndex )
        {
            break;
        }

        returnToPool( mItemAtSlot.last() );
        mItemAtSlot.removeLast();
        mIndexAtSlot.removeLast();
    }

    if ( mItemAtSlot.isEmpty() )
    {
        // fill area with all needed items
        for ( int i = firstVisibleIndex; i <= lastVisibleIndex; ++i )
        {
            QGraphicsItem* item = constructItem( i, true );
            if ( item )
            {
                item->setPos( QPointF( indexToOffset( i ), 0 ) );
            }
        }
    }
    else
    {
        // add missing items at the front
        int firstItemToCreate = mIndexAtSlot.first()-1;
        for ( int i = firstItemToCreate; i >= firstVisibleIndex; --i )
        {
            QGraphicsItem* item = constructItem( i, false );
            if ( item )
            {
                item->setPos( QPointF( indexToOffset( i ), 0 ) );
            }
        }

        // add missing items at the end
        firstItemToCreate = mIndexAtSlot.last()+1;
        for ( int i = firstItemToCreate; i <= lastVisibleIndex; ++i )
        {
            QGraphicsItem* item = constructItem( i, true );
            if ( item )
            {
                item->setPos( QPointF( indexToOffset( i ), 0 ) );
            }
        }
    }
}

/*!
 * \reimp
 */
bool RadioStripBase::scrollByAmount( const QPointF& delta )
{
    bool ret = HbScrollArea::scrollByAmount( delta );

    adjustItems();
    scrollPosChanged();

    return ret;
}
