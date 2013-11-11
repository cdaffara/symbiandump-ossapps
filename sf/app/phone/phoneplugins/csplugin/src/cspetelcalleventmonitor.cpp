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
* Description:  Implementation of class CSPEtelCallEventMonitor which
*                monitors call events from ETel and notifies observer
*                accordingly.
*
*/


#include <mccptransferobserver.h>
#include <mccpforwardobserver.h>
#include <mccpconferencecallobserver.h>

#include "cspetelcalleventmonitor.h"
#include "csplogger.h"
#include "cspcall.h"



// ---------------------------------------------------------------------------
// CSPEtelCallEventMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelCallEventMonitor* CSPEtelCallEventMonitor::NewL( MCSPCallObserver& aObserver,
                                                        RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallEventMonitor::NewL()" );
    CSPEtelCallEventMonitor* self = new ( ELeave ) CSPEtelCallEventMonitor( 
                                        aObserver, aCall );
    return self;    
    }

// ---------------------------------------------------------------------------
// Destructs the object by canceling first ongoing monitoring.
// ---------------------------------------------------------------------------
//
CSPEtelCallEventMonitor::~CSPEtelCallEventMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallEventMonitor::~CSPEtelCallEventMonitor()" );
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
void CSPEtelCallEventMonitor::StartMonitoring()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelCallEventMonitor::StartMonitoring" );
    
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelCallEventMonitor::StartMonitoring: \
                    Request RMobilePhone::NotifyCallEvent" );
        iCall.NotifyCallEvent( iStatus, iCallEvent );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelCallEventMonitor::StartMonitoring: Already active" );
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles event notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelCallEventMonitor::RunL()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelCallEventMonitor::RunL")
    
    // Survive from monitor destruction during observing sequence
    TBool destroyed = EFalse;
    iDestrPtr = &destroyed;
    
    if ( iStatus == KErrNone )
        {
        switch ( iCallEvent )
            {
            // The call has been terminated by the remote party. 
            case RMobileCall::ERemoteTerminated:
                {
                CSPLOGSTRING( CSPINT, 
                   "CSP CSPEtelCallEventMonitor::RunL: Remotely terminated" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteTerminated );
                break;
                }
             // The call has been placed on hold as a result of a local action
            case RMobileCall::ELocalHold:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Locally held" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPLocalHold );
                break;
                }
                
            // The call has been resumed as a result of a local action. 
            case RMobileCall::ELocalResume:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Locally resumed" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPLocalResume );
                break;
                }
            // The call has been deflected to another remote party as a result
            // of a local action.
            case RMobileCall::ELocalDeflectCall:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Locally deflected" );
                iObserver.NotifyTransferCallEventOccurred(  
                        MCCPTransferObserver::ECCPLocalTransfer );
                break;
                }
            // The call has been transferred to another remote party as a 
            // result of a local action. 
            case RMobileCall::ELocalTransfer:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Locally transfered" );
                iObserver.NotifyTransferCallEventOccurred( 
                        MCCPTransferObserver::ECCPLocalTransfer );
                break;
                }
            // The call has been placed on hold by the remote connected party
            case RMobileCall::ERemoteHold:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely held" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteHold );
                break;
                }
            // The call has been resumed by the remote connected party. 
            case RMobileCall::ERemoteResume:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely resumed" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteResume );
                break;
                }
            // The call has been joined by the remote connected party to other
            // call(s) to create/add to a conference call. 
            case RMobileCall::ERemoteConferenceCreate:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely created \
                             conference" );
                
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteConferenceCreate );
                        
                break;
                }
            case RMobileCall::ERemoteTransferring:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely created \
                     conference" );
                iObserver.NotifyTransferCallEventOccurred( 
                        MCCPTransferObserver::ECCPRemoteTransferring );
                break;
                }
            case RMobileCall::ERemoteTransferAlerting:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely created \
                     conference" );
                iObserver.NotifyTransferCallEventOccurred( 
                            MCCPTransferObserver::ECCPRemoteTransferAlerting );
                break;
                }
            // The outgoing call has been barred by the remote party
            case RMobileCall::ERemoteBarred:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely barred" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteBarred );
                break;
                }

            // The call is being forwarded by the remote party. 
            case RMobileCall::ERemoteForwarding:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely forwarded" );
                iObserver.NotifyForwardEventOccurred( 
                        MCCPForwardObserver::ECCPRemoteForwarding );
                break;
                }

            // The call is waiting at the remote end. 
            case RMobileCall::ERemoteWaiting:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Remotely waiting" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPRemoteWaiting );
                break;
                }

            // The outgoing call has been barred by the local party. 
            case RMobileCall::ELocalBarred:
                {
                CSPLOGSTRING( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Locally barred" );
                iObserver.NotifyCallEventOccurred( 
                        MCCPCallObserver::ECCPLocalBarred );
                break;
                }

            default:
                {
                CSPLOGSTRING2( CSPINT, 
                    "CSP CSPEtelCallEventMonitor::RunL: Unspecified/protocol \
                     specific call event: %d", iCallEvent );
                break;
                }
            }
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
// Request canceling.
// ---------------------------------------------------------------------------
//
void CSPEtelCallEventMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSPEtelCallEventMonitor::DoCancel" );
    iCall.CancelAsyncRequest( EMobileCallNotifyCallEvent );
    }
    
// ---------------------------------------------------------------------------
// Constructs the monitor.
// ---------------------------------------------------------------------------
//
CSPEtelCallEventMonitor::CSPEtelCallEventMonitor( MCSPCallObserver& aObserver,
                                                  RMobileCall& aCall ) : 
                                   CActive( EPriorityStandard ), 
                                   iObserver( aObserver ),
                                   iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallEventMonitor::CSPEtelCallEventMonitor()" );
    CActiveScheduler::Add( this );
    }

// End of File

