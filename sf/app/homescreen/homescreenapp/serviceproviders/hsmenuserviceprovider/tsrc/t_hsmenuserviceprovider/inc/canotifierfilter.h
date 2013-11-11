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
 * Description: canotifierfilter.h
 *
 */

#ifndef CANOTIFIERFILTER_H
#define CANOTIFIERFILTER_H

#include <QList>
#include <cadefs.h>

class CaQuery;

class CaNotifierFilter
{

public:

    CaNotifierFilter() {}
    virtual ~CaNotifierFilter() {}


    QList<int> getIds() const {return mIds;}
    void setIds(const QList<int> &ids) {mIds = ids;}

    EntryRoles getEntryRole() const;
    void setEntryRole(EntryRoles entryRole);

    QStringList getTypeNames() const;
    void setTypeNames(const QStringList &typeNames);

    QList<int> mIds;
};

#endif // CANOTIFIERFILTER_H
