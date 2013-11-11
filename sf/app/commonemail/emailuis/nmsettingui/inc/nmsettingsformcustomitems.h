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

#ifndef NMSETTINGSFORMCUSTOMITEMS_H_
#define NMSETTINGSFORMCUSTOMITEMS_H_

#include <QObject>
#include <hbdataformviewitem.h>
#include <hbdataformmodelitem.h>

class QGraphicsItem;
class QModelIndex;


class NmSettingsFormCustomItems : public HbDataFormViewItem
{
    Q_OBJECT

public: // Data types

    enum ItemType {
        NmButtonItem = HbDataFormModelItem::CustomItemBase + 2
    };


public:

    NmSettingsFormCustomItems(QGraphicsItem *parent=NULL);

    ~NmSettingsFormCustomItems();

    bool canSetModelIndex(const QModelIndex &index) const;

    HbAbstractViewItem* createItem();

public slots:

    void restore();

protected:

    HbWidget* createCustomWidget();
};


#endif /* NMSETTINGSFORMCUSTOMITEMS_H_ */

// End of file.
