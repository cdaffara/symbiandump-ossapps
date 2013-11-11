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
#include "irplsmodel.h"
#include "irplaylist.h"

IRPlsModel::IRPlsModel(IRPlayList *aPlayList, QObject *aParent) : QAbstractListModel(aParent), iPlayList(aPlayList)
{
    
}

int IRPlsModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iPlayList->getNumberOfEntries();
}

QVariant IRPlsModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= rowCount(QModelIndex()))
        return QVariant();
    
    if (aRole == Qt::DisplayRole)
    {
        int row = aIndex.row();
        QVariantList list;
        IRPlayListItem *item = iPlayList->getEntry(row);
        if (item)
        {
            list.append(item->title());
            list.append(item->file());
        }
        return list;
    }
    
    return QVariant();
}

const QString& IRPlsModel::getFile(int aIndex) const
{
    IRPlayListItem *item = iPlayList->getEntry(aIndex);
    return item->file();
}

const QString& IRPlsModel::getTitle(int aIndex) const
{
    IRPlayListItem *item = iPlayList->getEntry(aIndex);
    return item->title();
}
