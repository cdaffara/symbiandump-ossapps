/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   base class for mms operations
*
*/




// INCLUDE FILES
#include    <msventry.h>
#include    <logcli.h>
#include    <logview.h>
// socket error when switching to offline mode
#include    <es_sock.h>
// Internet error when switching to offline mode
#include    <inet6err.h>
#include    <CoreApplicationUIsSDKCRKeys.h>
#include    <e32property.h> // PubSub
#include    <connect/sbdefs.h>
#include    <apgcli.h>  // RApaLsSession
#include    <fileprotectionresolver.h>
#include    <mmsvattachmentmanager.h>

#include    "mmsbaseoperation.h"
#include    "mmsservercommon.h"
#include    "mmserrors.h"
#include    "mmssettings.h"
#include    "mmsserverentry.h"
#include    "mmssession.h"
#include    "mmsheaders.h"
#include    "mmsdecode.h"
#include    "mmsencode.h"
#include    "mmsscheduledentry.h"
#include    "mmslog.h"
#include    "mmsconninit.h"
#include    "MmsServerDebugLogging.h"
#include    "MmsPhoneClient.H"
#include    "mmscodecdatasupplier.h"
#include    "mmscodecdatasink.h"
#include 	"mmsregisteredapplications.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TInt KMmsBackupWait = 300000000; // wait for 5min, no more
const TInt KMmsServerReadyWait = 5000000; // wait for 5 sec for server ready after backup end

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsBaseOperation::CMmsBaseOperation
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMmsBaseOperation::CMmsBaseOperation( RFs& aFs ):CMsgActive( KMmsActiveObjectPriority ),
iFs( aFs )
    {
    // As this class is derived from CBase, all uninitialized variables are set to 0.
    }

// -----------------------------------------------------------------------------
// CMmsBaseOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMmsBaseOperation::ConstructL( CMmsSettings* aMmsSettings )
    {
    // Default critical state
    // If nothing is stored, operations just fall through, and there is no error
    iCriticalState = EMmsOperationEncodingPDU;
    iMmsSettings = aMmsSettings;

    // connect to socket to keep connection open as long as we need it
    TInt error = KErrNone;
    error = iSocketServ.Connect();
#ifndef _NO_MMSS_LOGGING_
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("BaseOperation: Connect Socket server returned error %d"), error );
        }
#endif
    User::LeaveIfError( error );
    error = iConnection.Open( iSocketServ );
#ifndef _NO_MMSS_LOGGING_
    if ( error != KErrNone )
        {
        TMmsLogger::Log( _L("BaseOperation: RConnection::Open returned error %d"), error );
        }
#endif
    User::LeaveIfError( error );
    iFailed = new( ELeave ) CMsvEntrySelection;
    iSuccessful = new( ELeave ) CMsvEntrySelection;
    iResponse = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    iEncoder = CMmsEncode::NewL( iFs );
    iDecoder = CMmsDecode::NewL( iFs );
    // expand size is arbitrary. It is not used, we
    // always allocate the amount we need
    iEncodeBuffer = CBufFlat::NewL( 0x100 );
	// coverity[size_error][buffer_alloc]
    iMmsSession = CMmsSession::NewL( KMmsActiveObjectPriority, iSocketServ, iConnection );
    iRemoteParties = new ( ELeave )CDesCArrayFlat( KMmsArrayAllocationNumber );
    // observe backup/restore event ends
    iBackupStart = EFalse;
    iBackupEnd = EFalse;
    }

// -----------------------------------------------------------------------------
// CMmsBaseOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMmsBaseOperation* CMmsBaseOperation::NewL( RFs& aFs, CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation* self = new( ELeave ) CMmsBaseOperation( aFs );
    
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsBaseOperation::~CMmsBaseOperation()
    {
    // derived class must cancel.
	Cancel();		// framework requirement
    // iSelection, iServerEntry, and iMmsSettings
    // are not deleted, because they belong to caller

// start of ROAMING CHECK handling
    delete iPhone;
// end ROAMING CHECK handling
    if ( iRemoteParties )
        {
        iRemoteParties->Reset();
        }
    delete iRemoteParties;
    delete iEncodeBuffer;
    delete iFailed;
    delete iSuccessful;
    delete iEntryWrapper;
    delete iUri;
    delete iResponse;
    delete iEncoder;
    delete iDecoder;
    delete iMmsLog;
    delete iLogEvent;
    delete iLogViewEvent;
    delete iLogClient;
    delete iMmsSession;

    // connection initiator should be NULL already...
    delete iConnectionInitiator;
    delete iIAPArray;
    iConnection.Close();
    iSocketServ.Close();

    }

// ---------------------------------------------------------
// CMmsBaseOperation::NetworkOperationsAllowed()
//
// ---------------------------------------------------------
//
TBool CMmsBaseOperation::NetworkOperationsAllowed()
    {
    TBool networkAllowed = ETrue; // optimist
    // If there is no such key, we will continue normally.
    // This means that in a system where online/offline mode switching
    // is not supported, we behave as we were always online
    
    CRepository* repository = NULL;
    TInt error = KErrNone;
    TInt value = ECoreAppUIsNetworkConnectionAllowed;
    TRAP( error, repository = CRepository::NewL( KCRUidCoreApplicationUIs ) );
    if( error == KErrNone )
        {
        repository->Get( KCoreAppUIsNetworkConnectionAllowed, value );
        delete repository;
        repository = NULL;
        if ( value == ECoreAppUIsNetworkConnectionNotAllowed )
            {
            networkAllowed = EFalse;
            }
        }

    return networkAllowed;
    }

