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
*     State machine for sending a delete request to network
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

// the rest are local includes, needed always
#include    "mmsconst.h"
#include    "mmsdeleteoperation.h"
#include    "mmssession.h"
#include    "mmssettings.h"
#include    "mmsservercommon.h"
#include    "mmsheaders.h"
#include    "mmsmmboxmessageheaders.h"
#include    "mmsencode.h"
#include    "mmsdecode.h"
#include    "mmsscheduledentry.h"
#include    "mmsgenutils.h"
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
// CMmsDeleteOperation::CMmsDeleteOperation
//
// ---------------------------------------------------------
//
CMmsDeleteOperation::CMmsDeleteOperation( RFs& aFs ):
    CMmsBaseOperation( aFs )
    {
    //
    // members that get initial value 0, need not be initialized here
    //
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::ConstructL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::ConstructL( CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsRequestHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::NewL
//
// ---------------------------------------------------------
//
CMmsDeleteOperation* CMmsDeleteOperation::NewL( RFs& aFs, CMmsSettings* aMmsSettings )
    {
    CMmsDeleteOperation* self = new ( ELeave ) CMmsDeleteOperation( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );
    return self;
    }

    
// ---------------------------------------------------------
// CMmsDeleteOperation::~CMmsDeleteOperation
//
// ---------------------------------------------------------
//
CMmsDeleteOperation::~CMmsDeleteOperation()
    {
    Cancel();
    delete iMmsRequestHeaders;
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::DoCancel
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::DoCancel()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::DoCancel") );
    #endif
    CMmsBaseOperation::DoCancel();

    // Clear possible flags
    for( TInt i = iSelection->Count()-1; i >= 0; i-- )
        {
        // original entry
        if ( iServerEntry->SetEntry( iSelection->At( i ) ) == KErrNone )
            {
            TMsvEntry tEntry = iServerEntry->Entry();
            TMsvId parent = tEntry.Parent();
            // If notification in iSelection is in mmbox folder, mark the
            // possible duplicate notification in inbox too. 
            if ( parent == iMmsSettings->MMBoxFolder() )
                { 
    #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("possible duplicates has to be checked") );
    #endif            
                TInt error = KErrNone;
                TRAP ( error, MarkDuplicateL( EMmsNotificationOperationFailed, *iServerEntry ) );
    #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Trap failed: error %d "), error );
    #endif
                }
            
            // reload to make sure any changes are included
            // (there should be no changes)
            tEntry = iServerEntry->Entry();
            MarkNotificationOperationFailed( tEntry );
            
            // put to read only state after duplicates have been marked
            // because MarkDuplicateL may want to edit link to related entry
            tEntry.SetReadOnly( ETrue );
            iServerEntry->ChangeEntry( tEntry );        

            }
        }
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::StartL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::StartL(
    TMmsDeleteOperationType aDeleteType,
    CMsvEntrySelection& aSelection,
    CMsvServerEntry& aServerEntry,
    TMsvId aService,
    TRequestStatus& aStatus )
    {
    iDeleteType = aDeleteType;
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::StartL") );
    if( iDeleteType == EMmsDeleteNotificationOnly ) TMmsLogger::Log( _L("- EMmsDeleteNotificationOnly") );
    if( iDeleteType == EMmsDeleteMMBoxOnly ) TMmsLogger::Log( _L("- EMmsDeleteMMBoxOnly") );
    if( iDeleteType == EMmsDeleteBoth ) TMmsLogger::Log( _L("- EMmsDeleteBoth") );
    #endif
    // Make sure the version number is correctly set
    CMmsBaseOperation::StartL( aSelection, aServerEntry, aService, aStatus );
    iMmsRequestHeaders->Reset();

    //
    // Delete operation does not work as regular operations:
    //
    // iFailed list is emptied because retries are not used for delete
    iFailed->Reset();
    // iCurrentMessageNo makes no sense in case of delete
    iCurrentMessageNo = 0;
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::StartL
// This just overrides the base class version.
// ---------------------------------------------------------
//
void CMmsDeleteOperation::StartL(
    CMsvEntrySelection&,
    CMsvServerEntry&,
    TMsvId,
    TRequestStatus& )
    {
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::EncodePDUL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::EncodePDUL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::EncodePDU") );
    #endif
    TInt err = KErrNone;  // used to trap errors that are not propagated further
   
    // Go through iSelection and make various checks   
    TMsvEntry tEntry;
    for( TInt i = iSelection->Count()-1; i >= 0; i-- )
        {
        if ( iServerEntry->SetEntry( iSelection->At( i ) ) == KErrNone )
            {
            tEntry = iServerEntry->Entry();

            // Check that selection contains only "idle" notifications
            if( ( tEntry.iMtm != KUidMsgMMSNotification ) ||
                ( tEntry.iMtm == KUidMsgMMSNotification && 
                tEntry.iMtmData2 & KMmsNewOperationForbidden ) )
                {
                // Take out all non-notifications and notifications that have some operation ongoing
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- notif not idle, skipping" ) );
    #endif
                iSelection->Delete( i );
                }
            else
                {
                // If only local delete requested, delete the entry
                if( iDeleteType == EMmsDeleteNotificationOnly )
                    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- just deleting the notif entry" ) );
    #endif
                    TRAP( err, DeleteNotificationEntryL() );
    #ifndef _NO_MMSS_LOGGING_
                    if( err != KErrNone )
                        {
                        TMmsLogger::Log( _L( "- ERROR: DeleteNotificationEntryL left" ) );
                        }
    #endif
                    iSelection->Delete( i );
                    }
                else // EMmsDeleteMMBoxOnly OR EMmsDeleteBoth
                    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- mmbox deletion wanted" ) );
    #endif
                    // Is there a message in MMBox.
                    if( !(tEntry.iMtmData2 & KMmsStoredInMMBox ) )
                        {
                        // No need to care about possible duplicate notification. 
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- accordig to mmbox flag, message is not in mmbox" ) );
    #endif
                        // If both requested, but no message in MMBox,
                        // just delete the notification.
                        if( iDeleteType == EMmsDeleteBoth )
                            {                        
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- local deletion requested, performing it" ) );
    #endif
                            TRAP( err, DeleteNotificationEntryL() );
    #ifndef _NO_MMSS_LOGGING_
                            if( err != KErrNone )
                                {
                                TMmsLogger::Log( _L( "- ERROR: DeleteNotificationEntryL left" ) );
                                }
    #endif
                            }
                        else // iDeleteType == EMmsDeleteMMBoxOnly
                            {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- can not delete from mmbox, clearing entry flags" ) );
    #endif
                            MarkNotificationOperationFailed( tEntry );
                            tEntry.SetReadOnly( ETrue );
                            iServerEntry->ChangeEntry( tEntry );
                            }
                        // As message not in MMBox, delete entry from selection
                        iSelection->Delete( i );
                        }
                    }
                }
            }
        else
            {
            // take out an entry that could not be accessed
            iSelection->Delete( i );
            }
        }

    // delete from mmbox is wanted and the notifications are stored in mmbox
    // Check duplicates, if the parent is in mmbox folder.
    
    // If duplicate is found, check if the duplicate is free. 
    // If the duplicate is free, mark the duplicate as "deleting operation going on". 
    // If duplicate is not free, drop the original notification from iSelection.

    for ( TInt j = iSelection->Count()-1; j >= 0; j-- )
        {
        if ( iServerEntry->SetEntry( iSelection->At( j ) ) == KErrNone ) 
            {
            // if original notification cannot be accessed there is no way
            // to check the notification
            tEntry = iServerEntry->Entry();

            if ( tEntry.Parent() == iMmsSettings->MMBoxFolder() )
                {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- check if duplicate is found" ) );
    #endif
                // parent is mmbox folder.
                CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
                CleanupStack::PushL( mmsHeaders ); 
                CMsvStore* store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store );           
                mmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                store = NULL;

                TMsvId duplicate = KMsvNullIndexEntryId;
                FindDuplicateNotificationL( KMsvGlobalInBoxIndexEntryIdValue,
                    *mmsHeaders, duplicate );
                    
                if ( duplicate != KMsvNullIndexEntryId )
                    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- duplicate found" ) );
    #endif
                    if ( iServerEntry->SetEntry( duplicate ) == KErrNone )
                        {
                        // Mark duplicate, if it is free for a new operation
                        TMsvEntry dupEntry = iServerEntry->Entry();

                        if ( FreeNotification( dupEntry, KMmsOperationDelete ) )
                            {
    #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- Marking duplicate" ) );
    #endif
                            MarkNotificationOperationReserved( dupEntry, KMmsOperationDelete );
                        
                            iServerEntry->ChangeEntry( dupEntry );

                            // Set duplicate as related entry to the original notification
                            if ( iServerEntry->SetEntry( iSelection->At( j ) ) == KErrNone )
                                {
                                tEntry = iServerEntry->Entry();
                                tEntry.SetReadOnly( EFalse );
                                iServerEntry->ChangeEntry( tEntry );

                                store = iServerEntry->EditStoreL();
                                CleanupStack::PushL( store );
                                mmsHeaders->RestoreL( *store );

                                mmsHeaders->SetRelatedEntry( duplicate );
                                mmsHeaders->StoreL( *store );
                                store->CommitL();
                                CleanupStack::PopAndDestroy( store );
                                store = NULL;
                                }
                            }
                        else
                            {
                            // duplicate is not free, drop the original from iSelection. 
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "- Duplicate is not free, drop the original notification from selection" ) );
    #endif
                            iSelection->Delete( j );
                            }
                        }
                    }
                CleanupStack::PopAndDestroy( mmsHeaders );
                }
            }
        }
    
    // iSelection contains notifications that are deleted from mmbox
    // Mark notifications as "deleting operation going on"

    for ( TInt k = iSelection->Count()-1; k >= 0; k--  )
        {
        if ( iServerEntry->SetEntry( iSelection->At( k ) ) == KErrNone )
            {
            tEntry = iServerEntry->Entry();
    #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L( "- reserve notif for deletion" ) );
    #endif

            // Mark all remaining notifications as operation-ongoing
            // operation is from now on delete
            MarkNotificationOperationReserved( tEntry, KMmsOperationDelete );
            
            tEntry.SetReadOnly( EFalse );   
            iServerEntry->ChangeEntry( tEntry );
            }
        }
    // 
    // If remote delete is required, creating PDU to be sent
    //
    if( ( iDeleteType != EMmsDeleteNotificationOnly ) && 
        ( iSelection->Count() > 0 ) )
        {
#ifndef _NO_MMSS_LOGGING_
TMmsLogger::Log( _L( "- creating PDU" ) );
#endif
        //
        // Create headers structure
        //
        // iMmsRequestHeaders is a member that has been created in the constructor
        // And Reset in the StartL function

        // MessageType
        iMmsRequestHeaders->SetMessageType( KMmsMessageTypeMBoxDeleteReq );

        // TransactionId
        TBufC8<KMMSMAXTIDLENGTH> tid;
        tid.Des().NumUC( AllocateTID(), EHex );
        iMmsRequestHeaders->SetTidL( tid );

        //
        // Content-Location array
        // Loop through selection and put all content locations into array
        //
        CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
        CleanupStack::PushL( mmsHeaders ); // ***
        for( TInt i = iSelection->Count() - 1; i >= 0; i-- )
            {
            // Get entry
            if ( iServerEntry->SetEntry( iSelection->At( i ) ) == KErrNone )
                {
                // Get read-only message store
                CMsvStore* store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store ); // ***
                // Restore all parts of multimedia message
                mmsHeaders->RestoreL( *store );

                // Add array to headers
                iMmsRequestHeaders->MMBoxMessageHeadersL().ContentLocationList()
                    .AppendL( mmsHeaders->ContentLocation() );
                CleanupStack::PopAndDestroy( store );
                }
            }
        CleanupStack::PopAndDestroy( mmsHeaders );

        //
        // Encode the PDU
        //
        iEncoder->EncodeHeadersL( *iMmsRequestHeaders, *iEncodeBuffer );
        }

    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::ConnectToIAPL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::ConnectToIAPL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::ConnectToIAPL") );
    #endif

    if( iDeleteType == EMmsDeleteNotificationOnly || iSelection->Count() == 0 )
        {
        FallThrough();
        }
    else 
        {
        CMmsBaseOperation::ConnectToIAPL();
        }
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::InitializeSessionL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::InitializeSessionL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::InitializeSessionL") );
    #endif

    if( iDeleteType == EMmsDeleteNotificationOnly || iSelection->Count() == 0 )
        {
        FallThrough();
        }
    else 
        {
        CMmsBaseOperation::InitializeSessionL();
        }
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::SubmitTransactionL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::SubmitTransactionL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L("CMmsDeleteOperation::SubmitTransactionL"));
#endif
    //
    // Maybe networking not needed at all
    // 
    if( iDeleteType == EMmsDeleteNotificationOnly || iSelection->Count() == 0 )
        {
        FallThrough();
        return;
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

    //
    // Networking is needed. Is connection open.
    //
    if ( !iConnected )
        {
        if ( iError == KErrNone )
            {
            iError = KErrCouldNotConnect;
            }
        }
        
    //
    // If something has gone wrong, just fall through
    // 
    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    //
    // Send
    //
    iMmsSession->SendMessageL(
        iUri->Des(),
        *iEncodeBuffer,
        *iEncoder,
        *iDecoder,
        iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::DecodeResponseL()
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::DecodeResponseL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsDeleteOperation::DecodeResponseL()" ) );
#endif

    if( iDeleteType != EMmsDeleteNotificationOnly && iSelection->Count() > 0 )
        {
        // iError is checked in base class method.
        CMmsBaseOperation::DecodeResponseL();
        }
    else
        {
        FallThrough();
        }
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::UpdateEntryStatusL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::UpdateEntryStatusL()
    {
#ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsDeleteOperation::UpdateEntryStatusL()" ) );
    TMmsLogger::Log( _L( "- Number of ids in iSelection: %d" ), iSelection->Count() );
#endif

    if( iDeleteType == EMmsDeleteNotificationOnly || iSelection->Count() == 0 )
        {
        FallThrough();
        return;
        }
                
    //
    // If error has occurred and there is nothing received from the network,
    // just correcting the flags in the notification entries as they were before.
    //
    if( iError != KErrNone )
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- error has occurred" ) );
        #endif
        MarkSelectionAsFailed();
        FallThrough();
        return;
        }
    //
    // Following is the the default behaviour: no errors, response received
    //
    else
        {
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- no errors" ) );
        #endif

        //
        // Get delete result array from response
        //
        const RPointerArray<CMmsDeleteResultArray>& resultArray
            = iResponse->DeleteResultArray();
        #ifndef _NO_MMSS_LOGGING_
        TMmsLogger::Log( _L( "- resultarray length: %d" ), iResponse->DeleteResultArray().Count() );
        #endif

        //
        // Make overall check for the response before checking each entry separately
        // First, messagetype
        //
        if( iResponse->MessageType() != KMmsMessageTypeMBoxDeleteConf )
            {
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L( "- ERROR: Response has wrong message type!" ) );
            #endif
            MarkSelectionAsFailed();
            FallThrough();
            return;
            }
        // Second, if result array contains status Unsupported-Message with no
        // related Content-Location, the transaction is interpreted as failed.
        for( TInt j = 0; j < resultArray.Count(); j++ )
            {
            if( resultArray[j]->ContentLocation().Length() == 0 && 
                resultArray[j]->ResponseStatus() == KMmsErrorStatusUnsupportedMessage )
                {
                // If array contains item that has no Content-Location,
                // something is wrong
                #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L( "- Response contains response-status with no corresponding ContentLocation!" ) );
                TMmsLogger::Log( _L( "  Interpreting this as delete failure." ) );
                #endif
                MarkSelectionAsFailed();
                FallThrough();
                return;
                }
            }

        //
        // Moving from response level checks into entry level checks...
        //
        // Loop through iSelection to separate failed and successful ones
        // (contains notifications that were sent to network)
        //
        CMmsHeaders* mmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
        CleanupStack::PushL( mmsHeaders );
        for( TInt i = iSelection->Count(); i > 0; i-- )
            {
            #ifndef _NO_MMSS_LOGGING_
            TMmsLogger::Log( _L( "- looping in iSelection[%d]" ), i-1 );
            #endif
            // Get Content-location of this notification
            if ( iServerEntry->SetEntry( iSelection->At( i-1 ) ) == KErrNone )
                {
                CMsvStore* store = iServerEntry->ReadStoreL();
                CleanupStack::PushL( store );
                mmsHeaders->RestoreL( *store );
                CleanupStack::PopAndDestroy( store );
                store = NULL;
                #ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L( "- store read" ) );
                #endif            
                TPtrC8 pContentLocation = mmsHeaders->ContentLocation();
                TMsvId duplicate = mmsHeaders->RelatedEntry();

                //
                // Search for the same content-location from response PDU
                // (if it is found, it's mmbox delete has failed)
                //
                TBool bMmboxDeleteSucceeded = ETrue;
                for ( TInt j = 0; j < resultArray.Count(); j++ )
                    {
                    // Check match
                    #ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L( "- looping in resultArray[%d]" ), j );
                    #endif
                    if( resultArray[j]->ContentLocation().Compare( pContentLocation ) == 0 )
                        {
                        // Get response status that will be investigated next
                        TInt32 responseStatus = resultArray[j]->ResponseStatus();
                        #ifndef _NO_MMSS_LOGGING_
                        TMmsLogger::Log( _L( "- Match found. X-Mms-Response-Status == %d" ), responseStatus );
                        #endif
                        // Found a match, now check the reason
                        if( responseStatus != KMmsStatusOk &&
                            responseStatus != KMmsErrorMessageNotFound && // this should not be used by the server
                            responseStatus != KMmsErrorPermanentMessageNotFound )
                            {
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- delete did not succeed for this notif" ) );
                            #endif
                            // This entry failed, removing it from iSelection
                            iSelection->Delete( i-1 );
                            bMmboxDeleteSucceeded = EFalse;

                            // Notification will not be deleted any longer regardless of iDeleteType
                            // Correcting the notification entry
                            // StoredInMMBox flag value not changed.
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- clearing entry flags" ) );
                            #endif

                            // if the original notification is in mmbox folder and a duplicate is found,
                            // mark the status of the duplicate notification too. 

                            if ( duplicate != KMsvNullIndexEntryId &&
                                iServerEntry->Entry().Parent() == iMmsSettings->MMBoxFolder() )
                                {
                                #ifndef _NO_MMSS_LOGGING_
                                TMmsLogger::Log( _L(
                                    "- duplicate exists and original notification is in mmbox folder" )
                                    );
                                #endif
                                // Clear related entry from the original notification
                                // and mark the duplicate as failed
                                TRAP_IGNORE( MarkDuplicateL(
                                    EMmsNotificationOperationFailed, *iServerEntry ) );
                                }

                            // Mark original notification. iServerEntry still points to it
                            TMsvEntry tEntry = iServerEntry->Entry();
                            MarkNotificationOperationFailed( tEntry );
                            tEntry.SetReadOnly( ETrue );
                            iServerEntry->ChangeEntry( tEntry );
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- original marked failed" ) );
                            #endif
                            }
                        }
                    } // inner for loop (resultArray)

                // Has the current entry been deleted from mmbox...
                if( bMmboxDeleteSucceeded )
                    {
                    #ifndef _NO_MMSS_LOGGING_
                    TMmsLogger::Log( _L( "- this notification was deleted from mmbox" ) );
                    #endif
                    
                    if ( iServerEntry->SetEntry( iSelection->At( i-1 ) ) == KErrNone )
                        {
                        // if notification is in inbox, check if a duplicate exits in mmbox folder
                        if ( iServerEntry->Entry().Parent() == KMsvGlobalInBoxIndexEntryIdValue )
                            {
                            TMsvId mmboxFolder = iMmsSettings->MMBoxFolder();
                            if ( mmboxFolder != KMsvNullIndexEntryId )
                                {
                                #ifndef _NO_MMSS_LOGGING_
                                TMmsLogger::Log( _L( "- original notification is in inbox" ) );
                                TMmsLogger::Log( _L( "- finding duplicate from mmbox inbox" ) );
                                #endif
                                FindDuplicateNotificationL( mmboxFolder, *mmsHeaders, duplicate );
                                }
                            }
                        
                        // if duplicate exits, mark it as "deleted successfully from mmbox"
                        if ( duplicate != KMsvNullIndexEntryId )
                            {
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- duplicate exists" ) );
                            #endif
                            
                            // iServerEntry should still be pointing to the original notification
                            
                            // Clear related entry from the original notification
                            // and mark the duplicate as "deleted from mmbox"
                            TRAP_IGNORE( MarkDuplicateL( EMmsDeletedFromMmbox, *iServerEntry ) )
                            }

                        // Should notification also be deleted...
                        if( iDeleteType == EMmsDeleteBoth )
                            {
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- about to delete notif entry" ) );
                            #endif
                            DeleteNotificationEntryL();
                            }
                        else // deletetype not equal to delete-both
                            {
                            #ifndef _NO_MMSS_LOGGING_
                            TMmsLogger::Log( _L( "- clearing notif flags" ) );
                            #endif
                            TMsvEntry tEntry = iServerEntry->Entry();
                            tEntry.iMtmData2 &= ~KMmsStoredInMMBox;         // not-stored-in-mmbox
                            tEntry.iMtmData2 &= ~KMmsNewOperationForbidden; // operations allowed
                            tEntry.iMtmData2 &= ~KMmsOperationOngoing;      // not ongoing
                            tEntry.iMtmData2 |= KMmsOperationFinished;      // finished
                            tEntry.iMtmData2 &= ~KMmsOperationResult;       // OK
                            tEntry.SetReadOnly( ETrue );
                            iServerEntry->ChangeEntry( tEntry );
                            }
                        }
                    }
                }
            } // outer for loop (iSelection)
        CleanupStack::PopAndDestroy( mmsHeaders );
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();    
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::DeleteNotificationEntryL
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::DeleteNotificationEntryL()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsDeleteOperation::DeleteNotificationEntryL()" ) );
    #endif
    // Get indexentry
    TMsvEntry tEntry = iServerEntry->Entry(); 
    TInt error = iServerEntry->SetEntry( tEntry.Parent());
    if ( error == KErrNone )
        {
        // Can delete entry only if setting entry to parent was successful
        iServerEntry->DeleteEntry( tEntry.Id() );        
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    }

// ---------------------------------------------------------
// CMmsDeleteOperation::MarkSelectionAsFailed
//
// ---------------------------------------------------------
//
void CMmsDeleteOperation::MarkSelectionAsFailed()
    {
    #ifndef _NO_MMSS_LOGGING_
    TMmsLogger::Log( _L( "CMmsDeleteOperation::MarkSelectionAsFailed()" ) );
    #endif

    // Loop through iSelection
    for( TInt i = iSelection->Count(); i > 0; i-- )
        {
        TInt error = KErrNone;
        error = iServerEntry->SetEntry( iSelection->At( i-1 ) );
        if ( error == KErrNone )
            {
            // If notification in iSelection is in mmbox folder, mark the
            // possible duplicate notification in inbox too. 
            if ( iServerEntry->Entry().Parent() == iMmsSettings->MMBoxFolder() )
                {
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L( "- check possible duplicate" ) );
#endif
                TRAP ( error, MarkDuplicateL( EMmsNotificationOperationFailed, *iServerEntry ) );
#ifndef _NO_MMSS_LOGGING_
                TMmsLogger::Log( _L("Trap failed: error %d "), error );
#endif
                }

            // Mark the original notification in selection
            // MarkDuplicateL does not mess up the original context of iServerEntry
            TMsvEntry tEntry = iServerEntry->Entry();
            MarkNotificationOperationFailed( tEntry );
            tEntry.SetReadOnly( ETrue );
            error = iServerEntry->ChangeEntry( tEntry );
#ifndef _NO_MMSS_LOGGING_
            if( error != KErrNone )
                {
                TMmsLogger::Log( _L( "ERROR: ChangeEntry() failed" ) );
                }
            else
                {
                TMmsLogger::Log( _L( "- selection[%d] flags are cleared" ), i );
                }
#endif
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File
