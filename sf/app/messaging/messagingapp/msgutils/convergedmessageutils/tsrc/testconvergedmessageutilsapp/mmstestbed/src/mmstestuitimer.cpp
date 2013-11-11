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
 */

#include "mmstestuitimer.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CTestUiTimer::CTestUiTimer(): CTimer( 5 )
    {
    period = KPeriod;
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestUiTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CTestUiTimer* CTestUiTimer::NewL()
    {
    CTestUiTimer* self = new(ELeave) CTestUiTimer();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CTestUiTimer::~CTestUiTimer()
    {
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestUiTimer::IssueRequest()
    {
    // No operation to cancel if this function is called
    iObject = NULL;
    After(period);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestUiTimer::TimeoutOperation( CActive* aObject, TTimeIntervalSeconds aTimeoutInSeconds )
    {
    iObject = aObject;
    TTimeIntervalMicroSeconds32 timeout = aTimeoutInSeconds.Int() * 1000000;
    After( timeout );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestUiTimer::DoCancel()
    {
    CTimer::DoCancel();
    if ( iObject )
        {
        iObject->Cancel();
        iObject = NULL;
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestUiTimer::RunL()
    {
    if ( iObject )
        {
        // If we timeout while holding an active object, we cancel it
        iObject->Cancel();
        iObject = NULL;
        }
    else
        {
        CActiveScheduler::Stop();
        }
    }

// ======== GLOBAL FUNCTIONS ========

