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

#include <utf.h>

#include "emailtrace.h"
#include "ipsplgheaders.h"

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation* CIpsPlgMessagePartStorerOperation::NewL(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMessagePartStorerOperation* self =
            new (ELeave) CIpsPlgMessagePartStorerOperation(aMsvSession,
                    aObserverRequestStatus, aPlugin, aMessageParts,
                    aFSOperationObserver, aRequestId);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation* CIpsPlgMessagePartStorerOperation::NewLC(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId)
    {
    FUNC_LOG;
    CIpsPlgMessagePartStorerOperation* self =
            CIpsPlgMessagePartStorerOperation::NewL(aMsvSession,
                    aObserverRequestStatus, aPlugin, aMessageParts,
                    aFSOperationObserver, aRequestId);
    CleanupStack::PushL(self);
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation::~CIpsPlgMessagePartStorerOperation()
	{
	FUNC_LOG;
	// Cancel outstanding request, clear data buffers
	Cancel();
    iMessageParts.ResetAndDestroy();
    iMessageParts.Close();
	if (iDataBuffer)
		{
		delete iDataBuffer;
		iDataBuffer = NULL;
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::DoCancel()
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
const TDesC8& CIpsPlgMessagePartStorerOperation::ProgressL()
    {
    FUNC_LOG;
    return KNullDesC8;
    }

// <qmail>
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
const TDesC8& CIpsPlgMessagePartStorerOperation::GetErrorProgressL( TInt /*aError*/ )
    {
    FUNC_LOG;
    
    return KNullDesC8; // error progress info not supported
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//   
TFSProgress CIpsPlgMessagePartStorerOperation::GetFSProgressL() const
    {
    FUNC_LOG;
    
    return iFSProgress;
    }

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------    
TIpsOpType CIpsPlgMessagePartStorerOperation::IpsOpType() const
    {
    FUNC_LOG;
    return EIpsOpTypeMessagePartStorerOp;
    }
// </qmail>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
CIpsPlgMessagePartStorerOperation::CIpsPlgMessagePartStorerOperation(
        CMsvSession& aMsvSession, TRequestStatus& aObserverRequestStatus,
        CFSMailPlugin& aPlugin,
        RPointerArray<CFSMailMessagePart> &aMessageParts,
        MFSMailRequestObserver& aFSOperationObserver, const TInt aRequestId) :
    CIpsPlgBaseOperation(aMsvSession, aObserverRequestStatus, aRequestId, TFSMailMsgId()), 
    iPlugin(aPlugin), 
    iMessageParts(aMessageParts), 
    iFSOperationObserver(aFSOperationObserver),
    iExecutionIndex(0), 
    iDataBuffer(NULL)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::ConstructL()
    {
    FUNC_LOG;
    CActiveScheduler::Add(this);
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::RunL()
	{
	FUNC_LOG;
	
	// Clear data buffer if it was used in last request
	if (iDataBuffer)
		{
		delete iDataBuffer;
		iDataBuffer = NULL;
		}
	
	if (!StoreNextPartL())
		{
		if (&iFSOperationObserver)
			{
		    // <qmail>
		    iFSProgress.iProgressStatus = TFSProgress::EFSStatus_RequestComplete;
	        iFSProgress.iError = KErrNone;
	        iFSProgress.iParam = NULL;

			TRAP_IGNORE( iFSOperationObserver.RequestResponseL( iFSProgress, iFSRequestId ) );
			// </qmail>
			}
		TRequestStatus* status = &iObserverRequestStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TInt CIpsPlgMessagePartStorerOperation::RunError(TInt aError)
    {
    FUNC_LOG;
    TInt error = aError;
    switch (aError)
        {
        // Handling RunL errors can be done here
        default:
            error = KErrNone;
        }
    return error;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
TBool CIpsPlgMessagePartStorerOperation::StoreNextPartL()
	{
	TBool partFound = EFalse;
	TRequestStatus* status;
	TInt partsCount = iMessageParts.Count();
	if (partsCount && iExecutionIndex < partsCount)
		{
		partFound = ETrue;
		CFSMailMessagePart* part = iMessageParts[iExecutionIndex];

		// Change execution index to point next part
		iExecutionIndex++;

		// Check if part was found
		if (part)
			{
			if (part->GetContentType().Compare(KFSMailContentTypeTextPlain) == 0)
				{
				StoreTextPlainPartL(part);
				}
			else if (part->GetContentType().Compare(KFSMailContentTypeTextHtml) == 0)
				{
				StoreTextHtmlPartL(part);
				}
			}
		else
			{
			status = &iObserverRequestStatus;
			iStatus = KRequestPending;
			SetActive();
			User::RequestComplete(status, KErrNotFound);
			}
		}
	return partFound;
	}
	


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::StoreTextPlainPartL(
		CFSMailMessagePart* aPart)
	{
	User::LeaveIfNull(aPart);
	TRequestStatus* status;
	
	// Text buffer for plain text content - synchronous function
	HBufC* contentBuffer = aPart->GetLocalTextContentLC();
	iPlugin.SetContentL(*contentBuffer, aPart->GetMailBoxId(),
			aPart->GetFolderId(), aPart->GetMessageId(), aPart->GetPartId());
	CleanupStack::PopAndDestroy(contentBuffer);

	// Request should be completed immediately, because sync function was used
	status = &iStatus;
	iStatus = KRequestPending;
	SetActive();
	User::RequestComplete(status, KErrNone);
	}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
void CIpsPlgMessagePartStorerOperation::StoreTextHtmlPartL(
		CFSMailMessagePart* aPart)
	{
	User::LeaveIfNull(aPart);
	
	// Text buffer for html text content
	HBufC* data16 = aPart->GetLocalTextContentLC();

	// Convert from 16 to 8 bit data - data must exist until request is completed
	iDataBuffer = HBufC8::NewL((data16->Length() * 2) + 1);
	TPtr8 ptr8(iDataBuffer->Des());
	CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr8, *data16);

	// Get text/html part file for write
	RFile file = aPart->GetContentFileL(ETrue);
	CleanupClosePushL(file);

    // if we don't do SetSize(0) characters from the original mail are left in the end of the mail
	// if the modified mail contains less characters.
	User::LeaveIfError( file.SetSize( 0 ) );

	// Write new content to text/html part file - async function
	file.Write( 0, *iDataBuffer, iDataBuffer->Length(), iStatus );
	
	CleanupStack::PopAndDestroy(2, data16);
	SetActive();
	}
