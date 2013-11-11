/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for message receiving
*
*/



// INCLUDE FILES
#include    <apparc.h>
#include    <msventry.h>
#include    <msvids.h>
#include    <charconv.h>
#include    <badesca.h>
#include    <cmsvmimeheaders.h>
#include    <mmsvattachmentmanager.h>
#include    <mmsvattachmentmanagersync.h>

#include    <centralrepository.h>

// mms private headers
#include    "mmsconst.h"
#include    "mmsreceivemessage.h"
#include    "mmssession.h"
#include    "mmssettings.h"
#include    "mmsservercommon.h"
#include    "mmsheaders.h"
#include    "mmsdecode.h"
#include    "mmsencode.h"
#include    "mmsscheduledentry.h"
#include    "mmserrors.h"
#include    "mmsserverentry.h"
#include    "mmsconninit.h"
#include    "MmsEnginePrivateCRKeys.h"
#include    "MmsPhoneClient.H"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic(TMmsPanic aPanic);

// CONSTANTS
// return codes for handling of messages addressed to an application
const TInt KMmsMessageForUnregisteredApplication = 1;
const TInt KMmsMessageMovedToApplicationFolder = 2;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMmsReceiveMessage::CMmsReceiveMessage( RFs& aFs )
    :CMmsBaseOperation( aFs )
    {
    }

// Symbian OS default constructor can leave.
void CMmsReceiveMessage::ConstructL( CMmsSettings* aMmsSettings )
    {
    iFileOpen = EFalse;
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    iNotification = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    iBad = new( ELeave ) CMsvEntrySelection;
    TRAPD ( error, {iMessageDrive = MessageServer::CurrentDriveL( iFs );});
    if ( error != KErrNone )
        {
        // if cannot ask, use default
        iMessageDrive = EDriveC;
        }

    CActiveScheduler::Add( this );
    }

// Two-phased constructor.
CMmsReceiveMessage* CMmsReceiveMessage::NewL( RFs& aFs, CMmsSettings* aMmsSettings )
    {
    CMmsReceiveMessage* self = new ( ELeave ) CMmsReceiveMessage( aFs );
    
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CMmsReceiveMessage::~CMmsReceiveMessage()
    {
    
    Cancel();

    delete iMmsHeaders;
    delete iNotification;
    delete iBad;
    // This is the message for local mode testing
    // It is closed here only to make sure that it is
    // always closed even if we encounter a serious error.
    // otherwise it should already be closed by now.
    if ( iFileOpen )
        {
        iFile.Close();
        }
    }


// ---------------------------------------------------------
// CMmsReceiveMessage::StartL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::StartL(
    CMsvEntrySelection& aSelection,
    CMsvServerEntry& aServerEntry,
    TMsvId aService,
    TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iState == EMmsOperationIdle, gPanic( EMmsAlreadyBusy ) );

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("ReceiveMsg StartL") );
#endif
    // just initialize common member variables
    CMmsBaseOperation::InitializeL( aSelection, aServerEntry, aService );
    
    iReceivingMode = iMmsSettings->ReceivingModeHome();

    iNotificationParent = KMsvNullIndexEntryId;
    iBad->Reset();
    iNotification->Reset();
    iResponse->Reset(); // This sets the correct version
    iMmsHeaders->Reset();
    iEntryUnderConstruction = KMsvNullIndexEntryId;
    iAlreadyDeferred = EFalse;
    iFileOpen = EFalse;
    
    // selection is supposed to point to a bunch of notifications
    // containing the URIs the messages are to be fetched from
    TInt count = iCurrentMessageNo;
    iError = KErrNone;

    while ( count > 0 && ( iNotificationParent == KMsvNullIndexEntryId ) )
        {
        // Load the notification
        // iCurrentMessageNo (and therefore also count) has originally been
        // set to iSelection->Count() so the index is always valid.
        count--; // indexes are zero based
        if ( count < iSelection->Count() )
            {
            iError = iServerEntry->SetEntry( iSelection->At( count ) );
            }
        else
            {
            iError = KErrNotFound;
            }
        if ( iError == KErrNone )
            {
            TMsvEntry entry = iServerEntry->Entry();
            iNotificationParent = entry.Parent();
            }
        }
    
    // release the last notification
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    if (  iNotificationParent == KMsvNullIndexEntryId )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- could not access any entry to get parent") );
#endif
        // nothing to receive. Give up immediately
        aStatus = KRequestPending;
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, iError );
        return;
        }

    // Initialize everything into the failed list.
    // Very pessimistic indeed.
    
    iBad->SetReserveL( iCurrentMessageNo );

    Queue( aStatus );
    
    // We complete ourselves to get into the state machine loop.
    // The first thing to do is to check the roaming state.
    FallThrough();

    }

// ---------------------------------------------------------
// CMmsReceiveMessage::DoComplete
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::DoComplete(TInt& aError )
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg DoComplete"));
#endif
    // We should get here if we are cancelled, or if
    // the cycle has completed (with or without error)
    // Make sure the store is released
    iDecoder->RelaseDataSink();

    // If we have an error, we must delete the entry
    // under construction.
    // If we have successfully created the entry, we have set
    // iEntryUnderConstruction to KMsvNullIndexEntryId.
    // If iEntryUnderConstruction still points to an entry, it means
    // that the cycle has been aborted, and the incomplete entry
    // must be deleted.
    if ( iEntryUnderConstruction != KMsvNullIndexEntryId )
        {
        if ( iServerEntry->SetEntry( KMsvGlobalInBoxIndexEntryId ) == KErrNone )
            {
            iServerEntry->DeleteEntry ( iEntryUnderConstruction );
            }
        iEntryUnderConstruction = KMsvNullIndexEntryId;
        }

    // Set error to entries that are still left
    // If some of the entries already have an error set,
    // it is not overridden.
    UnSetSendMask( *iFailed, aError );
    
    // If we are supporting MMBox, we must also clear the duplicates
    // if we have notifications in bad list
    
    if ( iMmsSettings->MMBoxFolder() != KMsvNullIndexEntryId )
        {
        TInt i = 0;
#ifndef _NO_MMSS_LOGGING_
        if ( iBad->Count() > 0 )
            {
            TMmsLogger::Log( _L("- %d notifications on bad list "), iBad->Count() );
            }
#endif

        for ( i = 0; i < iBad->Count(); i++ )
            {
            if ( iServerEntry->SetEntry( iBad->At( i )) == KErrNone )
                {
                // we don't want to leave here, we do our best
                TRAP_IGNORE( ClearDuplicateEntryOperationL() ); 
                }
            }
        }
    }

// start of ROAMING CHECK handling
// ---------------------------------------------------------
// CMmsReceiveMessage::RoamingCheck
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::RoamingCheck()
    {
    
    TInt error = KErrNone;
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg roaming check start"));
#endif

    if ( !iMmsSettings->LocalMode() && !iMmsSettings->FetchOverride() &&
         ( ( iMmsSettings->ReceivingModeHome() != iMmsSettings->ReceivingModeForeign() ) ||
         ( iMmsSettings->ReceivingModeForeign() == EMmsReceivingManual ) ||
         ( iMmsSettings->ReceivingModeForeign() == EMmsReceivingPostpone ) ) )
        {
        // In offline mode we cannot ask if we are roaming - just give up
        if ( !NetworkOperationsAllowed() )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- Offline mode - cannot check network status"));
#endif
            error = KMmsErrorOfflineMode;
            iError = error;
            }
        else
            {
            // if we get an error we have not been able to connect to phone module
            // and we cannot ask if we are roaming
            TRAP( error, iPhone = CMmsPhoneClient::NewL() )
            }

        if ( error == KErrNone )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- Receivemsg getting network status"));
#endif
            iPhone->Roaming( iStatus );
            }
        }
    else
        {
        // no roaming check needed
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- Roaming check not needed"));
#endif
        TRequestStatus* status = &iStatus;
        iStatus = KRequestPending;
        // 0 = home network. If we don't care, we behave as at home
        User::RequestComplete( status, 0 );
        }
        
    if ( error != KErrNone )
        {
        // we got an error - iPhone is not going to complete us
        TRequestStatus* status = &iStatus;
        iStatus = KRequestPending;
        User::RequestComplete( status, error );
        }
    SetActive();
        
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::GetRoamingState
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::GetRoamingState()
    {
    
    // if current receiving modes are valid, we have to set the message fetch state
    // according to the receiving mode of the current (home or foreign ) network 
    
    // Set the message fetch state according to current network value
    if( iRegistrationStatus != 0 )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("roaming") );
#endif
        iReceivingMode = iMmsSettings->ReceivingModeForeign();
        }
    else
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("in home network") );
#endif
        iReceivingMode = iMmsSettings->ReceivingModeHome();        
        }
        
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- message receiving mode is %d"), iReceivingMode );
#endif
    
    // if something is wrong, entries are marked failed.
    if ( iError != KErrNone )
        {
        // Set error to all entries
        // If some of the entries already have an error set,
        // it is not overridden.
        UnSetSendMask( *iFailed, iError );

        }
    // if we have something in iError, we shall not continue
    // to open IAP, and RunL will complete. 
    // All entries will be rescheduled

    }
// end of ROAMING CHECK handling

