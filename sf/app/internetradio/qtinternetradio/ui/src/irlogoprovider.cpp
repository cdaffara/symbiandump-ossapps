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

#include "irlogoprovider.h"
#include "imgwrapper.h"
#include "irqisdsclient.h"
#include "irqisdsdatastructure.h"
#include "iruidefines.h"


IRLogoProvider::IRLogoProvider(IRQIsdsClient *aIsdsClient) : iIsdsClient(aIsdsClient)
{
    
}

IRLogoProvider::~IRLogoProvider()
{
}

void IRLogoProvider::getLogo(IRQPreset *aPreset)
{
    imgWrapper wrapper;
    columnMap map;
    
    map.insert(channelId, QString::number(aPreset->presetId));
    map.insert(imgUrl, aPreset->imgUrl);
    QList<QVariant*> *dataSet = wrapper.getImg(&map);
    if (dataSet)
    {
        //logo data has been saved into database
        if (dataSet->count() > 0)
        {
            QVariant *row = dataSet->at(0);
            QByteArray logo = row[sLogoGet].toByteArray();
            if (logo.size() > 0)
            {
                emit logoData(logo);
            }
            else
            {
                //logo data hasn't been saved into database, call isds client to get it
                iIsdsClient->isdsLogoDownSendRequest(aPreset, 0, KBitmapSize, KBitmapSize); 
            }
        }
        else
        {
            //logo data hasn't been saved into database, call isds client to get it
            iIsdsClient->isdsLogoDownSendRequest(aPreset, 0, KBitmapSize, KBitmapSize); 
        }
        
        while (!dataSet->isEmpty())
        {
            delete []dataSet->takeFirst();
        }
        delete dataSet;
    }
    else
    {
        //logo data hasn't been saved into database, call isds client to get it
        iIsdsClient->isdsLogoDownSendRequest(aPreset, 0, KBitmapSize, KBitmapSize); 
    }
}

void IRLogoProvider::activate(const QObject *aReceiver, const char *aFunc)
{
    connect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset* )),
            this, SLOT(presetLogoDownload(IRQPreset* )));
    connect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
            this, SLOT(presetLogoDownloadError()));
    connect(this, SIGNAL(logoData(const QByteArray&)), aReceiver, aFunc);
}

void IRLogoProvider::deactivate()
{
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloaded(IRQPreset*)),
               this, SLOT(presetLogoDownload(IRQPreset* )));
    disconnect(iIsdsClient, SIGNAL(presetLogoDownloadError()),
               this, SLOT(presetLogoDownloadError()));
    disconnect(SIGNAL(logoData(const QByteArray&)));
}

void IRLogoProvider::presetLogoDownload(IRQPreset* aPreset)
{
    if (NULL == aPreset)
    {
        presetLogoDownloadError();
        return;
    }
    
    //logo data is available, save it to database
    imgWrapper wrapper;
    columnMap rowData;
    rowData.insert(imgUrl, aPreset->imgUrl);
    
    logoMap logo;
    logo.insert(bLogo, QByteArray());
    logo.insert(sLogo, aPreset->logoData);
    bool ret = wrapper.updateImg(&rowData, aPreset->presetId, &logo);
    
    emit logoData(aPreset->logoData);
    delete aPreset;
}

void IRLogoProvider::presetLogoDownloadError()
{
    QByteArray data;
    emit logoData(data);
}
