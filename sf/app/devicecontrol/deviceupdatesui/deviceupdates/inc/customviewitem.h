/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  Custom list item implementation
 *
 */

#ifndef CUSTOMVIEWITEM_H
#define CUSTOMVIEWITEM_H

#include <hblistviewitem.h>
#include <hbpushbutton.h>
#include <hbview.h>
#include <hbmenu.h>
#include <HbMainWindow.h>
#include <hbaction.h>

class DmAdvancedView;
const qreal topAndBottomMargin = 0;
const qreal leftMargin = 40;
const qreal rightMargin = 50;

class CustomViewItem : public HbListViewItem
    {
Q_OBJECT
public:
    enum
        {
        ItemType = Hb::ItemType_Last + 1001
        };
    CustomViewItem(DmAdvancedView* serversview, QGraphicsItem * parent = 0);
    ~CustomViewItem();
    int type() const;
    bool canSetModelIndex(const QModelIndex &index);
    HbAbstractViewItem * createItem();
    void updateChildItems();
private:
    HbPushButton *mButton;
    DmAdvancedView* callBackView;
    };

#endif 

