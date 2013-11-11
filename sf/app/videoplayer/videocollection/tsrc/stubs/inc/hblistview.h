/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  stub hbview
*
*/

#ifndef HBLISTVIEW_H
#define HBLISTVIEW_H

#include <hbglobal.h>
#include "hbwidget.h"
#include "hblistviewitem.h"
#include "hbscrollbar.h"
#include "hbabstractitemview.h"
#include "videoproxymodelgeneric.h"

#include <QGraphicsItem>
#include <QModelIndex>
#include <qgraphicswidget.h>
#include <QList>
#include <QPointF>
#include <qitemselectionmodel.h>

class HbScrollArea
{
public:
    enum ClampingStyle
    {
        InvalidClamping,
        BounceBackClamping
    };
    enum ScrollingStyle
    {
        InvalidScrolling,
        PanWithFollowOn
    };
};

class HbListView : public HbAbstractItemView
{
    Q_OBJECT
    
public:

signals:

    /**
     * dummy signal
     */
    void scrollPositionChanged(const QPointF &);
    
    /**
     * dummy signal
     */
    void scrollingEnded();
    
    /**
     * dummy signal
     */
    void scrollingStarted();
        
public:  
    /**
     * contructor
     */
    HbListView(QGraphicsItem *parent = 0) :
            mItem(0), mVerticalSB(0), mSelectionModel(0)
    {
        Q_UNUSED(parent);
        mItem = new HbListViewItem();
        mVerticalSB = new HbScrollBar();
    }
    
    /**
     * destructor
     */
    virtual ~HbListView()
    {
        delete mSelectionModel;
        delete mItem;
        delete mVerticalSB;
        QList<HbAbstractViewItem*>::const_iterator iter = mVisibleItems.constBegin();
        while(iter != mVisibleItems.constEnd())
        {
            delete *iter;
            ++iter;            
        }
        mVisibleItems.clear();
    }
    
    HbScrollBar* verticalScrollBar()
    {
        if(mVScrollBarIsNull)
        {
            return 0;
        }
        return mVerticalSB;
    }
    
    /**
     * dummy method
     */
    void setModel(VideoProxyModelGeneric *model)
    {
        mLatestModel = model;
    }
    
    /**
     * dummy method
     */
    HbListViewItem *listItemPrototype()
    {
        if(mReturnNullPrototype)
        {
            return 0;
        }
        return mItem;    
    }
    
    /**
     * dummy method
     */
    void setItemRecycling(bool value)
    {
        mLatestrecycling = value;
    }
    
    /**
     * dummy method
     */
    void setClampingStyle(HbScrollArea::ClampingStyle style)
    {
        mLatestClamping = style;    
    }
    
    /**
     * dummy method
     */
    void setScrollingStyle(HbScrollArea::ScrollingStyle style)
    {
        mLatestScrolling = style;
    }
    
    /**
     * dummy method
     */
    void setFrictionEnabled(bool value)
    {
        mLatestFrictionEnabled = value;
    }
    
    /**
     * dummy method
     */
    void setUniformItemSizes(bool value)
    {
        mLatestUniformItemSizes = value;
    }
    
    /**
     * dummy method
     */
    void setVisible(bool value)
    {
        mLatestVisibility = value;
    }
    
    /**
     * dummy method
     */
    void setEnabled(bool value)
    {
        mLatestEnableValue = value;
    }
    
    /**
     * dummy method
     */
    void setSelectionMode(HbAbstractItemView::SelectionMode mode)
    {
        mSelectionMode = mode;
    }
    
    QList<HbAbstractViewItem*> visibleItems()
    {
        return mVisibleItems;
    }
    
    /**
     * dummy method
     */
    HbAbstractItemView::SelectionMode selectionMode()
    {
        return mSelectionMode;
    }
    
    /**
     * dummy method
     */
    QModelIndex currentIndex()
    {
        return mCurrentIndex;
    }
    
