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
#ifndef IRQISDSCLIENTIMPL_H_
#define IRQISDSCLIENTIMPL_H_

#include "irqisdsclient.h"
#include "irqabstractisdsclientimpl.h"
#include "misdsresponseobserver.h"
#include "mlogodownloadobserver.h"

class CIRIsdsClient;
class IRQFavoritesDB;
class CIRLogoDownloadEngine;
class IRQIsdsClientImpl : public QObject,
	public IRQAbstractIsdsClientImpl,
	public MIsdsResponseObserver,
	public MLogoDownloadObserver
{
Q_OBJECT

public:
    
    IRQIsdsClientImpl(IRQIsdsClient *aParent);
    
    ~IRQIsdsClientImpl();
    
    void isdsSearchRequestImpl(const QString& aIsdsSearchString);
    
     
    void isdsCategoryRequestImpl(
            IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache);

    bool isdsIsCategoryCachedImpl(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType);
    
    bool isdsIsChannelCachedImpl(int aIndex);
    
    void isdsChannelRequestImpl(int aIndex, bool& aCache);

     
    void isdsListenRequestImpl(int aCurrentIndex, bool aHistoryBool =
            false);

     
    int isdsSyncPresetImpl(int aPresetId, const QString& aIfModifySince, IRQFavoritesDB *aFavPresets);   
     
    void isdsCancelRequestImpl();    
    
    bool isdsIsCategoryBannerImpl();
     
    bool isdsIsChannelBannerImpl();
     
    void isdsLogoDownSendRequestImpl(IRQPreset* aPreset, int aNPVReq = 1,
            int aXValue = 0, int aYValue = 0);
    
    bool isdsIsLogoCachedImpl(IRQPreset* aPreset, int aXValue = 0, int aYValue = 0);
    
    void isdsLogoDownCancelTransactionImpl();
     
    bool isdsLogoDownIsRunningImpl() const;
     
    void isdsLogoDownCheckCacheLogoImpl(const QString& aURL, int& aStatus);
     
    TDesC8& isdsLogoDownSendCacheLogoImpl();
    
    void isdsPostLogImpl(const QString &aFileName);
    
    void isdsGetIRIDImpl();

    void isdsGetBrowseBannerImpl(QString& aBannerUrl, QString& aClickThroughUrl);
    
    void isdsMultSearchImpl(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText);
        
    bool isdsIsConstructSucceed() const;
    
private:

    /**
     *IRQIsdsClientImpl::IsdsErrorL()
     *used to indicate errors in retrieving data from isds server
     *@param int, the error code sent by the http receiver 
     **/
    void IsdsErrorL(int aErrCode);
    /**
     *IRQIsdsClientImpl::IsdsCatogoryDataReceivedL()
     *when we get the category data from low layer, the function is called and
     *we will generate the data pushed to UI
     **/
    void IsdsCatogoryDataReceivedL(
            CArrayPtrFlat<CIRBrowseCatagoryItems> & aParsedStructure);
    /**
     *IRQIsdsClientImpl::IsdsChannelDataReceivedL()
     *when we get the channels data from low layer, the function is called and
     *we will generate the data pushed to UI
     **/
    void IsdsChannelDataReceivedL(
            CArrayPtrFlat<CIRBrowseChannelItems> & aParsedStructure);
    /**
     *IRQIsdsClientImpl::IsdsPresetDataReceivedL()
     *when we get the presets data from low layer, the function is called and
     *we will generate the data pushed to UI
     **/
    void IsdsPresetDataReceivedL(
            CArrayPtrFlat<CIRIsdsPreset> & aParsedStructure);
    /**
     *IRQIsdsClientImpl::IsdsPresetRemovedL()
     *when we find that the preset to be syc is deleted from isds server, the function 
     *will be called to notify the UI.
     **/
    void IsdsPresetRemovedL(TInt aId);
    /**
     *IRQIsdsClientImpl::IsdsPresetChangedL()
     *when we find that the preset to be syc is changed from isds server, the function 
     *will be called to notify the UI.
     **/
    void IsdsPresetChangedL(CIRIsdsPreset& aPreset);
    /**
     *IRQIsdsClientImpl::IsdsPresetNoChangeL()
     *when we find that the preset to be syc is not changed from isds server, the function 
     *will be called to notify the UI.
     **/
    void IsdsPresetNoChangeL();
    /**
     * IRQIsdsClientImpl::PresetLogoDownloadedL()
     * called back when a preset's logo has downloaded
     * @param CIRIsdsPreset*, preset with downloaded logo
     */
    void PresetLogoDownloadedL(CIRIsdsPreset* aPreset);

    /**
     * IRQIsdsClientImpl::PresetLogoDownloadError()
     * called back when a preset's logo has not downloaded
     * @param CIRIsdsPreset*, preset with no logo data
     */
    void PresetLogoDownloadError(CIRIsdsPreset* aPreset);   
    
    
    void IsdsOtaInfoRecieved( CIROTAUpdate &aOtaData);
    
    /*
     * IRQIsdsClientImpl::IsdsIRIDRecieved()
     * call back from MIsdsResponseObserver
     */
    void IsdsIRIDRecieved(const TDesC& aIRID);    
	
private:   
 
    /**
     * iISDSClient
     * ISDS INTERFACE
     */    
    CIRIsdsClient *iISDSClient;    
    
    /*
     * iFavPresets
     * Instance of FavoritesDb
     */
    IRQFavoritesDB *iFavPresets;

    /**
     * iChannelBannerUrl
     * banner url in statons view
     */
    QString iChannelBannerUrl;
    
    /**
     * iCatBannerUrl
     * banner url in category view
     */
    QString iCatBannerUrl;
    
    /**
     * iChannelClickThroughUrl
     * click through url in stations view
     */
    QString iChannelClickThroughUrl;
    
    /**
     * iCatClickThroughUrl
     * click through url in category view
     */
    QString iCatClickThroughUrl;
    
    /**
     * iCatBannerTag
     * bool value to show is there any category banner
     */
    bool iCatBannerTag;
    
    /**
     * iChannelBannerTag
     * bool value to show is there any channel banner
     */
    bool iChannelBannerTag;

    
    /**
     *Pointet to Logo download
     */
    CIRLogoDownloadEngine* iLogoDownloadEngine;
	
    /**
     *Pointet to father
     */
	IRQIsdsClient *q_ptr;

};

#endif

