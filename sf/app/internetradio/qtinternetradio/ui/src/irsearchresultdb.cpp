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

#include "searchrltwrapper.h"
#include "irsearchresultdb.h"
#include "irqisdsdatastructure.h"

const int KTruncateSize = 255;

IRSearchResultDB::IRSearchResultDB() : iSearchRltWrapper(NULL)
{
    iSearchRltWrapper = new searchRltWrapper();
}

IRSearchResultDB::~IRSearchResultDB()
{
    delete iSearchRltWrapper;
}

 
IRQError IRSearchResultDB::cacheChannelList(QList<IRQChannelItem*> *aChannelList)
{
    IRQError ret = EIRQErrorNone;
    
    if( NULL == aChannelList )
    {
        return EIRQErrorBadParameter;
    }
    
    if( iSearchRltWrapper )
    {
        clearCache();

        //create column struct of table;
        columnMap* pColMap = new columnMap();
        QString num, name, imageURL, shortDescription;
        int chnlID;
        bool initFlag = true;
        for(int i=0; i<aChannelList->count(); i++)
        {
            IRQChannelItem* insertItem = aChannelList->at(i);
            name = insertItem->channelName;
            chnlID = insertItem->channelID;
            num.setNum(chnlID);
            imageURL = insertItem->imageURL;
            shortDescription = insertItem->shortDescription;
            
            //if some data overflows, we just skip it.note that
            //the VARCHAR is word-based, so here we use the real size 
            name.truncate(KTruncateSize);
            imageURL.truncate(KTruncateSize);
            shortDescription.truncate(KTruncateSize);

            pColMap->clear();
            pColMap->insert(channelId, num);
            pColMap->insert(channelName, name);
            pColMap->insert(imgUrl, imageURL);
            pColMap->insert(shortDesc, shortDescription);

            iSearchRltWrapper->addSearchRlt(pColMap, initFlag);
            initFlag = false;
            
        }
        
        iSearchRltWrapper->addSearchRltFinished();
        delete pColMap;
    }
    else
    {
        ret = EIRQErrorGeneral;
    }
    
    return ret;
}

QList<IRQChannelItem*> *IRSearchResultDB::getCahcedChannelList()
{
    if( !iSearchRltWrapper )
    {
        return NULL;
    }

    QList<QVariant*>* searchRlt = NULL;
    QList<IRQChannelItem*> *channelList = new QList<IRQChannelItem*>();
    searchRlt = iSearchRltWrapper->getSearchRlt(NULL, NULL);

    if (NULL == searchRlt)
    {
        return channelList;
    }

    for(int i = 0; i < searchRlt->size(); i++)
    {
        IRQChannelItem *oneItem = new IRQChannelItem();
        oneItem->channelName = (*( searchRlt->at(i) + channelName) ).toString();
        oneItem->channelID = (*( searchRlt->at(i) + channelId)).toUInt();
        oneItem->imageURL = (*( searchRlt->at(i) + imgUrl) ).toString();
        oneItem->shortDescription = (*( searchRlt->at(i) + shortDesc) ).toString();
        channelList->append(oneItem);
    }

    while(false == searchRlt->isEmpty())
    {
        delete []searchRlt->takeFirst();
    }
    searchRlt->clear();
    delete searchRlt;
    searchRlt = NULL;

    return channelList;    
}
  
IRQError IRSearchResultDB::clearCache()
{
    IRQError ret = EIRQErrorNone;
    if( !iSearchRltWrapper)
    {
        ret = EIRQErrorServiceUnavailable;
    }
    else
    { 
        bool retval = iSearchRltWrapper->deleteSearchRlt(NULL, NULL);
        if (!retval)
        {
            ret = EIRQErrorServiceUnavailable;
        }
    }
    
    return ret;
}
