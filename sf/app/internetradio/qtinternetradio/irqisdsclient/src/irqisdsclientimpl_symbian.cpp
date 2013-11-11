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
#include "irqenums.h"
#include "irqfavoritesdb.h"
#include "irlogodownloadengine.h"
#include "irdataprovider.h"
#include "irhttpdataprovider.h"
#include "irqutility.h"
#include "irbrowsecatagoryitems.h"
#include "irbrowsechannelitems.h"
#include "isdsclientdll.h"
#include "irqisdsdatastructure.h"
#include "irqsettings.h"
#include "irqlogger.h"

IRQIsdsClientImpl::IRQIsdsClientImpl(IRQIsdsClient *aParent) : iISDSClient(NULL), iFavPresets(NULL),
                                                                    iLogoDownloadEngine(NULL), q_ptr(aParent)
{
    IRQSettings *irSettings = IRQSettings::openInstance();    
    QString isdsUrl = irSettings->getIsdsUrl();
    irSettings->closeInstance();
    
    if (isdsUrl.endsWith('/')) // remove the last '/' at the end of the url
    {
        isdsUrl.chop(1);
    }
    TPtrC irqisdsbaseurl(reinterpret_cast<const TUint16*>(isdsUrl.utf16()));

    TRAPD(error, iISDSClient = CIRIsdsClient::NewL(*this, irqisdsbaseurl));
    if(KErrNone != error)
    {
        return;
    }
    
    iLogoDownloadEngine
            = iISDSClient->GetDataProvider()->GetHttpDataProvider()->GetLogoDownloadEngine();
}

IRQIsdsClientImpl::~IRQIsdsClientImpl()
{
    delete iISDSClient;
}

//used to indicate errors in retrieving data from isds server
//@param int, the error code sent by the http receiver 
//
void IRQIsdsClientImpl::IsdsErrorL(int aErrCode)
{
    LOG_FORMAT( "This is a QString %d", aErrCode);
    if (KNotFound == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorNotFound);
    }
    else if (KErrCouldNotConnect == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorCouldNotConnect);
    }
    else if (KDndTimedOut == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorTimeOut);
    }
    else if (KServiceUnavailable == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorServiceUnavailable);
    }
    else if (KErrCorrupt == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorCorrupt);
    }
    else if (KDataProviderTimeout == aErrCode)
    {
        emit q_ptr->operationException(EIRQErrorTimeOut);
    }
    else
        emit q_ptr->operationException(EIRQErrorGeneral);
}

void IRQIsdsClientImpl::isdsSearchRequestImpl(const QString& aIsdsSearchString)
{

    TPtrC16 searchDes(
            reinterpret_cast<const TUint16*> (aIsdsSearchString.utf16()));
   
    TRAPD( err, iISDSClient->IRISDSSearchL(searchDes));
    if (KErrNone != err)
    {
        emit q_ptr->operationException(EIRQErrorGeneral);
    }
}

 

//Send the category request by the category type
// 
void IRQIsdsClientImpl::isdsCategoryRequestImpl(
        IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType, bool& aCache)
{
    iCatBannerTag = false;
    TRAPD( err, aCache = !(iISDSClient->IRIsdsClientIntefaceL((CIRIsdsClient::TIRIsdsclientInterfaceIDs)aIDType)));
    if (err != KErrNone)
    {
        emit q_ptr->operationException(EIRQErrorGeneral);
    }
}

bool IRQIsdsClientImpl::isdsIsCategoryCachedImpl(IRQIsdsClient::IRQIsdsClientInterfaceIDs aIDType)
{
    bool cache = false;
    TRAP_IGNORE(cache = iISDSClient->IRIsdsIsCategoryCachedL((CIRIsdsClient::TIRIsdsclientInterfaceIDs)aIDType));    
    return cache;
}

