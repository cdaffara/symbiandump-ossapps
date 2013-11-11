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

#ifndef __APGTASK_H__
#define __APGTASK_H__

#include "coemainstub.h"
#include <e32base.h>

class TApaTask
{
public:
    TApaTask(RWsSession& /*aWsSession*/) {}
    
    TBool Exists() const { return mExistsReturnValue; }
    void BringToForeground() { mBringToForegroundCallcount++; }

    static bool mExistsReturnValue;
    static int mExistsCallCount;
    static int mBringToForegroundCallcount;
};

class TApaTaskList
{
public:
    TApaTaskList(RWsSession& /*aWsSession*/) {}
    TApaTask FindApp(TUid /*aAppUid*/) { mFindAppCallCount++; return TApaTask(CCoeEnv::mSession); }

    static int mFindAppCallCount;
};

#endif

// End of file
