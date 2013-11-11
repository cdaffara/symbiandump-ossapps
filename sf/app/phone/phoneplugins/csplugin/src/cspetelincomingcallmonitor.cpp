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
* Description:  Implements the class CSPEtelIncomingCallMonitor
*
*/


#include "cspetelincomingcallmonitor.h"
#include "csplogger.h"
#include "cspcall.h"
#include "cspconsts.h"


// ---------------------------------------------------------------------------
// CSPEtelIncomingCallMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelIncomingCallMonitor* CSPEtelIncomingCallMonitor::NewL( 
                                        MCSPIncomingCallObserver& aObserver, 
                                        RMobileLine& aLine,
                                       RCSPLineContainer::TCSPLineId aLineId )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelIncomingCallMonitor::NewL <");
    CSPEtelIncomingCallMonitor* self = new ( ELeave ) CSPEtelIncomingCallMonitor( 
                                        aObserver, aLine, aLineId );
    CSPLOGSTRING(CSPOBJECT, "CSPEtelIncomingCallMonitor::NewL >");
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelIncomingCallMonitor::~CSPEtelIncomingCallMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelIncomingCallMonitor::~CSPEtelIncomingCallMonitor" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the monitor.
// ---------------------------------------------------------------------------
//
void CSPEtelIncomingCallMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelIncomingCallMonitor::StartMonitoring");

    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPEtelIncomingCallMonitor::StartMonitoring NotifyIncomingCall");
        iCallName.Zero();
        iLine.NotifyIncomingCall( iStatus, iCallName );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSPEtelIncomingCallMonitor::StartMonitoring Error already active");
        }
    }
// ---------------------------------------------------------------------------
// From CActive.
// Handles line status change notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelIncomingCallMonitor::RunL()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelIncomingCallMonitor::RunL <")
    
    if ( iStatus == KErrNone )
        {
        iIncomingCallObserver.IncomingCallArrived( iLine, iCallName,
                                                       iLineId );
        // Continue monitoring
        StartMonitoring();        
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, "CSPEtelIncomingCallMonitor::RunL: Error \
                    from RMobileLine: %d", iStatus.Int() );
        }
    
    CSPLOGSTRING(CSPREQEND, "CSPEtelIncomingCallMonitor::RunL >");
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelIncomingCallMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelIncomingCallMonitor::DoCancel" );

    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPEtelIncomingCallMonitor::DoCancel Canceling pending request.." );
        iLine.NotifyIncomingCallCancel();
        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelIncomingCallMonitor::CSPEtelIncomingCallMonitor(
            MCSPIncomingCallObserver& aObserver, 
            RMobileLine& aLine,
            RCSPLineContainer::TCSPLineId aLineId ) : 
            CActive( EPriorityStandard + 2 ),
            iIncomingCallObserver( aObserver ), 
            iLine( aLine ),
            iLineId( aLineId )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelIncomingCallMonitor::CSPEtelIncomingCallMonitor" );
    CActiveScheduler::Add( this );
    }

// End of File
