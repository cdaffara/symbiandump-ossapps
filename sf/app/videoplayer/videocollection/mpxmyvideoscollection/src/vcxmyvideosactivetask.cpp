/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Active object to split up long running db tasks*
*/



// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include "vcxmyvideosactivetask.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask::CVcxMyVideosActiveTask( MVcxMyVideosActiveTaskObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask* CVcxMyVideosActiveTask::NewL( MVcxMyVideosActiveTaskObserver& aObserver )
    {
    CVcxMyVideosActiveTask* self = new(ELeave) CVcxMyVideosActiveTask( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosActiveTask::~CVcxMyVideosActiveTask()
    {
    Cancel();
    delete iCurCommand;
    }

// ---------------------------------------------------------------------------
// Start the operation
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::StartL( TMPXCommandId aTask, const CMPXCommand& aCommand )
    {
    delete iCurCommand;
    iCurCommand = NULL;
    iCurCommand = CMPXMedia::NewL( aCommand );
    iCurTask = aTask;
    iCurStep = 0;

    // Start the AO
    iStatus = KRequestPending;
    MPX_DEBUG1("CVcxMyVideosActiveTask::StartL SetActive");
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// Get the current step
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosActiveTask::GetStep()
    {
    return iCurStep;
    }

// ---------------------------------------------------------------------------
// Get the current task
// ---------------------------------------------------------------------------
//
TMPXCommandId CVcxMyVideosActiveTask::GetTask()
    {
    return iCurTask;
    }

// ---------------------------------------------------------------------------
// Get the current media
// ---------------------------------------------------------------------------
//
CMPXMedia& CVcxMyVideosActiveTask::GetCommand()
    {
    return *iCurCommand;
    }

// ---------------------------------------------------------------------------
// Get the current media
// ---------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosActiveTask::Command()
    {
    return iCurCommand;
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::RunL()
    {
    MPX_DEBUG1("CVcxMyVideosActiveTask::RunL() start");

    switch ( iObserver.HandleStepL() )
        {
        case MVcxMyVideosActiveTaskObserver::EDone:
            Done();
            break;

        case MVcxMyVideosActiveTaskObserver::EMoreToCome:
            ContinueStepping();
            break;

        //observer is responsible for calling Done, Cancel or ContinueStepping
        case MVcxMyVideosActiveTaskObserver::EStopStepping:
            MPX_DEBUG1("CVcxMyVideosActiveTask::RunL stopped stepping");
            iStopped = ETrue;
            break;
        }
    MPX_DEBUG1("CVcxMyVideosActiveTask::RunL() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::Done
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::Done()
    {
    MPX_DEBUG1("CVcxMyVideosActiveTask::Done() start");

    iObserver.HandleOperationCompleted( KErrNone );
    delete iCurCommand;
    iCurCommand = NULL;
    iStopped = EFalse;

    MPX_DEBUG1("CVcxMyVideosActiveTask::Done() exit");
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosActiveTask::ContinueStepping
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::ContinueStepping()
    {
    ++iCurStep;
    iStatus = KRequestPending;
    MPX_DEBUG1("CVcxMyVideosActiveTask::ContinueStepping SetActive");
    SetActive();
    iStopped = EFalse;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

void CVcxMyVideosActiveTask::Cancel()
    {
    if ( iStopped && !IsActive() )
        {
        MPX_DEBUG1("CVcxMyVideosActiveTask:: was paused, calling DoCancel()");
        DoCancel();
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosActiveTask:: wasn't paused, calling CActive::Cancel() normally");
        CActive::Cancel();
        }
    }
// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CVcxMyVideosActiveTask::DoCancel()
    {
    MPX_DEBUG1("CVcxMyVideosActiveTask::DoCancel() start");
    // Callback and cleanup
    iObserver.HandleOperationCompleted( KErrCancel );
    delete iCurCommand;
    iCurCommand = NULL;
    iStopped = EFalse;
    MPX_DEBUG1("CVcxMyVideosActiveTask::DoCancel() exit");
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
TInt CVcxMyVideosActiveTask::RunError( TInt aError )
    {
    MPX_DEBUG1("CVcxMyVideosActiveTask::RunError() start");
    
    // Callback and cleanup
    iObserver.HandleOperationCompleted( aError );
    delete iCurCommand;
    iCurCommand = NULL;
    iStopped = EFalse;

    MPX_DEBUG1("CVcxMyVideosActiveTask::RunError() exit");
    return KErrNone;
    }

// END OF FILE
