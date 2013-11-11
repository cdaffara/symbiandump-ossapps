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
#ifndef IRQABSTRACTISDSCLIENTIMPL_H_
#define IRQABSTRACTISDSCLIENTIMPL_H_

class IRQAbstractIsdsClientImpl
{

public:
	virtual ~IRQAbstractIsdsClientImpl(){};
      
    virtual void isdsSearchRequestImpl(const QString& aIsdsSearchString) = 0;
    
    virtual void isdsCategoryRequestImpl(
            IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache) = 0;

    virtual bool isdsIsCategoryCachedImpl(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType) = 0;
    
    virtual bool isdsIsChannelCachedImpl(int aIndex) = 0;
    
    virtual void isdsChannelRequestImpl(int aIndex, bool& aCache) = 0;

     
    virtual void isdsListenRequestImpl(int aCurrentIndex, bool aHistoryBool =
            false) = 0;

    virtual int isdsSyncPresetImpl(int aPresetId, const QString& aIfModifySince, IRQFavoritesDB *aFavPresets) = 0;   
     
    virtual void isdsCancelRequestImpl() = 0;    
    
    virtual bool isdsIsCategoryBannerImpl() = 0;
     
    virtual bool isdsIsChannelBannerImpl() = 0;
     
    virtual void isdsLogoDownSendRequestImpl(IRQPreset* aPreset, int aNPVReq = 1,
            int aXValue = 0, int aYValue = 0) = 0;
    
    virtual bool isdsIsLogoCachedImpl(IRQPreset* aPreset, int aXValue = 0, int aYValue = 0) = 0;
    
    virtual void isdsLogoDownCancelTransactionImpl() = 0;
     
    virtual bool isdsLogoDownIsRunningImpl() const = 0;
     
    virtual void isdsLogoDownCheckCacheLogoImpl(const QString& aURL, int& aStatus) = 0;
     
    virtual TDesC8& isdsLogoDownSendCacheLogoImpl() = 0;
    
    virtual void isdsPostLogImpl(const QString &aFileName) = 0;
    
    virtual void isdsGetIRIDImpl() = 0;

    virtual void isdsGetBrowseBannerImpl(QString& aBannerUrl, QString& aClickThroughUrl) = 0;
    
    virtual void isdsMultSearchImpl(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText) = 0;
        
    virtual bool isdsIsConstructSucceed() const = 0;

};

#endif
