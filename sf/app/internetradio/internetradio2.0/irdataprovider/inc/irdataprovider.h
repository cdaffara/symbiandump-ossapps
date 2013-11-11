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
* Description:  Engine to fetch the logos 
*
*/


#ifndef IRDATAPROVIDER_H
#define IRDATAPROVIDER_H

#include <f32file.h>

#include "irdataprovidertimer.h"
#include "irhttpdataproviderobserver.h"

class CIRHttpDataProvider;
class CIRHttpRequestData;
class CIRHttpResponseData;
class CIRSettings;
class MIRDataProviderObserver;

/**
 * This class provides the interface with the UI for IRDataProvider component
 *
 * @code
 *
 * // Create the data provider object. This is used for interaction with
 * // the iSDS
 *
 * CIRDataProvider* iDataProvider = CIRDataProvider::NewL(iObserver);
 *
 * // This API is used to give a request to the iSDS and the aRequestObject
 * // contains the headers to be set for the request url. The call back
 * // function as defined by MIRDataProviderObserver is called to indicate
 * // any responses.
 *
 * iDataProvider->IRHttpIssueRequestL( aRequestObject );
 *
 * // This API is called to cancel a previously issued request.
 * // If there is no previously issued request it is ignored.
 *
 *  iDataProvider->IRHttpCancelRequest();
 *
 * // This API is called to indicate the data provider component to
 * // release the resources held by it generally called when the network
 * // connection gets disconnected.
 *
 * iDataProvider->ReleaseResources();
 * @endcode
 */

