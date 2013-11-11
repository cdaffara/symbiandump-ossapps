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

#include "filebrowsersortfilterproxymodel.h"

#include <QString>
#include <QStringList>

FileBrowserSortFilterProxyModel::FileBrowserSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool FileBrowserSortFilterProxyModel::filterAcceptsRow(int sourceRow,
                                                       const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    QVariant displayRole = index.data(Qt::DisplayRole);
    QStringList stringList;
    if (displayRole.isValid()) {
        if (displayRole.canConvert<QString>()) {
            stringList.append(displayRole.toString());
        } else if (displayRole.canConvert<QStringList>()) {
            stringList = displayRole.toStringList();
        }
        QString itemName = stringList.at(0);
        //QString criteria = filterRegExp().pattern();
        QString criteria = filterCriteria();
        if (!criteria.isEmpty()) {
            bool contains = itemName.contains(criteria, Qt::CaseInsensitive);
            return contains;
        } else {
            //invalidateFilter();
            return true;
        }
    }
    return true;
}

void FileBrowserSortFilterProxyModel::setFilterCriteria(const QString &criteria)
{
    mFilterCriteria = criteria;
    invalidate();
}


// End of File
