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
#include <QVariant>

#include "irqfavoritesdb.h" 
#include "favoriteswrapper.h"
#include "channelhistorywrapper.h"
#include "urlinfowrapper.h"
#include "irqisdsdatastructure.h"
#include "irqenums.h"

IRQFavoritesDB::IRQFavoritesDB()
{
    iFavoritesWrapper = new favoritesWrapper;
    iHistoryWrapper = new channelHistoryWrapper;
    iUrlWrapper = new urlInfoWrapper;
}

IRQFavoritesDB::~IRQFavoritesDB()
{
    delete iFavoritesWrapper;
    delete iHistoryWrapper;
    delete iUrlWrapper;
}

//add a preset to favorites
//@param IRQPreset& the preset
//@return  errcode
int IRQFavoritesDB::addPreset(const IRQPreset& aPreset)
{
    bool ret = false;
    bool newRow = false;
    
    columnMap map;
    map.insert(channelId, QString::number(aPreset.presetId));
    ret = iFavoritesWrapper->putFavorites(&map, newRow);
    
    if (!ret)
    {
        return EIRQErrorNotFound;  //presetId is not found
    }
    
    if (newRow)
    {
        return EIRQErrorNone;
    }
    else
    {
        return EIRQErrorAlreadyExist;
    }
}

//delete a preset
//@return errcode
//@param
//
int IRQFavoritesDB::deletePreset(const IRQPreset& aPreset)
{
    columnMap map;
    map.insert(channelId, QString::number(aPreset.presetId));
    bool ret = iFavoritesWrapper->deleteFavorites(&map, NULL);

    if (ret)
    {
        return EIRQErrorNone;
    }
    else
    {
        return EIRQErrorNotFound;
    }
}

//replace with new preset
//@return errcode 
//
void IRQFavoritesDB::replacePreset(const IRQPreset& aNewPreset)
{
    //write the preset data to database
    columnMap map;
    QString url;
    
    if (IRQPreset::EIsds == aNewPreset.type)
    {
        map.insert(channelId, QString::number(aNewPreset.presetId));
    }
    map.insert(channelName, aNewPreset.name);
    map.insert(languageCode, aNewPreset.languageCode);
    map.insert(languageName, aNewPreset.languageName);
    map.insert(countryCode, aNewPreset.countryCode);
    map.insert(countryName, aNewPreset.countryName);
    map.insert(lastModified, aNewPreset.lastModified);
    map.insert(musicStoreStatus, aNewPreset.musicStoreStatus);
    map.insert(description, aNewPreset.description);
    map.insert(shortDesc, aNewPreset.shortDesc);
    map.insert(genreId, aNewPreset.genreId);
    map.insert(genreName, aNewPreset.genreName);
    map.insert(advertisementUrl, aNewPreset.advertisementUrl);
    map.insert(advertisementInUse, aNewPreset.advertisementInUse);
    map.insert(imgUrl, aNewPreset.imgUrl);
    
    uint cIdResult = 0;
    if (IRQPreset::EIsds == aNewPreset.type)
    {
        cIdResult = aNewPreset.presetId;
        columnMap condAnd;
        condAnd.insert(channelId, QString::number(aNewPreset.presetId));
        iHistoryWrapper->putChannelHistory(&map, NULL, NULL, &condAnd);
    }
    else
    {
        columnMap condAnd;
        condAnd.insert(channelType, QString::number(aNewPreset.type));
        aNewPreset.getChannelUrlAt(0, url);
        condAnd.insert(channelUrl, url);
        condAnd.insert(channelName, aNewPreset.name);
        QList<uint> *ids = iHistoryWrapper->getChannelId(&condAnd);
        if (ids && ids->count() > 0)
        {
            //the user-defined preset is already in database
            cIdResult = ids->at(0);
            condAnd.clear();
            condAnd.insert(channelId, QString::number(cIdResult));
            iHistoryWrapper->putChannelHistory(&map, NULL, NULL, &condAnd);
        }
        else
        {
            //the user-defined preset is not in database
            iHistoryWrapper->putChannelHistory(&map, &cIdResult);
        }
        
        delete ids;
    }
    
    //write url info to database
    columnUrlInfoInsertMap urlmap;
    
    unsigned int bitrate = 0;
    for (int i = 0; i < aNewPreset.getChannelURLCount(); i++)
    {
        aNewPreset.getChannelBitrate(i, bitrate);
        aNewPreset.getChannelUrlAt(i, url);
        urlmap.insert(url, bitrate);
    }
    
    iUrlWrapper->resetUrlInfo(&urlmap, cIdResult);
}

