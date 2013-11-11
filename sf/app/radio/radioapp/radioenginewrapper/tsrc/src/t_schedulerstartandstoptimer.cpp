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
* Description:  Implementation of a timer interface.
*
*/



#include "t_schedulerstartandstoptimer.h"
#include "trace.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSchedulerStopAndStartTimer* CSchedulerStopAndStartTimer::NewL(
        MSchedulerStartAndStopTimerObserver& aSchedulerStartAndStopTimerObserver )
    {
    FUNC_LOG;

    CSchedulerStopAndStartTimer* self = new ( ELeave ) CSchedulerStopAndStartTimer(
            aSchedulerStartAndStopTimerObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// Destructor
CSchedulerStopAndStartTimer::~CSchedulerStopAndStartTimer()
    {
    FUNC_LOG;
    Cancel();
    }

// -----------------------------------------------------------------------------
// CSchedulerStopAndStartTimer::StartTimer
// .
// -----------------------------------------------------------------------------
//
void CSchedulerStopAndStartTimer::StartTimer(
        TInt aTime,
        TUint aTimerId  )
    {
    FUNC_LOG;
    INFO_2( "Timer Id: %d, Time: %d", aTimerId, aTime)
    iTime = aTime;
    iTimerId = aTimerId;

    if ( ETimerIdRunMUT == iTimerId )
        {
        After( ( TTimeIntervalMicroSeconds32 ) iTime );
        }
    else
        {
        After( ( TTimeIntervalMicroSeconds32 ) 1 );
        }
    CActiveScheduler::Start();
    }

// -----------------------------------------------------------------------------
// CSchedulerStopAndStartTimer::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSchedulerStopAndStartTimer::RunL()
    {
    FUNC_LOG;
    iSchedulerStartAndStopTimerObserver.Timeout( iTimerId );        
    if ( ETimerIdCreateMUT == iTimerId )
        {
        iSchedulerStartAndStopTimerObserver.CreateMUT();
        iTimerId = ETimerIdRunMUT;
        After( ( TTimeIntervalMicroSeconds32 ) iTime );        
        }
    else if ( ETimerIdDeleteMUT == iTimerId )
        {
        iSchedulerStartAndStopTimerObserver.DeleteMUT();
        CActiveScheduler::Stop();
        }
    else
        {
        CActiveScheduler::Stop();
        }
    }


// -----------------------------------------------------------------------------
// CSchedulerStopAndStartTimer::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSchedulerStopAndStartTimer::RunError( TInt aError )
    {
    FUNC_LOG;
     // Avoid Panic in CActiveScheduler
    aError = KErrNone;
    return aError;
    }


// -----------------------------------------------------------------------------
// CSchedulerStopAndStartTimer::CSchedulerStopAndStartTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSchedulerStopAndStartTimer::CSchedulerStopAndStartTimer(
        MSchedulerStartAndStopTimerObserver& aSchedulerStartAndStopTimerObserver) : 
    CTimer( CActive::EPriorityStandard ),
    iSchedulerStartAndStopTimerObserver( aSchedulerStartAndStopTimerObserver )
    {
    FUNC_LOG;
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSchedulerStopAndStartTimer::ConstructL()
    {
    FUNC_LOG;
    CTimer::ConstructL();
    CActiveScheduler* AS = CActiveScheduler::Current();
    CActiveScheduler::Add( this );
    }


// ======== GLOBAL FUNCTIONS ========