// ---------------------------------------------------------
// CMmsReceiveMessage::DecodeResponseL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::DecodeResponseL()
    {
    FallThrough();
    return;
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::ConnectToIapL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::ConnectToIAPL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg open IAP") );
#endif
    if ( !iMmsSettings->LocalMode() )
        {
        iError = iMmsSettings->ValidateSettings();
        }
    CMmsBaseOperation::ConnectToIAPL();
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::InitializeSessionL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::InitializeSessionL()
    {

    // Here we should connect to MMSC

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg Connecting") );
#endif

    // for testing purposes our messages are in files in the folder
    // defined in iMmsSettings->iRootFolder.
    if ( iMmsSettings->LocalMode() )
        {
        HBufC* filename = HBufC::NewL( iMmsSettings->LocalModeIn().Length() );
        CleanupStack::PushL( filename );
        TPtr fileNamePtr = filename->Des();
        fileNamePtr.Copy( iMmsSettings->LocalModeIn() );
        iFs.SetSessionPath( fileNamePtr );
        CleanupStack::PopAndDestroy( filename );
        }

    CMmsBaseOperation::InitializeSessionL();

    }

// ---------------------------------------------------------
// CMmsReceiveMessage::SubmitTransactionL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::SubmitTransactionL()
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg Receiving") );
#endif
    iEntryUnderConstruction = KMsvNullIndexEntryId;
    iResponse->Reset(); // Reset sets the correct version
    iAlreadyDeferred = EFalse;
    iMmsHeaders->Reset();
    iEncodeBuffer->Reset();

    // We don't totally give up in case of an error.
    // We may have a long list of entries to handle.
    // One entry may be garbage while others will be ok.
    
    // Call to IsConnected() clears error for a new entry or
    // sets the error if we are not connected. If we are not
    // connected, we must terminate the state machine loop
    if ( !IsConnected() )
        {
        return;
        }

    TMsvEntry entry;

    // Load the notification. If an error occurs, we cannot continue
    // with this notification.
    if ( !LoadNotificationL( entry ) )
        {
        // LoadNotificationL sets the state machine active if we can continue
        // to next entry.
        // If the situation is hopeless, the state machine is not active, and
        // the loop terminates
        return;
        }


    TInt size = iNotification->MessageSize();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- message size %d"), size);
#endif
    // If the size in notification is bigger than the maximum size we can receive
    // use the maximum size instead for the memory checks (we are not receiving
    // more than that anyway)
    if ( size > iMmsSettings->MaximumReceiveSize() )
        {
        // reduce the size to maximum allowed
        size = iMmsSettings->MaximumReceiveSize();
        }

    // Now we must check if the message should be rejected or deferred
    // The function IsNotificationInsaneL() checks if the notification
    // has incorrect message type or incorrect major version number.
    // If the message is unrecognized, the response will be "unrecognized"
    // The function sets the status, and later the response will be sent
    // but there is no need to check for expiration or disk space in
    // case the message will be rejected anyway.
    
    if ( !IsNotificationInsaneL() )
        {
        // Check if message has expired.
        // If it has, and we are in automatic mode, there is no need
        // to send a response.
        if ( !CheckExpirationL() )
            {
            return;
            }

        // Check disk space.    
        // we must add the safety margin here...
        size += KMmsDiskSafetyMargin;

        // Query about disk space.
        if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
            &iFs, size, iMessageDrive ) )
            {
            iNotification->SetStatus( KMmsMessageStatusDeferred );
            iError = KErrDiskFull;
            }
        }

    // if we have got an error for this message (KErrNoMemory or
    // KErrDiskFull) we must store the information in the notification
    // entry to discern these situations from cases where there
    // is something wrong with the actual transaction.
    if ( iError != KErrNone )
        {
        TInt error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
        // If we cannot update our entry - that's too bad
        if ( error == KErrNone )
            {
            TMsvEntry tEntry;
            tEntry = iServerEntry->Entry();
            tEntry.iError = iError;
            iServerEntry->ChangeEntry( tEntry );
            }
        // we clear the error now. The rest of the branching
        // is not based on iError but on the notification status
        iError = KErrNone;
        // Release the entry
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        }

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- notification status %d"), iNotification->Status() );
    switch ( iNotification->Status() )
        {
        case KMmsMessageStatusRejected:
            TMmsLogger::Log( _L("- rejected") );
            break;
        case KMmsMessageStatusDeferred:
            TMmsLogger::Log( _L("- deferred") );
            break;
        case KMmsMessageStatusRetrieved:
            TMmsLogger::Log( _L("- retrieved") );
            break;
        case KMmsMessageStatusUnrecognized:
            TMmsLogger::Log( _L("- unrecognized") );
            break;
        default :
            TMmsLogger::Log( _L("- whatever") );
            break;
        }
#endif

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- receiving mode: %d"), iReceivingMode );
#endif

    // Sending the response tells the MMSC that the 
    // notification has been received and understood.
    // If we do not want to fetch the message the
    // WantToFetchThisL() subroutine will change the state
    // so that after return the response will be sent to MMSC
    
    if ( !WantToFetchThisL( entry ) )
        {
        return;
        }

    // Now it seems that we are supposed to fetch something
    // The notification has cleared all our tests and fetching is on

    // Now we should start a GET transaction with the
    // URI we obtain from the headers
           
    // If we fail to get anything because the message has expired,
    // we set iEntryUnderConstruction to KMsvNullIndexEntryId
    // Otherwise we must reschedule the receive

    // At this point we will invoke the transaction that sends
    // our get request to MMSC and when the transaction completes,
    // the data has been stored in our encode buffer, if the status
    // is KErrNone. Otherwise the completion status will contain the
    // error code.

    if ( iMmsSettings->LocalMode() )
        {
        LocalModeFetchL();
        // if only local mode, we complete ourselves
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("- local mode, completing fetch with status %d"), iError );
#endif
        if ( !IsActive() )
            {
            // If Local mode fetch fails it already calls ChangeStateL() and becomes active
            // we cannot set us active a second time
            FallThrough();
            }
        }
    else // global mode
        {
        // Chunked mode in global mode, too
        // chunked mode is turned on here - other changes are generic
        DoCreateEntryL();
        iDecoder->InitializeChunkedMode( *iEntryWrapper, *iMmsHeaders, *iEncodeBuffer );
        // end of chunked preparation
        
        // we get this far only if we are connected
        HBufC* uri = HBufC::NewL( iNotification->ContentLocation().Length() + KMMSBufferExtra );
        CleanupStack::PushL( uri );
        uri->Des().Copy( iNotification->ContentLocation() );
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("...Fetching from URI: %S"), uri );
#endif
        iMmsSession->FetchMessageL(
            *uri, 
            *iEncodeBuffer,
// Use this buffer size when in chunked mode             
            KMmsChunkedBufferSize, // tell that only this buffer size is needed
            *iEncoder,
            *iDecoder,
            iStatus );
        CleanupStack::PopAndDestroy( uri );
        SetActive();
        }

    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
