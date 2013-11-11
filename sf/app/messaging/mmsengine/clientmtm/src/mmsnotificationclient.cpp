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
*     A Client MTM to access MMS notifications in a manual fetch mode.
*     All this is needed to support new mtm type for Symbian OS messaging
*     UI components.
*
*/



// INCLUDE FILES
#include <msvids.h>
#include "mmsheaders.h"
#include "mmsnotificationclient.h"
#include "mmscmds.h"
#include "mmsmessageoperation.h"
#include "mmsmmboxviewheaders.h"
#include "mmssettings.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
const TInt KMmsAttributeArrayGranularity = 8;

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsClientNotificationMtm::NewL
// ---------------------------------------------------------
//
EXPORT_C CMmsNotificationClientMtm* CMmsNotificationClientMtm::NewL(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    {
    CMmsNotificationClientMtm* self = new ( ELeave ) CMmsNotificationClientMtm(
        aRegisteredMtmDll, aSession );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ---------------------------------------------------------
// CMmsClientNotificationMtm::CMmsNotificationClientMtm
// ---------------------------------------------------------
//
CMmsNotificationClientMtm::CMmsNotificationClientMtm(
    CRegisteredMtmDll& aRegisteredMtmDll,
    CMsvSession& aSession )
    : CMmsClientMtm( aRegisteredMtmDll, aSession )
    {
    __ASSERT_DEBUG( Type() == KUidMsgMMSNotification, gPanic( EMmsBadMtmTypeUid ) );
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::~CMmsNotificationClientMtm
// ---------------------------------------------------------
//
CMmsNotificationClientMtm::~CMmsNotificationClientMtm()
    {
    
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::ConstructL
// 
// ---------------------------------------------------------
//
void CMmsNotificationClientMtm::ConstructL()
    {
    // First loading settings
    iMmsSettings = CMmsSettings::NewL();
    iMmsSettings->LoadSettingsL();
    // Get the base values to detect changes
    iHomeMode = iMmsSettings->ReceivingModeHome();
    iRoamingMode = iMmsSettings->ReceivingModeForeign();
    iAccessPointCount = iMmsSettings->AccessPointCount();
    
    iMessageDrive = EDriveC;
    TInt error = KErrNone;

    TRAP ( error, { iMessageDrive = Session().CurrentDriveL(); } );

    if ( error != KErrNone )
        {
        // if cannot ask, use default
        iMessageDrive = EDriveC;
        }

    // Notification client mtm does not create new service
    iServiceId = iMmsSettings->Service();

    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );

    iAttributes  = new( ELeave ) CDesCArrayFlat( KMmsAttributeArrayGranularity );

    // Set the original context to the service entry
    SwitchCurrentEntryL( iServiceId );
    }

// ---------------------------------------------------------
// CMmsClientMtm::QueryCapability
// ---------------------------------------------------------
//
TInt CMmsNotificationClientMtm::QueryCapability(
    TUid /*aCapability*/,
    TInt& /*aResponse*/ )
    {
    // Nothing Supported at the moment
    return KErrNotSupported;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::ForwardL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::ForwardL(
    TMsvId aDestination,
    TMsvPartList /*aPartList*/,
    TRequestStatus& aCompletionStatus )
    {
    //
    // Actual creation is done synchronously below
    //
    TMsvId id = CreateForwardEntryL( aDestination );

    //
    // This method has to be asynchronous..
    //
    TPckgC < TMsvId > progress = id;
    return  CMsvCompletedOperation::NewL( Session(), Type(), progress, 
        KMsvLocalServiceIndexEntryId, aCompletionStatus );
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::CreateForwardEntryL
// ---------------------------------------------------------
//
TMsvId CMmsNotificationClientMtm::CreateForwardEntryL( TMsvId aDestination )
    {
    //
    // Check that current context is valid
    //
    if( iMsvEntry->Entry().iType != KUidMsvMessageEntry
        || iMsvEntry->Entry().iMtm != KUidMsgMMSNotification )
        {
        User::Leave( KErrUnknown );
        }

    // It is assumed that a created forward entry can not be saved to drafts.
    // I.e. the application calling this, is responsible to take care that
    // the entry is either SENT or DESTROYED.

    //
    // Load entry to local cache if necessary
    //
    if( iMmsHeaders->ContentLocation() == TPtrC8() )
        {
        LoadMessageL();
        }

    //
    // Create new Mms message object into aDestination folder
    //
    TMsvId forwardId = CMmsMessageOperation::CreateForwardWithoutRetrievalL(
        Session(),
        *iMmsHeaders,
        iMsvEntry->EntryId(),
        aDestination,
        iServiceId );

    return forwardId;
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::SendL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::SendL( 
    TRequestStatus& aCompletionStatus,
    const TTime aSendingTime )
    {
    //
    // Is it possible to proceed with the operation...
    // If yes, the notification is marked as "further-operations-not-allowed"
    //
    TMsvId relatedNotificationId = iMmsHeaders->RelatedEntry();
    TInt retval = ReserveNotificationOperationL( relatedNotificationId, KMmsOperationForward );
    if( retval != KErrNone )
        {
        TPckgC < TMsvId > progress = relatedNotificationId;
      	return CMsvCompletedOperation::NewL(
      	    Session(),
      	    KUidMsgMMSNotification,
      	    progress, // progress contains the related entry id
      	    iServiceId,
      	    aCompletionStatus,
      	    KErrInUse);
        }

    //
    // Create the selection to be sent
    //
    CMsvEntrySelection* selection = new( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection ); // ***
    selection->AppendL( iMsvEntry->Entry().Id() );

    //
    // Make sure the message is in OUTBOX
    //
    TMsvId currentParent = iMsvEntry->Entry().Parent();
    if ( currentParent != KMsvGlobalOutBoxIndexEntryId )
        {
        CMsvEntry* cEntry = NULL;
        cEntry = Session().GetEntryL( currentParent );
        CleanupStack::PushL( cEntry );
        cEntry->MoveL( iMsvEntry->Entry().Id(), KMsvGlobalOutBoxIndexEntryId );
        CleanupStack::PopAndDestroy( cEntry );
        }

    //
    // Create parameters structure
    //
    TCommandParameters parameters;
    TTime now;
    now.UniversalTime();
    if ( aSendingTime > now )
        {
        aSendingTime.SecondsFrom( now, parameters.iInitialDelay );
        }
    TCommandParametersBuf paramPack( parameters );

    //
    // Call Server MTM
    //
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsScheduledForward,
        *selection,
        paramPack,
        aCompletionStatus );
 
    //
    // Cleanup and return
    //
    CleanupStack::PopAndDestroy( selection );
    return op;
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::SendL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::SendL(
    CMsvEntrySelection& /*aSelection*/,
    TRequestStatus& /*aCompletionStatus*/,
    TTime /*aSendingTime*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL; // makes the compiler happy
    }

// ---------------------------------------------------------
// CMmsClientNotificationMtm::ReserveNotificationOperationL
// ---------------------------------------------------------
//
TInt CMmsNotificationClientMtm::ReserveNotificationOperationL( 
    TMsvId aNotifId,
    const TUint32 aOperation )
    {
    //
    // Get hands on index entry based on entryId
    //
    CMsvEntry* cEntry = NULL;
    cEntry = Session().GetEntryL( aNotifId );
    CleanupStack::PushL( cEntry ); // ***
    TMsvEntry tEntry = cEntry->Entry();

    //
    // Check that entry is a notification
    //
    if( cEntry->Entry().iType != KUidMsvMessageEntry
        || cEntry->Entry().iMtm != KUidMsgMMSNotification )
        {
        CleanupStack::PopAndDestroy( cEntry );
        return KErrNotSupported;
        }

    //
    // Make some checks concerning the current status of notification and
    // return with error if reservation is not possible
    //
    // if operation is forbidden OR
    // if ( (the aOperation is fetch or forward) and notification is deleted successfully from mmbox)
    if( tEntry.iMtmData2 & KMmsNewOperationForbidden ||
        ( ( aOperation == KMmsOperationFetch || aOperation == KMmsOperationForward ) &&
        tEntry.iMtmData2 & KMmsOperationFinished && 
        ! ( tEntry.iMtmData2 & KMmsOperationResult ) &&
        ! ( tEntry.iMtmData2 & KMmsStoredInMMBox ) &&
        tEntry.iMtmData2 & KMmsOperationDelete ) )
        {
        CleanupStack::PopAndDestroy( cEntry );
        return KErrInUse;        
        }

    //
    // Set the flags
    //
    // reserve the operation
    MarkNotificationOperationReserved( tEntry, aOperation );
    tEntry.SetReadOnly( EFalse );

    //
    // Save the entry
    //
    cEntry->ChangeL( tEntry );
    CleanupStack::PopAndDestroy( cEntry );
    return KErrNone;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::MarkFreeNotificationsReservedL
// ---------------------------------------------------------
//
void CMmsNotificationClientMtm::MarkFreeNotificationsReservedL(
    CMsvEntrySelection& aNotifications, const TUint32 aOperation )
    {
    TInt count = aNotifications.Count();
    if ( count == 0 )
        {
        return;
        }
    CMsvEntry* clientEntry = Session().GetEntryL( aNotifications.At( 0 ));
    CleanupStack::PushL( clientEntry );

    for ( TInt i = aNotifications.Count() - 1 ; i >= 0 ; --i ) 
        {
        TBool drop = EFalse;
        TBool alreadyMarked = EFalse;

        clientEntry->SetEntryL( aNotifications.At( i ) );
        TMsvEntry entry = clientEntry->Entry();
        TMsvId duplicate = KMsvNullIndexEntryId;
        if ( FreeNotification( entry, aOperation ) )
            {
            // Check possible duplicate, if mmbox folder even exists
            TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
            if ( mmboxFolder != KMsvNullIndexEntryId )
                {
                TMsvId parent = entry.Parent();
                
                CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
                CleanupStack::PushL( mmsHeaders );
                CMsvStore* store = clientEntry->ReadStoreL();
                CleanupStack::PushL( store );
                mmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                store = NULL;
                
                if ( parent == KMsvGlobalInBoxIndexEntryId )
                    {
                    
                    FindDuplicateNotificationL( mmboxFolder, 
                        *mmsHeaders, duplicate );
                    }
                else if ( parent == mmboxFolder )
                    {
                    FindDuplicateNotificationL( KMsvGlobalInBoxIndexEntryId, 
                        *mmsHeaders, duplicate );
                    }
                else
                    {
                    // keep LINT happy
                    }
                if ( duplicate != KMsvNullIndexEntryId )
                    {
                    // check if duplicate is free for a operation
                    clientEntry->SetEntryL( duplicate );
                    TMsvEntry dupEntry = clientEntry->Entry();
                    if ( FreeNotification( dupEntry, aOperation ))
                        {
                        // if original notification is in mmbox folder
                        // mark the duplicate reserved too.
                        if ( parent == mmboxFolder )
                            {
                            MarkNotificationOperationReserved( dupEntry, aOperation );
                            clientEntry->ChangeL( dupEntry );

                            clientEntry->SetEntryL( aNotifications.At( i ) );
                            entry = clientEntry->Entry();
                            MarkNotificationOperationReserved( entry, aOperation );
                            entry.SetReadOnly( EFalse );
                            clientEntry->ChangeL( entry );
                            alreadyMarked = ETrue;

                            store = clientEntry->EditStoreL();
                            CleanupStack::PushL( store );
                            mmsHeaders->RestoreL( *store );
                            mmsHeaders->SetRelatedEntry( duplicate );
                            mmsHeaders->StoreL( *store );
                            store->CommitL();
                            CleanupStack::PopAndDestroy( store );
                            store = NULL; 
                            }
                        clientEntry->SetEntryL( aNotifications.At( i ) );
                       
                        }
                    else // duplicate is not free-> original notification has to be dropped
                        {
                        drop = ETrue;
                        }
                    
                    }
                CleanupStack::PopAndDestroy( mmsHeaders );
                }
            
            }
        else // original notification is not free
            {
            drop = ETrue;
            }

        if ( drop )
            {
            // Remove the entry from selection
            aNotifications.Delete( i );
            }
        else // mark original notification reserved, unless it is already marked
            {
            if ( !alreadyMarked) 
                {
                clientEntry->SetEntryL( aNotifications.At( i ) );              
                entry = clientEntry->Entry();
                MarkNotificationOperationReserved( entry, aOperation );
                entry.SetReadOnly( EFalse );
                clientEntry->ChangeL( entry );
                }   
            }
        }
    aNotifications.Compress();

    CleanupStack::PopAndDestroy( clientEntry );

    }

 
// ---------------------------------------------------------
// CMmsNotificationClientMtm::FreeNotification
// ---------------------------------------------------------
//       
TBool CMmsNotificationClientMtm::FreeNotification( TMsvEntry& aEntry, const TUint32 aOperation )
    {
    if ( aEntry.iMtm != KUidMsgMMSNotification)
        {
        return EFalse;
        }
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
// CMmsNotificationClientMtm::MarkNotificationOperationReserved
// ---------------------------------------------------------
//       
void CMmsNotificationClientMtm::MarkNotificationOperationReserved( TMsvEntry& aEntry, 
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
// CMmsNotificationClientMtm::FindDuplicateNotificationL
// ---------------------------------------------------------
// 
void CMmsNotificationClientMtm::FindDuplicateNotificationL( TMsvId aParent, 
                                                           CMmsHeaders& aHeaders, 
                                                           TMsvId& aDuplicate )
    {

    aDuplicate = KMsvNullIndexEntryId;
    if ( aParent == KMsvNullIndexEntryId )
        {
        return;
        }

    CMsvEntry* cEntry = Session().GetEntryL( aParent );
    CleanupStack::PushL( cEntry );

    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgMMSNotification );
    CleanupStack::PushL( selection );

    TInt count = selection->Count();
    if ( count == 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        CleanupStack::PopAndDestroy( cEntry );
        return;
        }

    CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CleanupStack::PushL( mmsHeaders );
     
    for ( TInt i = count; i > 0 && ( aDuplicate == KMsvNullIndexEntryId ); --i )
        {
        cEntry->SetEntryL( selection->At( i - 1 ) );
                   
        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );
        mmsHeaders->RestoreL( *store );
        CleanupStack::PopAndDestroy( store );

        // content location must match 
        if ( mmsHeaders->ContentLocation().Compare( aHeaders.ContentLocation() ) == 0 )
            {
            aDuplicate = selection->At( i - 1 );
            }        
        }

    CleanupStack::PopAndDestroy( mmsHeaders );
    CleanupStack::PopAndDestroy( selection );
    CleanupStack::PopAndDestroy( cEntry );
    }
    
// ---------------------------------------------------------
// CMmsClientNotificationMtm::GetExtendedText
// ---------------------------------------------------------
//
const TPtrC CMmsNotificationClientMtm::GetExtendedText() const
    {
    return iMmsHeaders->ExtendedNotification();
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::FetchMessagesL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::FetchMessagesL( 
    const CMsvEntrySelection& aSelection,
    TRequestStatus& aCompletionStatus )
    {
    if ( aSelection.Count() == 0 )
        {
        User::Leave( KErrNotFound );
        }

    CMsvEntrySelection* notifications = aSelection.CopyLC();
  
     // mark free notifications reserved. Others are dropped out.
    MarkFreeNotificationsReservedL( *notifications, KMmsOperationFetch );

    // check if none of the entries are allowed to start fetch
    if ( notifications->Count() == 0 )
        {
        // leave will destroy reserved items from cleanupstack
        User::Leave( KErrNotSupported );
        }

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    // Fetch messages
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsScheduledReceiveForced,
        *notifications,
        paramPack,
        aCompletionStatus ); 

    CleanupStack::PopAndDestroy( notifications );
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::FetchMessageL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::FetchMessageL( 
    TMsvId aUid, 
    TRequestStatus& aCompletionStatus )
    {
    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection );
    selection->AppendL( aUid );

    // Mark the notification reserved for fetching is possible
    MarkFreeNotificationsReservedL( *selection, KMmsOperationFetch );

    // if notification is not allowed to be fetched, 
    // the notification is dropped from selection. 
    if ( selection->Count() == 0 )
        {
        CleanupStack::PopAndDestroy( selection );
        return NULL;
        }

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    // Fetch the message
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsScheduledReceiveForced,
        *selection,
        paramPack,
        aCompletionStatus ); 

    CleanupStack::PopAndDestroy( selection );
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::FetchAllL
// ---------------------------------------------------------
//

CMsvOperation* CMmsNotificationClientMtm::FetchAllL( TRequestStatus& aCompletionStatus,
    TBool aForced )
    {
    // List notifications that have no active operation
    CMsvEntrySelection* notifications = ListNotificationsL();
    if ( notifications->Count() == 0 )
        {
        delete notifications;
        notifications = NULL;
        User::Leave( KErrNotFound );
        }
    CleanupStack::PushL( notifications );
   
    // Loop through the notifications and reserve them for the operation
    TInt retval = KErrNone;
    TInt count = notifications->Count();
    for ( TInt i = count; i > 0; --i ) 
        {
        retval = ReserveNotificationOperationL( notifications->At( i - 1 ), KMmsOperationFetch );
        if( retval != KErrNone )
            {
            // Remove the entry from selection
            notifications->Delete( i - 1 );
            } 
        }

    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    // Fetch the message
    CMsvOperation* op = NULL;
    if ( aForced )
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceiveForced,
            *notifications,
            paramPack,
            aCompletionStatus ); 
        }
    else
        {
        op = InvokeAsyncFunctionL(
            EMmsScheduledReceive,
            *notifications,
            paramPack,
            aCompletionStatus ); 
        }

    CleanupStack::PopAndDestroy( notifications );
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::NotificationCount
// ---------------------------------------------------------
//
TInt CMmsNotificationClientMtm::NotificationCount()
    {
    TInt numberOfNotifications = -1;

    TInt error = KErrNone;
    CMsvEntrySelection* notifications = NULL;

    TRAP( error, 
        { notifications = ListNotificationsL();
        });
        
    if ( error == KErrNone )
        {
        numberOfNotifications = notifications->Count();
        }
        
    delete notifications;
    notifications = NULL;    
    
    return numberOfNotifications;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::ListNotificationsL
// ---------------------------------------------------------
//
CMsvEntrySelection* CMmsNotificationClientMtm::ListNotificationsL()
    {
    return CMmsClientMtm::ListNotificationsInInboxL();
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::DeleteNotificationL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::DeleteNotificationL( 
    const CMsvEntrySelection& aSelection,
    TMmsDeleteOperationType aDeleteType,
    TRequestStatus& aCompletionStatus )
    {
    
    if ( aSelection.Count() == 0 )
        {
        // if nothing to delete, operation is complete
        TPckgC < TMsvId > progress = 0;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress, 
            KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone );
        }
    
    //
    // aDeleteType has to be delivered to server side.
    // Using TCommandParameters.iError for it on purpose.
    //
    TCommandParameters parameters;
    parameters.iError = aDeleteType;
    TCommandParametersBuf paramPack( parameters );

    //
    // Modifying indexentries
    //
    CMsvEntry* cEntry = NULL;
    cEntry = Session().GetEntryL( aSelection.At( 0 ) );
    CleanupStack::PushL( cEntry ); // ***
    for ( TInt i = 0; i < aSelection.Count(); ++i ) 
        {
        cEntry->SetEntryL( aSelection.At( i ) );
        TMsvEntry tEntry = cEntry->Entry();
        tEntry.SetReadOnly( EFalse );
        cEntry->ChangeL( tEntry );
        }
    CleanupStack::PopAndDestroy( cEntry );

    //
    // NOTE: Notification delete operations perform majority of the work in
    // the server side. This includes also notification reservation for the operation.
    //

    //
    // Calling server side
    //
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsScheduledNotificationDelete,
        aSelection,
        paramPack,
        aCompletionStatus ); 
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::UnscheduledDeleteNotificationL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::UnscheduledDeleteNotificationL( 
    const CMsvEntrySelection& aSelection,
    TMmsDeleteOperationType aDeleteType,
    TRequestStatus& aCompletionStatus )
    {
    
    if ( aSelection.Count() == 0 )
       {
        // if nothing to delete, operation is complete
        TPckgC < TMsvId > progress = 0;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress, 
            KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone );
        }
    
    //
    // aDeleteType has to be delivered to server side.
    // Using TCommandParameters.iError for it on purpose.
    //
    TCommandParameters parameters;
    parameters.iError = aDeleteType;
    TCommandParametersBuf paramPack( parameters );

    //
    // NOTE: Notification delete operations perform majority of the work in
    // the server side. This includes also notification reservation for the operation.
    //

    //
    // Calling server side
    //
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsNotificationDelete,
        aSelection,
        paramPack,
        aCompletionStatus ); 
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::DeleteAllNotificationsL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::DeleteAllNotificationsL( 
    TMmsDeleteOperationType aDeleteType,
    TRequestStatus& aCompletionStatus )
    {
    //
    // Get selection of all the notifications in Inbox
    // (operations that do not have ongoing operations)
    // 
    CMsvEntrySelection* selection = ListNotificationsL();
    
    if ( selection->Count() == 0 )
        {
        delete selection;
        selection = NULL;
        // if nothing to delete, operation is complete
        TPckgC < TMsvId > progress = 0;
        return  CMsvCompletedOperation::NewL( Session(), Type(), progress, 
            KMsvLocalServiceIndexEntryId, aCompletionStatus, KErrNone );
        }
    
    CleanupStack::PushL( selection );

    //
    // Call DeleteNotificationL with the selection just created
    //
    CMsvOperation* op = DeleteNotificationL( 
        *selection, 
        aDeleteType, 
        aCompletionStatus );
    CleanupStack::PopAndDestroy( selection );
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::DeleteForwardEntryL
// ---------------------------------------------------------
//
void CMmsNotificationClientMtm::DeleteForwardEntryL( const CMsvEntrySelection& aSelection )
    {
    //
    // Loop through the selection of forward entries
    //
    TInt count = aSelection.Count();
    for( TInt index = 0; index < count; ++index )
        {
        iMsvEntry->SetEntryL( aSelection.At( index ) );
        TMsvEntry tEntry = iMsvEntry->Entry();
        // Test entry type
        if( tEntry.iType != KUidMsvMessageEntry ||
            tEntry.iMtm != KUidMsgMMSNotification ||
            !( tEntry.iMtmData1 & KMmsMessageForwardReq ) )
            {
            continue;
            }
        // Test entry location
        TMsvId parentId = tEntry.Parent();
        if( ( parentId != KMsvGlobalOutBoxIndexEntryId ) &&
            ( parentId != KMsvSentEntryId ) )
            {
            continue;
            }
        // Test that entry is not in a middle of some other action
        if( tEntry.SendingState() == KMsvSendStateSending )
            {
            // Too late to delete
            continue;
            }
        else
            {
            // Quickly "reserve" the entry for deletion
            tEntry.SetSendingState( KMsvSendStateSuspended );
            iMsvEntry->ChangeL( tEntry );
            }
        //
        // Entry will do; it will be deleted.
        //

        // But first clearing possible related notification
        CMsvStore* store = iMsvEntry->ReadStoreL();
        CleanupStack::PushL( store ); // ***
        CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
        CleanupStack::PushL( mmsHeaders );
        mmsHeaders->RestoreL( *store );
        TMsvId relatedId = mmsHeaders->RelatedEntry();
        CleanupStack::PopAndDestroy( mmsHeaders );
        CleanupStack::PopAndDestroy( store );

        // If forward entry has a "related id" (i.e. id of the notification),
        // context is changed to it and it will be cleared.
        if( relatedId != KMsvNullIndexEntryId )
            {
            // Change to related notification
            iMsvEntry->SetEntryL( relatedId );
            tEntry = iMsvEntry->Entry();
            // "Clear" the flags
            tEntry.iMtmData2 &= ~KMmsOperationIdentifier;   // clear possible old operations
            tEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // not forbidden
            tEntry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
            tEntry.iMtmData2 &= ~KMmsOperationFinished;     // not finished (this is notification's initial state)
            tEntry.iMtmData2 &= ~KMmsOperationResult;       // clear also the result flag ("assuming OK")
            tEntry.SetReadOnly( ETrue );
            iMsvEntry->ChangeL( tEntry );
            }

        // Activating parent entry of the forward entry
        iMsvEntry->SetEntryL( parentId );
        // Delete forward entry
        iMsvEntry->DeleteL( aSelection.At( index ) );
        } // for loop
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::MmboxInfoL
// ---------------------------------------------------------
//
TBool CMmsNotificationClientMtm::MmboxInfoL( TMmboxInfo& aMmboxInfo )
    {
    // check first, if quota information is available.
    TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
    CMsvEntry* cEntry = Session().GetEntryL( KMsvLocalServiceIndexEntryId );
    CleanupStack::PushL( cEntry );
    TInt pushedToStack = 1;

    cEntry->SetEntryL( mmboxFolder );
    // Show invisible entries
    cEntry->SetSortTypeL( TMsvSelectionOrdering( KMsvNoGrouping, EMsvSortByIdReverse, ETrue ) );
    CMsvEntrySelection* selection = cEntry->ChildrenWithMtmL( KUidMsgTypeMultimedia );
    CleanupStack::PushL( selection );
    pushedToStack++;

    TBool quotaInfoAvailable = EFalse;

    if ( selection->Count() > 0 )
        {                
        cEntry->SetEntryL( selection->At( 0 ));
        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );
        pushedToStack++;
        CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
        CleanupStack::PushL( mmsHeaders );
        pushedToStack++;
        mmsHeaders->RestoreL( *store );
        
        // Get quota info
        CMmsMMBoxViewHeaders& viewHeaders = mmsHeaders->MMBoxViewHeadersL();

        aMmboxInfo.mmboxTotalInBytes = viewHeaders.MMBoxTotalSize();
        aMmboxInfo.mmboxTotalInMessageCount = viewHeaders.MMBoxTotalNumber();
        aMmboxInfo.mmboxQuotaInBytes = viewHeaders.MMBoxQuotaSize();
        aMmboxInfo.mmboxQuotaInMessageCount = viewHeaders.MMBoxQuotaNumber();

        // get Date info
        TMsvEntry tEntry = cEntry->Entry();
        aMmboxInfo.date = tEntry.iDate;
        
        // error
        aMmboxInfo.error = tEntry.iError;
        
        quotaInfoAvailable = ETrue;
        }

    CleanupStack::PopAndDestroy( pushedToStack );
    return quotaInfoAvailable;
    }
// ---------------------------------------------------------
// CMmsNotificationClientMtm::UpdateMmBoxListL
// ---------------------------------------------------------
//
CMsvOperation* CMmsNotificationClientMtm::UpdateMmBoxListL(
            TRequestStatus& aCompletionStatus )
    {
    TCommandParameters parameters; // initialized to zero
    TCommandParametersBuf paramPack( parameters );

    CMsvEntrySelection* selection = new ( ELeave ) CMsvEntrySelection;
    CleanupStack::PushL( selection ); 
    selection->AppendL( iServiceId );

    // Update mmbox list
    CMsvOperation* op = InvokeAsyncFunctionL(
        EMmsUpdateMmboxList,
        *selection,
        paramPack,
        aCompletionStatus ); 

    CleanupStack::PopAndDestroy( selection );
    return op;
    }

// ---------------------------------------------------------
// CMmsNotificationClientMtm::GetMmboxFolderL
// ---------------------------------------------------------
//
TMsvId CMmsNotificationClientMtm::GetMmboxFolderL()
    {
    return iMmsSettings->MMBoxFolder();
    }
   
// ---------------------------------------------------------
// CMmsNotificationClientMtm
// ---------------------------------------------------------
//
const TPtrC CMmsNotificationClientMtm::GetApplicationId() const
    {
    return iMmsHeaders->ApplicId();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  


