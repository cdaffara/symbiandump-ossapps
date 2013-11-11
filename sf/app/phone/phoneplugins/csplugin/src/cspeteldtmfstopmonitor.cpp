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
* Description:  Implements the class CSPEtelDtmfStopMonitor
*
*/



#include <mccpdtmfobserver.h>
#include <gsmerror.h>

#include "cspeteldtmfstopmonitor.h"
#include "csplogger.h"
#include "cspdtmfprovider.h"
#include "cspconsts.h"
#include "cspdtmfprovider.h"


// ---------------------------------------------------------------------------
// CSPEtelDtmfStopMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfStopMonitor* CSPEtelDtmfStopMonitor::NewL( 
                                        CSPDTMFProvider& aObserver, 
                                        RMobilePhone& aPhone ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfStopMonitor::NewL()" );
    CSPEtelDtmfStopMonitor* self = new ( ELeave ) CSPEtelDtmfStopMonitor( 
                                        aObserver, aPhone );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfStopMonitor::~CSPEtelDtmfStopMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfStopMonitor::~CSPEtelDtmfStopMonitor()" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the monitor.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfStopMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPINT, "CSPEtelDtmfStopMonitor::StartMonitoring()" );

    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPINT, 
            "CSP: CSPEtelDtmfStopMonitor::StartMonitoring: Request \
                    iPhone.NotifyStopInDTMFString( iStatus, iEventData )" );
        iPhone.NotifyStopInDTMFString( iStatus );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelDtmfStopMonitor::StartMonitoring: Already active" );
        }
    }
// ---------------------------------------------------------------------------
// From CActive.
// Handles line status change notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfStopMonitor::RunL()
    {
    TInt err = iStatus.Int();
    CSPLOGSTRING2( CSPINT, 
        "CSPEtelDtmfStopMonitor::RunL: status: %d", err );
    
    if ( err != KErrCancel &&  err != KErrServerTerminated )
        {
        
        TChar tone = NULL;
        MCCPDTMFObserver::TCCPDtmfEvent event = 
                MCCPDTMFObserver::ECCPDtmfStopInDtmfString;
        iObserver.NotifyDTMFEvent( event, err, tone );
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, "CSPEtelDtmfStopMonitor::RunL: Error \
                    from RMobilePhone: %d", iStatus.Int() );
        }
    
    
    // Continue if not in offline mode
    if ( err != KErrGsmOfflineOpNotAllowed && err != KErrCancel
        && err != KErrNotSupported )
        {
        StartMonitoring();
        }
    CSPLOGSTRING( CSPREQEND, "CSPEtelDtmfStopMonitor::RunL End of RunL." );
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfStopMonitor::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPEtelDtmfStopMonitor::DoCancel Canceling pending request.." );
        iPhone.CancelAsyncRequest( EMobilePhoneNotifyStopInDTMFString );

        }
    }

// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfStopMonitor::CSPEtelDtmfStopMonitor(
            CSPDTMFProvider& aObserver, 
            RMobilePhone& aPhone ) : 
            CActive( EPriorityStandard ),             
            iObserver( aObserver ),
            iPhone( aPhone )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfStopMonitor::CSPEtelDtmfStopMonitor()" );
    CActiveScheduler::Add( this );
    }

// End of file
