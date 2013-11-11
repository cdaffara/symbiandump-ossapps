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
 * Description:  This class provides custom list item for the
 * DataForm
 *
 */

#include <msgsettingsdataformcustomitem.h>
#include <hbdataformmodelitem.h>
#include <hbpushbutton.h>
#include <hbdataformmodel.h>
#include <hbdataform.h>

MsgSettingsDataFormCustomItem::MsgSettingsDataFormCustomItem(
        QGraphicsItem *parent) :
    HbDataFormViewItem(parent)
    {
    }

MsgSettingsDataFormCustomItem::~MsgSettingsDataFormCustomItem()
    {
    }

HbAbstractViewItem* MsgSettingsDataFormCustomItem::createItem()
    {
    return new MsgSettingsDataFormCustomItem(*this);
    }

HbWidget* MsgSettingsDataFormCustomItem::createCustomWidget()
    {
    HbDataFormModelItem::DataItemType
            itemType =
                    static_cast<HbDataFormModelItem::DataItemType> (modelIndex().data(
                            HbDataFormModelItem::ItemTypeRole).toInt());

    switch (itemType)
        {
        case HbDataFormModelItem::CustomItemBase + 1:
            {
            //custom button
            HbPushButton *button = new HbPushButton();

            return button;
            }
        default:
            return 0;
        }
    }

bool MsgSettingsDataFormCustomItem::canSetModelIndex(const QModelIndex &index) const
    {
    HbDataFormModelItem::DataItemType itemType =
            static_cast<HbDataFormModelItem::DataItemType> (index.data(
                    HbDataFormModelItem::ItemTypeRole).toInt());

    if (itemType >= HbDataFormModelItem::CustomItemBase && itemType
            <= HbDataFormModelItem::CustomItemBase + 10)
        {
        return true;
        }
    return false;
    }

void MsgSettingsDataFormCustomItem::restore()
    {
    HbDataFormModelItem
            * mItem =
                    static_cast<HbDataFormModel*> (itemView()->model())->itemFromIndex(
                            modelIndex());

    if (mItem)
        {
        QString str = mItem->contentWidgetData("text").toString();
        HbPushButton *button = static_cast<HbPushButton*>(dataItemContentWidget());
        if (button)
            {
            button->setText(str);
            }
        }
    }
