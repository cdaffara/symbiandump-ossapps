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

#ifndef MEMSPYVIEWTHREADINFOITEMGENERIC_H
#define MEMSPYVIEWTHREADINFOITEMGENERIC_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyengineobjectthreadinfoobjects.h>
#include <memspy/api/memspyapithreadinfoitem.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class CAknWaitDialog;
class CMemSpyProcess;
class CMemSpyThread;
class CMemSpyThreadInfoContainer;
class CMemSpyThreadInfoItemBase;
class RMemSpySession;

class CMemSpyViewThreadInfoItemGeneric : public CMemSpyViewBase
    {
public:
    //CMemSpyViewThreadInfoItemGeneric( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aInfoContainer, TMemSpyThreadInfoItemType aType );
	CMemSpyViewThreadInfoItemGeneric( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcessId, TThreadId aId, TMemSpyThreadInfoItemType aType );
    ~CMemSpyViewThreadInfoItemGeneric();

public: // From CMemSpyViewBase
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // API
    CMemSpyProcess& Process() const;
    CMemSpyThread& Thread() const;
    CMemSpyThreadInfoContainer& Container() const;
    CMemSpyThreadInfoItemBase& InfoItem() const;
    TThreadId ThreadId() { return iThreadId; }
    
public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    TBool HandleCommandL( TInt aCommand );

protected: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );
    void HandleListBoxItemSelectedL( TInt aCurrentIndex );

private: // Internal methods
    void ShowWaitNoteL();
    void DestroyWaitNote();
    static TInt CheckForItemConstructionComplete( TAny* aSelf );

protected: // Member data
    //CMemSpyThreadInfoContainer& iContainer;
    //CMemSpyThreadInfoItemBase* iInfoItem;
    
private: // Member data
    CAknWaitDialog* iWaitNote;
    CPeriodic* iWaitConstructionChecker;
    TThreadId iThreadId;
    TProcessId iParentProcessId;
    TMemSpyThreadInfoItemType iType;
    };


#endif
