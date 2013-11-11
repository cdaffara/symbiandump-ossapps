/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header of IRLogoDownloadEngine.cpp
*
*/


#ifndef IRLOGODOWNLOADENGINE_H
#define IRLOGODOWNLOADENGINE_H

#include <mhttpdatasupplier.h>
#include <mhttptransactioncallback.h>
#include <rhttptransaction.h>
#include <rhttpsession.h>
#include <rhttpheaders.h>
#include <http.h>
#include <es_sock.h>

#include "irisdspreset.h"
#include "mlogodownloadobserver.h"
#include "ircacheobserver.h"
#include "iractivenetworkobserver.h"


//Used to specify the maximum header-field value length
const TInt KMAXHEADERLENGTHS = 255;

//Forward Declarations
class RHTTPSession;
class RHTTPTransaction;
class CIRIsdsPreset;
class CIRNetworkController;
class MLogoDownloadObserver;
class CIRCacheMgmt; //for logo cache mgmt
class CIRHttpResponseData;
class MIRActiveNetworkObserver;//for network up and downs



/**
 * class CIRLogoDownlaodEngine
 */
class CIRLogoDownloadEngine : public CBase,
                              public MHTTPTransactionCallback,
                              public MHTTPDataSupplier,
                              public MIRCacheObserver,
                              public MIRActiveNetworkObserver
    {
public:
    
    /**
     * enum TState
     * status of the transaction
     */
   	enum TState
        {   
        EIdle,
        ESendingEvent
        };

    /**
     * CIRLogoDownloadEngine* NewL()
     * Creates instance of CIRLogoDownloadEngine.
     */
    IMPORT_C static CIRLogoDownloadEngine* NewL();


    /**
     * SendRequestL(CIRIsdsPreset* aPreset)
     * API is called from the SearchResultsView for to download logo
     */
    IMPORT_C void SendRequestL( CIRIsdsPreset* aPreset, TInt aXValue = 0,
                                TInt aYValue = 0 );

    /**
     * SendRequestL()
     * API is called from the NowPlayingView for to download logo 
     */
    IMPORT_C void SendRequestL( CIRIsdsPreset* aPreset,
                    MLogoDownloadObserver* aLogoHandle, TInt aNPVReq = 1,
                    TInt aXValue = 0, TInt aYValue = 0 );

    /**
     * IsLogoCachedL()
     * 
     */
    IMPORT_C TBool IsLogoCachedL( CIRIsdsPreset* aPreset, TInt aXValue = 0,
                    TInt aYValue = 0 );

    /**
     * CancelTransaction()
     * cancels the current transaction
     */
    IMPORT_C void CancelTransaction();

    /**
     * TBool IsRunning() const
     * To know the status of downloading logo
     */
    IMPORT_C TBool IsRunning() const;

    /**
     * ~CIRLogoDownloadEngine()
     * Destructs instance of CIRLogoDownloadEngine
     */
    IMPORT_C ~CIRLogoDownloadEngine();

    /**
     * void SendPresetWithLogoL()
     * sends the preset with logo to the requestors
     */
    IMPORT_C void SendPresetWithLogoL();

    /**
     * GetCacheLogoL(TDesC& aUrl,TInt& aStatus)
     * takes the url as a parameter and returns the logo data which is in cache
     * this API is called form the search results for to display logo on the view
     */
    IMPORT_C void GetCacheLogoL( const TDesC& aUrl, TInt& aStatus );

    /**
     * TDesC8& SendCacheLogo()
     * sends the logo which is cached
     */
    IMPORT_C TDesC8& SendCacheLogo();

    
    
    /**
     * void FetchLogoDataL(CIRIsdsPreset* aPreset)
     * API is used to download logo from isds/internet
     * called in the API SendRequestL(CIRIsdsPreset* aPreset)
     */
    void FetchLogoDataL( CIRIsdsPreset* aPreset );

    /**
     * void FetchLogoDataL()
     * API is used to download logo from isds/internet
     * called in the API SendRequestL()
     */
    void FetchLogoDataL( CIRIsdsPreset* aPreset,
                         MLogoDownloadObserver* aLogoHandle, TInt aNPVReq );

    /**
     * void ManagePresetArrayIndexL()
     * To update the iPresetArray current index
     */
    void ManagePresetArrayIndexL();

    /**
     * void ManageCachePresetArrayIndexL()
     * To update the iCachePresetArray current index
     */
    void ManageCachePresetArrayIndexL();

    /**
     * void CacheError()
     * Called from cachemgmt in case of an error
     * callback API from MIRCacheObserver
     */
    void CacheError();

    /**
     * void CacheFailed()
     * Called from cachemgmt in case of cache failure
     * callback API from MIRCacheObserver
     */
    void CacheFailed();

    /**
     * void CacheInvalid()
     * Called from cachemgmt in case cache is invalid
     * callback API from MIRCacheObserver
     */
    void CacheInvalid();

    /**
     * void CachedStructureL()
     * Called from cachemgmt data retrival is successful
     *@param aChoice,it gives the data type of the cache
     * callback API from MIRCacheObserver
     */
    void CachedStructureL( TInt aChoice );

    /**
     *  ReleaseResources();
     *  Releases the resources held by logo download engine
     */
    void ReleaseResources();
	
	//from MIRActiveNetworkObserver
	/**
	 * Notified by network controller when network is active
	 * to reissue the request  
	 * NotifyActiveNetworkObserversL()
	 */	
	 void NotifyActiveNetworkObserversL(TIRNetworkEvent aEvent);

    /**
     * Notifies all observers whose network request is active
     * to reset the pending request status  
     * ResetPendingRequests()
     */
    void ResetPendingRequests( TBool aValue );

private:
    
    /**
     * void ConstructL()
     * 2nd phase construction
     */
    void ConstructL();

    /**
     * CIRLogoDownloadEngine()
     * Standard C++ constructor
     */
    CIRLogoDownloadEngine();

    /**
     * void SetHeaderL(RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue) const
     * to set the headers of the http transaction
     */
    void SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField,
                     const TDesC8& aHdrValue ) const;

    /**
     * void IssueLogoDownloadRequest()
     * submits the transaction for to download the logo
     */
    void IssueLogoDownloadRequestL();

    /**
     * void CheckCacheForLogoL()
     * checks the logo in the cache
     */
    void CheckCacheForLogoL( TInt aValue );

    /**
     * void MHFRunL()
     * from MHTTPTransactionCallback
     */
    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent );

    /**
     * TInt MHFRunError()
     * //from MHTTPTransactionCallback
     */
    TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction,
                      const THTTPEvent& aEvent );

    /**
     * void ReleaseData()
     * from MHTTPDataSupplier
     */
    void ReleaseData();

    /**
     * TBool GetNextDataPart()
     * from MHTTPDataSupplier
     */
    TBool GetNextDataPart( TPtrC8& aDataPart );

    /**
     * TInt Reset()
     * from MHTTPDataSupplier
     */
    TInt Reset();

    /**
     * TInt OverallDataSize()
     * from MHTTPDataSupplier
     */
    TInt OverallDataSize();

    /**
     *  CIRLogoDownloadEngine::ExtractResponseHeadersL()
     *  Used to Extract the response headers.
     *  @param aTransaction HTTP Transaction object
     */
    void ExtractResponseHeadersL( const RHTTPTransaction& aTransaction );

    /**
     *  CIRLogoDownloadEngine::HttpHeaderReceived()
     *  indicate that an HTTP header is received.
     *  @param aHeaderData Reference to the Header Info
     */
    virtual void HttpHeaderReceived( const TDesC8 &aHeaderData );

    /**
     * CIRLogoDownloadEngine::ExtractHeaderValue()
     * Used to build the CIRHttpResponseData
     */
    void ExtractHeaderValue( const TDesC8& aHeaderData,
                    const TDesC8& aHeaderName, const TDesC8& aDelimeter,
                    TDes8& aHolder ) const;

    /**
     * CIRLogoDownloadEngine::HttpDateHeaderReceived()
     * Used to build the CIRHttpResponseData members
     */
    void HttpDateHeaderReceived( const TDesC8 &aHeader, const TTime& aTime );