bool IRQIsdsClientImpl::isdsIsChannelCachedImpl(int aIndex)
{
    bool cache = false;
    TRAP_IGNORE(cache = iISDSClient->IRIsdsIsChannelCachedL(aIndex)); 
    return cache;
}

//Send the channels request by the channel index in the specify category
//
void IRQIsdsClientImpl::isdsChannelRequestImpl(int aIndex, bool& aCache)
{
    iChannelBannerTag = false;
    TRAPD( err, aCache = !(iISDSClient->IRIsdsClientIntefaceL(aIndex, CIRIsdsClient::ECatagory)));
    if (err != KErrNone)
    {
        emit q_ptr->operationException(EIRQErrorGeneral);
    }
}

//issue a listen request to the isds client
//@param int,bool, the current index of channel, the history tag  
//
void IRQIsdsClientImpl::isdsListenRequestImpl(int aCurrentIndex,
        bool aHistoryBool)
{
    if (aHistoryBool)
    {
        TRAPD( err, iISDSClient->IRIsdsClientIntefaceL(aCurrentIndex, CIRIsdsClient::EChannels, ETrue));
        if (err != KErrNone)
        {
            emit q_ptr->operationException(EIRQErrorGeneral);
        }
    }
    else
    {
         
        TRAPD( err, iISDSClient->IRIsdsClientIntefaceL(aCurrentIndex, CIRIsdsClient::EChannels));
        if (err != KErrNone)
        {
            emit q_ptr->operationException(EIRQErrorGeneral);
        }
    }
}

//to syncronize presets
//@param int,QString, the preset id and the last modified tag for the preset   
//
int IRQIsdsClientImpl::isdsSyncPresetImpl(int aPresetId,
        const QString& aIfModifySince, IRQFavoritesDB *aFavPresets)
{
    iFavPresets = aFavPresets;
    TPtrC16 modifySinceDes(
            reinterpret_cast<const TUint16*> (aIfModifySince.utf16()));
    TInt result = 0;
    TRAP_IGNORE(result = iISDSClient->SyncPresetL(aPresetId,modifySinceDes));     
    return result;
}

//Cacel the request sent by the UI.
//@param None
//
void IRQIsdsClientImpl::isdsCancelRequestImpl()
{
    TRAPD( err, iISDSClient->IRISDSCancelRequest());
    if (err != KErrNone)
    {
        emit q_ptr->operationException(EIRQErrorGeneral);
    }
}

//to see wether category view has a banner.
//@param None
//
bool IRQIsdsClientImpl::isdsIsCategoryBannerImpl()
{
    return iCatBannerTag;
}

//
//to see wether channel view has a banner.
//@param None
bool IRQIsdsClientImpl::isdsIsChannelBannerImpl()
{
    return iChannelBannerTag;
}

 
//the api is called from the UI(nowplaying view) to download logo.
//@param None
//
void IRQIsdsClientImpl::isdsLogoDownSendRequestImpl(IRQPreset* aPreset,
        int aNPVReq, int aXValue, int aYValue)
{
    if (NULL == aPreset)
        return;

    CIRIsdsPreset *cirPreset = NULL;
    TRAP_IGNORE(cirPreset = CIRIsdsPreset::NewL());
    IRQUtility::convertIRQPreset2CIRIsdsPreset(*aPreset, *cirPreset);
    
    aXValue = aXValue % 1000;
    aYValue = aYValue % 1000;
    
    TRAPD( err, iLogoDownloadEngine->SendRequestL(cirPreset,this, aNPVReq, aXValue, aYValue));//0 0 
    delete cirPreset;
    if (err != KErrNone)
    {
        emit q_ptr->operationException(EIRQErrorGeneral);
    }
}

