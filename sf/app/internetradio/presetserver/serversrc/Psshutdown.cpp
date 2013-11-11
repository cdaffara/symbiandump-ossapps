/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Shutdown timer for the preset server
*
*/


#include "psdebug.h"
#include "psshutdown.h"

const TInt KPSShutdownPriority = CActive::EPriorityStandard; // The priority of the shutdown timer.

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CPSShutdown* CPSShutdown::NewL()
    {
    CPSShutdown* self = new ( ELeave ) CPSShutdown;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CPSShutdown::CPSShutdown()
    : CTimer( KPSShutdownPriority )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second-phase constructor.
// ---------------------------------------------------------------------------
//
void CPSShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CPSShutdown::~CPSShutdown()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the shutdown timer.
// ---------------------------------------------------------------------------
//
void CPSShutdown::Start(TTimeIntervalMicroSeconds32 aDelay)
    {
    PSDEBUG3( "CPSShutdown::Start() - Should start the shutdown timer now (IsActive() = %d, aDelay = %d).", 
              IsActive(), aDelay.Int() );
    Cancel();              
    After( aDelay );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Called when the timer expires.
// ---------------------------------------------------------------------------
//
void CPSShutdown::RunL()
    {
    PSDEBUG( "CPSShutdown::RunL() >> Timer expired. Stopping the preset server now!" );
    CActiveScheduler::Stop();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Called when the timer expires.
// ---------------------------------------------------------------------------
//
void CPSShutdown::DoCancel()
    {
    PSDEBUG( "CPSShutdown::DoCancel() >> Timer halted." );
    CTimer::DoCancel();
    }

// ======== GLOBAL FUNCTIONS ========