    /**
     * sets mLongPressedPoint
     */
    void longPressGesture (const QPointF &point)
    {
        mLongPressedPoint = point;
    }
    
    /**
     * sets mLongPressedPoint
     */
    void panGesture (const QPointF &point)
    {
        mPanGesturePoint = point;
    }
    
    /**
     * dummy method
     */
    void clearSelection()
    {
        mAllSelectedStatus = 0;
    }
    
    /**
     * dummy method
     */
    void scrollTo(const QModelIndex &index)
    {
        mLatestScrollToIndex = index;
    }
    
    /**
     * dummy method
     */
    void selectAll()
    {
        mAllSelectedStatus = 1;
    }
    
    /**
     * dummy method
     */
    QItemSelectionModel *selectionModel()
    {
        if (!mSelectionModel)
        {
            mSelectionModel = new QItemSelectionModel(mLatestModel);
        }
        return mSelectionModel;
    }

    /**
     * dummy method
     */
    HbAbstractViewItem* itemAtPosition(const QPointF &position)
    {
        Q_UNUSED(position);
        return mItem;
    }
    
    void setObjectName(const QString name)
    {
        Q_UNUSED(name);
    }
    
    /**
     * dummy method
     */
    void rowsInserted(const QModelIndex &parent, int start, int end)
    {
        Q_UNUSED(parent);
        Q_UNUSED(start);
        Q_UNUSED(end);
    }

    /**
     * dummy method
     */
    void rowsRemoved(const QModelIndex &parent, int start, int end)
    {
        Q_UNUSED(parent);
        Q_UNUSED(start);
        Q_UNUSED(end);
    }
    
    /**
     * Dummy method
     */
    void setItemPixmapCacheEnabled(bool enabled)
    {
        Q_UNUSED(enabled);    
    }
    
public:
    
    /**
     * items to be returned from visibleItems
     */
    static QList<HbAbstractViewItem*> mVisibleItems;
    
    /**
     * selection model to be returned 
     */
    static HbAbstractItemView::SelectionMode mSelectionMode;
    
    /**
     * provided model
     */
    static VideoProxyModelGeneric *mLatestModel;
    
    /**
     * -1 == initialized
     *  0 == nothing selected
     *  1 == all selected
     */
    static int mAllSelectedStatus;
    
    /**
     * item to returned from listItemPrototype and from itemAtPosition
     */
    HbListViewItem *mItem;
    
    /**
     * item to be returned from verticalScrollBar
     */
    HbScrollBar *mVerticalSB;
    
    /**
     * selection model
     */
    QItemSelectionModel *mSelectionModel;

    /**
     * if true verticalScrollBar return null
     */
    static bool mVScrollBarIsNull;
    
    /**
     * if true, listItemPrototype returns null 
     */
    static bool mReturnNullPrototype; 
    
    /**
     * latest value settes in setItemRecycling
     */
    static bool mLatestrecycling;
    
    /**
     * value setted in longPressGesture
     */
    static QPointF mLongPressedPoint;
    
    /**
     * value setted in panGesture
     */
    static QPointF mPanGesturePoint;
    
    /**
     * latest value settes in setClampingStyle
     */
    static HbScrollArea::ClampingStyle mLatestClamping;
    
    /**
     * latest value setted in setScrollingStyle
     */
    static HbScrollArea::ScrollingStyle mLatestScrolling;
    
    /**
     * latest value setted in setFrictionEnabled
     */
    static bool mLatestFrictionEnabled;
    
    /**
     * latest value setted in setUniformItemSizes
     */
    static bool mLatestUniformItemSizes;
    
    /**
     * latest value setted in setVisible
     */
    static bool mLatestVisibility;
    
    /**
     * latest value setted in setEnabled
     */
    static bool mLatestEnableValue;
    
    /**
     *  latest index setted in scrollTo
     */
    static QModelIndex mLatestScrollToIndex;
    
    /**
     * index value to be returned from current index 
     */
    static QModelIndex mCurrentIndex;
};

#endif 
