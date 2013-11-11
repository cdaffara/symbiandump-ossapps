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
* Description:  Monitor UUI messages.
*
*/

#include "csplogger.h"
#include "cspuuimonitor.h"
#include "mcspuusmessageobserver.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSPUUIMonitor::CSPUUIMonitor(
    RMobileCall& aCall, 
    MCSPUUSMessageObserver& aUUIHandler ) 
    : CActive( CActive::EPriorityStandard ),
    iCall( aCall ),
    iObserver( aUUIHandler )
    {
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CSPUUIMonitor::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// Static constructor
// ---------------------------------------------------------------------------
//
CSPUUIMonitor* CSPUUIMonitor::NewL(
    RMobileCall& aCall,
    MCSPUUSMessageObserver& aUUIHandler )
    {
    CSPUUIMonitor* self = 
        new( ELeave ) CSPUUIMonitor( aCall, aUUIHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSPUUIMonitor::~CSPUUIMonitor()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts monitoring the UUI messages.
// ---------------------------------------------------------------------------
//
void CSPUUIMonitor::StartMonitor()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPUUIMonitor::StartMonitor");
    Cancel();
    iCall.ReceiveUUI( iStatus, iUUI ); 
    SetActive();
    }

// ---------------------------------------------------------------------------
// From class CActive
// Cancels out standign request
// ---------------------------------------------------------------------------
//
void CSPUUIMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileCallReceiveUUI ) ;
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// Notifies observer.
// ---------------------------------------------------------------------------
//
void CSPUUIMonitor::RunL()
    {
    CSPLOGSTRING2(CSPINT, "CSPUUIMonitor::RunL %d iStatus", iStatus.Int() );
    if( iStatus == KErrNone )
        {
        iObserver.UUSMessageReceived( iUUI );
        StartMonitor();
        }
    else if( iStatus != KErrNotSupported )
        {
        StartMonitor();
        }
    }
