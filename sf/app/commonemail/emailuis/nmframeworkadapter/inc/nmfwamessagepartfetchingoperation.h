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

#ifndef NMFWAMESSAGEPARTFETCHINGOPERATION_H_
#define NMFWAMESSAGEPARTFETCHINGOPERATION_H_

#include <nmoperation.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaMessagePartFetchingOperation : public NmOperation, public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessagePartFetchingOperation(
        const NmId &mailboxId,
        const NmId &folderId,
        const NmId &messageId,
        const NmId &messagePartId,
        CFSMailClient &mailClient);
    
    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    void doRunAsyncOperation();
    void doCompleteOperation();
    void doCancelOperation();
    
private:
    ~NmFwaMessagePartFetchingOperation();
    void doRunAsyncOperationL();

private:
    NmId mMailboxId;
    NmId mFolderId;
    NmId mMessageId;
    NmId mMessagePartId;   
    CFSMailClient &mMailClient;  
    int mLastProgressValue;   
    TInt mRequestId;
};

#endif /* NMFWAMESSAGEPARTFETCHINGOPERATION_H_ */
