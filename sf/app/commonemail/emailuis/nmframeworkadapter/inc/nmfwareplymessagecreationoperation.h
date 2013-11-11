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

#ifndef NMFAREPLYMESSAGECREATIONOPERATION_H_
#define NMFAREPLYMESSAGECREATIONOPERATION_H_

#include "nmfwamessagecreationoperation.h"

class NmFwaReplyMessageCreationOperation : public NmFwaMessageCreationOperation
{
    Q_OBJECT
public:
    NmFwaReplyMessageCreationOperation(
        const NmId &mailboxId,
        const NmId &originalMessageId,
        const bool replyAll,
        CFSMailClient &mailClient);
    
protected:
    void doRunAsyncOperation();
    
private:
    ~NmFwaReplyMessageCreationOperation();
    
private:
    NmId mOriginalMessageId;
    bool mReplyAll;
};

#endif /* NMFAREPLYMESSAGECREATIONOPERATION_H_ */