// ---------------------------------------------------------
// CMmsBaseOperation::AllocateTID
//
// ---------------------------------------------------------
//
TInt64 CMmsBaseOperation::AllocateTID()
    {
    TTime currentTime;
    currentTime.UniversalTime();
    return currentTime.Int64();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::StartL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::StartL(
    CMsvEntrySelection& aSelection,
    CMsvServerEntry& aServerEntry,
    TMsvId aService,
    TRequestStatus& aStatus )
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation starting"));
#endif

    InitializeL( aSelection, aServerEntry, aService );

    if ( iCurrentMessageNo < 1 )
        {
        // nothing to send. Give up immediately
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNotFound );
        return;
        }

    // The first thing to do is to encode the first message.
    // We don't start connecting before we have something to send
    // We want to minimize the connection time in order to minimize
    // the probability of the connection being broken.

    // In the connect routine we must check that the connection
    // exists, and open it if it doesn't

    // the last call in derived class StartL before SetActive() must be Queue.
    // This will store the caller's status and set it to "request pending" state
    Queue( aStatus );
    FallThrough();
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::ContainsClosedContent
//
// ---------------------------------------------------------
//
TInt CMmsBaseOperation::CheckClosedContentL( CMsvServerEntry& aServerEntry, RFs& aFs )
    {
    RApaLsSession apaLsSession;
    TInt err = apaLsSession.Connect();
    
    if ( err != KErrNone )
        {
        return err;
        }
    CleanupClosePushL( apaLsSession );
    
    CFileProtectionResolver* fileProt = CFileProtectionResolver::NewLC( aFs );
        
    TDataRecognitionResult* dataType = new( ELeave )TDataRecognitionResult;
    CleanupStack::PushL( dataType );
        
    CMsvStore* store = aServerEntry.ReadStoreL();
    CleanupStack::PushL( store );
        
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    
    TInt attaCount = attachMan.AttachmentCount();
        
    TInt i = 0;
    // we check all attachments, but if we find even one forbidden type, we give up.
    for ( i = 0; i < attaCount && err == KErrNone; i++ )
        {
       	CMsvAttachment* attaInfo =  attachMan.GetAttachmentInfoL( i );
        RFile file;
        err = file.Open( aFs, attaInfo->FilePath(), EFileRead|EFileShareReadersOnly );
        delete attaInfo;
        attaInfo = NULL;
        
        if ( err == KErrNone )
            {
            // We continue only if we managed to open the file successfully    
            CleanupClosePushL( file );
        
            err = apaLsSession.RecognizeData( file, *dataType );
#ifndef _NO_MMSS_LOGGING_
            if ( err != KErrNone )
                {
                TMmsLogger::Log(_L("- Error in recognization of data type"), err );
                // We assume that the file cannot be closed content, if its type
                // is not recognized
                err = KErrNotSupported;
                }
            else
                {
                HBufC16* buf16 = HBufC16::NewLC( KMaxFileName );
                TPtrC dummy;
                TPtr ptr = buf16->Des();
                file.Name( ptr );
                // we cannot log indefinitely long strings.
                // We get this long strings only if the message is corrupted.
                dummy.Set( ptr.Left( KMmsMaxLogStringLength ) );
                TMmsLogger::Log( _L("- filename: %S"), &dummy );
                CleanupStack::PopAndDestroy( buf16 );

                TBuf<KMaxDataTypeLength> buffer;
                buffer.Copy( dataType->iDataType.Des8() );
                TMmsLogger::Log(_L("- Data type recognizer result: %S"), &buffer );
                if ( dataType->iConfidence == CApaDataRecognizerType::ECertain )
                    {
                    TMmsLogger::Log(_L("- Confidence level: Certain"));
                    }
                else if ( dataType->iConfidence == CApaDataRecognizerType::EProbable )
                    {
                    TMmsLogger::Log(_L("- Confidence level: Probable"));
                    }
                else if ( dataType->iConfidence == CApaDataRecognizerType::EPossible )
                    {
                    TMmsLogger::Log(_L("- Confidence level: Possible"));
                    }
                else if ( dataType->iConfidence == CApaDataRecognizerType::EUnlikely )
                    {
                    TMmsLogger::Log(_L("- Confidence level: Unlikely"));
                    }
                else if ( dataType->iConfidence == CApaDataRecognizerType::ENotRecognized )
                    {
                    TMmsLogger::Log(_L("- Confidence level: Not recognized"));
                    }
                else
                    {
                    TMmsLogger::Log(_L("- Unknown confidence level"));
                    }
                }
#endif
            // We Must set limit to "possible". The recognition is rather uncertain
            if ( err == KErrNone && dataType->iConfidence >= CApaDataRecognizerType::EPossible )
                {
                TInt pos = 0;
                file.Seek( ESeekStart, pos ); // rewind just in case
                TInt prot = fileProt->ProtectionStatusL( file, dataType->iDataType );
                if ( ( prot & ( EFileProtForwardLocked | EFileProtClosedContent ) ) &&
                    !( prot & EFileProtSuperDistributable ) )
                    {
                    err = KMmsErrorProtectedContent;
                    }
                }
            CleanupStack::PopAndDestroy( &file ); //close file    
            }
#ifndef _NO_MMSS_LOGGING_
        else
            {
            TMmsLogger::Log(_L("- Error in opening file for recognization of data type"), err );
            }
#endif
        }

    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( dataType );
    CleanupStack::PopAndDestroy( fileProt );
        
    CleanupStack::PopAndDestroy( &apaLsSession ); // close apaSession
    
    if ( err == KErrNotSupported )
        {
        // the content type was not recognized, we must assume it is not closed content
        err = KErrNone;
        }
    
    return err;
    }

// ---------------------------------------------------------
// CMmsBaseOperation::HandleBackupOperationEventL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::HandleBackupOperationEventL(
    const TBackupOperationAttributes& aBackupOperationAttributes )
    {
    switch ( aBackupOperationAttributes.iOperation )
        {
        case EStart:
            iBackupStart = ETrue;
            iBackupEnd = EFalse;
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log(_L("Backup/Restore Start"));
#endif
            break;
        case EEnd:
        case EAbort:
            iBackupStart = EFalse;
            if ( iHaveTimer )
                {
                iTimer.Cancel();
                // Set this to true only if we cancel timer because of backup end
                iBackupEnd = ETrue;
                // do not do it twice
                iDoNotWaitForBackupEnd = ETrue;
                iMustWait = ETrue;
                }
#ifndef _NO_MMSS_LOGGING_
            if ( aBackupOperationAttributes.iOperation == EEnd )
                {
                TMmsLogger::Log(_L("Backup/Restore End"));
                }
            else // if not end, then it was abort
                {
                TMmsLogger::Log(_L("Backup/Restore Abort"));
                }
#endif
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::RunL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::RunL()
    {
    // DoRunL() takes the AO through the states, queuing another
    // asynch step as required.
    // if DoRunL() detects the end of the cycle it returns
    // without queuing another cycle.

    // If Run() would exit without queuing another cycle it reports
    // completion to the client.
    // In CMsgActive this is true if the asynch step or DoRunL fails,
    // or the state cycle is complete
    // However, we must keep our cycle going in spite of error.
    // If any step reports an error, we must cleanup the current
    // operation, and continue to the next entry in the selection.
    // Only if we lose the connection, we complete the whole loop.

    // If we have completed ourselves, the error is already in iError.
    // However, if an asynchronous operation completed us, the error
    // comes back to us in iStatus.
    // As we always complete ourselves by returning iError as the
    // status, we get back either our original iError, or a new
    // error value set by an asynchronous operation.
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation RunL status %d, error %d"),
        iStatus.Int(), iError );
#endif
    TInt status=iStatus.Int();

    if ( iBackupEnd && ( status == KErrCancel || status == KErrTimedOut ) )
        {
        // timer cancelled because of backup end - continue as if no error
        status = KErrNone;
        }

    // wait for 5 seconds after backup end
    if ( iMustWait )
        {
        // Timer is never active if iMustWait == ETrue,
        // Actually it is not possible to check if iTimer is active.
        // We must rely on iMustWait.
        // iMustWait is set true only when the timer was just cancelled
        // because of backup end, and we must wait a few seconds before continuing.
        iMustWait = EFalse;
        iTimer.After( iStatus, KMmsServerReadyWait );
        SetActive();
        return;
        }

    // when we come here after wait, the timer can go.
    if ( iHaveTimer )
        {
        iTimer.Cancel();
        iTimer.Close();
        iHaveTimer = EFalse;
        iDoNotWaitForBackupEnd = ETrue;
        iBackupEnd = EFalse;
        }

    if ( status <= (TInt) KMsvMediaUnavailable &&
        status >= (TInt) KMsvIndexRestore &&
        !iDoNotWaitForBackupEnd )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- must wait for backup end ") );
#endif
        iState = EMmsOperationWaitingForBackupEnd;
        // This will take us back to RunL
        WaitForBackupEnd();
        // iError has been set when the media unavailable error has occurred.
        if ( iError == status )
            {
            iError = KErrNone;
            }
        return;
        }
        
    if ( iState == EMmsOperationCheckingRoaming )
        {
        // the status is the result of the roaming query
        iRegistrationStatus = status;
        delete iPhone;
        iPhone = NULL;
        if ( status > 0 )
            {
            // If the status is a successful registration state result,
            // it must not be set into iError
            // However, if something has gone wrong, the error should
            // be propagated.
            status = 0;
            }
        }

    // Don't override old error.
    // Connection state returns index of successful connection in status,
    // it must be handled separately.
    // And if logging fails, the error is not stored.
    // Logging is done on "best effort" basis
    if ( iError == KErrNone &&
        iState != EMmsOperationConnectingToIAP &&
        iState != EMmsOperationLogging)
        {
        iError = status;
        }

    // The connection may go wrong, which means there's no use continuing now.
    // On the other hand, if one operation fails, the next one may be all right.
    // We don't exit in the middle of the loop without deciding
    // what cleanup work must be done.
    // Only cancel can exit.
    if ( status != KErrCancel )
        {
        // iError contains the possible error from previous step.
        // It may be needed to decide what to do with current message
        // in next step.
        TRAPD( error,DoRunL() );    // continue operations, may re-queue
        // must not requeue in error situations
        // If we want to continue the loop in spite of error,
        // we must not leave, but return the error in iError.
        // Symbian code may leave, so we must trap the leaves here,
        // as we want to keep the loop going unless something
        // is really fatally wrong (we are completely out of memory,
        // or the system has crashed)
#ifndef _NO_MMSS_LOGGING_
        if ( error != KErrNone )
            {
            TMmsLogger::Log(_L("BaseOperatin DoRunL left with error %d"),
                error );
            if ( IsActive() )
                {
                TMmsLogger::Log(_L("- still active!"));
                }
            }
#endif
        __ASSERT_DEBUG( error==KErrNone || !IsActive(), User::Invariant() );
        if ( IsActive() ) // requeued
            {
            return;
            }
        status=error;
        if ( error == KErrNone )
            {
            // If DoRunL did not leave, possible error is in iError
            status = iError;
            }
        }
    
    Complete( status );
    }

