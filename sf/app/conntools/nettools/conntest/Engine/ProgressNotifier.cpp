/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: CProgressNotifier is used to notify about 
* the current state of the request
*
*/

// INCLUDE FILES
#include "ProgressNotifier.h"
#include "ProgressNotifyHandler.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CProgressNotifier::NewL(...)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CProgressNotifier* CProgressNotifier::NewL(RConnection& aConnection,
                                           MProgressNotifyHandler& aHandler)
{
    CProgressNotifier* self = CProgressNotifier::NewLC(aConnection, aHandler);
    CleanupStack::Pop(self);
    return self;
}

// ----------------------------------------------------------------------------
// CProgressNotifier::NewLC(...)
// Two-phase constructor
// ----------------------------------------------------------------------------
//
CProgressNotifier* CProgressNotifier::NewLC(RConnection& aConnection,
                                            MProgressNotifyHandler& aHandler)
{
    CProgressNotifier* self = new (ELeave) CProgressNotifier(aConnection, aHandler);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ----------------------------------------------------------------------------
// CProgressNotifier::CProgressNotifier(...)
// First-phase constructor
// ----------------------------------------------------------------------------
//
CProgressNotifier::CProgressNotifier(RConnection& aConnection,
                                     MProgressNotifyHandler& aHandler)
: CActive(EPriorityStandard),
iConnection(aConnection),
iHandler(aHandler)
{
}

// ----------------------------------------------------------------------------
// CProgressNotifier::~CProgressNotifier()
// Destructor
// ----------------------------------------------------------------------------
//
CProgressNotifier::~CProgressNotifier()
{
    Cancel();
}

// ----------------------------------------------------------------------------
// CProgressNotifier::ConstructL()
// Second-phase constructor
// ----------------------------------------------------------------------------
//
void CProgressNotifier::ConstructL()
{
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------
// Start waiting for the progress notifications.
// ---------------------------------------------------------
void CProgressNotifier::StartNotify()
{
    ASSERT(!IsActive());
	iConnection.ProgressNotification(iProgressBuf, iStatus);
	SetActive();
}

// ---------------------------------------------------------
// CProgressNotifier::RunL()
// Called when request has completed.
// ---------------------------------------------------------
//
void CProgressNotifier::RunL()
{
    // Active object request complete handler.
    if(iStatus == KErrNone)
    {
        NotifyL();
    }
    else
    {
        iHandler.ProgressNotifyError(iStatus.Int());
    }
}

// ---------------------------------------------------------
// CProgressNotifier::DoCancel()
// Cancel ongoing requests.
// ---------------------------------------------------------
//
void CProgressNotifier::DoCancel()
{
    iConnection.CancelProgressNotification();
}

// ---------------------------------------------------------
// CProgressNotifier::Notify()
// Get the current stage and issue a new request, unless 
// the interface has gone down. Inform the progress
// notification handler about a new notification.
// ---------------------------------------------------------
//
void CProgressNotifier::NotifyL()
{
    // Store the values into temporary variables, otherwise we might send incorrect
    // values to iHandler, because iProgressBuf might get immediately overwritten
    // if progress notifications are already available.
    //
    // Note! This object is also sometimes deleted in iHandler.ProgressNotifyReceived
    // call (not nice, but no other workaround possible), so order of following calls
    // is crucial.
    
    TInt stage = iProgressBuf().iStage;
    TInt error = iProgressBuf().iError;
    
    // Issue new notification
    iConnection.ProgressNotification(iProgressBuf, iStatus);
    SetActive();

    // Inform handler about notification
    iHandler.ProgressNotifyReceivedL(stage, error);
}


