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
* Description:  Implementation of class CSPEtelCallStatusMonitor which
*                monitors call status changes from ETel and notifies observer
*                according to call status change.
*
*/


#include <mccpcallobserver.h>

#include "cspetelcallstatusmonitor.h"
#include "mcspcallobserver.h"
#include "csplogger.h"



// ---------------------------------------------------------------------------
// CSPEtelCallEventMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelCallStatusMonitor* CSPEtelCallStatusMonitor::NewL( MCSPCallObserver& aObserver,
                                                        RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallEventMonitor::NewL()" );
    CSPEtelCallStatusMonitor* self = new ( ELeave ) CSPEtelCallStatusMonitor( 
                                        aObserver, aCall );
    return self;    
    }


// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelCallStatusMonitor::~CSPEtelCallStatusMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallStatusMonitor::~CSPEtelCallStatusMonitor()" );
    Cancel();
       
    if ( iDestrPtr )
        {
        *iDestrPtr = ETrue;
        iDestrPtr = NULL;
        }
        
    }


// ---------------------------------------------------------------------------
// Starts the monitoring.
// ---------------------------------------------------------------------------
//
void CSPEtelCallStatusMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelCallStatusMonitor::StartMonitoring()" );
    if ( !IsActive() && iCall.SubSessionHandle() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelCallStatusMonitor::StartMonitoring: Request \
                    RMobilePhone::NotifyMobileCallStatusChange" );
        iCall.NotifyMobileCallStatusChange( iStatus, iCallStatus );
        CSPLOGSTRING( CSPREQOUT, "CSPEtelCallStatusMonitor:: Notify OK" );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelCallStatusMonitor::StartMonitoring: Already active" );
        }
    }


// ---------------------------------------------------------------------------
// Starts the monitoring.
// ---------------------------------------------------------------------------
//    
RMobileCall::TMobileCallStatus CSPEtelCallStatusMonitor::State()
    {
    return iCallStatus;
    }
    
// ---------------------------------------------------------------------------
// From CActive
// Handles call status notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelCallStatusMonitor::RunL()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelCallStatusMonitor::RunL")   
    
    // Survive from monitor destruction during observing sequence
    TBool destroyed = EFalse;
    iDestrPtr = &destroyed;
     
    if ( iStatus == KErrNone )
        {
        iObserver.NotifyCallStateChangedETel( iCallStatus );
        }
    
    if ( !destroyed )
        {        
        // In case instance has not been deleted, pointer must be cleared.
        iDestrPtr = NULL;
        if ( iStatus == KErrNone )
            {
            StartMonitoring();
            }
        }
    else
        {
        // already destroyed, do not touch members.
        }
    }


// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelCallStatusMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelCallStatusMonitor::DoCancel" );
    iCall.CancelAsyncRequest( EMobileCallNotifyMobileCallStatusChange );
    }



    
// ---------------------------------------------------------------------------
// Constructs the monitor..
// ---------------------------------------------------------------------------
//
CSPEtelCallStatusMonitor::CSPEtelCallStatusMonitor( MCSPCallObserver& aObserver,
                                                    RMobileCall& aCall ) : 
                                   CActive( EPriorityStandard ),
                                   iObserver( aObserver ),
                                   iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallStatusMonitor::CSPEtelCallStatusMonitor()" );
    CActiveScheduler::Add( this );
    }
    


// End of file
