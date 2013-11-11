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
#include <hbglobal.h>

#include "irmainmodel.h"
#include "irplaylist.h"

IRMainModel::IRMainModel(IRPlayList *aPlayList, QObject *aParent) : QAbstractListModel(aParent), iPlayList(aPlayList)
{
    initModel();
}

int IRMainModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iPrimaryTexts.count();
}

QVariant IRMainModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= rowCount(QModelIndex()))
        return QVariant();
    
    if (aRole == Qt::DisplayRole)
    {
        int row = aIndex.row();
        QVariantList list;
        list.append(iPrimaryTexts.at(row));
        return list;
    }
    
    return QVariant();
}

void IRMainModel::initModel()
{
#ifdef SUBTITLE_STR_BY_LOCID
    iPrimaryTexts << hbTrId("txt_irad_list_stations_by_country_region")
                  << hbTrId("txt_irad_list_stations_by_language")
                  << hbTrId("txt_irad_list_stations_recently_played")
                  << hbTrId("txt_irad_list_songs_recently_played");
#else
    iPrimaryTexts << hbTrId("Stations by country/region")
                  << hbTrId("Stations by language")
                  << hbTrId("Stations recently played")
                  << hbTrId("Songs recently played");    
#endif
}

void IRMainModel::checkUpdate()
{
    if (NULL == iPlayList)
    {
        return;
    }
    
#ifdef SUBTITLE_STR_BY_LOCID
    QString primary = hbTrId("txt_irad_subtitle_stations_from_play_list");
#else
    QString primary = hbTrId("Stations from play list");    
#endif
    
    if (iPlayList->getNumberOfEntries() > 0)
    {
        //if pls item is not in collections view, add
        if (iPrimaryTexts.indexOf(primary) == -1)
        {
            int number = iPrimaryTexts.count();
            beginInsertRows(QModelIndex(), number, number);
            iPrimaryTexts << primary;
            endInsertRows();
        }
    }
    else
    {
        //if pls item is in collections view, remove
        int index = iPrimaryTexts.indexOf(primary);
        if (index != -1)
        {
            beginRemoveRows(QModelIndex(), index, index);
            iPrimaryTexts.removeAt(index);
            endRemoveRows();
        }
    }
}
