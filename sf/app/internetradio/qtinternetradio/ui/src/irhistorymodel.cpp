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

#include <HbIcon>
#include <QTimer>

#include "irqisdsdatastructure.h"
#include "channelhistorywrapper.h"
#include "urlinfowrapper.h"
#include "irhistorymodel.h"
#include "irqisdsclient.h"
#include "irlogoprovider.h"

IRHistoryModel::IRHistoryModel(QObject *aParent) : QAbstractListModel(aParent),
    iHistoryEngine(NULL), iUrlInfoWrapper(NULL), iStationLogo(NULL),
    iIsdsClient(NULL), iLogoProvider(NULL), iTimer(NULL)
{
    iStationLogo = new HbIcon("qtg_large_internet_radio");

    iHistoryEngine = new channelHistoryWrapper();
    
    iUrlInfoWrapper = new urlInfoWrapper();
    
    iIsdsClient = IRQIsdsClient::openInstance();
    iLogoProvider = new IRLogoProvider(iIsdsClient);
    
    iTimer = new QTimer();
    iTimer->setInterval(10);
    connect(iTimer, SIGNAL(timeout()), this, SLOT(downloadNextLogo()));
        
    refreshModel();
}

IRHistoryModel::~IRHistoryModel()
{
    clearModel();
	
    delete iStationLogo;
    iStationLogo = NULL;
    
    delete iHistoryEngine;
    iHistoryEngine = NULL;
    
    delete iUrlInfoWrapper;
    iUrlInfoWrapper = NULL;
    
	stopDownloadingLogo();
	
    delete iLogoProvider;
    if (iIsdsClient)
    {
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    delete iTimer;    
}

int IRHistoryModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    return iHistoryList.count();
}

QString IRHistoryModel::getImageUrl(int aRow) const
{
    return iHistoryList.at(aRow)->imgUrl;
}

void IRHistoryModel::setLogo(HbIcon *aIcon, int aIndex)
{
    int elementCountNeedToAdd = aIndex + 1 - iLogos.size();
    while (elementCountNeedToAdd > 0)
    {
        iLogos.append(NULL);
        elementCountNeedToAdd--;
    }
    iLogos[aIndex] = aIcon;
    emit dataChanged(index(aIndex), index(aIndex));
}

QVariant IRHistoryModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
    {
        return QVariant();
    }

    if (aIndex.row() >= iHistoryList.count())
    {
        return QVariant();
    }
    
    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;
        
        int row = aIndex.row();
        QString primaryText = iHistoryList.at(row)->nickName;
        list.append(primaryText);
        // fix bug #9888,if left descriptions as blank, only one line appears
        QString tempDes = iHistoryList.at(row)->shortDesc;
        if (0 == tempDes.length())
        {
            tempDes = " ";
        }
        list.append(tempDes);
        
        return list;
    }
    else if (aRole == Qt::DecorationRole)
    {
        QVariantList list;
        int row = aIndex.row();
        if(row < iLogos.size())
        {
            const HbIcon *icon = iLogos[row];
            if (icon)
            {
                list.append(*icon);
            }
            else
            {
                list.append(*iStationLogo);
            }            
        }
        else
        {
            list.append(*iStationLogo);
        }       
        return list;
    }

    return QVariant();
}

IRQPreset* IRHistoryModel::getHistoryInfo(int aIndex)
{
    if (aIndex >= 0 && aIndex < iHistoryList.count())
    {
        return iHistoryList.at(aIndex);
    }

    return NULL;
}

void IRHistoryModel::clearAllHistory()
{
    if (iHistoryEngine->deleteChannelHistory())
    {
        clearModel();
        emit modelChanged();
    }
}

bool IRHistoryModel::checkHistoryUpdate()
{
    refreshModel();    
    return true;
}

void IRHistoryModel::clearAndDestroyLogos()
{
    int size = iLogos.size(); 
    for (int i = 0; i < size; i ++)
    {
        delete iLogos[i];
        iLogos[i] = NULL;
    }
    iLogos.clear();
}