// ---------------------------------------------------------
// CMmsBaseOperation::DoRunL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::DoRunL()
    {

    // This routine takes the state machine through the states
    // until an error is encountered or the cycle completes.

    if ( iState != EMmsOperationFinalizing )
        {
        SelectNextState();
        // If appropriate, ChangeState makes us active again
        ChangeStateL();
        }
    else
        {
        iState = EMmsOperationIdle;
        iStatus = iError;
        // If we return from DoRunL without becoming active again,
        // RunL completes.
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::DoComplete
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::DoComplete( TInt& aError )
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation DoComplete"));
#endif
    // We should get here if we are cancelled, or if
    // the cycle has completed (with or without error)

    // Only final cleanup can be done here, nothing asychronous is allowed

    UnSetSendMask( *iFailed, aError );

    }

// ---------------------------------------------------------
// CMmsBaseOperation::DoCancel
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::DoCancel()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation DoCancel"));
#endif
    // cancel anything that is still active

    if ( iPhone )
        {
        iPhone->Cancel();
        }
    if ( iDecoder )
        {
        iDecoder->Cancel();
        }
    if ( iEncoder )
        {
        iEncoder->Cancel();
        }
    if ( iConnectionInitiator )
        {
        iConnectionInitiator->Cancel();
        }
    if ( iMmsSession )
        {
        iMmsSession->Cancel();
        }
    if ( iHaveTimer )
        {
        iTimer.Cancel();
        iTimer.Close();
        iHaveTimer = EFalse;
        }

    CMsgActive::DoCancel();
    if ( iError == KErrNone )
        {
        iError = KErrCancel;
        }

    }

// ---------------------------------------------------------
// CMmsBaseOperation::SelectNextState
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::SelectNextState()
    {

    // If appropriate, the functions called within the switch statement
    // will make us active again. If all is done, the asynchronous request
    // will complete

    // iCritical state determines the state where the operation must be
    // continued if progress is interrupted by an error caused by backup/restore.
    // The critical state changes when enough information has been saved so
    // that backtracking is no longer needed. The critical state is always a 
    // stat where information is read from disk or stored to disk, because
    // disk operations may fail because of backup/restore.
    // Continuation is usually possible only after backup, not after restore,
    // but because available events do not specify which operation is in progress,
    // every situation must be handled as if it would be possible to continue
    // as soon as disk files can be accessed again.

    switch ( iState )
        {
        case EMmsOperationIdle:
            iState = EMmsOperationCheckingRoaming;
            break;
        case EMmsOperationCheckingRoaming:
            // check what we got from the roaming query and adjust any 
            // roaming state dependent modes needed by the operation
            GetRoamingState();
            iState = EMmsOperationEncodingPDU;
            break;
        case EMmsOperationEncodingPDU:
            if ( iConnected )
                {
                iState = EMmsOperationSubmittingTransaction;
                }
            else
                {
                iState = EMmsOperationConnectingToIAP;
                }
            iCriticalState = EMmsOperationCreatingEntry;
            break;
        case EMmsOperationConnectingToIAP:
            CheckConnectionState();
            if ( iConnected )
                {
                iState = EMmsOperationInitializingSession;
                }
            else
                {
                // if could not connect, give up
                iState = EMmsOperationUpdatingEntryStatus; 
                }
            break;
        case EMmsOperationInitializingSession:
            iState = EMmsOperationSubmittingTransaction;
            break;
        case EMmsOperationSubmittingTransaction:
            iState = EMmsOperationCreatingEntry;
            iCriticalState = EMmsOperationCreatingEntry;
            break;
        case EMmsOperationCreatingEntry:
            iState = EMmsOperationDecodingResponse;
            break;
        case EMmsOperationDecodingResponse:
            iState = EMmsOperationStoringResponseData;
            iCriticalState = EMmsOperationStoringResponseData;
            break;
        case EMmsOperationStoringResponseData:
            iState = EMmsOperationEncodingAck;
            iCriticalState = EMmsOperationUpdatingEntryStatus;
            break;
        case EMmsOperationEncodingAck:
            iState = EMmsOperationSendingAck;
            break;
        case EMmsOperationSendingAck:
            iState = EMmsOperationUpdatingEntryStatus;
            break;
        case EMmsOperationUpdatingEntryStatus:
            iState = EMmsOperationLogging;
            iCriticalState = EMmsOperationMovingEntry;
            break;
        case EMmsOperationLogging:
            iState = EMmsOperationMovingEntry;
            break;
        case EMmsOperationMovingEntry:
            iCurrentMessageNo--;
            if ( iCurrentMessageNo > 0 )
                {
                iState = EMmsOperationEncodingPDU;
                iCriticalState = EMmsOperationEncodingPDU;
                }
            else
                {
                iState = EMmsOperationFinalizing;
                iCriticalState = EMmsOperationFinalizing;
                }
            break;
        case EMmsOperationFinalizing:
            // no more states
            break;
        case EMmsOperationWaitingForBackupEnd:
            // return to failure point
            iState = iCriticalState;
            break;
        default:
            // If we are in an illegal state, we don't change it.
            // The only way we can end here is if someone overwrites
            // our stack, and in that case things are so terribly wrong
            // that there is nothing to be done
            // Change state will terminate the loop.
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("BaseOperation in illegal state %d "), iState );
#endif
            break;
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::ChangeStateL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::ChangeStateL()
    {
    switch ( iState )
        {
        case EMmsOperationCheckingRoaming:
            RoamingCheck();
            break;
        case EMmsOperationEncodingPDU:
            EncodePDUL();
            break;
        case EMmsOperationConnectingToIAP:
            ConnectToIAPL();
            break;
        case EMmsOperationInitializingSession:
            InitializeSessionL();
            break;
        case EMmsOperationSubmittingTransaction:
            SubmitTransactionL();
            break;
        case EMmsOperationCreatingEntry:
            CreateEntryL();
            break;
        case EMmsOperationDecodingResponse:
            DecodeResponseL();
            break;
        case EMmsOperationStoringResponseData:
            StoreResponseL();
            break;
        case EMmsOperationEncodingAck:
            EncodeAckL();
            break;
        case EMmsOperationSendingAck:
            SendAckL();
            break;
        case EMmsOperationUpdatingEntryStatus:
            UpdateEntryStatusL();
            break;
        case EMmsOperationLogging:
            LogL();
            break;
        case EMmsOperationMovingEntry:
            MoveEntryL();
            break;
        case EMmsOperationFinalizing:
            FinalizeL();
            break;
        case EMmsOperationWaitingForBackupEnd:
            WaitForBackupEnd();
            break;
        case EMmsOperationIdle:
            // We should not become idle again. This is startup state
            break;
        default:
            // Totally messed up. Someone overwrote the stack.
            // If we return without becoming active again, the 
            // cycle completes.
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("BaseOperation in illegal state %d "), iState );
#endif
            iError = KErrUnknown;
            iStatus = iError;
            break;
        }
    }


