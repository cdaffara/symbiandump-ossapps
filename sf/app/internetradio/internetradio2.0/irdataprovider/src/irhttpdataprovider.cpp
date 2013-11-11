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


#include <httpstringconstants.h>
#include <rhttpheaders.h>
#include <mmfcontrollerpluginresolver.h> //to get the supported MIME types
#ifdef ENABLE_USAGE_REPORT_TEST
#include <bautils.h>
#include "irreportsettings.h"
#endif

#include "irdataprovider.h"
#include "irdebug.h"
#include "irhttpdataprovider.h"
#include "irhttppost.h"
#include "irhttprequestdata.h"
#include "irnetworkcontroller.h"
#include "irdatatransferobserver.h"
#include "irlogodownloadengine.h"
#include "irfavoritesdb.h"
#include "irsettings.h"

//Added for ALR/SNAP
const TInt KBufflengthMaxLength = 255;
const TInt KWapProfileMaxLength = 20;
const TInt KNokiaIrAppAcceptMaxLength = 25;
const TInt KSize = 1024;
const TInt KMaxSize = 2048;
const TInt KBufSize = 10;
const TUid KUidHelixController        = { 0x101F8514 }; // Helix Video controller UID
const TInt KLogbufferLenth = KLogBufferSize/2 - 1;

// Since the supported formats gotten from Helix don't include aac & aacp while it
// does support them, we add them here.
_LIT( KIRDPAacMimeType, "audio/aac" );
_LIT( KIRDPAacpMimeType, "audio/aacp" );
_LIT( KComma, "," );

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::CIRHttpDataProvider(MIRHttpDataProviderObserver&
// aObserver):iObserver(aObserver),iRunning(EFalse)
// Creates instance of CIRHttpDataProvider.
// ---------------------------------------------------------------------------
//
CIRHttpDataProvider::CIRHttpDataProvider( MIRHttpDataProviderObserver
    &aObserver ): iRunning( EFalse ), iObserver( aObserver )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::CIRHttpDataProvider" );
    // Definition not required
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::~CIRHttpDataProvider()
// Destructs instance of CIRHttpDataProvider.
// ---------------------------------------------------------------------------
//
CIRHttpDataProvider::~CIRHttpDataProvider()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::~CIRHttpDataProvider - Entering" );
    // remove E32USER-CBase-66
#ifdef _DEBUG
    TRAP_IGNORE( iHttpSession.Close() );
#else
    iHttpSession.Close();
#endif
    delete iLogMessage;
    if ( iIRNetworkControllerHandle )
        {
        iIRNetworkControllerHandle->Close();
        }
    if(iLogoDownloadEngine)
    	{
	    delete iLogoDownloadEngine;
	    iLogoDownloadEngine = NULL;
    	}
    IRLOG_DEBUG( "CIRHttpDataProvider::~CIRHttpDataProvider - Exiting." );
    }


