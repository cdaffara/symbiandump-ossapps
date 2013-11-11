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
* Description:   ECOM plugin .dll for handling MMS related WAP push messages.
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <msvapi.h>
#include <msvids.h>
#include <msvuids.h>
#include <e32math.h>
#include <apparc.h>

#include <push/cpushhandlerbase.h>
#include <ecom/implementationproxy.h>
#include <push/pluginkiller.h>
#include <push/pushmessage.h>
#include <push/pushlog.h>

#include "mmsconst.h"
#include "mmscmds.h"
#include "CMmsPushHandler.h"
#include "mmsgenutils.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
const TUint KMinimumNumberOfMessageFields = 6;
// ECOM framework related
const TImplementationProxy ImplementationTable[] = 
    {
    // different definition for new compiler
    IMPLEMENTATION_PROXY_ENTRY(0x101F4692, CMmsPushHandler::NewL)
    };

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// ============================= LOCAL FUNCTIONS ===============================
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMmsPushHandler::New
// 1st phase constructor
// Returns: <new CMmsPushHandler>:
// -----------------------------------------------------------------------------
//
CMmsPushHandler* CMmsPushHandler::NewL()
    {
    CMmsPushHandler* self = new( ELeave ) CMmsPushHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    LOGTEXT(_L("***** CMmsPushHandler NewL() *****"));
    return self;
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CMmsPushHandler
// Initialises the object with zero/NULL values.
// -----------------------------------------------------------------------------
//
CMmsPushHandler::CMmsPushHandler()
    :CPushHandlerBase(),
    iState( EInitial ),
    iServiceId( KMsvNullIndexEntryId ),
    iMsvSession( NULL ),
    iMsvOperation( NULL ),
    iPushMsg( NULL ),
    iBody( NULL ),
    iRetryCount( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::ConstructL
//
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::ConstructL()
    {
    // Connect to filesystem
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iTimer.CreateLocal() );

    // Add plugin to AS
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::~CMmsPushHandler
// Destructor. Calls also baseclass destructor which calls
//  REcomSession::DestroyedImplementation(iDtor_ID_Key)
// -----------------------------------------------------------------------------
//
CMmsPushHandler::~CMmsPushHandler()
    {
    LOGTEXT(_L("~CMmsPushHandler()"));

    // AO's destructor always calls Cancel()
    Cancel();

    // Delete the necessary instance attributes
    delete iMsvSession;
    delete iMsvOperation;
    delete iPushMsg;
    delete iBody;
    iFs.Close();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::HandleMessageL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::HandleMessageL(
    CPushMessage* aPushMsg,
    TRequestStatus& aStatus )
    {
    LOGTEXT(_L("Asynchronous HandleMessageL() called but not implemented."));

    iPushMsg = aPushMsg;
    SetConfirmationStatus( aStatus );
    SignalConfirmationStatus( KErrNotSupported );
    iPluginKiller->KillPushPlugin();
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::HandleMessageL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::HandleMessageL( CPushMessage* aPushMsg )
    {
    LOGTEXT( _L("Synchronous HandleMessageL() called.") );

    iPushMsg = aPushMsg;

    //
    // Do sanity checks for the message.
    //
    TInt error = PerformChecks();
    if ( error )
        {
        LOGTEXT2( _L("HandleMessageL(): PerformChecks failed with code %d, returning silently. Notification is lost."), error );
        
        iPluginKiller->KillPushPlugin();        
        return;
        }

    //
    // Store the messagebody into handler's attribute
    // If memory runs out, tough luck..
    //
    TPtrC8 messageBodyPtr;
    if ( iPushMsg->GetMessageBody( messageBodyPtr ) )
        {
        iBody = messageBodyPtr.Alloc();
        if ( !iBody )
            {
        	LOGTEXT( _L("HandleMessageL(): Out of memory when allocating body buffer") );
        	// Commit suicide - the caller expects it even if we leave
        	iPluginKiller->KillPushPlugin();
        	User::Leave( KErrNoMemory );
            }
        }
    
    //
    // Setting 'this' active and complete in order to get to RunL
    //
    TRequestStatus* status = &iStatus;
    iStatus = KRequestPending;
    User::RequestComplete( status, KErrNone ); //iStatus = KErrNone
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CancelHandleMessage
// Cancels the pending asynchronous HandleMessageL.
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::CancelHandleMessage()
    {
    LOGTEXT( _L("CancelHandleMessage() called, but not supported.") );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::HandleSessionEventL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::HandleSessionEventL(
    TMsvSessionEvent aEvent, 
    TAny* /*aArg1*/, 
    TAny* /*aArg2*/, 
    TAny* /*aArg3*/)
    {
    switch (aEvent)
        {
        // If session is closed down, the object will be deleted
        case EMsvCloseSession:
            LOGTEXT(_L("HandleSessionEventL(): Session event EMsvCloseSession"));
            // fall through on purpose
        case EMsvServerTerminated:
            LOGTEXT(_L("HandleSessionEventL(): Session event EMsvServerTerminated"));
            if ( iMsvSession )
                {
                delete iMsvSession;
                iMsvSession = NULL;
                }
            break;
        case EMsvMediaUnavailable:
            LOGTEXT(_L("HandleSessionEventL(): EMsvMediaUnavailable"));
            break;
        default:
            //Nothing
            break;
        }
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::DoCancel()
    {
    LOGTEXT( _L("DoCancel()") );
    if( iState == ETransferCommand && iMsvOperation )
        {
        iMsvOperation->Cancel();
        }
    else if( iState == EMsDriveChange && iMsvOperation )
        {
        iMsvOperation->Cancel();
        }
    else if( iState == ETimer )
        {
        iTimer.Cancel();
        }
    else if( iState == EDiskSpaceWait )
        {
        iFs.NotifyDiskSpaceCancel();
        }

    // Finally clean up
    iPluginKiller->KillPushPlugin();
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::RunL
// Loops forever if MessageServer/ServerMtm respond correctly
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::RunL()
    {
    LOGTEXT(_L("RunL(): Starting"));

    // Check for cancel
    if( iStatus == KErrCancel )
        {
        LOGTEXT( _L("RunL(): Operation cancelled.") );
        iPluginKiller->KillPushPlugin();
        return;
        }

    // Checking the retry count, because if something just keeps going wrong
    // there is nothing that can be done
    if( ++iRetryCount > KRetryCount )
        {
        LOGTEXT( _L("RunL(): Retrycount exceeded, quitting. Notification is lost.") );
        iPluginKiller->KillPushPlugin();
        return;
        }

    TInt error = KErrNone;

    //
    // Operation: TransferCommand
    //
    if( iState == ETransferCommand )
        {
        // Clean up
        if( iMsvOperation )
            {
            delete iMsvOperation;
            iMsvOperation = NULL;
            }

        // Go through statuses
        if( iStatus == KErrNone )
            {
            LOGTEXT( _L("RunL(): TransferCommandL() succeeded.") );
            iPluginKiller->KillPushPlugin();
            return;
            }
        else if( iStatus == KErrLocked ) // for a short wait
            {
            LOGTEXT2( _L("RunL(): TransferCommandL failed with code %d, entering short wait."), iStatus.Int() );
            iTimer.After( iStatus, KShortWait );
            iState = ETimer;
            SetActive();
            return;
            }
        else // default is long wait
            {
            LOGTEXT2( _L("RunL(): TransferCommandL failed with code %d, entering long wait."), iStatus.Int() );
            iTimer.After( iStatus, KLongWait );
            iState = ETimer;
            SetActive();
            return;
            }
        }

    //
    // Open session if not open
    // Session is needed to check drive
    //
    if ( !iMsvSession  )
        {
        TRAP( error, OpenSessionL() );
        if( error )
            {
            LOGTEXT2( _L("RunL: OpenSessionL left with code %d, entering long wait."), error );
            iState = ETimer;
            iTimer.After( iStatus, KLongWait );
            SetActive();
            return;
            }
        }
    //
    // Operation: MessageStore Drive Change
    //
    if( iState == EMsDriveChange )
        {
        // Clean up
        if( iMsvOperation )
            {
            delete iMsvOperation;
            iMsvOperation = NULL;
            }

        // Drive should be now on 'C:'
        // Asserting this:
        if( iMsvSession->CurrentDriveL() != EDriveC )
            {
            // if not, giving up without retrying
            LOGTEXT( _L("RunL(): Drive change attempt did not work, returning silently. Notification is lost." ) );
            iPluginKiller->KillPushPlugin();
            return;                        
            }
        LOGTEXT( _L("RunL(): Drive change attempt succeeded." ) );        
        }

    //
    // Find MMS default serviceId if not yet found
    //
    if( iServiceId == 0 )
        {
        TRAP( error, FindServiceL() );
        // If MessageStore is not available and push message is a notification,
        // The message store must be moved to C:
        // (delivery reports are handled on best effort basis)
        if( ( error == KErrCorrupt ||         
              error == KErrNotReady )          
            && (*iBody)[1] == 0x82 )
            { 
            // Try changing MessageStore to C:
            LOGTEXT2( _L("RunL: FindServiceL left with code %d, changing drive."), error );
            TRAP( error, { iMsvOperation = iMsvSession->ChangeDriveL( EDriveC, iStatus ); } );
            if( error == KErrNone )
                {
                iState = EMsDriveChange;
                SetActive();
                return;
                }
            else
                {
                LOGTEXT2( _L("RunL: ChangeDriveL left with code %d."), error );
                // This falls to the next if statement (below) on purpose
                }
            }
        if( error != KErrNone ) // error is something else
            {
            LOGTEXT2( _L("RunL(): FindServiceL left with code %d, returning silently. Notification is lost."), error );
            iPluginKiller->KillPushPlugin();
            return;            
            }
        if( iServiceId == 0 ) // if iService still is 0 (FindServiceL should have found this)
            {
            LOGTEXT( _L("RunL(): FindServiceL didn't find MMS serviceId, returning silently. Notification is lost." ) );        
            iPluginKiller->KillPushPlugin();
            return;            
            }
        }

    //
    // Checking disk space before using it
    //
    TRAP( error, { error = CheckDiskSpaceL(); } );
    if( error == KErrDiskFull )
        {
        // CheckDiskSpaceL has handled the situation, i.e. we should active already
        return;
        }
    else if( error != KErrNone ) // other error
        {
        LOGTEXT2( _L("RunL(): CheckDiskSpaceL left with code %d, entering short wait."), error );
        iTimer.After( iStatus, KShortWait );
        iState = ETimer;
        SetActive();
        return;
        }

    //
    // Transfer the push message to MessageServer side
    //
    TRAP( error, TransferMessageL() );
    if( error == KErrCorrupt     // MMC is not present
        && (*iBody)[1] == 0x82 ) // push message is a notification
        {
        LOGTEXT( _L("RunL: TransferMessageL left with code %d, changing drive.") );
        // Try changing MessageStore to C:
        TRAP( error, { iMsvOperation = iMsvSession->ChangeDriveL( EDriveC, iStatus ); } );
        if( error == KErrNone )
            {
            iState = EMsDriveChange;
            SetActive();
            return;
            }
        else
            {
            LOGTEXT2( _L("RunL: ChangeDriveL left with code %d."), error );
            // This falls to the next if statement (below) on purpose
            }
        }
    if( error != KErrNone ) // other error
        {
        LOGTEXT2( _L("RunL(): TransferMessageL left with code %d, entering short wait."), error );
        iTimer.After( iStatus, KShortWait );
        iState = ETimer;
        SetActive();
        return;
        }
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CPushHandlerBase_Reserved1
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::CPushHandlerBase_Reserved1()
    {
    User::Panic(
        _L("CPushHandlerBase_Reserved1() not supported."), 
        KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CPushHandlerBase_Reserved2
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::CPushHandlerBase_Reserved2()
    {
    User::Panic(
        _L("CPushHandlerBase_Reserved2() not supported."),
        KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::PerformChecksL
// 
// -----------------------------------------------------------------------------
//
TInt CMmsPushHandler::PerformChecks()
    {
    // Check that message is not equal to NULL
    if( !iPushMsg )
        {
        LOGTEXT( _L("ERROR: Message == NULL.") );
        return KErrCorrupt;
        }

    // Check that body is not equial to NULL
    TPtrC8 messageBodyPtr;
    TBool bodyPresent = EFalse;
    bodyPresent = iPushMsg->GetMessageBody( messageBodyPtr );
    if( !bodyPresent )
        {
        LOGTEXT( _L("ERROR: Message has NULL body.") );
        return KErrCorrupt;
        }

    // KMinimumNumberOfMessageFields (6) is minimum number of message fields. 
    // This is a very "mild" check but
    // at least it guarantees that there is no empty body in the message
    if( messageBodyPtr.Size() < KMinimumNumberOfMessageFields )
        {
        LOGTEXT( _L("ERROR: Message has too short body.") );
        return KErrCorrupt;
        }

    // Check message-type
    TPtrC contentTypePtr;
    iPushMsg->GetContentType( contentTypePtr );
    _LIT( KMmsContentType, "application/vnd.wap.mms-message" );
    if( contentTypePtr.Compare( KMmsContentType ) != 0 )
        {
        LOGTEXT2( _L("ERROR: ContentType == %S"), &contentTypePtr );
        return KErrCorrupt;
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::OpenSessionL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::OpenSessionL()
    {
    LOGTEXT( _L("OpenSessionL()") );
    iMsvSession = CMsvSession::OpenSyncL( *this );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::FindServiceL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::FindServiceL()
    {
    LOGTEXT( _L("FindServiceL()") );
    
    // Use first service found. There should never be more than one MMS Service
    
    // Create a new entry, showing invisible entries (because the service entry may be invisible)
    TMsvSelectionOrdering ordering(KMsvNoGrouping, EMsvSortByNone, ETrue);
    CMsvEntry* entry = CMsvEntry::NewL(*iMsvSession, KMsvRootIndexEntryId, ordering);
    CleanupStack::PushL(entry);
	
 	CMsvEntrySelection *sel=entry->ChildrenWithMtmL(KUidMsgTypeMultimedia);
	CleanupStack::PushL(sel);
	if(sel->Count() == 0)
	    {
	    User::Leave(KErrNotFound);
	    } 
	iServiceId=sel->At(0);
 	CleanupStack::PopAndDestroy( sel );
 	CleanupStack::PopAndDestroy( entry );
 	return;
    
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::TransferMessageL
// 
// -----------------------------------------------------------------------------
//
void CMmsPushHandler::TransferMessageL()
    {
    LOGTEXT( _L("TransferMessageL()") );

    // In order to get to mms service fast (not wait for our turn)
    // we create an entry into our invisible mms directory.
    // This entry will contain the binary notification
    // and the service says "local service" with "MMS mtm"

    //
    // Get entry id of the notifications folder
    //
    TMsvId mmsFolder = GetMMSFolderL();

    // Create selection array
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );

    TMsvId tMsvId = CreateEntryL( mmsFolder );
    iMsvSession->CleanupEntryPushL( tMsvId );
    LOGTEXT2( _L("TransferMessageL(): Entry Created: %d"), tMsvId );

    //
    // The binary notification has been streamed into this entry.
    // Now we have an entry that says: local service, MMS MTM
    //
    if ( tMsvId != KMsvNullIndexEntryId )
        {
        LOGTEXT( _L("TransferMessageL(): iPushMessageId != KMsvNullIndexEntryId") );
        selection->AppendL( tMsvId );
        }
    else
        {
        LOGTEXT( _L("TransferMessageL(): iPushMessageId == KMsvNullIndexEntryId") );
        selection->AppendL( iServiceId );
        }
    LOGTEXT( _L("TransferMessageL(): iPushMessageId Appended to Selection.") );

    // Watcher parameters have become obsolete as all data is transferred in the entry
    TWatcherParameters parameters;
    TWatcherParametersBuf paramPack( parameters );

    //
    // Make asynch call to transfer the push message
    //
    LOGTEXT( _L("TransferMessageL(): invoking asynchronous TransferCommandL()") );
    iMsvOperation = iMsvSession->TransferCommandL(
        *selection,
        EMmsDecodePushedMessage, 
        paramPack, 
        iStatus );
    iState = ETransferCommand;
    SetActive();

    iMsvSession->CleanupEntryPop(); // tMsvId
    CleanupStack::PopAndDestroy( selection );
    LOGTEXT( _L("TransferMessageL() exiting") );
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CheckDiskSpaceL
//
// -----------------------------------------------------------------------------
//
TInt CMmsPushHandler::CheckDiskSpaceL()
    {
    LOGTEXT( _L("CheckDiskSpaceL()") );
    TInt messageDrive = EDriveC; // C is default
    messageDrive = iMsvSession->CurrentDriveL();
    LOGTEXT2( _L("CheckDiskSpaceL(): Current Drive = %d"), messageDrive );
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( 
        &iFs, 
        KRequiredDiskSpace, 
        messageDrive ) )
        {
        LOGTEXT( _L("CheckDiskSpaceL(): Not Enough Diskspace, subscribing a notification from FileServer.") );
        TVolumeInfo volumeInfo;
        User::LeaveIfError( iFs.Volume( volumeInfo, messageDrive ) );
        iFs.NotifyDiskSpace( 
            ( volumeInfo.iFree + KRequiredDiskSpace ), 
            messageDrive, 
            iStatus );
        iState = EDiskSpaceWait;
        SetActive();
        return KErrDiskFull;
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::GetMMSFolderL
// Creates an MMS folder into MessageServer's message store.
// (As a subfolder of LocalService.)
// Returns: <new TMsvId>: Entry Id of the created MMS folder.
// -----------------------------------------------------------------------------
//
TMsvId CMmsPushHandler::GetMMSFolderL()
    {
    LOGTEXT( _L("GetMMSFolderL()") );

    // First check if proper MMS folder already exists
    TMsvId mmsFolderId = FindMMSFolderL();

    // If below is true, the folder did not exist already and has to be created.
    // This should not happen unless the message store is corrupted
    if ( mmsFolderId == KMsvNullIndexEntryId )
        {
        mmsFolderId = CreateMMSFolderL();
        }

    return mmsFolderId;
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CreateMMSFolderL
// Creates MMSNotifications folder into Message Store.
// (As a subfolder of LocalService.)
// Returns: <new TMsvId>: Entry Id of the created MMS folder.
// -----------------------------------------------------------------------------
//
TMsvId CMmsPushHandler::CreateMMSFolderL()
    {
    LOGTEXT( _L("CreateMMSFolderL()") );
    //
    // Create entry
    //
    TMsvEntry entry;
    entry.iType = KUidMsvFolderEntry;
    entry.iMtm = KUidMsvLocalServiceMtm;
    entry.iDetails.Set( KMMSNotificationFolder );
    entry.SetVisible( EFalse );
    entry.SetInPreparation( EFalse );
    entry.iServiceId = KMsvLocalServiceIndexEntryId;

    //
    // Make the created entry to be a subfolder of LocalService
    //
    CMsvEntry* cMsvEntry 
        = iMsvSession->GetEntryL( KMsvLocalServiceIndexEntryId );
    CleanupStack::PushL( cMsvEntry );
    cMsvEntry->CreateL( entry );
    CleanupStack::PopAndDestroy( cMsvEntry );

    return entry.Id();
    }


// -----------------------------------------------------------------------------
// CMmsPushHandler::FindMMSFolderL
// Checks if MMS folder entry already exists.
// Returns: KMMSNotificationFolder: Entry Id of the found MMS folder
//          KMsvNullIndexEntryId:   MMS folder was not found
// -----------------------------------------------------------------------------
//
TMsvId CMmsPushHandler::FindMMSFolderL()
    {
    LOGTEXT( _L("FindMMSFolderL()") );

    TMsvId mmsFolderId = KMsvNullIndexEntryId;

    // Get Local Service as parent entry
    CMsvEntry* cMsvEntry = iMsvSession->GetEntryL( KMsvLocalServiceIndexEntryId );
    CleanupStack::PushL( cMsvEntry );
    // show invisible entries
    cMsvEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, 
                                                    EMsvSortByIdReverse, 
                                                    ETrue ) );
    // Get all folder entries.                                                    
    CMsvEntrySelection* selection 
        = cMsvEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PushL( selection );

    // Now we should have a list of all local folders.
    // Prune away the standard folders.
    // They should be at the end of the list
    TInt count = selection->Count();
    TInt i;
    for ( i = count - 1; i >= 0; --i )
        {
        if ( selection->At( i ) <= KMsvDeletedEntryFolderEntryId )
            {
            // Anything below this should not be ours
            selection->Delete( i );
            }
        }

    // Check if anything left.
    count = selection->Count();
    // Loop through the rest and find possible correct folder
    for ( i = 0; i < count && mmsFolderId == KMsvNullIndexEntryId; ++i )
        {
        cMsvEntry->SetEntryL( selection->At( i ) );
        // must be exact match
        if (cMsvEntry->Entry().iDetails.Compare( KMMSNotificationFolder ) == 0)
            {
            mmsFolderId = selection->At( i );
            }
        }
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( cMsvEntry );

    // If folder does not exist -> returns KMsvNullIndexEntryId
    // If folder exists -> returns its Id
    return mmsFolderId;
    }

// -----------------------------------------------------------------------------
// CMmsPushHandler::CreateEntryL
// 
// -----------------------------------------------------------------------------
//
TMsvId CMmsPushHandler::CreateEntryL( TMsvId aFolder )
    {
    LOGTEXT( _L("CreateEntryL()") );
    //
    // Basic "NULL test"
    //
    if( aFolder == KMsvNullIndexEntryId )
        {
        // No folder, no entry
        return KMsvNullIndexEntryId;
        }

    //
    // Get CMsvEntry context of the target folder
    //
    CMsvEntry* cMsvEntry = iMsvSession->GetEntryL( aFolder );
    CleanupStack::PushL( cMsvEntry ); // ***

    //
    // Create an entry representing the notification
    //
    TMsvEntry tMsvEntry;
    tMsvEntry.iType = KUidMsvMessageEntry;
    tMsvEntry.iMtm = KUidMsgTypeMultimedia;
    tMsvEntry.iServiceId = KMsvLocalServiceIndexEntryId;
    tMsvEntry.iRelatedId = iServiceId;
    tMsvEntry.iMtmData2 = KMmsNotificationBinary;
    // Visible and inPreparation flags will be changed after data has been
    // successfully streamed
    tMsvEntry.SetVisible( EFalse );
    tMsvEntry.SetInPreparation( ETrue );
    cMsvEntry->CreateL( tMsvEntry );

    //
    // Stream 
    // 1) length of the data as 32 bit integer
    // 2) pushed message data
    // into created entry's stream  
    //
    cMsvEntry->SetEntryL( tMsvEntry.Id() );
    CMsvStore* store = cMsvEntry->EditStoreL();
    CleanupStack::PushL( store );   // ***
    RMsvWriteStream outs;
    outs.AssignLC( *store, KUidBinaryNotificationStream ); // ***
    outs.WriteUint32L( iBody->Size() );
    LOGTEXT2( _L(" - streamed %d bytes into dummy-entry's store"), iBody->Size() );
    outs.WriteL( *iBody );
    outs.CommitL();
    outs.Close();
    store->CommitL();
    CleanupStack::PopAndDestroy( &outs );  //outs
    CleanupStack::PopAndDestroy( store );
    //
    // Now, change the flags to their final values
    //
    tMsvEntry.SetVisible( ETrue );
    tMsvEntry.SetInPreparation( EFalse );
    cMsvEntry->ChangeL( tMsvEntry );
    CleanupStack::PopAndDestroy( cMsvEntry );  

    return tMsvEntry.Id();
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns: TImplementationProxy*: Implementation table to the ECOM framework 
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

//  End of File