void IRHistoryModel::refreshModel()
{
    clearModel();
    
    QList<QVariant*>* historySet = NULL;
    historySet = iHistoryEngine->getChannelHistory();
    
    if (NULL == historySet)
    {
        return;
    }
    int dataSize = historySet->size();
    for (int i = 0; i < dataSize; i++)
    {
        IRQPreset* preset = new IRQPreset();
        preset->presetId = (*(historySet->at(i) + channelId)).toUInt();  
        preset->name = (*(historySet->at(i) + channelName)).toString();
        preset->nickName = (*(historySet->at(i) + channelNickName)).toString();
        
        preset->genreName = (*(historySet->at(i) + genreName)).toString();
        preset->genreId = (*(historySet->at(i) + genreId)).toString();
        preset->languageName = (*(historySet->at(i) + languageName)).toString();
        preset->languageCode = (*(historySet->at(i) + languageCode)).toString(); 
        
        preset->countryName = (*(historySet->at(i) + countryName)).toString();
        preset->countryCode = (*(historySet->at(i) + countryCode)).toString();
        preset->description = (*(historySet->at(i) + description)).toString();
        preset->shortDesc = (*(historySet->at(i) + shortDesc)).toString();
        
        preset->lastModified = (*(historySet->at(i) + lastModified)).toString();
        preset->type = (*(historySet->at(i) + channelType)).toInt();
        preset->musicStoreStatus = (*(historySet->at(i) + musicStoreStatus)).toString(); 
        
        preset->imgUrl = (*(historySet->at(i) + imgUrl)).toString();
        
        preset->advertisementUrl = (*(historySet->at(i) + advertisementUrl)).toString();
        preset->advertisementInUse = (*(historySet->at(i) + advertisementInUse)).toString();
       
        columnMap selectCriteria;
        selectCriteria.insert(channelId, QString::number(preset->presetId));
        
        QList<QVariant*>* urlInfoList = iUrlInfoWrapper->getUrlInfo(&selectCriteria, NULL, i, dataSize-1);        
        if (NULL == urlInfoList)
        {
            delete preset;
            preset = NULL;
            continue;
        }
        int urlSize = urlInfoList->size();
        for (int j = 0; j < urlSize; j++)
        {
            IRQChannelServerURL urlInfo;
            urlInfo.serverName = preset->name;
            urlInfo.url = (*(urlInfoList->at(j) + channelUrl_URL)).toString();
            urlInfo.bitrate = (*(urlInfoList->at(j) + bitRate_URL)).toInt();
            preset->insertChannelServer(urlInfo);
        }        
        
        while(false == urlInfoList->isEmpty())
        {
            delete []urlInfoList->takeFirst();
        }
        urlInfoList->clear();
        
        delete urlInfoList;
        urlInfoList = NULL;
        
        iHistoryList.append(preset);
    }
    
    while(false == historySet->isEmpty())
    {
        delete []historySet->takeFirst();
    }
    historySet->clear();
    
    delete historySet;
    historySet = NULL;
    
    updateIconIndexArray();
    
    emit modelChanged();
}

void IRHistoryModel::clearModel()
{
    while (!iHistoryList.isEmpty())
    {
        IRQPreset *firstItem = iHistoryList.takeFirst();
        delete firstItem;
    }
	
    clearAndDestroyLogos();	
	updateIconIndexArray();    
}    

bool IRHistoryModel::deleteHistory(int aIndex)
{
    if (aIndex < 0 && aIndex >= iHistoryList.count())
    {
        return false;
    }
        
    if (!iIconIndexArray.empty())
    {
        iIsdsClient->isdsLogoDownCancelTransaction();
        iTimer->stop();
    }
            
    columnMap selectCriteria;
    selectCriteria.insert(channelId, QString::number(iHistoryList.at(aIndex)->presetId));
         
    if (!iHistoryEngine->deleteChannelHistory(&selectCriteria))
    {
        return false;                
    }
    
    beginRemoveRows(QModelIndex(), aIndex, aIndex);
    
    delete iHistoryList.at(aIndex);
    iHistoryList.removeAt(aIndex);
    
    if (aIndex < iLogos.size())
    {
        delete iLogos[aIndex];
        iLogos.removeAt(aIndex);
    }
	
    updateIconIndexArray();
    
    endRemoveRows(); 
    
    emit modelChanged();  
    
    if (!iIconIndexArray.empty())
    {
        iTimer->start();
    }
          
    return true;
}

void IRHistoryModel::startDownloadingLogo()
{
    iLogoProvider->activate(this, SLOT(logoData(const QByteArray&)));
    iTimer->start();
}

void IRHistoryModel::stopDownloadingLogo()
{
    iIsdsClient->isdsLogoDownCancelTransaction();
    iTimer->stop();
    iIconIndexArray.clear();
    iLogoProvider->deactivate();
}

void IRHistoryModel::downloadNextLogo()
{
    iTimer->stop();
    int leftCount = iIconIndexArray.count();

    if (0 != leftCount)
    {
         iLogoProvider->getLogo(iHistoryList.at(iIconIndexArray[0]));
    }
}

void IRHistoryModel::logoData(const QByteArray &aLogoData)
{
    if (aLogoData.size() > 0)
    {
        QPixmap tempMap;
        bool ret = tempMap.loadFromData((const unsigned char*)aLogoData.constData(), aLogoData.size());
        if( ret )
        {
            QIcon convertIcon(tempMap);
            HbIcon *hbIcon = new HbIcon(convertIcon);
            int index = iIconIndexArray[0];
            setLogo(hbIcon, index);  
        }
    }
    
    iIconIndexArray.removeAt(0);
    int leftCount = iIconIndexArray.count();
    if( leftCount > 0 )
    {
        iTimer->start();  
    }
}

bool IRHistoryModel::isLogoReady(int aIndex) const
{
    int logoListCount = iLogos.count();
    if (aIndex >= 0 
        && aIndex < logoListCount)
    {
        return iLogos[aIndex] != NULL;
    }
    else
    {
        return false;
    }
}

void IRHistoryModel::updateIconIndexArray()
{
    iIconIndexArray.clear();
    
    for (int i = 0; i < rowCount(); ++i)
    {
        if (getImageUrl(i) != "" && !isLogoReady(i))
        {
            iIconIndexArray.append(i);
        }
    } 
}