// ---------------------------------------------------------------------------
// CIRHttpDataProvider::NewL()
// Creates instance of CIRHttpDataProvider.
// ---------------------------------------------------------------------------
//
CIRHttpDataProvider *CIRHttpDataProvider::NewL( 
                                      MIRHttpDataProviderObserver &aObserver )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::NewL - Entering" );
    CIRHttpDataProvider *self = NewLC( aObserver );
    CleanupStack::Pop(self);
    IRLOG_DEBUG( "CIRHttpDataProvider::NewL - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::NewLC()
// Creates instance of CIRHttpDataProvider.
// ---------------------------------------------------------------------------
//
CIRHttpDataProvider *CIRHttpDataProvider::NewLC( 
                                      MIRHttpDataProviderObserver &aObserver )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::NewLC - Entering" );
    CIRHttpDataProvider *self = new( ELeave )CIRHttpDataProvider( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRHttpDataProvider::NewLC - Exiting" );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::ConstructL()
// 2nd phase construction
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::ConstructL()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::ConstructL - Entering" );
    iFirstTime = ETrue;
    iIRNetworkControllerHandle = CIRNetworkController::OpenL();
    iLogMessage = CIRHttpPost::NewL(iHttpTransaction);
    iLogoDownloadEngine = CIRLogoDownloadEngine::NewL();
    IRLOG_DEBUG( "CIRHttpDataProvider::ConstructL - Exiting" );
    }

EXPORT_C CIRLogoDownloadEngine* CIRHttpDataProvider::GetLogoDownloadEngine()
{
IRLOG_DEBUG( "CIRHttpDataProvider::GetLogoDownloadEngine" );
	return iLogoDownloadEngine;
}

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::CancelTransactionL()
// Cancel the issued Http transaction
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::CancelTransaction()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::CancelTransaction  - Entering" );
    // Make sure that logfile is closed
    CloseLogFile ();
    if ( !iRunning )
        {
        return ;
        }
    // Close() also cancels transaction (Cancel() can also be used but
    // resources allocated by transaction must be still freed with Close())
    iHttpTransaction.Close();
    // Not running anymore
    iRunning = EFalse;
    IRLOG_DEBUG( "CIRHttpDataProvider::CancelTransaction - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::CloseLogFile()
// Used to Close the log file LogUsage.gz externally from other sources (bug-fixing)
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::CloseLogFile ()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::CloseLogFile  - Entering" );
    if ( iLogMessage )
        {
        iLogMessage->CloseLogFile();
        }
    IRLOG_DEBUG( "CIRHttpDataProvider::CloseLogFile  - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::IssueHttpRequestL()
// Used to issue the request.
// ---------------------------------------------------------------------------
//
TInt CIRHttpDataProvider::IssueHttpRequestL( CIRHttpRequestData &aRequestInfo )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::IssueHttpRequestL - Entering" );
    iNotModified = EFalse;
    
    if ( !( iIRNetworkControllerHandle->GetNetworkStatus() ) )
        {
        // Error Network connection inactive
        return KErrCouldNotConnect;
        }
    if ( iFirstTime )
        {
        InitializeHttpDataProviderL();
        iIRNetworkControllerHandle->InitializeHttpSessionL( iHttpSession,
                            MIRDataTransferTracker::EIRTransferCategoryIsds );
        iFirstTime = EFalse;
        }
    // Parse string to URI (as defined in RFC2396)
    TUriParser8 uri;
    uri.Parse( aRequestInfo.iUri );
    // Get request method string
    RStringF method;
    switch ( aRequestInfo.iMethod )
        {
        case EIRHttpGET:
            {
            method = iHttpSession.StringPool().StringF( HTTP::EGET,
                                                   RHTTPSession::GetTable() );
            }
            break;
        case EIRHttpPOST:
            {
            method = iHttpSession.StringPool().StringF( HTTP::EPOST,
                                                   RHTTPSession::GetTable() );
            }
            break;
        default:
            {
            }
            break;
        }
    CleanupClosePushL( method );

    IRLOG_DEBUG( "CIRHttpDataProvider::IssueHttpRequestL - Opening transaction" );
    // Open transaction with previous method and parsed uri. This class will
    // receive transaction events in MHFRunL and MHFRunError.
    iHttpTransaction = iHttpSession.OpenTransactionL( uri,  *this, method );

#ifdef ENABLE_USAGE_REPORT_TEST
    RFs iFs;
    User::LeaveIfError(iFs.Connect());
    
    TTime timeStamp;
    //timeStamp.UniversalTime();
    timeStamp.HomeTime();
    TDateTime dateTime =  timeStamp.DateTime();
    
    _LIT(KFormatTime,"-%d-%d-%d %d.%d.%d.xml.gz\n");
    TBuf<32>    sendTime;
    sendTime.Format(KFormatTime,dateTime.Year(),TInt(dateTime.Month()+1),dateTime.Day()+1,
                                dateTime.Hour(),dateTime.Minute(),dateTime.Second());

    _LIT( KDstFilePath, "c:\\data\\IR_Log\\");
    TFileName dstFileName(KDstFilePath);
    dstFileName.Append(KGZipLogFileName);
    dstFileName.Append(sendTime);
    
    iFs.MkDirAll(dstFileName);
    
    if ( aRequestInfo.iMethod == EIRHttpPOST )
        {
        CIRSettings * settings = CIRSettings::OpenL();
        if ( NULL != settings )
            {
            TFileName filePath = settings->PrivatePath();
            filePath.Append( KGZipLogFileName );                   

            BaflUtils::CopyFile( iFs, filePath, dstFileName );
            settings->Close();
            }
        }
#endif
    
    if ( aRequestInfo.iMethod == EIRHttpPOST )
        {
        IRLOG_DEBUG( "CIRHttpDataProvider::IssueHttpRequestL - POST method" );
        // Close the LogFile
        iLogMessage->CloseLogFile ();
        iLogMessage->SetTransaction( iHttpTransaction );
        TInt FileSize(KErrNone);
        TRAPD(err, iLogMessage->GetFileToPostL(&FileSize));
        if ( err )
            {
            // failed to open the file to be posted hence cancel the request
            CleanupStack::PopAndDestroy( &method );
            iHttpTransaction.Close();
            return KErrCouldNotConnect;
            }
        TBuf8<KBufflengthMaxLength> bufflength;
        _LIT8(KFormat,"%d");
        bufflength.Format(KFormat,FileSize);
        aRequestInfo.iContentType.Copy(KPostContentType);
        RHTTPRequest LogRequest;
        LogRequest = iHttpTransaction.Request();
        LogRequest.SetBody((MHTTPDataSupplier&) *iLogMessage);
        RHTTPHeaders header = iHttpTransaction.Request().GetHeaderCollection();
        SetHeaderL(header,HTTP::EContentType,aRequestInfo.iContentType );
        SetHeaderL(header,HTTP::EContentLength,bufflength);
        }
    
     // Setup the headers for the HTTP request
    BuildHeadersL(aRequestInfo);
  
    // Submit the transaction. After this the framework will give transaction
    // events via MHFRunL and MHFRunError.
    iHttpTransaction.SubmitL();
   
#ifdef ENABLE_USAGE_REPORT_TEST  
    if ( aRequestInfo.iMethod == EIRHttpPOST )
        {
        _LIT( KSentPrefix, "X__");
        TFileName sentLogFileName(KDstFilePath);
        sentLogFileName.Append(KSentPrefix);
        sentLogFileName.Append(KGZipLogFileName);
        sentLogFileName.Append(sendTime);
        BaflUtils::RenameFile(iFs, dstFileName, sentLogFileName);      
        }
    iFs.Close();
#endif    
    
    CleanupStack::PopAndDestroy( &method );
    iRunning = ETrue;
    IRLOG_INFO( "CIRHttpDataProvider::IssueHttpRequestL - Submitted HTTP request" );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::SetHttpContentType()
// Set the Http content type
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::SetHttpContentType( const TDesC &aContentType,
                                              const TDesC &aCharSet )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::SetHttpContentType - Entering" );
    iContentType.Copy( aContentType );
    iCharSet.Copy( aCharSet );
    IRLOG_DEBUG( "CIRHttpDataProvider::SetHttpContentType - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::InitializeHttpDataProviderL()
// Initialize the data provider component
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::InitializeHttpDataProviderL()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::InitializeHttpDataProviderL - Entering" );
    iHttpSession.Close();
    TRAPD( LeaveValue, iHttpSession.OpenL() );
    if ( LeaveValue != KErrNone )
        {
        // Most common error; no access point configured, and session creation
        // leaves with KErrNotFound.
        // Load a string from the resource file and add the error code to string
        User::Leave( LeaveValue );
        }
    // Set the HTTP connection properties
    RStringPool strP = iHttpSession.StringPool();
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    // RSocketServ Handle
    connInfo.SetPropertyL( strP.StringF( HTTP::EHttpSocketServ,
                    RHTTPSession::GetTable() ), THTTPHdrVal(
                    iIRNetworkControllerHandle->GetIRSocketServer().Handle() ) );
    // RConnection Handle
    TInt connPtr = REINTERPRET_CAST( TInt, &( iIRNetworkControllerHandle->GetIRConnection() ) );

    connInfo.SetPropertyL( strP.StringF( HTTP::EHttpSocketConnection,
                    RHTTPSession::GetTable() ), THTTPHdrVal( connPtr ) );
    // HTTP Version ( Specify that i am using HTTP/1.1
    connInfo.SetPropertyL( strP.StringF( HTTP::EVersion,
                    RHTTPSession::GetTable() ), THTTPHdrVal( strP.StringF(
                    HTTP::EHttp11, RHTTPSession::GetTable() ) ) );

    RStringF temp = strP.OpenFStringL( KAccept );
    CleanupClosePushL( temp );
    connInfo.SetPropertyL( strP.StringF( HTTP::EAccept,
                    RHTTPSession::GetTable() ), THTTPHdrVal( temp ) );

    CleanupStack::PopAndDestroy( &temp );
    iContentType.Copy( KDefaultContentType );
    iCharSet.Copy( KDefaultCharSet );

    IRLOG_DEBUG( "CIRHttpDataProvider::InitializeHttpDataProviderL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::MHFRunL()
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::MHFRunL( RHTTPTransaction aTransaction,
    const THTTPEvent &aEvent )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::MHFRunL - Entering." );
    
    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - EGotResponseHeaders" );
            // HTTP response headers have been received. Use
            // aTransaction.Response() to get the response. However, it's not
            // necessary to do anything with the response when this event occurs.
            // Get HTTP status code from header (e.g. 200)
            RHTTPResponse resp = aTransaction.Response();
            iStatusCode = resp.StatusCode();
            // Get status text (e.g. "OK")
            iStatusText.Copy(
                    (const unsigned short int*)resp.StatusText().DesC().Ptr(),
                    iStatusText.MaxLength() );
            // Extract header field names and value
            ExtractResponseHeadersL( aTransaction );
            // Validate the status code
            ValidateStatusCode( iStatusCode );
            }
            break;
        case THTTPEvent::EGotResponseBodyData:
            {
            // Part  of response's body data received. Use
            // aTransaction.Response().Body()->GetNextDataPart() to get the actual
            // body data.
            // Get the body data supplier
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - EGotResponseBodyData" );
            MHTTPDataSupplier *body = aTransaction.Response().Body();
            TPtrC8 dataChunk;
            body->GetNextDataPart( dataChunk );
            iObserver.HttpBodyReceived( dataChunk );
            // To release the body data.
            body->ReleaseData();
            }
            break;
        case THTTPEvent::ESucceeded:
            {
            IRLOG_INFO( "CIRHTTPDATAPROVIDER::MHFRUNL - ESucceeded" );
            // Transaction can be closed now. It's not needed anymore.
            aTransaction.Close();
            iRunning = EFalse;
            iObserver.HttpEventComplete();
            }
            break;
        case THTTPEvent::EFailed:
            {
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - EFailed" );
            IRLOG_INFO2( "CIRHttpDataProvider::MHFRunL aEvent.iStatus= %d", aEvent.iStatus );
            IRLOG_INFO2( "CIRHttpDataProvider::MHFRunL iStatusCode= %d", iStatusCode );
            // Transaction completed with failure.
            aTransaction.Close();
            iRunning = EFalse;
            iObserver.HttpTransactionError( iStatusCode );
            }
            break;
       case THTTPEvent::ENotifyNewRequestBodyPart:
            {
            iLogMessage->ReleaseData();
            }
            break;
        case KServiceUnavailable:
            {
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - KServiceUnavailable" );
            aTransaction.Close();
            iRunning = EFalse;
            iObserver.HttpTransactionError( KServiceUnavailable );
            }
            break;
        case KDndTimedOut:
            {
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - KDndTimedOut" );
            // Just close the transaction on errors
            aTransaction.Close();
            iRunning = EFalse;
            iObserver.HttpTransactionError( KDndTimedOut );
            }
            break;      
        default:
            // There are more events in THTTPEvent,
            // Which is needed can be implemented incase required
            // by the HTTP Data Provider
            {
            IRLOG_INFO( "CIRHttpDataProvider::MHFRunL - default" );
            if ( aEvent.iStatus < 0 )
                {
                IRRDEBUG2( "CIRHttpDataProvider::MHFRunL aEvent.iStatus= %d", aEvent.iStatus );
                // Just close the transaction on errors
                aTransaction.Close();
                iRunning = EFalse;
                iObserver.HttpTransactionError( aEvent.iStatus );
                }
            // Other events are not errors
            // (e.g. permanent and temporary redirections)
            // ignore such errors
            }
            break;
        }
    IRLOG_DEBUG( "CIRHttpDataProvider::MHFRunL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::MHFRunError()
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction errors
// ---------------------------------------------------------------------------
//
TInt CIRHttpDataProvider::MHFRunError( TInt /* aError*/,
    RHTTPTransaction /*aTransaction*/, const THTTPEvent & /*aEvent*/ )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::MHFRunError - Entering" );
    // Handle error and return KErrNone.
    IRLOG_DEBUG( "CIRHttpDataProvider::MHFRunError - Exiting." );
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::ExtractResponseHeadersL()
// Used to Extract the response headers.
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::ExtractResponseHeadersL(
                                        const RHTTPTransaction& aTransaction )
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::ExtractResponseHeadersL - Entering" );
    RHTTPResponse response = aTransaction.Response();
    RHTTPHeaders respHeader = response.GetHeaderCollection();
    THTTPHdrFieldIter iterator = respHeader.Fields();
    RStringPool httpStringPool = aTransaction.Session().StringPool();
    iterator.First();
    HBufC8 *headerField = HBufC8::NewLC( KMaxHeaderNameLength +
                                         KMaxHeaderValueLength );
    HBufC8 *fieldValBuf = HBufC8::NewLC( KMaxHeaderValueLength );
    while ( iterator.AtEnd() == EFalse )
        {
        RStringTokenF fieldName = iterator();
        RStringF fieldNameStr = httpStringPool.StringF( fieldName );
        THTTPHdrVal fieldVal;
        if ( respHeader.GetField( fieldNameStr, 0, fieldVal ) == KErrNone )
            {
            const TDesC8 &fieldNameDesC = fieldNameStr.DesC();
            headerField->Des().Copy(
                            fieldNameDesC.Left( KMaxHeaderNameLength ) );
            fieldValBuf->Des().Zero();
            switch ( fieldVal.Type() )
                {
                // the value is an integer
                //lint restore -e747:Significant prototype
                //coercion (arg. no. 1) int to long long  :
                case THTTPHdrVal::KTIntVal:
                    fieldValBuf->Des().Num( fieldVal.Int() );
                    break;
                    // the value is a case-insensitive string
                case THTTPHdrVal::KStrFVal:
                    {
                    RStringF fieldValStr = httpStringPool.StringF(
                                                            fieldVal.StrF() );
                    const TDesC8 &fieldValDesC = fieldValStr.DesC();
                    fieldValBuf->Des().Copy( fieldValDesC.Left(
                                                    KMaxHeaderValueLength ) );
                    }
                    break;
                    // the value is a case-sensitive string
                case THTTPHdrVal::KStrVal:
                    {
                    RString fieldValStr = httpStringPool.String(
                                    fieldVal.Str() );
                    const TDesC8 &fieldValDesC = fieldValStr.DesC();
                    fieldValBuf->Des().Copy( fieldValDesC.Left(
                                    KMaxHeaderValueLength ) );
                    }
                    break;
                    // the value is a date/time
                case THTTPHdrVal::KDateVal:
                    {
                    TDateTime date = fieldVal.DateTime();
                    TTime t( date );
                    if ( !iSetNonUAProfUserAgent )
                        {
                        iObserver.HttpDateHeaderReceived( *headerField, t );
                        }
                    }
                    break;
                    // the value is type is unknown
                default:
                    break;
                }
            // Display HTTP header field name and value
            headerField->Des().Append( KDPColon );
            headerField->Des().Append( *fieldValBuf );
            iObserver.HttpHeaderReceived( *headerField );
            }
        ++iterator;
        }
    
    CleanupStack::PopAndDestroy( fieldValBuf );
    CleanupStack::PopAndDestroy( headerField );
    IRLOG_DEBUG( "CIRHttpDataProvider::ExtractResponseHeadersL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::SetHeaderL()
// Used to set the Http header
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::SetHeaderL( RHTTPHeaders aHeaders,
                         TInt aHeaderField, const TDesC8 &aHeaderValue ) const
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::SetHeaderL - Entering" );
    RStringF valStr = iHttpSession.StringPool().OpenFStringL( aHeaderValue );
    CleanupClosePushL( valStr );
    THTTPHdrVal val( valStr );
    aHeaders.SetFieldL( iHttpSession.StringPool().StringF( aHeaderField,
                                            RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr );
    IRLOG_DEBUG( "CIRHttpDataProvider::SetHeaderL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::BuildHeadersL()
//  Build the headers for the request
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::BuildHeadersL(const CIRHttpRequestData &aRequestInfo)
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::BuildHeadersL - Entering" );
    RBuf logstr;
    logstr.CreateL(KMaxSize);
    logstr.CleanupClosePushL();
     // Set headers for request; user agent and accepted content type
    RHTTPHeaders header = iHttpTransaction.Request().GetHeaderCollection();

    SetHeaderL( header, HTTP::EAccept, KAccept );
     logstr.Copy(KAccept);
    IRLOG_DEBUG2( "CIRHttpDataProvider::BuildHeadersL - HTTP::EAccept = %S", &logstr );

    // Set the User-Agent header to UAProf string
    SetHeaderL( header, HTTP::EUserAgent,
                iIRNetworkControllerHandle->GetUAProfString()->Des() );
       logstr.Copy(iIRNetworkControllerHandle->GetUAProfString()->Des());
    IRLOG_DEBUG( "CIRHttpDataProvider::BuildHeadersL - HTTP::EUserAgent =");		   
    TInt logstrLenth = logstr.Length();
    TBuf<KLogbufferLenth> tempStr;
    TInt index = 0;
    while( logstrLenth > 0 )
      {
      if( logstrLenth > KLogbufferLenth )
          {
          tempStr.Copy( &logstr[index], KLogbufferLenth );
          }
      else
          {
          tempStr.Copy( &logstr[index], logstrLenth );
          }
      IRLOG_DEBUG2( "%S", &tempStr ); 
      index += KLogbufferLenth;
      logstrLenth -= KLogbufferLenth;
      }  
    // Set the Accept Character set header
    SetHeaderL( header, HTTP::EAcceptCharset, KAcceptCharset );
   logstr.Copy(KAcceptCharset);
    IRLOG_DEBUG2( "CIRHttpDataProvider::BuildHeadersL - HTTP::EAcceptCharset = %S", &logstr );

    // Set the Accept-Language header as specified in aRequestInfo
    if ( aRequestInfo.iAcceptLanguage.Length() != 0 )
        {
        SetHeaderL( header, HTTP::EAcceptLanguage,
                        aRequestInfo.iAcceptLanguage );
        logstr.Copy(aRequestInfo.iAcceptLanguage);
	    IRLOG_DEBUG2( "CIRHttpDataProvider::BuildHeadersL - HTTP::EAcceptLanguage = %S", &logstr );
        }
    // If not specified then set the default accept language as "en"
    else
        {
        SetHeaderL( header, HTTP::EAcceptLanguage, KAcceptLanguage );
        logstr.Copy(KAcceptLanguage);
        IRLOG_DEBUG2( "CIRHttpDataProvider::BuildHeadersL - HTTP::EAcceptLanguage = %S", &logstr );
        }

#ifdef REQUEST_ZIP_RESPONSE
    // Add Accept-Encoding: gzip HTTP request header. The server will use compression to improve data
	// transfer speed.
    SetHeaderL( header, HTTP::EAcceptEncoding, KAcceptEncoding );
    logstr.Copy(KAcceptEncoding);
    IRLOG_DEBUG2( "CIRHttpDataProvider::BuildHeadersL - HTTP::EAcceptEncoding = %S", &logstr );
#endif

    // Set the If-Modified-Since header if required
    if ( aRequestInfo.isIfModifiedSet )
        {
        THTTPHdrVal val( aRequestInfo.iIfModifiedSince );
        header.SetFieldL( iHttpSession.StringPool().StringF(
                    HTTP::EIfModifiedSince, RHTTPSession::GetTable() ), val );
        }

    // Set the x-wap-profile header

    TBuf8<KWapProfileMaxLength> xWapProfile;
    _LIT(KXWapProfile,"x-wap-profile");
    xWapProfile.Copy( KXWapProfile );
    RStringF xWapProfileString = iHttpSession.StringPool().OpenFStringL(
                    xWapProfile );
    RStringF xWapProfileValueString = iHttpSession.StringPool().OpenFStringL(
                    iIRNetworkControllerHandle->GetWapProfString()->Des() );
   logstr.Copy(iIRNetworkControllerHandle->GetWapProfString()->Des());
   IRLOG_DEBUG( "CIRHttpDataProvider::BuildHeadersL - x-wap-profile =");   
   logstrLenth = logstr.Length();
   index = 0;
   while( logstrLenth > 0 )
      {
      if( logstrLenth > KLogbufferLenth )
          {
          tempStr.Copy( &logstr[index], KLogbufferLenth );
          }
      else
          {
          tempStr.Copy( &logstr[index], logstrLenth );
          }
      IRLOG_DEBUG2( "%S", &tempStr ); 
      index += KLogbufferLenth;
      logstrLenth -= KLogbufferLenth;
      }  
   
    THTTPHdrVal xWapProfileHeader( xWapProfileValueString );
    header.SetFieldL( xWapProfileString, xWapProfileHeader );
    xWapProfileString.Close();
    xWapProfileValueString.Close();
   
    //this piece of code is to get the MIME types supported by a device
    TInt i,ii,j;
    CMMFControllerPluginSelectionParameters *cs = 
                             CMMFControllerPluginSelectionParameters::NewLC();
    CMMFFormatSelectionParameters * fs = 
                                       CMMFFormatSelectionParameters::NewLC();
    cs->SetRequiredPlayFormatSupportL( *fs );
    cs->SetRequiredRecordFormatSupportL( *fs );

    RMMFControllerImplInfoArray controllers;
    CleanupResetAndDestroyPushL( controllers );
    cs->ListImplementationsL( controllers );

    TInt contrCount = controllers.Count();
    RBuf8 audioMIMEs;
	CleanupClosePushL (audioMIMEs);
    audioMIMEs.CreateL( KSize );
    RBuf audioBuf;
	CleanupClosePushL (audioBuf);
    audioBuf.CreateL( KBufSize );
    _LIT(KAudio,"audio");
    audioBuf.Copy( KAudio );
    for ( i = 0; i < controllers.Count(); i++ )
        {
        if( KUidHelixController == controllers[i]->Uid() )
            {
            //supported play formats
            const RMMFFormatImplInfoArray &pf = controllers[i]->PlayFormats();
            TInt pfCount = pf.Count();
            for ( ii = 0; ii < pf.Count(); ii++ )
                {
                RBuf z;
                RBuf z1;
                TBool first;
                //file extensions
                const CDesC8Array &fe = pf[ii]->SupportedFileExtensions();
                first = TRUE;
                z.CreateL(KSize);
                z.CleanupClosePushL();
                z1.CreateL(KSize);
                z1.CleanupClosePushL();
                z1.Zero();
                for ( j = 0; j < fe.Count(); j++ )
                    {
                    _LIT(KNone,", ");
                    if ( !first )
                        z1.Append( KNone );
                    first = FALSE;
                    z.Copy( fe[j] );
                    z1.Append( z );
                    };
    
                //MIME types
                const CDesC8Array &mt = pf[ii]->SupportedMimeTypes();
                first = TRUE;
                z1.Zero();
                TInt tempD = 0;
                for ( j = 0; j < mt.Count(); j++ )
                    {
                    z.Copy( mt[j] );
                    TInt res = z.Find( audioBuf );
                    if ( res != KErrNotFound )
                        {
                        if ( !first )
                            {
                            _LIT(KNone,", ");
                            z1.Append( KNone );
                            }
                        first = FALSE;
                        z1.Append( z );
                        audioMIMEs.Append( z );
                        if ( j == mt.Count() && ii == pf.Count() && 
                             i == controllers.Count() )
                            {
                            //do nothing
                            }
                        else
                            {
                            audioMIMEs.Append( KComma );
                            }
                        tempD++;
                        }
                    };
                CleanupStack::PopAndDestroy(2);
                };// for play formats
            }
        };
    
    // Since the supported formats gotten from Helix don't include aac & aacp while it
    // does support them, we add them here.
    audioMIMEs.Append( KIRDPAacMimeType );
    audioMIMEs.Append( KComma );
    audioMIMEs.Append( KIRDPAacpMimeType );
    audioMIMEs.Append( KComma );

    // Set the X-Nokia-iRAPP-Accept header
    TBuf8<KNokiaIrAppAcceptMaxLength> xNokiaIrAppAccept;
    _LIT(KXNokiaIrAppAccept,"X-Nokia-iRAPP-Accept");
    xNokiaIrAppAccept.Copy( KXNokiaIrAppAccept );
    RStringF xNokiaIrAppAcceptString = 
                  iHttpSession.StringPool().OpenFStringL( xNokiaIrAppAccept );
    RStringF xNokiaIrAppAcceptValueString =
                         iHttpSession.StringPool().OpenFStringL( audioMIMEs );
   logstr.Copy(audioMIMEs);
   IRLOG_DEBUG( "CIRHttpDataProvider::BuildHeadersL - X-Nokia-iRAPP-Accept =" );
   logstrLenth = logstr.Length();
   index = 0;
   while( logstrLenth > 0 )
      {
      if( logstrLenth > KLogbufferLenth )
          {
          tempStr.Copy( &logstr[index], KLogbufferLenth );
          }
      else
          {
          tempStr.Copy( &logstr[index], logstrLenth );
          }
      IRLOG_DEBUG2( "%S", &tempStr ); 
      index += KLogbufferLenth;
      logstrLenth -= KLogbufferLenth;
      }     
    THTTPHdrVal xNokiaIrAppAcceptHeader( xNokiaIrAppAcceptValueString );
    header.SetFieldL(xNokiaIrAppAcceptString, xNokiaIrAppAcceptHeader);
    xNokiaIrAppAcceptString.Close();
    xNokiaIrAppAcceptValueString.Close();
 
    CleanupStack::PopAndDestroy( 3 );  // audioMIMEs + audioBuf + RMMFControllerImplInfoArray controllers;
    CleanupStack::PopAndDestroy( fs );
    CleanupStack::PopAndDestroy( cs );
    
   
    CleanupStack::PopAndDestroy(&logstr);
    IRLOG_DEBUG( "CIRHttpDataProvider::BuildHeadersL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::ValidateStatusCode()
// Validate the status code returned
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::ValidateStatusCode( TInt aStatusCode )
    { 
    IRLOG_DEBUG( "CIRHttpDataProvider::ValidateStatusCode- Entering" );
    // Callbacks will be provided as and when the call handling in UI is ready
    switch ( aStatusCode )
        {
        case KOk:
            {
            }
            break;
        case KCreated:
            {
            }
            break;
        case KAccepted:
            {
            }
            break;
        case KNonAuthoritativeInformation:
            {
            }
            break;
        case KNoContent:
            {
            }
            break;
        case KResetContent:
            {
            }
            break;
        case KPartialContent:
            {
            }
            break;
        // Redirection 3xx status codes
        case KMultipleChoices:
            {
            }
            break;
        case KMovedPermanently:
            {
            }
            break;
        case KFound:
            {
            }
            break;
        case KSeeOther:
            {
            }
            break;
        case KNotModified:
            {
            iNotModified = ETrue;
            iObserver.HttpResponseCodeRecieved( KNotModified );
            }
            break;
        case KUseProxy:
            {
            }
            break;
        // This status code is not used as per RFC 2616 
        // const TInt KUnused = 306
        case KTemporaryRedirect:
            {
            }
            break;
        // Client Error 4xx status codes
        case KBadRequest:
            {
            }
            break;
        case KUnauthorized:
            {
            }
            break;
        case KPaymentRequired:
            {
            }
            break;
        case KForbidden:
            {
            }
            break;
        case KNotFound:
            {
            iObserver.HttpResponseCodeRecieved( KNotFound );
            }
            break;
        case KMethodNotAllowed:
            {
            }
            break;
        case KNotAcceptable:
            {
            }
            break;
        case KProxyAuthenticationRequired:
            {
            }
            break;
        case KRequestTimeout:
            {
            }
            break;
        case KConflict:
            {
            }
            break;
        case KGone:
            {
            }
            break;
        case KLengthRequired:
            {
            }
            break;
        case KPreconditionFailed:
            {
            }
            break;
        case KRequestEntityTooLarge:
            {
            }
            break;
        case KRequestURITooLong:
            {
            }
            break;
        case KUnsupportedMediaType:
            {
            }
            break;
        case KRequestedRangeNotSatisfiable:
            {
            }
            break;
        case KExpectationFailed:
            {
            }
            break;
        // Server Error 5xx status codes
        case KInternalServerError:
            {
            }
            break;
        case KNotImplemented:
            {
            }
            break;
        case KBadGateway:
            {
            }
            break;
        case KServiceUnavailable:
            {
            }
            break;
        case KGatewayTimeout:
            {
            }
            break;
        case KHTTPVersionNotSupported:
            {
            }
            break;
        default:
            break;
        } // End of switch(aStatusCode)
    IRLOG_DEBUG( "CIRHttpDataProvider::ValidateStatusCode - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRHttpDataProvider::ReleaseResources()
// Used to free all resources
// ---------------------------------------------------------------------------
//
void CIRHttpDataProvider::ReleaseResources()
    {
    IRLOG_DEBUG( "CIRHttpDataProvider::ReleaseResources - Entering" );
	iLogoDownloadEngine->ReleaseResources();
    CancelTransaction();
    iHttpSession.Close();
    iFirstTime = ETrue;
    IRLOG_DEBUG( "CIRHttpDataProvider::ReleaseResources - Exiting." );
    }
