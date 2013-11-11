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
* Description:  stub hbabstractitemview
* 
*/

#ifndef HBABSTRACTITEMVIEW_H
#define HBABSTRACTITEMVIEW_H

#include "hbwidget.h"
#include <QGraphicsWidget>
#include <QItemSelectionModel>

class HbAbstractViewItem;

class HbAbstractItemView : public HbWidget
{
    Q_OBJECT
    
    Q_PROPERTY(ItemAnimations enabledAnimations READ enabledAnimations WRITE setEnabledAnimations)
    
public:
    
    enum ItemAnimation
    {
        None               = 0x00000,
        Appear             = 0x00001,
        Disappear          = 0x00002,
        TouchDown          = 0x00004,
        All                = 0xFFFFF
    };
    
    Q_DECLARE_FLAGS(ItemAnimations, ItemAnimation)
    
    enum SelectionMode
    {
        InvalidSelection,
        NoSelection,
        SingleSelection,
        MultiSelection,
        ContiguousSelection
    };
    
    HbAbstractItemView(QGraphicsWidget *parent = 0) : HbWidget(parent){}
    
    void setEnabledAnimations(ItemAnimations flags)
    {
        Q_UNUSED(flags);
    }
    
    ItemAnimations enabledAnimations() const
    {
        return None;
    }
    
signals:

    void activated(const QModelIndex &index);
    
    void longPressed(HbAbstractViewItem *item, const QPointF &point);

};

#endif
