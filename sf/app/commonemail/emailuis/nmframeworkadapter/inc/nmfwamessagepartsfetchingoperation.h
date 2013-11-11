/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef NMFWAMESSAGEPARTSFETCHINGOPERATION_H_
#define NMFWAMESSAGEPARTSFETCHINGOPERATION_H_

#include <nmoperation.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaMessagePartsFetchingOperation : public NmOperation, public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessagePartsFetchingOperation(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const QList<NmId> &messagePartIds,
        CFSMailClient &mailClient);
    
    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    void doRunAsyncOperation();
    void doCompleteOperation();
    void doCancelOperation();
    
private:
    ~NmFwaMessagePartsFetchingOperation();
    void doRunAsyncOperationL();

private:
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
    RArray<TFSMailMsgId> mMessagePartIds;
    
    CFSMailClient &mMailClient;
    
    int mLastProgressValue;
    
    TInt mRequestId;
};

#endif /* NMFWAMESSAGEPARTSFETCHINGOPERATION_H_ */
