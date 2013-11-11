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

#include "irchanneldataprovider.h"
#include "irqisdsdatastructure.h"
#include "irqisdsclient.h"
#include "irsearchresultdb.h"
#include "iruidefines.h"

const int KDownloadLogoInterval = 10; //msec

IRChannelDataProvider::IRChannelDataProvider(QObject *aParent) : HgDataProviderModel(aParent),
                       iChannelList(NULL), iLogoPreset(NULL), iActivated(false)
{
    iDefaultLogo = HbIcon("qtg_large_internet_radio");
    
    iLogoDownloadTimer = new QTimer(this);
    iLogoDownloadTimer->setInterval(KDownloadLogoInterval);
    connect(iLogoDownloadTimer, SIGNAL(timeout()), this, SLOT(downloadNextLogo()));
    
    iIsdsClient = IRQIsdsClient::openInstance();
}

IRChannelDataProvider::~IRChannelDataProvider()
{
    if (iIsdsClient)
    {
        iIsdsClient->closeInstance();
        iIsdsClient = NULL;
    }
    
    delete iLogoPreset;
    iLogoPreset = NULL;
    
    clearAndDestroyItems();
    clearAndDestroyLogos();
}

void IRChannelDataProvider::activate()
{
    iActivated = true;
    connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
            this, SLOT(presetLogoDownload(IRQPreset* )));
    connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
            this, SLOT(presetLogoDownloadError()));
    
    if (iRequestedItems.count() > 0)
    {
        iLogoDownloadTimer->start();
    }
}

void IRChannelDataProvider::deactivate()
{
    iActivated = false;
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
               this, SLOT(presetLogoDownload(IRQPreset* )));
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
               this, SLOT(presetLogoDownloadError()));
    
    iRequestedItems.clear();
    iIsdsClient->isdsLogoDownCancelTransaction();
    iLogoDownloadTimer->stop();
}

IRQChannelItem* IRChannelDataProvider::getChannelItemByIndex(int aIndex)
{
    if( aIndex < 0 || aIndex >= iChannelList->count() )
    {
        return NULL;
    }
    
    return iChannelList->at(aIndex);
}

void IRChannelDataProvider::doReleaseData(QList<int> aList, bool aSilent)
{
    Q_UNUSED(aSilent);
    iIsdsClient->isdsLogoDownCancelTransaction();
    iLogoDownloadTimer->stop();
    
    int count = aList.count();
    int index = 0;
    for (int idx = 0; idx < count; ++idx)
    {
        index = aList[idx];
        releasePixmap(index);
        //if the index is saved in iRequestedItems, remove it
        iRequestedItems.removeOne(index);
    }
}

void IRChannelDataProvider::doRequestData(QList<int> aList, bool aSilent)
{
    Q_UNUSED(aSilent);
    int count = aList.count();
    int index = 0;
    for (int idx = 0; idx < count; ++idx)
    {
        index = aList[idx];
        if (index < iChannelList->count())
        {
            if (iChannelList->at(index)->imageURL != "")
            {
                if (NULL == iLogos[index])
                {
                    //logo is not downloaded, add to requested items
                    iRequestedItems.append(index);
                }
                else
                {
                    //logo is already downloaded, update icon
                    updateIcon(index, *iLogos[index]);
                }
            }
        }
    }
    
    if (iActivated && iRequestedItems.count() > 0)
    {
        iLogoDownloadTimer->start();
    }
}

QVariant IRChannelDataProvider::defaultIcon() const
{
    return iDefaultLogo;
}

void IRChannelDataProvider::doResetModel()
{
    int count = iChannelList->count();
    for (int i = 0; i < count; ++i)
    {
        QList< QPair< QVariant, int > > item;
        QVariantList list;
        list.append(iChannelList->at(i)->channelName);
        QString tempDes = iChannelList->at(i)->shortDescription;        
        if( 0 == tempDes.length() )
        {
            tempDes = " ";
        }
        list.append(tempDes); 
        item.append( QPair<QVariant, int>(list, Qt::DisplayRole));
        
        newItem(&item);
    }
}

//data received from ISDS server, add items
void IRChannelDataProvider::updateData(QList<IRQChannelItem*> *aPushItemsList)
{
    if (NULL == aPushItemsList)
    {
        return;
    }
    
    if (iChannelList != aPushItemsList)
    {
        clearAndDestroyItems();
        clearAndDestroyLogos();
        
        iChannelList = aPushItemsList;
        iRequestedItems.clear();
        clearCache();
        resetModel();
        emit dataAvailable();
    }
}

void IRChannelDataProvider::presetLogoDownload(IRQPreset* aPreset)
{
    if( NULL == aPreset )
    {
        presetLogoDownloadError();
        return;
    } 
    
    delete iLogoPreset;            
    iLogoPreset = aPreset;    

    if (iLogoPreset->logoData.size() > 0)
    {         
        QPixmap tempMap;     
        bool ret = tempMap.loadFromData((const unsigned char*)iLogoPreset->logoData.constData(), iLogoPreset->logoData.size());
       
        if( ret )
        {            
            QIcon convertIcon(tempMap);
            HbIcon *hbIcon = new HbIcon(convertIcon);
            int index = iRequestedItems[0];
            iLogos[index] = hbIcon;
            updateIcon(index, *hbIcon);
            iRequestedItems.removeAt(0); 
            if ( iRequestedItems.count() > 0 )
            {
                iLogoDownloadTimer->start();  
            }
            return;
        }     
    }    
    
    presetLogoDownloadError(); 
}

void IRChannelDataProvider::presetLogoDownloadError()
{
    // if the logo download fails, try to download the next
    iRequestedItems.removeAt(0);
    if ( iRequestedItems.count() > 0 )
    {
        iLogoDownloadTimer->start();
    }   
}

void IRChannelDataProvider::downloadNextLogo()
{
    iLogoDownloadTimer->stop();
    
    if (iRequestedItems.count() > 0)
    {
        startDownloading(iRequestedItems[0]);   
    }
}

void IRChannelDataProvider::startDownloading(int aIndex)
{
    QString url = iChannelList->at(aIndex)->imageURL;
    IRQPreset tempPreset;
    tempPreset.imgUrl = url;
    tempPreset.type = IRQPreset::EIsds;
    
    iIsdsClient->isdsLogoDownSendRequest(&tempPreset, 0, KBitmapSize, KBitmapSize); 
}

void IRChannelDataProvider::clearAndDestroyItems()
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

void IRChannelDataProvider::clearAndDestroyLogos()
{
    for (QMap<int, HbIcon*>::iterator it = iLogos.begin(); it != iLogos.end(); ++it)
    {
        delete it.value();
    }
    
    iLogos.clear();
}