// ---------------------------------------------------------
// CMmsBaseOperation::FallThrough
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::FallThrough()
    {
    TRequestStatus* status = &iStatus;
    iStatus = KRequestPending;
    SetActive();
    User::RequestComplete( status, iError );
    }


// ---------------------------------------------------------
// CMmsBaseOperation::InitializeL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::InitializeL(
    CMsvEntrySelection& aSelection,
    CMsvServerEntry& aServerEntry,
    TMsvId aService )
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation Initialize"));
#endif

    iSelection = &aSelection;
    iServerEntry = &aServerEntry;
    iService = aService;

    iDoNotWaitForBackupEnd = EFalse;
    iCriticalState = EMmsOperationEncodingPDU;

    delete iEntryWrapper;
    iEntryWrapper = NULL;
    iEntryWrapper = CMmsServerEntry::NewL(
        iFs,
        aServerEntry,
        iService );

    iFailed->Reset();
    iSuccessful->Reset();
    iResponse->Reset();

    iCurrentMessageNo = iSelection->Count();
    iError = KErrNone;

    // clean these in case the class is reused
    delete iUri;
    iUri = NULL;
    delete iIAPArray;
    iIAPArray = NULL;
    iConnectionIndex = 0;
    iRegistrationStatus = 0; // hope for the best

    // Initialize everything into the failed list.
    // Very pessimistic indeed.

    if ( iCurrentMessageNo > 0 )
        {
        iFailed->AppendL( iSelection->Back( 0 ), iCurrentMessageNo );
        iSuccessful->SetReserveL( iCurrentMessageNo );
        }

    SetSendMask( *iFailed );
    
    //
    // NOTE: receive operation must override this setting to 'iLocalModeIn'
    //
    if ( iMmsSettings->LocalMode() )
        {
        iFs.SetSessionPath( iMmsSettings->LocalModeOut() );
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::EncodePDUL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::EncodePDUL()
    {
    // This state is quite variable.
    // Each operation will need to know what to encode,
    // no common implementation available.
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation EncodePDUL - FallThrough"));
#endif

    // this always starts a new round. iError should be cleared.
    FallThrough();

    }

// ---------------------------------------------------------
// CMmsBaseOperation::ConnectToIAPL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::ConnectToIAPL()
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation Connect to IAP"));
#endif

    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    // This operation should be identical for all derived classes.
    TRequestStatus* status = &iStatus;

    // clean URI in case the class is used more than once
    // Just at the last minute we check if network operations are allowed
    if ( !NetworkOperationsAllowed() )
        {
#ifdef __WINS__
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, KMmsErrorOfflineMode );
        return;
#else
        // if offline is on and global mode is on in HW, normal offline behaviour.
        // if offline is on and local mode is on in HW, continue.
        // The purpose of this is to allow fetching precreated messages even if
        // network operations were not allowed during the first boot.
        if ( !iMmsSettings->LocalMode() )
            {
            iStatus = KRequestPending;
            SetActive();
            User::RequestComplete( status, KMmsErrorOfflineMode );
            return;
            }
#endif
        }

    // Use WAP AP from Comms database table.
    // This contains the home page URI, and is linked to the actual IAP.
    TInt count = iMmsSettings->AccessPointCount();
    if( !iMmsSettings->LocalMode() )
        {
        if ( count < 1 )
            {
            User::Leave( KMmsErrorNoWAPAccessPoint );
            }
        iIAPArray = new (ELeave) CArrayFixFlat<TUint32>( count );
        for( TInt index = 0; index < count; index++ )
            {
            iIAPArray->AppendL( iMmsSettings->AccessPoint( index ) );
            }

        iConnectionInitiator = CMmsConnectionInitiator::NewL();
        // CMmsConnInit sets our status to "KRequestPending"
        iConnectionInitiator->ConnectL(
            iConnection,
			*iIAPArray,	
			iStatus );

        SetActive();
        }
    else
        {
        // local mode has no real wap access point
        iStatus = KRequestPending;
        SetActive();
        // we return status 1 (as if we got connection to most
        // preferred access point)
        User::RequestComplete( status, 1 );
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::InitializeSessionL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::InitializeSessionL()
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation InitializeSession"));
#endif

    if ( iError != KErrNone || iMmsSettings->LocalMode() )
        {
        FallThrough();
        return;
        }

// When we are supporting only HTTP, there is no separate session connection stage.
// Session initialization only sets necessary headers. (User agent and UAProf)
    delete iUri;
    iUri = NULL;
    TUint32 iap = 0;
    TBool proxyUsed = EFalse;
    HBufC8* proxyAddress = NULL;
    HBufC* uri = NULL;

    // We are not supposed to be here, if we haven't succeeded
    // in making connection to one of specified access points
    CMmsConnectionInitiator::GetParametersL( 
        iIAPArray->At( iConnectionIndex - 1 ),
        iap,
        uri, 
        proxyUsed,
        proxyAddress
        );

    // We must remember the uri, it will be used later.
    iUri = uri;
    uri = NULL;

    CleanupStack::PushL( proxyAddress );
    
    iMmsSession->OpenL(
        ( !proxyAddress )?TPtrC8():proxyAddress->Des(),
        proxyUsed,
        iMmsSettings->MaximumReceiveSize(),
        iMmsSettings->MaximumSendSize(),
        iStatus );

    CleanupStack::PopAndDestroy( proxyAddress );
    SetActive();

    }

