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

#ifndef NMFASTOREENVELOPESOPERATION_H_
#define NMFASTOREENVELOPESOPERATION_H_

#include <nmstoreenvelopesoperation.h>
#include <nmcommon.h>
#include <MFSMailRequestObserver.h>

class CFSMailClient;
class CFSMailMessage;

class NmFwaStoreEnvelopesOperation : public NmStoreEnvelopesOperation, public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaStoreEnvelopesOperation(
            const NmId &mailboxId,
            RPointerArray<CFSMailMessage> messages,
            CFSMailClient &mailClient);
    
    // from MFSMailRequestObserver
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    void doRunAsyncOperation();
    void doCancelOperation();
    
private:
    ~NmFwaStoreEnvelopesOperation();

private:
    NmId                            mMailboxId;
    CFSMailClient                   &mMailClient;
    RPointerArray<CFSMailMessage>   mMessages;
    TInt                            mRequestId;
};

#endif //NMFASTOREENVELOPESOPERATION_H_
