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
* Description:  
*     State machine for sending a forward request
*
*/



// INCLUDE FILES
#include    <apparc.h>
#include    <msventry.h>
#include    <msvids.h>
#include    <logcli.h>
#include    <commdb.h>
#include    <in_sock.h>
#include    <commdbconnpref.h>
#include    <featmgr.h>

// the rest are local includes, needed always
#include    "mmsconst.h"
#include    "mmsforwardoperation.h"
#include    "mmssession.h"
#include    "mmssettings.h"
#include    "mmsservercommon.h"
#include    "mmsheaders.h"
#include    "mmsencode.h"
#include    "mmsdecode.h"
#include    "mmsscheduledentry.h"
#include    "mmsgenutils.h"
#include    "mmslog.h"
#include    "mmserrors.h"
#include    "mmsserverentry.h"
#include    "mmsconninit.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic( TMmsPanic aPanic );

// CONSTANTS

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ==================== LOCAL FUNCTIONS ====================
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsForwardOperation::CMmsForwardOperation
//
// ---------------------------------------------------------
//
CMmsForwardOperation::CMmsForwardOperation( RFs& aFs ):
    CMmsBaseOperation( aFs )
    {
    //
    // members that get initial value 0, need not be initialized here
    //
    }

// ---------------------------------------------------------
// CMmsForwardOperation::ConstructL
//
// ---------------------------------------------------------
//
void CMmsForwardOperation::ConstructL( CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsRequestHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMmsForwardOperation::NewL
//
// ---------------------------------------------------------
//
CMmsForwardOperation* CMmsForwardOperation::NewL( RFs& aFs, CMmsSettings* aMmsSettings )
    {
    CMmsForwardOperation* self = new ( ELeave ) CMmsForwardOperation( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );
    return self;
    }

    
// ---------------------------------------------------------
// CMmsForwardOperation::~CMmsForwardOperation
//
// ---------------------------------------------------------
//
CMmsForwardOperation::~CMmsForwardOperation()
    {
    Cancel();
    delete iMmsRequestHeaders;
    }

// ---------------------------------------------------------
// CMmsForwardOperation::Failed
//
// ---------------------------------------------------------
//
CMsvEntrySelection& CMmsForwardOperation::Failed() const
    {
    return *iFailed;
    }

// ---------------------------------------------------------
// CMmsForwardOperation::EncodePDUL
//
// ---------------------------------------------------------
//
void CMmsForwardOperation::EncodePDUL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsForwardOperation EncodePDU") );
    #endif

    if( iError != KErrNone )
        {
        FallThrough();
        return;
        }
    //
    // Set entry's context to next message in the selection
    //
    iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if( iError != KErrNone )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- ERROR: SetEntry: %d"), iError );
        #endif
        FallThrough();
        return;
        }

    //
    // Getting write access to message's store because headers have to be
    // altered.
    //
    CMsvStore* store = NULL;
    TRAP( iError, store = iServerEntry->EditStoreL(); )
    if ( iError != KErrNone )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- ERROR: EditStoreL: %d"), iError );
        #endif
        FallThrough();
        return;
        }
    CleanupStack::PushL( store ); // ***

    //
    // Filling headers object from the data in the MessageStore
    //
    iMmsRequestHeaders->RestoreL( *store );

    //
    // Forward request headers that are not filled by application,
    // are added below
    //

    //
    // TransactionId
    //
    TBufC8<KMMSMAXTIDLENGTH> tid;
    tid.Des().NumUC( AllocateTID(), EHex );
    iMmsRequestHeaders->SetTidL( tid );

    //
    // Set messagetype
    //
    iMmsRequestHeaders->SetMessageType( KMmsMessageTypeForwardReq );

    //
    // Saving headers back to Message Store
    // No diskspace check done here, because the size of the data has not
    // really changed that much (headers already are on the disk).
    //
    iMmsRequestHeaders->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    //
    // Encode the message
    //
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    iError = iEntryWrapper->SetCurrentEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( iError != KErrNone )
        {
        FallThrough();
        }
    else
        {
        iEncoder->EncodeHeadersL( *iMmsRequestHeaders, *iEncodeBuffer );
        FallThrough();
        }
    }