// ---------------------------------------------------------
// CMmsBaseOperation::SubmitTransactionL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::SubmitTransactionL()
    {
    // Standard transaction is sending a PDU to MMSC in global mode
    // Local mode will need an override if supported (send and fetch only)
    // Fetch will need an override, because it uses HTTP GET
    // and not HTTP POST like other operations.

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation SubmitTransaction"));
#endif

    if ( !iConnected )
        {
        if ( iError == KErrNone )
            {
            iError = KErrCouldNotConnect;
            }
        }
        
    // This check is needed only when running tests in local mode
    // if length of URI is 0, Symbian code will panic    
    if ( !iUri )
        {
        iError = KMmsErrorNoURI1;
        }
    else if ( iUri->Des().Length() == 0 )
        {
        iError = KMmsErrorNoURI1;
        }
    else
        {
        // keep LINT happy
        }

    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    // Set entry
    TInt error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );

    //
    // Last minute check to make sure entry has not been deleted or suspended 
    //
    if( error == KErrNone )
        {
        TMsvEntry entry = iServerEntry->Entry();
        if( entry.SendingState() == KMsvSendStateSuspended || entry.Deleted() )
            {
            // The message has disappeared and we cannot do anything with it.
            #ifndef _NO_MMSS_LOGGING_
            if ( entry.SendingState() == KMsvSendStateSuspended )
                {
                TMmsLogger::Log( _L("- message in suspended") );
                }
            if ( entry.Deleted() )
                {
                TMmsLogger::Log( _L("- message deleted") );
                }
            TMmsLogger::Log( _L("-> finished with this entry") );
            #endif
            // delete entry from the list
            iFailed->Delete( iCurrentMessageNo - 1 );

            //
            // Set error and fall through
            // 
            iError = KErrNotFound;
            FallThrough();
            return;
            }
        }

    //
    // Update the sending state of our trigger
    //
    if( error == KErrNone )
        {
        TMsvEntry entry = iServerEntry->Entry();
        entry.SetConnected( ETrue );
        entry.SetSendingState( KMsvSendStateSending );
        // if this fails, our sending state is wrong, but so what...
        iServerEntry->ChangeEntry( entry );
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    //
    // Send
    //
    iMmsSession->SendMessageL( iUri->Des(),
        *iEncodeBuffer,
        *iEncoder,
        *iDecoder,
        iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::CreateEntryL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::CreateEntryL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation CreateEntry - FallThrough"));
#endif
    // When sending messages or command PDUs (forward) there is no entry to create.
    // These fall through, only receive message and view MMBox have incoming data.
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::DecodeResponseL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::DecodeResponseL()
    {
    // Default action is to decode response into headers.
    // The result will be analyzed in StoreResponse function
    // There some data may be updated to original entry or just
    // error code will be set.

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation DecodeResponse"));
#endif

    iResponse->Reset();
    if ( !( iError != KErrNone || iEncodeBuffer->Size() == 0 ) )
        {
        // we have got a response - try to decode it
        TRAP( iError,
            {
            // We don't need an actual entry here.
            iEntryWrapper->SetCurrentEntry( KMsvNullIndexEntryId );
            iDecoder->DecodeHeadersL(
               *iEntryWrapper,
               *iResponse,
               *iEncodeBuffer );
            });
        // Response status is mandatory in most responses, but not in quite all.
        // Some operations may need an override.
        if ( iResponse->ResponseStatus() != 0 )
            {
            MapResponseStatus( *iResponse );
            }
        }
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::StoreResponseL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::StoreResponseL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation StoreResponse - FallThrough"));
#endif
    // Here the response from MMSC is stored.
    // When receiving message, the results have already been stored while decoding.
    // Here the message would become visible.
    // However, if there is an error, the error text is saved here for all messages.
    // Other information may also be saved to the trigger entry (for example message id).
    // When a message is forwarded, the relevant information is stored in the notification.

    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::EncodeAckL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::EncodeAckL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation EncodeAck - FallThrough"));
#endif
    // Only receive message has ack, others fall through
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::SendAckL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::SendAckL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation SendAck - FallThrough"));
#endif
    // Only receive message sends ack, others fall through
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::UpdateEntryStatusL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::UpdateEntryStatusL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation UpdateEntryStatus - FallThrough"));
#endif
    // This will update the status of the trigger as successful
    // or unsuccessful
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::MoveEntryL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::MoveEntryL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation MoveEntry - FallThrough"));
#endif
    // This will need an override
    // Send message moves entry to sent folder
    // Receive message checks the fate of a notification.
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::LogL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::LogL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation Log - FallThrough"));
#endif
    // This will need an override - all operations do not log
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::FinalizeL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::FinalizeL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation Finalize"));
#endif
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::WaitForBackupEnd
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::WaitForBackupEnd()
    {
    LOG( _L("BaseOperation WaitForBackupEnd"));
    TInt error = KErrNone;
    TInt value = 0;
    error = RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, value );

    if( value != conn::ENoBackup && error == KErrNone )
        {
        // We are not going to wait forever: using a timer
        LOG( _L(" - backup running ") );
        error = iTimer.CreateLocal();
        if( error == KErrNone )
            {
            iHaveTimer = ETrue;
            // The timer cannot be active as it was just created.
            // It makes no sense to check.
            // And besides it is impossible to check if RTimer is active.
            iTimer.After( iStatus, KMmsBackupWait );
            SetActive();
            return;
            }
        }
#ifndef _NO_MMSS_LOGGING_
    else
        {
        LOG( _L(" - backup not running ") );
        }
#endif

    // No backup - media gone for other reason
    iDoNotWaitForBackupEnd = ETrue;
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsBaseOperation::SetSendMask
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::SetSendMask( const CMsvEntrySelection& aSelection )
    {
    TInt index = aSelection.Count();
    TInt error = KErrNone;
    while ( index-- )
        {
        error = ( iServerEntry->SetEntry( aSelection[index] ) );
        if ( error == KErrNone )
            {
            TMsvEntry tEntry = iServerEntry->Entry();
            // State is set to "sending" as late as possible to allow cancelling
            // until the last possible moment
            // Error in an entry is set to KErrNone to allow the error
            // to be updated in case we fail halfway through a selection, and want to
            // set error to unhandled entries.
            tEntry.iError = KErrNone;
            iServerEntry->ChangeEntry( tEntry );
            }
        }
    // Release the entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    }

// ---------------------------------------------------------
// CMmsBaseOperation::UnSetSendMask
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::UnSetSendMask( CMsvEntrySelection& aSelection, TInt aError )
    {
    TInt index = aSelection.Count();

    // If we have a connection error because switching to offline mode
    // has interrupted our sending operation, we suspend our failed messges
    if ( !NetworkOperationsAllowed() )
        {
        aError = KMmsErrorOfflineMode;
        }

    while ( index-- )
        {
        UnSetSendMask( aSelection[index], aError );
        // When the entry is in no list, it will not be touched by UpDateEntriesL
        // function in CMmsServerMtm.
        // When we switch back to "online" mode, these enries will automatically
        // be rescheluded.
        if ( aError == KMmsErrorOfflineMode )
            {
            aSelection.Delete( index );
            }
        }
    // Release the entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMmsBaseOperation::UnSetSendMask
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::UnSetSendMask( const TMsvId aId, TInt aError )
    {
    if ( ( iServerEntry->SetEntry( aId ) ) != KErrNone )
        {
        return; // no can do
        }
    TInt error = KErrNone;    

    TMsvEntry tEntry = iServerEntry->Entry();
    tEntry.SetConnected( EFalse );
    tEntry.SetFailed( EFalse ); // optimistic
    if ( tEntry.iError == KErrNone )
        {
        tEntry.iError = aError;
        }

    if ( aError == KMmsErrorOfflineMode )
        {
        // special case: we did not necessarily fail, but we are not allowed to send now
        // Set entry to "suspended" state, but do not update recipient so that this does
        // not count as a send or receive attempt
        tEntry.iError = aError;
        // value for KMmsOffLineStae defined in mmsservercommon.h
        tEntry.SetSendingState( KMmsOffLineState );
        
        // if the notification is in inbox or in mmbox folder, its operation flags have to be marked
        // that the operation has finished due to offline mode. 
        // In addition if notification in mmbox folder has a duplicate, 
        // the duplicate has to be marked as well.
        TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
        if ( tEntry.Parent() == mmboxFolder )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L(" - CMmsBaseOperation:: duplicates has to be searched") );
#endif
            TInt err = KErrNone;
            TRAP ( err, MarkDuplicateL( EMmsNotificationOperationFailed, *iServerEntry ) );
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Trap failed: error %d "), err );
#endif
            }
        // Mark original notification
        if ( tEntry.Parent() == mmboxFolder ||
            tEntry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
            {
            MarkNotificationOperationFailed( tEntry );                   
            tEntry.SetReadOnly( ETrue );
            }
        }
    else
        {
        // we must also update the recipient information
        // in case it was not set already.

        // We trap all leaves.
        // If we cannot update recipient because we are in very low memory situation,
        // that cannot be helped.
        // DoCancel and DoComplete cannot leave.

        CMmsScheduledEntry* mmsScheduledEntry = NULL;
        TRAP( error, 
            {
            mmsScheduledEntry = CMmsScheduledEntry::NewL( tEntry );
            CleanupStack::PushL( mmsScheduledEntry );
            CMsvStore * store = NULL;
            store = iServerEntry->EditStoreL();
            CleanupStack::PushL( store );
            mmsScheduledEntry->RestoreL( *store );
            // if status is still NotYetSent, recipient has not been updated yet
            if ( mmsScheduledEntry->MmsRecipient().Status() == CMsvRecipient::ENotYetSent )
                {
                UpdateRecipient( tEntry.iError, *mmsScheduledEntry );
                }
            mmsScheduledEntry->StoreL( *store );
            store->CommitL();
            CleanupStack::PopAndDestroy( store );
            CleanupStack::PopAndDestroy( mmsScheduledEntry );
            };)
        mmsScheduledEntry = NULL;

        // Don't override previous error
        // We may have entries that have failed for different reasons
        if ( tEntry.iError != KErrNone )
            {
            tEntry.SetFailed( ETrue );
            }
        }
    error = iServerEntry->ChangeEntry( tEntry );
    }

// ---------------------------------------------------------
// CMmsBaseOperation::UpdateRecipient
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::UpdateRecipient( TInt aError, CMmsScheduledEntry& aMmsScheduledEntry )
    {
    aMmsScheduledEntry.MmsRecipient().Time().UniversalTime();
    aMmsScheduledEntry.MmsRecipient().SetError( aError );
    if ( aError == KErrNone )
        {
        aMmsScheduledEntry.MmsRecipient().SetStatus( CMsvRecipient::ESentSuccessfully );
        }
    else
        {
        aMmsScheduledEntry.MmsRecipient().SetStatus( CMsvRecipient::EFailedToSend );
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::MapResponseStatus
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::MapResponseStatus( CMmsHeaders& aResponse )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation MapResponseStatus"));
#endif
    // response status was initialize to 0
    // KMmsStatusOk = 128

    // This is common respose status mapping. Retrieve status is different,
    // MMBox view confirmation uses this mapping.

    switch ( aResponse.ResponseStatus() )
        {
        case KMmsStatusOk:
            // No error, don't change iError
            break;
        case KMmsErrorPermanentSendingAddressUnresolved:
        case KMmsErrorSendingAddressUnresolved:
            iError = KMmsErrorStatusMessageAddressUnresolved;
            break;
        case KMmsErrorTransientSendingAddressUnresolved:
            iError = KMmsErrorStatusTransientAddressUnresolved;
            break;
        case KMmsErrorPermanentMessageNotFound:
        case KMmsErrorMessageNotFound:
            iError = KMmsErrorStatusMessageNotFound;
            break;
        case KMmsErrorTransientMessageNotFound:
            iError = KMmsErrorStatusTransientMessageNotFound;
            break;
        case KMmsErrorTransientNetworkProblem:
        case KMmsErrorNetworkProblem:
            iError = KMmsErrorStatusNetworkProblem;
            break;
        case KMmsErrorPermanentServiceDenied:
        case KMmsErrorServiceDenied:
            iError = KMmsErrorStatusServiceDenied;
            break;
        case KMmsErrorPermanentMessageFormatCorrupt:
        case KMmsErrorMessageFormatCorrupt:
            iError = KMmsErrorStatusMessageFormatCorrupt;
            break;
        case KMmsErrorPermanentContentNotAccepted:
        case KMmsErrorNoContentAccepted:
            iError = KMmsErrorStatusContentNotAccepted;
            break;
        case KMmsErrorPermanentReplyChargingLimitationsNotMet:
            iError = KMmsErrorStatusReplyChargingLimitationsNotMet;
            break;
        case KMmsErrorPermanentReplyChargingRequestNotAccepted:
            iError = KMmsErrorStatusReplyChargingRequestNotAccepted;
            break;
        case KMmsErrorPermanentReplyChargingForwardingDenied:
            iError = KMmsErrorStatusReplyChargingForwardingDenied;
            break;
        case KMmsErrorPermanentReplyChargingNotSupported:
            iError = KMmsErrorStatusReplyChargingNotSupported;
            break;
        case KMmsErrorTransientFailure:
            iError = KMmsErrorStatusTransientFailure;
            break;
        case KMmsErrorUnspecified:
            iError = KMmsErrorStatusUnspecified;
            break;
        case KMmsErrorPermanentFailure:
            iError = KMmsErrorStatusPermanentFailure;
            break;
        case KMmsErrorUnsupportedMessage:
            iError = KMmsErrorStatusUnsupportedMessage;
            break;
        default:
            // No known status, but did not leave either
            if ( iError == KErrNone )
                {
                if ( ( iResponse->ResponseStatus() & KMmsErrorRangeMask ) == KMmsErrorTransient )
                    {
                    iError = KMmsErrorStatusTransientFailure;
                    }
                else
                    {
                    iError = KMmsErrorStatusPermanentFailure;
                    }
                }
            break;
        }
    }

// ---------------------------------------------------------
// CMmsBaseOperation::InitializeLoggingL
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::InitializeLoggingL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("BaseOperation InitializeLogging"));
#endif
    if ( !iLogClient )
        {
        iLogClient = CLogClient::NewL( iFs );
        }
    if ( !iLogViewEvent )
        {
        iLogViewEvent = CLogViewEvent::NewL( *iLogClient );
        }
    if ( !iMmsLog )
        {
        iMmsLog = CMmsLog::NewL( *iLogClient, *iLogViewEvent, iFs );
        }
    if ( !iLogEvent )
        {
        iLogEvent = CLogEvent::NewL();
        }
    }


// ---------------------------------------------------------
// CMmsBaseOperation::CheckConnectionState
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::CheckConnectionState()
    {
    TInt status=iStatus.Int();
    // these can go now.
    delete iConnectionInitiator;
    iConnectionInitiator = NULL;
    if ( status > 0 )
        {
        iConnectionIndex = status;
        // We say we are connected when we have the connection to IAP.
        // When protocol is HTTP there is no separate session opening step.
        iConnected = ETrue;
        }
    else
        {
        iError = status;
        if ( status == KErrGeneral )
            {
            iError = KErrCouldNotConnect;
            }
        }
    }
// ---------------------------------------------------------
// CMmsBaseOperation::FindDuplicateNotificationL
// 
// ---------------------------------------------------------
//
TInt CMmsBaseOperation::FindDuplicateNotificationL(
    TMsvId aParent, CMmsHeaders& aHeaders, TMsvId& aDuplicate )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L(" CMmsBaseOperation::FindDuplicateNotificationL") );
#endif

    TInt error = KErrNone;    
    aDuplicate = KMsvNullIndexEntryId;
    if ( aParent == KMsvNullIndexEntryId )
        {
        return KErrNotSupported;
        }

    CMsvServerEntry* workingEntry = NULL;
    TRAP( error, workingEntry = iServerEntry->NewEntryL( aParent ) );
    CleanupStack::PushL( workingEntry );
    
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( workingEntry ); // workingEntry
        return error;
        }

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection; 
    CleanupStack::PushL( selection );

    error = workingEntry->GetChildrenWithMtm( KUidMsgMMSNotification, *selection );

    TInt count = selection->Count();
    if ( count == 0 )
        {
        error = KErrNotSupported;
        }

    if ( error != KErrNone  )
        {
        CleanupStack::PopAndDestroy( selection );
        CleanupStack::PopAndDestroy( workingEntry );
        return error;
        }

    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( mmsHeaders );
     
    for ( TInt i = count; i > 0 && ( aDuplicate == KMsvNullIndexEntryId ); i-- )
        {
        if ( workingEntry->SetEntry( selection->At( i - 1 ) ) == KErrNone )
            {            
            CMsvStore* store = workingEntry->ReadStoreL();
            CleanupStack::PushL( store );
            mmsHeaders->RestoreL( *store );
            CleanupStack::PopAndDestroy( store );

            // content location must match 
            if ( mmsHeaders->ContentLocation().Compare( aHeaders.ContentLocation() ) == 0 )
                {
                // Identical. This probably means that we have not sent a response yet,
                // and MMSC has sent us a new notification.

#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- content locations match") );
#endif
                aDuplicate = workingEntry->Entry().Id();
                }
            }
        }

    CleanupStack::PopAndDestroy( mmsHeaders );
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( workingEntry );

    return error;
    }

