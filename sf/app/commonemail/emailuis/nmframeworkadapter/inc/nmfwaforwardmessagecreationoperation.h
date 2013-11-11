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

#ifndef NMFAFORWARDMESSAGECREATIONOPERATION_H_
#define NMFAFORWARDMESSAGECREATIONOPERATION_H_

#include "nmfwamessagecreationoperation.h"

class NmFwaForwardMessageCreationOperation : public NmFwaMessageCreationOperation
{
    Q_OBJECT
public:
    NmFwaForwardMessageCreationOperation(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        CFSMailClient &mailClient);
       
protected:
    void doRunAsyncOperation();
private:
    ~NmFwaForwardMessageCreationOperation();
   
private:
    NmId mOriginalMessageId;
};

#endif /* NMFAFORWARDMESSAGECREATIONOPERATION_H_ */
