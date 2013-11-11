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
* Description:
*
*/

#include <QVariant>
#include <HbAbstractItemView>
#include <HbDataFormModel>
#include <HbDataFormModelItem>

#include "nmipssettingscustomitem.h"
#include "nmipssettingslabeledcombobox.h"
#include "nmipssettingstimeeditor.h"
#include "nmipssettingsmultiselectionitem.h"

/*!
    \class NmIpsSettingsCustomItem
    \brief The class implements a custom HbDataFormViewItem.
*/

// ======== MEMBER FUNCTIONS ========

/*!
    Constructor of NmIpsSettingsCustomItem.
*/
NmIpsSettingsCustomItem::NmIpsSettingsCustomItem(QGraphicsItem *parent)
: HbDataFormViewItem(parent)
{
}

/*!
    Destructor of NmIpsSettingsCustomItem.
*/
NmIpsSettingsCustomItem::~NmIpsSettingsCustomItem()
{
}

/*!
    Creates the custom data form view item.
*/
HbAbstractViewItem* NmIpsSettingsCustomItem::createItem()
{
    return new NmIpsSettingsCustomItem(*this);
}

/*!
    \param index Model index of the item.
    \return true if item type for the index is supported, otherwise false.
*/
bool NmIpsSettingsCustomItem::canSetModelIndex(const QModelIndex &index) const
{
    int type(index.data(HbDataFormModelItem::ItemTypeRole).toInt());
    return type == LabeledComboBox || type == TimeEditor || type == MultiSelectionItem;
}

/*!
    Creates the custom widget.
*/
HbWidget *NmIpsSettingsCustomItem::createCustomWidget()
{
    QVariant data(modelIndex().data(HbDataFormModelItem::ItemTypeRole));
    int type(data.toInt());

    HbWidget *widget = NULL;

    switch (type) {
        case LabeledComboBox: {
            widget = new NmIpsSettingsLabeledComboBox();
            connect(widget, SIGNAL(propertyChanged(QMap<QString, QVariant>)),
                    this, SLOT(propertyChanged(QMap<QString, QVariant>)));
            break;
            }
        case TimeEditor: {
            widget = new NmIpsSettingsTimeEditor();
            break;
            }
        case MultiSelectionItem: {
            widget = new NmIpsSettingsMultiSelectionItem();
            connect(widget, SIGNAL(propertyChanged(QMap<QString, QVariant>)),
                this, SLOT(propertyChanged(QMap<QString, QVariant>)));
            break;
        }
        default: {
            // Unknown/unhandled item type.
            break;
        }
    }

    return widget;
}

/*!
    Sets the custom widget's properties from the model item.
*/
void NmIpsSettingsCustomItem::restore()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>(
        modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());

    if (itemType == LabeledComboBox || itemType == TimeEditor || itemType == MultiSelectionItem) {

        HbDataFormModel *model = static_cast<HbDataFormModel *>
            (static_cast<HbAbstractViewItem *>(this)->itemView()->model());

        HbDataFormModelItem *modelItem = model->itemFromIndex(modelIndex());

        QHash<QString ,QVariant> properties =
            modelItem->data(HbDataFormModelItem::PropertyRole).toHash();

        if (itemType == LabeledComboBox) {
            // Set combobox properties in specific order. currentIndex must be set last so that
            // both the labelTexts and comboItems have been set before. Also, labelTexts must
            // have been set before comboItems. Otherwise the setting of current item doesn't work.
            // Block signals which are generated when comboItems are set.
            setWidgetProperty("labelTexts", properties);
            dataItemContentWidget()->blockSignals(true);
            setWidgetProperty("comboItems", properties);
            dataItemContentWidget()->blockSignals(false);
            setWidgetProperty("currentIndex", properties);

        } else {
            // Set item properties. Simply copy all set properties to the widget.
            QStringList propertyNames = properties.keys();
            foreach (QString key, propertyNames) {
                QVariant value = properties.value(key);
                dataItemContentWidget()->setProperty(key.toAscii().data(), value);
            }
        }
    }
}

/*!
    Copies all given properties from widget to model.

    \param properties Composited widget properties.
*/
void NmIpsSettingsCustomItem::propertyChanged(QMap<QString, QVariant> properties)
{
    HbDataFormModel* model =
        static_cast<HbDataFormModel*>(static_cast<HbAbstractViewItem*>(this)->itemView()->model());

    model->blockSignals(true);
    HbDataFormModelItem* modlItem = model->itemFromIndex(modelIndex());
    QStringList keys = properties.keys();
    foreach (QString key, keys) {
        modlItem->setContentWidgetData(key, properties.value(key));
    }
    model->blockSignals(false);
}

/*!
    Sets \a property to the content widget if found from \a properties.
    \param property Name of the property to set.
    \param properties Available properties.
*/
void NmIpsSettingsCustomItem::setWidgetProperty(const QString &property,
    const QHash<QString, QVariant> &properties)
{
    if (properties.contains(property)) {
        dataItemContentWidget()->setProperty(property.toAscii().data(), properties.value(property));
    }
}
