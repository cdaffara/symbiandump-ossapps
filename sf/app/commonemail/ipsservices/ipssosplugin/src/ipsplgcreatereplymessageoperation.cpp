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
* Description: This file implements class CIpsPlgCreateReplyMessageOperation.
*
*/

// <qmail>

// INCLUDE FILES
#include "emailtrace.h"
#include "ipsplgheaders.h"

// <qmail> removed description

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::CIpsPlgCreateReplyMessageOperation
// ----------------------------------------------------------------------------
//
//<qmail> aSmtpService to reference
CIpsPlgCreateReplyMessageOperation::CIpsPlgCreateReplyMessageOperation(
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
// CIpsPlgCreateReplyMessageOperation::NewL
// ----------------------------------------------------------------------------
//
//<qmail> aSmtpService to reference
CIpsPlgCreateReplyMessageOperation* CIpsPlgCreateReplyMessageOperation::NewL(
    CIpsPlgSmtpService& aSmtpService,
    CMsvSession& aMsvSession,
    TRequestStatus& aObserverRequestStatus,
    TMsvPartList aPartList,
    TFSMailMsgId aMailBoxId,
    TMsvId aOriginalMessageId, 
    MFSMailRequestObserver& aOperationObserver,
    TInt aRequestId )
    {
    FUNC_LOG;
    CIpsPlgCreateReplyMessageOperation* self =
        new (ELeave) CIpsPlgCreateReplyMessageOperation(
            aSmtpService, 
            aMsvSession, 
            aObserverRequestStatus,
            aPartList, 
            aMailBoxId, 
            aOriginalMessageId, 
            aOperationObserver, 
            aRequestId );
    CleanupStack::PushL( self );
    self->ConstructL(); // Use base class constructor
    CleanupStack::Pop( self ); 
    return self;
    }
//</qmail>
// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::~CIpsPlgCreateReplyMessageOperation
// ----------------------------------------------------------------------------
//
CIpsPlgCreateReplyMessageOperation::~CIpsPlgCreateReplyMessageOperation()
    {
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::RunL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateReplyMessageOperation::RunL()
    {
    FUNC_LOG;

    if( iStatus.Int() == KErrNone )
        {
        CFSMailMessage* newMessage = NULL;
        
        // <qmail> removed TRAP 
        // new message creation has finished and we have an id...
        TMsvId msgId;
        msgId = GetIdFromProgressL( iOperation->FinalProgress() );
        //</qmail>
        
		//<qmail> 
        //  ...so we can create an FS type message
        newMessage = iSmtpService.CreateFSMessageAndSetFlagsL( 
                msgId, iOriginalMessageId, iFSMailboxId.Id() );
        CleanupStack::PushL( newMessage );
    
        // dig out new reply message's header
        CMsvEntry* cEntry = iMsvSession.GetEntryL( msgId );
        CleanupStack::PushL( cEntry );
        CMsvStore* store = cEntry->ReadStoreL();
        CleanupStack::PushL( store );
        if( store->IsPresentL( KUidMsgFileIMailHeader ) == EFalse )
             {
             User::Leave(KErrCorrupt);
             }
        CImHeader* header = CImHeader::NewLC();
        header->RestoreL( *store );
        
        // Start handling recipients
        HBufC* emailAddr( NULL );
        CFSMailAddress* fsAddr( NULL );
        
        // copy to recipients            
        TInt toRecipientCount( header->ToRecipients().Count() );
        for( TInt i = 0; i < toRecipientCount; i++ )
            {
            emailAddr = header->ToRecipients()[i].AllocLC();
            fsAddr = CFSMailAddress::NewLC();
            fsAddr->SetEmailAddress( *emailAddr ); // Copy created
            newMessage->AppendToRecipient( fsAddr ); // No copy
            CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
            CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
            }

        // get additional recipients (reply all case)
        if( iPartList & KMsvMessagePartRecipient )
            {
            // <qmail> code moved to own function
            RecipientsCallToOrderL( header, newMessage );
            // </qmail>
            }
                    
        CleanupStack::PopAndDestroy( 3, cEntry ); // header, store, cEntry
        CleanupStack::Pop( newMessage ); // fsMsg is given to client
        
        // save the added recipients to the message
        newMessage->SaveMessageL();
        
        // <qmail> removed bracket
    	//</qmail>
        // relay the created message (observer takes ownership)
        SignalFSObserver( iStatus.Int(), newMessage );        
        }
    
    // nothing left to process, so complete the observer
    TRequestStatus* status = &iObserverRequestStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::StartMessageCreationL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateReplyMessageOperation::StartMessageCreationL()
    {
    FUNC_LOG;
    delete iOperation;
    iOperation = NULL;
    
    // <qmail> removed useless parameter
    // Start a new operation, execution continues in RunL 
    // once the operation has finished.
    iOperation = CImEmailOperation::CreateReplyL(
            iStatus, 
            iMsvSession,
            iOriginalMessageId,
            KMsvDraftEntryId, 
            iPartList, 
            KIpsPlgReplySubjectFormat,
            KMsvEmailTypeListMHTMLMessage,
            KUidMsgTypeSMTP);
    // </qmail>
    }
//<qmail>
// ----------------------------------------------------------------------------
// CIpsPlgCreateReplyMessageOperation::RecipientsCallToOrderL
// ----------------------------------------------------------------------------
//
void CIpsPlgCreateReplyMessageOperation::RecipientsCallToOrderL( 
        CImHeader* aHeader,
        CFSMailMessage* aNewMessage
        )
    {
    // check if CC recipient read from header was present in To field
    // of original message. If so, copy it into To recipients.
    // 
    HBufC* emailAddr( NULL );
    CFSMailAddress* fsAddr( NULL );
    
    CMsvEntry* tmpEntry = iMsvSession.GetEntryL( iOriginalMessageId );
    CleanupStack::PushL( tmpEntry );
    CMsvStore* tmpStore = tmpEntry->ReadStoreL();
    CleanupStack::PushL( tmpStore );
    if( tmpStore->IsPresentL( KUidMsgFileIMailHeader ) )
        {
        CImHeader* tmpHeader = CImHeader::NewLC();
        tmpHeader->RestoreL( *tmpStore );
       
        TInt originalToRecipientsCount = tmpHeader->ToRecipients().Count(); 
        TBool present = EFalse;
        for( TInt i = 0; i < aHeader->CcRecipients().Count(); i++ )
            {
            emailAddr = aHeader->CcRecipients()[i].AllocLC();
            fsAddr = CFSMailAddress::NewLC();
            fsAddr->SetEmailAddress( *emailAddr ); // Copy created
           
            present = EFalse;
            for( TInt j = 0; j < originalToRecipientsCount; j++ )
                {
                if( emailAddr->Find( tmpHeader->ToRecipients()[j]) != KErrNotFound )
                    {
                    present = ETrue;
                    break;
                    }
                }
           
            if( present )
                {
                aNewMessage->AppendToRecipient( fsAddr ); // No copy
                }
            else
                {
                aNewMessage->AppendCCRecipient( fsAddr ); // No copy
                }
           
            CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
            CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
            }
        CleanupStack::PopAndDestroy( tmpHeader );
        }
    CleanupStack::PopAndDestroy( 2, tmpEntry );
   
    // copy bcc recipients
    for( TInt i = 0; i < aHeader->BccRecipients().Count(); i++ )
        {
        emailAddr = aHeader->BccRecipients()[i].AllocLC();
        fsAddr = CFSMailAddress::NewLC();
        fsAddr->SetEmailAddress( *emailAddr ); // Copy created
        aNewMessage->AppendBCCRecipient( fsAddr ); // No copy
        CleanupStack::Pop( fsAddr ); // fsAddr belong now to fsMsg
        CleanupStack::PopAndDestroy( emailAddr ); // emailAddr not used
        }
    }
//</qmail>
//  End of File

// </qmail>