// ---------------------------------------------------------
// CMmsBaseOperation::FreeNotification
// 
// ---------------------------------------------------------
//
TBool CMmsBaseOperation::FreeNotification( TMsvEntry aEntry, const TUint32 aOperation )
    {
    if ( aEntry.iMtmData2 & KMmsNewOperationForbidden ||
        ( aEntry.iMtmData2 & KMmsOperationFinished &&
        !( aEntry.iMtmData2 & KMmsOperationResult ) &&
        !( aEntry.iMtmData2 & KMmsStoredInMMBox ) &&
        ( aOperation == KMmsOperationFetch || aOperation == KMmsOperationForward )))
        {
        return EFalse;
        }

    return ETrue;
    }

// ---------------------------------------------------------
// CMmsBaseOperation::MarkNotificationOperationFailed
// 
// ---------------------------------------------------------
//
void CMmsBaseOperation::MarkNotificationOperationFailed( TMsvEntry& aEntry )
    {
    aEntry.iMtmData2 |= KMmsOperationFinished;        // finished
    aEntry.iMtmData2 |= KMmsOperationResult;          // failed
    aEntry.iMtmData2 &= ~KMmsOperationOngoing;        // operation not active
    aEntry.iMtmData2 &= ~KMmsNewOperationForbidden;   // new operation allowed
    }

