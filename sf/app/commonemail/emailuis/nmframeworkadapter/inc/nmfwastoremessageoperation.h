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

#ifndef NMFASTOREMESSAGEOPERATION_H_
#define NMFASTOREMESSAGEOPERATION_H_

#include <nmoperation.h>
#include <nmcommon.h>
#include <MFSMailRequestObserver.h>

class CFSMailClient;
class CFSMailMessage;

class NmFwaStoreMessageOperation : public NmOperation,
                                   public MFSMailRequestObserver
{
    Q_OBJECT
public:
    NmFwaStoreMessageOperation(
			CFSMailMessage *messages,
            CFSMailClient &mailClient);
    
    void RequestResponseL(TFSProgress aEvent, TInt aRequestId);

protected:
    void doRunAsyncOperation();
    void doCompleteOperation();
    void doCancelOperation();

private:
    ~NmFwaStoreMessageOperation();
    enum TStatus
        {
        EStoreHeader,
        EStoreSubParts,
        EComplete
        };
    
    CFSMailMessage  *mMessage;    //owned
    CFSMailClient   &mMailClient;
    TInt            mRequestId;
    TStatus         mStatus;
};

#endif //NMFASTOREMESSAGEOPERATION_H_
