/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CMMCScBkupState implementation
*
*
*/

#include "CMMCScBkupState.h"

// User includes
#include "MMCScBkupLogger.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CMMCScBkupState::CMMCScBkupState()
// 
// C++ constructor.
// ---------------------------------------------------------------------------
CMMCScBkupState::CMMCScBkupState( MMMCScBkupDriver& aDriver, TInt aPriority )
:   CActive( aPriority ), iDriver( aDriver )
    {
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::~CMMCScBkupState()
// 
// Destructor.
// ---------------------------------------------------------------------------
CMMCScBkupState::~CMMCScBkupState()
    {
    Cancel();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::RunL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::RunL()
    {
    User::LeaveIfError(iStatus.Int());
    //
    PerformAsynchronousStateStepL();
    //
    if  (!IsActive())
        {
        // If we've processed all the asynchronous steps, then
        // perform the last rights on the state.
        //
        // If this causes a leave, then we cascade the failure
        // to the observer - i.e. the error is treated as fatal.
        TRAPD(err, PerformLastRightsL() );
        CompleteObserver( err );
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::DoCancel()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::DoCancel()
    {
    __LOG3("CMMCScBkupState::DoCancel() - START - state: 0x%08x, IsActive: %d, status: %d", StateId().iUid, IsActive(), iStatus.Int() );
    PerformAsynchronousCancellation();
    CompleteObserver( KErrCancel );
    __LOG3("CMMCScBkupState::DoCancel() - END - state: 0x%08x, IsActive: %d, status: %d", StateId().iUid, IsActive(), iStatus.Int() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::RunError()
// 
// 
// ---------------------------------------------------------------------------
TInt CMMCScBkupState::RunError(TInt aError)
    {
#ifdef MMCSCBKUP_USE_BREAKPOINTS
    __BREAKPOINT();
#endif
    //
    if  ( aError == KErrNoMemory || aError == KErrDiskFull || aError == KErrNotReady || aError == KErrServerTerminated || aError == KErrWrite )
        {
        __LOGFILE2("CMMCScBkupState::RunError() - **** - FATAL ERROR - state: 0x%08x, aError: %d - Notifying Observer (Engine)", StateId().iUid, aError );
        CompleteObserver( aError );
        }
    else
        {
        __LOGFILE2("CMMCScBkupState::RunError() - **** - ATTEMPT TO HANDLE ERROR - state: 0x%08x, aError: %d...", StateId().iUid, aError );

        const TBool errorHandled = PerformAsynchronousErrorCleanup( aError );

        __LOGFILE1("CMMCScBkupState::RunError() - **** - handle error result: %d", errorHandled );

        // If the cleanup callback didn't set us active again, then
        // assume the object didn't perform any recovery. Therefore 
        // inform our observer about the error and give up.
        if  (!errorHandled || !IsActive())
            {
            CompleteObserver( aError );
            }
        }
    //
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::ExecuteL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::ExecuteL(TRequestStatus& aObserver)
    {
    SetObserver(aObserver);
    TRAPD(error, PerformStateInitL());

    // If the object is active, then it is making use of asynchronous
    // functionality, in which case we do nothing.
    //
    // If the object is not active or the call to PerformStateActionL left, 
    // then we complete the observer with the result of the trap
    if  (error != KErrNone || IsActive() == EFalse)
        {
        if  (error == KErrNone)
            {
            PerformLastRightsL();
            }
        //
        CompleteObserver(error);
        }
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::PerformAsynchronousStateStepL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::PerformAsynchronousStateStepL()
    {
    // Derived classes should override this if they implement
    // asynchronous behaviour
    User::Invariant();
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::PerformAsynchronousCancellation()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::PerformAsynchronousCancellation()
    {
    // Derived classes are expected to implement this if they have
    // resources to free. For calls to CompleteSelf(), no action is needed.
    __LOG3("CMMCScBkupState::PerformAsynchronousCancellation() - ERROR - cancellation not implemented for state: 0x%08x, IsActive: %d, status: %d", StateId().iUid, IsActive(), iStatus.Int() );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::PerformLastRightsL()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::PerformLastRightsL()
    {
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::PerformAsynchronousErrorCleanup()
// 
// 
// ---------------------------------------------------------------------------
TBool CMMCScBkupState::PerformAsynchronousErrorCleanup( TInt aError )
    {
    (void) aError;
    __LOGFILE2("CMMCScBkupState::PerformAsynchronousErrorCleanup() - error: %d, state id: 0x%08x", aError, StateId().iUid );
    return EFalse;
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::CompleteSelf()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::CompleteSelf( TInt aCompletionCode )
    {
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aCompletionCode );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::CompleteObserver()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::CompleteObserver( TInt aCompletionCode )
    {
    __ASSERT_ALWAYS(iObserver != NULL, User::Invariant());
    User::RequestComplete( iObserver, aCompletionCode );
    }


// ---------------------------------------------------------------------------
// CMMCScBkupState::SetObserver()
// 
// 
// ---------------------------------------------------------------------------
void CMMCScBkupState::SetObserver(TRequestStatus& aStatus)
    {
    __ASSERT_ALWAYS(iObserver == NULL, User::Invariant());
    aStatus = KRequestPending;
    iObserver = &aStatus;
    }







