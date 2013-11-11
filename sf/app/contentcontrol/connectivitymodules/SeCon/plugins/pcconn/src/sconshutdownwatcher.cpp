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
* Description:  Power shutdown -watcher implementation
*
*/


// INCLUDE FILES

#include "sconshutdownwatcher.h"
#include "debug.h"

#include <startupdomainpskeys.h>



// ============================= MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CShutdownWatcher::NewL( MShutdownObserver* aObserver )
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CShutdownWatcher* CShutdownWatcher::NewL( MShutdownObserver* aObserver )
    {
    LOGGER_ENTERFN( "CShutdownWatcher::NewL()" );
    CShutdownWatcher* self = new (ELeave) CShutdownWatcher( aObserver );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    LOGGER_LEAVEFN( "CShutdownWatcher::NewL()" );
    return self;
    }

// -----------------------------------------------------------------------------
// CShutdownWatcher::~CShutdownWatcher()
// Destructor
// -----------------------------------------------------------------------------
//
CShutdownWatcher::~CShutdownWatcher()
    {
    LOGGER_ENTERFN( "CShutdownWatcher::~CShutdownWatcher()" );
    Cancel();
    iProperty.Close();
    LOGGER_LEAVEFN( "CShutdownWatcher::~CShutdownWatcher()" );
    }

// -----------------------------------------------------------------------------
// CShutdownWatcher::StartShutdownWatcher()
// Starts to watch shutdown status
// -----------------------------------------------------------------------------
//
void CShutdownWatcher::StartShutdownWatcher()
    {
    LOGGER_ENTERFN( "CShutdownWatcher::StartShutdownWatcher()" );
    TInt status;
    TInt err = iProperty.Get( KPSUidStartup, KPSGlobalSystemState, status );
    LOGGER_WRITE_2( "CShutdownWatcher::StartShutdownWatcher() :\
     err %d, KPSGlobalSystemState status %d",err, status);
    if ( err == KErrNone )
        {
        if ( status == ESwStateShuttingDown )
            {
            LOGGER_WRITE( "Call NotifyShutdown" );
            iObserver->NotifyShutdown();
            return;
            }
        }
    
    err = iProperty.Attach( KPSUidStartup, KPSGlobalSystemState );
    LOGGER_WRITE_1( "CShutdownWatcher::StartShutdownWatcher() :\
     iProperty.Attach( KPSUidStartup, KPSGlobalSystemState ) : err %d", err );
    iProperty.Subscribe( iStatus );
    LOGGER_WRITE( "CShutdownWatcher::StartShutdownWatcher() :\
     iProperty.Subscribe( iStatus ) : ok" );
    SetActive();

    LOGGER_LEAVEFN( "CShutdownWatcher::StartShutdownWatcher()" );
    }

// -----------------------------------------------------------------------------
// CShutdownWatcher::DoCancel()
// Executed when CActive is canceled.
// -----------------------------------------------------------------------------
//
void CShutdownWatcher::DoCancel()
    {
    LOGGER_ENTERFN( "CShutdownWatcher::DoCancel()" );
    iProperty.Cancel();
    LOGGER_LEAVEFN( "CShutdownWatcher::DoCancel()" );
    }

// -----------------------------------------------------------------------------
// CShutdownWatcher::RunL()
// Executed when disk watcher is triggered
// -----------------------------------------------------------------------------
//
void CShutdownWatcher::RunL()
    {
    LOGGER_ENTERFN( "CShutdownWatcher::RunL()" );
    TInt status;
    iProperty.Get( status );
    LOGGER_WRITE_1( "CShutdownWatcher::RunL() : status %d", status );
    
    if ( status == ESwStateShuttingDown )
        {
        LOGGER_WRITE( "Call NotifyShutdown" );
        iObserver->NotifyShutdown();
        return;
        }
    
    iProperty.Cancel();
    iProperty.Close();
    
    LOGGER_LEAVEFN( "CShutdownWatcher::RunL()" );
    }


// -----------------------------------------------------------------------------
// CShutdownWatcher::CShutdownWatcher( MShutdownObserver* aObserver )
// Default constructor
// -----------------------------------------------------------------------------
//
CShutdownWatcher::CShutdownWatcher( MShutdownObserver* aObserver )
: CActive(EPriorityStandard),
    iObserver(aObserver)
    {
    }

// -----------------------------------------------------------------------------
// CShutdownWatcher::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CShutdownWatcher::ConstructL()
    {
    LOGGER_ENTERFN( "CShutdownWatcher::ConstructL()" );
    CActiveScheduler::Add(this);
    LOGGER_LEAVEFN( "CShutdownWatcher::ConstructL()" );
    }

// End of file
