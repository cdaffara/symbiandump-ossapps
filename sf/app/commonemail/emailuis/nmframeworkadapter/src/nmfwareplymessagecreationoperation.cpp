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

NmFwaReplyMessageCreationOperation::NmFwaReplyMessageCreationOperation(
    const NmId &mailboxId,
    const NmId &originalMessageId,
    const bool replyAll,
    CFSMailClient &mailClient) :
    NmFwaMessageCreationOperation(mailboxId, mailClient),
    mOriginalMessageId(originalMessageId),
    mReplyAll(replyAll)
{
    NM_FUNCTION;
}

NmFwaReplyMessageCreationOperation::~NmFwaReplyMessageCreationOperation()
{
    NM_FUNCTION;
}

void NmFwaReplyMessageCreationOperation::doRunAsyncOperation()
{
    NM_FUNCTION;
    
    const TFSMailMsgId mailMsgId(mMailboxId.pluginId32(), mMailboxId.id32());

    CFSMailBox *mailBox(NULL);
    TRAP_IGNORE(mailBox = mMailClient.GetMailBoxByUidL(mailMsgId));
    if (mailBox) {
        TRAPD(err, mRequestId = mailBox->CreateReplyMessageL(mOriginalMessageId, mReplyAll, *this));
        if (err == KErrFSMailPluginNotSupported) {
            CFSMailMessage *fsMessage = mailBox->CreateReplyMessage(mOriginalMessageId, mReplyAll);
            mMessage = fsMessage->GetNmMessage();
            
            delete fsMessage;
            fsMessage = NULL;
            
            completeOperation(NmNoError);
        }
        else if (err != KErrNone) {
            completeOperation(NmGeneralError);
        }
        
        delete mailBox;
        mailBox = NULL;
    }
    else {
        completeOperation(NmNotFoundError);
    }
}
