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
* Description: CTimeOutTimer is used to generate time out events
*
*/

#include "TimeOutTimer.h"
#include "TimeOutNotify.h"


// ---------------------------------------------------------
// CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
{
    CTimeOutTimer* self = CTimeOutTimer::NewLC(aPriority, aTimeOutNotify);
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------
// CTimeOutTimer::NewLC(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewLC(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
{
    CTimeOutTimer* self = new (ELeave) CTimeOutTimer(aPriority, aTimeOutNotify);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------
// CTimeOutTimer::CTimeOutTimer(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
// Constructor
// ---------------------------------------------------------
//
CTimeOutTimer::CTimeOutTimer(const TInt aPriority, MTimeOutNotify& aTimeOutNotify)
: CTimer(aPriority), iNotify(aTimeOutNotify)
{
}

// ---------------------------------------------------------
// CTimeOutTimer::~CTimeOutTimer()
// Destructor
// ---------------------------------------------------------
//
CTimeOutTimer::~CTimeOutTimer()
{
    Cancel();
}

// ---------------------------------------------------------
// CTimeOutTimer::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CTimeOutTimer::ConstructL()
{
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------
// CTimeOutTimer::RunL()
// Timer event has been generated and timer request completed.
// ---------------------------------------------------------
//
void CTimeOutTimer::RunL()
{
    // Timer request has completed, so notify the timer's owner
    iNotify.TimerExpired();
}