TBool CMmsReceiveMessage::IsConnected()
    {
    if ( iConnected )
        {
        // We clear the error at this point to tell that this message
        // is all right so far. Notifications for failed messages
        // remain in the selection, and they are rescheduled later
        // The error is saved in the schedule data for the notification.
        iError = KErrNone;
        }
    else
        {
        // We are in a totally hopeless mess.
        // We don't have a connection. iError should contain the reason
        // all we can do is give up
        // If we return from RunL without becoming active again
        // RunL completes.
        // That's all we can do here...
        // The selection arrays have information about
        // which messages have succeeded and which not.

        // DoComplete sets an error code to those entries in the
        // failed list that do not already have their error set.
        // If we don't know what went wrong, we just say "disconnected"
        if ( iError == KErrNone )
            {
            iError = KErrDisconnected;
            }
        }
    return iConnected;    
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsReceiveMessage::LoadNotificationL( TMsvEntry& aEntry )
    {
    // set entry to next notification in list
    iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( iError != KErrNone )
        {
        // The code called by SkipEntryL will make us active again
        // We may now try the next entry.
        SkipEntryL();
        return EFalse;
        }

    aEntry = iServerEntry->Entry();
    if ( aEntry.iMtmData2 & KMmsNotifyResponseSent )
        {
        iAlreadyDeferred = ETrue;
        }

    CMsvStore* store = NULL;

    TRAP (iError, {store = iServerEntry->ReadStoreL();});
    // We have managed to generate some mysterious notifications
    // where the ReadStoreL fails.
    // These must be deleted, as they are garbage.
    // I assume they are used by someone else.
    // This means we just delete them from our list and forget them
    // If we cannot get a read store, we cannot do anything with the
    // schedule either.
    if ( iError == KErrNoMemory )
        {
        // Out of memory - hopeless.
        // We return without becoming active again and the 
        // whole loop completes.
        // We should try to disconnect, but if there is a serious
        // memory problem I doubt that anything will work.

        // Release the entry
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        return EFalse;
        }
        
    if ( iError != KErrNone )
        {
        // don't discard the entry completely now
        delete store;
        store = NULL;
        // The code called by SkipEntryL will make us active again
        SkipEntryL();
        return EFalse;
        }
        
    // We have access to the notification
    CleanupStack::PushL( store );
    iNotification->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    
    // We must clear deferred status always:
    // If we did not have enough memory the first time,
    // the message was deferred. If we now have enough 
    // memory the old status must be cleared

    iNotification->SetResponseStatus( 0 ); // clear possible old status
    iNotification->SetResponseTextL( TPtrC() ); // clear possible old text

    if ( iNotification->Status() == KMmsMessageStatusDeferred )
        {
        iNotification->SetStatus( 0 );
        }

    // We must check if message fetching is deferred or totally off.
    // if iReceivingMode == EMmsReceivingReject, notification has been deleted already.
    if ( !iMmsSettings->FetchOverride() &&
        iNotification->Status() == 0 &&
        ( iReceivingMode == EMmsReceivingManual || iReceivingMode == EMmsReceivingPostpone ) )
        {
        iNotification->SetStatus( KMmsMessageStatusDeferred );
        }
    
    // We get this far only if we are connected
    aEntry.SetConnected( ETrue );
    aEntry.SetSendingState( KMsvSendStateSending );
    aEntry.iError = KErrNone; // optimistic...
    iServerEntry->ChangeEntry( aEntry );

    // Release the entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    // Continue with this notification
    return ETrue;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsReceiveMessage::CheckExpirationL()
    {
    
    // If the message has expired, it can be rejected without further ado
    // We allow a little slack here, if it expired just a few seconds ago,
    // we still have hope...
    // use universal time in case user changes location
    TTime now;
    TMsvEntry entry;
    TBool canContinue = ETrue; // optimistic

    now.UniversalTime();
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("MMS terminal universal datetime: ") );
    CMmsBaseOperation::LogDateL( now );
    TMmsLogger::Log( _L("MMS message expiry datetime:") );
    CMmsBaseOperation::LogNetworkFormatDateL( iNotification->ExpiryDate() );
#endif
    if ( ( ( iNotification->ExpiryDate() + iMmsSettings->ExpiryOvershoot() ) * KMmsMillion ) <
        now.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64() )
        {
#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("message has expired by %d seconds"),
            ( now.MicroSecondsFrom( TTime( KMmsYear1970String ) ).Int64())/KMmsMillion -
            iNotification->ExpiryDate() - iMmsSettings->ExpiryOvershoot() );
#endif
        // If the notification is in inbox or in MMBox folder, fetch will continue.
        if( entry.Parent() == KMsvGlobalInBoxIndexEntryId  ||
            entry.Parent() == iMmsSettings->MMBoxFolder() )
            {
            // Mark the notification expired
            iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
            if ( iError == KErrNone )
                {
                entry = iServerEntry->Entry();
                entry.iMtmData2 |= KMmsMessageExpired;
                iServerEntry->ChangeEntry( entry );
                }
            // Release the entry
            iServerEntry->SetEntry( KMsvNullIndexEntryId );

#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Expired notification. Fetch continues") ); 
#endif
            }
        else
            {
            // iError == KErrNone, the last stage will just remove our entry from the list
            // There is no need to send any reply to an expired message.
            // The MMSC will not be interested in getting a "reject"
            // response to an expired message.
            iResponse->SetStatus( KMmsMessageStatusRejected );
            iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
            // return to the beginning of the state machine loop
            iState = EMmsOperationUpdatingEntryStatus;
            // The code called by ChangeStateL will make us active again
            ChangeStateL();
            canContinue = EFalse; // loop to next state
            }
        }
    return canContinue; // can continue
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsReceiveMessage::IsNotificationInsaneL()
    {
    TBool isInsane = EFalse; // optimistic
    // If the message is unrecognized, the response will be "unrecognized"
    // Either the message type is unknown, or the major version number differs
    if  ( ( iNotification->MmsVersion() & KMmsMajorVersionMask ) !=
        ( iMmsSettings->MmsVersion() & KMmsMajorVersionMask ) )
        {
        // major versions differ, send back 1.0 version response
        iResponse->SetMmsVersion( KMmsVersion1 );
        iNotification->SetStatus( KMmsMessageStatusUnrecognized );
        iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
        isInsane = ETrue;
        }
    else if ( iNotification->MessageType() != KMmsMessageTypeMNotificationInd && 
        iNotification->MessageType() != KMmsMessageTypeMBoxDescr )
        { 
        // We have been pushed something we cannot handle.
        // delivery reports have been picked out earlier, so they cause
        // no problems here.
        iNotification->SetStatus( KMmsMessageStatusUnrecognized );
        iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
        isInsane = ETrue;
        }
    return isInsane;
    }
       
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsReceiveMessage::WantToFetchThisL( TMsvEntry& aEntry )
    {
    if ( ( iReceivingMode != EMmsReceivingAutomatic && !iMmsSettings->FetchOverride() ) ||
        iNotification->Status() == KMmsMessageStatusRejected ||
        iNotification->Status() == KMmsMessageStatusDeferred ||
        iNotification->Status() == KMmsMessageStatusRetrieved ||
        iNotification->Status() == KMmsMessageStatusUnrecognized )
        {
        // We don't want this message for one reason or another
        // We shall send reject message or deferred message and
        // remove this notification from the scheduling.
        // If the fetching is deferred, the notification should remain
        // in the machine waiting for better times.
        // If the message is rejected, the notification is deleted.
        // In both cases the schedule must be removed. A deferred
        // notification is not rescheduled before the message fetching
        // is turned on again.

        // If the whole message is in the extended notification,
        // the status is "retrieved"

        // However, if the "deferred" response has already been sent,
        // we should not send it again

        iResponse->SetStatus( iNotification->Status() );

        if ( iResponse->Status() != KMmsMessageStatusRejected &&
            iResponse->Status() != KMmsMessageStatusDeferred &&
            iResponse->Status() != KMmsMessageStatusRetrieved &&
            iResponse->Status() != KMmsMessageStatusUnrecognized )
            {
            // We have an unlucky message. The notification arrived
            // while we were supposed to fetch it, but before it was
            // actually fetched, receiving was turned off.
            // We try to defer it hoping it can be fetched before it
            // expires.
            // A message should be scheduled for fetching only a few
            // seconds after the notification arrives, so that if the 
            // user manages to turn the fetching off before we fetch 
            // the message we can assume that he does not want this one either
            // (at least not now). We are a bit conservative, though,
            // we don't reject this one as it was supposed to be fetched
            // before it fell into the time trap.
            iResponse->SetStatus( KMmsMessageStatusDeferred );
            }

        if ( aEntry.iMtmData2 & KMmsNotifyResponseSent )
            {
            // if the notify response has already been sent for this message,
            // we don't send it again.
            // When the message fetch state is switched back on,
            // server Mtm must check the pending notifications and
            // reschedule them for fetching.
            // An already deferred notification should not be in our
            // list, but I'm paranoid and check in case some notification
            // has fallen into a time trap.
            iAlreadyDeferred = ETrue;
            iState = EMmsOperationStoringResponseData;
            }
        else
            {
            // The function called by ChangeStateL will make us active again
            iState = EMmsOperationStoringResponseData;
            }
        // We are not fetching this message.
        // Either we send a response to MMSC or we just move to next notification.
        ChangeStateL();
        return EFalse;
        }
    return ETrue;
    }
        
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::DoCreateEntryL()
    {
    
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg DoCreateEntryL") );
#endif

    // We create a message entry into the inbox
    // it is invisible and in preparation until
    // we decide that we have successfully fetched the message
    // The entry must exist before the message is fetched, because
    // decode puts data into an existing entry, and
    // creates children to the entry for attachments.

    // If we have failed in storing data to the entry, it may be non-zero
    // We try to delete it before creating a new one

    if ( iEntryUnderConstruction != KMsvNullIndexEntryId )
        {
        if ( iServerEntry->SetEntry( KMsvGlobalInBoxIndexEntryId ) == KErrNone )
            {
            iServerEntry->DeleteEntry ( iEntryUnderConstruction );
            }
        iEntryUnderConstruction = KMsvNullIndexEntryId;
        }

    // If the creation of the entry is successful, we
    // set our entry to point to the newly created entry
    // to get data content to it.
    if ( iError == KErrNone )
        {
        TMsvId targetId = KMsvGlobalInBoxIndexEntryId;

        TMsvEntry tEntry;

        // set all relevant flags in tMsvEntry
        tEntry.iType = KUidMsvMessageEntry;
        tEntry.iMtm = KUidMsgTypeMultimedia;
        tEntry.iServiceId = iService;
        tEntry.SetUnread( ETrue );
        tEntry.SetNew( ETrue );
        tEntry.SetVisible( EFalse );
        tEntry.SetComplete( EFalse );
        tEntry.SetInPreparation( ETrue );
        tEntry.SetReadOnly( EFalse );

        // Query about disk space.
        if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
            &iFs, KMmsIndexEntryExtra, iMessageDrive ) )
            {
            // we use standard error code here
            iError = KErrDiskFull;
            }
        if ( iError == KErrNone )
            {
            iError = iServerEntry->SetEntry( targetId );
            }

        if ( iError == KErrNone )
            {
            iError = iServerEntry->CreateEntry( tEntry );
            }
        iEntryUnderConstruction = tEntry.Id();
        if ( iError == KErrNone )
            {
            iError = iEntryWrapper->SetCurrentEntry( iEntryUnderConstruction );
            }
        }
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::StoreResponseL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::StoreResponseL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg Store Response") );
#endif
    TInt error;
    TInt fatal = EFalse; // fatal error, no use to retry

    // If we have got this far, we don't return to CreateEntry anymore
    // even if we encounter backup/restore problems later.

    if ( iError == KErrNone )
        {
        // if we have no error, me must retry to make the entry visible
        iCriticalState = EMmsOperationStoringResponseData;
        }
    else
        {
        iCriticalState = EMmsOperationSendingAck;
        }

    // At the moment we should not receive any extra message types,
    // so that the only error of this type would
    // be wrong type of message either from WAP stack or from MMSC.
    // Send request is all right in local mode, as we get back
    // the messages we send ourselves.
    // We accept the send requests as retrieve confirmations,
    // as they are importand during testing phase:
    // We can put any message someone has sent into the folder
    // representing the simulated MMSC and try to fetch it as
    // if it were a real message.
    // We can't check the message type if we have encountered an error.
    // In that case the headers may not have been correctly decoded
    if ( iError == KErrNone &&
        iResponse->Status() != KMmsMessageStatusRejected &&
        iResponse->Status() != KMmsMessageStatusDeferred &&
        iResponse->Status() != KMmsMessageStatusRetrieved &&
        iMmsHeaders->MessageType() != KMmsMessageTypeMRetrieveConf &&
        iMmsHeaders->MessageType() != KMmsMessageTypeMSendReq )
        {
        iResponse->SetStatus( KMmsMessageStatusUnrecognized );
        iError = KMmsErrorStatusUnsupportedMessage;
        }

    // If we have managed to decode the response, it may contain a status
    // that must be mapped to an error. The error may be permanent or temporary

    if ( iError == KErrNone )
        {
        // we have got a response from MMSC, and decoded it, but in version 1.1
        // or later the response may still contain an error code.
        // if response text length was 0, it was deleted

        // - if error is permanent, notification will be deleted.
        // - if error is transient, response text is copied to notification
        // and UI components may show it to the user if needed.
        
        // iError is set to the correct value depending on response status
        // "fatal" indicates if the error is permanent or temporary
        fatal = MapErrorStatus( iMmsHeaders->ResponseStatus() );
        }

    // If we get a transient failure from MMSC, we store the possible text and status code
    // into our notification. UI may show the text to the user in case there is something
    // the user might do about the failure. Permament failures are stored in the message
    // entry.
    // The application id is also copied to the notification in case application id is present
    // in the message but not in the notification.
    // If the application has not been registered or memory runs out, the user may be
    // informed that the notification refers to a message that belongs to an application.
        
    CopyDataFromMessageToNotificationL();

    // If there has been an error and logging is allowed, dump the buffer into a file
    // to support error analysis later
    DumpIfNeededL();
    
    // We must try to finalize even if we have encountered an error.
    // If the decode has failed, iError contains something like "KErrCorrupt".
    // If the operation has been successful, we make the entry visible
    // and remove the in preparation flag. The user should then be able
    // to see the new message entry.

    // free the memory we don't need anymore
    if ( iEncodeBuffer )
        {
        iEncodeBuffer->Reset();
        }
    TMsvEntry tEntry;
    
    // Check if the message is going to an unregistered application.
    // If it is, it's going to be rejected
    iRegistered = EFalse;
    
    if ( iMmsHeaders->ApplicId().Length() > 0 )
        {
        iRegistered = RegisteredL( iMmsHeaders->ApplicId() );
        if ( !iRegistered )
            {
            // not registered - reject it
            iResponse->SetStatus( KMmsMessageStatusRejected );
            // we clear whatever error we may have because we are
            // going to reject the message anyway.
            iError = KErrNone;
            }
        }
        
    // If the message is going to an application we must check if we can free disk
    // space by deleting older messages addressed to the same application.
    
    if ( iError == KErrDiskFull && iRegistered )
        {
        // Check if we can free disk space and reschedule the fetch.
        // In manual mode the fetch will not be rescheduled.
        // If we can free memory we will change the error code to KMmsErrorApplicationDiskFull
        // If we cannot free memory, the error code is left as is and the user must
        // delete something else in order to be able to receive the message.
        DeleteApplicationMessagesL();
        }

    // If we have tried to fetch a message, and got 
    // EMRUExceeded, we'll reject the message. If it
    // was already deferred, no new response is sent.
    // The notification will be deleted, it is hopeless.

    if ( iError == KMmsErrorEMRUExceeded )
        {
        iResponse->SetStatus( KMmsMessageStatusRejected );
        iFailed->Delete( iCurrentMessageNo - 1 );
        iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
        }

    if ( iError != KErrNone ||
        iResponse->Status() == KMmsMessageStatusRejected ||
        iResponse->Status() == KMmsMessageStatusDeferred ||
        iResponse->Status() == KMmsMessageStatusRetrieved ||
        iResponse->Status() == KMmsMessageStatusUnrecognized )
        {
        // If the failure was fatal (permanent error from MMSC)
        // we don't send any response back. If the failure was
        // not fatal, we send "deferred" hoping we can retry later
        if ( fatal )
            {
            // The notification is deleted, as we have got an MMS message.
            iFailed->Delete( iCurrentMessageNo - 1 );
            iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
            // we don't send back a response if failing was fatal.
            iResponse->Reset();
            }
        else if ( iError == KMmsErrorHTTPConfiguration ||
            iError == KMmsErrorHTTPNotFound ||
            iError == KMmsErrorHTTPServerDown ||
            iError == KMmsErrorHTTPClientError )
            {
            // if we have got a HTTP error, there is no sense to send ack back
            iResponse->Reset();
            }
        else if ( !iAlreadyDeferred &&
            iResponse->Status() != KMmsMessageStatusRejected &&
            iResponse->Status() != KMmsMessageStatusRetrieved &&
            iResponse->Status() != KMmsMessageStatusUnrecognized)
            {
            iResponse->SetStatus( KMmsMessageStatusDeferred );
            EncodeNotifyResponseL();
            }
        else if ( iResponse->Status() == KMmsMessageStatusUnrecognized || 
            iResponse->Status() == KMmsMessageStatusRejected)
            {
            EncodeNotifyResponseL();
            }
        else if ( iResponse->Status() == KMmsMessageStatusRetrieved )
            {
            EncodeNotifyResponseL();
            iFailed->Delete( iCurrentMessageNo - 1 );
            iSuccessful->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
            }
        else
            {
            // We have nothing to send, we hope we can try
            // again later.
            iResponse->Reset();
            }
        }

