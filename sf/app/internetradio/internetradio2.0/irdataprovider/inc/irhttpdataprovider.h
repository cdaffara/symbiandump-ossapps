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
* Description:  ?Description
*
*/


#ifndef IRHTTPDATAPROVIDER_H
#define IRHTTPDATAPROVIDER_H

#include <mhttptransactioncallback.h>
#include <rhttpsession.h>

#include "irdataproviderconstants.h"
#include "irfavoritesdb.h"

class CIRHttpPost;
class CIRHttpRequestData;
class CIRNetworkController;
class CIRLogoDownloadEngine;
class CIRFavoritesDb;
class MIRHttpDataProviderObserver;

/**
 * This class provides the implementation of the HTTP client
 */

NONSHARABLE_CLASS( CIRHttpDataProvider ) : public CBase,
                                           public MHTTPTransactionCallback
    {
public:
    
    /**
     * CIRHttpDataProvider::CIRHttpDataProvider()
     * Creates instance of CIRHttpDataProvider.
     * @param aObserver Reference to HTTP Data provider observer
     */
    CIRHttpDataProvider( MIRHttpDataProviderObserver &aObserver );
    
    /**
     * CIRHttpDataProvider::~CIRHttpDataProvider()
     * Destructs instance of CIRHttpDataProvider.
     */
    virtual ~CIRHttpDataProvider();
    
    /**
     * CIRHttpDataProvider::NewL()
     * Creates instance of CIRHttpDataProvider.
     * @param aObserver Reference to HTTP Data provider observer
     */
    static CIRHttpDataProvider *NewL( MIRHttpDataProviderObserver &aObserver );
    
    /**
     *  CIRHttpDataProvider::NewLC()
     *  Creates instance of CIRHttpDataProvider.
     *  @param aObserver Reference to HTTP Data provider observer
     */
    static CIRHttpDataProvider *NewLC( MIRHttpDataProviderObserver &aObserver );
    
    /**
     * CIRHttpDataProvider::ConstructL()
     * 2nd phase construction
     */
    void ConstructL();
    
    /**
     * CIRHttpDataProvider::CancelTransactionL()
     * Cancel the issued Http transaction
     */
    void CancelTransaction();
    
    /**
     * CIRHttpDataProvider::GetHttpSession()
     * returns the iHttpSession
     */
    RHTTPSession GetHttpSession();

    /**
     *  CIRHttpDataProvider::IssueHttpRequestL()
     *  Used to issue the request.
     *  @param aRequestInfo Reference to request object
     */
    TInt IssueHttpRequestL( CIRHttpRequestData &aRequestObject );
    
    /**
     *  CIRHttpDataProvider::SetHttpContentType()
     *  Set the Http content type
     *  @param aContentType Specifies the Content-Type
     *  @param aCharSet Specifies the Charecter set
     */
    void SetHttpContentType( const TDesC &aContentType, const TDesC &aCharSet );

    /**
     *  CIRHttpDataProvider::InitializeHttpDataProviderL();
     *  Initialize the data provider component
     */
    void InitializeHttpDataProviderL();

    /**
     *  CIRHttpDataProvider::ReleaseResources();
     *  Releases the resources held by data provider
     */
    void ReleaseResources();

    IMPORT_C CIRLogoDownloadEngine* GetLogoDownloadEngine();

    private:
    /**
     * CIRHttpDataProvider::MHFRunL()
     * Inherited from MHTTPTransactionCallback
     * Called by framework to pass transaction events.
     * @param aTransaction HTTP Transaction object
     * @param aEvent HTTP Transaction Event
     */
    void MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent &aEvent );
    
    /**
     * CIRHttpDataProvider::MHFRunError()
     * Inherited from MHTTPTransactionCallback
     * Called by framework to pass transaction errors
     * @param aError Error code of the HTTP Transactin
     * @return TInt Return code KErrNone if success
     */
    TInt MHFRunError( TInt aError, RHTTPTransaction aTransaction,
                      const THTTPEvent &aEvent );
    
    /**
     *  CIRHttpDataProvider::ExtractResponseHeadersL()
     *  Used to Extract the response headers.
     *  @param aTransaction HTTP Transaction object
     */
    void ExtractResponseHeadersL( const RHTTPTransaction& aTransaction );
    
    /**
     *  CIRHttpDataProvider::SetHeaderL()
     *  Used to set the Http header
     *  @param aHeaders HTTP Headers object
     *  @param aHeaderField Header field
     *  @param aHeaderValue Header value
     */
    void SetHeaderL( RHTTPHeaders aHeaders, TInt aHeaderField,
                     const TDesC8 &aHeaderValue ) const;

    /**
     * CIRHttpDataProvider::BuildHeadersL()
     * Build the headers for the request
     * @param CIRHttpRequestData&
     */
    void BuildHeadersL( const CIRHttpRequestData &aRequestInfo );

    /**
     * CIRHttpDataProvider::ValidateStatusCode()
     * Validate the status code returned
     * @param aStatusCode Status code
     */
    void ValidateStatusCode( TInt aStatusCode );

    /**
     *  CIRHttpDataProvider::CloseLogFile ();
     *  Used to Close the log file LogUsage.gz externally 
     *  from other sources (bug-fixing)
     */
    void CloseLogFile();

public:

    /**
     * iSetNonUAProfUserAgent Set when the UAProf should not be used
     * may be used for image data provider
     */
    TBool iSetNonUAProfUserAgent;

private:

    /**
     * Status code of the response
     */
    TInt iStatusCode;

    /**
     * Indicates if the transaction is active ie. there is a pending
     * request.
     */
    TBool iRunning;

    /**
     * Used to identify first request to the data provider object
     */
    TBool iFirstTime;

    /**
     * HTTP session object
     */
    RHTTPSession iHttpSession;

    /**
     * HTTP transaction object
     */
    RHTTPTransaction iHttpTransaction;

    /**
     * HTTP Post object
     */
    CIRHttpPost* iLogMessage;

    CIRFavoritesDb* iFavDb;

    /**
     * Network controller handle pointer
     */
    CIRNetworkController *iIRNetworkControllerHandle;

    /**
     * Reference of the Http data provider Observer
     */
    MIRHttpDataProviderObserver &iObserver;

    /**
     * Status text of the response
     */
    TBuf<KMaxStatusTextLength> iStatusText;

    /**
     * Content type header info
     */
    TBuf8<KDPMaxContentTypeLength> iContentType;

    /**
     * Character set header info
     */
    TBuf8<KMaxCharSetLength> iCharSet;

    /**
     * Set to ETrue if the response code is KNotModified ( 304 )
     */
    TBool iNotModified;

    /**
     * Pointet to Logo download
     */
    CIRLogoDownloadEngine* iLogoDownloadEngine;

    };
#endif // IRHTTPDATAPROVIDER_H


