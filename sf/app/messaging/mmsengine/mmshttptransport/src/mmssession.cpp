/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     MMS transport session
*
*/



// INCLUDE FILES
#include    <httpstringconstants.h>
#include    <stringpool.h>
#include    <uaproffilter_interface.h>
#include    <http/cecomfilter.h>
#include    <featmgr.h>

#include    "mmssession.h"
#include    "mmstransaction.h"
#include    "mmsbearerstatus.h"
#include    "mmsconst.h"
#include    "mmsservercommon.h"
#include    "mmserrors.h"
#include    "MmsServerDebugLogging.h"
   
// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic( TMmsPanic aPanic );

// CONSTANTS
// Timervalue used in network transactions (i.e. when using HTTP API)
const TInt KMmsTransactionTimer = 900; // 900 sec = 15 min
const TInt KMmsSecondsToMicroseconds = 1000000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsSession::CMmsSession
// ---------------------------------------------------------
//
CMmsSession::CMmsSession( TInt aPriority )
    :CActive( aPriority ),    
    iConnected( EFalse ),
    iTransaction( NULL ),
    iRequestStatus( NULL),
    iSessionStatus( ESessionIdle ),
    iHeadersCreated( EFalse ),
    iSessionOpened( EFalse ),
    iTransferControl( NULL ),
    iUserAgent( NULL ),
    iUaProf( NULL )
    {
    }

// ---------------------------------------------------------
// CMmsSession::ConstructL
// ---------------------------------------------------------
//
void CMmsSession::ConstructL(
    RSocketServ& aSocketServ,
    RConnection& aConnection )
    {
    CActiveScheduler::Add(this);

    iSocketServ = &aSocketServ;
    iConnection = &aConnection;
    
    // Construct bearer status object
    iTransferControl = CMmsBearerStatus::NewL( aConnection );

    // initialize to defaults
    iTransactionTimeout = KMmsTransactionTimer * KMmsSecondsToMicroseconds;
    
    // Open HTTP session
    iHTTPSession.OpenL( KProtocolHTTP );
    iSessionOpened = ETrue;
    }

// ---------------------------------------------------------
// CMmsSession::NewL
// ---------------------------------------------------------
//
EXPORT_C CMmsSession* CMmsSession::NewL(
    TInt aPriority,
    RSocketServ& aSocketServ,
    RConnection& aConnection )
    {
    CMmsSession* self = new(ELeave) CMmsSession( aPriority );
    CleanupStack::PushL( self );
    self->ConstructL( aSocketServ, aConnection );
    CleanupStack::Pop( self );
    return self;
    }
   
// ---------------------------------------------------------
// CMmsSession::~CMmsSession
// ---------------------------------------------------------
//
CMmsSession::~CMmsSession()
    {
    Cancel();

    // clean up the headers if we have added them
    if ( iHeadersCreated )
        {
        iSessionHeaders.RemoveAllFields();
        }
    delete iUserAgent;
    delete iUaProf;
    delete iTransaction;
    delete iTransferControl;
    if ( iSessionOpened ) // cannot close if not opened
        {
        iHTTPSession.Close();
        }
    }