#ifndef _NO_MMSS_LOGGING_
    // log the response status we are about to send
    TMmsLogger::Log( _L("- Response status to be: %d"), iResponse->Status() );
#endif

    // If we have no entry, the fetching has failed. If the failing was fatal,
    // the notification has been appended to the iBad list.
    // It will be deleted from the iFailed list later.
    // Those entries that end in the iBad list are just
    // deleted, not rescheduled.
    
    TInt applicationMessageStatus = 0;
    
    // If we got an error, we delete the entry under construction.
    // But if the error is "fatal" it means MMSC returned some error message,
    // and the error message entry must be kept and shown to the user
    // The corresponding notification remains in the failed list
    // in order to be rescheduled.
    // If the notification was bad, the corresponding notification
    // has already been appended to bad list to be removed from 
    // task scheduler.
    if ( iEntryUnderConstruction != KMsvNullIndexEntryId && iError != KErrNone && !fatal )
        {
        if ( iMmsSettings->LocalMode() )
            {
            CloseLocalFileL( EFalse );
            }
        // if setting server entry to inbox fails, deletion of entry will fail,
        // but there is nothing we can do about it.    
        iServerEntry->SetEntry( KMsvGlobalInBoxIndexEntryId );
        iServerEntry->DeleteEntry ( iEntryUnderConstruction );
        iEntryUnderConstruction = KMsvNullIndexEntryId;
        }
        
    if ( iEntryUnderConstruction != KMsvNullIndexEntryId )
        {
        // We don't care if this leaves. We just do our best.
        // If this function fails, our message will have some extra parts
        TRAP( error, HandleMultipartAlternativeL() );
        }
    
    // If we have an entry we have either successfully fetched a message
    // or received an error message about a fatal (permanent) error from MMSC
    // The entry must now be made visible to the user.    
    if ( iEntryUnderConstruction != KMsvNullIndexEntryId )
        {
        error = iServerEntry->SetEntry( iEntryUnderConstruction );

        if ( error == KErrNone )
            {
            // we don't check disk space here, as this
            // is local mode only: We just save the message id
            // for testing purposes. The message id is about 5 bytes
            // And we clear possible TID, that makes message shorter than before.
            CMsvStore* store = NULL;
            TRAP ( error, store = iServerEntry->EditStoreL() )
            if ( store )
                {
                CleanupStack::PushL( store );
                }
            if ( iMmsSettings->LocalMode() &&
                iMmsHeaders->MessageId().Length() == 0 )
                {
                iMmsHeaders->SetMessageIdL(
                    iNotification->ContentLocation().Mid(
                    iMmsSettings->LocalModeIn().Length() ) );
                }
            if ( iAlreadyDeferred )
                {
                // we need to save the TID from the message to the response
                iResponse->SetTidL( iMmsHeaders->Tid() );
                }
            // clear TID from the message to prevent the same TID from being used later
            // in case the message is copied around.
            iMmsHeaders->SetTidL( TPtrC8() );
            // Data is copied in case the message does not contain sender or subject
            // Data is copied only if disk space is not below critical level
            CopyDataFromNotificationToMessageL();
            if ( store )
                {
                TRAP ( error, 
                    {
                    iMmsHeaders->StoreL(*store);
                    store->CommitL();
                    });
                // If we managed to create the store without leaving, it is on cleanup stack
                CleanupStack::PopAndDestroy( store );
                store = NULL;
                }
            if ( iAlreadyDeferred )
                {
                // restore the TID
                iMmsHeaders->SetTidL( iResponse->Tid() );
                }

            tEntry = iServerEntry->Entry();
            
            // If message contains application id, check, 
            // if a folder, the name of which is application id is found
            // and move the message to this folder.
            // If the application is not registered, the message is deleted
            applicationMessageStatus = MoveToApplicationFolderIfNeededL( tEntry );
            
            if ( applicationMessageStatus != KMmsMessageForUnregisteredApplication )
                {
                // reload the entry in case the message was moved
                tEntry = iServerEntry->Entry();
                            
                SetIndexEntryBitsForReceivedMessage( tEntry );

                // if we cannot make our entry visible because of backup/restore, we must retry.
                error = iServerEntry->ChangeEntry( tEntry );

                // This is the critical point.
                // We must return before iEntryUnderConstruction is set to KMsvNullIndexEntryId
                // and the notification list is edited

                if ( error <= (TInt) KMsvMediaUnavailable &&
                    error >= (TInt) KMsvIndexRestore &&
                    !iDoNotWaitForBackupEnd )
                    {
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- must wait for backup end ") );
#endif
                    iState = EMmsOperationWaitingForBackupEnd;
                    // This will take us back to RunL
                    WaitForBackupEnd();
                    return;
                    }
                }

            if ( iMmsSettings->LocalMode() )
                {
                CloseLocalFileL( ETrue );
                }

            // done with this entry
            iEntryUnderConstruction = KMsvNullIndexEntryId;
            }

        // The task manager seems to handle deletion of entries
        // gracefully nowadays. We can just delete handled entries
        // We delete the notification as soon as possible
        // as we don't want it hanging around after the message
        // has been successfully fetched

        if ( !fatal )
            {
            // Check duplicate before deleting notification entry
            if ( iNotificationParent == KMsvGlobalInBoxIndexEntryIdValue ||
                ( iNotificationParent == iMmsSettings->MMBoxFolder() ))
                {
                // For the time being Java wants these notifications to be deleted
                // and applicationMessageStatus is always 0.
                // This may change for other type of applications.
                ClearOperationL( applicationMessageStatus );
                }
            else
                {
                // If the notification is not visible to the user,
                // we don't need to save the application routing status.
                // The rest of the handling can proceed normally - 
                // the notification will be deleted
                applicationMessageStatus = 0;
                }

            // If the notification does not need to be left around to inform user,
            // it can be deleted now                    
            if ( applicationMessageStatus == 0 )
                {
                error = iServerEntry->SetEntry( iNotificationParent );
                if ( error == KErrNone )
                    {
                    iServerEntry->DeleteEntry( iSelection->At( iCurrentMessageNo - 1 ) );
                    }
                else
                    {
                    // we use this list as backup:
                    // if we cannot delete now, we try again later.
                    iSuccessful->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
                    }
                }
            // Even if the notification was left in the inbox,
            // it is deleted from the failed list because it needs no more handling    
            iFailed->Delete( iCurrentMessageNo - 1 );
            
            // All went well. We can acknowledge the delivery.
            // If the MMSC is not interested in the acknowledgement,
            // it has sent us no TID.
            // If there is no TID, the response won't be sent.
            // If Retrieve confirmation has not been sent, it must be
            // sent instead of acknowledge indication
            
            // We don't send an ack back if we have received a response
            // from MMSC that indicates a fatal failure.
            // Most likely the message was expired or has disappeared
            // for some other reason (or did not exist in the first place)
            
            iResponse->SetStatus( KMmsMessageStatusRetrieved );
            if ( !iAlreadyDeferred )
                {
                EncodeNotifyResponseL();
                }
            else
                {
                EncodeAcknowledgeIndicationL();
                }
            }
        }

    if ( iFileOpen )
        {
        iFile.Close(); // just in case...
        iFileOpen = EFalse;
        }
        
    // release entry in case we were holding the service entry
    // We should not leave our entry pointing to the serive entry 
    // when we become active, others may want to use it.
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    // We propagate the original error no matter what has happened here
    User::RequestComplete( status, iError );

    }

