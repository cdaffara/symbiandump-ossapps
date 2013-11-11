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
 * Description: hsmenucollectionsitemmodel.h
 *
 */

#ifndef HSMENUCOLLECTIONSITEMMODEL_H
#define HSMENUCOLLECTIONSITEMMODEL_H

// System includes
#include <caquery.h>
#include <HbIcon>

// User includes
#include "hsmenuservice_global.h"
#include "hsmenuitemmodel.h"

// Class declaration
class MENUSERVICE_EXPORT HsMenuCollectionsItemModel: public HsMenuItemModel
{

public:

    // Function declarations
    explicit HsMenuCollectionsItemModel(const CaQuery &query,
                                        QObject *parent = 0);
    ~HsMenuCollectionsItemModel();
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;

private:

    // Function declarations
    QString getSecondLine(const CaEntry *entry) const;

};

#endif // HSMENUCOLLECTIONSITEMMODEL_H
