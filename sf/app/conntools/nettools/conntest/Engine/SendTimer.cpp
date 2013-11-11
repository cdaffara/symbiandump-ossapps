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
* Description: CSendTimer is used for sending data at specified intervals
*
*/

#include "SendTimer.h"
#include "SocketsEngine.h"

// ---------------------------------------------------------
// CSendTimer::NewL(const TInt aPriority, CSocketsEngine* aEngine)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSendTimer* CSendTimer::NewL(const TInt aPriority, CSocketsEngine* aEngine)
{
    CSendTimer* self = CSendTimer::NewLC(aPriority, aEngine);
    CleanupStack::Pop(self);
    return self;
}


// ---------------------------------------------------------
// CSendTimer::NewLC(const TInt aPriority, CConnTestView* aView)
// EPOC two phased constructor
// ---------------------------------------------------------
//
CSendTimer* CSendTimer::NewLC(const TInt aPriority, CSocketsEngine* aEngine)
{
    CSendTimer* self = new (ELeave) CSendTimer(aPriority, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}


// ---------------------------------------------------------
// CSendTimer::CSendTimer(const TInt aPriority, CConnTestView* aView)
// Constructor
// ---------------------------------------------------------
//
CSendTimer::CSendTimer(const TInt aPriority, CSocketsEngine* aEngine)
: CTimer(aPriority), iEngine(aEngine)
{
}


// ---------------------------------------------------------
// CSendTimer::~CSendTimer()
// Destructor
// ---------------------------------------------------------
//
CSendTimer::~CSendTimer()
{
    Cancel();
}


// ---------------------------------------------------------
// CSendTimer::ConstructL()
// EPOC two-phased constructor
// ---------------------------------------------------------
//
void CSendTimer::ConstructL()
{
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
}

// ---------------------------------------------------------
// CSendTimer::RunL()
// Timer event has been generated and timer request completed.
// ---------------------------------------------------------
//
void CSendTimer::RunL()
{
    // Timer request has completed, so notify the timer's owner
    iEngine->DoSendDataL();    
}

