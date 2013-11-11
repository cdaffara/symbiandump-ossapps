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
* Description:  Implements the class CSPEtelDtmfMonitor
*
*/



#include <mccpdtmfobserver.h>
#include <rmmcustomapi.h>
#include <gsmerror.h>

#include "cspeteldtmfmonitor.h"
#include "csplogger.h"
#include "cspdtmfprovider.h"
#include "cspconsts.h"
#include "cspdtmfprovider.h"



// ---------------------------------------------------------------------------
// CSPEtelDtmfMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfMonitor* CSPEtelDtmfMonitor::NewL( 
                                        CSPDTMFProvider& aObserver, 
                                        RMmCustomAPI& aMmCustom ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfMonitor::NewL()" );
    CSPEtelDtmfMonitor* self = new ( ELeave ) CSPEtelDtmfMonitor( 
                                        aObserver, aMmCustom );
    return self;    
    }


// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfMonitor::~CSPEtelDtmfMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfMonitor::~CSPEtelDtmfMonitor()" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the monitor.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPINT, 
        "CSPEtelDtmfMonitor::StartMonitoring()" );

    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPINT, 
            "CSP: CSPEtelDtmfMonitor::StartMonitoring: Request \
                    iMmCustom.NotifyDTMFEvent( iStatus, iEventData )" );
        iMmCustom.NotifyDtmfEvent( iStatus, iEventData );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelDtmfMonitor::StartMonitoring: Already active" );
        }
    }
// ---------------------------------------------------------------------------
// From CActive.
// Handles line status change notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfMonitor::RunL()
    {
    TInt err = iStatus.Int();
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelDtmfMonitor::RunL: status: %d", err );
    
    if ( err == KErrNone )
        {
        MCCPDTMFObserver::TCCPDtmfEvent event;
        if ( iEventData.iEvent == RMmCustomAPI::EDtmfStart 
             && iEventData.iType == RMmCustomAPI::EDtmfManual )
            {
            event = MCCPDTMFObserver::ECCPDtmfManualStart;
            }
        else if ( iEventData.iEvent == RMmCustomAPI::EDtmfStart 
             && iEventData.iType == RMmCustomAPI::EDtmfSequence )
            {
            event = MCCPDTMFObserver::ECCPDtmfSequenceStart;
            }
        else if ( iEventData.iEvent == RMmCustomAPI::EDtmfStop 
             && iEventData.iType == RMmCustomAPI::EDtmfManual )
            {
            event = MCCPDTMFObserver::ECCPDtmfManualStop;
            }
        else if ( iEventData.iEvent == RMmCustomAPI::EDtmfStop 
             && iEventData.iType == RMmCustomAPI::EDtmfSequence )
            {
            event = MCCPDTMFObserver::ECCPDtmfSequenceStop;
            }
        else if ( iEventData.iEvent == RMmCustomAPI::EDtmfAbort 
             && iEventData.iType == RMmCustomAPI::EDtmfManual )
            {
            event = MCCPDTMFObserver::ECCPDtmfManualAbort;
            }
        else if ( iEventData.iEvent == RMmCustomAPI::EDtmfAbort 
             && iEventData.iType == RMmCustomAPI::EDtmfSequence )
            {
            event = MCCPDTMFObserver::ECCPDtmfSequenceAbort;
            }
        else
            {
            // Unknown event
            CSPLOGSTRING3( CSPERROR, 
                "CSPEtelDtmfMonitor::RunL: Unknown event = %d, type = %d", 
                    iEventData.iEvent, iEventData.iType );
            
            // Refresh and return (no observer notfiying).
            StartMonitoring();
            return;
            }
        
        iObserver.NotifyDTMFEvent( event, err, iEventData.iTone );
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, "CSPEtelDtmfMonitor::RunL: Error \
                    from DTMF: %d", err );
        }
    
    // Continue if not in offline mode
    if ( err != KErrGsmOfflineOpNotAllowed && err != KErrCancel 
         && err != KErrNotSupported )
        {
        StartMonitoring();
        }
    CSPLOGSTRING( CSPREQEND, "CSPEtelDtmfMonitor::RunL End of RunL." );
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelDtmfMonitor::DoCancel()
    {
    if ( iStatus == KRequestPending )
        {
        CSPLOGSTRING( CSPINT, 
            "CSPEtelDtmfMonitor::DoCancel Canceling pending request.." );
        iMmCustom.CancelAsyncRequest( ECustomNotifyDtmfEventIPC ); 
        }
    }


// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelDtmfMonitor::CSPEtelDtmfMonitor(
            CSPDTMFProvider& aObserver, 
            RMmCustomAPI& aMmCustom ) : 
                CActive( EPriorityStandard ),             
                iObserver( aObserver ),
                iMmCustom( aMmCustom )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelDtmfMonitor::CSPEtelDtmfMonitor()" );
    CActiveScheduler::Add( this );
    }
    

// End of file
