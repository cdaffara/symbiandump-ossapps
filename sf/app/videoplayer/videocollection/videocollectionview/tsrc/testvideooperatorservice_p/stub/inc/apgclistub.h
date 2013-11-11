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

#ifndef __APGCLI_H__
#define __APGCLI_H__

#include <e32base.h>
#include "apaidstub.h" 

class RApaLsSession : public RSessionBase
    {
public:

public: 
    RApaLsSession() {}

    TInt GetAppInfo(TApaAppInfo& aInfo, TUid /*aAppUid*/) const
        {
        aInfo.iFullName = mFileName;
        mGetAppInfoCallCount++;
        return 0;
        }
  
    TInt Connect() { return 0; }
    void Close() {}
    
    static TFileName mFileName;
    static TInt mGetAppInfoCallCount;
    };

#endif  // __APGCLI_H__
