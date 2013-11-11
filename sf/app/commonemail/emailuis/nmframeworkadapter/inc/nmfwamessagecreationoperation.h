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

#ifndef NMFAMESSAGECREATIONOPERATION_H_
#define NMFAMESSAGECREATIONOPERATION_H_

#include <nmmessagecreationoperation.h>
#include <nmcommon.h>
#include <CFSMailCommon.h>
#include <MFSMailRequestObserver.h>

class NmMessage;
class CFSMailClient;

class NmFwaMessageCreationOperation : public NmMessageCreationOperation,
                                      public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaMessageCreationOperation(
        const NmId &mailboxId,
        CFSMailClient &mailClient);
       
    NmMessage *getMessage();
    NmId getMessageId();

    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    virtual void doRunAsyncOperation();    
    virtual void doCompleteOperation();
    virtual void doCancelOperation();
    virtual ~NmFwaMessageCreationOperation();
protected:
    NmId mMailboxId;
    NmMessage *mMessage;   
    CFSMailClient &mMailClient;   
    TInt mRequestId;
    
};

#endif /* NMFAMESSAGECREATIONOPERATION_H_ */
