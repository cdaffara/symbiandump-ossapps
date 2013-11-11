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
* Description:  The implementation for presentation elements.
*
*/


#ifndef ISDSCLIENTDLL_H
#define ISDSCLIENTDLL_H

#include <e32base.h>

#include "ircacheobserver.h"
#include "irdataproviderobserver.h"
#include "irlanguagemapper.h"
#include "irparserobserver.h"

class CIRCacheMgmt;
class CIRDataProvider;
class CIRHttpRequestData;
class CIRIsdsPreset;
class CIRReportGenerator;
class CIRSettings;
class CIRXMLContentHandler;
class MIsdsResponseObserver;

/** 
 * This class is the one which acts as a interface between UI and network
 * component which interacts with the isds server.It provides the data to 
 * be diplayed to the UI either from cache or fetches it from the ISDS 
 * server.
 * 
 *
 * @code
 * creates the instance of isdsclient 
 * iISDSClient = CIRIsdsClient::NewL(*this , iIRSettings->GetISDSBaseUrlL());
 * iISDSClient->IRIsdsClientIntefaceL(EGenre); //requests for genre data
 * //requests for selected channel
 * iISDSClient->IRIsdsClientIntefaceL(iIndex, CIRIsdsClient::EChannels);
 * //cancels any previous requests
 * iISDSClient->IRISDSCancelRequest();
 * //gets irid
 * iISDSClient->IRGetIRIDL();
 * iISDSClient->GetOtaStatusL(forceUpdate);//gets ota update status
 * iISDSClient->IRISDSSearchL(aIsdsSearchString);
 * iISDSClient->IRISDSPostL(pathtopost);
 * SyncPresetL(iUi->iFavPresets->iFavPresetList->At(aUrlIndex)->GetId(),
 * iUi->iFavPresets->iFavPresetList->At(aUrlIndex)->GetLastModifiedTime());
 * iISDSClient->ReleaseResources();
 * @endcode
 *
 */

