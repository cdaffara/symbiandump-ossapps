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
    \class NmFwaMessageSendingOperation
    
    \brief NmFwaMessageSendingOperation is an async operation which creates a
           new message.
    
    NmFwaMessageSendingOperation is an async operation which creates a new
    message. \sa NmOperation
 */

/*!
    Constructor
    
    \param message pointer to the message under sent, ownership is transferred to operation
    \param mailClient Reference to mail client object.
 */
NmFwaMessageSendingOperation::NmFwaMessageSendingOperation(
    NmDataPluginInterface &pluginInterface,
    NmMessage *message,
    CFSMailClient &mailClient) :
        mPluginInterface(pluginInterface),
        mSaveOperation(NULL),
        mMessage(message),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError),
        mSaved(false)
{
    NM_FUNCTION;
    
    mMailClient.IncReferenceCount();
}

/*!
    Destructor
 */
NmFwaMessageSendingOperation::~NmFwaMessageSendingOperation()
{
    NM_FUNCTION;
    
    if (mSaveOperation && mSaveOperation->isRunning()) {
        mSaveOperation->cancelOperation();
    }
    doCancelOperation();
    mMailClient.Close(); // decrease ref count
    delete mMessage;
}

/*!
    returns pointer to message, ownership is not transferred
 */
const NmMessage *NmFwaMessageSendingOperation::getMessage() const
{
    NM_FUNCTION;
    
    return mMessage;
}

/*!
    Called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaMessageSendingOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    int err = NmNoError;
    
    if (mSaved) {
        TRAPD(trapped, err = sendMessageL());

        if (trapped == KErrNotFound) {
            err = NmNotFoundError;
        }
        else if (trapped != KErrNone) {
            err = NmGeneralError;
        }
    }
    else {
        err = saveMessageWithSubparts();
    }

    if (err != NmNoError) {
        completeOperation(err);
    }
    // err == NmNoError means everything went well and the operation is proceeding
}

/*!
    
 */
void NmFwaMessageSendingOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

/*!
    
 */
void NmFwaMessageSendingOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
    if (mSaveOperation && mSaveOperation->isRunning()) {
        mSaveOperation->cancelOperation();
    }
}
    
/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaMessageSendingOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete) {
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

/*!
    Handle completed store message operation
 */
void NmFwaMessageSendingOperation::handleCompletedSaveOperation(int error)
{
    NM_FUNCTION;
    
    if (error == NmNoError) {
        mSaved = true;
        doRunAsyncOperation();
    }
    else {
        completeOperation(NmGeneralError);
    }
}

/*!
    Saves a message with its subparts (into message store).
*/
int NmFwaMessageSendingOperation::saveMessageWithSubparts()
{
    NM_FUNCTION;
    
    int ret = NmNotFoundError;
    if (mMessage) {
        if (mSaveOperation && mSaveOperation->isRunning()) {
            mSaveOperation->cancelOperation();
        }
        mSaveOperation = mPluginInterface.saveMessageWithSubparts(*mMessage);
        
        if (mSaveOperation) {
            connect(mSaveOperation, SIGNAL(operationCompleted(int)), this,
                SLOT(handleCompletedSaveOperation(int)));
            ret = NmNoError;
        }
        else {
            ret = NmGeneralError;
        }
            
    }

    return ret;
}

/*!
    Sends the message.
 */
int NmFwaMessageSendingOperation::sendMessageL()
{
    NM_FUNCTION;
    
    int ret = NmNotFoundError;
    
    if (mMessage) {
        TFSMailMsgId mailboxId = NmConverter::nmIdToMailMsgId(mMessage->envelope().mailboxId());
        CFSMailBox *currentMailbox( NULL );
        currentMailbox = mMailClient.GetMailBoxByUidL(mailboxId);
        if (!currentMailbox) {
            User::Leave( KErrNotFound );
        }
        CleanupStack::PushL(currentMailbox);
        CFSMailMessage *msg = CFSMailMessage::NewL(*mMessage); // no leave -> msg != NULL
        CleanupStack::PushL(msg);
        
        mRequestId = currentMailbox->SendMessageL(*msg, *this);
        CleanupStack::PopAndDestroy(2); // msg, currentMailbox
        
        ret = NmNoError;
    }
    
    return ret;
}
