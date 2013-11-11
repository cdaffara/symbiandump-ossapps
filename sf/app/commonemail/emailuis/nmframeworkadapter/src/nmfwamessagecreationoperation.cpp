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
    \class NmFwaMessageCreationOperation
    
    \brief NmFwaMessageCreationOperation is an async operation which creates a new message.
    
    NmFwaMessageCreationOperation is an async operation which creates a new message.
    \sa NmOperation
 */

/*!
    Constructor
    
    \param mailboxId Id of mailbox the message is to be created with.
    \param mailClient Reference to mail client object.
 */
NmFwaMessageCreationOperation::NmFwaMessageCreationOperation(
    const NmId &mailboxId,
    CFSMailClient &mailClient) :
        mMailboxId(mailboxId),
        mMessage(NULL),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError)
{
    NM_FUNCTION;
}

/*!
    Destructor
 */
NmFwaMessageCreationOperation::~NmFwaMessageCreationOperation()
{
    NM_FUNCTION;
    
    doCancelOperation();
    delete mMessage;
}

/*!
    Called after base object construction via timer event, runs the async operation.
    
    \sa NmOperation
 */
void NmFwaMessageCreationOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    const TFSMailMsgId mailMsgId(mMailboxId.pluginId32(), mMailboxId.id32());

    CFSMailBox *mailBox = NULL;
    TRAP_IGNORE(mailBox = mMailClient.GetMailBoxByUidL(mailMsgId));

    // Call the asynchronous version first. If it returns KErrNotSupported
	// then use the synchronous version. Thus a protocol plugin does not need
	// implement the asynchronous version if there is no hit to UI performance
	// or overall robustness when using the synchronous version.
	if (mailBox) {
        TRAPD(err, mRequestId = mailBox->CreateMessageToSendL(*this));

        if (err == KErrFSMailPluginNotSupported) {
            CFSMailMessage *fsMessage = mailBox->CreateMessageToSend();

            if (fsMessage)
                {
                mMessage = fsMessage->GetNmMessage();

                delete fsMessage;
                fsMessage = NULL;
                completeOperation(NmNoError);
                }
            else
                {
                completeOperation(NmNotFoundError);
                }
            }
        else if (err != KErrNone)
            {
            completeOperation(NmGeneralError);
        }
        
        delete mailBox;
        mailBox = NULL;
    }
    else {
        completeOperation(NmNotFoundError);
    }
}

/*!
 * Complete the operation
 */
void NmFwaMessageCreationOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

/*!
    Cancels the async operation. \sa NmOperation
 */
void NmFwaMessageCreationOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}

/*!
    Returns the message created by the operation, or null if the operation has not
    completed, ownership is transferred to caller and following calls will return null.
    
    \return NmMessage* The new message.
 */
NmMessage *NmFwaMessageCreationOperation::getMessage()
{
    NM_FUNCTION;
    
    // Ownership changes
    NmMessage *ret = mMessage;
    mMessage = NULL;
    return ret;
}

/*!
    Returns the id of the message created by the operation, or NmId() if operation has not
	completed or ownership has already been transferred through getMessage().
    
    \return NmId Id of the new message.
 */
NmId NmFwaMessageCreationOperation::getMessageId()
{
    NM_FUNCTION;
    
    NmId messageId;
    
    if (mMessage) {
        messageId = mMessage->envelope().messageId();
    }
    
    return messageId;
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaMessageCreationOperation::RequestResponseL(TFSProgress aEvent,
                                                     TInt aRequestId)
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete
            && aEvent.iParam) {

            CFSMailMessage *fsMessage =
                static_cast<CFSMailMessage *>(aEvent.iParam);

            mMessage = fsMessage->GetNmMessage();
            
            delete fsMessage;
            fsMessage = NULL;
            
            completeOperation(NmNoError);
        }
        else if (status == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}

