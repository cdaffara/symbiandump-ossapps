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

#ifndef __COEMAIN_H__
#define __COEMAIN_H__

typedef int RWsSession;

#include <e32base.h>

class CCoeEnv;

extern CCoeEnv *g_Env;

class CCoeEnv
{
public:

    static CCoeEnv* Static() 
    { 
        if(mStaticReturnsNull) 
            return 0;
        if(!g_Env) 
            g_Env = new (ELeave) CCoeEnv; 
        return g_Env; 
    }
    
    /**
     * Must be called before test exits if it has called the static method. 
     */
    static void Reset() { delete g_Env; g_Env = 0; }

    RWsSession &WsSession() { return mSession; }
    
    // Construction and destruction
    CCoeEnv() { }
    ~CCoeEnv() {}
    
    static int mCCoeEnvCallCount;
    static RWsSession mSession;
    static bool mStaticReturnsNull;
};

#endif

// End of file.
