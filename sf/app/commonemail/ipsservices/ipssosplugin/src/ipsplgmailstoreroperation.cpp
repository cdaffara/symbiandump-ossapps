/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include "emailtrace.h"
#include "ipsplgheaders.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation* CIpsPlgMailStorerOperation::NewL(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMailStorerOperation* self =
        new (ELeave)CIpsPlgMailStorerOperation(
                aMsvSession,
                aObserverRequestStatus,
                aPlugin,
                aMessages,
                aFSOperationObserver,
                aRequestId);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation* CIpsPlgMailStorerOperation::NewLC(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMailStorerOperation* self = CIpsPlgMailStorerOperation::NewL(
        aMsvSession,
        aObserverRequestStatus,
        aPlugin,
        aMessages,
        aFSOperationObserver,
        aRequestId);
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation::~CIpsPlgMailStorerOperation()
    {
    FUNC_LOG;
    Cancel();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::DoCancel()
    {
    // <qmail>
    TRequestStatus* status = &iObserverRequestStatus;
    if ( status && status->Int() == KRequestPending )
        {
        if (&iFSOperationObserver)
            {
            iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestCancelled;
            iFSProgress.iError = KErrCancel;
            iFSProgress.iParam = NULL;

            TRAP_IGNORE( iFSOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
            }
        User::RequestComplete( status, iStatus.Int() );
        }
    // </qmail>
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
const TDesC8& CIpsPlgMailStorerOperation::ProgressL()
    {
    return KNullDesC8;
    }

// <qmail>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgMailStorerOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgMailStorerOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    
    return iFSProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgMailStorerOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeMailStorerOp;
    }
// </qmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMailStorerOperation::CIpsPlgMailStorerOperation(
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    CFSMailPlugin& aPlugin,
    RPointerArray<CFSMailMessage> &aMessages,
    MFSMailRequestObserver& aFSOperationObserver,
    const TInt aRequestId):
// <qmail>
    CIpsPlgBaseOperation(aMsvSession, aObserverRequestStatus, aRequestId, TFSMailMsgId()), 
    iPlugin(aPlugin),
    iMessages(aMessages),
    iFSOperationObserver(aFSOperationObserver)
// </qmail>
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::ConstructL()
    {
    FUNC_LOG;
    iError = KErrNone;
    iExecutionIndex = 0;

    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMailStorerOperation::RunL()
    {
    FUNC_LOG;
    if( iMessages.Count() &&
        iExecutionIndex < iMessages.Count() )
        {
        TInt error=KErrNone;
        CFSMailMessage *message = iMessages[0];

        TRequestStatus* status;

        if(message)
            {
            TFSMailMsgId id = message->GetMessageId();
            iPlugin.StoreMessageL( id, *message );

            iExecutionIndex++;

            status = &iStatus;
            }
        else
            {
            error = KErrNotFound;
            status = &iObserverRequestStatus;
            }
        iStatus = KRequestPending;
        SetActive();
        User::RequestComplete(status, error);
        }
    else
        {
        if( &iFSOperationObserver )
            {
            // <qmail>
            iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
            iFSProgress.iError = KErrNone;
            iFSProgress.iParam = NULL;

            TRAP_IGNORE( iFSOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
            // </qmail>
            }

        TRequestStatus* status = &iObserverRequestStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgMailStorerOperation::RunError()
    {
    FUNC_LOG;
    return KErrNone;
    }