// ---------------------------------------------------------
// CMmsSession::OpenL
// ---------------------------------------------------------
//
EXPORT_C void CMmsSession::OpenL( 
    const TDesC8& aProxyAddress,
    const TBool aUseProxy,  
    const TInt32 aMaxReceiveSize,
    const TInt32 aMaxSendSize,
    TRequestStatus& aStatus )
    {
    LOG( _L("CMmsSession::OpenL") );
    __ASSERT_DEBUG( iSessionStatus == ESessionIdle, gPanic( EMmsAlreadyBusy ) );

    iError = KErrNone;
    iRequestStatus = &aStatus;

    // 
    // Setting max send/receive sizes. 0 means not limited.
    // Values always come from CMmsSettings.
    // 
    iMaxReceiveSize = aMaxReceiveSize;
    iMaxSendSize = aMaxSendSize;
  
    // Get pointer HTTP session headers
    iSessionHeaders = iHTTPSession.RequestSessionHeadersL();
    iHeadersCreated = ETrue;

    // Check if we are connected already
    if ( iConnected )
        {
        aStatus = KRequestPending;                             
        User::RequestComplete( iRequestStatus, KMmsErrorSessionAlreadyOpen );
        }
    else 
        {

        // set the session properties
        // the handle comes from iHTTP session. I think we should not close it.
        RStringPool strPool = iHTTPSession.StringPool();
        RHTTPConnectionInfo connInfo = iHTTPSession.ConnectionInfo();
        connInfo.SetPropertyL( strPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ),
            THTTPHdrVal( iSocketServ->Handle() ) );
        TInt connPtr = REINTERPRET_CAST( TInt, iConnection );
        connInfo.SetPropertyL(
            strPool.StringF( HTTP::EHttpSocketConnection, RHTTPSession::GetTable() ),
            THTTPHdrVal( connPtr ) );

        // Add proxy usage if requested
        if ( aUseProxy )
            {
            THTTPHdrVal proxyUsage( strPool.StringF( HTTP::EUseProxy, RHTTPSession::GetTable() ) );
            connInfo.SetPropertyL(
                strPool.StringF( HTTP::EProxyUsage, RHTTPSession::GetTable() ), proxyUsage );
        
            RStringF proxyString = strPool.OpenFStringL( aProxyAddress );
            CleanupClosePushL( proxyString );
            THTTPHdrVal proxyAddr( proxyString );
            connInfo.SetPropertyL(
                strPool.StringF( HTTP::EProxyAddress, RHTTPSession::GetTable() ), proxyAddr );
            CleanupStack::PopAndDestroy( &proxyString );
            }
            
		// Install MMS X-id header filter if required
    	FeatureManager::InitializeLibL();
    	if ( FeatureManager::FeatureSupported( KFeatureIdMmsXidHeaderHttpFilter ) )  
	        {
	        // Install the MMS X-id header filter explicitly, since only MMS requires this filter.
			const TUid KHttpFilterMmsXidHeaderImplementationUid = { 0x1020738D };
			CEComFilter::InstallFilterL(iHTTPSession, KHttpFilterMmsXidHeaderImplementationUid);
	        }
    	FeatureManager::UnInitializeLib();
    	
        // Install UAProf filter
        TInt error = KErrNotFound;
        TRAP( error, CHttpUAProfFilterInterface::InstallFilterL( iHTTPSession ) );
        if ( error != KErrNone )
            {
            LOG2( _L("ERROR: UAProf filter left with code %d. Using hardcoded UserAgent string.."), error );
            
            // Manually set UA string
            SetFixedUserAgentL();

            // UserAgent header must be added always - either hard coded string or string from ini file
            SetHeaderL( iSessionHeaders, HTTP::EUserAgent, iUserAgent->Des() );

            // Add UAProf if available
            if ( iUaProf && iUaProf->Length() > 0 )
                {
                RStringF tempString = strPool.OpenFStringL( iUaProf->Des() );
                CleanupClosePushL( tempString );
                RStringF tempString2 = strPool.OpenFStringL( KXProfile );
                CleanupClosePushL( tempString2 );
                iSessionHeaders.SetFieldL( tempString2, tempString );
                CleanupStack::PopAndDestroy( &tempString2 );
                CleanupStack::PopAndDestroy( &tempString );
                }
            }

        // Accept headers moved to transaction as global filter overrides
        // session headers

        // HTTP session, no need for separate connect procedure
        iConnected = ETrue;
        // We may or may not get
        iHTTPSession.SetSessionEventCallback( this );
        aStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, KErrNone );
        }
    }

// ---------------------------------------------------------
// CMmsSession::SendMessageL
// ---------------------------------------------------------
//
EXPORT_C void CMmsSession::SendMessageL( 
    const TDesC& aUri,
    CBufFlat& aMessageBuffer,                            
    MMmsCodecDataSupplier&  aDataSupplier,
    MMmsCodecDataSink&      aDataSink,
    TRequestStatus& aStatus )
    {
    LOG( _L("CMmsSession::SendMessageL ") );
    __ASSERT_DEBUG( iSessionStatus == ESessionIdle, gPanic(EMmsAlreadyBusy) );
  
    TInt method = HTTP::EPOST; 
   
    iDataSupplier = &aDataSupplier;
    iDataSink = &aDataSink;
    iRequestStatus = &aStatus;

    // Check if the buffer is empty

    if ( aMessageBuffer.Size() == 0 )
        {
        aStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, KMmsErrorBufferEmpty );
        }

    // Check if we are connected
    
    else if ( !iConnected )
        {        
        aStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, KMmsErrorSessionNotOpen );
        }
       
    else
        {
        // Start the sending procedures
       
        if ( !iTransaction )
            {
            iTransaction = CMmsTransaction::NewL();
            }

        iTransaction->ExecuteL(
            iHTTPSession,
            *iTransferControl,
            aUri,
            method,
            aMessageBuffer,
            iTransactionTimeout,
            iMaxReceiveSize,
            0, // "expected receive size" not needed for sending
            *iDataSupplier,
            *iDataSink,
            iStatus );
        
        aStatus = KRequestPending;
        
        iSessionStatus = ESessionSending;

        SetActive();
        }
    }

