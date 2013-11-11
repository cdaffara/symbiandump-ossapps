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
 * Description:  Custom list item implementation.
 *
 */

#include <hblistviewitem.h>
#include <QGraphicsLinearLayout.h>
#include "customviewitem.h"
#include "dmadvancedview.h"

CustomViewItem::CustomViewItem(DmAdvancedView* serversview,
        QGraphicsItem * parent) :
    HbListViewItem(parent), mButton(0), callBackView(serversview)
    {
    }

CustomViewItem::~CustomViewItem()
    {
    }

HbAbstractViewItem * CustomViewItem::createItem()
    {
    return new CustomViewItem(*this);
    }

void CustomViewItem::updateChildItems()
    {
    int itemType(modelIndex().data(Hb::ItemTypeRole).toInt());
    if ( itemType == CustomViewItem::ItemType )
        {
        if ( mButton == 0 )
            {
            QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
            mButton = new HbPushButton(this);

            mButton->setText(hbTrId(
                    "txt_device_update_button_new_server_profile"));            
            connect(mButton, SIGNAL(clicked()), callBackView, SLOT(
                    createNewProfile()));
            layout->addItem(mButton);            
            layout->setContentsMargins(leftMargin, topAndBottomMargin,
                    rightMargin, topAndBottomMargin);
            setStretchingStyle(HbListViewItem::StretchLandscape);
            setLayout(layout);
            }
        }
    else
        {
        HbListViewItem::updateChildItems();
        setStretchingStyle(HbListViewItem::StretchLandscape);
        }
    }

bool CustomViewItem::canSetModelIndex(const QModelIndex &index)
    {
    Q_UNUSED(index);
    return true;
    }

int CustomViewItem::type() const
    {    
    return CustomViewItem::ItemType;
    }

