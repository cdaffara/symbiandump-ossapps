/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     This class implements a timer used when waiting for information from WAP GW
*
*/



// INCLUDE FILES
#include    "mmsoperationtimer.h"
#include    "mmsconst.h"
#include    "mmstransportobserver.h"
#include    "mmsservercommon.h"
#include    "MmsServerDebugLogging.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
#ifndef _NO_MMSS_LOGGING_
const TInt KMmsSecondsToMilliseconds = 1000000;
#endif
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ==================== LOCAL FUNCTIONS ====================
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMmsOperationTimer
// ---------------------------------------------------------
//
CMmsOperationTimer::CMmsOperationTimer()
    :CTimer ( EPriorityStandard )
    {   
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CMmsOperationTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CMmsOperationTimer* CMmsOperationTimer::NewL()
    {
    CMmsOperationTimer* self = new ( ELeave ) CMmsOperationTimer;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// ~CMmsOperationTimer
// ---------------------------------------------------------
//
CMmsOperationTimer::~CMmsOperationTimer()
    {
    Cancel();        
    }

// ---------------------------------------------------------
// Start
// ---------------------------------------------------------
//
void CMmsOperationTimer::Start(
    MMmsTransportObserver* aRequester,
    const TInt aTimerValue )
    {
    LOG2( _L("CMmsOperationTimer::Start (%d seconds)"), aTimerValue/KMmsSecondsToMilliseconds );

    // no problem with prototype coercion - LINT
    iOriginalTimerValue = aTimerValue;

    iRequester = (MMmsTransportObserver*) aRequester;
    
    // time now
    iStartTime.HomeTime();

    // endtime
    iEndTime = iStartTime + iOriginalTimerValue;
    
    At( iEndTime ); // At() calls SetActive() 
    }

// ---------------------------------------------------------
// Pause
// ---------------------------------------------------------
//
void CMmsOperationTimer::Pause()
    {
    LOG( _L("CMmsOperationTimer::Pause") );
    // The status of timer is still pending
    Cancel();   // cancel the request
    // Calculate how much was left
    iCurrentTime.HomeTime(); 
    iTimeLeft = iEndTime.MicroSecondsFrom( iCurrentTime );      
    }

// ---------------------------------------------------------
// Continue
// ---------------------------------------------------------
//
void CMmsOperationTimer::Continue() 
    {    
    LOG( _L("CMmsOperationTime::Continue") );
    // If timer active, Continue() will do nothing
    if ( !IsActive() )
        {   
        // just make sure
        Cancel();

        // New expiration time will be time now + how much was left before pausing...
        iCurrentTime.HomeTime();
        iEndTime = iCurrentTime + iTimeLeft;

        At( iEndTime );
        }
    }

// ---------------------------------------------------------
// RunL()
// ---------------------------------------------------------
//
void CMmsOperationTimer::RunL()
    {
    //
    // timer completes and control is returned to caller
    //
    LOG2( _L("CMmsOperationTimer::RunL status %d"), iStatus.Int() );
     if ( iStatus.Int() == KErrAbort )
        {
        LOG( _L("- CMmsOperationTimer aborted abnormally - continuing.") );
        iCurrentTime.HomeTime(); 
        iTimeLeft = iEndTime.MicroSecondsFrom( iCurrentTime );
        Continue();
        return;
        }

    iTimeLeft = iOriginalTimerValue;
    iRequester->TimerExpired();  
    }

// ---------------------------------------------------------
// DoCancel()
// ---------------------------------------------------------
//
void CMmsOperationTimer::DoCancel()
    {
    LOG( _L("CMmsOperationTimer::DoCancel") );
    CTimer::DoCancel();
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

//  End of File  

