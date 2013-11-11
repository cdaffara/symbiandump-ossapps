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
* Description: This file implements class CIpsPlgCreateForwardMessageOperation.
*
*/

// <qmail>

// INCLUDE FILES
#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> removed comment

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::CIpsPlgCreateForwardMessageOperation
// ----------------------------------------------------------------------------
//
//<qmail> aSmtpService changed to reference
//<qmail> KMsvNullIndexEntry para removed
CIpsPlgCreateForwardMessageOperation::CIpsPlgCreateForwardMessageOperation(
    CIpsPlgSmtpService& aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    TMsvId aOriginalMessageId, 
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId ) 
    :
    CIpsPlgCreateMessageOperation(
        aSmtpService,
        aMsvSession,
        aObserverRequestStatus,
        aPartList,
        aMailBoxId,
        aOperationObserver,
        aRequestId),
    iOriginalMessageId(aOriginalMessageId)
    {
    FUNC_LOG;
    }
//</qmail>	


// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::NewL
// ----------------------------------------------------------------------------
//
//<qmail> aSmtpService changed to reference
CIpsPlgCreateForwardMessageOperation* CIpsPlgCreateForwardMessageOperation::NewL(
    CIpsPlgSmtpService& aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    TMsvId aOriginalMessageId, 
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
//</qmail>	
    {
    FUNC_LOG;
    CIpsPlgCreateForwardMessageOperation* self =
        new (ELeave) CIpsPlgCreateForwardMessageOperation(
            aSmtpService, 
            aMsvSession, 
            aObserverRequestStatus,
            aPartList, 
            aMailBoxId, 
            aOriginalMessageId, 
            aOperationObserver, 
            aRequestId );
    CleanupStack::PushL( self );
    // <qmail>
    self->ConstructL(); // Use base class constructor
    // </qmail>
    CleanupStack::Pop( self ); 
    return self;
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::~CIpsPlgCreateForwardMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateForwardMessageOperation::~CIpsPlgCreateForwardMessageOperation()
    {
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateForwardMessageOperation::RunL()
    {
    FUNC_LOG;

    if( iStatus.Int() == KErrNone )
        {
        // new message creation has finished so make an FS type message
        CFSMailMessage* newMessage = NULL;
        //<qmail> TRAP removed
        TMsvId msgId;
        
        msgId = GetIdFromProgressL( iOperation->FinalProgress() );
            
        newMessage = iSmtpService.CreateFSMessageAndSetFlagsL( 
                msgId, iOriginalMessageId, iFSMailboxId.Id(), ETrue );
        //</qmail>
        // relay the created message (observer takes ownership)
        SignalFSObserver( iStatus.Int(), newMessage );        
        }
    
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateForwardMessageOperation::StartMessageCreationL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateForwardMessageOperation::StartMessageCreationL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    //<qmail> commented parameter removed
    // Start a new operation, execution continues in RunL 
    // once the operation has finished.
    iOperation = CImEmailOperation::CreateForwardL(
            iStatus, 
            iMsvSession,
            iOriginalMessageId,
            KMsvDraftEntryId,
            iPartList, 
            KIpsPlgForwardSubjectFormat,
            KMsvEmailTypeListMHTMLMessage,
            KUidMsgTypeSMTP);
    //</qmail>
    }

//  End of File

// </qmail>
