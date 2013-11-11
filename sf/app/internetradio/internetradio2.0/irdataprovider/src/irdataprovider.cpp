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
* Description:
*
*/
#ifdef REQUEST_ZIP_RESPONSE
#include <ezgzip.h>
#include <bautils.h>
#include <f32file.h>
#endif

#include "irdataprovider.h"
#include "irdataproviderobserver.h"
#include "irdebug.h"
#include "irhttpdataprovider.h"
#include "irhttprequestdata.h"
#include "irsettings.h"

#ifdef REQUEST_ZIP_RESPONSE
_LIT( KGzXmlFile, "iSdsResponse.xml.gz" );
#endif
_LIT( KXmlFile, "iSdsResponse.xml" );

// ---------------------------------------------------------------------------
//  CIRDataProvider::NewL()
//  Creates instance of CIRDataProvider.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRDataProvider *CIRDataProvider::NewL(
                                          MIRDataProviderObserver &aObserver )
    {
    IRLOG_DEBUG( "CIRDataProvider::NewL - Entering" );
    CIRDataProvider *self = NewLC( aObserver );
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRDataProvider::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::NewL()
//  Creates instance of CIRDataProvider.
// ---------------------------------------------------------------------------
//
EXPORT_C CIRDataProvider *CIRDataProvider::NewL(
                  MIRDataProviderObserver &aObserver, const TDesC &aFileName )
    {
    IRLOG_DEBUG( "CIRDataProvider::NewL(..., &aFileName) - Entering" );
    CIRDataProvider *self = NewLC( aObserver, aFileName );
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRDataProvider::NewL(..., &aFileName) - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::~CIRDataProvider()
//  Destructs an instance of CIRDataProvider.
// ---------------------------------------------------------------------------
//
CIRDataProvider::~CIRDataProvider()
    {
    IRLOG_DEBUG( "CIRDataProvider::~CIRDataProvider() - Entering" );

    if ( iHttpDataProvider )
        {
        iHttpDataProvider->CancelTransaction();
        delete iHttpDataProvider;
        }

    if ( iIRSettings )
        {
        iIRSettings->Close();
        }

    if ( iDataProviderTimer )
        {
        iDataProviderTimer->Cancel();
        delete iDataProviderTimer;
        }

    if ( iResponseHeaders )
        {
        delete iResponseHeaders;
        }
    
    iFile.Close();
    iFsSession.Close(); // Close the file server session
    IRLOG_DEBUG( "CIRDataProvider::~CIRDataProvider() - Exiting" );
    }

// General functions exported ( These are the API`s exposed )( HTTP )

// ---------------------------------------------------------------------------
//  CIRDataProvider::IRHttpIssueRequest(TDesC8& aUri)
//  Used to issue an Http request
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRDataProvider::IRHttpIssueRequestL(
     CIRHttpRequestData &aRequestObject )
    {
    IRHttpCancelRequest();
    IRLOG_DEBUG( "CIRDataProvider::IRHttpIssueRequestL - Entering" );
    IRRDEBUG2("CIRDATAPROVIDER::IRHTTPISSUEREQUESTL",KNullDesC);
    CIRHttpResponseData* newResponseHeaders = new ( ELeave ) CIRHttpResponseData;
    delete iResponseHeaders;
    iResponseHeaders = newResponseHeaders;

    // Create or replace the file used to store xml response from iSDS
#ifdef REQUEST_ZIP_RESPONSE
    User::LeaveIfError( iFile.Replace( iFsSession, iXmlGzFilePath, EFileWrite ) );
#else
    User::LeaveIfError( iFile.Replace( iFsSession, iXmlFilePath, EFileWrite ) );
#endif
    
    iHttpDataProvider->CancelTransaction();

    TInt err = iHttpDataProvider->IssueHttpRequestL( aRequestObject );
    // Cancel the timer if active
    iDataProviderTimer->Cancel();
    // Start the timer for timeout
    iDataProviderTimer->After( iTimeOut );
    if ( err == KErrCouldNotConnect )
        {
        // If error in IssueHttpRequest then close the open file
        iFile.Close();
        iDataProviderTimer->Cancel();
        iDataProviderObserver.IRHttpGeneralError( err );
        // Cancel any possibly pending transactions
        iHttpDataProvider->CancelTransaction();
        }

    IRLOG_DEBUG( "CIRDataProvider::IRHttpIssueRequestL - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::IRHttpCancelRequest()
//  Used to cancel a request
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRDataProvider::IRHttpCancelRequest()
    {
    IRLOG_DEBUG( "CIRDataProvider::IRHttpCancelRequest - Entering" );
    // Cancel the timer if active
    iDataProviderTimer->Cancel();
    // Cancel any possibly pending transactions
    iHttpDataProvider->CancelTransaction();
    // Close the file handle used to store the xml response
    iFile.Close();
    IRLOG_DEBUG( "CIRDataProvider::IRHttpCancelRequest - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::ReleaseResources()
//  Used to release the resources held by the IRHttpDataProvider
// ---------------------------------------------------------------------------
//
EXPORT_C void CIRDataProvider::ReleaseResources()
    {
    IRLOG_DEBUG( "CIRDataProvider::ReleaseResources - Entering" );
    // Release the resources held by the IRHttpDataProvider
    iHttpDataProvider->ReleaseResources();
    IRLOG_DEBUG( "CIRDataProvider::ReleaseResources - Exiting" );
    }


//These are the callback functions used by CIRHttpDataProvider to
//provide the CIRDataProvider with the data after processing the
//HTTP request.

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpEventComplete()
//  Used to Indicate to the observer that the request event has completed
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpEventComplete()
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpEventComplete - Entering" );
    iFile.Close();
    iDataProviderTimer->Cancel();
    
#ifdef REQUEST_ZIP_RESPONSE
    // unzip the gz file
    TRAP_IGNORE( UnzipFileL( iXmlFilePath ) );
    // delete the original gz file
    BaflUtils::DeleteFile( iFsSession, iXmlGzFilePath );
#endif
    
    // Need to take a member to a local variable, as the IRHttpDataReceived 
    // may initiate an IRHttpIssueRequestL() call, causing the headers 
    // to be replaced with empty ones, and causing crashes.
    CIRHttpResponseData* currentHeaders = iResponseHeaders;
    // prevents the destructor to delete in case something streange happens.
    iResponseHeaders = NULL;
    iDataProviderObserver.IRHttpDataReceived( iXmlFilePath, *currentHeaders );

    delete currentHeaders;
    IRLOG_DEBUG( "CIRDataProvider::HttpEventComplete - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::ExtractHeaderValue()
//  
// ---------------------------------------------------------------------------
//
void CIRDataProvider::ExtractHeaderValue( const TDesC8& aHeaderData,
                                          const TDesC8& aHeaderName, 
                                          const TDesC8& aDelimeter,
                                          TDes8& aHolder ) const
    {
    IRLOG_DEBUG( "CIRDataProvider::ExtractHeaderValue - Entering" );
    TInt position = aHeaderData.Find( aHeaderName );
    if ( position >= 0 )
        {
        TPtrC8 headerValue = aHeaderData.Mid( position );
        TInt delimeterPosition = headerValue.Find( aDelimeter );
        if ( delimeterPosition != KErrNotFound )
            {
            delimeterPosition++;
            TPtrC8 value = headerValue.Mid( delimeterPosition );
            aHolder.Copy( value );
            aHolder.TrimAll();
            }
        }
    IRLOG_DEBUG( "CIRDataProvider::ExtractHeaderValue - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpHeaderReceived()
//  Used by CIRHttpDataProvider to indicate that an HTTP header is received.
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpHeaderReceived( const TDesC8& aHeaderData )
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpHeaderReceived - Entering." );
    _LIT8(KDelim,":");
    _LIT8(KContentType,"Content-Type");
    ExtractHeaderValue( aHeaderData, KContentType, KDelim,
                        iResponseHeaders-> iContentType );
    _LIT8(KMaxAge,"max-age");
    _LIT8(KDelimEqual,"=");
    ExtractHeaderValue( aHeaderData, KMaxAge, KDelimEqual,
                        iResponseHeaders-> iMaxAge );
    _LIT8(KContentLength,"Content-Length");
    ExtractHeaderValue( aHeaderData, KContentLength, KDelim,
                        iResponseHeaders-> iContentLength );
    _LIT8(KExpires,"Expires");
    ExtractHeaderValue( aHeaderData, KExpires, KDelim,
                        iResponseHeaders->iExpires );
    IRLOG_DEBUG( "CIRDataProvider::HttpHeaderReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpDateHeaderReceived()
//  Used by CIRHttpDataProvider to indicate that an HTTP header is received.
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpDateHeaderReceived( const TDesC8 &aHeader,
                                              const TTime& aTime )
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpDateHeaderReceived - Entering." );
    _LIT8(KDate,"Date");
    _LIT8(KLastModified,"Last-Modified");
    TInt position = aHeader.Find( KDate );
    if ( position != KErrNotFound )
        {
        iResponseHeaders->iDate = aTime;
        //find the difference between device time and response time
        //and storing the offset
        SetOffsetSeconds( aTime );
        return;
        }
    position = aHeader.Find( KLastModified );
    if ( position != KErrNotFound )
        {
        iResponseHeaders->iLastModified = aTime;
        }
    IRLOG_DEBUG( "CIRDataProvider::HttpDateHeaderReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpBodyReceived()
//  Used by CIRHttpDataProvider to indicate that an HTTP response body
//  is received.
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpBodyReceived( const TDesC8 &aBodyData )
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpBodyReceived - Entering" );
    TInt FileWritePos = 0;
    iFile.Seek( ESeekEnd, FileWritePos );
    iFile.Write( FileWritePos, aBodyData );
    IRLOG_DEBUG( "CIRDataProvider::HttpBodyReceived - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpTransactionError()
//  Used by CIRHttpDataProvider to indicate that a HTTP Transaction error
//  has occured.
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpTransactionError( TInt aErrCode )
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpTransactionError - Entering" );
    iFile.Close();
    iDataProviderTimer->Cancel();
    iDataProviderObserver.IRHttpGeneralError( aErrCode );
    // Cancel any possibly pending transactions
    iHttpDataProvider->CancelTransaction();
    IRLOG_DEBUG( "CIRDataProvider::HttpTransactionError - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::HttpResponseCodeRecieved()
//  Used by CIRHttpDataProvider to indicate to the iSDS Client that a
//  304 Not Changed response received
//  Note: Implemented in version 0.2
// ---------------------------------------------------------------------------
//
void CIRDataProvider::HttpResponseCodeRecieved( TInt aResponseCode )
    {
    IRLOG_DEBUG( "CIRDataProvider::HttpResponseCodeRecieved - Entering" );
    iDataProviderObserver.IRHttpResponseCodeReceived( aResponseCode, *iResponseHeaders );
    IRLOG_DEBUG( "CIRDataProvider::HttpResponseCodeRecieved- Exiting" );
    }

// constructor support
// don't export these, because used only by functions in this DLL
// ---------------------------------------------------------------------------
//  CIRDataProvider::TimerExpired()
//  Called to indicate expiry of timer
// ---------------------------------------------------------------------------
//
void CIRDataProvider::TimerExpired()
    {
    IRLOG_INFO( "CIRDataProvider::TimerExpired - Entering" );
    IRHttpCancelRequest();
    HttpTransactionError( KDataProviderTimeout );
    IRLOG_INFO( "CIRDataProvider::TimerExpired - Exiting" );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::CIRDataProvider()
//  Default Constructor
// ---------------------------------------------------------------------------
//
CIRDataProvider::CIRDataProvider( MIRDataProviderObserver &aObserver ):
    iDataProviderObserver( aObserver ) // first-phase C++ constructor
    {
    IRLOG_INFO( "CIRDataProvider::CIRDataProvider" );
    // Definition not required
    }

// ---------------------------------------------------------------------------
//  void CIRDataProvider::ConstructL()
//  2nd Phase construction
// ---------------------------------------------------------------------------
//
void CIRDataProvider::ConstructL()
    {
    IRLOG_DEBUG( "CIRDataProvider::ConstructL - Entering" );
#ifdef REQUEST_ZIP_RESPONSE
    ConstructL( KGzXmlFile );
#else
    ConstructL(KXmlFile);
#endif
    
    iHttpDataProvider->iSetNonUAProfUserAgent = EFalse;
    IRLOG_DEBUG( "CIRDataProvider::ConstructL - Exiting" );
    }

// ---------------------------------------------------------------------------
//  void CIRDataProvider::ConstructL(TDesC& aFilePath)
//  2nd Phase construction
// ---------------------------------------------------------------------------
//
void CIRDataProvider::ConstructL( const TDesC &aFileName )
    {
    IRLOG_DEBUG( "CIRDataProvider::ConstructL(const TDesC &aFileName) - Entering" );
    iHttpDataProvider = CIRHttpDataProvider::NewL( *this );
    iDataProviderTimer = CIRDataProviderTimer::NewL( EPriorityHigh,  *this );
    User::LeaveIfError( iFsSession.Connect() );
    iIRSettings = CIRSettings::OpenL();
#ifdef REQUEST_ZIP_RESPONSE
    iXmlGzFilePath = iIRSettings->PrivatePath();
    iXmlFilePath = iXmlGzFilePath;
    iXmlGzFilePath.Append( aFileName );
    iXmlFilePath.Append( KXmlFile );
#else
    iXmlFilePath = iIRSettings->PrivatePath();
    iXmlFilePath.Append( aFileName );
#endif
    
    iTimeOut = iIRSettings->GetTimeOut();
    iHttpDataProvider->iSetNonUAProfUserAgent = ETrue;
    IRLOG_DEBUG( "CIRDataProvider::ConstructL(const TDesC &aFileName) - Exiting." );
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::NewLC(MIRDataProviderObserver& aObserver)
//  Creates instance of CIRDataProvider.
// ---------------------------------------------------------------------------
//
CIRDataProvider *CIRDataProvider::NewLC( MIRDataProviderObserver &aObserver )
    {
    IRLOG_DEBUG( "CIRDataProvider::NewLC - Entering." );
    CIRDataProvider *self = new( ELeave )CIRDataProvider( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRDataProvider::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
//  CIRDataProvider::NewLC(MIRDataProviderObserver& aObserver)
//  Creates instance of CIRDataProvider.
// ---------------------------------------------------------------------------
//
CIRDataProvider *CIRDataProvider::NewLC( MIRDataProviderObserver &aObserver,
    const TDesC &aFileName )
    {
    IRLOG_DEBUG( "CIRDataProvider::NewLC - Entering." );
    CIRDataProvider *self = new( ELeave )CIRDataProvider( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    IRLOG_DEBUG( "CIRDataProvider::NewLC - Exiting." );
    return self;
    }

EXPORT_C CIRHttpDataProvider* CIRDataProvider::GetHttpDataProvider()
{
IRLOG_DEBUG( "CIRDataProvider::GetHttpDataProvider" );
	return iHttpDataProvider;
}

// ---------------------------------------------------------------------------
//  CIRDataProvider::SetOffsetSeconds()
//  Stores the offset between device time and response header in settings
// ---------------------------------------------------------------------------
//
void CIRDataProvider::SetOffsetSeconds( const TTime& aTime )
    {
    IRLOG_DEBUG( "CIRDataProvider::SetOffsetSeconds - Entering" );
    TTime currenttime;
    //calculates the current time
    currenttime.UniversalTime();
    //finds offset from isds response
    TTimeIntervalSeconds offsetseconds;
    //Find offset from isds response
    TInt err = currenttime.SecondsFrom( aTime, offsetseconds );
    if ( err )
        {
        //if error offsetseconds is set to zero
        offsetseconds = 0;
        }
    //storing the offset value in setting
    TRAP_IGNORE( iIRSettings->SetTimeCorrectionL(offsetseconds.Int()) )
    IRLOG_DEBUG( "CIRDataProvider::SetOffsetSeconds - Exiting." );
    }

#ifdef REQUEST_ZIP_RESPONSE
// ---------------------------------------------------------------------------
//  CIRDataProvider::UnzipFileL
// ---------------------------------------------------------------------------
//
void CIRDataProvider::UnzipFileL( const TDesC& aOutputFile )
    {
    IRLOG_DEBUG( "CIRDataProvider::UnzipFileL - Enter." );
    RFile outputFile;
    CleanupClosePushL( outputFile );
    User::LeaveIfError( outputFile.Replace( iFsSession, aOutputFile, 
                        EFileStream | EFileWrite | EFileShareExclusive ) );
    CEZGZipToFile* gZip = 
        CEZGZipToFile::NewLC( iFsSession, iXmlGzFilePath, outputFile );

    while ( gZip->InflateL() )
        {
        // unzip the gz file, quit when finish
        }
    CleanupStack::PopAndDestroy( gZip );
    CleanupStack::PopAndDestroy( &outputFile );
    IRLOG_DEBUG( "CIRDataProvider::UnzipFileL - Exit." );
    }
#endif

// End of file
