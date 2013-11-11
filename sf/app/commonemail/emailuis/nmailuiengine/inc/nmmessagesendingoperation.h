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

#ifndef NMMESSAGESENDINGOPERATION_H_
#define NMMESSAGESENDINGOPERATION_H_

#include "nmoperation.h"

class NmMessage;

class NMUIENGINE_EXPORT NmMessageSendingOperation : public NmOperation
{
public:
    NmMessageSendingOperation(){};
    virtual const NmMessage *getMessage() const = 0;
    
protected:
    virtual ~NmMessageSendingOperation(){};
};

#endif /* NMMESSAGESENDINGOPERATION_H_ */
