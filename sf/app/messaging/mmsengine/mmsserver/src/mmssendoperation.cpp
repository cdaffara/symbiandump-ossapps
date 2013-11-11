/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     State machine for sending messages
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

// LOCAL INCLUDE FILES
#include    "mmsconst.h"
#include    "mmssendoperation.h"
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
#include    "MmsServerDebugLogging.h"

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
// CMmsSendOperation
// ---------------------------------------------------------
//
CMmsSendOperation::CMmsSendOperation( RFs& aFs ): CMmsBaseOperation( aFs )
    // members that get initial value 0, need not be initialized here
    {
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMmsSendOperation::ConstructL( CMmsSettings* aMmsSettings )
    {
    CMmsBaseOperation::ConstructL( aMmsSettings );
    iMmsHeaders = CMmsHeaders::NewL( iMmsSettings->MmsVersion() );
    // iSent is just a copy to retain old name for the time being.
    iSent = iSuccessful;
    User::LeaveIfError( iShareProtectedFileSession.Connect() );
    iShareProtectedFileSessionOpened = ETrue;
    User::LeaveIfError( iShareProtectedFileSession.ShareProtected() ); 
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CMmsSendOperation* CMmsSendOperation::NewL( RFs& aFs, CMmsSettings* aMmsSettings )
    {
    CMmsSendOperation* self = new ( ELeave ) CMmsSendOperation( aFs );
    CleanupStack::PushL( self );
    self->ConstructL( aMmsSettings );
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------
// ~CMmsSendOperation
// ---------------------------------------------------------
//
CMmsSendOperation::~CMmsSendOperation()
    {
    Cancel();
    
    if ( iShareProtectedFileSessionOpened )
        {
        iShareProtectedFileSession.Close();
        }
    
    // iSelection, iServerEntry, and iMmsSettings
    // are not deleted, because they belong to caller

    // iSent is not deleted because it is just a copy of iSuccessful

    delete iMmsHeaders;
    }

// ---------------------------------------------------------
// StartL
// ---------------------------------------------------------
//
void CMmsSendOperation::StartL(
    CMsvEntrySelection& aSelection,
    CMsvServerEntry& aServerEntry,
    TMsvId aService,
    TRequestStatus& aStatus )
    {
    LOG(_L("CMmsSendOperation::StartL") );
    
    __ASSERT_DEBUG( iState==EMmsOperationIdle, gPanic( EMmsAlreadyBusy ) );
    CMmsBaseOperation::StartL( aSelection, aServerEntry, aService, aStatus );
    iMmsHeaders->Reset();
    }

// ---------------------------------------------------------
// EncodePDUL
// ---------------------------------------------------------
//
void CMmsSendOperation::EncodePDUL()
    {
    LOG( _L("CMmsSendOperation::EncodePDUL") );
    
    CMsvStore* store = NULL;
    // In case of multiple entries, iError is reset here on every round
    // (because EncodePDUL starts the round)
    iError = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );

    // If user tries to delete the entry before it has been sent,
    // it is first put to suspended state, and then deleted.
    // If entry is suspended -> message will not be sent
    if( iError == KErrNone )
        {
        if( iServerEntry->Entry().SendingState() == KMsvSendStateSuspended ||
            iServerEntry->Entry().Deleted() )
            {
            // All activity stopped with this entry
            iError = KErrNotFound;
#ifndef _NO_MMSS_LOGGING_
            if( iServerEntry->Entry().SendingState() == KMsvSendStateSuspended )
                {
                LOG( _L("- message in suspended state") );
                }
            if( iServerEntry->Entry().Deleted() )
                {
                LOG( _L("- message in deleted state") );
                }
#endif
            }
        }    
#ifndef _NO_MMSS_LOGGING_
    else
        {
        LOG2( _L("- ERROR in SetEntry(): %d"), iError );
        }
#endif

    // Check if message contains closed content that should not be sent
    // ContainsClosedContentL leaves only if memory runs out
    
    if ( iError == KErrNone )
        {
        iError = CheckClosedContentL( *iServerEntry, iShareProtectedFileSession );
        }

    // Attempt to delete the message after this point will come too late,
    // and the message will be sent
    if( iError == KErrNone )
        {
        TRAP( iError, store = iServerEntry->EditStoreL(); )
        }
    if( iError == KErrNone )
        {
        CleanupStack::PushL( store ); // ***
        }
        
    // If error encountered quit here.
    if( iError != KErrNone )
        {
        FallThrough();
        return;
        }
        
    // Restore the message in order to encode it into binary
    // if RestoreL leaves -> serious OOM condition -> cannot continue.
    iMmsHeaders->RestoreL( *store );

    // Adding TID. If found already (retrying..), using it.
    if ( iMmsHeaders->Tid().Length() == 0 )
        {
        TBufC8<KMMSMAXTIDLENGTH> tid;
        tid.Des().NumUC( AllocateTID(), EHex );
        iMmsHeaders->SetTidL( tid );
        }
        
    // Clear old response text in case the message has failed before
    iMmsHeaders->SetResponseTextL( TPtrC() );

    // Message type
    iMmsHeaders->SetMessageType( KMmsMessageTypeMSendReq );

    // MMS version that is used
    // Make sure we specify our current version even if the message
    // has been created elsewhere and brought in by sychronization etc.
    iMmsHeaders->SetMmsVersion( iMmsSettings->MmsVersion() );

    // Store the changed headers. 
    // No check for critical disk space level here, because only a 16-byte TID
    // was added.
    iMmsHeaders->StoreL( *store );
    store->CommitL();

    // Close the store now (otherwise attachments can not be read..)
    CleanupStack::PopAndDestroy( store );

#ifndef _NO_MMSS_LOGGING_
    // calculate space needed and resize buffer
    const TMsvEntry& entry = iServerEntry->Entry();
    TInt size = entry.iSize;
    LOG2( _L("- message size about %d"), size );
#endif
    
    // ... instead using entry wrapper class
    iError = iEntryWrapper->SetCurrentEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if( iError != KErrNone )
        {
        iServerEntry->SetEntry( KMsvNullIndexEntryId ); // not needed any more..
        FallThrough();
        }
    else
        {
        // The actual encoding is executed by mmsencode class
        iEncoder->StartChunkedL( *iEntryWrapper, *iMmsHeaders, *iEncodeBuffer, iStatus );
        // release the service entry to allow cancelling while conncetion is being opened
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
        SetActive();
        }
    }

// ---------------------------------------------------------
// SubmitTransactionL
// ---------------------------------------------------------
//
void CMmsSendOperation::SubmitTransactionL()
    {
    LOG( _L("CMmsSendOperation::SubmitTransactionL") );
    
    if ( iError != KErrNone )
        {
        FallThrough();
        return;
        }

    // "Sending" is either local or global.
    // Global is normal case, local is only for test purposes.
    if ( !iMmsSettings->LocalMode() )
        {
        LOG( _L("- global mode") );
        // The base class does HTTP POST to home page URI
        CMmsBaseOperation::SubmitTransactionL();
        }
    else
        {
        // No disk space checking here, because this is test code
        LOG( _L("- local mode") );
        TInt error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
        if( error == KErrNone )
            {
            TMsvEntry entry = iServerEntry->Entry();
            if( iConnected )
                {
                entry.SetConnected( ETrue );
                }
            entry.SetSendingState( KMsvSendStateSending );
            iServerEntry->ChangeEntry( entry );
            }
        iServerEntry->SetEntry( KMsvNullIndexEntryId );

        // sending uses the localmode OUT directory
        TFileName* fileName = new( ELeave ) TFileName;
        CleanupStack::PushL( fileName );
        fileName->Copy( iMmsSettings->LocalModeOut() );
        _LIT( KRelated, "m.mms");
        iParse.Set( *fileName, &KRelated, NULL );
        fileName->Copy( iParse.FullName() );
        User::LeaveIfError( CApaApplication::GenerateFileName( iFs, *fileName ) );

        RFile file;
        iError = file.Create( iFs, *fileName, EFileWrite | EFileShareExclusive );
        // for message id generation
        iParse.Set( *fileName, NULL, NULL );
        LOG2( _L("- local mode, file create status %d "), iError );
        TPtrC8 ptr /*= iEncodeBuffer->Ptr( 0 )*/;
        TBool isLastChunk = EFalse; 
        while ( iError == KErrNone && !isLastChunk )
            {
            MMmsCodecDataSupplier* dataSupplier = iEncoder;
            iError = dataSupplier->GetNextDataPart( ptr, isLastChunk );
            file.Write( ptr );
            file.Flush();
            iError = dataSupplier->ReleaseData();
            }
        
        // discard the contents
        iEncodeBuffer->Reset();
            
        // done - close files
        file.Close();
        CleanupStack::PopAndDestroy( fileName );
        fileName = NULL;
        FallThrough();
        }
    }

// ---------------------------------------------------------
// StoreResponseL()
// ---------------------------------------------------------
//
void CMmsSendOperation::StoreResponseL()
    {
    LOG( _L("CMmsSendOperation::StoreResponseL") );

    // First we must analyze the response message to see if sending
    // was successful. In case we want to retry, we probably keep
    // our encoded message until we are either satisfied with the
    // result, or decide that it is hopeless.

    // The response from MMSC has been decoded earlier by the base class. 
    // Now the result must be saved - either the error code and text
    // or the message id depending if the sending was successful

    // The possible response message from MMSC is in iEncodeBuffer.
    // When we called the send transaction, the buffer contained
    // the encoded message. If the sending was successful, the
    // buffer contains the return message from MMSC

    // The transaction will not complete until a response has been
    // received from MMSC

    // 'error' tries to keep track of failures to access the entry
    // because of backup/restore or media unavailable
    TInt error = KErrNone; 
                           
    LOG2( _L("- iEncodeBuffer size %d "), iEncodeBuffer->Size() );

#ifndef _NO_MMSS_LOGGING_
    // In logging version, unsuccessful attempts' responses are dumped into file
    if( iError != KErrNone && iEncodeBuffer->Size() > 0 )
        {
        TFileName fileName;
        fileName.Copy( KMmsDefaultLogDirectory );
        TUint att;
        if ( iFs.Att( fileName, att ) == KErrNone )
            {
            _LIT( KRelated, "SendResp.mms");
            // don't mess up iParse - it may contain local mode filename
            TParse parse;
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
        error = KErrNone;
        }
#endif

    // We must check the result, and mark the entry as failed
    // if the sending fails. Failed entries may be retried later.

    // The error should stay in iError
    // Above error was set if we failed to set iEntryWrapper to point to NULL entry
    // that should never fail, so our error should still be KErrNone

    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    // if we could not access the entry we want, we are in really bad trouble.
    // We might be running backup/restore or the MMC might have been removed

    if( error == KErrNone )
        {
        // The actual send transaction should return a message ID
        // we should save its value
        CMsvStore* store = NULL;
        TRAP( error, {store = iServerEntry->EditStoreL();} )
#ifndef _NO_MMSS_LOGGING_
        if ( error != KErrNone )
            {
            LOG2( _L("-ERROR getting edit store: %d"), error );
            }
#endif
        // We add a couple of strings.
        // In any case, if we were successful, we are going to remove
        // the scheduling soon. That will free more disk space that
        // we take here. So we don't check here.
        // Besides, we need to update our entry, and if we cannot do it
        // we are in trouble.
        if( error == KErrNone )
            {
            LOG( _L("- update recipient scheduling data") );
            // Get the recipient scheduling info to be updated later.
            CleanupStack::PushL( store ); // ***
            CMmsScheduledEntry* mmsScheduledEntry =
                CMmsScheduledEntry::NewL( iServerEntry->Entry() );
            CleanupStack::PushL( mmsScheduledEntry ); // ***
            mmsScheduledEntry->RestoreL( *store );
            UpdateRecipient( iError, *mmsScheduledEntry );
            if( iError == KErrNone || iResponse->ResponseText().Length() > 0 )
                {
                // We must add the message ID for delivery reports
                // in local mode we use the generated filename
                if ( iResponse->MessageId().Length() == 0 && iMmsSettings->LocalMode() )
                    {
                    HBufC8* id = HBufC8::NewL( KMaxFileName );
                    CleanupStack::PushL( id );
                    id->Des().Copy( iParse.NameAndExt() );
                    iResponse->SetMessageIdL( id->Des() );
                    CleanupStack::PopAndDestroy( id );
                    id = NULL;
                    }

                iMmsHeaders->RestoreL( *store );
                if( iError == KErrNone )
                    {
                    // message was successfully sent, store ID
                    iMmsHeaders->SetMessageIdL( iResponse->MessageId() );
                    // clear TID to prevent same TID from being used for two different messages.
                    // If a sent message is copied elsewhere and then resent, it is considered
                    // a different message.
                    iMmsHeaders->SetTidL( TPtrC8() );
                    }
                if( iError != KErrNone )
                    {
                    iMmsHeaders->SetResponseTextL( iResponse->ResponseText() );
                    }
                iMmsHeaders->StoreL( *store );
                }
            // update entry fields to match scheduled entry
            // mainly this updates the recipient info (possible error)
            mmsScheduledEntry->StoreL( *store );
            store->CommitL();
            CleanupStack::PopAndDestroy( mmsScheduledEntry );
            CleanupStack::PopAndDestroy( store );
            }

        TMsvEntry entry = iServerEntry->Entry();
        entry.SetConnected( EFalse );
        entry.iError = iError;

        if( iError == KErrNone )
            {
            entry.SetReadOnly( EFalse );
            entry.SetFailed( EFalse );
            entry.SetSendingState( KMsvSendStateSent );
            }
        else
            {
            // We have a hopelessly failed message.
            // We mark it failed later, not yet
            entry.SetReadOnly( EFalse );
            }

        // We cannot set the entry to read only state here,
        // because we must still update the schedule.
        TInt tempError;
        tempError = iServerEntry->ChangeEntry( entry );
#ifndef _NO_MMSS_LOGGING_
        if( tempError != KErrNone )
            {
            LOG2( _L("- ERROR in ChangeEntry: %d"), tempError );            
            }
#endif
        // don't override error if already set
        if( error == KErrNone )
            {
            error = tempError;
            }
        }

    iServerEntry->SetEntry( KMsvNullIndexEntryId );

    // If the error is no longer KErrNone, there has been problems in updating
    // the message entry. Reason might be backup/restore or removal of MMC.
    // If the failure is due to backup/restore, we should try to wait for the
    // end of the operation and then retry from the start of this state.

    if( error <= (TInt) KMsvMediaUnavailable && error >= (TInt) KMsvIndexRestore )
        {
        LOG( _L("- must wait for backup end ") );
        TRequestStatus* status = &iStatus;
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete( status, error );
        return;
        }
    else if ( iError == KErrNone )
        {
        // The following will not fail
        iSuccessful->AppendL( ( *iFailed )[iCurrentMessageNo - 1] );
        iFailed->Delete( iCurrentMessageNo - 1 );
        LOG( _L("- Successful case: entry moved from iFailed to iSent selection") );
        }
    else
        {
        // keep LINT happy
        }

    // If we have updated our entry, the buffer can go
    iEncodeBuffer->Reset();

    FallThrough();        
    }

// ---------------------------------------------------------
// LogL
// ---------------------------------------------------------
//
void CMmsSendOperation::LogL()
    {
    LOG( _L("CMmsSendOperation::LogL") );
    
    TInt error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if ( error != KErrNone || iError != KErrNone ||
        ( iMmsHeaders->DeliveryReport() != EMmsYes  && iMmsHeaders->ReadReply() != EMmsYes ) )
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
        CommonLogEventInitializationL( *iMmsHeaders, entry );
        iServerEntry->SetEntry( KMsvNullIndexEntryId );
   
        iLogEvent->SetDataL( iMmsHeaders->MessageId() );
    
        // CMmsLog will set our status to KRequestPending
        iMmsLog->StartL( *iLogEvent, *iRemoteParties, iStatus );
        SetActive();
        isActive = ETrue;
        });

    // If logging has been successfully initialized, iMmsLog will complete us.
    // If something has gone wrong, we complete ourselves because we want to
    // continue finalization of the sending. We free our entry in any case
    
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    if ( !isActive )
        {
        FallThrough();
        return;
        }
    }

// ---------------------------------------------------------
// MoveEntryL
// ---------------------------------------------------------
//
void CMmsSendOperation::MoveEntryL()
    {
    // Moving entry into Sent folder or deleting it depending on settings
    LOG( _L("CMmsSendOperation::MoveEntryL") );
    
    TInt error;
    error = iServerEntry->SetEntry( iSelection->At( iCurrentMessageNo - 1 ) );
    if( iError == KErrNone && error == KErrNone )
        {
        TMsvEntry entry = iServerEntry->Entry();

        if( iMmsSettings->MoveToSent() )
            {

            if ( iMmsHeaders->DeliveryReport() == KMmsYes)
                {
                LOG( _L("CMmsSendOperation: Delivery reports wanted") );
                TInt recipientCount(0);
            	//Reset flags in mtmdata2
                entry.iMtmData2 |= KMmsDeliveryStatusPending;
                entry.iMtmData2 &= ~KMmsDeliveryStatusPartial;
                entry.iMtmData2 &= ~KMmsDeliveryStatysFailed; 
                entry.iMtmData2 &= ~KMmsDeliveryStatysDelivered; 
         
         
                recipientCount = ( iMmsHeaders -> ToRecipients()).MdcaCount()
                    + (iMmsHeaders -> CcRecipients()).MdcaCount()
                    + (iMmsHeaders -> BccRecipients()).MdcaCount();
         
            
               entry.iMtmData3 |= recipientCount << KMmsSentItemTotalRecipientsShift;
               entry.iMtmData3 &= ~KMmsSentItemSuccessfullyDeliveredMask;
               entry.iMtmData3 &= ~KMmsSentItemFailedDeliveryMask; 
                }
            else  //Reports not wanted
                {
            	//Reset delivery status flags in mtmdata2
                LOG( _L("CMmsSendOperation: Delivery reports not wanted") );
                entry.iMtmData2 &= ~KMmsDeliveryStatusMask;
                }
                
            iServerEntry->ChangeEntry(entry);
                
            // Move entry from Outbox to Sent Folder
            iError = iServerEntry->SetEntry( entry.Parent() );
            if ( iError == KErrNone )
                {
                iServerEntry->MoveEntryWithinService( entry.Id(), KMsvSentEntryIdValue );
                LOG( _L("CMmsSendOperation::Moved to SENT") );           
                }
            }
        else
            {
            // Move entry to Message Heaven
            iError = iServerEntry->SetEntry( entry.Parent() );
            if ( iError == KErrNone )
                {
                iServerEntry->DeleteEntry( entry.Id() );
                }
            }
        }
    iServerEntry->SetEntry( KMsvNullIndexEntryId );
    FallThrough();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

