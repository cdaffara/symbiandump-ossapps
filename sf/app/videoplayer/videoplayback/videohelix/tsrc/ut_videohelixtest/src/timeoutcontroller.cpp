/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Timeout controller
*
*/

// Version : %version: 3 %


#include "timeoutcontroller.h"
#include "mpxvideo_debug.h"

/*
-------------------------------------------------------------------------------
Class: CSimpleTimeout
Method: CSimpleTimeout
Description: Default constructor
C++ default constructor can NOT contain any code, that might leave.
Parameters: None
Return Values: None
Errors/Exceptions: None
Status: Approved
-------------------------------------------------------------------------------
*/
CSimpleTimeout::CSimpleTimeout()
    : CActive (CActive::EPriorityStandard)
{
    MPX_DEBUG(_L("CSimpleTimeout::CSimpleTimeout"));
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::ConstructL()
// Symbian OS second phase constructor.
// -----------------------------------------------------------------------------
//
void
CSimpleTimeout::ConstructL( MTimeoutObserver* aObserver, CStifLogger* aLogger )
{
    MPX_DEBUG(_L("CSimpleTimeout::ConstructL"));

    iObserver = aObserver;
    iLog = aLogger;
    iTimer.CreateLocal();
    iTestCaseTimeout = 0;  // Initialize

    // Add to active scheduler
    CActiveScheduler::Add ( this );
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::NewL()
// Two-phased constructor.
// Returns: CSimpleTimeout* : pointer to created object
// -----------------------------------------------------------------------------
//
CSimpleTimeout*
CSimpleTimeout::NewL( MTimeoutObserver* aTestClass, CStifLogger* aLogger )
{
    MPX_DEBUG(_L("CSimpleTimeout::NewL"));

    CSimpleTimeout* self = new ( ELeave ) CSimpleTimeout();
    CleanupStack::PushL( self );
    self->ConstructL( aTestClass, aLogger);
    CleanupStack::Pop( self );

    return self;
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::~CSimpleTimeout()
// Destructor.
// -----------------------------------------------------------------------------
//
CSimpleTimeout::~CSimpleTimeout()
{
    MPX_DEBUG(_L("CSimpleTimeout::~CSimpleTimeout"));
    Cancel();
    iTimer.Close();
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::Start()
// Start timeout counting.
// -----------------------------------------------------------------------------
//
void
CSimpleTimeout::Start( TTimeIntervalMicroSeconds aTimeout )
{
    MPX_DEBUG(_L("CSimpleTimeout::Start"));

    if ( IsActive() )
    {
        Cancel();
    }

    // Request timer
    TTime endTime;
    endTime.HomeTime();
    endTime = endTime + aTimeout;

    TInt64 miliseconds = aTimeout.Int64();
    miliseconds /= 1000;

    TBuf<30> dateString;
    TRAP_IGNORE(endTime.FormatL(dateString, KFormatTimeStamp));
    iLog->Log(_L("Timer=%LD ms, EndTime=%S"), miliseconds, &dateString);

    // Store absolute timeout
    iTestCaseTimeout = endTime;

    // Taken from STIF engine
    // Note: iTimer.After() method cannot use because there needed
    // TTimeIntervalMicroSeconds32 and it is 32 bit. So then cannot create
    // timeout time that is long enough. At() uses 64 bit value=>Long enough.
    iTimer.At( iStatus, endTime );
    SetActive();
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::Stop()
// Start timeout counting.
// -----------------------------------------------------------------------------
//
void
CSimpleTimeout::Stop()
{
    MPX_DEBUG(_L("CSimpleTimeout::Stop"));

    if ( IsActive() )
    {
        Cancel();
    }
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::RunL()
// RunL handles completed timeouts.
// -----------------------------------------------------------------------------
//
void
CSimpleTimeout::RunL()
{
    MPX_DEBUG(_L("CSimpleTimeout::RunL"));

    iLog->Log(_L("CSimpleTimeout::RunL"));

    TTime timeout;
    timeout.HomeTime();

    // Handle the abort case when system time gets changed, but timeout is
    // still valid. All other cases should timeout since they invalidate the
    // logic of the timers.
    if ( iStatus == KErrAbort)
    {
        if ( iTestCaseTimeout > timeout )
        {
            MPX_DEBUG(_L(
                "Absolute timer still valid. Restaring timer. iStatus: %d" ),
                iStatus.Int() );

            // Start new timer
            iStatus = KErrNone; // reset value
            iTimer.At ( iStatus, iTestCaseTimeout );  // restart timer
            SetActive();
        }
        else
        {
            // Absolute timer no longer valid. Must timeout.
            iLog->Log(_L("Absolute timeout no longer valid"));
            iObserver->HandleTimeout(KErrNone);
        }

    }
    else
    {
        // Status was not KErrAbort. Timing out!
        // iLog->Log(_L("CSimpleTimeout::RunL - Timeout !!"), iTimeout);
        iLog->Log(_L("Timing out"));
        iObserver->HandleTimeout(KErrNone);
    }
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::DoCancel()
// Cancel active request.
// -----------------------------------------------------------------------------
//
void
CSimpleTimeout::DoCancel()
{
    MPX_DEBUG(_L("CSimpleTimeout::DoCancel"));
    iTimer.Cancel();
}

// -----------------------------------------------------------------------------
// CSimpleTimeout::RunError()
// Handle errors. Just let framework handle errors because
// RunL does not leave.
// Returns: Symbian OS error code
// -----------------------------------------------------------------------------
//
TInt CSimpleTimeout::RunError( TInt aError )
{
    MPX_DEBUG(_L("CSimpleTimeout::RunError"));
    iLog->Log(_L("Timeout error %d"), aError);
    iObserver->HandleTimeout(aError);
    return aError;
}

// EOF
