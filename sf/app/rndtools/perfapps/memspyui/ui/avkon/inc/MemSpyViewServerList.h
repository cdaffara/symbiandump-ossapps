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

#ifndef MEMSPYVIEWSERVERLIST_H
#define MEMSPYVIEWSERVERLIST_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewBase.h"

#include <memspy/api/memspyapiserver.h>
#include <memspysession.h>

// Classes referenced
class CMemSpyEngineServerList;
class CMemSpyEngineServerEntry;
class RMemSpySession;

class CMemSpyViewServerList : public CMemSpyViewBase
    {
public:
    //CMemSpyViewServerList( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
	CMemSpyViewServerList( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewServerList();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_SERVER_LIST; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdServerList; }

private: // Command handlers
    void OnCmdServerListSortByNameL();
    void OnCmdServerListSortBySessionCountL();
    void OnCmdServerListOutputSummaryL();
    void OnCmdServerListOutputDetailedL();
    void OnCmdServerListOutputGenericL( TBool aDetailed );

private: // From CMemSpyViewBase
    void SetListBoxModelL();
    void HandleListBoxItemActionedL( TInt aCurrentIndex );

private: // Data members
    CMemSpyEngineServerList* iList;
    const CMemSpyEngineServerEntry* iActionedItem;
    TInt iActionedItemIndex;
    RPointerArray<CMemSpyApiServer> iServers;
    TSortType iSort;
    };


#endif