// ---------------------------------------------------------
// CMmsReceiveMessage::SendAckL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::SendAckL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg Sending response to receive request") );
#endif

    // Send response to MMSC.
    // response is in iResponse.
    // If no reply should actually be sent, length of TID is 0.

    // We may try to send response even if we have encountered an error.
    // We try to send "deferred" if we couldn't fetch the message.
    // We hope the MMSC will try to keep the message in that case.

    // iResponse has Tid, if it contains something reasonable.

    if ( iResponse->Tid().Length() > 0 )
        {
        if ( !iMmsSettings->LocalMode() )
            {
            // send the response
            iMmsSession->SendMessageL(
                iUri->Des(),
                *iEncodeBuffer,
                *iEncoder,
                *iDecoder,
                iStatus );
            SetActive();
            }
        else
            {
            iStatus = KRequestPending;
            SetActive();
            TRequestStatus* status = &iStatus;
            // We propagate the original error no matter what has happened here
            User::RequestComplete( status, iError );
            }
        }
    else
        {
        // No response to send.
        // Just continue to next state.
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        // We propagate the original error no matter what has happened here
        User::RequestComplete( status, iError );
        }
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::UpdateEntryStatusL()
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::UpdateEntryStatusL()
    {

#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Receivemsg Checking response success") );
#endif
    
    TInt error;
    TMsvEntry tEntry;
    TInt count = iBad->Count() - 1;

    // update scheduling info for this notification
    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( error == KErrNone )
        {
        tEntry = iServerEntry->Entry();
        error = tEntry.iError;
        if ( error == KErrNone )
            {
            error = iError;
            }
        // In some rare cases accessing the store for the notification may fail
        // in that case the schedule is not updated.
        TRAP( error,
            {
            CMsvStore* store = iServerEntry->EditStoreL();
            CleanupStack::PushL( store );
            CMmsScheduledEntry* mmsScheduledEntry =
                CMmsScheduledEntry::NewL( iServerEntry->Entry() );
            CleanupStack::PushL( mmsScheduledEntry );
            mmsScheduledEntry->RestoreL( *store );
            UpdateRecipient( iError, *mmsScheduledEntry );
            mmsScheduledEntry->StoreL( *store );
            store->CommitL();
            CleanupStack::PopAndDestroy( mmsScheduledEntry );
            CleanupStack::PopAndDestroy( store );
            });
        }

    // If the current notification has entered the "bad" list,
    // it should not be rescheduled.
    // For example, the message may be "deferred" because it is too
    // big to ever fit in our phone. It must be deleted from the "failed"
    // list, because it should not be rescheduled.

    TInt count2 = iFailed->Count() - 1;
    if ( count >= 0 &&
        count2 >= ( iCurrentMessageNo - 1 ) &&
        iBad->At( count ) == iFailed->At( iCurrentMessageNo - 1 ) )
        {
        // should not be rescheduled.
        iFailed->Delete( iCurrentMessageNo - 1 );
        }
    else if ( iError == KErrNone )
        {

#ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("Receivemsg all is well") );
#endif
        // All is well.
        // If message was rejected the notification can be thrown away
        
        if ( iResponse->Status() == KMmsMessageStatusRejected ||
             iResponse->Status() == KMmsMessageStatusUnrecognized )
            {
            // The notifications in the received list will be deleted.
            // If the message was rejected, the notification will be
            // deleted.
            error = iServerEntry->SetEntry( iNotificationParent );
            if ( error == KErrNone )
                {
                iServerEntry->DeleteEntry( iSelection->At( iCurrentMessageNo - 1 ) );
                }
            else
                {
                // we use this list as backup:
                // if we cannot delete now, we try again later.
                iSuccessful->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
                }
            iFailed->Delete( iCurrentMessageNo - 1 );
            }

        if ( iResponse->Status() == KMmsMessageStatusDeferred )
            {

#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("Receivemsg responsestatus is deferred.") );
#endif
            // we have sent a "deferred" response.
            // We should keep the notification, but not reschedule it.
            // If we delete it from failed list, it wont get rescheduled.
            // If we don't add it into received list, it will not be deleted.
            // We must mark, that the "deferred" response was successfully sent
            // so that we will not send it again later.

            error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
            // If we cannot update our entry - that's too bad
            // it just means that the response may be sent a second time...
            // The system must not get mad about that.
            // The system may be unreliable anyway, and some response may get lost.
            // We just do our best.
            if ( error == KErrNone )
                {
                iNotification->SetStatus( KMmsMessageStatusDeferred );
                CMsvStore * store = iServerEntry->EditStoreL();
                CleanupStack::PushL( store );
                iNotification->StoreL( *store );
                store->CommitL();
                CleanupStack::PopAndDestroy( store );
                tEntry = iServerEntry->Entry();
                tEntry.iMtmData2 |= KMmsNotifyResponseSent;
                tEntry.iMtmData2 &= ~KMmsDeferredButResponseNotSent;
                tEntry.SetConnected( EFalse );
                tEntry.SetFailed( EFalse );
                tEntry.SetSendingState( KMsvSendStateUnknown );

#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Receivemsg notify response sent") );
#endif

                // change the iMtm type to notification 
                // and move the notification to the inbox
                TBool moveToInbox = EFalse;
                
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- message receiving mode %d"), iReceivingMode );
#endif
                if( iReceivingMode == EMmsReceivingManual && !iMmsSettings->FetchOverride() )
                    {          
                    // Change the iMtm from KUidMsgTypeMultimedia to 
                    // KUidMsgMMSNotification   
                    tEntry.iMtm = KUidMsgMMSNotification;
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("marking as a notification") );
#endif

#ifndef _NO_MMSS_LOGGING_
TMmsLogger::Log( _L("tentry.iMtm is %d"), tEntry.iMtm.iUid );
#endif
                    tEntry.SetVisible( ETrue );
                    tEntry.SetComplete( ETrue );
                    tEntry.SetInPreparation( EFalse );
                    tEntry.SetReadOnly( ETrue );
                    // This is a notification, therefore there is no sending time available.
                    // We can only display the arrival time
                    tEntry.iDate.UniversalTime(); // this is arrival time
                    
                    tEntry.iMtmData1 |= KMmsMessageMobileTerminated;
            
                    // Subject, sender and size are set in CMmsDecode.
                    // Notification cannot contain multiple recipients
                    // no need to check or set the multiple recipient bit
                    
                    moveToInbox = ETrue;
                    }
                iServerEntry->ChangeEntry( tEntry );

                           
                // if manual mode is on and the fetchoverride is not on.
                // we have received a notification and sent a notifyresponse to the MMSC
                // the notification should be moved to the inbox
                if ( moveToInbox )
                    {

#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("Receivemsg notification entry to Inbox") );
#endif
                   
                    if ( iServerEntry->SetEntry( tEntry.Parent() ) == KErrNone )
                        {
                        iServerEntry->MoveEntryWithinService( tEntry.Id(),
                            KMsvGlobalInBoxIndexEntryIdValue );
                        }
                    }
                }
            
            if ( iReceivingMode != EMmsReceivingAutomatic && !iMmsSettings->FetchOverride() )
                {
                // If fetching is on, the message was deferred because of an error.
                // In that case we let it remain in the failed list for retries.
                // If fetching is deferred, manual or off, we don't keep the failed list,
                // The receiving mode must be changed before we retry the operation.

                // Should we remove the schedule...
                iFailed->Delete( iCurrentMessageNo - 1 );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Receivemsg deleting from the iFailedList") );
#endif
                }
            }
        }
    else
        {
        // keep LINT happy
        }

    // release entry in case we were holding the service entry
    // We should not leave our entry pointing to the serive entry 
    // when we become active, others may want to use it.
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    // just return back to the start of the loop
    // to handle next message
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    // We propagate the original error no matter what has happened here
    User::RequestComplete( status, iError );

    }

// ---------------------------------------------------------
// CMmsReceiveMessage::EncodeNotifyResponseL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::EncodeNotifyResponseL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- encoding m-notifyresp-ind") );
#endif
    iResponse->SetMessageType( KMmsMessageTypeMNotifyRespInd );
    iResponse->SetTidL( iNotification->Tid() );
    // message status has been set before we are called.
    iResponse->SetReportAllowed( iMmsSettings->DeliveryReportSendingAllowed() );
    iEncoder->EncodeHeadersL( *iResponse, *iEncodeBuffer );
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::EncodeAcknowledgeIndicationL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::EncodeAcknowledgeIndicationL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("- encoding m-acknowledge-ind") );
#endif
    iResponse->SetMessageType( KMmsMessageTypeAcknowledgeInd );
    
    // The TID has already been set to iResponse earlier, do not override it here
    
    // version value is set by the encoder.
    // This response has no status. It is only sent when the status is "Retrieved"
    iResponse->SetReportAllowed( iMmsSettings->DeliveryReportSendingAllowed() );
    iEncoder->EncodeHeadersL( *iResponse, *iEncodeBuffer );
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::HandleMultipartAlternativeL()
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::HandleMultipartAlternativeL()
    {
    // This is just "best effort"
    // If we cannot properly clear the extra attachments, they are just left "as is"
    
    if ( iMmsHeaders->MultipartType() != KMmsAssignedApplicationVndWapMultipartAlternative )
        {
        return; // nothing to do
        }

    User::LeaveIfError( iEntryWrapper->SetCurrentEntry( iEntryUnderConstruction ) );
    CMsvStore* store = iEntryWrapper->EditStoreL();
    CleanupStack::PushL( store );
    
    MMsvAttachmentManager& attachMan = store->AttachmentManagerL();
    MMsvAttachmentManagerSync& attachManSync = store->AttachmentManagerExtensionsL();
    
    TInt count = attachMan.AttachmentCount();

    if ( count <= 1 )
        {
        CleanupStack::PopAndDestroy( store );
        return; // only one part, keep it
        }

    TInt i;
    CMsvMimeHeaders* mimeHeaders = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mimeHeaders );

    for ( i = count; i > 0; i-- )
        {
        CMsvAttachment* attachInfo = attachMan.GetAttachmentInfoL( i - 1 );
        CleanupStack::PushL( attachInfo );
        TPtrC8 pointer;
        pointer.Set( attachInfo->MimeType() );
        HBufC8* temp = NULL;
        if ( pointer.Length() == 0 )
            {
            // mime type not set. Try mime headers
            mimeHeaders->RestoreL( *attachInfo );
            temp = HBufC8::NewL( mimeHeaders->ContentType().Length() +
                mimeHeaders->ContentSubType().Length() + 1 );
            temp->Des().Copy( mimeHeaders->ContentType() );
            temp->Des().Append( KMmsSlash8 );
            temp->Des().Append( mimeHeaders->ContentSubType() );
            pointer.Set( temp->Des() );
            }
        CleanupStack::PushL( temp );                
        if ( pointer.CompareF( KMmsTextPlain ) != 0 )
            {
            attachManSync.RemoveAttachmentL( i - 1 );
            count--;
            }
        CleanupStack::PopAndDestroy( temp );
        CleanupStack::PopAndDestroy( attachInfo );
        temp = NULL; 
        attachInfo = NULL;
        }

    if ( count > 0 )
        {
        // we have something left in the selection
        // When we commit the store, deleted attachments should disappear
        store->CommitL();
        }

    CleanupStack::PopAndDestroy( mimeHeaders );
    CleanupStack::PopAndDestroy( store );
    iEntryWrapper->SetCurrentEntry( KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMmsReceiveMessage::::EncodePDUL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::EncodePDUL()
    {
    // This state is used to check what must be done to the notification
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReceiveMessage EncodePDUL - Finalize of continue"));
#endif

    iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    
    if ( iError != KErrNone )
        {
        // The code called by SkipEntryL will make us active again
        SkipEntryL();
        return;
        }
        
    TMsvEntry tEntry;
    tEntry = iServerEntry->Entry();
        
    if ( iReceivingMode == EMmsReceivingAutomatic || iMmsSettings->FetchOverride() )
        {
        // if notification is in Inbox, set the fetch flags on
        if ( tEntry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
            {
            // mark the flags to reserve the notification for fetch
            MarkNotificationOperationReserved( tEntry, KMmsOperationFetch );
            
            iServerEntry->ChangeEntry( tEntry );
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("mark notification that is being fetched") );
            #endif
            }
        // continue fetching
        FallThrough();
        return;  
        }

    // mode is deferred of reject
    
    // entry is in inbox - clear flags and leave it
    if ( tEntry.Parent() == KMsvGlobalInBoxIndexEntryIdValue )
        {
        tEntry.iMtmData2 &= ~KMmsNewOperationForbidden;
        tEntry.SetReadOnly( ETrue );
        iServerEntry->ChangeEntry( tEntry );
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("notification in inbox. Let it be") );
        #endif
        LoopToNextEntryL();
        return;
        }
    
    // entry is in mms folder or mmbox folder

    // already deferred - leave it and loop to next entry
    if ( tEntry.iMtmData2 & KMmsNotifyResponseSent )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("notification already deferred. Let it be") );
        #endif
        tEntry.iMtmData2 &= ~KMmsNewOperationForbidden;
        iServerEntry->ChangeEntry( tEntry );
        LoopToNextEntryL();
        return;
        }  
        
    // reject mode is on 
    if ( iReceivingMode == EMmsReceivingReject )
        {
        // Let the notification be, if we have received this notification 
        // earlier in the manual or postpone mode when we have roamed.
        // In that case KMmsDeferredButResponseNotSent flag is set.
        // Delete it, if it is a new one that has arrives while we are in reject mode 
        tEntry.iMtmData2 &= ~KMmsNewOperationForbidden;
        iServerEntry->ChangeEntry( tEntry );
        if ( !( tEntry.iMtmData2 & KMmsDeferredButResponseNotSent ) )
            {
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("deleting the notification") );
            #endif
            // we ignore the error. If setting entry to parent fails, deletion will fail
            iServerEntry->SetEntry( tEntry.Parent() );
            iServerEntry->DeleteEntry( iSelection->At( iCurrentMessageNo - 1 ) );
            }               
        LoopToNextEntryL();
        return;
        }
 
    // deferred and response not yet sent
    
    if ( iRegistrationStatus == 0 ) // home network - continue
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("in homenw and deferred mode") );
        #endif
        // continue fetching - will send "deferred" response
        FallThrough();
        return;  
        }
        
    // state is deferred, and we are roaming    
    
    // if the status of the notification is retrieved, this
    // notification will be deleted.
    // This happens if the notification was extended notification 
    // and the whole message arrived in the notification.
    // When roaming, no response is sent. The extended notification
    // is already in the inbox. This notification is deleted.

    CMsvStore* store = iServerEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iNotification->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    store = NULL;
    
    if ( iNotification->Status() == KMmsMessageStatusRetrieved )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L(" the whole extended notification received") );
        TMmsLogger::Log( _L("deleting the notification") );
        #endif
        // ignore error, if setting parent fails, deletion will fail
        iServerEntry->SetEntry( tEntry.Parent() );
        iServerEntry->DeleteEntry( iSelection->At( iCurrentMessageNo - 1 ) );
        LoopToNextEntryL();
        return;
        }
        
    // The notification is not completely fetched, finalize flags    
    
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("Finalizing the notification") );
    #endif
    iNotification->SetStatus( KMmsMessageStatusDeferred );
    store = iServerEntry->EditStoreL();
    CleanupStack::PushL( store );
    iNotification->StoreL( *store );
    store->CommitL();
    CleanupStack::PopAndDestroy( store );
    store = NULL;

    tEntry.SetConnected( EFalse );
    tEntry.SetFailed( EFalse );
    tEntry.SetSendingState( KMsvSendStateUnknown );

    tEntry.iMtmData1 |= KMmsMessageMobileTerminated;

    // When roaming it is not allowed to send a notify response
    // to MMSC.
    tEntry.iMtmData2 |= KMmsDeferredButResponseNotSent;

    // The receiving mode is either manual or postpone, when roaming. 
    // If the manual mode is on, the mtm type is changed to notification
    // and the notification is moved to Inbox.
    // 
    if ( iMmsSettings->ReceivingModeForeign() != EMmsReceivingManual )
        {
        // this is all we need
        iServerEntry->ChangeEntry( tEntry );
        }
    else
        {
        // Change the iMtm from KUidMsgTypeMultimedia to 
        // KUidMsgMMSNotification   
        tEntry.iMtm = KUidMsgMMSNotification; 
        tEntry.iMtmData2 &= ~KMmsNewOperationForbidden;
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("marking as a notification") );
        #endif                  
        // Readonly flag has to be set on in inbox.
        // Not in mms folder
        tEntry.SetReadOnly( ETrue );
        iServerEntry->ChangeEntry( tEntry );
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("moving to inbox") );
        #endif
        // Ignore error from set entry. If it fails, move will fail (not panic)
        // Setting service entry to a folder should not fail.
        iServerEntry->SetEntry( tEntry.Parent() );
        iServerEntry->MoveEntryWithinService( tEntry.Id(), KMsvGlobalInBoxIndexEntryIdValue );
        }
        
    LoopToNextEntryL();

    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::::SkipEntryL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::SkipEntryL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("receivemsg - can't access notification") );
