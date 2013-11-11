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
* Description:  Implements the class CSPEtelLineStatusMonitor
*
*/



#include <etelmm.h>

#include "cspetellinestatusmonitor.h"
#include "mcsplinestatusobserver.h"
#include "csplogger.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// CSPEtelCallAddedMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelLineStatusMonitor* CSPEtelLineStatusMonitor::NewL( 
                                        MCSPLineStatusObserver& aObserver,
                                        RMobileLine& aLine,
                                       RCSPLineContainer::TCSPLineId aLineId )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelLineStatusMonitor::NewL()" );
    CSPEtelLineStatusMonitor* self = new ( ELeave ) CSPEtelLineStatusMonitor( 
                                        aObserver, aLine, aLineId );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelLineStatusMonitor::~CSPEtelLineStatusMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelLineStatusMonitor::~CSPEtelLineStatusMonitor()" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the monitor.
// ---------------------------------------------------------------------------
//
void CSPEtelLineStatusMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelLineStatusMonitor::StartMonitoring()" );

    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPINT, 
            "CSP: CSPEtelLineStatusMonitor::StartMonitoring: Request \
                    iLine.NotifyStatusChange( iStatus, iLineStatus )" );

        iLine.NotifyMobileLineStatusChange( iStatus, iLineStatus );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelLineStatusMonitor::StartMonitoring: Already active" );
        }
    }

// ---------------------------------------------------------------------------
// From CActive.
// Handles line status change notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelLineStatusMonitor::RunL()
    {
    CSPLOGSTRING2( CSPINT, "CSPEtelLineStatusMonitor::RunL: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        CSPLOGSTRING( CSPINT, "CSPEtelLineStatusMonitor::RunL status changed")
        iObserver.LineStatusChanged( iLine, iLineId, iLineStatus );
        
        StartMonitoring();
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelLineStatusMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelLineStatusMonitor::DoCancel" );

    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPEtelLineStatusMonitor::DoCancel Canceling pending request" );
        iLine.CancelAsyncRequest( EMobileLineNotifyMobileLineStatusChange );
        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelLineStatusMonitor::CSPEtelLineStatusMonitor(
        MCSPLineStatusObserver& aObserver,
            RMobileLine& aLine,
            RCSPLineContainer::TCSPLineId aLineId ) :
            CActive( EPriorityStandard ), 
            iLine( aLine ),
            iLineId( aLineId ),
            iObserver( aObserver )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelLineStatusMonitor::CSPEtelLineStatusMonitor()" );
    CActiveScheduler::Add( this );
    }
    

// End of file