// ---------------------------------------------------------
// CMmsForwardOperation::UpdateEntryStatusL
//
// ---------------------------------------------------------
//
void CMmsForwardOperation::UpdateEntryStatusL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsForwardOperation UpdateEntryStatusL()" ) );
    TMmsLogger::Log( _L( "- Updating notification flags" ) );
    #endif

    TInt error = KErrNone;
    TMsvEntry tEntry;
    
    // Fix for a case where the server is sending no response to forward request.
    // If we have got a response and already set iError to something, we don't
    // override it.
    if ( iResponse->MessageType() != KMmsMessageTypeForwardConf && iError == KErrNone )
        {
    #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- Incorrect PDU type received: %d" ), iResponse->MessageType() );
    #endif
        iError = KMmsErrorStatusUnsupportedMessage;
        }

    //
    // Update the status of the notification entry
    //
    TMsvId notifId = iMmsRequestHeaders->RelatedEntry();
    error = iServerEntry->SetEntry( notifId );
    if ( error == KErrNone )
        {
        tEntry = iServerEntry->Entry();

        if( iError == KErrNone )
            {
            //
            // OK
            //
            tEntry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
            tEntry.iMtmData2 |= KMmsOperationFinished;      // finished
            tEntry.iMtmData2 &= ~KMmsOperationResult;       // OK
            tEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // not forbidden
            // KMmsStoredInMMBox is left untouched
            }
        else if( iError == KMmsErrorOfflineMode )
            {
            // Leaving flags as they are
            }
        else // iError not equal to KErrNone/KMmsErrorOfflineMode
            {
            //
            // NOK
            //
            tEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // not forbidden
            tEntry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
            tEntry.iMtmData2 |= KMmsOperationFinished;      // finished
            tEntry.iMtmData2 |= KMmsOperationResult;        // NOK
            // KMmsStoredInMMBox is left untouched
            }
        tEntry.SetReadOnly( ETrue );
        error = iServerEntry->ChangeEntry( tEntry );
        }
        
    #ifndef _NO_MMSS_LOGGING_
    if( error != KErrNone )
        {
        TMmsLogger::Log( _L( "- ERROR: could not change notification entry's flags!" ) );
        }
    #endif

    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- Updating forward entry state" ) );
    #endif
    //
    // Set context to forward entry
    //
    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if( error != KErrNone )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- ERROR: could not get forward entry" ) );
        #endif
        if( iError == KErrNone )
            {
            iError = error;
            }
        FallThrough();
        return;
        }

    //
    // If notification has been successfully updated, clear the relatedId link
    //
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "0" ) );
    #endif
    if( iError != KMmsErrorOfflineMode )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- clearing link to original notification" ) );
        #endif
        CMsvStore* store = iServerEntry->EditStoreL();
        CleanupStack::PushL( store ); // ***
        iMmsRequestHeaders->SetRelatedEntry( KMsvNullIndexEntryId );
        iMmsRequestHeaders->StoreL( *store );
        store->CommitL();
        CleanupStack::PopAndDestroy( store );
        }

    //
    // Update the status of the forward entry
    //
    tEntry = iServerEntry->Entry();
    if( iError == KErrNone )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- forward succeeded" ) );
        #endif
        tEntry.SetFailed( EFalse );
        tEntry.SetSendingState( KMsvSendStateSent );
        // Take the entry out of failed list
        // (only offline cases should get rescheduled)
        iFailed->Delete( iCurrentMessageNo - 1 );
        }
    else if( iError == KMmsErrorOfflineMode )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- forward failed due to Offline mode" ) );
        #endif
        tEntry.SetFailed( EFalse );
        tEntry.SetSendingState( KMmsOffLineState );        
        }
    else
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- forward failed" ) );
        #endif
        tEntry.SetFailed( ETrue );
        tEntry.SetSendingState( KMsvSendStateFailed );
        // Take the entry out of failed list in order not to get it rescheduled
        // (only offline cases should get rescheduled)
        iFailed->Delete( iCurrentMessageNo - 1 );
        }
    tEntry.SetConnected( EFalse );
    tEntry.iError = iError;
    tEntry.SetReadOnly( ETrue );
    error = iServerEntry->ChangeEntry( tEntry );
    #ifndef _NO_MMSS_LOGGING_
    if( error != KErrNone )
        {
        TMmsLogger::Log( _L( "- ERROR: could not change forward entry's flags!" ) );
        }
    #endif

    // Clear up
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsForwardOperation::MoveEntryL
//
// ---------------------------------------------------------
//
void CMmsForwardOperation::MoveEntryL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsForwardOperation MoveEntryL") );
    #endif

    //
    // If something has gone wrong, just falling through
    //
    if( iError != KErrNone )
        {
        FallThrough();
        return;
        }
    
    //
    // If all is well, the forwarded entry is moved to Sent folder,
    // or deleted, depending on the settings
    //
    TInt error = KErrNone;
    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( error == KErrNone )
        {
        TMsvEntry entry = iServerEntry->Entry();
        error = iServerEntry->SetEntry( entry.Parent() );
        
        if ( error == KErrNone )
            {
            if ( iMmsSettings->MoveToSent() )
                {
                // Move entry from Outbox to Sent Folder
                iServerEntry->MoveEntryWithinService( entry.Id(), KMsvSentEntryIdValue );
                }
            else
                {
                // Delete entry
                iServerEntry->DeleteEntry( entry.Id() );
                }
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsForwardOperation::LogL
//
// ---------------------------------------------------------
//
void CMmsForwardOperation::LogL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsForwardOperation LogL") );
    #endif

    TInt error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( error != KErrNone || iError != KErrNone ||
        iMmsRequestHeaders->DeliveryReport() != EMmsYes )
        {
        // Can't access the entry or do not want logging, can't update log
        FallThrough();
        return;
        }
        
    TBool isActive = EFalse;    
    TRAP( error,
        {
        InitializeLoggingL();
        
        TMsvEntry entry = iServerEntry->Entry();
        // set event type and recipients to log event
        CommonLogEventInitializationL( *iMmsRequestHeaders, entry );
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        
        // if MMSC has accepted our message for fowarding, the message id is present
        // If the entry was not accepted, iError reflects the error   
        iLogEvent->SetDataL( iResponse->MessageId() );
        // CMmsLog will set our status to KRequestPending
        iMmsLog->StartL( *iLogEvent, *iRemoteParties, iStatus );
        SetActive();
        isActive = ETrue;
        });
        
    // If logging has been successfully initialized, iMmsLog will complete us.
    // If something has gone wrong, we complete ourselves because we want to
    // continue finalization of the operation. We free our entry in any case
    
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    if ( !isActive )
        {
        FallThrough();
        return;
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