#endif
    // If the notification cannot be accessed, we must try the next one
    // If the notification was not found, or was used by someone else,
    // then nothing can be done. Nothing can be done to the scheduling data.

    // We completely discard only entries that cannot be found
    if ( iError == KErrNotFound )
        {
        iFailed->Delete( iCurrentMessageNo - 1 );
        }
    // we must return to the beginning of the loop without doing anything
    iState = EMmsOperationUpdatingEntryStatus;
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    // The code called by ChangeStateL will make us active again
    ChangeStateL();
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::::LoopToNextEntryL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::LoopToNextEntryL()
    {
    // release entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    // loop to next entry
    iFailed->Delete( iCurrentMessageNo - 1 );
    iState = EMmsOperationUpdatingEntryStatus;
    SelectNextState();
    // The code called by ChangeStateL will make us active again
    ChangeStateL();
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::FolderEntryL
//
// ---------------------------------------------------------
//
TInt CMmsReceiveMessage::FolderEntryL(
    TMsvId aParent, const TDesC& aFolderName, TMsvId& aFolderId )
    {
    aFolderId = KMsvNullIndexEntryId;

    TInt error = KErrNone;
    
    // get a new entry, don't mess up with the original entry.
    CMsvServerEntry* workingEntry = NULL;
    TRAP( error, workingEntry = iServerEntry->NewEntryL( aParent ) );
    CleanupStack::PushL( workingEntry );
    
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( workingEntry ); // workingEntry
        return error;
        }
   
    // Show invisible entries
    TMsvSelectionOrdering ordering =
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByIdReverse, ETrue );
    workingEntry->SetSort( ordering );
    
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    error = workingEntry->GetChildrenWithType( KUidMsvFolderEntry, *selection );
    if ( error == KErrNone )
        {           
        // If selection contains folders, check if the folder is found.
        TInt count = selection->Count();
        for ( TInt i = 0; i < count && aFolderId == KMsvNullIndexEntryId; i++ )
            {
            error = workingEntry->SetEntry( selection->At( i ) );
            if ( error == KErrNone )
                {
                // must be exact match
                if ( workingEntry->Entry().iDetails.Compare( aFolderName ) == 0 )
                    {
                    aFolderId = selection->At( i );
                    }                    
                }                           
            }
        }
    delete selection; 
    selection = NULL;
    CleanupStack::PopAndDestroy( workingEntry );
    workingEntry = NULL;
    
    return error; 
        
    }
 
 // -----------------------------------------------------------------------------
// CMmsReceiveMessage::CreateFolderEntryL
//
// -----------------------------------------------------------------------------
//
TInt CMmsReceiveMessage::CreateFolderEntryL(
    TMsvId aParentFolder, const TDesC& aFolderName, TMsvId& aFolderId )
    {
    
    // Check if the folder already exists under parent folder.
    TInt error = FolderEntryL( aParentFolder, aFolderName, aFolderId );    
    if ( aFolderId != KMsvNullIndexEntryId || error != KErrNone )
        {
        // if error == KErrNone and aFolderId not equal to KMsvNullIndexEntryId
        // the folder already exists, and we don't need to create anything
        return error;   
        }
     
    CMsvServerEntry* serverEntry = NULL;
    TRAP( error, serverEntry = iServerEntry->NewEntryL( aParentFolder ) );
    CleanupStack::PushL( serverEntry );
    
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( serverEntry );
        return error;
        }
          
    // Create a new folder.
    error = serverEntry->SetEntry( aParentFolder );
    
    if ( error == KErrNone )
	    {
        TMsvEntry entry;
	    entry.iType = KUidMsvFolderEntry;
	    entry.iMtm = KUidMsvLocalServiceMtm;
	    entry.iDetails.Set( aFolderName );
	    entry.SetVisible( EFalse );
	    entry.SetInPreparation( EFalse );
	    entry.iServiceId = KMsvLocalServiceIndexEntryId;
	    error = serverEntry->CreateEntry( entry );    
	    aFolderId = entry.Id();    
	    }
	CleanupStack::PopAndDestroy( serverEntry );
    serverEntry = NULL;    
	return error;   
       
    }

    
// ---------------------------------------------------------
// CMmsReceiveMessage::FindDuplicateNotificationL
// 
// ---------------------------------------------------------
//
TInt CMmsReceiveMessage::FindDuplicateNotificationL(
    TMsvId aParent, CMmsHeaders& aHeaders, TMsvId& aDuplicate )
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsReceiveMessage::FindDuplicateNotificationL") );
#endif
    
    aDuplicate = KMsvNullIndexEntryId;
 
    if ( aParent == KMsvNullIndexEntryId )
        {
        return KErrNotSupported;
        }

    // Get a new entry, dont mess up with our original entry
    TInt error = KErrNone;

    CMsvServerEntry* workingEntry = NULL;
    TRAP( error, workingEntry = iServerEntry->NewEntryL( aParent ));
    CleanupStack::PushL( workingEntry );
    
    if ( error != KErrNone )
        {
        CleanupStack::PopAndDestroy( workingEntry );
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
        error = workingEntry->SetEntry( selection->At( i - 1 ) );
        if ( error == KErrNone )
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
                TMsvEntry entry = workingEntry->Entry();
                aDuplicate = entry.Id();
                }
            }
        }

    CleanupStack::PopAndDestroy( mmsHeaders );
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( workingEntry );

    return error;
    }    
    
// ---------------------------------------------------------
// CMmsReceiveMessage::LocalModeFetchL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::LocalModeFetchL()
    {
    
    HBufC* buffer = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( buffer );
    if ( iNotification->ContentLocation().Length() <= KMaxPath && 
        iMmsSettings->LocalMode() )
        {
        buffer->Des().Copy( iNotification->ContentLocation() );
        }
    TPtr temp = buffer->Des();
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("...Fetching from : %S"), &temp );
    #endif

    HBufC* filename = HBufC::NewL( iMmsSettings->LocalModeIn().Length() );
    CleanupStack::PushL( filename );
    TPtr fileNamePtr = filename->Des();
    fileNamePtr.Copy( iMmsSettings->LocalModeIn() );
            
    if ( iNotification->ContentLocation().Length() <= KMaxPath &&
        buffer->Des().FindF( fileNamePtr ) == 0 )
        {
        // The file reading code is for local mode (simulated MMSC)
        // Only one can try to get the file at a time
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L("opening file") );
        #endif
        iError = iFile.Open( iFs, temp, EFileShareExclusive );

        if ( iError == KErrNone )
            {
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("file opened") );
            #endif
            iFileOpen = ETrue;
            TInt size = 0;
            iError = iFile.Size( size );
            
            // Here we try to use chunked buffer.
            // Read the message in chunks and keep calling decode
            // Before we can do that, we must create the message entry
            // Because it must be ready to receive data
            // If receiving fails, the entry must be removed
            
            DoCreateEntryL();
            iDecoder->InitializeChunkedMode( *iEntryWrapper, *iMmsHeaders, *iEncodeBuffer );
            TInt chunkSize = KMmsChunkedBufferSize;
            if ( iError == KErrNone )
                {
                TRAP( iError, iEncodeBuffer->ResizeL( chunkSize ) );
                }
            if ( iError != KErrNone )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Receivemsg can't resize buffer, error %d"), iError );
                TMmsLogger::Log( _L("- message size %d"), chunkSize );
