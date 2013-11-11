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
*      CMsvOperation class for MMS message creation.
*
*/



// INCLUDE FILES
#include    <msvids.h>
#include    <msvstore.h>
#include    <mtmdef.h>
#include    <mtmuids.h> 
#include    <mmsvattachmentmanager.h>
#include    <cmsvattachment.h>
#include    <cmsvmimeheaders.h>

#include    "mmsmessageoperation.h"
#include    "mmsconst.h"
#include    "mmssettings.h"
#include    "mmsheaders.h"
#include    "mmsgenutils.h"


// EXTERNAL FUNCTION PROTOTYPES  
extern void gPanic( TMmsPanic aPanic );

// CONSTANTS
const TInt KMmsRecipientGranularity = 6;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// ==================== LOCAL FUNCTIONS ====================
//

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//

CMmsMessageOperation::~CMmsMessageOperation()
    {
    Cancel();
    delete iSourceStore;
    delete iTargetStore;
    delete iSourceEntry;
    delete iTargetEntry;
    delete iMsvOperation;
    delete iSettings;
    delete iNewMmsHeaders;
    delete iDescription;
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateNewL
//
// ---------------------------------------------------------
EXPORT_C CMmsMessageOperation* CMmsMessageOperation::CreateNewL(
    TRequestStatus& aObserverRequestStatus,
    CMsvSession& aMsvSession,
    TMsvId aDestination,
    TMsvId aServiceId,
    TBool  aVisible,
    TBool  aInPreparation )
    {

    TMsvPartList parts = 0;

    CMmsMessageOperation* self = new(ELeave) CMmsMessageOperation(
        aObserverRequestStatus,
        aMsvSession,
        aDestination,
        parts,
        aServiceId,
        aVisible,
        aInPreparation );
    CleanupStack::PushL( self );
    self->ConstructL( KMsvNullIndexEntryId, ENew );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateReplyL
//
// ---------------------------------------------------------
EXPORT_C CMmsMessageOperation* CMmsMessageOperation::CreateReplyL(
    TRequestStatus& aObserverRequestStatus,
    CMsvSession& aMsvSession,
    TMsvId aMessageId,
    TMsvId aDestination,
    TMsvPartList aPartList,
    TMsvId aServiceId,
    TBool  aVisible,
    TBool  aInPreparation )
    {
    if ( aMessageId == KMsvNullIndexEntryId )
        {
        // if no source cannot reply
        User::Leave( KErrArgument );
        }

    // Set the attachment part flag off
    TMsvPartList parts = aPartList & ~KMsvMessagePartAttachments;

    CMmsMessageOperation* self = new(ELeave) CMmsMessageOperation(
        aObserverRequestStatus,
        aMsvSession,
        aDestination,
        parts,
        aServiceId,
        aVisible,
        aInPreparation );
    CleanupStack::PushL( self );
    self->ConstructL( aMessageId, EReply );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateForwardL
//
// ---------------------------------------------------------
EXPORT_C CMmsMessageOperation* CMmsMessageOperation::CreateForwardL(
    TRequestStatus& aObserverRequestStatus,
    CMsvSession& aMsvSession,
    TMsvId aMessageId,
    TMsvId aDestination,
    TMsvPartList aPartList,
    TMsvId aServiceId,
    TBool  aVisible,
    TBool  aInPreparation )
    {

    if ( aMessageId == KMsvNullIndexEntryId )
        {
        // if no source cannot forward
        User::Leave( KErrArgument );
        }

    // Set the attachment part flag on
    TMsvPartList parts = aPartList | KMsvMessagePartAttachments;

    CMmsMessageOperation* self = new(ELeave) CMmsMessageOperation(
        aObserverRequestStatus,
        aMsvSession,
        aDestination,
        parts,
        aServiceId,
        aVisible,
        aInPreparation );
    CleanupStack::PushL( self );
    self->ConstructL( aMessageId, EForward );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateForwardWithoutRetrievalL
//
// ---------------------------------------------------------
//
EXPORT_C TMsvId CMmsMessageOperation::CreateForwardWithoutRetrievalL( 
    CMsvSession& aMsvSession,
    const CMmsHeaders& aNotificationHeaders,
    const TMsvId aRelatedEntry,
    const TMsvId aDestination,
    TMsvId aServiceId,
    TBool  aVisible,
    TBool  aInPreparation )
    {
    //
    // Create entry
    //
    TMsvEntry entry;
    CMsvEntry* cMsvEntry = CMsvEntry::NewL( 
        aMsvSession, aDestination, TMsvSelectionOrdering() );
    CleanupStack::PushL( cMsvEntry );
    entry.iType = KUidMsvMessageEntry;
    entry.iServiceId = aServiceId;
    entry.iMtm = KUidMsgMMSNotification;
    entry.iMtmData1 |= KMmsMessageForwardReq;
    entry.iDate.UniversalTime();
    entry.SetInPreparation( aInPreparation );    
    entry.SetVisible( aVisible );
    cMsvEntry->CreateL( entry );

    //
    // Get MMS settings
    //
    CMmsSettings* settings = CMmsSettings::NewL();
    CleanupStack::PushL( settings );
    settings->LoadSettingsL();
    
    //
    // Create headers object
    //
    CMmsHeaders* headers = CMmsHeaders::NewL( settings->MmsVersion() );
    CleanupStack::PushL( headers );
    headers->SetSettings( settings );
    
    //
    // Set ContentLocation to headers object
    //
    headers->SetContentLocationL( aNotificationHeaders.ContentLocation() );
    headers->SetRelatedEntry( aRelatedEntry );
    // Following entries are copied to the forward request on purpose
    // even though they do not belong to the PDU sent to the network
    headers->SetSubjectL( aNotificationHeaders.Subject() );
    headers->SetMessageSize( aNotificationHeaders.MessageSize() );   
    headers->SetMessageClass( aNotificationHeaders.MessageClass() );
    headers->SetMessagePriority( aNotificationHeaders.MessagePriority() );
    // Set Report Allowed according to settings
    headers->SetReportAllowed( settings->DeliveryReportSendingAllowed() );

    //
    // Check diskspace before storing headers
    //
    RFs fs = aMsvSession.FileSession();
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL(
        &fs, 
        headers->Size(),
        aMsvSession.CurrentDriveL() ) )
        {
        User::Leave( KErrDiskFull );
        }

    //
    // Store headers to MessageStore
    //
    cMsvEntry->SetEntryL( entry.Id() );
    CMsvStore* store = cMsvEntry->EditStoreL();
    CleanupStack::PushL( store );   // ***
    headers->StoreL( *store );
    store->CommitL();

    //
    // Update and save index entry
    //
    entry.iSize = headers->Size();
    cMsvEntry->ChangeL( entry );

    CleanupStack::PopAndDestroy( store );
    CleanupStack::PopAndDestroy( headers );
    CleanupStack::PopAndDestroy( settings );
    CleanupStack::PopAndDestroy( cMsvEntry );
    return entry.Id();
    }

// ---------------------------------------------------------
// CMmsMessageOperation::ConstructL
//
// ---------------------------------------------------------
void CMmsMessageOperation::ConstructL( 
            TMsvId aMessageId, 
            TMmsOperation aOperation )
    {

    iOrigMessageId = aMessageId;
    iOperation = aOperation;
    iDataMember = KMsvNullIndexEntryId;
    iState = ECreateMmsHeaders;
    iAttachmentsSize = 0;
    iDescription = NULL;
    iOriginalRoot = 0;
    iAttachmentCount = 0;
    iNewRoot = -1; // not found yet

    // Create new message entry object to work with
    // iDestinationId is the target folder
    iTargetEntry = CMsvEntry::NewL( 
        iMsvSession, iDestinationId, TMsvSelectionOrdering() );
    // Show invisible entries too.
    iTargetEntry->SetSortTypeL( TMsvSelectionOrdering( 
        KMsvNoGrouping, EMsvSortByNone, ETrue ));

    if ( iOrigMessageId != KMsvNullIndexEntryId )
        {
        // Create entry for the original entry
        iSourceEntry = CMsvEntry::NewL( 
            iMsvSession, iOrigMessageId, TMsvSelectionOrdering() );
        // Show invisible entries too.
        iSourceEntry->SetSortTypeL( TMsvSelectionOrdering( 
            KMsvNoGrouping, EMsvSortByNone, ETrue ));
        // if we have an original entry, it will be read    
        iSourceStore = iSourceEntry->ReadStoreL();    
        }

    iSettings = CMmsSettings::NewL();
    
    // Add this objet to the scheduler
    CActiveScheduler::Add( this );

    ChangeStateL();

    iObserverRequestStatus = KRequestPending;

    SetActive();

    }

// ---------------------------------------------------------
// CMmsMessageOperation::FinalProgress
//
// ---------------------------------------------------------
//
EXPORT_C const TDesC8& CMmsMessageOperation::FinalProgress()
   {
    __ASSERT_ALWAYS( !IsActive(), gPanic( EMmsActiveInFinalProgress ));
    const TDesC8* progress = &KNullDesC8();
    TRAPD( leave, {progress = &ProgressL();} );
    __ASSERT_ALWAYS( leave == KErrNone, gPanic( EMmsFinalProgressFailed ));
    return *progress;
   }

// ---------------------------------------------------------
// CMmsMessageOperation::ProgressL
//
// ---------------------------------------------------------
//
const TDesC8& CMmsMessageOperation::ProgressL()
   {
   if (iState == EFinished )
       {
       iDataMember() = iNewMessageId;
       }
   return iDataMember;
   }

// ---------------------------------------------------------
// CMmsMessageOperation::RunL
//
// ---------------------------------------------------------
//
void CMmsMessageOperation::RunL()
    {

    if (iStatus.Int() != KErrNone )
        {
        ErrorRecovery( iStatus.Int() );
        return;
        }

    // Check the progress status in case that real progress is available.
    // only entry creation returns correct progress info
    if ( iState == ECreateMessageEntry ) 
        {
        TMsvLocalOperationProgress  progress = 
            McliUtils::GetLocalProgressL( *iMsvOperation );
        if ( progress.iError != KErrNone )
            {
            ErrorRecovery( progress.iError );
            return;
            }
        }
    if ( iState != EFinished )
        {
        TRAPD( error, SelectAndChangeToNextStateL() );
        if ( error )
            {
            ErrorRecovery( error );
            return;
            }
        else if ( iState != EFinished )
            {
            // Here we go again.
            SetActive();
            }
        else
            {
            // keep LINT happy
            }
        }
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CMmsMessageOperation
//
// ---------------------------------------------------------
CMmsMessageOperation::CMmsMessageOperation(
    TRequestStatus& aObserverRequestStatus, 
    CMsvSession& aMsvSession, 
    TMsvId aDestination, 
    TMsvPartList aPartList,
    TMsvId aServiceId,
    TBool aVisible,
    TBool  aInPreparation )
    : CMsvOperation( aMsvSession, EPriorityStandard, aObserverRequestStatus ),
      iDestinationId( aDestination ),
      iPartList( aPartList ),
      iServiceId( aServiceId ),
      iSettings( NULL ),
      iNewMmsHeaders( NULL ),
      iVisible( aVisible ),
      iInPreparation ( aInPreparation )
    {
    }

// ---------------------------------------------------------
// CMmsMessageOperation::ErrorRecovery
//
// ---------------------------------------------------------
void CMmsMessageOperation::ErrorRecovery( TInt aError )
    {
    // Delete entry and store objects associated with the new entry.
    // If an error is encountered, store is not committed, and the
    // entry under construction is deleted.
    // Deleting the message entry deletes all attachments that are
    // stored in the message store. Linked attachments are not deleted.
    delete iTargetStore;
    iTargetStore = NULL;
    delete iTargetEntry;
    iTargetEntry = NULL;
    // Remove the new message entry
    if ( iNewMessageId != KMsvNullIndexEntryId )
        {
        iMsvSession.RemoveEntry( iNewMessageId );
        }
    delete iSourceStore;
    iSourceStore = NULL;
    delete iSourceEntry;
    iSourceEntry = NULL;
        
    // complete the observer with error
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    }

// ---------------------------------------------------------
// CMmsMessageOperation::ChangeStateL
//
// ---------------------------------------------------------
void CMmsMessageOperation::ChangeStateL()
    {
    switch (iState)
        {
        case ECreateMmsHeaders:
            CreateNewMmsHeaderL();
            break;
        case ECreateMessageEntry:
            CreateMessageEntryL();
            break;
        case ECopyAttachments:
            CopyAttachmentsL();
            // Attachments are copied one by one.
            // We stay in ECopyAttachment state until all attachments have
            // been handled.
            iCurrentAttachment++;
            break;
        case EStoreMmsHeaders:
            StoreMmsHeadersL();
            break;
        case ECompleteMessage:
            CompleteMessageL();
            break;
        case EFinished:
            {
            TRequestStatus* status = &iObserverRequestStatus;
            User::RequestComplete( status, KErrNone );
            }
            break;
        default:
            // We are never supposed to be here.
            User::LeaveIfError( KErrNotSupported );
            break;
        }
    }

// ---------------------------------------------------------
// CMmsMessageOperation::RequestComplete
//
// ---------------------------------------------------------
void CMmsMessageOperation::RequestComplete( TInt aError )
    {
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aError );
    }

// ---------------------------------------------------------
// CMmsMessageOperation::SelectNextStateL
//
// ---------------------------------------------------------
void CMmsMessageOperation::SelectNextStateL()
    {

    switch (iState)
        {
        case ECreateMmsHeaders:
            iState = ECreateMessageEntry;
            break;
        case ECreateMessageEntry:
            {
            // Get the id of new entry
            TMsvLocalOperationProgress progress = 
                McliUtils::GetLocalProgressL( *iMsvOperation );
            iNewMessageId = progress.iId;
            
            iTargetEntry->SetEntryL( iNewMessageId );
            iTargetStore = iTargetEntry->EditStoreL();
            }
            // Fall through on purpose.
            // Attachment are copied before MMS headers are saved
            // so that we can adjust the root attachment id.
            // If the message has no attachments, we proceed directly
            // to saving MMS headers.
        case ECopyAttachments:
            if ( iPartList & KMsvMessagePartAttachments &&
                iCurrentAttachment < iAttachmentCount )
                {
                iState = ECopyAttachments;
                }
            else
                {
                iState = EStoreMmsHeaders;
                }
            break;
        case EStoreMmsHeaders:
            iState = ECompleteMessage;
            break;
        case ECompleteMessage:
            iState = EFinished;
            break;
        default:
            User::LeaveIfError( KErrNotSupported );
        }
    }

// ---------------------------------------------------------
// CMmsMessageOperation::SelectAndChangeToNextStateL
//
// ---------------------------------------------------------
void CMmsMessageOperation::SelectAndChangeToNextStateL()
    {
    SelectNextStateL();
    ChangeStateL();
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateNewMmsHeaderL
//
// ---------------------------------------------------------
void CMmsMessageOperation::CreateNewMmsHeaderL()
    {
    // Load settings
    iSettings->LoadSettingsL();

    // Create MMS headers object
    CMmsHeaders* originalMmsHeaders = NULL;
    delete iNewMmsHeaders;
    iNewMmsHeaders = NULL;

    if ( ( iOperation == EReply ) ||
         ( iOperation == EForward ))
        {
        // Copy appropriate parts from original MMS headers
        // We checked in the beginning that we have a source entry
        originalMmsHeaders = CMmsHeaders::NewL( iSettings->MmsVersion() );
        CleanupStack::PushL( originalMmsHeaders );     // ***
        // Get the original message's MMS headers
        originalMmsHeaders->RestoreL( *iSourceStore );
        // Copy data
        TBool isReply = iOperation == EReply ? ETrue : EFalse;
        
        iNewMmsHeaders = originalMmsHeaders->CopyL( iPartList, isReply, iMsvSession.FileSession() );
        
        // Remove own number from list if creating Reply
        if ( iOperation == EReply )
            {
            CDesCArray* recipientList = new ( ELeave )CDesCArrayFlat( KMmsRecipientGranularity );
            CleanupStack::PushL( recipientList );
            TInt i;
            for ( i = 0; i < iNewMmsHeaders->ToRecipients().MdcaCount(); i++ )
                {
                recipientList->AppendL(
                    TMmsGenUtils::PureAddress( iNewMmsHeaders->ToRecipients().MdcaPoint( i ) ) );
                }
            for ( i = 0; i < iNewMmsHeaders->CcRecipients().MdcaCount(); i++ )
                {
                recipientList->AppendL(
                    TMmsGenUtils::PureAddress( iNewMmsHeaders->CcRecipients().MdcaPoint( i ) ) );
                }
            for ( i = 0; i < iNewMmsHeaders->BccRecipients().MdcaCount(); i++ )
                {
                recipientList->AppendL(
                    TMmsGenUtils::PureAddress( iNewMmsHeaders->BccRecipients().MdcaPoint( i ) ) );
                }
                
           
            CleanupStack::PopAndDestroy( recipientList );
            }

        // the root must always be retained if it is specified
        if ( iPartList & KMsvMessagePartAttachments )
            {
            iOriginalRoot = originalMmsHeaders->MessageRoot();
            iNewMmsHeaders->SetRootContentIdL( originalMmsHeaders->RootContentId() );
            }
        CleanupStack::PopAndDestroy( originalMmsHeaders );
        }
    else
        {
        // No original message available
        iNewMmsHeaders = CMmsHeaders::NewL( iSettings->MmsVersion() );
        }
    // Reset MMS settings
    iNewMmsHeaders->SetSettings( iSettings );

    RequestComplete( KErrNone );

    }

// ---------------------------------------------------------
// CMmsMessageOperation::CreateMessageEntryL
//
// ---------------------------------------------------------
void CMmsMessageOperation::CreateMessageEntryL()
    {

    // Create new message entry

    // CREATE NEW INDEX ENTRY
    TMsvEntry entry;
    entry.iType = KUidMsvMessageEntry;
    entry.iServiceId = iServiceId;
    entry.iMtm = KUidMsgTypeMultimedia;
    entry.iDate.UniversalTime();

    // Set iMtmData extension flags
    if ( iOperation == EForward )
        {
        entry.iMtmData1 |= KMmsMessageForwarded;
        }

    // Set iDetails
    TPtrC to;
    if ( iNewMmsHeaders->ToRecipients().Count() > 0 )
        {
        to.Set( iNewMmsHeaders->ToRecipients()[0] );
        }
    HBufC* buffer = HBufC::NewL( KMmsMaxDescription );
    CleanupStack::PushL( buffer );
    TPtr pBuffer = buffer->Des();

    if ( TMmsGenUtils::IsValidMMSPhoneAddress( to, EFalse ) )
        {
        if ( TMmsGenUtils::GenerateDetails( to,
            pBuffer, KMmsMaxDescription, iMsvSession.FileSession() ) == KErrNone )
            {
            entry.iDetails.Set( pBuffer );
            }
        else
            {
            entry.iDetails.Set( to );
            }
        }
    else
        {
        entry.iDetails.Set( to );
        }

    // Set iDescription
    entry.iDescription.Set( iNewMmsHeaders->Subject());
    // if no subject, copy old description
    if ( ( iPartList & KMsvMessagePartAttachments ) &&
         ( iOrigMessageId != KMsvNullIndexEntryId ) &&
         ( entry.iDescription.Length() == 0 ) )
        {
        // Copy the old description, no need to scan message files again ...
        TPtrC pp;
        pp.Set( iSourceEntry->Entry().iDescription );
        delete iDescription;
        iDescription = NULL;
        iDescription = pp.AllocL();
        entry.iDescription.Set( *iDescription );
        }

    // The following ones will be set in completion.
    entry.SetInPreparation( ETrue );    
    entry.SetVisible( EFalse );         
    entry.iSize = 0;
    
    // Create the new message entry asynchronically.
    delete iMsvOperation;
    iMsvOperation = NULL;
    iCurrentAttachment = 0;
    
    if ( iPartList & KMsvMessagePartAttachments )
        {
        // get number of attachments
        MMsvAttachmentManager& sourceAttaMan = iSourceStore->AttachmentManagerL();
        iAttachmentCount = sourceAttaMan.AttachmentCount();
        }

    // Check that sufficient disk space available
    // for index entry
    iTargetEntry->SetEntryL( iDestinationId );
    RFs fs = iMsvSession.FileSession();
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( &fs, 
        sizeof( TMsvEntry ) 
        + entry.iDescription.Length() 
        + entry.iDetails.Length(),
        iMsvSession.CurrentDriveL() ) )
            {
            // we use standard error code here
            User::Leave( KErrDiskFull );
            }
    // Target entry still points to destination folder
    iMsvOperation = iTargetEntry->CreateL( entry, iStatus );

    CleanupStack::PopAndDestroy( buffer );
    }

// ---------------------------------------------------------
// CMmsMessageOperation::StoreMmsHeadersL
//
// ---------------------------------------------------------
void CMmsMessageOperation::StoreMmsHeadersL()
    {

    // save the correct id for new root
    if ( iPartList & KMsvMessagePartAttachments && 
        iNewRoot != -1 )
        {
        MMsvAttachmentManager& targetAttaMan = iTargetStore->AttachmentManagerL();
        CMsvAttachment* targetAtta = targetAttaMan.GetAttachmentInfoL( iNewRoot );
        CleanupStack::PushL( targetAtta );
        iNewMmsHeaders->SetMessageRoot( targetAtta->Id() );
        CleanupStack::PopAndDestroy( targetAtta );
        }
    else
        {
        iNewMmsHeaders->SetRootContentIdL( TPtrC8() );
        }
        

    // Check that sufficient disk space available
    // for MMS headers stream
    RFs fs = iMsvSession.FileSession();
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( &fs, 
        iNewMmsHeaders->Size(),
        iMsvSession.CurrentDriveL() ) )
        {
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }

    // Externalize MMS headers
    iNewMmsHeaders->StoreL( *iTargetStore );
    // This is the last stage in creating the new entry.
    // Now the store is committed and deleted.
    iTargetStore->CommitL();
    delete iTargetStore;
    iTargetStore = NULL;

    // Free the memory
    iMmsHeaderSize = iNewMmsHeaders->Size();
    delete iNewMmsHeaders;
    iNewMmsHeaders = NULL;
    delete iDescription;
    iDescription = NULL;

    RequestComplete( KErrNone );

    }

// ---------------------------------------------------------
// CMmsMessageOperation::CopyAttachmentsL
//
// ---------------------------------------------------------
void CMmsMessageOperation::CopyAttachmentsL()
    {
    
    // Attachments must be copied one by one because of the asynchronous
    // nature of the attachment manager functions.
    
    // We get here only if the message has attachments and the part list
    // specifies that the attachments must be copied.

    // Get next attachment
    
    MMsvAttachmentManager& sourceAttaMan = iSourceStore->AttachmentManagerL();
    CMsvAttachment* sourceAtta = sourceAttaMan.GetAttachmentInfoL( iCurrentAttachment );
    CleanupStack::PushL( sourceAtta );
    CMsvMimeHeaders* mime = CMsvMimeHeaders::NewL();
    CleanupStack::PushL( mime );
    mime->RestoreL( *sourceAtta );
    
    delete iMsvOperation;
    iMsvOperation = NULL;
    
    TInt size = 0;
    // We need to check disk space for mime headers and file attachments
    // Linked attachments are not copied, and do not take up space
    
    size += mime->Size(); // size of mime headers
    if ( sourceAtta->Type() == CMsvAttachment::EMsvFile )
        {
        // Add file size only if the attachment file must be copied.
        // Linked attachment files are not copied, just the link is added
        size += sourceAtta->Size();
        }
    
    RFs fs = iMsvSession.FileSession();
    
    if ( TMmsGenUtils::DiskSpaceBelowCriticalLevelL( &fs, 
        size,
        iMsvSession.CurrentDriveL() ) )
        {
        delete iTargetStore;
        iTargetStore = NULL;
        delete iTargetEntry;
        iTargetEntry = NULL;
        CleanupStack::PopAndDestroy( mime );
        CleanupStack::PopAndDestroy( sourceAtta );
        // we use standard error code here
        User::Leave( KErrDiskFull );
        }
    
    // Copy the attachment
    
    MMsvAttachmentManager& targetAttaMan = iTargetStore->AttachmentManagerL();
    CMsvAttachment* targetAtta = NULL;
    
    HBufC8* mimeType = sourceAtta->MimeType().AllocL();
    CleanupStack::PushL( mimeType );
    TParse fParse;
    User::LeaveIfError( fParse.Set( sourceAtta->FilePath(), NULL, NULL ) );
    HBufC* attaName = fParse.NameAndExt().AllocL();
    CleanupStack::PushL( attaName );
    
    targetAtta = CMsvAttachment::NewL(
         sourceAtta->Type(),
         sourceAtta->Size(),
         mimeType,
         attaName
         );
    CleanupStack::Pop( attaName ); // ownership of attaName transferred to targetAtta
    CleanupStack::Pop( mimeType ); // ownership of mimeType transferred to targetAtta
         
    CleanupStack::PushL( targetAtta );
    
    mime->StoreL( *targetAtta );
        
    if ( sourceAtta->Type() == CMsvAttachment::EMsvFile )
        {
        RFile sourceFile = sourceAttaMan.GetAttachmentFileL( iCurrentAttachment );
        CleanupClosePushL( sourceFile );
        targetAttaMan.AddAttachmentL( sourceFile, 
                                      targetAtta, 
                                      iStatus );
        CleanupStack::Pop( &sourceFile ); // close file
        }
    else // linked file
        {
        targetAttaMan.AddLinkedAttachmentL( sourceAtta->FilePath(), targetAtta, iStatus );
        }
        
    if ( sourceAtta->Id() == iOriginalRoot )
        {
        iNewRoot = iCurrentAttachment; // save the index
        }
    
    // Ownership of targetAtta was transferred to attachment manager
    CleanupStack::Pop( targetAtta ); // targetAtta
    CleanupStack::PopAndDestroy( mime );
    CleanupStack::PopAndDestroy( sourceAtta );

// RunL sets us active
    }

// ---------------------------------------------------------
// CMmsMessageOperation::CompleteMessageL
//
// ---------------------------------------------------------
void CMmsMessageOperation::CompleteMessageL()
    {
// As the attachments are not separate entries, iTargetEntry always points to
// the new message entry. No need to set iTargetEntry to the new entry anymore.
    TMsvEntry entry = iTargetEntry->Entry();
    AttachmentsSizeL();
    // iTargetStore is recreated in AttachmentSizeL(), we must kill it
    // to prevent a crash if the user cancels the operation at the last minute.
    // The actual problem is in Symbian attachment manager code, DoCancel() 
    // should not do anything if the active object is not active (Symbian code does
    // not check the state).
    delete iTargetStore;
    iTargetStore = NULL;
    entry.iSize = ( iAttachmentsSize + iMmsHeaderSize );
    entry.SetVisible( iVisible );
    entry.SetInPreparation( iInPreparation );
    entry.SetAttachment( iAttachmentsSize ? ETrue : EFalse );
    
    iTargetEntry->ChangeL( entry );

    RequestComplete( KErrNone );

    }

// ---------------------------------------------------------
// CMmsMessageOperation::DoCancel
//
// ---------------------------------------------------------
void CMmsMessageOperation::DoCancel()
    {

    // Cancel all the pending requests
    if ( iMsvOperation )
        {
        iMsvOperation->Cancel();
        }
        
    if ( iTargetStore )
        {
        // cancel attachment manager operations if any are active
        TRAP_IGNORE( 
            {
            // This will leave only if there is no attachment manager
            // Otherwise it will just return reference to the existing manager
            MMsvAttachmentManager& attaMan = iTargetStore->AttachmentManagerL();
            // If there is no attachment manager, there cannot be any ongoing request
            // So the request needs to be cancelled only if the previous function
            // did not leave
            attaMan.CancelRequest();
            });
        delete iTargetStore;
        iTargetStore = NULL;
        }
        
    delete iTargetEntry;
    iTargetEntry = NULL;
    delete iSourceStore;
    iSourceStore = NULL;
    delete iSourceEntry;
    iSourceEntry = NULL;
                
    // Remove the new message entry
    if ( iNewMessageId != KMsvNullIndexEntryId )
        {
        iMsvSession.RemoveEntry( iNewMessageId );
        }
    iNewMessageId = KMsvNullIndexEntryId;

    TRequestStatus* st = &iObserverRequestStatus;
    User::RequestComplete( st, KErrCancel );
    }

// ---------------------------------------------------------
// CMmsMessageOperation::AttachmentsSizeL
//
// ---------------------------------------------------------
void CMmsMessageOperation::AttachmentsSizeL()
    {
    // We just scan the attachment entries and
    // and calculate the total amount of size.

    iAttachmentsSize = 0;
    
    if ( !iTargetStore )
        {
        iTargetStore = iTargetEntry->ReadStoreL();
        }
    MMsvAttachmentManager& targetAttaMan = iTargetStore->AttachmentManagerL();
    
    TInt counter = targetAttaMan.AttachmentCount();
    
    for ( TInt i = 0; i < counter; i++ )
        {
        CMsvAttachment* attaInfo = targetAttaMan.GetAttachmentInfoL( i );
        CleanupStack::PushL( attaInfo );
        iAttachmentsSize += attaInfo->Size();
        CMsvMimeHeaders* mime = CMsvMimeHeaders::NewL();
        CleanupStack::PushL( mime );
        mime->RestoreL( *attaInfo );
        iAttachmentsSize += mime->Size();
        CleanupStack::PopAndDestroy( mime );
        CleanupStack::PopAndDestroy( attaInfo );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