// ---------------------------------------------------------
// CMmsBaseOperation::MarkNotificationDeletedFromMmbox
// 
// ---------------------------------------------------------
//
void CMmsBaseOperation::MarkNotificationDeletedFromMmbox( TMsvEntry& aEntry )
    {
    aEntry.iMtmData2 &= ~KMmsStoredInMMBox;         // not in mmbox
    aEntry.iMtmData2 &= ~KMmsOperationOngoing;      // operation not active
    aEntry.iMtmData2 |= KMmsOperationFinished;      // finished
    aEntry.iMtmData2 &= ~KMmsOperationResult;      // successfully 
    aEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // new operation allowed
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::MarkDuplicateL
// 
// ---------------------------------------------------------
//
void CMmsBaseOperation::MarkDuplicateL( TInt aState, CMsvServerEntry& aServerEntry )
    {
    // iServerEntry must point to the original notification
    // We can put default version here as the headers will be restored immediately anyway
    // This is a static function and does not have access to iMmsSettings where the actual
    // version is.
    // Restoring the headers reads the correct version for the message in question
    // Headers are used here only to clear the duplicate id from the headers.
    // Other headers are not changed.
    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( KMmsDefaultVersion );
    CleanupStack::PushL( mmsHeaders );
    CMsvStore* store = aServerEntry.ReadStoreL();
    CleanupStack::PushL( store );
    mmsHeaders->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    TMsvId duplicate = mmsHeaders->RelatedEntry();
    if ( duplicate != KMsvNullIndexEntryId )
        {
        // Clear related entry from the original notification
        store = aServerEntry.EditStoreL();
        CleanupStack::PushL( store );
        mmsHeaders->SetRelatedEntry( KMsvNullIndexEntryId );
        mmsHeaders->StoreL( *store );
        store->CommitL();
        CleanupStack::PopAndDestroy( store );
        store = NULL;
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- related entry cleared" ) );
#endif
        
        // Leaves if entry cannot be accessed.
        // We are inside a trap already...
        CMsvServerEntry* duplicateEntry = aServerEntry.NewEntryL( duplicate );
        CleanupStack::PushL( duplicateEntry );
        // Mark duplicate
        TMsvEntry dupEntry = duplicateEntry->Entry();
        switch( aState )
            {
            case EMmsNotificationOperationFailed:
                MarkNotificationOperationFailed( dupEntry );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L( "- duplicate marked failed" ) );
#endif
                break;
            case EMmsDeletedFromMmbox:
                MarkNotificationDeletedFromMmbox( dupEntry );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L( "- duplicate marked deleted" ) );
#endif
                break;
            default:
                break;    
            }
        duplicateEntry->ChangeEntry( dupEntry );
        CleanupStack::PopAndDestroy( duplicateEntry );

        }
    CleanupStack::PopAndDestroy( mmsHeaders );
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::MarkNotificationOperationReserved
// ---------------------------------------------------------
//       
void CMmsBaseOperation::MarkNotificationOperationReserved( TMsvEntry& aEntry, 
                                                                  const TUint32 aOperation )
    {
    aEntry.iMtmData2 &= ~KMmsOperationIdentifier;   // clear possible old operation
    aEntry.iMtmData2 |= KMmsNewOperationForbidden;  // forbidden   
    aEntry.iMtmData2 |= KMmsOperationOngoing;       // operation is active
    aEntry.iMtmData2 |= aOperation;                 // operation
    aEntry.iMtmData2 &= ~KMmsOperationFinished;     // not finished
    aEntry.iMtmData2 &= ~KMmsOperationResult;       // not failed  
    }
    
