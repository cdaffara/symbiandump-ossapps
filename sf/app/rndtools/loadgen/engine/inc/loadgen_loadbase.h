/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef LOADGEN_LOADBASE_H
#define LOADGEN_LOADBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "loadgen_loadbase.h"


// FORWARD DECLARATIONS



// CLASS DECLARATIONS

class CLoadBase : public CBase
    {
public:
    enum TLoadState
        {
        ELoadStateInvalid = -1,
        ELoadStateConstructed,
        ELoadStateRunning,
        ELoadStateSuspended,
        ELoadStateClosed
        };
        
protected: // Constructors
    CLoadBase();
    void ConstructL();

public:  // New methods
    virtual void Resume();
    virtual void Suspend();
    virtual void SetPriority();
    virtual void Close();
    virtual TPtrC Description();
    inline TInt State() { return iState; }
    inline TInt Type() { return iType; }

protected: // Data
    TInt iState;
    TInt iType;
    };
 

#endif
