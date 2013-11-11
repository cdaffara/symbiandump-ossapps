/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*             This class watches for changes to the ECOM registry
*             and forces a recheck of Meeting request availablity
*
*/


// ----------------------------------------------------------------------------
// INCLUDE FILES
// ----------------------------------------------------------------------------
//
#include "CalenEcomWatcher.h"
#include "CalenInterimUtils2Impl.h"
#include "calenecomchangeobserver.h"

//debug
#include "calendarengines_debug.h"

// -----------------------------------------------------------------------------
// CCalenEComWatcher::NewL()
// Creates and returns a new CCalenEComWatcher object.
// -----------------------------------------------------------------------------
EXPORT_C CCalenEComWatcher* CCalenEComWatcher::NewL( MCalenEComChangeObserver& aObserver )
    {
    TRACE_ENTRY_POINT;
    
    CCalenEComWatcher* self = new(ELeave) CCalenEComWatcher( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    TRACE_EXIT_POINT;
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::CCalenEcomWatcher()
// Default constructor
// -----------------------------------------------------------------------------
CCalenEComWatcher::CCalenEComWatcher( MCalenEComChangeObserver& aObserver )
    : CActive(EPriorityNormal), iEComObserver( aObserver )
    {
    TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::ConstructL()
// Second phase construction.  Opens the ecom session and requests notifications
// -----------------------------------------------------------------------------
void CCalenEComWatcher::ConstructL()
    {
    TRACE_ENTRY_POINT;
    
    iSession = REComSession::OpenL();
    iSession.NotifyOnChange(iStatus);
    CActiveScheduler::Add(this);
    SetActive();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::~CCalenEComWatcher()
// Destructor
// -----------------------------------------------------------------------------
CCalenEComWatcher::~CCalenEComWatcher()
    {
    TRACE_ENTRY_POINT;
    
    Cancel();
    iSession.Close();
    REComSession::FinalClose();
    
    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::RunL()
// From CActive
// Called whenever a change to the Ecom registry occurs
// -----------------------------------------------------------------------------
void CCalenEComWatcher::RunL()
    {
    TRACE_ENTRY_POINT;

    // Force a recheck of meeting request availability. Ignore the return value.
    iEComObserver.EComChanged();
    iSession.NotifyOnChange(iStatus);
    SetActive();

    TRACE_EXIT_POINT;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::RunError()
// From CActive
// This is a simple watcher class.  If RunL leaves then meeting request support 
// will be disabled anyway, at least until the next ecom notification occurs.  
// If we can't even retrieve a list of mailboxes without leaving we've got no 
// chance of displaying the viewers correctly.
// Swallow the error.
// -----------------------------------------------------------------------------
TInt CCalenEComWatcher::RunError()
    {
    TRACE_ENTRY_POINT;

    iSession.NotifyOnChange(iStatus);
    SetActive();

    TRACE_EXIT_POINT;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCalenEComWatcher::DoCancel()
// From CActive
// Stop watching the ecom session
// -----------------------------------------------------------------------------
void CCalenEComWatcher::DoCancel()
    {
    TRACE_ENTRY_POINT;

    iSession.CancelNotifyOnChange(iStatus);

    TRACE_EXIT_POINT;
    }

// End of file