// ---------------------------------------------------------
// 
// ---------------------------------------------------------
//       
TBool CMmsBaseOperation::RegisteredL( const TDesC& aApplicationId )
    {
    TBool registered = EFalse;
    // check if application id is registered
    CMmsRegisteredApplications* regAppId = CMmsRegisteredApplications::NewL();
    CleanupStack::PushL( regAppId );
    regAppId->LoadRegisteredApplicationsL();
    registered = regAppId->RegisteredL( aApplicationId );
    CleanupStack::PopAndDestroy( regAppId );
    return registered;
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::CommonLogEventInitializationL
// 
// ---------------------------------------------------------
//
void CMmsBaseOperation::CommonLogEventInitializationL(
    CMmsHeaders& aMmsHeaders,
    TMsvEntry& aEntry )
    {
    if ( !iLogEvent || !iLogClient )
        {
        // something wrong with log - cannot do logging
        User::Leave( KErrGeneral );
        }
        
    iLogEvent->SetEventType( KLogMmsEventTypeUid );
    iLogClient->GetString( iLogString, R_LOG_DIR_OUT );
    iLogEvent->SetDirection( iLogString );
    iLogEvent->SetDurationType( KLogDurationNone );

    iLogEvent->SetSubject( aEntry.iDescription );
    iLogEvent->SetLink( aEntry.Id() );

    iRemoteParties->Reset();
    iNumberOfRemoteParties = 0;

    // Get some fields into log entry
    TTime now;
    // the dates in log must be in universal time, not local time
    now.UniversalTime();
    iLogEvent->SetTime( now );
    // We only log if the sending has been successful.
    // We don't generate a log entry if iError not equal to KErrNone
    // Therefore the status is always "pending"
    iLogClient->GetString( iLogString, R_LOG_DEL_PENDING );
    iLogEvent->SetStatus( iLogString );

    // Generate remote party list (pure addresses only)
    TInt i;
    TPtrC dummy;
    // To list
    for (i = 0; i < aMmsHeaders.ToRecipients().MdcaCount(); i++)
        {
        dummy.Set( TMmsGenUtils::PureAddress( aMmsHeaders.ToRecipients().MdcaPoint( i ) ) );
        if ( iMmsSettings->LogEmailRecipients() ||
            ( dummy.Locate( '@' ) == KErrNotFound ) )
            {
            iRemoteParties->AppendL( dummy );
            iNumberOfRemoteParties++;
            }
        }
    // Cc list
    for (i = 0; i < aMmsHeaders.CcRecipients().MdcaCount(); i++)
        {
        dummy.Set( TMmsGenUtils::PureAddress( aMmsHeaders.CcRecipients().MdcaPoint( i ) ) );
        if ( iMmsSettings->LogEmailRecipients() ||
            ( dummy.Locate( '@' ) == KErrNotFound ) )
            {
            iRemoteParties->AppendL( dummy );
            iNumberOfRemoteParties++;
            }
        }
    // Bcc list
    for (i = 0; i < aMmsHeaders.BccRecipients().MdcaCount(); i++)
        {
        dummy.Set( TMmsGenUtils::PureAddress( aMmsHeaders.BccRecipients().MdcaPoint( i ) ) );
        if ( iMmsSettings->LogEmailRecipients() ||
            ( dummy.Locate( '@' ) == KErrNotFound ) )
            {
            iRemoteParties->AppendL( dummy );
            iNumberOfRemoteParties++;
            }
        }
    }
    
// start of ROAMING CHECK handling
// ---------------------------------------------------------
// CMmsBaseOperation::RoamingCheck
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::RoamingCheck()
    {
    // base operation does nothing
    // This is just a placeholder
    FallThrough();
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::GetRoamingState
//
// ---------------------------------------------------------
//
void CMmsBaseOperation::GetRoamingState()
    {
    // Nothing needs to be done here
    }
    
    
    
#ifndef _NO_MMSS_LOGGING_
// ---------------------------------------------------------
// CMmsBaseOperation::LogDateL
//
// ---------------------------------------------------------
void CMmsBaseOperation::LogDateL( const TTime& aDate )
    {
    TBuf<KMmsDateBufferLength> dateString;
    aDate.FormatL(dateString,(_L("%*E%*D%X%*N%Y %1 %2 '%3")));
    TMmsLogger::Log( _L(" - date %S"), &dateString );
    aDate.FormatL(dateString,(_L("%-B%:0%J%:1%T%:2%S%:3%+B")));
    TMmsLogger::Log( _L(" - time %S"), &dateString );
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::LogNetworkFormatDateL
//
// ---------------------------------------------------------
void CMmsBaseOperation::LogNetworkFormatDateL( const TInt64& aDateInSeconds )
    {
    TBuf<KMmsDateBufferLength> dateString;
    TMmsLogger::Log( _L(" - %d seconds from 1.1.1970 (UTC)"), aDateInSeconds );
    TTime time = TTime( KMmsYear1970String ) +
        TTimeIntervalMicroSeconds( aDateInSeconds * KMmsMillion );
    time.FormatL(dateString,(_L("%*E%*D%X%*N%Y %1 %2 '%3")));
    TMmsLogger::Log( _L(" - date %S"), &dateString );
    time.FormatL(dateString,(_L("%-B%:0%J%:1%T%:2%S%:3%+B")));
    TMmsLogger::Log( _L(" - time %S"), &dateString );
    }
    
#else
// ---------------------------------------------------------
// CMmsBaseOperation::LogDateL
//
// ---------------------------------------------------------
void CMmsBaseOperation::LogDateL( const TTime& )
    {
    }
    
// ---------------------------------------------------------
// CMmsBaseOperation::LogNetworkFormatDateL
//
// ---------------------------------------------------------
void CMmsBaseOperation::LogNetworkFormatDateL( const TInt64& )
    {
    }
#endif
    
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