NONSHARABLE_CLASS ( CIRDataProvider ): public CBase,
                                       public MIRHttpDataProviderObserver,
                                       public MIRDataProviderTimeoutNotifier
    {
public:

    /**
     *  CIRDataProvider::NewL
     *  Creates instance of CIRDataProvider.
     *  @param aObserver Observer for HTTP request.
     *  @return CIRDataProvider*
     */
    IMPORT_C static CIRDataProvider *NewL(MIRDataProviderObserver &aObserver);

    /**
     *  CIRDataProvider::NewL()
     *  Creates instance of CIRDataProvider.
     *  @param aObserver Observer for HTTP request.
     *  @return CIRDataProvider*
     */
    IMPORT_C static CIRDataProvider *NewL( MIRDataProviderObserver &aObserver,
                                           const TDesC &aFilePath );

    /**
     *  CIRDataProvider::~CIRDataProvider()
     *  Destructs an instance of CIRDataProvider.
     */
    virtual ~CIRDataProvider();

    /**
     *  CIRDataProvider::IRHttpIssueRequest()
     *  Used to issue an Http request
     *  @param aRequestObject A reference of CIRHttpRequestData containing
     *  HTTP request parameters
     */
    IMPORT_C void IRHttpIssueRequestL( CIRHttpRequestData &aRequestObject );

    /**
     *  CIRDataProvider::IRHttpCancelRequest()
     *  Used to cancel a request
     */
    IMPORT_C void IRHttpCancelRequest();

    /**
     *  CIRDataProvider::ReleaseResources()
     *  Used to release all the resources help by data provider
     *  Called when there is a network disconnection
     */
    IMPORT_C void ReleaseResources();

    IMPORT_C CIRHttpDataProvider* GetHttpDataProvider();
	
    /**
     *  CIRDataProvider::ExtractHeaderValue()
     *  
     *  
     */
    void ExtractHeaderValue( const TDesC8& aHeaderData,
                    const TDesC8& aHeaderName, const TDesC8& aDelimeter,
                    TDes8& aHolder ) const;

    /*
     *These are the callback functions used by CIRHttpDataProvider to
     *provide the CIRDataProvider with the data after processing the
     *HTTP request.
     */
    /**
     *  CIRDataProvider::HttpEventComplete()
     *  Used to Indicate to the observer that the request event has completed
     */
    virtual void HttpEventComplete();

    /**
     *  CIRDataProvider::HttpHeaderReceived()
     *  Used by CIRHttpDataProvider to indicate that an HTTP header is
     *  received.
     *  @param aHeaderData Reference to the Header Info
     */
    virtual void HttpHeaderReceived( const TDesC8 &aHeaderData );

    /**
     *  CIRDataProvider::HttpDateHeaderReceived()
     *  Used by CIRHttpDataProvider to indicate that an HTTP header is
     *  received.
     *  @param aHeader Reference to the Header Info
     *  @param aTime Reference to the TTime object containing time to be
     *  set in the header
     */
    virtual void HttpDateHeaderReceived( const TDesC8 &aHeader,
                    const TTime& aTime );

    /**
     *  CIRDataProvider::HttpBodyReceived()
     *  Used by CIRHttpDataProvider to indicate that an HTTP response
     *  body is received.
     *  @param aHeaderData Reference to the Body data of HTTP request
     */
    virtual void HttpBodyReceived( const TDesC8 &aBodyData );

    /**
     *  CIRDataProvider::HttpTransactionError()
     *  Used by CIRHttpDataProvider to indicate that a HTTP Transaction
     *  error has occured.
     *  @param aErrCode Error code indicating the HTTP Transaction error
     */
    virtual void HttpTransactionError( TInt aErrCode );

    /**
     *  CIRDataProvider::HttpResponseCodeRecieved()
     *  Used by CIRHttpDataProvider to indicate to the iSDS Client that a
     *  304 Not Changed response received
     *  Note: Implemented in version 0.2
     *  @param aResponseCode Response code received after HTTP Transaction
     */
    virtual void HttpResponseCodeRecieved( TInt aResponseCode );

    /**
     *  CIRDataProvider::TimerExpired( )
     *  Called to indicate expiry of timer
     */
    virtual void TimerExpired();

private:
    
    /**
     *  CIRDataProvider::CIRDataProvider()
     *  Default Constructor
     *  @param aObserver Reference to the IRDataProvider Observer
     */
    CIRDataProvider( MIRDataProviderObserver &aObserver );
    
    /**
     *  void CIRDataProvider::ConstructL()
     *  2nd Phase construction
     */
    void ConstructL();
    
    /**
     *  void CIRDataProvider::ConstructL()
     *  2nd Phase construction
     */
    void ConstructL( const TDesC &aFileName );
    
    /**
     *  CIRDataProvider::NewLC(MIRDataProviderObserver& aObserver)
     *  Creates instance of CIRDataProvider.
     *  @param aObserver Observer for HTTP request.
     *  @return CIRDataProvider*
     */
    static CIRDataProvider *NewLC( MIRDataProviderObserver &aObserver );
    
    /**
     *  CIRDataProvider::NewLC()
     *  Creates instance of CIRDataProvider.
     *  @param aObserver Observer for HTTP request.
     *  @return CIRDataProvider*
     */
    static CIRDataProvider *NewLC( MIRDataProviderObserver &aObserver,
                                   const TDesC &aFileName );

    /**
     *  CIRDataProvider::SetOffsetSeconds()
     *  Stores the offset between device time and response header in settings
     *  @param isds time
     */
    void SetOffsetSeconds( const TTime& aTime );
    
#ifdef REQUEST_ZIP_RESPONSE
    /**
     *  CIRDataProvider::UnzipFileL()
     *  Unzip the gz file to xml file
     *  @param aOutputFile out put file
     */
    void UnzipFileL( const TDesC& aOutputFile );
#endif
    
private:

    /**
     * Reference of the observer object to which call backs are returned
     */
    MIRDataProviderObserver &iDataProviderObserver;

    /**
     * Pointer to the HttpDataProvider object which is the actual
     * implementation of RFC 2616
     */
    CIRHttpDataProvider *iHttpDataProvider;

    /**
     * Pointer to timer object
     * Used for Timeout
     */
    CIRDataProviderTimer *iDataProviderTimer;
    
    /**
     * File server session handle
     * Used for creation of XML data
     */
    RFs iFsSession;
    
    /**
     * File handle to create the xml file
     */
    RFile iFile;

#ifdef REQUEST_ZIP_RESPONSE
    /**
     * Name of the Xml Gz file to be created
     */
    TFileName iXmlGzFilePath;
#endif
    
    /**
     * Name of the Xml file to be created
     */
    TFileName iXmlFilePath;

    /**
     * Pointer to Response header data
     */
    CIRHttpResponseData* iResponseHeaders;

    /**
     * Used to access Settings object
     */
    CIRSettings* iIRSettings;

    /**
     * Timer timeout value
     */
    TInt iTimeOut;
    };

#endif // IRDATAPROVIDER_H





