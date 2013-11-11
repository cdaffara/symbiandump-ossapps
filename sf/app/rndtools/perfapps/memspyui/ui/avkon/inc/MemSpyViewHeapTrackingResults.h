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

#ifndef MEMSPYVIEWHEAPTRACKINGRESULTS_H
#define MEMSPYVIEWHEAPTRACKINGRESULTS_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyenginehelpersysmemtrackerobserver.h>
#include <memspy/api/memspyapimemorytrackingcycle.h>

// Driver includes
#include <memspy/driver/memspydriverobjectsshared.h>

// User includes
#include "MemSpyViewBase.h"

// Classes referenced
class CMemSpyEngineHelperSysMemTrackerCycle;
class CMemSpyEngineHelperSysMemTrackerCycleChange;
class RMemSpySession;

class CMemSpyViewHeapTrackingResults : public CMemSpyViewBase, public MMemSpyEngineHelperSysMemTrackerObserver
    {
public:
    //CMemSpyViewHeapTrackingResults( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver );
	CMemSpyViewHeapTrackingResults( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewHeapTrackingResults();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // From MMemSpyEngineHelperSysMemTrackerObserver
    void HandleCyclesResetL();
    void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
private:
    RArray<CMemSpyApiMemoryTrackingCycle*> iCycles;
    };







class CMemSpyViewHeapTrackingResultsCycleInfo : public CMemSpyViewBase, public MMemSpyEngineHelperSysMemTrackerObserver
    {
public:
   // CMemSpyViewHeapTrackingResultsCycleInfo( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
	CMemSpyViewHeapTrackingResultsCycleInfo( RMemSpySession& aSession, MMemSpyViewObserver& aObserver, const CMemSpyApiMemoryTrackingCycle& aCycle );
    ~CMemSpyViewHeapTrackingResultsCycleInfo();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // From MMemSpyEngineHelperSysMemTrackerObserver
    void HandleCyclesResetL();
    void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    //const CMemSpyEngineHelperSysMemTrackerCycle& iCycle;
    const CMemSpyApiMemoryTrackingCycle& iCycle;    
    };





/*
class CMemSpyViewHeapTrackingResultsChangeDescriptor : public CMemSpyViewBase, public MMemSpyEngineHelperSysMemTrackerObserver
    {
public:
    CMemSpyViewHeapTrackingResultsChangeDescriptor( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, const CMemSpyEngineHelperSysMemTrackerCycle& aCycle, const CMemSpyEngineHelperSysMemTrackerCycleChange& aChangeDescriptor, TInt aIndex );
    ~CMemSpyViewHeapTrackingResultsChangeDescriptor();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // From MMemSpyEngineHelperSysMemTrackerObserver
    void HandleCyclesResetL();
    void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );

private: // Data members
    const CMemSpyEngineHelperSysMemTrackerCycle& iCycle;
    const CMemSpyEngineHelperSysMemTrackerCycleChange& iChangeDescriptor;
    TInt iIndex; // For selection rune when moving to parent view
    };
*/




#endif
