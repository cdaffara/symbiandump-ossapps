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

#ifndef MEMSPYVIEWTHREADINFOITEMSTACK_H
#define MEMSPYVIEWTHREADINFOITEMSTACK_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

class RMemSpySession;

class CMemSpyViewThreadInfoItemStack : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemStack( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_STACK; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdStack; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // Command handlers
    void OnCmdDeviceStackSummaryL();
    void OnCmdStackInfoL();
    void OnCmdStackDataUserL();
    void OnCmdStackDataKernelL();
    void OnCmdStackDataUserAllThreadsL();
    void OnCmdStackDataKernelAllThreadsL();
    void OnCmdStackWatchForHighWatermarkL();
    };


#endif
