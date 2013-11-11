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
#include "irqsonghistoryinfo.h"
#include "irsonghistorymodel.h"
#include "songhistorywrapper.h"
#include "irdbviewstruct.h"

IRSongHistoryModel::IRSongHistoryModel(QObject *aParent) : QAbstractListModel(aParent),
                                                           mHistoryWrapper(NULL)
{   
    mHistoryWrapper = new songHistoryWrapper();
    getAllList();
}

IRSongHistoryModel::~IRSongHistoryModel()
{
    while (!mSongHistoryList.isEmpty())
    {
        delete mSongHistoryList.takeFirst();
    }   
    
    delete mHistoryWrapper;
}

int IRSongHistoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return mSongHistoryList.count();
}
 
 
QVariant IRSongHistoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }
    
    int row = aIndex.row();

    if (aIndex.row() >= mSongHistoryList.count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;      
             
        QString artistName = mSongHistoryList.at(row)->getArtistName().trimmed();
        if( "" == artistName )
        {
#ifdef SUBTITLE_STR_BY_LOCID
            artistName = hbTrId("txt_irad_list_unknown_artist_p");
#else
            artistName = hbTrId("Unknown artist");            
#endif
        }
      
        QString songName = mSongHistoryList.at(row)->getSongName().trimmed();
        if( "" == songName )
        {
#ifdef SUBTITLE_STR_BY_LOCID
            songName = hbTrId("txt_irad_list_unknown_song_p");
#else
            songName = hbTrId("Unknown song");
#endif
        }
        
        QString stationName = mSongHistoryList.at(row)->getStationName().trimmed();       
        if (Qt::Vertical == mOrientation)
        {
            list.append(artistName);
            list.append(songName);
        }
        else
        {
            list.append(artistName + " - " + songName);
            list.append(stationName);
        }
        return list;
    }

    return QVariant();
}

IRQSongInfo* IRSongHistoryModel::getSongHistoryInfo(int aIndex)
{
    if (aIndex >= 0 && aIndex < mSongHistoryList.count())
    {
        return mSongHistoryList.at(aIndex);
    }

    return NULL;
}

void IRSongHistoryModel::clearList()
{
    while (!mSongHistoryList.isEmpty())
    {
        IRQSongInfo *firstItem = mSongHistoryList.takeFirst();
        delete firstItem;
    }    
    mHistoryWrapper->deleteSongHistory();

    emit modelChanged();
}

bool IRSongHistoryModel::checkSongHistoryUpdate()
{   
    getAllList();    
    return true;
}
 

void IRSongHistoryModel::getAllList()
{
    
    QList<QVariant*> *dataSet = mHistoryWrapper->getSongHistory();
    int dataCount = 0;
    
    if( NULL != dataSet )
    {
        dataCount = dataSet->count();
    }

    while (!mSongHistoryList.isEmpty())
    {
        IRQSongInfo *firstItem = mSongHistoryList.takeFirst();
        delete firstItem;
    }
     
    QString stationsName, songsName, artistsName, status, channelsID;

    if (0 < dataCount)
    {
        for (int i = 0; i < dataCount; ++i)
        {
            IRQSongInfo *songInfo = new IRQSongInfo();
            QVariant *row = dataSet->at(i);
            stationsName = row[channelNickName].toString();
            songsName = row[songName].toString();
            artistsName = row[artistName].toString();
            status = row[musicStoreStatus].toString();
            channelsID = row[channelId].toString();
            songInfo->setAllInfo(stationsName, songsName, artistsName, status, channelsID);

            delete[] row;
            mSongHistoryList.append(songInfo);
        }
    }

    delete dataSet;
    emit modelChanged();
}

void IRSongHistoryModel::setOrientation(Qt::Orientation aOrientation)
{
    mOrientation = aOrientation;
}

bool IRSongHistoryModel::deleteOneItem(int aIndex)
{
    if( aIndex < 0 || aIndex >= mSongHistoryList.count() )
    {
        return false;
    }
    
    IRQSongInfo *currentItem = mSongHistoryList.at(aIndex);   
     
    columnMap map;
    map.insert(songName, currentItem->getSongName());
    map.insert(artistName, currentItem->getArtistName());
    map.insert(channelId, currentItem->getChannelID());

    bool ret = mHistoryWrapper->deleteSongHistory(&map);
    
    if( !ret )
    {
        return false;                
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    //delete the current item to avoid memory leaking
    delete currentItem;
    mSongHistoryList.removeAt(aIndex);
    endRemoveRows(); 
    
    emit modelChanged();    
    return true;
}
