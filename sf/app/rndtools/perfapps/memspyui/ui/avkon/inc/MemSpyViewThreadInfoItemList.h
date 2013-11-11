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

#ifndef MEMSPYVIEWTHREADINFOITEMLIST_H
#define MEMSPYVIEWTHREADINFOITEMLIST_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"
#include <memspy/engine/memspyengineobjectthreadinfocontainer.h>

// Classes referenced
class CMemSpyProcess;
class CMemSpyThread;
class CMemSpyThreadInfoItemBase;
class RMemSpySession;

class CMemSpyViewThreadInfoItemList : public CMemSpyViewBase, public MMemSpyThreadInfoContainerObserver
    {
public:
    //CMemSpyViewThreadInfoItemList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThread& aThread );
	CMemSpyViewThreadInfoItemList( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcess, TThreadId aThread );
    ~CMemSpyViewThreadInfoItemList();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TMemSpyThreadInfoItemType iType);

public: // API
    const CMemSpyThreadInfoItemBase& CurrentInfoItem() const;

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_THREAD; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdThread; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // Command handlers
    void OnCmdInfoHandlesL();

private: // From MMemSpyThreadInfoContainerObserver
    void HandleMemSpyEngineInfoContainerEventL( TEvent aEvent, TMemSpyThreadInfoItemType aType );

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );
    void HandleListBoxItemSelectedL( TInt aCurrentIndex );
    static TInt IdleUpdateListBoxModel( TAny* aSelf );
    void DoIdleUpdateListBoxModelL();   

private: // Member data
    //CMemSpyThread& iThread;
    TThreadId iThreadId;
    TProcessId iParentProcessId;
    CMemSpyThreadInfoItemBase* iCurrentInfoItem;
    TInt iCurrentInfoItemId;
    CPeriodic* iIdleResetListboxTimer;
    };


#endif
