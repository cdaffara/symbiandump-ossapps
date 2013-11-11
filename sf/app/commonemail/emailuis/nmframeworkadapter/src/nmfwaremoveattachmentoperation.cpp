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
#include "nmframeworkadapterheaders.h"

/*!
    \class NmFwaRemoveAttachmentOperation
    
    \brief NmFwaRemoveAttachmentOperation is an async operation which removes
           attachment from the message.
           
    NmFwaRemoveAttachmentOperation is an async operation which removes attachment from the message.
    \sa NmOperation
 */

/*!
    Constructor
    
    \param message Message where attachment to be removed.
    \param fileName File name of the removed file.
    \param mailClient Reference to mail client object.
 */
NmFwaRemoveAttachmentOperation::NmFwaRemoveAttachmentOperation(
    const NmMessage &message,
    const NmId &attachmentPartId,
    CFSMailClient &mailClient) :
        mMessage(message),
        mMailClient(mailClient)
{
    NM_FUNCTION;
    
    // Take a copy because original will be deleted during the operation
    mAttachmentPartId.setId(attachmentPartId.id());
}

/*!
    Destructor
 */
NmFwaRemoveAttachmentOperation::~NmFwaRemoveAttachmentOperation()
{
    NM_FUNCTION;
    
    doCancelOperation();
}

/*!
    Called after base object construction, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaRemoveAttachmentOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    TRAPD(err, doRunAsyncOperationL());
    if (err != KErrNone) {
        completeOperation(NmGeneralError);
    }
}

/*!
    Leaving function for async operation
    \sa NmOperation
 */
void NmFwaRemoveAttachmentOperation::doRunAsyncOperationL()
{
    NM_FUNCTION;
    
    CFSMailMessage *msg = NULL;

    msg = CFSMailMessage::NewL(mMessage);
    
    // Get attachment list from the message
    RPointerArray<CFSMailMessagePart> attachments;
    attachments.Reset();
    msg->AttachmentListL(attachments);
        
    // Search through all attachments from message and remove attachment
    // if message part match.
    bool found(false);
    for (int i=0; i<attachments.Count(); ++i) {
        if (mAttachmentPartId.id() == attachments[i]->GetPartId().GetNmId().id()) {
            mRequestId = msg->RemoveChildPartL(attachments[i]->GetPartId(),*this);
            found = true;
            break;
        }
    }
    attachments.ResetAndDestroy();   
    delete msg;
    msg = NULL;
    // if attachment is not found, request to plugin is not made
    // and the operation should be completed here
    if (!found) {
        completeOperation(NmNotFoundError);
    }
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaRemoveAttachmentOperation::RequestResponseL(TFSProgress aEvent,
                                                      TInt aRequestId)
{
    NM_FUNCTION;
    
    if (aRequestId == mRequestId) {
        TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;
        if (status == TFSProgress::EFSStatus_RequestComplete) {
            // Request completed. Let's check the result
            switch (aEvent.iError) {
                case KErrNone: {
                    completeOperation(NmNoError);
                    break;
                }
                case KErrNotFound:
                    completeOperation(NmNotFoundError);
                    break;
                default:
                    completeOperation(NmGeneralError);
            }
        }
        else if (status == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
    else {
        completeOperation(NmGeneralError);
    }
}

/*!
    Cancels the async operation. \sa NmOperation
 */
void NmFwaRemoveAttachmentOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = KErrNotFound;
    }
}

