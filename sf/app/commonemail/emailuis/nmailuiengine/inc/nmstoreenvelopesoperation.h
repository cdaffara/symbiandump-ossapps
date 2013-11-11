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

#ifndef NMSTOREENVELOPESOPERATION_H_
#define NMSTOREENVELOPESOPERATION_H_

#include "nmoperation.h"

class NMUIENGINE_EXPORT NmStoreEnvelopesOperation: public NmOperation
{
    Q_OBJECT
public:
    NmStoreEnvelopesOperation(){};
    
protected:
    virtual ~NmStoreEnvelopesOperation(){};
};

#endif //#ifndef NMSTOREENVELOPESOPERATION_H_
