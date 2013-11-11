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

#ifndef MEMSPYVIEWTHREADINFOITEMMEMORYTRACKING_H
#define MEMSPYVIEWTHREADINFOITEMMEMORYTRACKING_H

// System includes
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyengineprocessmemorytracker.h>

// User includes
#include "MemSpyViewThreadInfoItemGeneric.h"

// Classes referenced
class CMemSpyEngineProcessMemoryTracker;


class CMemSpyViewThreadInfoItemMemoryTracking : public CMemSpyViewThreadInfoItemGeneric, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    CMemSpyViewThreadInfoItemMemoryTracking( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();
    TBool HandleCommandL( TInt aCommand );
    TInt IndexByViewType( TMemSpyViewType aType );
    void HandleListBoxItemActionedL( TInt aIndex );
    void RefreshL();
    void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_MEMORY_TRACKING; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdMemoryTracking; }

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL();
    void HandleMemoryTrackingStoppedL();
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

private: // Command handlers
    void OnCmdTrackingStartL();
    void OnCmdTrackingStopL();
    void OnCmdHWMResetL();
    void OnCmdTotalWithSharedMemL();
    void OnCmdTotalWithoutSharedMemL();
    };


class CMemSpyViewThreadInfoItemMemoryTrackingCurrent : public CMemSpyViewThreadInfoItemGeneric, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    CMemSpyViewThreadInfoItemMemoryTrackingCurrent( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    void RefreshL();
    void SetListBoxModelL();

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL() { }
    void HandleMemoryTrackingStoppedL() { }
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );
    };



class CMemSpyViewThreadInfoItemMemoryTrackingHWM : public CMemSpyViewThreadInfoItemGeneric, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    CMemSpyViewThreadInfoItemMemoryTrackingHWM( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    TBool HandleCommandL( TInt aCommand );
    void RefreshL();
    void SetListBoxModelL();

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL() { }
    void HandleMemoryTrackingStoppedL() { }
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );

public: // Menu framework
    TUint MenuCascadeResourceId() const { return R_MEMSPY_MENUPANE_MEMORY_TRACKING_HWM; }
    TInt MenuCascadeCommandId() const { return EMemSpyCmdMemoryTrackingHWM; }

private: // Command handlers
    void OnCmdHWMResetL();
    };
    



class CMemSpyViewThreadInfoItemMemoryTrackingPeak : public CMemSpyViewThreadInfoItemGeneric, public MMemSpyEngineProcessMemoryTrackerObserver
    {
public:
    CMemSpyViewThreadInfoItemMemoryTrackingPeak( CMemSpyEngine& aEngine, MMemSpyViewObserver& aObserver, CMemSpyThreadInfoContainer& aContainer );
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    TBool HandleCommandL( TInt aCommand );
    void RefreshL();
    void SetListBoxModelL();

private: // From MMemSpyEngineProcessMemoryTrackerObserver
    void HandleMemoryTrackingStartedL() { }
    void HandleMemoryTrackingStoppedL() { }
    void HandleMemoryChangedL( const TProcessId& aPid, const TMemSpyDriverProcessInspectionInfo& aCurrentInfo, const TMemSpyDriverProcessInspectionInfo& aHWMInfoIncShared, const TMemSpyDriverProcessInspectionInfo& aHWMInfoExcShared );
    };

#endif
