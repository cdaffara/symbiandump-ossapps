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
* Description:  Implementation of class CSPEtelConferenceEventMonitor which
*                monitors call events from ETel and notifies observer
*                accordingly.
*
*/


#include "cspetelconferenceeventmonitor.h"

#include <mccpconferencecallobserver.h>

#include "csplogger.h"
#include "cspconferencecall.h"



// ---------------------------------------------------------------------------
// CSPEtelConferenceEventMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceEventMonitor* CSPEtelConferenceEventMonitor::NewL( 
                                                    CSPConferenceCall& aOwner,
                                                    RMobileConferenceCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceEventMonitor::NewL()" );
    CSPEtelConferenceEventMonitor* self = 
                    new ( ELeave ) CSPEtelConferenceEventMonitor( 
                                        aOwner, aCall );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;    
    }



// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceEventMonitor::~CSPEtelConferenceEventMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceEventMonitor::~CSPEtelConferenceEventMonitor()" );
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
void CSPEtelConferenceEventMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPOBJECT, 
          "CSPEtelConferenceEventMonitor::StartMonitoring()" );

    
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelConferenceEventMonitor::StartMonitoring: \
                    Request RMobilePhone::NotifyEvent" );
        
        iCall.NotifyConferenceEvent( iStatus, iEvent, iCallName );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelConferenceEventMonitor::StartMonitoring: Already active" );
        }
    }
    

// ---------------------------------------------------------------------------
// From CActive
// Handles event notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceEventMonitor::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelConferenceEventMonitor::RunL: status: %d", iStatus.Int() );
    
    // Survive from monitor destruction during observing sequence
    TBool destroyed = EFalse;
    iDestrPtr = &destroyed;

    
    if ( iStatus == KErrNone )
        {
        CSPLOGSTRING2( CSPINT, 
           "CSP CSPEtelConferenceEventMonitor::RunL  Event related callname = %S", 
            &iCallName );

        switch ( iEvent )
            {
            case RMobileConferenceCall::EConferenceCallAdded:
                {
                CSPLOGSTRING( CSPINT, 
                   "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceCallAdded" );
                iOwner.NotifyEvent( 
                    MCCPConferenceCallObserver::ECCPConferenceCallAdded, 
                    iCallName );
                    
                break;
                }
             // The call has been placed on hold as a result of a local action
            case RMobileConferenceCall::EConferenceCallRemoved:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceCallRemoved" );
                iOwner.NotifyEvent( 
                    MCCPConferenceCallObserver::ECCPConferenceCallRemoved,
                    iCallName );
                break;
                }
                
            // The call has been resumed as a result of a local action. 
            case RMobileConferenceCall::EConferenceBuilt:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceBuilt" );
                iOwner.NotifyEvent( 
                        MCCPConferenceCallObserver::ECCPConferenceBuilt );
                break;
                }
            
            case RMobileConferenceCall::EConferenceTerminated:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceTerminated" );
                iOwner.NotifyEvent( 
                        MCCPConferenceCallObserver::ECCPConferenceTerminated );
                break;
                }
            
            case RMobileConferenceCall::EConferenceSwapped:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceSwapped" );
                iOwner.NotifyEvent( 
                        MCCPConferenceCallObserver::ECCPConferenceSwapped );
                break;
                }
            
            case RMobileConferenceCall::EConferenceSplit:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ECCPConferenceSplit" );
                iOwner.NotifyEvent( 
                        MCCPConferenceCallObserver::ECCPConferenceSplit,
                        iCallName );
                break;
                }

            default:
                {
                CSPLOGSTRING2( CSPERROR, 
                    "CSP CSPEtelConferenceEventMonitor::RunL: ERROR Unspecified \
                     call event: %d", iEvent );
                break;
                }
                
            }
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, 
            "CSP CSPEtelConferenceEventMonitor::RunL err %d", iStatus.Int() );
        }
        
    if ( !destroyed )
        {
        // In case instance has not been deleted, it is important to clear
        // iPtr pointer.
        iDestrPtr = NULL;
        
        // ok to modify member variables.
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
// Request canceling.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceEventMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileConferenceCallNotifyConferenceEvent );
    }

    
// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceEventMonitor::CSPEtelConferenceEventMonitor( CSPConferenceCall& aOwner,
                                                  RMobileConferenceCall& aCall ) : 
                                   CActive( EPriorityStandard ), 
                                   iOwner( aOwner ),
                                   iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceEventMonitor::CSPEtelConferenceEventMonitor()" );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceEventMonitor::ConstructL()
    {
    // Implementation not required.
    }


// End of file