bool IRQIsdsClientImpl::isdsIsLogoCachedImpl(IRQPreset* aPreset, int aXValue, int aYValue)
{
    if( NULL == aPreset )
        return false;
    
    bool cached = false;
    CIRIsdsPreset *cirPreset = NULL;
    TRAP_IGNORE(cirPreset = CIRIsdsPreset::NewL());
    IRQUtility::convertIRQPreset2CIRIsdsPreset(*aPreset, *cirPreset);
    aXValue = aXValue % 1000;
    aYValue = aYValue % 1000;
    
    TRAP_IGNORE(cached = iLogoDownloadEngine->IsLogoCachedL(cirPreset,aXValue, aYValue));
	  delete cirPreset;
	  cirPreset = NULL;
	
    return cached;
}

//
//the api is called to cancel the current transaction
//@param None
//
void IRQIsdsClientImpl::isdsLogoDownCancelTransactionImpl()
{
    iLogoDownloadEngine->CancelTransaction();
}

//
//To know the status of downloading logo
bool IRQIsdsClientImpl::isdsLogoDownIsRunningImpl() const
{
    return iLogoDownloadEngine->IsRunning();
}

//takes the url as a parameter and returns the logo data which is in cache
//this API is called form the search results for to display logo on the view
//@param QString: the url of the img, int: the status for getting
//
void IRQIsdsClientImpl::isdsLogoDownCheckCacheLogoImpl(
        const QString& aURL, int& aStatus)
{
    TInt status = 0;
    TPtrC16 url(reinterpret_cast<const TUint16*> (aURL.utf16()));
    TRAP_IGNORE(iLogoDownloadEngine->GetCacheLogoL(url, status));       
    aStatus = status;
    
}

//get the cache logo from the logodown engine. The "send" is the point from a logodown engine
//@param None
//
TDesC8& IRQIsdsClientImpl::isdsLogoDownSendCacheLogoImpl()
{
    return iLogoDownloadEngine->SendCacheLogo();
}

void IRQIsdsClientImpl::isdsPostLogImpl(const QString &aFileName)
{
    TPtrC16 fileName(
                reinterpret_cast<const TUint16*> (aFileName.utf16()));
    TBuf<256> fileBuf = fileName;
    TRAP_IGNORE(iISDSClient->IRISDSPostL(fileBuf));     
}

void IRQIsdsClientImpl::isdsGetIRIDImpl()
{
    TRAP_IGNORE(iISDSClient->IRGetIRIDL());
}

void IRQIsdsClientImpl::isdsGetBrowseBannerImpl(QString& aBannerUrl, QString& aClickThroughUrl)
{
    aBannerUrl = iCatBannerUrl;
    aClickThroughUrl = iCatClickThroughUrl;
}

