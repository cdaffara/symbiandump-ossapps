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
 * Description: caquery.h
 *
 */

#ifndef CAQUERY_H
#define CAQUERY_H

#include <QStringList>
#include <QString>
#include <QDebug>
#include <QMap>
#include "caclient_global.h"
#include "cadefs.h"

// mocks Content Storage CaQuery
class CaQuery
{

public:
    CaQuery():
        mSetParentId(0),
        mSetSortAttribute(NameSortAttribute),
        mSetSortOrder(Qt::AscendingOrder)
    {}

    void setEntryRoles(EntryRoles entryRoles);

    void setParentId(int id);


    void addEntryTypeName(const QString &entryTypeName);

    void setFlagsOn(const EntryFlags &onFlags);

    void setFlagsOff(const EntryFlags &offFlags);

    void setSort(SortAttribute sortAttribute,
        Qt::SortOrder sortOrder = Qt::AscendingOrder);

    QMap<QString, QString> attributes() const { return QMap<QString, QString>(); }
    QString attribute(const QString &name) const {
        Q_UNUSED(name);
        return QString(); }
    void setAttribute(const QString &name, const QString &value) {
        Q_UNUSED(name);
        Q_UNUSED(value);
    }

    void removeAttribute(const QString &name) { Q_UNUSED(name); }


    // test supporting members
    mutable int mSetParentId;
    mutable EntryRoles mSetEntryRoles;
    mutable QList<QString> mAddEntryTypeNames;
    mutable EntryFlags mSetFlagsOn;
    mutable EntryFlags mSetFlagsOff;
    mutable SortAttribute  mSetSortAttribute;
    mutable Qt::SortOrder mSetSortOrder;
    mutable QString mQueryId;

    // test helper, not belonging to Content Storage CaQuery
    void clear()
    {
        mSetParentId = 0;
        mSetEntryRoles = 0;
        mAddEntryTypeNames.clear();
        mSetFlagsOn = 0;
        mSetFlagsOff = 0;
        mSetSortAttribute = NameSortAttribute;
        mSetSortOrder = Qt::AscendingOrder;
    }

};

inline void CaQuery::setEntryRoles(EntryRoles entryRoles)
{
    mSetEntryRoles = entryRoles;
}

inline void CaQuery::setParentId(int id)
{
    mSetParentId = id;
}


inline void CaQuery::addEntryTypeName(const QString &entryTypeName)
{
    mAddEntryTypeNames.append(entryTypeName);
}

inline void CaQuery::setFlagsOn(const EntryFlags &onFlags)
{
    mSetFlagsOn = onFlags;
}

inline void CaQuery::setFlagsOff(const EntryFlags &offFlags)
{
    mSetFlagsOff = offFlags;
}

inline void CaQuery::setSort(SortAttribute sortAttribute,
    Qt::SortOrder sortOrder)
{
    mSetSortAttribute = sortAttribute;
    mSetSortOrder = sortOrder;
}
#endif // CAQUERY_H
