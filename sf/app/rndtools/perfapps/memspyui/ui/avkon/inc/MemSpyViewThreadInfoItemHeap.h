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

#ifndef MEMSPYVIEWTHREADINFOITEMHEAP_H
#define MEMSPYVIEWTHREADINFOITEMHEAP_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

class RMemSpySession;

class CMemSpyViewThreadInfoItemHeap : public CMemSpyViewThreadInfoItemGeneric
    {
public:
    CMemSpyViewThreadInfoItemHeap( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, TProcessId aProcId, TThreadId aId, TMemSpyThreadInfoItemType aType  );

public: // From CMemSpyViewBase
    TBool HandleCommandL( TInt aCommand );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_HEAP; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdHeap; }
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

private: // Command handlers
    void OnCmdHeapDataL();
    void OnCmdHeapCellListingL();
    void OnCmdHeapInfoL();
    };


#endif