void IRQIsdsClientImpl::isdsMultSearchImpl(QString aGenreID, QString aCountryID, QString aLanguageID, QString aSearchText)
{
    TPtrC16 genreID(reinterpret_cast<const TUint16*> (aGenreID.utf16()));
    TPtrC16 countryID(reinterpret_cast<const TUint16*> (aCountryID.utf16()));
    TPtrC16 languageID(reinterpret_cast<const TUint16*> (aLanguageID.utf16()));
    TPtrC16 searchText(reinterpret_cast<const TUint16*> (aSearchText.utf16()));
    TRAP_IGNORE(iISDSClient->IRISDSMultiSearchL(genreID, countryID, languageID, searchText));
}
//when we get the category data from low layer, the function is called and
//we will generate the data pushed to UI, the IRQIsdsClientImpl will not free the 
//memory.
//
void IRQIsdsClientImpl::IsdsCatogoryDataReceivedL(CArrayPtrFlat<
        CIRBrowseCatagoryItems> & aParsedStructure)
{
    iCatBannerTag = false;
    iCatBannerUrl.clear();
    iCatClickThroughUrl.clear();

    /* the data is pushed to the UI and irqisds is not care when it's deleted */
    QList<IRQBrowseCategoryItem *> * pushBrowseCategoryItemList = new QList<
            IRQBrowseCategoryItem *> ;
    for (TInt i = 0; i < aParsedStructure.Count(); i++)
    {
        if (NULL != aParsedStructure[i]->iCatBannerUrl)
        {
            iCatBannerTag = true;
            if (0 != aParsedStructure[i]->iCatBannerUrl->Length())
            {
                iCatBannerUrl = QString::fromUtf16(
                        aParsedStructure[i]->iCatBannerUrl->Des().Ptr(),
                        aParsedStructure[i]->iCatBannerUrl->Des().Length());
            }

            if (NULL != aParsedStructure[i]->iCatClickThroughUrl)
            {
                if (0 != aParsedStructure[i]->iCatClickThroughUrl->Length())
                {
                    iCatClickThroughUrl
                            = QString::fromUtf16(
                                    aParsedStructure[i]->iCatClickThroughUrl->Des().Ptr(),
                                    aParsedStructure[i]->iCatClickThroughUrl->Des().Length());
                }
            }
        }// end if ( NULL != ) 

        if (NULL != aParsedStructure[i]->iCatName)
        {
            IRQBrowseCategoryItem * oneItem = new IRQBrowseCategoryItem();
            oneItem->catName = QString::fromUtf16(
                    aParsedStructure[i]->iCatName->Des().Ptr(),
                    aParsedStructure[i]->iCatName->Des().Length());
            oneItem->size = aParsedStructure[i]->iSize;
            pushBrowseCategoryItemList->append(oneItem);
        }
    }// end for

    /* now we get the data and we need to signal the ui to stop the 
     dialog and emit and call the setdata of model*/
    emit
    q_ptr->categoryItemsChanged(pushBrowseCategoryItemList);

}

//when we get the channels data from low layer, the function is called and
//we will generate the data pushed to UI
//
void IRQIsdsClientImpl::IsdsChannelDataReceivedL(CArrayPtrFlat<
        CIRBrowseChannelItems> & aParsedStructure)
{
    iChannelBannerTag = false;
    iChannelBannerUrl.clear();
    iChannelClickThroughUrl.clear();

    QList<IRQChannelItem *> *pushBrowseChannelItemList = new QList<
            IRQChannelItem *> ;

    for (int i = 0; i < aParsedStructure.Count(); i++)
    {
        if (NULL != aParsedStructure[i]->iBannerUrl)
        {
            iChannelBannerTag = true;

            if (0 != aParsedStructure[i]->iBannerUrl->Length())
            {
                iChannelBannerUrl = QString::fromUtf16(
                        aParsedStructure[i]->iBannerUrl->Des().Ptr(),
                        aParsedStructure[i]->iBannerUrl->Des().Length());
            }

            if (NULL != aParsedStructure[i]->iClickThroughUrl)
            {
                if (0 != aParsedStructure[i]->iClickThroughUrl->Length())
                {
                    iChannelClickThroughUrl
                            = QString::fromUtf16(
                                    aParsedStructure[i]->iClickThroughUrl->Des().Ptr(),
                                    aParsedStructure[i]->iClickThroughUrl->Des().Length());
                }
            }
        } //end if aParsedStructure[i]->

        if (NULL != aParsedStructure[i]->iChannelName)
        {
            IRQChannelItem * oneChannelItem = new IRQChannelItem();
            oneChannelItem->channelName = QString::fromUtf16(
                    aParsedStructure[i]->iChannelName->Des().Ptr(),
                    aParsedStructure[i]->iChannelName->Des().Length());
            oneChannelItem->shortDescription = QString::fromUtf16(
                    aParsedStructure[i]->iShortDescription->Des().Ptr(),
                    aParsedStructure[i]->iShortDescription->Des().Length());
            //added for search result's cache
            oneChannelItem->channelID = aParsedStructure[i]->iChannelID;

            if (0 != aParsedStructure[i]->iImgUrl.Length())
            {

                oneChannelItem->imageURL = QString::fromUtf16(
                        aParsedStructure[i]->iImgUrl.Ptr(),
                        aParsedStructure[i]->iImgUrl.Length());;
            }
            else
                oneChannelItem->imageURL = "";

            pushBrowseChannelItemList->append(oneChannelItem);
        }
    }

    /* after we get data, we push it to the UI to show*/
    emit
    q_ptr->channelItemsChanged(pushBrowseChannelItemList);
    
}

