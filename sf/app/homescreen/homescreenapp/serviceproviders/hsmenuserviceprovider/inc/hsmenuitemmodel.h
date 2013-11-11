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
 * Description: hsmenuitemmodel.h
 *
 */

#ifndef HSMENUITEMMODEL_H
#define HSMENUITEMMODEL_H

// System includes
#include <caquery.h>
#include <caitemmodel.h>
#include <HbIcon>

// User includes
#include "hsmenuservice_global.h"
#include "hsmenuservice.h"

class CaUninstallNotifier;
class HsIconsIdleLoader;
// Class declaration
class MENUSERVICE_EXPORT HsMenuItemModel: public CaItemModel
{
HS_SERVICE_TEST_FRIEND_CLASS(MenuServiceTest)
Q_OBJECT
public:

    // Data types
    enum IconSize {
        SmallIconSize,
        BigIconSize
    };

    // Function declarations
    explicit HsMenuItemModel(const CaQuery &query,
                             QObject *parent = 0);
    ~HsMenuItemModel();
    void setSort(Hs::HsSortAttribute sortAttribute);
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const;
    bool newIconNeeded(const QModelIndex &index) const;
    void preloadIcons();
private slots:
    void uninstallChange(int componentId, int valueOfProgress);
    
private:
    HsIconsIdleLoader* mIconsIdleLoader;
    CaUninstallNotifier* mUninstallNotifier;//not own
    int mComponentId;
    QList<int> mIds;
};

#endif // HSMENUITEMMODEL_H 
