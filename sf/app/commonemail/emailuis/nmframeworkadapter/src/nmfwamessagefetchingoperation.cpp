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

NmFwaMessageFetchingOperation::NmFwaMessageFetchingOperation(
    const NmId &mailboxId,
    const NmId &folderId,
    const NmId &messageId,
    CFSMailClient &mailClient) 
      : mMailboxId(mailboxId),
        mFolderId(folderId),
        mMessageId(messageId),
        mMailClient(mailClient),
        mRequestId(0)

{
    NM_FUNCTION;
}

NmFwaMessageFetchingOperation::~NmFwaMessageFetchingOperation()
{
    NM_FUNCTION;
    
    doCancelOperation();
}

void NmFwaMessageFetchingOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    const TFSMailMsgId mailboxId(mMailboxId.pluginId32(), mMailboxId.id32());
    const TFSMailMsgId folderId(mFolderId.pluginId32(), mFolderId.id32());
    const TFSMailMsgId messageId(mMessageId.pluginId32(), mMessageId.id32());

    CFSMailFolder *folder(NULL);
    TRAP_IGNORE(folder = mMailClient.GetFolderByUidL(mailboxId, folderId));
   
    if (folder) {
        RArray<TFSMailMsgId> messageIds; // Cleanupstack not needed
        messageIds.Append(messageId);
        TRAPD(err, mRequestId = folder->FetchMessagesL(messageIds, EFSMsgDataStructure, *this));
        messageIds.Close();
        if (KErrNone != err) {
            completeOperation(NmGeneralError);
        }
        delete folder;
        folder = NULL;
    }
    else {
        completeOperation(NmNotFoundError);
    }
}

/*!
 * Complete the operation
 */
void NmFwaMessageFetchingOperation::doCompleteOperation()
{
    NM_FUNCTION;
    
    mRequestId = NmNotFoundError;
}

/*!
    Cancels the async operation. \sa NmOperation
 */
void NmFwaMessageFetchingOperation::doCancelOperation()
{
    NM_FUNCTION;
    
    if (mRequestId >= 0) {
        TRAP_IGNORE(mMailClient.CancelL(mRequestId));
        mRequestId = NmNotFoundError;
    }
}

/*!
 asynchronous request response message
 
 \param aEvent plugin event description
 \param mailClient Reference to mail client object
 */
void NmFwaMessageFetchingOperation::RequestResponseL(TFSProgress aEvent, TInt aRequestId)
{
    NM_FUNCTION;
    
    if (aRequestId == mRequestId) {
        if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestComplete) {
            completeOperation(NmNoError);
        } else if (aEvent.iProgressStatus == TFSProgress::EFSStatus_RequestCancelled) {
            completeOperation(NmCancelError); 
        }
    }
}
