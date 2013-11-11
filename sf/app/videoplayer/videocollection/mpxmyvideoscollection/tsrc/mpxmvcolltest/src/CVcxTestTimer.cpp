/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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

// INCLUDE FILES
#include "CVcxTestTimer.h"
#include "MVcxTestTimerObserver.h"
#include "VCXTestLog.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVcxTestTimer::
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVcxTestTimer* CVcxTestTimer::NewL(MVcxTestTimerObserver& aObserver, TInt aId)
    {
    VCXLOGLO1(">>>CVcxTestTimer::NewL");
    CVcxTestTimer* self = new (ELeave) CVcxTestTimer(aObserver, aId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    VCXLOGLO1("<<<CVcxTestTimer::NewL");
    return self;
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::~CVcxTestTimer
// destructor
// -----------------------------------------------------------------------------
//
CVcxTestTimer::~CVcxTestTimer()
    {
    VCXLOGLO1(">>>CVcxTestTimer::~CVcxTestTimer");
    iRestartAfterCancel = EFalse;
    Cancel();
    iTimer.Close();

   	if( IsAdded() )
        {
        Deque(); // calls also Cancel()
        }

    VCXLOGLO1("<<<CVcxTestTimer::~CVcxTestTimer");
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::~CVcxTestTimer
// destructor
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::CancelTimer()
    {
    VCXLOGLO1(">>>CVcxTestTimer::CancelTimer");
    if( IsActive() )
    	{
    	iRestartAfterCancel = EFalse;
    	Cancel();
    	}
    VCXLOGLO1("<<<CVcxTestTimer::CancelTimer");
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::CVcxTestTimer
//
// -----------------------------------------------------------------------------
//
CVcxTestTimer::CVcxTestTimer(MVcxTestTimerObserver& aObserver, TInt aId) : CActive(EPriorityStandard), iObserver(aObserver),
	iId(aId)
    {
    VCXLOGLO1(">>>CVcxTestTimer::CVcxTestTimer");
    CActiveScheduler::Add(this);
    VCXLOGLO1("<<<CVcxTestTimer::CVcxTestTimer");
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::ConstructL
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::ConstructL()
    {
    VCXLOGLO1(">>>CVcxTestTimer::ConstructL");
    User::LeaveIfError(iTimer.CreateLocal());
    VCXLOGLO1("<<<CVcxTestTimer::ConstructL");
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::After
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::After(TTimeIntervalMicroSeconds32 aInterval)
    {

    iInterval = aInterval;

    if( !IsActive() )
    	{
	    VCXLOGLO2("CVcxTestTimer::After: Setting active. (%d)", this);
    	iTimer.After( iStatus, aInterval );
	    SetActive();
	    iRestartAfterCancel = EFalse; 
    	}
	else
		{
		// Cancel and start again when cancel is done.
		VCXLOGLO2("CVcxTestTimer::After: Restarting! (%d)", this);
		iRestartAfterCancel = ETrue;
		iTimer.Cancel();
		}
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::RunL
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::RunL()
    {
    TInt status = iStatus.Int();

    if( status == KErrNone )
        {
        iObserver.TimerComplete( iId, iStatus.Int() );
        }
    else
    if( status == KErrCancel && iRestartAfterCancel )
        {
        iRestartAfterCancel = EFalse;
        iTimer.After( iStatus, iInterval );
        SetActive();
        VCXLOGLO2("CVcxTestTimer::RunL: Restarted! (%d)", this);
        }
    }

// -----------------------------------------------------------------------------
// CVcxTestTimer::DoCancel
//
// -----------------------------------------------------------------------------
//
void CVcxTestTimer::DoCancel()
    {
    VCXLOGLO1(">>>CVcxTestTimer::DoCancel");
    iTimer.Cancel();
    VCXLOGLO1("<<<CVcxTestTimer::DoCancel");
    }

//  End of File