// ---------------------------------------------------------
// CMmsSession::FetchMessageL
// ---------------------------------------------------------
//
EXPORT_C void CMmsSession::FetchMessageL(
    const TDesC& aUri,
    CBufFlat& aMessageBuffer,
    TInt32 aExpectedReceiveSize,
    MMmsCodecDataSupplier&  aDataSupplier,
    MMmsCodecDataSink&      aDataSink,
    TRequestStatus& aStatus )
    {
    LOG( _L("CMmsSession: FetchMessageL ") );
    __ASSERT_DEBUG( iSessionStatus == ESessionIdle, gPanic(EMmsAlreadyBusy) );

    TInt method = HTTP::EGET; 
    
    iDataSupplier = &aDataSupplier;
    iDataSink = &aDataSink;
    iRequestStatus = &aStatus;

    // Check if we are connected or not.
    if ( !iConnected )
        {  
        aStatus = KRequestPending;
        User::RequestComplete( iRequestStatus, KMmsErrorSessionNotOpen );
        }
    else
        {
        // Start the fetching procedures
        if ( !iTransaction )
            {
            iTransaction = CMmsTransaction::NewL();
            }

        iTransaction->ExecuteL(
            iHTTPSession,
            *iTransferControl,
            aUri,
            method,
            aMessageBuffer,
            iTransactionTimeout,
            iMaxReceiveSize,
            aExpectedReceiveSize,
            *iDataSupplier,
            *iDataSink,
            iStatus );
        
        aStatus = KRequestPending;
        iSessionStatus = ESessionFetching;
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMmsSession::RunL
// ---------------------------------------------------------
//
void CMmsSession::RunL()
    {
    // If WAP stack has terminated, we cannot continue
    if ( iStatus.Int() == KErrServerTerminated )
        {
        iSessionStatus = ESessionIdle;
        iConnected = EFalse;

        LOG2( _L("CMmsSession::RunL: Completing with %d"), iStatus.Int() );
        User::RequestComplete( iRequestStatus, iStatus.Int() );
        return;
        }
    
    switch ( iSessionStatus )
        {
       
        case ESessionSending:          
        case ESessionFetching:
            if ( iStatus == KErrDisconnected )
                {
                iConnected = EFalse;
                // Store the error returned by connect operation to be further returned
                // to server mtm after resources are freed 
                iError = iStatus.Int();
                }
            break;
               
        case ESessionIdle:    
        default:
            // We should not get here
            LOG( _L("CMmsSession::RunL: Illegal status") );
            iStatus = KErrUnknown;
            break;
        }
    iSessionStatus = ESessionIdle;
    LOG2( _L("CMmsSession::RunL: Completing with %d"), iStatus.Int() );
    User::RequestComplete( iRequestStatus, iStatus.Int() );
    }

// ---------------------------------------------------------
// CMmsSession::DoCancel
// ---------------------------------------------------------
//
void CMmsSession::DoCancel()
    {
    // Cancel whatever is being processed
    switch( iSessionStatus )
        {
        case ESessionSending:
        case ESessionFetching:
            iTransaction->Cancel();
            iSessionStatus = ESessionIdle;                  
            break;
        case ESessionIdle:
            // server has terminated if we get here - nothing to do.
            break;
        default:
            // Severe error if here
            LOG( _L("CMmsSession::DoCancel: Illegal status") );
            iStatus = KErrUnknown;
            break;
        }
    // Complete caller with cancel status
    User::RequestComplete( iRequestStatus, KErrCancel );
    }

// ---------------------------------------------------------
// CMmsSession::SetFixedUserAgentL
//
// ---------------------------------------------------------
//
void CMmsSession::SetFixedUserAgentL()
    {
    LOG( _L("CMmsSession::SetFixedUserAgentL") );
    // Use hardcoded UA information
    // Note: this is for testing purposes if uaprof filter keeps failing
    if ( !iUserAgent || iUserAgent->Length() == 0 )
        {
        // static useragent string
        if ( iUserAgent )
            {
            delete iUserAgent;
            iUserAgent = NULL;
            }
        iUserAgent = HBufC8::NewL( sizeof( KMmsDefaultUserAgent ) );
        iUserAgent->Des().Copy( KMmsDefaultUserAgent );
        }
    }

// ---------------------------------------------------------
// CMmsSession::SetHeaderL
//
// ---------------------------------------------------------
//
void CMmsSession::SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue )
    {
    RStringF valStr = iHTTPSession.StringPool().OpenFStringL( aHdrValue );
    THTTPHdrVal val( valStr );
    CleanupClosePushL( valStr );
    aHeaders.SetFieldL(
        iHTTPSession.StringPool().StringF( aHdrField, RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr );
    }

// ---------------------------------------------------------
// CMmsSession::MHFSessionRunL
//
// ---------------------------------------------------------
//
void CMmsSession::MHFSessionRunL( const THTTPSessionEvent& aEvent )
    {
    // We should not get events here.
    LOG2( _L("CMmsSession::MHFSessionRunL event %d"), aEvent.iStatus );
    iHTTPEvent = aEvent;
    }
        
// ---------------------------------------------------------
// CMmsSession::MHFSessionRunError
//
// ---------------------------------------------------------
//
TInt CMmsSession::MHFSessionRunError(
    TInt aError,
    const THTTPSessionEvent& aEvent )
    {
    LOG2( _L("CMmsSession::MHFSessionRunError %d"), aError );
    iHTTPEvent = aEvent;
    iError = aError;
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

