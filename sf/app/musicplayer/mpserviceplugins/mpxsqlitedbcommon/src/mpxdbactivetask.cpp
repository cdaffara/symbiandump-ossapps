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
* Description:  Active object to split up long running db tasks
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include "mpxdbactivetask.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMPXDbActiveTask::CMPXDbActiveTask( MMPXDbActiveTaskObserver& aObserver ) :
    CActive( CActive::EPriorityStandard ),
    iObserver( aObserver )
    {
    MPX_FUNC("CMPXDbActiveTask::CMPXDbActiveTask");
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CMPXDbActiveTask::ConstructL()
    {
    MPX_FUNC("CMPXDbActiveTask::ConstructL");
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXDbActiveTask* CMPXDbActiveTask::NewL( MMPXDbActiveTaskObserver& aObserver )
    {
    MPX_FUNC("CMPXDbActiveTask::NewL");

    CMPXDbActiveTask* self = new(ELeave) CMPXDbActiveTask( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXDbActiveTask::~CMPXDbActiveTask()
    {
    MPX_FUNC("CMPXDbActiveTask::~CMPXDbActiveTask");

    Cancel();
    delete iChangeMessages;
    delete iCurCommand;
    }

// ---------------------------------------------------------------------------
// Start the operation
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXDbActiveTask::StartL( TMPXCommandId aTask, const CMPXCommand& aCommand )
    {
    delete iCurCommand;
    iCurCommand = NULL;
    iCurCommand = CMPXMedia::NewL( aCommand );
    iCurTask = aTask;
    iCurStep = 0;
    delete iChangeMessages;
    iChangeMessages = NULL;
    iChangeMessages = CMPXMessageArray::NewL();
    iCurVisibleChange = ENotVisibile;

    // Start the AO
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    }

// ---------------------------------------------------------------------------
// Get the current step
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbActiveTask::GetStep()
    {
    MPX_FUNC("CMPXDbActiveTask::GetStep");
    return iCurStep;
    }

// ---------------------------------------------------------------------------
// Get the current task
// ---------------------------------------------------------------------------
//
EXPORT_C TMPXCommandId CMPXDbActiveTask::GetTask()
    {
    MPX_FUNC("CMPXDbActiveTask::GetTask");
    return iCurTask;
    }

// ---------------------------------------------------------------------------
// Get the current media
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMedia& CMPXDbActiveTask::GetCommand()
    {
    MPX_FUNC("CMPXDbActiveTask::GetCommand");
    return *iCurCommand;
    }

// ---------------------------------------------------------------------------
// Get the current change messages
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXMessageArray& CMPXDbActiveTask::GetChangeMessages()
    {
    MPX_FUNC("CMPXDbActiveTask::GetChangeMessages");
    return *iChangeMessages;
    }

// ---------------------------------------------------------------------------
// Set the visible change flag
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXDbActiveTask::SetVisibleChange( TChangeVisibility aChange )
    {
    MPX_FUNC("CMPXDbActiveTask::SetVisibleChange");
    iCurVisibleChange = aChange;
    }

// ---------------------------------------------------------------------------
// Get the visible change flag
// ---------------------------------------------------------------------------
//
EXPORT_C CMPXDbActiveTask::TChangeVisibility CMPXDbActiveTask::GetVisibleChange()
    {
    MPX_FUNC("CMPXDbActiveTask::GetVisibleChange");
    return iCurVisibleChange;
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXDbActiveTask::RunL()
    {
    MPX_FUNC("CMPXDbActiveTask::RunL");

    // ETrue is done, EFalse is more to do
    //
    if( iObserver.HandleStepL() )
        {
        iObserver.HandleOperationCompleted( KErrNone );
        delete iCurCommand;
        iCurCommand = NULL;
        delete iChangeMessages;
        iChangeMessages = NULL;
        }
    else
        {
        ++iCurStep;
        iStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
EXPORT_C void CMPXDbActiveTask::DoCancel()
    {
    MPX_FUNC("CMPXDbActiveTask::DoCancel");

    // Callback and cleanup
    iObserver.HandleOperationCompleted( KErrCancel );
    delete iCurCommand;
    iCurCommand = NULL;
    iChangeMessages->Reset();
    }

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CMPXDbActiveTask::RunError( TInt aError )
    {
    MPX_FUNC("CMPXDbActiveTask::RunError");

    // Callback and cleanup
    iObserver.HandleOperationCompleted( aError );
    delete iCurCommand;
    iCurCommand = NULL;
    iChangeMessages->Reset();

    return KErrNone;
    }

// END OF FILE
