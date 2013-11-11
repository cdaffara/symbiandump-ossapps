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

#ifndef NMIPSSETTINGSCUSTOMITEM_H
#define NMIPSSETTINGSCUSTOMITEM_H

#include <HbDataFormViewItem>
#include <HbDataFormModelItem>

class QGraphicsItem;
class HbAbstractViewItem;
class QModelIndex;
class HbWidget;

class NmIpsSettingsCustomItem : public HbDataFormViewItem
{
    Q_OBJECT

public:

    enum { LabeledComboBox = HbDataFormModelItem::CustomItemBase + 10,
           TimeEditor, MultiSelectionItem};

    explicit NmIpsSettingsCustomItem(QGraphicsItem *parent=0);
    ~NmIpsSettingsCustomItem();
    HbAbstractViewItem* createItem();
    bool canSetModelIndex(const QModelIndex &index) const;

protected:

    virtual HbWidget* createCustomWidget();

private slots:

    void restore();
    void propertyChanged(QMap<QString, QVariant> properties);

private:

    void setWidgetProperty(const QString &property, const QHash<QString, QVariant> &properties);

};

#endif // NMIPSSETTINGSCUSTOMITEM_H
