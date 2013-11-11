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

#ifndef MEMSPYVIEWTHREADS_H
#define MEMSPYVIEWTHREADS_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>
#include <e32const.h>

// User includes
#include "MemSpyViewBase.h"

#include <memspy/api/memspyapithread.h>

// Classes referenced
class CMemSpyProcess;
class CMemSpyThread;
class RMemSpySession;

class CMemSpyViewThreads : public CMemSpyViewBase
    {
public:
    //CMemSpyViewThreads( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyProcess& aProcess );
	CMemSpyViewThreads( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aId, TThreadId aThreadId );
    ~CMemSpyViewThreads();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // API
    //const CMemSpyProcess& Process() const;
    //CMemSpyThread& CurrentThread();
    TProcessId Process() const;
    TThreadId CurrentThread();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_THREAD; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdThread; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    TBool HandleCommandL( TInt aCommand );

private: // Command handlers
    void OnCmdEndKillL();
    void OnCmdEndTerminateL();
    void OnCmdEndPanicL();
    void OnCmdSetPriorityL( TInt aCommand );
    void OnCmdInfoHandlesL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );
    void HandleListBoxItemSelectedL( TInt aCurrentIndex );    
    void AppendPriority( TDes& aDes, TThreadPriority aPriority );

private: // Member data
    //CMemSpyProcess& iParentProcess;
    //CMemSpyThread* iCurrentThread;
    TThreadId iCurrentThreadId;
    TProcessId iParentProcessId;
    
    RPointerArray<CMemSpyApiThread> iThreads;
    };


#endif
