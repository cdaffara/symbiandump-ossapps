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

#ifndef NMCHECKOUTBOXOPERATION_H_
#define NMCHECKOUTBOXOPERATION_H_

#include "nmoperation.h"

class NmId;

class NMUIENGINE_EXPORT NmCheckOutboxOperation: public NmOperation
{
    Q_OBJECT
public:
    NmCheckOutboxOperation(){};
    virtual bool getMessageId(NmId &messageId) const = 0;

protected:
    virtual ~NmCheckOutboxOperation(){};
};

#endif /* NMCHECKOUTBOXOPERATION_H_ */