NONSHARABLE_CLASS( CIRIsdsClient ) : public CBase ,
                                     public MIRDataProviderObserver,
                                     public MIRParserObserver,
                                     public MIRCacheObserver
    {

public: //public data

    /**
     * These Enums correspond to a particular UI event which 
     * requires request to be made to isds.
    */
    enum TIRIsdsclientInterfaceIDs
        {
        EGenre,
        ELanguages,
        ECountries,
        Ehotpicks
        };
    
    enum TIRIsdsClientInterfaces
        {
        ECatagory,
        EChannels,
        EPresets,
        EOtaInfo
        }; 
    
    /**
    *type of data recieved from ISDS   
    */
     enum TDataRecieved
        {
        ECategoryInfoRecieved,
        EChannelListRecieved,
        EPresetDataRecieved,
        EOtaInfoRecieved,
        EIridRecieved,
        ESessionLogRecieved
        };

public:

    /**
     * CIRIsdsClient::NewL(MIsdsResponseObserver& aobserver)
     * Used by UI to create instance of CIRIsdsClient.
     */
    IMPORT_C static CIRIsdsClient* NewL(MIsdsResponseObserver& aobserver,
            const TDesC& aISDSBaseUrl);

    /**
     * CIRIsdsClient::NewLC(MIsdsResponseObserver& aobserver)
     * Creates instance of CIRIsdsClient.
     */
    
    static CIRIsdsClient* NewLC(MIsdsResponseObserver& aobserver,
            const TDesC& aISDSBaseUrl);

    /**
     * CIRIsdsClient::~CIRIsdsClient()
     * Destructor
     */
    
    ~CIRIsdsClient();

    /**
     * CIRIsdsClient::ConstructL()
     * Symbian two phased constructor
     */
    void ConstructL(const TDesC& aISDSBaseUrl);

    /**
     * CIRIsdsClient::CIRIsdsClient(MIsdsResponseObserver& aobserver)
     * Default constructor
     */
    CIRIsdsClient(MIsdsResponseObserver& aobserver); 

    /**
     * void CIRIsdsClientInteface(TIRIsdsclientInterfaceIDs aISDSClientRequest)
     * Called by UI to make request to ISDS for catogories
     * @param :ENUM value depending on catogory which user wants to explore
     */
    IMPORT_C TBool IRIsdsClientIntefaceL(
            TIRIsdsclientInterfaceIDs aISDSClientRequest);

    /**
     * TBool IRIsdsIsCategoryCachedL(
     *       TIRIsdsclientInterfaceIDs aISDSClientRequest);
     * 
     */
    IMPORT_C TBool IRIsdsIsCategoryCachedL(
            TIRIsdsclientInterfaceIDs aISDSClientRequest);
    
    /**
     * void CIRIsdsClientInteface(TUint aSelectedOption,TIRIsdsClientInterfaces aChoice)
     * This API is used by the UI to make request for isds listed channels
     * @param : aSelectedOption-returns the index of the option selected by the user
     * @param : aChoice-ENUM value specifies what data is expected preset or channel data
     */
    IMPORT_C TBool IRIsdsClientIntefaceL(TUint aSelectedOption,
            TIRIsdsClientInterfaces aChoice, TInt aHistoryBool = EFalse);
    
    /**
     * TBool IRIsdsIsChannelCachedL(TUint aSelectedOption);
     * 
     */
    IMPORT_C TBool IRIsdsIsChannelCachedL(TUint aSelectedOption);

    /**
     * void CIRISDSCancelRequest()
     * This API is used by the UI to cancel a request made.
     */
    IMPORT_C void IRISDSCancelRequest();

    /**
     * void CIRGetIRID()
     * This API is used to get IR ID from isds server 
     * isds if it doesnt have one
     */
    IMPORT_C void IRGetIRIDL();

    /**
     * void GetOtaStatusL()
     * This API is used to get the OTA status from the server
     * @param TBool,if ETrue the call is from UI and a call back has to be made to ui
     * TBool contains the source of update (cache/isds)upon return.Used by Ui to load the
     * loading screen based on this value.
     */
    IMPORT_C void GetOtaStatusL(TBool& aDataFrom);

    /**
     * void CIRISDSSearch(TDesC& aSearchString)
     * This API is used by the UI to search channels from isds
     * @param : aSearchString-search string entered by user
     */
    IMPORT_C void IRISDSSearchL(const TDesC& aSearchString);

    /**
     * void IRISDSPostL(TFileName &aFileName)
     *function used to post the sesson logs
     *@param sesson log file name
     */
    IMPORT_C void IRISDSPostL(TFileName &aFileName);
    
    /**
     * CIRDataProvider* GetDataProvider();
     *function pointer to CIRDataProvider
     *@param None
     */
    IMPORT_C CIRDataProvider* GetDataProvider();

    /*
     * void IRISDSMultiSearch()
     * function used to search the stations by multi conditions
     * @param 
     */
    IMPORT_C void IRISDSMultiSearchL(const TDesC& aGenreID,
            const TDesC& aCountryID, const TDesC& aLanguageID,
            const TDesC& aSearchText);

    /**
     * void RHttpGeneralError(TInt aErrorCode)
     * It is a call back function called by the data provider 
     * to return error code in case of some error.
     *@param error code
     */
    void IRHttpGeneralError(TInt aErrCode);

    /**
     * void IRHttpDataReceived(TDesC& aXmlPath)
     * It is a call back function called by the data provider 
     * once it recieves the XML data
     */
    void IRHttpDataReceived(const TDesC& aXmlPath,
            const CIRHttpResponseData& aResponseHeaders);

    /**
     * void IRHttpContentNotChanged()
     * this is call back funtion called by Dataprovider in case 
     * HTTP content isnt changed by ISDS 
     */
    void IRHttpResponseCodeReceived(TInt aResponseCode,
            CIRHttpResponseData& aResponseHeaders);

    /**
     * void ParseError()
     * Call back funtion called by XML parser in case of parsing error
     */
    void ParseError(TInt aErrorCode);

    /**
     * void ParsedStructureL(TInt aChoice)
     * Call back funtion called by XML parser after parsing is over
     * @param : aChoice -specifies the type of data recieved from ISDS
     */
    void ParsedStructureL(TInt aChoice);

    /**
     * void CacheError()
     * Called from cachemgmt in case of an error
     */
    void CacheError();

    /**
     * void CacheError()
     * Called from cachemgmt in case of cache failure
     */
    void CacheFailed();

    /**
     * void CacheError()
     * Called from cachemgmt in case cache is invalid
     */
    void CacheInvalid();
    
    /**
     * void CacheError()
     * Called from cachemgmt data retrival is successful
     *@param aChoice,it gives the data type of the cache
     */
    void CachedStructureL(TInt aChoice);

    /**
     *void CIRIsdsClient::SyncPresetL()
     *Used to syncronize presets
     *@param TInt,TDesC, the preset id and the last modified tag for the preset          
     */
    IMPORT_C TInt SyncPresetL(TInt aPresetId, const TDesC&aIfModifiedSince);

    /**
     *void CIRIsdsClient::ConvertToTimeDate()
     *Used to convert the date-time stamp to Symbian DateTime format
     *@param TDesC&,TDateTime& the time stamp and the DateTime ref to 
     *hold the new data
     */
    void ConvertToTimeDate(const TDesC& aTime, TDateTime& aDateTime) const;

    /**
     *ReleaseResources()
     *relese the resources held by isds client
     */
    IMPORT_C void ReleaseResources();

    /**
     *CIRIsdsClient::GetMultiplePresetsL()
     *for multiple selected presets to be saved 
     *@param RArray&,TBool,the array of preset ids to be saved,return value
     */
    IMPORT_C void GetMultiplePresetsL(const RArray<TInt>& aIndexArray,
            TBool& aDataFrom);

    /**
     *CIRIsdsClient::PurgeOtaInfoL
     *delete all the data cached wrt OTA
     *reset the flags
     *function called when the ota info available earlier is no longer valid
     */
    IMPORT_C void PurgeOtaInfoL();

private:
    /**
     *void IRHttpIssueRequestL(CIRHttpRequestData &);
     *for making http request to Data Provider with Request object
     *@param aRequestObject :The request object which has URL and other details
     */
    void IRHttpIssueRequestL(CIRHttpRequestData &aRequestObject);

    /**
     * void EncodeUrlL( TDes &aQuery )
     * Encode the url using EEscapeUrlEncoded mode, This is used in session log
     * @param url to be encoded
     */
    void EncodeUrlL(TDes& aQuery);

private:
    //Session log
    TBuf<256> iBrowseUrl;
    TInt iChannId;

    //XML parser instance
    CIRXMLContentHandler* iXmlReader;

    //ISDS observer reference
    MIsdsResponseObserver& iIsdsResponseObserver;

    //DataProvider instance
    CIRDataProvider *iDataProvider;

    //cache mgmt
    CIRCacheMgmt *iCache;
    CIRSettings *iSettings;
    CArrayPtrFlat<CIRIsdsPreset>* iSendPreset;
    // used to store the isds base URL
    HBufC* iISDSBaseUrl;
    //contains catogory base URL
    TBuf<256> iCatgoryURL;
    //contains search base URL
    TBuf<256> iSearchURL;
    //contains irid base URL
    TBuf<256> iGetIridUrl;
    TBuf<128> iGetOtaStatus;
    TBool iSyncRequest;//1 if the preset data request is a sync request,0 if general request
    TInt iPresetToBeSynced;//keeps the presetid for which the sync request has been sent
    TBool iDataFrom;//data from isds =1,isds =0
    TBool iCatDataFrom;//0 if isds,1 for cache
    TBool iChannelDataFrom;//0 if isds,1 for cache
    TBuf<256> iCachePath;
    TBool iOtaReqFrom;//0 internal 1from UI
    //session log
    CIRReportGenerator* iBrowseReport;
    //conditionalGet
    TBool iConditonalGet;
    TTime iLastModifiedTime;
    TInt iRequestType;
    TBool iForceGet;
    TBool iOnceForTheUsageSession;
    TBool iPostResponsePending;
    TBufC8<KIRLanguageCodeLength> iShortLanguageCode;

    /**
     * boolean for to track whether the cache request has made or not
     */
    TBool iCacheReqMade;
    TBool iIRIDPending;
    TBool iReqPending;
    TBool iReqFromGetIRIDFunc;
    TBool iReqFromSync;

    /**
     * to store further pending request objects 
     */
    CIRHttpRequestData* iTempRequestObject;
    };

#endif //ISDSCLIENTDLL_H


