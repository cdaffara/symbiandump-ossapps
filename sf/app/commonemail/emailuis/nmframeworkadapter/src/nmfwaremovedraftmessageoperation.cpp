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
    \class NmFwaRemoveDraftMessageOperation
    
    \brief NmFwaRemoveDraftMessageOperation is an async operation which deletes a draft message.
    
    NmFwaRemoveDraftMessageOperation is an async operation which cdeletes a draft message. 
    \sa NmOperation
 */

/*!
    Constructor
    
    \param message pointer to the message under sent, ownership is transferred to operation
    \param mailClient Reference to mail client object.
 */
NmFwaRemoveDraftMessageOperation::NmFwaRemoveDraftMessageOperation(
    NmDataPluginInterface &pluginInterface,
    NmMessage *message,
    CFSMailClient &mailClient) :
        mPluginInterface(pluginInterface),
        mMessage(message),
        mMailClient(mailClient),
        mRequestId(NmNotFoundError)
{
    NM_FUNCTION;
    
    mMailClient.IncReferenceCount();
}

/*!
    Destructor
 */
NmFwaRemoveDraftMessageOperation::~NmFwaRemoveDraftMessageOperation()
{
    NM_FUNCTION;
   
    mMailClient.Close(); // decrease ref count
    delete mMessage;
    mMessage = NULL;
}

/*!
    Called after base object construction via timer event, runs the
    async operation.
    
    \sa NmOperation
 */
void NmFwaRemoveDraftMessageOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    TRAPD(err, removeMessageL());
        
    if (err != KErrNone) {
        completeOperation(NmGeneralError);
    }
}

/*!
    
 */
void NmFwaRemoveDraftMessageOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

/*!
    Asynchronous request response message.
    
    \param aEvent Plugin event description.
    \param aRequestId Request id of asyncronous operation.
 */
void NmFwaRemoveDraftMessageOperation::RequestResponseL(TFSProgress aEvent,
                                                    TInt aRequestId)
{
    NM_FUNCTION;
    
    TFSProgress::TFSProgressStatus status = aEvent.iProgressStatus;

    if (aRequestId == mRequestId) {
        if (status == TFSProgress::EFSStatus_RequestComplete && !aEvent.iError) {
            completeOperation(NmNoError);
        }
        else {
            completeOperation(NmGeneralError);
        }
    }
}

/*!
    Removes the message.
 */
void NmFwaRemoveDraftMessageOperation::removeMessageL()
{
    NM_FUNCTION;
    
    TFSMailMsgId mailboxId(mMessage->envelope().mailboxId());
    TFSMailMsgId folderId(mMessage->envelope().folderId());
    TFSMailMsgId messageId(mMessage->envelope().messageId());
    
    CFSMailFolder *folder = mMailClient.GetFolderByUidL(mailboxId, folderId);
    CleanupStack::PushL(folder);
    if (folder) {
        // try to use the asynchronous version first
        TRAPD(err, mRequestId = folder->RemoveMessageL(messageId, *this));
        
        if(err == KErrFSMailPluginNotSupported) {
            // async version not supported, use sync version
            folder->RemoveMessageL(messageId);
            completeOperation(NmNoError);
        } else if (KErrNone != err) {
            completeOperation(NmGeneralError);
        }
    }
    else {
        User::Leave(KErrNotFound);
    }
    CleanupStack::PopAndDestroy(folder);
}
