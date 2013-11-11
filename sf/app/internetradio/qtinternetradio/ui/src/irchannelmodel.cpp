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

#include "irchannelmodel.h"
#include "irqisdsdatastructure.h"
#include "irsearchresultdb.h"
#include "irqisdsclient.h"
#include "irlogoprovider.h"

IrChannelModel::IrChannelModel(QObject *aParent): QAbstractListModel(aParent)
                                                , iChannelList(NULL),iDB(NULL)
{
    iIsdsClient = IRQIsdsClient::openInstance();
    iLogoProvider = new IRLogoProvider(iIsdsClient);
    
    iTimer = new QTimer;
    iTimer->setInterval(10);
    connect(iTimer, SIGNAL(timeout()), this, SLOT(downloadNextLogo()));
    
    iStationLogo = new HbIcon("qtg_large_internet_radio");

    if( NULL == iDB )
    {
        iDB = new IRSearchResultDB();
    }
}

IrChannelModel::~IrChannelModel()
{
    clearAndDestroyItems();

    delete iStationLogo;
    iStationLogo = NULL;
    
    clearAndDestroyLogos();

    stopDownloadingLogo();

    if( iDB )
    {
        delete iDB;
        iDB = NULL;
    }

    delete iLogoProvider;
    if (iIsdsClient)
    {
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    delete iTimer;
}

int IrChannelModel::rowCount(const QModelIndex &aParent) const
{
    Q_UNUSED(aParent);
    int count = 0;
    
    if(iChannelList)
    {
        count = iChannelList->count();
    }
    return count;
}

void IrChannelModel::setLogo(HbIcon *aIcon, int aIndex)
{
    iLogos[aIndex] = aIcon;
    emit dataChanged(index(aIndex), index(aIndex));
}

QVariant IrChannelModel::data(const QModelIndex &aIndex, int aRole) const
{
    if (!aIndex.isValid())
        return QVariant();

    if (aIndex.row() >= iChannelList->count())
        return QVariant();

    if (aRole == Qt::DisplayRole)
    {
        QVariantList list;
        int row = aIndex.row();

        list.append(iChannelList->at(row)->channelName);       
        // fix bug #9888,if left descriptions as blank, only one line appears
        QString tempDes = iChannelList->at(row)->shortDescription;        
        if( 0 == tempDes.length() )
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
        const HbIcon *icon = iLogos.value(row);
        if (icon)
        {
            list.append(*icon);
        }
        else
        {
            list.append(*iStationLogo);
        }
        
        return list;
    }
    else
    {
        return QVariant();
    }
}

void IrChannelModel::updateData(QList<IRQChannelItem*> *aPushItemsList, bool bInit)
{
    if (iChannelList != aPushItemsList)
    {
        clearAndDestroyItems();
        iChannelList = aPushItemsList;
    }
    if(false == bInit)
    {
        save2Cache();
    
    }
    
    clearAndDestroyLogos();

    updateIconIndexArray();

    emit dataAvailable();
}

void IrChannelModel::initWithCache()
{
    updateData(iDB->getCahcedChannelList(), true);
}

void IrChannelModel::save2Cache()
{
    if( NULL == iChannelList )
    {
        return;
    }
    
    //ignore the return value   
    iDB->cacheChannelList(iChannelList);
}

IRQChannelItem * IrChannelModel::getChannelItemByIndex(int aIndex)
{
    if( aIndex < 0 || aIndex >= iChannelList->count() )
    {
        return NULL;
    }
    
    return iChannelList->at(aIndex);
}

void IrChannelModel::cleanupDatabase()
{
    clearAndDestroyItems();
    clearAndDestroyLogos();
    iDB->clearCache();
    iIconIndexArray.clear();

    emit dataAvailable();
}

void IrChannelModel::clearAndDestroyLogos()
{
    for (QMap<int, HbIcon*>::iterator it = iLogos.begin(); it != iLogos.end(); ++it)
    {
        delete it.value();
    }
    
    iLogos.clear();
}

void IrChannelModel::clearAndDestroyItems()
{
    if (iChannelList)
    {
        for (QList<IRQChannelItem*>::iterator it = iChannelList->begin(); it != iChannelList->end(); ++it)
        {
            delete *it;
        }
        delete iChannelList;
        iChannelList = NULL;
    }
}

void IrChannelModel::startDownloadingLogo()
{
    iLogoProvider->activate(this, SLOT(logoData(const QByteArray&)));
    iTimer->start();
}

void IrChannelModel::stopDownloadingLogo()
{
    iIsdsClient->isdsLogoDownCancelTransaction();
    iTimer->stop();
    iLogoProvider->deactivate();
}

void IrChannelModel::downloadNextLogo()
{
    iTimer->stop();
    int leftCount = iIconIndexArray.count();

    if (0 != leftCount)
    {
         int row = iIconIndexArray[0];
         IRQPreset preset;
         preset.name = iChannelList->at(row)->channelName;
         preset.shortDesc = iChannelList->at(row)->shortDescription;
         preset.type = IRQPreset::EIsds;
         preset.presetId = iChannelList->at(row)->channelID;
		 preset.imgUrl = iChannelList->at(row)->imageURL;
         iLogoProvider->getLogo(&preset);
    }
}

void IrChannelModel::logoData(const QByteArray &aLogoData)
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

void IrChannelModel::updateIconIndexArray()
{
    iIconIndexArray.clear();
    
    for (int i = 0; i < rowCount(); ++i)
    {
        if (iChannelList->at(i)->imageURL != "" && !iLogos.contains(i))
        {
            iIconIndexArray.append(i);
        }
    }

    if (iIconIndexArray.size() > 0)
    {
        startDownloadingLogo();
    }
}
