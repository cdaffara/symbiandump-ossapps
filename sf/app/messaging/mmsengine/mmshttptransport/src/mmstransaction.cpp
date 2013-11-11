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
*     Class implementing transaction within WAP session
*
*/



// INCLUDE FILES
#include    <uriutils.h>
#include    <httpstringconstants.h>
#include    <uaproffilter_interface.h>

#include    "mmstransaction.h"
#include    "mmsservercommon.h"
#include    "mmsconst.h"
#include    "mmssession.h"
#include    "mmserrors.h"
#include    "MmsServerDebugLogging.h"
#include    "mmscodecdatasupplier.h"
#include    "mmscodecdatasink.h"


// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic(TMmsPanic aPanic);

// CONSTANTS
const TInt KMmsMaxRetryCount = 4;
// HTTP error ranges
const TInt KMmsHTTP100 = 100; // 1xx informative
const TInt KMmsHTTP200 = 200; //
const TInt KMmsHTTP300 = 300; //
const TInt KMmsHTTP400 = 400; //
const TInt KMmsHTTP500 = 500; //
const TInt KMmsHTTP404 = 404; // Not Found
const TInt KMmsHTTP413 = 413; // Request entity too large

const TInt KMmsReserve = 10000; // extra for buffer
    
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ==================== LOCAL FUNCTIONS ====================
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsTransaction
// ---------------------------------------------------------
//
CMmsTransaction::CMmsTransaction()
    : CMsgActive ( KMmsActiveObjectPriority ),
    iState ( EMmsIdle ),
    iUri ( NULL ),
    iHTTPSession( NULL ),
    iTimer( NULL ),
    iTransferControl( NULL ),
    iTransactionOpened( EFalse )
    {
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMmsTransaction::ConstructL()
    {
    iTimer = CMmsOperationTimer::NewL();
    CActiveScheduler::Add( this );    
    }

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CMmsTransaction* CMmsTransaction::NewL()
    {
    CMmsTransaction* self = new (ELeave) CMmsTransaction();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

    
// ---------------------------------------------------------
// ~CMmsTransaction
// ---------------------------------------------------------
//
CMmsTransaction::~CMmsTransaction()
    {
    Cancel();
    if ( iTransactionOpened )
        {
        RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
        hdr.RemoveAllFields(); // clean up
        iTransaction.Close();
        }
    delete iUri;
    delete iTimer; 
    }

// ---------------------------------------------------------
// ExecuteL
// ---------------------------------------------------------
//
void CMmsTransaction::ExecuteL( 
    RHTTPSession& aSession,    
    CMmsBearerStatus& aTransferControl,
    const TDesC& aUri,
    const TInt aMethod,
    CBufFlat& aMessageBuffer,
    const TInt aTransactionTimer,
    TInt32 aMaxReceiveSize,
    TInt32 aExpectedReceiveSize,
    MMmsCodecDataSupplier& aDataSupplier,
    MMmsCodecDataSink& aDataSink,
    TRequestStatus& aStatus )
    {
    LOG( _L("CMmsTransaction::ExecuteL") );
    __ASSERT_DEBUG( iState == EMmsIdle, gPanic( EMmsAlreadyBusy ) );
    
    iError = KErrNone;
  
    iHTTPSession = &aSession;
    iDataSupplier = &aDataSupplier;
    iDataSink = &aDataSink;

    iMaxReceiveSize = aMaxReceiveSize;
    if( aMethod == HTTP::EGET )
        {
        //
        // Setting adequate receivebuffer size for receive transactions
        //
        TInt32 reserve = KMmsReserve; // fixed reserve for unexpected surplus data
        if ( aExpectedReceiveSize == KMmsChunkedBufferSize )
            {
            // we don't need extra when receiving in chunked mode
            reserve = 0;
            }
        if( ( aExpectedReceiveSize + reserve ) > aMaxReceiveSize )
            {
            iExpectedReceiveSize = aMaxReceiveSize;
            }
        else
            {
            iExpectedReceiveSize = aExpectedReceiveSize + reserve;
            }
        }
    iTransactionOpened = EFalse;

    delete iUri;
    iUri = NULL;
    HBufC* newUri = aUri.AllocL();
    iUri = newUri;    
    
    iBuffer = &aMessageBuffer;
    iMethod = aMethod;
    
    iTransactionTimeout = aTransactionTimer;
    iTransferControl = &aTransferControl;

    // clear the flags, because we are reusing the class,
    // we don't create a new instance every time
    iRetryCount = 0;
    iRequestOngoing = EFalse;
    iSuspendOccurred = EFalse;
    iGotBodyData = EFalse;
    iDataChunkNumber = 0; // no data obtained yet
    iCumulativeSize = 0; // received size or sent size, cumulative
    iChunkSize = 0;
    iEvent = THTTPEvent::ESubmit;

    Queue( aStatus );   // aStatus = iStatus = KRequestPending

    //
    // Start transaction timer
    //
    if ( iTimer->IsActive() )
        {
        iTimer->Cancel();
        }
    iTimer->Start( (MMmsTransportObserver*) this, iTransactionTimeout );

    //
    // If bearer is already blocked when starting, execution enters a wait
    //
    if( iTransferControl->IsSuspended() )
        {
        LOG( _L(" - GPRS is Suspended -> waiting for resume before sending") );

        // Transfer cancellation
        iTransferControl->SubscribeNotification( (MMmsTransportObserver*) this );
        // Start needed operations in suspend
        GprsSuspended();

        // someone must set our status to KRequestPending
        iStatus = KRequestPending;

#ifndef _NO_MMSS_LOGGING_
        if ( IsActive() )
            {
            LOG( _L(" - already active") );
            }
#endif  //_NO_MMSS_LOGGING_
        SetActive();
        }
    else // Bearer not blocked, continuing directly
        {
        iStatus = KRequestPending;
        SetActive();    
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }
        

// ---------------------------------------------------------
// RunL
// ---------------------------------------------------------
//
void CMmsTransaction::RunL()
    {
    // DoRunL() takes the AO through the states, queuing another
    // asynch step as required.
    // if DoRunL() detects the end of the cycle it returns
    // without queuing another cycle.

    // If Run() would exit without queuing another cycle it reports
    // completion to the client.
    // In CMsgActive this is true if the asynch step or DoRunL fails,
    // or the state cycle is complete
    // However, we must keep our cycle going in spite of error(s).
   
    // The CMmsOperationTimer reports the exceed situation with error
    // This must be handled in CMmsTransaction's DoRunL before returning to
    // client

    // Also the bearer suspend/resume status is informed via error
    // Also handled in CMmsTransaction. 

    // If we have completed ourselves, the error is already in iError.
    // However, if an asynchronous operation completed us, the error
    // comes back to us in iStatus.
    // As we always complete ourselves by returning iError as the
    // status, we get back either our original iError, or a new
    // error value set by an asynchronous operation.
    
    TInt status = iStatus.Int();
    
    if ( iError == KErrNone )
        {
        iError = status;
        }

    // Only cancel can exit.
    if ( status != KErrCancel || iError != KErrNone )
        {
        // iError contains the possible error from previous step.
     
        TRAPD( error, DoRunL() );    // continue operations, may re-queue
        // must not requeue in error situations
        // If we want to continue the loop in spite of error (yes, sir),
        // we must not leave, but return the error in iError.
        // Symbian code may leave, so we must trap the leaves here,
        // as we want to keep the loop going unless something
        // is really fatally wrong (we are completely out of memory,
        // or the system has crashed)
        __ASSERT_DEBUG( error == KErrNone || !IsActive(), User::Invariant() );
        // active means AO has been requeued, just return
        if ( IsActive() )             
            {
            return;
            }

        status = error;
        if ( error == KErrNone )
            {
            // If DoRunL did not leave, possible error is in iError
            status = iError;
            }
        }
    LOG2( _L("CMmsTransaction::RunL: Complete with %d"), status );
    Complete( status );
    }

// ---------------------------------------------------------
// DoRunL
// ---------------------------------------------------------
//
void CMmsTransaction::DoRunL()
    {
    //
    // This routine takes the state machine through the states
    // until an error is encountered or the cycle completes.
    //
    LOG3( _L("CMmsTransaction::DoRunL status %d, iError %d"), iStatus.Int(), iError );
    // If we got last chunk already, do not cancel, let HTTP complete
    if ( iError != KErrNone && iError != KMmsErrorBearerSuspended )
        {
        if ( iRequestOngoing )
            {
            iTransaction.Cancel();
            iRequestOngoing = EFalse;
            }
        // Error code copied to status code
        iStatus = iError;
        }

    // We continue to next state to finish properly
    SelectNextState();
    // If appropriate, ChangeState makes us active again
    ChangeStateL();
    }

// ---------------------------------------------------------
// SelectNextState
// ---------------------------------------------------------
//
void CMmsTransaction::SelectNextState()
    {
    //
    // The purpose of the switch statements is to cycle through
    // states. These states handle certain task(s) and after
    // completed CMmsTransaction is finished.
    //
    switch ( iState )
        {
        case EMmsIdle:
            // start the loop
            iState = EMmsSendingTransactionRequest;
            break;
        case EMmsSendingTransactionRequest:
            iState = EMmsFinished;            
            break;
        case EMmsFinished:
            // No more states
            iError = KErrUnknown;           
            break;
        default:
            // Illegal state, we should never get here
            iError = KErrUnknown;
            break;
        }  
    }
    
// ---------------------------------------------------------
// ChangeStateL
// ---------------------------------------------------------
//
void CMmsTransaction::ChangeStateL()
    {
    switch ( iState )
        {
        case EMmsSendingTransactionRequest:
            MakeTransactionL();
            break;
        
        case EMmsFinished:
            FinishL();
            break;
 
        default:
            // All other states are illegal
            LOG( _L("CMmsTransaction::ChangeStateL(): ERROR: Illegal state") );
            iError = KErrUnknown;
            break;
        }    
    }

// ---------------------------------------------------------
// MakeTransactionL
// ---------------------------------------------------------
//
void CMmsTransaction::MakeTransactionL()
    {
    LOG( _L("CMmsTransaction::MakeTransactionL") );

    // Useragent has been added to session headers:
    // It should always be present
    HBufC8* uri = HBufC8::NewL( iUri->Length() );
    CleanupStack::PushL( uri );
    uri->Des().Copy( iUri->Des() );
    TUriParser8 uri2;
    uri2.Parse( uri->Des() );

    RStringPool stringPool = iHTTPSession->StringPool();

    iTransaction = iHTTPSession->OpenTransactionL( uri2,
        *this,
        stringPool.StringF( iMethod, RHTTPSession::GetTable() ) );

    iTransactionOpened = ETrue;
    CleanupStack::PopAndDestroy( uri );

    // Set headers. If GET, no content type needed as no content present
    // common accept headers are set as session headers
    // these are done only on the first time. In retry cases, they already exist.
    RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();

    // Adding accept headers has been moved to transaction,
    // as automatic filters seemed to override session headers
    // (session headers are not added if transaction headers already exist)

    // NOTE: Currently Accept header contains the following line on purpose:
    // "Accept: */*, application/vnd.wap.mms-message, application/vnd.wap.sic"
    // in order to be compliant with some server vendors.

    // Accept: "*/*"
    SetHeaderL( hdr, HTTP::EAccept, KMmsAny );
    // Accept: "application/vnd.wap.mms-message"
    SetHeaderL( hdr, HTTP::EAccept, KMmsMessageContentType );
    // Accept: "application/vnd.wap.sic"
    SetHeaderL( hdr, HTTP::EAccept, KMmsWapSicContentType );

    // Accept-Language: en
    SetHeaderL( hdr, HTTP::EAcceptLanguage, KHTTPEnglish );

    // Accept-Charset: utf-8
    SetHeaderL( hdr, HTTP::EAcceptCharset, KHTTPUtf8 );
    // end of headers moved from session to transaction

    if( iMethod == HTTP::EPOST )
        {
        // Add contenttype header 
        SetHeaderL( hdr, HTTP::EContentType, KMmsMessageContentType );

        // Give payload supplier for stack in case of http post
        iTransaction.Request().SetBody( *this );
        }

    // Set the 'Host:' header
    SetHostHeaderL( hdr );

    // Set UA headers
    RStringF uaClient = iHTTPSession->StringPool().OpenFStringL( KUserAgentClientId );
    CleanupClosePushL<RStringF>( uaClient );
    iTransaction.PropertySet().SetPropertyL( uaClient, THTTPHdrVal( KUserAgentCliMMS ) );
    CleanupStack::PopAndDestroy( &uaClient );
    
    // no response headers yet
    iGotResponseHeaders = EFalse; 

    // Perform the actual sending
    iTransaction.SubmitL();
    
    // Start observer to monitor notifications
    iTransferControl->SubscribeNotification( (MMmsTransportObserver*) this );
    
    iRequestOngoing = ETrue;

    iStatus = KRequestPending;
    SetActive();
    }

// ---------------------------------------------------------
// FinishL
// ---------------------------------------------------------
//
void CMmsTransaction::FinishL()
    {
    LOG( _L("CMmsTransaction::FinishL") );

    //
    // Cancel timer and observer
    //
    iTransferControl->Cancel();

    //
    // Completion came from own timers
    //
    if( iStatus == KMmsErrorTimeout || iStatus == KMmsErrorSuspendTimeout )
        {
        LOG( _L(" - MmsEngine's timer expired") );
        if( iRequestOngoing )
            {
            iTransaction.Cancel();
            }

        if( iGotBodyData && iLastChunk )
            {
            LOG( _L(" - Data has been received, returning OK") );
            iError = KErrNone;
            }
        else // no body data, emptying 
            {
            iBuffer->Reset();
            }
        return;
        }

    //
    // Pause the timer (later it will be put back on, or cancelled)
    //
    iTimer->Pause();

    //
    // Completion came from HTTP stack
    // go through switch based on event status
    //
    switch ( iEvent.iStatus )
        {
        // Successful case
        case THTTPEvent::ESucceeded:
            {
            iRetryCount = 0;
            iSuspendOccurred = EFalse;
            iError = KErrNone; // successful.
            LOG( _L(" - HTTP status OK") );
            // If there was response data, we've already got it
            break;
            }

        // Failure cases
        case THTTPEvent::EFailed:
            {
            LOG2( _L(" - HTTP failed with eventcode %d"), iEvent.iStatus );

            //
            // Check for suspend cases:
            // Errors due to GPRS suspend are handled either with immediate retry
            // or immediatedly after suspend has resumed. Anyway, attempt is not given up.
            //
            if( ( iTransferControl->IsSuspended() || iSuspendOccurred ) &&
                !iGotResponseHeaders && iRetryCount < KMmsMaxRetryCount )
                {
                // If we haven't got any response headers yet, no need to reset the
                // data sink as it has not been given any data yet
                iRetryCount++;
                iTransaction.Close();
                iTransactionOpened = EFalse; // closed already
                iRequestOngoing = EFalse;
                iState = EMmsIdle;
                iError = KErrNone;
                iSuspendOccurred = EFalse;
                
                if( !iTransferControl->IsSuspended() ) // i.e. iSuspendOccurred
                    {
                    LOG( _L(" - Suspend has occurred, but has resumed -> trying again immediatedly!") );
                    TRequestStatus* status = &iStatus; 
                    User::RequestComplete( status, KErrNone );
                    iTimer->Continue();
                    SetActive();
                    return;
                    }
                else // GPRS is now suspended
                    {
                    LOG( _L(" - GPRS is now Suspended -> waiting for resume before retry.") );
                    // Subscribe notifications from transferControl watcher
                    // Also starting timer
                    iTransferControl->SubscribeNotification( (MMmsTransportObserver*) this );
                    iTimer->Continue();

                    // Not completing here.. it will be done by the observer or the timer
                    iStatus = KRequestPending;
                    SetActive();
                    return;
                    }
                }

            //
            // For some reason connection to network has been cut off after the first transaction.
            // The following code tries immediate resend for the transaction if this has been the case.
            // 
            if( iStatus == KErrDisconnected     // IF connection has been cut off
                && iRetryCount < 1 )            // AND no retries have been tried yet
                {
                LOG( _L(" - Connection has been disconnected, re-submitting.") );

                iRetryCount++;
                iState = EMmsSendingTransactionRequest;

                iCumulativeSize = 0;
                // reset data sink in case we got partial data.
                // If the data sink is not in chunked mode, this does nothing
                iDataSink->ResetDataSink();
                // Just re-submitting the transaction
                iTransaction.SubmitL();

                iRequestOngoing = ETrue;
                iError = KErrNone;
                
                // Start timer running
                iTimer->Continue();
                // start bearer observer
                iTransferControl->SubscribeNotification( (MMmsTransportObserver*) this );

                iStatus = KRequestPending;
                SetActive();
                return;
                }
                
            //
            // Timer can now be cancelled
            //
            iTimer->Cancel();

            RHTTPResponse resp = iTransaction.Response();
            LOG2( _L(" - HTTP status code %d "), resp.StatusCode() );

            TInt error = KErrNone;
            // crude mapping only based on first number
            const TInt KMms100 = 100;
            switch ( ( resp.StatusCode() / KMms100 ) * KMms100 )
                {
                case KMmsHTTP100: // 1xx Informative
                    // We map "informative" to KErrNone.
                    // Further analysis is needed to tell if there was a real error or not
                    // Decoding of response from MMSC will determine the final error.
                    error = KErrNone;
                    break;
                case KMmsHTTP200: // 2xx OK
                    error = KErrNone;
                    break;
                case KMmsHTTP300: // 3xx Multiple choices
                    error = KMmsErrorHTTPConfiguration;
                    break;
                case KMmsHTTP400: // 4xx Client error
	                if( resp.StatusCode() == KMmsHTTP404 )
                        {
            	        error = KMmsErrorHTTPNotFound;
                        }
                    else if ( resp.StatusCode() == KMmsHTTP413 )
                        {
                        error = KMmsErrorEMRUExceeded;
                        }
                    else
                        {
                        error = KMmsErrorHTTPClientError;
                        }
                    break;
                case KMmsHTTP500: // 5xx Server error
                    error = KMmsErrorHTTPServerDown;
                    break;
                default:
                    error = KErrUnknown;
                    break;
                }
            
            // If we get clear HTTP error code, we use it.
            // Usually iError is KErrUnknown at this point if we have got an error event.
            if ( error != KErrNone )
                {
                // If we have some previous error, KErrUnknown does not override
                if ( error != KErrUnknown || iError == KErrNone )
                    {
                    iError = error;
                    }
                }

            TBool mmsMessage = EFalse; // check if response data is of correct type
            if( iGotResponseHeaders )
                {
                RHTTPHeaders hdr = resp.GetHeaderCollection();
                RStringPool stringPool = iHTTPSession->StringPool();
                RStringF contentType = stringPool.StringF(
                    HTTP::EContentType, RHTTPSession::GetTable() );
                THTTPHdrVal fieldVal;
                if( hdr.GetField( contentType, 0, fieldVal ) == KErrNone )
                    {
                    // content type must match with KMmsMessageContentType
                    RStringF valStr = stringPool.OpenFStringL( KMmsMessageContentType );
                    CleanupClosePushL( valStr );
                    if ( valStr == fieldVal.StrF() )
                        {
                        mmsMessage = ETrue;
                        }
                    CleanupStack::PopAndDestroy( &valStr );
                    }
                }

            // If we got body data, we clear error in order to make
            // mms server mtm to decode what we got.
            // - But only if the content type really was "application/vnd.wap.mms-message"
            // It is possible that the message is incomplete, but in some cases
            // HTTP stack is not sure if it got all the data or not, so we must check
            // if what we got can be decoded without problems.
            // Because chunked decoding would cause problems otherwise,
            // We can declare "no error" only if we got the last chunk
            if( iGotBodyData && mmsMessage && iLastChunk )
                {
                iError = KErrNone;
                }
            break;
            }

        default:
            LOG( _L("CMmsTransaction::FinishL(): Unknown response") );
            // Other event = error
            // - but don't override the error if we already got something.
            if ( iError == KErrNone )
                {
                iError = KMmsErrorUnknownRespFromGw;
                }
            break;
         }
    
    iSuspendOccurred = EFalse;
    iRequestOngoing = EFalse;
    iStatus = iError;
    iState = EMmsIdle;
    }

// ---------------------------------------------------------
// DoCancel
// ---------------------------------------------------------
//
void CMmsTransaction::DoCancel()
    {
    LOG( _L("CMmsTransaction::DoCancel") );
    if ( iRequestOngoing )
        {
        iTransaction.Cancel();
        iRequestOngoing = EFalse;
        }
    iTimer->Cancel();
    iTransferControl->Cancel();
    iState = EMmsIdle;
    
    // Complete ourselves as no-one else is going to do it
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrCancel );

    // This completes the caller (session)
    CMsgActive::DoCancel();
    }

// ---------------------------------------------------------
// DoComplete
// ---------------------------------------------------------
//
void CMmsTransaction::DoComplete( TInt& )
    {
    // just to be sure that nothing is left active, cancel
    // all operations that may still be pending.
    LOG( _L("CMmsTransaction::DoComplete") );

    iTimer->Cancel();
    iTransferControl->Cancel();

    // transaction is automatically cancelled during close
    if ( iTransactionOpened )
        {
        RHTTPHeaders hdr = iTransaction.Request().GetHeaderCollection();
        hdr.RemoveAllFields(); // clean up
        iTransaction.Close();
        iTransactionOpened = EFalse; // closed already
        }
    if ( iDataSink )
        {
        // release data sink does nothing if there is nothing to release
        // it is safe to call it.
        iDataSink->RelaseDataSink();
        iDataSink = NULL;
        }
        
    // in case we have run out of memory or code leaves for some other
    // fatal reason, we make sure we are idle for the next loop.
    iState = EMmsIdle;
    }
    
// ---------------------------------------------------------
// TimerExpired
// ---------------------------------------------------------
//
void CMmsTransaction::TimerExpired()
    {
    LOG( _L("CMmsTransaction::TimerExpired") );
    // No transfer cancellation any more
    iTransferControl->Cancel();

    if( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        }

    // Setting the state so that we end up to FinishL() method
    iState = EMmsSendingTransactionRequest;

    // We complete with MMS Engine's own Timeout value in order 
    // to avoid confusion with timeouts coming from lower layers
    TRequestStatus* status = &iStatus; 
    User::RequestComplete( status, KMmsErrorTimeout );
        
    iRetryCount = 0;
    }

// ---------------------------------------------------------
// GprsSuspended
// ---------------------------------------------------------
//
void CMmsTransaction::GprsSuspended()
    {
    LOG( _L("CMmsTransaction::GprsSuspended") );
    }
    
// ---------------------------------------------------------
// GprsResumed
// ---------------------------------------------------------
//
void CMmsTransaction::GprsResumed()
    {
    //
    // GPRS context has resumed
    //
    LOG( _L("CMmsTransaction::GprsResumed") );
    if( iRequestOngoing )
        {
        // Set flag indicating that gprs was suspended at some stage
        iSuspendOccurred = ETrue;
        iTransferControl->SubscribeNotification( (MMmsTransportObserver*) this );
        }
    else
        {
        // No request ongoing yet, start state machine by completing ourselves
        iTransferControl->Cancel();
        TRequestStatus* status = &iStatus; 
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------
// TransferCancelled
// ---------------------------------------------------------
//
void CMmsTransaction::TransferCancelled()
    {
    LOG( _L("CMmsTransaction::TransferCancelled") );
    iTimer->Cancel();
    
    // It is possible that this method is called when there is no
    // transaction going i.e. object is not active.
    // It means that in that case we have complete ourselves 
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus; 
        User::RequestComplete( status, KMmsErrorTransferCancelled );
        }
    else
        {
        // cancel getting events from the stack
        if ( iRequestOngoing )
            {
            iTransaction.Cancel();
            iRequestOngoing = EFalse;
            }
        iError = KMmsErrorTransferCancelled;
        TRequestStatus* status = &iStatus; 
        User::RequestComplete( status, KMmsErrorTransferCancelled );
        }    
    }

// ---------------------------------------------------------
// SetHeaderL
// ---------------------------------------------------------
//
void CMmsTransaction::SetHeaderL( RHTTPHeaders aHeaders, TInt aHdrField, const TDesC8& aHdrValue )
    {
    RStringF valStr = iHTTPSession->StringPool().OpenFStringL( aHdrValue );
    THTTPHdrVal val( valStr );
    CleanupClosePushL( valStr );
    aHeaders.SetFieldL(
        iHTTPSession->StringPool().StringF( aHdrField, RHTTPSession::GetTable() ), val );
    CleanupStack::PopAndDestroy( &valStr );
    }

// ---------------------------------------------------------
// SetHostHeaderL
// ---------------------------------------------------------
//
void CMmsTransaction::SetHostHeaderL( RHTTPHeaders aHeaders )
    {
    LOG( _L("CMmsTransaction::SetHostHeaderL()") );
    //
    // Get needed URI parts
    //
    TUriC8 uri3 = iTransaction.Request().URI();
    TPtrC8 hostPtr = uri3.Extract(EUriHost); 
    TPtrC8 portPtr = uri3.Extract(EUriPort); 

    // If empty, nothing can be done
    if( hostPtr.Length() == 0 )
        {
        return;
        }

    //
    // If host-type == IPv6 -> include '[' and ']' characters to URI
    //
    UriUtils::TUriHostType hosttype = UriUtils::HostType( hostPtr );
    if( hosttype == UriUtils::EIPv6Host )
        {
        LOG( _L(" - host type == IPv6") );
        const TInt KMmsRoomForBrackets = 2;
        hostPtr.Set( hostPtr.Ptr()-1, hostPtr.Length() + KMmsRoomForBrackets );
        }

    //
    // If port not equal to 80 -> include it
    //
    _LIT8( KMmsPort, "80" );
    if ( portPtr.Length() > 0 && portPtr.Compare( KMmsPort ) )
        {
        hostPtr.Set( hostPtr.Ptr(), hostPtr.Length() + 1 + portPtr.Length() );
        }

    //
    // Remove possible old host-header and set new one
    //
    aHeaders.RemoveField( iHTTPSession->StringPool()
        .StringF( HTTP::EHost, RHTTPSession::GetTable() ) );
    SetHeaderL( aHeaders, HTTP::EHost, hostPtr );
    }

// ---------------------------------------------------------
// MHFRunL
// ---------------------------------------------------------
//
void CMmsTransaction::MHFRunL( RHTTPTransaction aTransaction, const THTTPEvent& aEvent )
    {
    TRequestStatus* status = &iStatus;
    iEvent = aEvent;

    // Switch through the events
    switch (aEvent.iStatus)
        {
        case THTTPEvent::EGotResponseHeaders:
            LOG( _L("CMmsTransaction::MHFRunL: Got response headers") );
            {
            // Clear buffer but do not compress
            // We get the data in small chunks, and reallocating all the time
            // may make us to run out of memory too soon.
            iBuffer->Delete( 0, iBuffer->Size() );
            iBuffer->ResizeL( 0 );
            iGotResponseHeaders = ETrue; // Got response headers, can check content type
            iDataChunkNumber = 0; // reset in case of retry
            } 
            break;

        case THTTPEvent::EGotResponseBodyData:
            {
            LOG( _L("CMmsTransaction::MHFRunL: Got response body data") );
            // Get the data
            TPtrC8 bodyData;
            MHTTPDataSupplier* dataSupplier = aTransaction.Response().Body();
            iLastChunk = EFalse;
            iLastChunk = dataSupplier->GetNextDataPart( bodyData );
            
            // We must remove CR and LF characters from the beginning of
            // the first data chunk representing message body because some
            // network proxies seem to add an extra CR-LF between headers and body.
            // (A well formed MMS PDU always begins with 0x8C, so that we 
            // never lose anything from a well formed PDU, and if the PDU
            // is not well formed, it is a mess anyway.)
            if ( iDataChunkNumber == 0 )
                {
                iPosition = 0;
                iCumulativeSize = 0; // first data chunk, start counting from here
                TInt overallDataSize = dataSupplier->OverallDataSize();
#ifndef _NO_MMSS_LOGGING_
                if ( iMethod == HTTP::EGET && iMaxReceiveSize != 0)
                    {
                    LOG2( _L("- can accept only %d bytes"), iMaxReceiveSize );
                    }
                if ( overallDataSize != KErrNotFound )
                    {
                    LOG2( _L("- going to get at least %d bytes"), overallDataSize );
                    }
                else
                    {
                    LOG( _L("- chunked transfer encoding used") );
                    }
#endif  //_NO_MMSS_LOGGING_
                TInt removedChars = 0;
                while ( bodyData.Find( KCr8 ) == 0 || bodyData.Find( KLf8 ) == 0 )
                    {
#ifndef _NO_MMSS_LOGGING_
                    if ( bodyData.Find( KCr8 ) == 0 )
                        {
                        LOG( _L("- removed CR") );
                        }
                    else
                        {
                        LOG( _L("- removed LF") );
                        }
#endif  //_NO_MMSS_LOGGING_
                    // remove first character
                    bodyData.Set( bodyData.Mid( 1 ) );
                    removedChars++;
                    }

                //
                // Check if we are about to receive too much data
                //
                if ( iMethod == HTTP::EGET &&
                    iMaxReceiveSize != 0 &&
                    overallDataSize != KErrNotFound &&
                    overallDataSize - removedChars > iMaxReceiveSize )
                    {
                    // The overall data size is more than what we want
                    iError = KMmsErrorEMRUExceeded;
                    }

                //
                // Size buffer to max message size
                //
                if ( iMethod == HTTP::EGET && iError != KMmsErrorEMRUExceeded )
                    {
                    LOG2( _L("- reserving %d bytes for receiving."), iExpectedReceiveSize );
                    iBuffer->ResizeL( iExpectedReceiveSize );
                    iBuffer->ResizeL( 0 );
                    }
                } // (iDataChunkNumber == 0) block

            //
            // Add the chunk number counter every time body data is received
            //
            iDataChunkNumber++;

            // When sending MMS, message size is not checked
            // When receiving MMS, size is checked against iMaxReceiveSize
            // If iMaxReceiveSize == 0 -> no check
            // If size > iMaxReceiveSize: "KMmsErrorEMRUExceeded"
            iCumulativeSize += bodyData.Length();
            if( iMethod == HTTP::EGET && 
                iMaxReceiveSize > 0 && 
                iCumulativeSize > iMaxReceiveSize )
                {
                iError = KMmsErrorEMRUExceeded;
                }

            //
            // Check if we are getting too much data (compared to iMaxReceiveSize)
            //
            if ( iError == KMmsErrorEMRUExceeded )
                {
                // Release data and cancel transaction
                // The following two statements have to be executed in this order
                dataSupplier->ReleaseData();
                aTransaction.Cancel();

                // we say we did not get anything even if we might have fetched
                // part of the data
                iGotBodyData = EFalse;
                iDataSink->RelaseDataSink();

                // transaction was cancelled -> not getting any more events:
                User::RequestComplete( status, iError ); 
                }
            else // (iError not equal to KMmsErrorEMRUExceeded)
                {
                iBuffer->ResizeL( bodyData.Length() + iPosition );
                TInt currentData = bodyData.Length();
                iBuffer->Write( iPosition, bodyData, currentData );
                // release the databuffer
                dataSupplier->ReleaseData();
#ifndef _NO_MMSS_LOGGING_
                LOG3( _L("- chunk size %d (cumulative %d)"), currentData, iCumulativeSize );
                if ( iLastChunk )
                    {
                    LOG( _L("- last data chunk received!") );
                    }
#endif  //_NO_MMSS_LOGGING_

                iPosition = 0;
/////                
                iError = iDataSink->NextDataPart( *iBuffer, iPosition, iLastChunk );
////                
                // if something goes wrong when saving the data, the transaction should be
                // cancelled
                // But if we got last chunk already, let the HTTP complete normally
                if ( iError != KErrNone && !iLastChunk )
                    {
                    // We can only get an error here if we are in chunked mode
                    // cancel transaction
                    aTransaction.Cancel();
                    iDataSink->RelaseDataSink();

                    // transaction was cancelled -> not getting any more events:
                    User::RequestComplete( status, iError );
                    return; 
                    }
                
                TInt amount = iBuffer->Size() - iPosition;
                if ( iPosition != 0 )
                    {
                    // some data handled
                    iBuffer->Delete( 0, iPosition );
                    }
                iBuffer->ResizeL( amount );
                iPosition = amount; // continue writing from here

                if ( iLastChunk )
                    {
                    iDataSink->RelaseDataSink();
                    }

                // If we get at least some body data, we continue to decode it even if 
                // it might not be complete.
                // The HTTP session may disconnect too soon, and HTTP stack may report error
                // even if we already got all our body data.
                iGotBodyData = ETrue;
                }
            } 
            break;
        case THTTPEvent::EResponseComplete:
            {
            LOG( _L("CMmsTransaction::MHFRunL: response complete") );
            // never mind, ESucceeded or EFailed will follow
            break;
            }

        case THTTPEvent::ESucceeded:
            {
            LOG( _L("CMmsTransaction::MHFRunL: Succeeded") );
            LOG2( _L("- Got %d data chunks"), iDataChunkNumber );
            User::RequestComplete( status, KErrNone );
            break;
            }

        case THTTPEvent::EFailed:
            {
            LOG( _L("CMmsTransaction::MHFRunL: Failed") );
            LOG2( _L("- Got %d data chunks"), iDataChunkNumber );
            if ( iError == KErrNone )
                {
                iError = KErrUnknown;
                }
            User::RequestComplete( status, iError ); 
            break;
            }

        default:
            {
            LOG2( _L("CMmsTransaction::MHFRunL: unknown event %d"), aEvent.iStatus );
            // save the error, but don't override a possible earlier error
            if ( aEvent.iStatus < 0 && iError == KErrNone )
                {
                iError = aEvent.iStatus;
                }
            break;
            }
        }
    }

// ---------------------------------------------------------
// MHFRunError
// ---------------------------------------------------------
//
TInt CMmsTransaction::MHFRunError(
    TInt aError,
    RHTTPTransaction aTransaction, 
    const THTTPEvent& aEvent )
    {
    LOG2( _L("CMmsTransaction::MHFRunError: %d"), aError );
    iEvent = aEvent;
    iError = aError;
    aTransaction.Close();
    iTransactionOpened = EFalse;
    iDataSink->RelaseDataSink();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aEvent.iStatus ); 
    return KErrNone;
    }

// ---------------------------------------------------------
// GetNextDataPart
// ---------------------------------------------------------
//
TBool CMmsTransaction::GetNextDataPart( TPtrC8& aDataPart )
    {
    iLastChunk = EFalse;
    iError = iDataSupplier->GetNextDataPart( aDataPart, iLastChunk );
    iChunkSize = aDataPart.Size();
    LOG2( _L("CMmsTransaction::GetNextDataPart, Chunk size %d"), iChunkSize );
#ifndef _NO_MMSS_LOGGING_
    if ( iLastChunk )
        {
        LOG( _L("CMmsTransaction:: last chunk") );
        }
#endif  //_NO_MMSS_LOGGING_
    
    
    if ( iError != KErrNone )
        {
        // could not get data - cancel the transaction
        iTransaction.Cancel();
        }

    return iLastChunk;
    }

// ---------------------------------------------------------
// ReleaseData
// ---------------------------------------------------------
//
void CMmsTransaction::ReleaseData()
    {
    iCumulativeSize += iChunkSize;
    iChunkSize = 0;
    LOG2( _L("CMmsTransaction::ReleaseData, cumulative %d"), iCumulativeSize );
    
    TInt error = iDataSupplier->ReleaseData();
    if ( iError != KErrNone )
        {
        iError = error;
        }
    
    if ( iError != KErrNone )
        {
        // could not get more data - cancel the transaction
        iTransaction.Cancel();
        }
    else if ( !iLastChunk )
        {
        // If we have sent last chunk already, there is no more body data
        // If the function leaves, there is nothing we can do about it.
        // ReleaseData() must not leave.
        TRAP_IGNORE( iTransaction.NotifyNewRequestBodyPartL() );
        }
    // else nont needed - no operation    
    }

// ---------------------------------------------------------
// OverallDataSize
// ---------------------------------------------------------
//
TInt CMmsTransaction::OverallDataSize()
    {
    TInt dataSize = iDataSupplier->OverallDataSize();
    LOG2( _L("CMmsTransaction::OverallDataSize %d"), dataSize );
    
    return dataSize;
    }

// ---------------------------------------------------------
// Reset
// ---------------------------------------------------------
//
TInt CMmsTransaction::Reset()
    {
    LOG( _L("CMmsTransaction::Reset") );
    // We always point to the start of data
//    return KErrNone;
    return iDataSupplier->ResetSupplier();

    }

//  End of File
