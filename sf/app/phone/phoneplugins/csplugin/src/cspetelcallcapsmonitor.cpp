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
* Description:  Implementation of class CSPEtelCallCapsMonitor which
*                monitors call status changes from ETel and notifies observer
*                according to call status change.
*
*/


#include "cspetelcallcapsmonitor.h"

#include <mccpcallobserver.h>

#include "mcspcallobserver.h"
#include "csplogger.h"


// ---------------------------------------------------------------------------
// Constructs the monitor in two phase.
// ---------------------------------------------------------------------------
//
CSPEtelCallCapsMonitor* CSPEtelCallCapsMonitor::NewL( 
                                                    MCSPCallObserver& aObserver,
                                                    RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallCapsMonitor::NewL()" );
    CSPEtelCallCapsMonitor* self = 
                        new ( ELeave ) CSPEtelCallCapsMonitor( 
                                                            aObserver, aCall );
    return self;    
    }


// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelCallCapsMonitor::~CSPEtelCallCapsMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallCapsMonitor::~CSPEtelCallCapsMonitor()" );
    Cancel();
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallCapsMonitor::~CSPEtelCallCapsMonitor() ok" );

    }


// ---------------------------------------------------------------------------
// Starts the monitoring.
// ---------------------------------------------------------------------------
//
void CSPEtelCallCapsMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPOBJECT, 
          "CSPEtelCallCapsMonitor::StartMonitoring()" );

    
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSPEtelCallCapsMonitor::StartMonitoring: Request \
                    RMobilePhone::NotifyCapsChange" );
        iCall.NotifyMobileCallCapsChange( iStatus, iCapsPckg );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSPEtelCallCapsMonitor::StartMonitoring: Already active" );
        }
    }
    
// ---------------------------------------------------------------------------
// From CActive
// Handles call status notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelCallCapsMonitor::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelCallCapsMonitor::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        TUint32 capsFlags = iCaps.iCallControlCaps;
        
        iObserver.CallCapsChanged( capsFlags );
        StartMonitoring();
        }
    
    }


// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelCallCapsMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileCallNotifyMobileCallCapsChange );
    }



    
// ---------------------------------------------------------------------------
// Constructs the monitor..
// ---------------------------------------------------------------------------
//
CSPEtelCallCapsMonitor::CSPEtelCallCapsMonitor( MCSPCallObserver& aObserver,
                                                    RMobileCall& aCall ) : 
                                   CActive( EPriorityStandard ),
                                   iObserver( aObserver ),
                                   iCall ( aCall ),
                                   iCapsPckg( iCaps )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallCapsMonitor::CSPEtelCallCapsMonitor()" );
    CActiveScheduler::Add( this );
    }
    

TUint32 CSPEtelCallCapsMonitor::FetchCallControlCapsL()
    {
    TInt err = iCall.GetMobileCallCaps( iCapsPckg );
    User::LeaveIfError( err );
    return iCaps.iCallControlCaps;
    }


// End of file
