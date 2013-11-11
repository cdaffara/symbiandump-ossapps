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
 * Description:  Timer service
 *
 */

#include "csptimer.h"
#include "mcsptimerobserver.h"
#include "csppanic.pan"

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CSPTimer* CSPTimer::NewL()
    {
    CSPTimer* self = CSPTimer::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CSPTimer* CSPTimer::NewLC()
    {
    CSPTimer* self = new (ELeave) CSPTimer;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSPTimer::~CSPTimer()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts notify request
// ---------------------------------------------------------------------------
//
void CSPTimer::NotifyAfter(TInt aTimeout, MCSPTimerObserver& aTimerObserver)
    {
    __ASSERT_ALWAYS(!IsNotifyOngoing(), Panic(ECSPPanicTimerAlreadyActive));
    After(aTimeout);
    iObserver = &aTimerObserver;
    }

// ---------------------------------------------------------------------------
// Cancels the outstancing request.
// ---------------------------------------------------------------------------
//
void CSPTimer::CancelNotify()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Determines if there is active request outstanding.
// ---------------------------------------------------------------------------
//
TBool CSPTimer::IsNotifyOngoing()
    {
    return IsActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Notifies the observer about request.
// ---------------------------------------------------------------------------
//
void CSPTimer::RunL()
    {
    if (iObserver)
        {
        iObserver->TimerEvent();
        }
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSPTimer::CSPTimer() :
    CTimer(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CSPTimer::ConstructL()
    {
    CTimer::ConstructL();
    }

// End of File