//change the preset type to user defined
//@return errcode
//
int IRQFavoritesDB::makePresetUserDefined(int /*aChannelId*/, int /*aUserDefinedChannelId*/)
{
    return 0;
}

//for CIRPreset is just an interface so we can wrapper it into the IRQPreset.
//the interface get the IRQPreset list. The upper layer will free all the items
//in the list and the list self
//
QList<IRQPreset*>* IRQFavoritesDB::getPresets() const
{
    favoritesWrapper favorites;
    QList<QVariant*> *dataSet = favorites.getFavorites();
    if (NULL == dataSet)
    {
        return NULL;
    }
    
    QList<IRQPreset*> *presetList = NULL;
    if (dataSet->count() > 0)
    {
        presetList = new QList<IRQPreset*>;
        int dataSize = dataSet->count();
        for (int i = 0; i < dataSize; ++i)
        {
            IRQPreset *preset = new IRQPreset;
            bool ok = false;
            QVariant *row = dataSet->at(i);
            preset->type = row[channelType].toInt(&ok);
            preset->presetId = row[channelId].toUInt(&ok);
            preset->name = row[channelName].toString();
            preset->nickName = row[channelNickName].toString();
            preset->languageCode = row[languageCode].toString();
            preset->languageName = row[languageName].toString();
            preset->countryCode = row[countryCode].toString();
            preset->countryName = row[countryName].toString();
            preset->lastModified = row[lastModified].toString();
            preset->musicStoreStatus = row[musicStoreStatus].toString();
            preset->description = row[description].toString();
            preset->shortDesc = row[shortDesc].toString();
            preset->genreName = row[genreName].toString();
            preset->genreId = row[genreId].toString();
            preset->advertisementInUse = row[advertisementInUse].toString();
            preset->advertisementUrl = row[advertisementUrl].toString();
            preset->imgUrl = row[imgUrl].toString();
            
            delete []row;
            
            //get url info and write to preset
            urlInfoWrapper urlInfo;
            columnMap cond;
            cond.insert(channelId, QString::number(preset->presetId));
            QList<QVariant*> *urlDataSet = urlInfo.getUrlInfo(&cond, NULL, i, dataSize-1);
            int urlSize =  urlDataSet->count();
            for (int j = 0; j < urlSize; ++j)
            {
                IRQChannelServerURL url;
                url.url = urlDataSet->at(j)[channelUrl_URL].toString();
                url.bitrate = urlDataSet->at(j)[bitRate_URL].toInt();
                preset->insertChannelServer(url);
                delete []urlDataSet->at(j);
            }
            delete urlDataSet;
            
            presetList->append(preset);
        }
    }
    
    delete dataSet;
    
    return presetList;
}

int IRQFavoritesDB::renamePreset(const IRQPreset &aPreset, const QString &aNewName)
{
    channelHistoryWrapper wrapper;
    columnMap map;

    map.insert(channelNickName, aNewName);
    columnMap condAND;
    condAND.insert(channelId, QString::number(aPreset.presetId));
    bool ret = wrapper.putChannelHistory(&map, NULL, NULL, &condAND);
    
    if (ret)
    {
        return EIRQErrorNone;
    }
    else
    {
        return EIRQErrorGeneral;
    }
}
