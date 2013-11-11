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

#include "irqisdsclientimpl_symbian.h"   

QMutex IRQIsdsClient::mMutex;
int IRQIsdsClient::mRef = 0;
IRQIsdsClient *IRQIsdsClient::mInstance = NULL;

//Static function
//to get an instance of the IRQIsdsClient
//@return IRQIsdsClient *
IRQIsdsClient *IRQIsdsClient::openInstance()
{
    mMutex.lock();
    if (NULL == mInstance)
    {
        mInstance = new IRQIsdsClient();
    }
	if (NULL != mInstance)
    {
        mInstance->mRef++;
	}

    mMutex.unlock();
    return mInstance;
}

//
//close the instance.
//@return void
void IRQIsdsClient::closeInstance()
{
    mMutex.lock();
    mRef--;

    if (0 == mRef)
    {
        mInstance = NULL;
        delete this;
    }

    mMutex.unlock();
    return;
}

//Issue a search request to the isds server
//@param QString, the requested search string
//
void IRQIsdsClient::isdsSearchRequest(const QString& aIsdsSearchString)
{
    d_ptr->isdsSearchRequestImpl(aIsdsSearchString);     
}

//Send the category request by the category type
// 
void IRQIsdsClient::isdsCategoryRequest(
        IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache)
{ 
    d_ptr->isdsCategoryRequestImpl(aIDType, aCache);
}

bool IRQIsdsClient::isdsIsCategoryCached(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType)
{
    return d_ptr->isdsIsCategoryCachedImpl(aIDType);
}

bool IRQIsdsClient::isdsIsChannelCached(int aIndex)
{
    return d_ptr->isdsIsChannelCachedImpl(aIndex);
}
//Send the channels request by the channel index in the specify category
//
void IRQIsdsClient::isdsChannelRequest(int aIndex, bool& aCache)
{
    
    if( 0 > aIndex )
        return;
    
    /* if the isds has the banner, the index can't be 0 , or it will crash*/
    if( isdsIsCategoryBanner() && 0 == aIndex )
        return;
    
    d_ptr->isdsChannelRequestImpl(aIndex, aCache);
}

//issue a listen request to the isds client
//@param int,bool, the current index of channel, the history tag  
//
void IRQIsdsClient::isdsListenRequest(int aCurrentIndex,
        bool aHistoryBool)
{ 
    if (0 > aCurrentIndex)
        return;

    /* if the isds has the banner, the index can't be 0 , or it will crash*/
    if (isdsIsChannelBanner() && 0 == aCurrentIndex)
        return;
    
    d_ptr->isdsListenRequestImpl(aCurrentIndex,aHistoryBool); 
}

//to syncronize presets
//@param int,QString, the preset id and the last modified tag for the preset   
//
int IRQIsdsClient::isdsSyncPreset(int aPresetId,
        const QString& aIfModifySince, IRQFavoritesDB *aFavPresets)
{     
    TInt result = 0;
    result = d_ptr->isdsSyncPresetImpl(aPresetId, aIfModifySince, aFavPresets);
    return result;
} 

//Cacel the request sent by the UI.
//@param None
//
void IRQIsdsClient::isdsCancelRequest()
{
    d_ptr->isdsCancelRequestImpl();  
}

 

//to see wether category view has a banner.
//@param None
//
bool IRQIsdsClient::isdsIsCategoryBanner()
{
    return d_ptr->isdsIsCategoryBannerImpl();
}

//
//to see wether channel view has a banner.
//@param None
bool IRQIsdsClient::isdsIsChannelBanner()
{
    return d_ptr->isdsIsChannelBannerImpl();
}

//the api is called from the UI(nowplaying view) to download logo.
//@param None
//
void IRQIsdsClient::isdsLogoDownSendRequest(IRQPreset* aPreset,
        int aNPVReq, int aXValue, int aYValue)
{ 
    d_ptr->isdsLogoDownSendRequestImpl(aPreset, aNPVReq, aXValue, aYValue);
}

bool IRQIsdsClient::isdsIsLogoCached(IRQPreset* aPreset, int aXValue, int aYValue)
{
    return d_ptr->isdsIsLogoCachedImpl(aPreset, aXValue, aYValue);
}
//
//the api is called to cancel the current transaction
//@param None
//
void IRQIsdsClient::isdsLogoDownCancelTransaction()
{
    d_ptr->isdsLogoDownCancelTransactionImpl();
}

//
//To know the status of downloading logo
bool IRQIsdsClient::isdsLogoDownIsRunning() const
{
    return d_ptr->isdsLogoDownIsRunningImpl();
}

//takes the url as a parameter and returns the logo data which is in cache
//this API is called form the search results for to display logo on the view
//@param QString: the url of the img, int: the status for getting
//
void IRQIsdsClient::isdsLogoDownCheckCacheLogo(const QString& aURL,
        int& aStatus)
{ 
    d_ptr->isdsLogoDownCheckCacheLogoImpl(aURL, aStatus);
}

//get the cache logo from the logodown engine. The "send" is the point from a logodown engine
//@param None
//
TDesC8& IRQIsdsClient::isdsLogoDownSendCacheLogo()
{
    return d_ptr->isdsLogoDownSendCacheLogoImpl();
}

void IRQIsdsClient::isdsPostLog(const QString& aFileName)
{
    d_ptr->isdsPostLogImpl(aFileName);
}

void IRQIsdsClient::isdsGetIRID()
{
    d_ptr->isdsGetIRIDImpl();
}

void IRQIsdsClient::isdsGetBrowseBanner(QString& aBannerUrl, QString& aClickThroughUrl)
{
    d_ptr->isdsGetBrowseBannerImpl(aBannerUrl, aClickThroughUrl);
}

void IRQIsdsClient::isdsMultSearch(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText)
{
    d_ptr->isdsMultSearchImpl(aGenreID, aCountryID, aLanguageID, aSearchText);
}

bool IRQIsdsClient::isdsIsConstructSucceed() const
{
    return d_ptr->isdsIsConstructSucceed();
}

/************************ private functions **************************/
//the c++ default destruction function
// private
IRQIsdsClient::~IRQIsdsClient()
{    
    delete d_ptr;
}


IRQIsdsClient::IRQIsdsClient()
{    
    d_ptr = new IRQIsdsClientImpl(this); 
    Q_ASSERT(d_ptr);
} 
 
