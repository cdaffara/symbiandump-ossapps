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
* Description:
*
*/

#ifndef T_HS_MOCK_MODEL_H_EEEEE
#define T_HS_MOCK_MODEL_H_EEEEE

#include <qstandarditemmodel.h>
#include "caentry.h"
#include "hsmenuitemmodel.h"

class ProxyModelTestMockModel: public HsMenuItemModel
{
    Q_OBJECT
public:
    ProxyModelTestMockModel(int entryId) :
        HsMenuItemModel(CaQuery()),
        mEntryId(entryId) {}

    virtual QModelIndex index(int, int,
        const QModelIndex &parent = QModelIndex()) const {
        Q_UNUSED(parent);
        return QModelIndex();
    }
    virtual QVariant data(const QModelIndex &,
                              int role = Qt::DisplayRole) const {
        Q_UNUSED(role);
        return QVariant(mEntryId);
    }

    const CaEntry *entry(const QModelIndex &index) const;
    
private:
    
    int mEntryId;
};

#endif
