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

#include "t_hsmockmodel.h"

MockModel::MockModel():
    HsMenuItemModel(CaQuery())
{
    mEntry.setAttribute("uri",
                        "homescreen.nokia.com/widget/clock");
    mEntry.setAttribute("library",
                        "hsresources/import/widgetregistry/20022F43/hsclockwidgetprovider.dll");
    mEntry.setEntryTypeName("widget");
}

QModelIndex MockModel::fixedIndex() const
{
    return QAbstractItemModel::createIndex(0, 0, 0);
}

const CaEntry*
MockModel::entry(const QModelIndex &index) const
{
    const CaEntry *result = NULL;
    if (index == fixedIndex()) {
        result = &mEntry;
    }
    return result;
}
