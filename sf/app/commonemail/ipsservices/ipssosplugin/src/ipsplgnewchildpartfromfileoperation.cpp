/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file implements class CCIpsPlgNewChildPartFromFileOperation.
*
*/

// <qmail>
// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

_LIT( KMimeTextCalRequest,  "text/calendar; method=REQUEST;" );
_LIT( KMimeTextCalResponse, "text/calendar; method=RESPONSE;" );
_LIT( KMimeTextCalCancel,   "text/calendar; method=CANCEL;" );
_LIT( KFileExtensionICS, ".ics" );

_LIT8( KMethod, "method" );
_LIT8( KRequest, "REQUEST" );
_LIT8( KResponse, "RESPONSE" );
_LIT8( KCancel, "CANCEL" );

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::CIpsPlgNewChildPartFromFileOperation
// ----------------------------------------------------------------------------
//
CIpsPlgNewChildPartFromFileOperation::CIpsPlgNewChildPartFromFileOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :
    CIpsPlgBaseOperation( 
        aMsvSession, 
        aObserverRequestStatus,
        aRequestId,
        aMailBoxId),
    iMessageId(aMessageId),
    iOperationObserver(aOperationObserver)
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgNewChildPartFromFileOperation* CIpsPlgNewChildPartFromFileOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMailBoxId,
    const TFSMailMsgId& aMessageId,
    const TDesC& aContentType,
    const TDesC& aFilePath,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId,
    CIpsPlgMsgMapper *aMsgMapper)
    {
    FUNC_LOG;
    CIpsPlgNewChildPartFromFileOperation* self =
        new (ELeave) CIpsPlgNewChildPartFromFileOperation(
            aMsvSession,
            aObserverRequestStatus,
            aMailBoxId,
            aMessageId,
            aOperationObserver,
            aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL(aMsgMapper,aContentType,aFilePath);
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::ConstructL(CIpsPlgMsgMapper *aMsgMapper,
    const TDesC& aContentType,
    const TDesC& aFilePath)
    {
    FUNC_LOG;
    iMsgMapper = aMsgMapper;
    iContentType = aContentType.AllocL();
    iFilePath = aFilePath.AllocL();

    // Start from attachment manager initialization
    InitAttachmentManagerL();
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::~CIpsPlgNewChildPartFromFileOperation
// ----------------------------------------------------------------------------
//
CIpsPlgNewChildPartFromFileOperation::~CIpsPlgNewChildPartFromFileOperation()
    {
    FUNC_LOG;
    Cancel(); // Cancel any request, if outstanding
    delete iOperation;
    iOperation = NULL;
    delete iContentType;
    iContentType = NULL;
    delete iFilePath;
    iFilePath = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::DoCancel
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::DoCancel()
    {
    FUNC_LOG;

    if (iOperation)
        {
        iOperation->Cancel();
        }

    if (iMessage)
        {
        iMessage->AttachmentManager().CancelRequest();
        iMessage->Cancel();
        }

    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        SignalFSObserver(iStatus.Int(),NULL);
        User::RequestComplete( status, iStatus.Int() );
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgSosBasePlugin::GetMessageEntryL( )
// Checks whether the requested message is already cached. If not, the cached 
// objects are deleted and new objects are created.
// ----------------------------------------------------------------------------
void CIpsPlgNewChildPartFromFileOperation::GetMessageEntryL( 
    TMsvId aId, 
    CMsvEntry*& aMessageEntry,
    CImEmailMessage*& aImEmailMessage )
    {
    FUNC_LOG;
    if ( !iCachedEntry || ( aId != iCachedEntry->Entry().Id() ) ||
            iCachedEmailMessage->IsActive() )
        {
        CleanCachedMessageEntries();
        
        iCachedEntry = iMsvSession.GetEntryL( aId );
        if ( iCachedEntry->Entry().iType == KUidMsvMessageEntry )
            {
            iCachedEmailMessage = CImEmailMessage::NewL( *iCachedEntry );
            }
        }
    aMessageEntry = iCachedEntry;
    aImEmailMessage = iCachedEmailMessage;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::CleanCachedMessageEntries()
    {
    FUNC_LOG;
    delete iCachedEmailMessage;
    iCachedEmailMessage = NULL;
    delete iCachedEntry;
    iCachedEntry = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
            // Divided to 3 steps. Run asunchronously after InitAttachmentManagerL.
            // PrepareMsvEntryL, PrepareStoreL and StoreMessagePartL respectively
            switch (iStep)
            {
            case EPrepareMsvEntry:
                PrepareMsvEntryL();
                break;
            case EPrepareStore:
                PrepareStoreL();
                break;
            case EStoreMessagePart:
                StoreMessagePartL();
                break;
            }
        }
    else if (iStatus.Int() == KErrCancel) 
        {
        // Do Nothing
        }
    else
        {
        User::Leave(iStatus.Int());
        }
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::InitAttachmentManagerL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::InitAttachmentManagerL()
    {
    iEntry = NULL;
    iMessage = NULL;
    RFile file;
    TInt fileSize( 0 );

    // Read attachment size
    User::LeaveIfError(
        file.Open( iMsvSession.FileSession(), iFilePath->Des(), EFileShareReadersOnly )
        );
 
    //in rare case that file has disappeared while sending
    //we just won't get the size for it
    file.Size( fileSize );
    file.Close();    

    // Initialize CMsvAttachment instance for the attachment creation
    CMsvAttachment* info = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
    CleanupStack::PushL( info );

    info->SetAttachmentNameL( iFilePath->Des() );
    info->SetSize( fileSize );

    // Create/acquire Symbian message entry objects
    GetMessageEntryL( iMessageId.Id(), iEntry, iMessage );

    // Start attachment creation
    iMessage->AttachmentManager().AddAttachmentL( 
        iFilePath->Des(), info, iStatus );
    CleanupStack::Pop( info ); // attachment manager takes ownership
    iStep = EPrepareMsvEntry; // Next step
    SetActive();
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::PrepareMsvEntryL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::PrepareMsvEntryL()
    {
    // Dig out the entry ID of the new attachment
    iMessage->GetAttachmentsListL( iEntry->Entry().Id( ), 
        CImEmailMessage::EAllAttachments, CImEmailMessage::EThisMessageOnly );
    TKeyArrayFix key( 0, ECmpTInt32 );
    CMsvEntrySelection* attachmentIds = iMessage->Selection().CopyLC();
    attachmentIds->Sort( key );
    if ( !attachmentIds->Count() )
        {
        User::Leave( KErrGeneral );
        }
    iNewAttachmentId = (*attachmentIds)[ attachmentIds->Count()-1 ];
    CleanupStack::PopAndDestroy( attachmentIds );
    
    CMsvEntry* cAtta = iMsvSession.GetEntryL( iNewAttachmentId );
    CleanupStack::PushL( cAtta );
    
    // Set filename to iDetails
    TMsvEntry tEntry = cAtta->Entry();
    tEntry.iDetails.Set(iFilePath->Des());

    // Do async
    iOperation = cAtta->ChangeL( tEntry, iStatus );
    CleanupStack::PopAndDestroy( cAtta );
    iStep = EPrepareStore; // Next step
    SetActive();
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::PrepareStoreL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::PrepareStoreL()
    {
    CMsvEntry* cAtta = iMsvSession.GetEntryL( iNewAttachmentId );
    CleanupStack::PushL( cAtta );
    TBool parentToMultipartAlternative( EFalse );
    if( cAtta->HasStoreL() )
        {
        CMsvStore* store = cAtta->EditStoreL();
        CleanupStack::PushL( store );
        CImMimeHeader* mimeHeader = CImMimeHeader::NewLC();
        
        if( store->IsPresentL( KUidMsgFileMimeHeader ) )
            {
            mimeHeader->RestoreL( *store );
            CDesC8Array& array = mimeHeader->ContentTypeParams();
            array.AppendL( KMethod );
            parentToMultipartAlternative = ETrue;
            
            if( iContentType->Des().Find( KMimeTextCalRequest ) != KErrNotFound )
                {    
                array.AppendL( KRequest );
                }
            else if( iContentType->Des().Find( KMimeTextCalResponse ) != KErrNotFound )
                {
                array.AppendL( KResponse );
                }
            else if( iContentType->Des().Find( KMimeTextCalCancel ) != KErrNotFound ) 
                {
                array.AppendL( KCancel );
                }
            else
                {
                parentToMultipartAlternative = EFalse;
                }
            mimeHeader->StoreWithoutCommitL( *store );
            store->CommitL();
            }
        
        CleanupStack::PopAndDestroy( 2, store );
        }

    if( parentToMultipartAlternative &&
        iFilePath->Find( KFileExtensionICS ) != KErrNotFound )
        {        
        TMsvEntry tAttaEntry = cAtta->Entry();
        TMsvId id = tAttaEntry.Parent();
        CMsvEntry* cParent = iMsvSession.GetEntryL( id );
        CleanupStack::PushL( cParent );
        
        TMsvEmailEntry tEntry = cParent->Entry();
        tEntry.SetMessageFolderType( EFolderTypeAlternative );
        
        // Do async again if needed

        iOperation = cParent->ChangeL( tEntry, iStatus );
        CleanupStack::PopAndDestroy( cParent );
        CleanupStack::PopAndDestroy( cAtta );
        }
    else
        {
        CleanupStack::PopAndDestroy( cAtta );
        iStatus = KRequestPending;
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status,KErrNone);
        }
    iStep = EStoreMessagePart; // Next step
    SetActive();
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::StoreMessagePartL
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::StoreMessagePartL()
    {
    // Delete the message entries to get all the changes to disk and 
    // possible store locks released
    CleanCachedMessageEntries();
    
    CFSMailMessagePart* result ( NULL );
    // Create the FS message part object
    result = iMsgMapper->GetMessagePartL( iNewAttachmentId, iFSMailboxId, 
        iMessageId );

    // Set attachment name
    result->SetAttachmentNameL(iFilePath->Des());

    // store message part
    result->SaveL();
    
    // set flag
    result->SetFlag(EFSMsgFlag_Attachments);
    
    SignalFSObserver(iStatus.Int(),result);
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::RunError
// ----------------------------------------------------------------------------
//
TInt CIpsPlgNewChildPartFromFileOperation::RunError(TInt aError)
    {
    FUNC_LOG;
    SignalFSObserver( aError, NULL );
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    return KErrNone; // RunError must return KErrNone to active sheduler.
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::SignalFSObserver
// ----------------------------------------------------------------------------
//
void CIpsPlgNewChildPartFromFileOperation::SignalFSObserver(
        TInt aStatus, CFSMailMessagePart* aMessagePart )
    {
    FUNC_LOG;
    if ( aStatus == KErrCancel )
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
        iFSProgress.iError = KErrCancel;
        iFSProgress.iParam = NULL;
        }
    else
        {
        iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
        iFSProgress.iError = aStatus;
        iFSProgress.iParam = aMessagePart;
        }

    TRAP_IGNORE( iOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::ProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgNewChildPartFromFileOperation::ProgressL()
    {
    FUNC_LOG;
    // Make sure that operation is active
    return (IsActive() && iOperation) ? 
        iOperation->ProgressL() : KNullDesC8;
    }

// ---------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::GetErrorProgressL
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgNewChildPartFromFileOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::GetFSProgressL
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgNewChildPartFromFileOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    return iFSProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::IpsOpType
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgNewChildPartFromFileOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeNewChildPartFromFile;
    }

//  End of File

// </qmail>