#endif
                iFile.Close();
                iFileOpen = EFalse;
                iDecoder->RelaseDataSink();
                CleanupStack::PopAndDestroy( filename );
                CleanupStack::PopAndDestroy( buffer );
                return;
                }
                
            // We now have an open file.
            // We read it in chunks and feed it to decoder piecewise
            TPtr8 pos = iEncodeBuffer->Ptr( 0 );
            
            TInt position = 0;
            TInt readData = 0;
            TInt amount = 0;
            TBool lastChunk = EFalse;
            TInt error = KErrNone;
            
            while ( readData < size && error == KErrNone )
                {
                chunkSize = KMmsChunkedBufferSize;
                const TInt KMmsTwo = 2;
                if ( chunkSize - amount < KMmsChunkedBufferSize / KMmsTwo )
                    {
                    // if almost all data has been left to buffer, increase buffer size
                    chunkSize = amount + KMmsChunkedBufferSize / KMmsTwo;
                    }
                iEncodeBuffer->ResizeL( chunkSize );
                pos.Set( iEncodeBuffer->Ptr( amount ) );
                pos.SetLength( chunkSize - amount );
                error = iFile.Read( pos, chunkSize - amount );
                amount += pos.Length();
                readData += pos.Length();
                if ( readData >= size )
                    {
                    lastChunk = ETrue;
                    }
                if ( error == KErrNone )
                    {
                    // this is how much data we have
                    iEncodeBuffer->ResizeL( amount ); 
                    error = iDecoder->NextDataPart( *iEncodeBuffer, position, lastChunk );
                    }
                if ( position > iEncodeBuffer->Size() )
                    {
                    position = iEncodeBuffer->Size();
                    }
                // Check if we can continue and shift the buffer if needed
                if ( error == KErrNone )
                    {
                    // now position points to the beginning of unhandled data
                    // it must be shifted to the beginning
                    amount -= position; // this much left unhandled
                    if ( amount < 0 )
                        {
                        amount = 0;
                        }
                    iEncodeBuffer->Delete( 0, position );
                    // start from the beginning the next time
                    position = 0;
                    }
                }
            if ( error != KErrNone )
                {
                iError = error;
                }
            // when the loop ends, all data should be handled and all attachments should be created
            // Decode has also saved message headers.
            // The changed state indicates what is complete
            iDecoder->RelaseDataSink();
            iState = EMmsOperationDecodingResponse; 
            }
        else
            {
            // This corresponds to the situation, where the message
            // has expired or for some other reason is no longer available.
            // We lie to RunL that we have already decoded the message
            iFile.Close();
            iFileOpen = EFalse;
            iState = EMmsOperationDecodingResponse;
            if ( iError != KErrInUse )
                {
                iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
                }
            }
        // We don't close the file yet.
        // We want to keep it exclusively to ourselves until we are done with it
        }
    else
        {
        // No directory found in notification
        // this notification cannot be handled.
        // It will be marked bad and deleted.
        iError = KErrNotFound;
        iState = EMmsOperationDecodingResponse;
        iBad->AppendL( iSelection->At( iCurrentMessageNo - 1 ) );
        }
    CleanupStack::PopAndDestroy( filename );
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::DumpIfNeededL
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::DumpIfNeededL()
    {
#ifndef _NO_MMSS_LOGGING_
    TInt error = KErrNone;
    TBool dumpIncoming = EFalse;
    //
    // Reading binary dumping from CenRep
    //
    CRepository* repository = NULL;
    TInt retval = KErrNone;
    TRAP( retval, repository = CRepository::NewL( KUidMmsServerMtm ) ); // ***
    if( retval == KErrNone )
        {
        CleanupStack::PushL( repository );
        TInt temp = 0;
        retval = repository->Get( KMmsEngineBinaryDump, temp );
        if( retval == KErrNone )
            {
            dumpIncoming = (TBool)temp;
            }
        CleanupStack::PopAndDestroy( repository );
        }    

    TMmsLogger::Log( _L("Receivemsg Dumping Entry") );
    // If the result was bad, we dump the binary data into file
    // - unless decode already did it.
    if ( ( ( !dumpIncoming ) && iError != KErrNone ) &&
        iEncodeBuffer &&
        iEncodeBuffer->Size() > 0 )
        {
        TParse parse;
        TFileName fileName;
        fileName.Copy( KMmsDefaultLogDirectory );
        TUint att;
        if ( iFs.Att( fileName, att ) == KErrNone )
            {
            _LIT( KRelated, "Rec.mms");
            parse.Set( fileName, &KRelated, NULL );
            fileName = parse.FullName();
            error = CApaApplication::GenerateFileName( iFs, fileName );
            if ( error == KErrNone )
                {
                RFile file;
                error = file.Create( iFs, fileName, EFileWrite | EFileShareExclusive );
                // for message id generation
                parse.Set( fileName, NULL, NULL );

                if ( error == KErrNone )
                    {
                    // the data is supposed to be in the encode buffer
                    TPtr8 ptr = iEncodeBuffer->Ptr( 0 );
                    file.Write( ptr );
                    file.Flush();
                    }

                // done - close files
                file.Close();
                }
            }
        }
#endif
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::CloseLocalFile
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::CloseLocalFileL( TBool aDeleteFile )
    {
    HBufC* buffer = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( buffer );
    buffer->Des().Copy( iNotification->ContentLocation() );
    HBufC* filename = HBufC::NewL( iMmsSettings->LocalModeIn().Length() );
    CleanupStack::PushL( filename );
    TPtr fileNamePtr = filename->Des();
    fileNamePtr.Copy( iMmsSettings->LocalModeIn() );
    if ( buffer->Des().FindF( fileNamePtr ) == 0 )
        {
        // this was our local message file
        iFile.Close();
        iFileOpen = EFalse;
        if ( aDeleteFile )
            {
            iFs.Delete( *buffer );
            }
        }
    CleanupStack::PopAndDestroy( filename );
    CleanupStack::PopAndDestroy( buffer );
    }
    
// ---------------------------------------------------------
// CMmsReceiveMessage::MoveToApplicationFolderIfNeededL
//
// ---------------------------------------------------------
//
TInt CMmsReceiveMessage::MoveToApplicationFolderIfNeededL( TMsvEntry& aEntry )
    {
    TInt error = KErrNone;
    TInt returnCode = 0;
    
    if ( iMmsHeaders->ApplicId().Length() > 0 )
        {
        // check if application id is registered
        if ( iRegistered )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- message to registered application") );
#endif
            TMsvId targetFolder = KMsvNullIndexEntryId;
            // target folder's parent is application folder
            TMsvId applicationFolder = iMmsSettings->ApplicationFolder();
            // code scanner gives false positive from the next line
            error = CreateFolderEntryL( applicationFolder, iMmsHeaders->ApplicId(), targetFolder );
            if ( targetFolder != KMsvNullIndexEntryId && error == KErrNone )
                {
                error = iServerEntry->SetEntry( aEntry.Parent() );
                if ( error == KErrNone )
	                {
	                error = iServerEntry->MoveEntryWithinService( aEntry.Id(), targetFolder );     	
	                }
	            if ( error == KErrNone )
	                {
	                // Message was successfully moved to application folder
	                // If we want to keep the notification in inbox we return 
                    // KMmsMessageMovedToApplicationFolder.
                    // As long as we serve only Java applications, the notifications
                    // will disappear from inbox if the message was moved to application folder.
                    // Java will start the applet if it is not running, and that should
                    // be sufficient information for the user.
#ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L("- moved to application folder") );
#endif
	                returnCode = 0;
	                }
	            // If we cannot set our entry back to the one we are handling,
	            // we are in deep trouble    
	            User::LeaveIfError( iServerEntry->SetEntry( iEntryUnderConstruction ));
                }                      
            }
        else
            {
            // There was an application id, but the application was not registered
            // Message will be deleted
            // If deletion fails, the message remains invisible and under construction
            // and will be deleted by message server at next boot.
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- message to unregistered application - deleted") );
#endif
            error = iServerEntry->SetEntry( aEntry.Parent() );
            if ( error == KErrNone )
                {
                iServerEntry->DeleteEntry( aEntry.Id() );
                }
            returnCode = KMmsMessageForUnregisteredApplication;
            }
        }
    // If no application id, the message is normal    
    return returnCode;
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::ClearDuplicateEntryOperationL()
    {
    CMsvStore* store = iServerEntry->ReadStoreL();
    CleanupStack::PushL( store );
    iMmsHeaders->RestoreL( *store );
    CleanupStack::PopAndDestroy( store );
    TMsvId duplicate = KMsvNullIndexEntryId;
    TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
    TMsvEntry dupEntry;

    if ( iNotificationParent == KMsvGlobalInBoxIndexEntryIdValue )
        {
        // if duplicate is found from mmbox folder, 
        // mark it as "deleted from mmbox server"
        if ( mmboxFolder != KMsvNullIndexEntryId )
            {
            FindDuplicateNotificationL( mmboxFolder, *iMmsHeaders, duplicate );
            }
        if ( duplicate != KMsvNullIndexEntryId )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- duplicate found ") );
#endif
            if ( iServerEntry->SetEntry( duplicate ) == KErrNone )
                {
                // Mark duplicate as deleted from mmbox server.
                // If it was successfully fetched it is no longer in MMBox.
                // This is not exactly correct for full MMBox support but as long as
                // we don't have full MMBox support this works correctly
                dupEntry = iServerEntry->Entry();
                MarkNotificationDeletedFromMmbox( dupEntry );
                iServerEntry->ChangeEntry( dupEntry );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("- duplicate marked as deleted from mmbox ") );
#endif
                }                                         
            }    
        }
    else if ( iNotificationParent == mmboxFolder )
        {
        duplicate = iMmsHeaders->RelatedEntry();                                
        if ( duplicate != KMsvNullIndexEntryId )
            {
#ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L("- duplicate found ") );
#endif
            if ( iServerEntry->SetEntry( duplicate ) == KErrNone )
                {
                // Mark it deleted just in case the actual deletion fails.
                // This is not exactly correct for full MMBox support but as long as
                // we don't have full MMBox support this works correctly
                dupEntry = iServerEntry->Entry();
                MarkNotificationDeletedFromMmbox( dupEntry );
                iServerEntry->ChangeEntry( dupEntry );
                // delete the duplicate notification from inbox 
                if ( iServerEntry->SetEntry( KMsvGlobalInBoxIndexEntryIdValue )
                    == KErrNone )
                    {
                    TInt err = iServerEntry->DeleteEntry( duplicate );
#ifndef _NO_MMSS_LOGGING_
                    if ( err == KErrNone )
                        {
                        TMmsLogger::Log( _L("- duplicate deleted from inbox ") );
                        }
#endif
                    }
                }
            }
        }
    else
        {
        // nothing to do - just keep compiler and code analyzer tools happy
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::ClearOperationL( TInt aApplicationMessageStatus )
    {
    if ( iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) ) == KErrNone )
        {
        if ( aApplicationMessageStatus != 0 )
            {
            // The entry is not deleted if it is in inbox or MMBox
            // and has been routed to application.
            // We save the error code or routing status to the entry.
            TMsvEntry tEntry = iServerEntry->Entry();
            MarkNotificationDeletedFromMmbox( tEntry );
            tEntry.SetConnected( EFalse );
            tEntry.SetReadOnly( ETrue );

            if ( aApplicationMessageStatus == KMmsMessageMovedToApplicationFolder )
                {
                // This is a successful case
                tEntry.iMtmData2 |= KMmsMessageRoutedToApplication;
                }
            if ( aApplicationMessageStatus ==
                KMmsMessageForUnregisteredApplication )
                {
                // This will be marked as failed operation
                // even if the message was actually successfully fetched
                // but it was discarded because it does not belong to anybody
                tEntry.iError = KMmsErrorUnregisteredApplication;
                // The earlier operation cleared the possible MMBox bit
                // This operation marks the result as failed.
                MarkNotificationOperationFailed( tEntry );
                }
                
            // error status not checked here - we just do our best    
            iServerEntry->ChangeEntry( tEntry );    
            }
        ClearDuplicateEntryOperationL();
        }
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::CopyDataFromNotificationToMessageL()
    {
    // Only subject and sender take up space when headers are added
    TInt size = iNotification->Sender().Length() +
                iNotification->Subject().Length();
    if ( size > 0 &&
        TMmsGenUtils::DiskSpaceBelowCriticalLevelL( &iFs, size, iMessageDrive ) )
        {
        size = 0;
        }
     
    // We copy data from notification to message if the values are missing
    // But if the message is already complete (status == OK)
    // we do not copy anything.
    if ( iMmsHeaders->ResponseStatus() > KMmsResponseStatusOK )
        {
        // The critical disk space is checked only when adding the subject.
        // The sender number is important and only takes a small amount of space
        // The other fields take the same amount of space regardless of the value.
        // As the headers have been saved already, only subject may consume
        // significantly more disk space.
        if ( size > 0 )
            {
            if ( iMmsHeaders->Subject().Length() == 0 )
                {
                iMmsHeaders->SetSubjectL( iNotification->Subject() );
                }
            }
        if ( iMmsHeaders->Sender().Length() == 0 )
            {
            iMmsHeaders->SetSenderL( iNotification->Sender() );
            }
        if ( iMmsHeaders->MessageClass() == 0 )
            {
            iMmsHeaders->SetMessageClass( iNotification->MessageClass() );
            }
        if ( iMmsHeaders->MessagePriority() == 0 )
            {
            iMmsHeaders->SetMessagePriority( iNotification->MessagePriority() );
            }
        // Copy the expiry, too
        if ( iMmsHeaders->ExpiryDate() == 0 && iMmsHeaders->ExpiryInterval() == 0 )
            {
            // No expiration data in the message - use what is in the notification
            iMmsHeaders->SetExpiryDate( iNotification->ExpiryDate() );
            }
        }
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::CopyDataFromMessageToNotificationL()
    {
    TBool saveNotification = EFalse;
    TInt error = KErrNone;
    
    iNotification->SetResponseTextL( iMmsHeaders->ResponseText() );
    iNotification->SetResponseStatus( iMmsHeaders->ResponseStatus() );

    // Notifications do not originally contain any response texts.
    // If the length is not 0, we have added something
    if ( iNotification->ResponseText().Length() > 0 || iNotification->ResponseStatus() != 0 )
        {
        saveNotification = ETrue;
        }
        
    if ( iNotification->ApplicId().Length() == 0 )
        {
        iNotification->SetApplicIdL( iMmsHeaders->ApplicId() );
        if ( iNotification->ApplicId().Length() > 0 )
            {
            // If we copied the application id, we must save it.
            saveNotification = ETrue;
            }
        }
       
    // update the notification
    if ( saveNotification )
        {
        //  We update the notification if we added something
        error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
        if ( error == KErrNone )
            {
            TRAP( error,
                {
                CMsvStore* store = iServerEntry->EditStoreL();
                CleanupStack::PushL( store );
                iNotification->StoreL( *store );
                store->CommitL();
                CleanupStack::PopAndDestroy( store );
                })
            }
        // If we cannot update the entry, the user does not see the error text, but that's not fatal
        }
    // Release the entry
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    
    }
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsReceiveMessage::MapErrorStatus( const TInt32 aErrorStatus )
    {
    TBool fatal = EFalse;
    
    switch ( aErrorStatus )
        {
        case 0: // if the header is not present, it is 0
        case KMmsStatusOk:
            // No error, don't change iError
            break;
        case KMmsErrorPermanentFailure:
            fatal = ETrue;
            iError = KMmsErrorStatusPermanentFailure;
            break;
        case KMmsErrorTransientFailure:
            iError = KMmsErrorStatusTransientFailure;
            break;
        case KMmsErrorReceivePermanentMessageNotFound:
            fatal = ETrue;
            iError = KMmsErrorStatusMessageNotFound;
            break;
        case KMmsErrorReceiveTransientMessageNotFound:
            iError = KMmsErrorStatusTransientMessageNotFound;
            break;
        case KMmsErrorReceiveTransientNetworkProblem:
            iError = KMmsErrorStatusNetworkProblem;
            break;
        case KMmsErrorPermanentServiceDenied:
            fatal = ETrue;
            iError = KMmsErrorStatusServiceDenied;
            break;
        case KMmsErrorReceivePermanentContentUnsupported:
            fatal = ETrue;
            iError = KMmsErrorStatusContentUnsupported;
            break;
        default:
            // Unknown error, if not transient, it is fatal
            if ( ( aErrorStatus & KMmsErrorRangeMask ) == KMmsErrorTransient )
                {
                iError = KMmsErrorStatusTransientFailure;
                }
            else
                {
                iError = KMmsErrorPermanentFailure;
                fatal = ETrue;
                }
            break;
        }
            
    return fatal;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::SetIndexEntryBitsForReceivedMessage( TMsvEntry& aEntry )
    {
    aEntry.iMtmData1 &= ~KMmsMessageTypeMask;
    // We override message type for now.
    // We get send requests instead of retrieve confirmations if in local mode
    aEntry.iMtmData1 |= KMmsMessageMRetrieveConf | KMmsMessageMobileTerminated;

    aEntry.SetVisible( ETrue );
    aEntry.SetComplete( ETrue );
    aEntry.SetInPreparation( EFalse );
    aEntry.SetReadOnly( ETrue );
    aEntry.iDate.UniversalTime(); // this is arrival time
    
    if ( iMmsSettings->ShowSentTime() )
        {
        // show the time the message was received by MMSC instead of 
        // the time the message was received by the terminal
        if ( iMmsHeaders->Date() > 0 )
            {
            // date is given as seconds from 1.1.1970, UTC time
            TTime time = TTime( KMmsYear1970String ) +
                TTimeIntervalMicroSeconds( iMmsHeaders->Date() * KMmsMillion );
            aEntry.iDate = time.Int64();
            }
        }
    // subject, sender and size are set in CMmsDecode
    // - except if we get an empty error message.
    // In that case the fields were copied from the notification,
    // and we set sender and subject here.
    if ( aEntry.iDetails.Length() == 0 )
        {
        aEntry.iDetails.Set( iMmsHeaders->Sender() );
        }
    if ( aEntry.iDescription.Length() == 0 )  
        {
        aEntry.iDescription.Set( iMmsHeaders->Subject() );
        }

    // Set multiple recipients
    if ( iMmsHeaders->ToRecipients().MdcaCount() +
        iMmsHeaders->CcRecipients().MdcaCount() +
        iMmsHeaders->BccRecipients().MdcaCount() > 1 )
        {
        aEntry.SetMultipleRecipients( ETrue );
        }
    }
    
    
// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsReceiveMessage::DeleteApplicationMessagesL()
    {
    // target folder's parent is application folder
    TMsvId messageFolder = KMsvNullIndexEntryId;
    TMsvId applicationFolder = iMmsSettings->ApplicationFolder();
    // code scanner gives false positive from the next line
    TInt error = KErrNone;
    error = FolderEntryL( applicationFolder, iMmsHeaders->ApplicId(), messageFolder );
   
    if ( messageFolder == KMsvNullIndexEntryId || error != KErrNone )
        {
        // We could not find the folder - we cannot free space
        return;
        }
        
    // List all messages in the folder and try to see if we can free enough room.
    TInt size = iNotification->MessageSize();
    if ( size > iMmsSettings->MaximumReceiveSize() )
        {
        // We cannot fetch more than this anyway
        size = iMmsSettings->MaximumReceiveSize();
        }
    // The safety margin must be added to the indicated size as the size in the
    // notification only specifies the payload, not all headers and space taken
    // by message entry and folder entry needed.
    size += KMmsDiskSafetyMargin;
    
    error = iServerEntry->SetEntry( messageFolder );
    if ( error != KErrNone )
        {
        // no can do
        return;
        }
    
    // sort entries by date
    TMsvSelectionOrdering ordering =
        TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByDate, EFalse );
    iServerEntry->SetSort( ordering );
    
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    error = iServerEntry->GetChildrenWithType( KUidMsvMessageEntry, *selection );
    if ( error != KErrNone )
        {
        // no can do
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        CleanupStack::PopAndDestroy( selection );
        }
   
    TInt i;
    TInt deleteCount = 0; // entries to be deleted to free space
    TInt oldSize = 0;
    for ( i = 0; i < selection->Count() && deleteCount == 0; i++ )
        {
        error = iServerEntry->SetEntry( selection->At( i ) );
        if ( error == KErrNone )
            {
            // If we could not access some entry, it will end up in the 
            // delete list anyway. It may or may not be deleted.
            oldSize += iServerEntry->Entry().iSize;
            }
        if ( oldSize >= size )
            {
            deleteCount = i + 1;
            }
        }
        
    // Check if we fond enough old messages to free space.
    if ( deleteCount > 0 )
        {
        for ( i = selection->Count(); i > deleteCount; ++i )
            {
            // We may be able to leave some entries
            selection->Delete( i - 1 );
            }
        
        error = iServerEntry->SetEntry( messageFolder );
        if ( error == KErrNone )
            {
            error = iServerEntry->DeleteEntries( *selection );
            }
        // If we get an error because some entry is locked or open
        // the original KErrNoDisk remains and no entries are deleted
        // The situation will follow normal low disk space handling
        // If the entries are being accessed by the application, the
        // space may be freed soon by the application.
        if ( error == KErrNone )
            {
            // We have successfully deleted enough entries
            // The new message should now fit.
            // It can be rescheduled or fetch can be manually restarted
            iError = KMmsErrorApplicationDiskFull;
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    CleanupStack::PopAndDestroy( selection );    
    }
    

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

