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

#ifndef MEMSPYVIEWHEAPTRACKING_H
#define MEMSPYVIEWHEAPTRACKING_H

// System includes
#include <hal.h>
#include <coecntrl.h>
#include <aknlists.h>

// Engine includes
#include <memspy/engine/memspyenginehelpersysmemtrackerconfig.h>
#include <memspy/engine/memspyenginehelpersysmemtrackerobserver.h>

// User includes
#include "MemSpyAsyncTracker.h"
#include "MemSpyViewBase.h"

// Classes referenced
class RMemSpySession;


class CMemSpyViewHeapTracking : public CMemSpyViewBase, public MMemSpyEngineHelperSysMemTrackerObserver
    {
public:
	CMemSpyViewHeapTracking( RMemSpySession& aSession, MMemSpyViewObserver& aObserver );
    ~CMemSpyViewHeapTracking();
    void ConstructL( const TRect& aRect, CCoeControl& aContainer, TAny* aSelectionRune = NULL );

public: // From CMemSpyViewBase
    void RefreshL();
    TMemSpyViewType ViewType() const;
    CMemSpyViewBase* PrepareParentViewL();
    CMemSpyViewBase* PrepareChildViewL();

public:
    static TInt AsyncStopTimerCallback( TAny* aParam );

private: // From CMemSpyViewBase
    void SetListBoxModelL();

private: // From MMemSpyEngineHelperSysMemTrackerObserver
    void HandleCyclesResetL();
    void HandleCycleStartedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    void HandleCycleFinishedL( const CMemSpyEngineHelperSysMemTrackerCycle& aCycle );
    
private: // Internal methods
    TInt IndexByViewTypeL( TMemSpyViewType aType );
    void SetConfigByModeL( TMemSpyEngineHelperSysMemTrackerConfig::TMemSpyEngineSysMemTrackerMode aMode, TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    TInt AsyncStopTimerCallback();
    void GetSwmtConfigL( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );
    void SetSwmtConfigL( TMemSpyEngineHelperSysMemTrackerConfig& aConfig );    	
	void DumpNowL();
	void StartSwmtTimerL();
    
private: // Enum
    enum TMemSpyViewHeapTrackingState
        {
        EMemSpyViewHeapTrackingStateIdle = 0,
        EMemSpyViewHeapTrackingStateTimerOn,
        EMemSpyViewHeapTrackingStateSingleOn        
        };

private: // Data
    TMemSpyEngineHelperSysMemTrackerConfig iOriginalConfig;
    TMemSpyViewHeapTrackingState iState;
    CAsyncCallBack* iStopTimerCallBack;
    };


#endif
