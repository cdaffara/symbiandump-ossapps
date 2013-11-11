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

#ifndef RADIOSTRIPBASE_H_
#define RADIOSTRIPBASE_H_

// System includes
#include <HbScrollArea>

// Forward declarations
class HbWidget;
class QAbstractItemModel;

// Class declaration
class RadioStripBase : public HbScrollArea
{
    Q_OBJECT
    Q_PROPERTY(int autoScrollTime READ autoScrollTime WRITE setAutoScrollTime)

public:

    virtual ~RadioStripBase();

    void setAutoScrollTime( const int time );
    int autoScrollTime() const;

    void setModel( QAbstractItemModel* model );
    QAbstractItemModel* model() const;

    void setCyclic( bool isCyclic );
    void setOverlap( qreal overlap );
    void setAutoCenter( bool autoCenter );
    void setItemSize( const QSizeF& size );
    void setIndex( int index, bool animateToCenter );

protected:

    RadioStripBase( QGraphicsItem* parent = 0 );

// from base class QGraphicsWidget

    void resizeEvent( QGraphicsSceneResizeEvent* event );

// from base class QGraphicsItem

    void mousePressEvent( QGraphicsSceneMouseEvent* event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

private:

    Q_DISABLE_COPY( RadioStripBase )

// from base class HbScrollArea

    bool scrollByAmount( const QPointF& delta );

    virtual void updateItemPrimitive( QGraphicsItem* itemToUpdate, int itemIndex ) = 0;
    virtual QGraphicsItem* createItemPrimitive( QGraphicsItem *parent ) = 0;

    virtual void scrollPosChanged() = 0;

    void moveAllItemsToPool();

    void populateAndLayout();

    QGraphicsItem* constructItem( int index, bool append );

    QGraphicsItem* getFromPool();

    void returnToPool( QGraphicsItem* item );

    qreal indexToOffset( int index );

    int offsetToIndex( qreal offset );

    void updateItemWithIndex( int index );

    void adjustItems();

protected: // data

    int                     mAutoScrollTime;

private: //data

    HbWidget*               mStripContainer;

    QAbstractItemModel*     mModel;

    bool                    mIsCyclic;
    bool                    mAutoCenter;

    qreal                   mOverlap;

    QSizeF                  mItemSize;

    QList<QGraphicsItem*>   mItemPool;
    QGraphicsWidget*        mItemPoolParent;

    int                     mCurrentIndex;
    int                     mPressedIndex;

    qreal                   mStripLength;
    qreal                   mContentsLength;

    QList<QGraphicsItem*>   mItemAtSlot;
    QList<int>              mIndexAtSlot; // Can be bigger than rowcount if cyclic is used

};


#endif // RADIOSTRIPBASE_H_