private:
    
    /**
     * pointer to the logo observer object
     */
    MLogoDownloadObserver* iLogoHandle;

    /**
     * pointer to the logo observer object
     */
    MLogoDownloadObserver* iTempLogoHandle;

    /**
     * pointer to the logo observer object
     */
    MLogoDownloadObserver* iCacheTempLogoHandle;

    /**
     * pointer to the preset object
     * temporary preset in the logo downloading usecase
     */
    CIRIsdsPreset* iTempPreset;

    /**
     * pointer to the preset object
     * temporary preset in the cache checking usecase
     */
    CIRIsdsPreset* iCacheTempPreset;

    /**
     * pointer to the preset object
     * preset used to conver CIRPreset to CIRIsdsPreset
     */
    CIRIsdsPreset* iCopyPreset;

    /**
     * Network controller handle pointer
     */
    CIRNetworkController *iIRNetworkControllerHandle;

    /**
     * Preset Array for downloading logo
     */
    CArrayPtrFlat<CIRIsdsPreset>* iPresetArray;

    /**
     * Preset Array for checking cache
     */
    CArrayPtrFlat<CIRIsdsPreset>* iCachePresetArray;

    /**
     * MLogoDownloadObserver array for downloading logo
     */
    CArrayPtrFlat<MLogoDownloadObserver>* iObserverArray;

    /**
     * MLogoDownloadObserver array for checking cache
     */
    CArrayPtrFlat<MLogoDownloadObserver>* iCacheObserverArray;

    /**
     * To keep track whether the req came for Favorites or NPV(downloading logo)
     */
    CArrayFixFlat<TInt>* iReqFromArray;

    /**
     * To keep track whether the req came for Favorites or NPV(checking cache)
     */
    CArrayFixFlat<TInt>* iCacheReqFromArray;

    /**
     * Separate session for the logo downloading
     */
    RHTTPSession iLogoSession;

    /**
     * http transaction
     */
    RHTTPTransaction iTransaction;

    /**
     * for the transactions with cache mgmt
     */
    CIRCacheMgmt *iCache;

    /**
     * Pointer to Response header data
     */
    CIRHttpResponseData* iResponseHeaders;

    /**
     * boolean for to know the status of the downloading logo
     */
    TBool iRunning;

    /**
     * body of the transaction request
     */
    TDesC8* iReqBody;

    /**
     * body of the transaction response
     */
    HBufC8* iRespBody;

    /**
     * size of the response body
     */
    TInt iRespBodySize;

    /**
     * state of the transaction
     */
    TState iState;

    /**
     * status, from which the preset has modified
     */
    TBool iPresetModifiedStatus;

    /**
     * iPresetArray position
     */
    TInt iPresetArrayPos;

    /**
     * position of the iReqFromArray
     */
    TInt iReqFromArrayPos;

    /**
     * position of the iObserverArray
     */
    TInt iObserverArrayPos;

    /**
     * position of the iCachePresetArray
     */
    TInt iCachePresetArrayPos;

    /**
     * position of the iCacheReqFromArray
     */
    TInt iCacheReqFromArrayPos;

    /**
     * position of the iCacheObserverArray
     */
    TInt iCacheObserverArrayPos;

    /**
     * status of cache request
     */
    TInt iCacheReqFrom;

    /**
     * temporary variable for the status of request
     */
    TInt iTempReqFrom;

    /**
     * boolean to verify first time or not
     */
    TBool iFirstTime;

    /**
     * ETrue if logo is available in cache
     */
    TBool iLogoCached;

    /**
     * boolean for forcibly getting the logo from cache
     */
    TBool iForceGet;

    /**
     * boolean for to track whether the cache request has made or not
     */
    TBool iCacheReqMade;

    /**
     * response from the cache for the availability of logo(CacheNotValid,NotCached,CacheUseable)
     */
    TInt iResult;

    /**
     * status on cache checking
     */
    TBool iCheckingCache;

    /**
     * integer for the status of sending the logo
     */
    TInt iSendingLogo;

    /**
     * Using for temporarily till the isds provides urls
     */
    TInt iImageIndex;

    /**
     * this is used while making a conditional GET request
     */
    TTime iLastModifiedTime;

    /**
     * this is used while making a conditional GET request
     */
    TBuf8<KMAXHEADERLENGTHS> iETag;

    /**
     * this is used to track whether the request is conditional GET or non-conditional GET
     */
    TBool iIsConditionalGET;

    /**
     * Status of the logo download in the network up and down scenarios
     */
    TBool iLogoRequestPending;

    /**
     * status of the response code
     */
    TInt iRespCode;
    };

#endif // IRLOGODOWNLOADENGINE_H
