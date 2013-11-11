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
* Description:  stub HbListViewItem
* 
*/

#ifndef HBLISTVIEWITEM_H
#define HBLISTVIEWITEM_H

#include <QGraphicsItem>
#include <QModelIndex>

#include "hbwidget.h"

class HbAbstractViewItem : public HbWidget
{
public:
    enum
    {
        PositionAtTop
    };
    
public:
   
    /**
     * contructor
     */
    HbAbstractViewItem(QGraphicsItem *parent=0) :
        HbWidget(parent)
    {
    }
   
    /**
     * destructor
     */
    virtual ~HbAbstractViewItem(){}
    
    /**
     * returns mModelIndex
     */
    QModelIndex modelIndex()
    {
        return mModelIndex;
    }
    
    /**
     * item index
     */
    QModelIndex mModelIndex;
    
};


class HbListViewItem : public HbAbstractViewItem
{
    Q_OBJECT
    

public:

    enum GraphicsSize
    {
        InvalidSize,
        Thumbnail,
        WideThumbnail,
    };
    
   
    HbListViewItem(QGraphicsItem *parent=0) :
        HbAbstractViewItem(parent)
    {
    }
    
    virtual ~HbListViewItem(){}
    
    void setGraphicsSize(GraphicsSize size)
    {
        mLatestSize = size;
    }
    static GraphicsSize mLatestSize;
    
};

#endif
