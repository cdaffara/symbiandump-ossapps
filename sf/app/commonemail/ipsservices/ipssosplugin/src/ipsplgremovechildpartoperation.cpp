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
* Description: This file implements class CCIpsPlgRemoveChildPartOperation.
*
*/

// <qmail>
// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// LOCAL CONSTANTS AND MACROS

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::CIpsPlgRemoveChildPartOperation
// ----------------------------------------------------------------------------
//
CIpsPlgRemoveChildPartOperation::CIpsPlgRemoveChildPartOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aPartId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    :
    CIpsPlgBaseOperation( 
        aMsvSession, 
        aObserverRequestStatus,
        aRequestId,
        TFSMailMsgId()),
    iMessageId(aMessageId),
    iPartId(aPartId),
    iOperationObserver(aOperationObserver)
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::NewL
// ----------------------------------------------------------------------------
//
CIpsPlgRemoveChildPartOperation* CIpsPlgRemoveChildPartOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    const TFSMailMsgId& aMessageId,
    const TFSMailMsgId& aPartId,
    MFSMailRequestObserver& aOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgRemoveChildPartOperation* self =
        new (ELeave) CIpsPlgRemoveChildPartOperation(
            aMsvSession,
            aObserverRequestStatus,
            aMessageId,
            aPartId,
            aOperationObserver,
            aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::ConstructL
// ----------------------------------------------------------------------------
//
void CIpsPlgRemoveChildPartOperation::ConstructL()
    {
    FUNC_LOG;
    StartOperationL();
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::~CIpsPlgRemoveChildPartOperation
// ----------------------------------------------------------------------------
//
CIpsPlgRemoveChildPartOperation::~CIpsPlgRemoveChildPartOperation()
    {
    FUNC_LOG;
    Cancel(); // Cancel any request, if outstanding
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::DoCancel
// ----------------------------------------------------------------------------
//
void CIpsPlgRemoveChildPartOperation::DoCancel()
    {
    FUNC_LOG;
    
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
// CIpsPlgRemoveChildPartOperation::StartOperation
// ----------------------------------------------------------------------------
//
void CIpsPlgRemoveChildPartOperation::StartOperationL()
    {
    TInt status( KErrNone );
    CMsvEntry* cEntry( NULL );
    TMsvEntry tEntry;
    TMsvId serviceId;
    status = iMsvSession.GetEntry( iPartId.Id(), serviceId, tEntry );
    
    if ( ( status == KErrNone ) && 
         ( tEntry.iType == KUidMsvAttachmentEntry ) )
        {
        iMessage = NULL;
        // We trust that the message ID really refers to a message
        GetMessageEntryL( iMessageId.Id(), cEntry, iMessage );
    
        MMsvAttachmentManager& attachmentMgr( iMessage->AttachmentManager() ); 

        // Start async request
        attachmentMgr.RemoveAttachmentL( 
            (TMsvAttachmentId) iPartId.Id(), iStatus );
        SetActive();
        }
    else if ( ( status == KErrNone ) && 
              ( tEntry.iType == KUidMsvFolderEntry ) )
        {
        cEntry = iMsvSession.GetEntryL( tEntry.Parent() );
        CleanupStack::PushL( cEntry );
        cEntry->DeleteL( tEntry.Id() );
        CleanupStack::PopAndDestroy( cEntry );
        }
    }
// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgRemoveChildPartOperation::RunL()
    {
    if (iStatus.Int() == KErrNone)
        {
        // Inform observer
        SignalFSObserver(iStatus.Int(),NULL);
        // Complete observer
        TRequestStatus* observerStatus = &iObserverRequestStatus;
        User::RequestComplete( observerStatus, iStatus.Int() );   
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
// CIpsPlgRemoveChildPartOperation::GetMessageEntryL( )
// Checks whether the requested message is already cached. If not, the cached 
// objects are deleted and new objects are created.
// ----------------------------------------------------------------------------
void CIpsPlgRemoveChildPartOperation::GetMessageEntryL( 
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
void CIpsPlgRemoveChildPartOperation::CleanCachedMessageEntries()
    {
    FUNC_LOG;
    delete iCachedEmailMessage;
    iCachedEmailMessage = NULL;
    delete iCachedEntry;
    iCachedEntry = NULL;
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::RunError
// ----------------------------------------------------------------------------
//
TInt CIpsPlgRemoveChildPartOperation::RunError(TInt aError)
    {
    FUNC_LOG;
    SignalFSObserver( aError, NULL );
    
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, aError );
    return KErrNone; // RunError must return KErrNone to active sheduler.
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::SignalFSObserver
// ----------------------------------------------------------------------------
//
void CIpsPlgRemoveChildPartOperation::SignalFSObserver(
        TInt aStatus, CFSMailMessagePart* /*aMessagePart*/ )
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
        iFSProgress.iParam = NULL;
        }

    TRAP_IGNORE( iOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
    }

// ----------------------------------------------------------------------------
// CIpsPlgRemoveChildPartOperation::ProgressL
// ----------------------------------------------------------------------------
//
const TDesC8& CIpsPlgRemoveChildPartOperation::ProgressL()
    {
    FUNC_LOG;
    return KNullDesC8;
    }

// ---------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::GetErrorProgressL
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgRemoveChildPartOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::GetFSProgressL
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgRemoveChildPartOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    return iFSProgress;
    }

// ----------------------------------------------------------------------------
// CIpsPlgNewChildPartFromFileOperation::IpsOpType
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgRemoveChildPartOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeNewChildPartFromFile;
    }

//  End of File

// </qmail>