//when we get the presets data from low layer, the function is called and
//we will generate the data pushed to UI. 
//
void IRQIsdsClientImpl::IsdsPresetDataReceivedL(
        CArrayPtrFlat<CIRIsdsPreset> & aParsedStructure)
{
    CIRIsdsPreset* preset = aParsedStructure[0];
    IRQPreset* qPreset = new IRQPreset();
    /* we need convert the CIR to QT */
    IRQUtility::convertCIRIsdsPreset2IRQPrest(*preset, *qPreset);
    qPreset->type = IRQPreset::EIsds;
    emit
    q_ptr->presetResponse(qPreset);
    
}

//when we find that the preset to be syc is deleted from isds server, the function 
//will be called to notify the UI. DISCUSSED FURTHER
//
void IRQIsdsClientImpl::IsdsPresetRemovedL(TInt aId)
{
    if(iFavPresets)
    {
        iFavPresets->makePresetUserDefined(aId, 0);
    }
    emit
    q_ptr->syncPresetResult(EIRQIsdsSycPresetRemoved, NULL);
}

//when we find that the preset to be syc is changed from isds server, the function 
//will be called to notify the UI.
//
void IRQIsdsClientImpl::IsdsPresetChangedL(CIRIsdsPreset& aPreset)
{
    IRQPreset* qPreset = new IRQPreset();
    IRQUtility::convertCIRIsdsPreset2IRQPrest(aPreset, *qPreset);
    qPreset->type = IRQPreset::EIsds;
    if(iFavPresets)
    {        
        iFavPresets->replacePreset(*qPreset);
    }
    emit q_ptr->syncPresetResult(EIRQIsdsSycPresetChanged, qPreset);
}

//when we find that the preset to be syc is changed from isds server, the function 
//will be called to notify the UI.
//
void IRQIsdsClientImpl::IsdsPresetNoChangeL()
{
    emit q_ptr->syncPresetResult(EIRQIsdsSycPresetNoChange, NULL);

}

//called back when a preset's logo has downloaded
//@param CIRIsdsPreset*, preset with downloaded logo
//
void IRQIsdsClientImpl::PresetLogoDownloadedL(CIRIsdsPreset* aPreset)
{
    if (NULL == aPreset)
        return;

    IRQPreset * irqPreset = new IRQPreset();
    IRQUtility::convertCIRIsdsPreset2IRQPrest(*aPreset, *irqPreset);

    emit q_ptr->presetLogoDownloaded(irqPreset);
}

//called back when a preset's logo has not downloaded
//@param CIRIsdsPreset*, preset with no logo data
//
void IRQIsdsClientImpl::PresetLogoDownloadError(CIRIsdsPreset* aPreset)
{
    if (NULL == aPreset)
        return;

    emit q_ptr->presetLogoDownloadError();
}

//receive the irid from isds server, not implementated
//
void IRQIsdsClientImpl::IsdsIRIDRecieved(const TDesC& aIRID)
{
    //nothing now
    QString irid = QString::fromUtf16(aIRID.Ptr(),aIRID.Length());
    emit q_ptr->iridReceived(irid);     
}

 


//
//receive the ota info from isds server, not implementated
//
void IRQIsdsClientImpl::IsdsOtaInfoRecieved(CIROTAUpdate &aOtaData)
{
    //nothing now
    Q_UNUSED(aOtaData);    
}

bool IRQIsdsClientImpl::isdsIsConstructSucceed() const
{
    return iISDSClient != NULL;
}
