/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of class CSPEtelConferenceStatusMonitor which
*                monitors call status changes from ETel and notifies observer
*                according to call status change.
*
*/


#include "cspetelconferencestatusmonitor.h"

#include <mccpconferencecallobserver.h>

#include "csplogger.h"
#include "cspconferencecall.h"


// ---------------------------------------------------------------------------
// CSPEtelConferenceStatusMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceStatusMonitor* CSPEtelConferenceStatusMonitor::NewL( 
                                                    MCSPConferenceStatusObserver& aOwner,
                                                    RMobileConferenceCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, "CSPEtelConferenceStatusMonitor::NewL()" );
    CSPEtelConferenceStatusMonitor* self = 
                        new ( ELeave ) CSPEtelConferenceStatusMonitor( 
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
CSPEtelConferenceStatusMonitor::~CSPEtelConferenceStatusMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceStatusMonitor::~CSPEtelConferenceStatusMonitor()" );
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
void CSPEtelConferenceStatusMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPOBJECT, 
          "CSPEtelConferenceStatusMonitor::StartMonitoring()" );
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelConferenceStatusMonitor::StartMonitoring: Request \
                    RMobilePhone::NotifyMobileStatusChange" );
        iCall.NotifyConferenceStatusChange( iStatus, iConferenceStatus );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelConferenceStatusMonitor::StartMonitoring: Already active" );
        }
    }
    
// ---------------------------------------------------------------------------
// From CActive
// Handles call status notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceStatusMonitor::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelConferenceStatusMonitor::RunL: status: %d", iStatus.Int() );
    
    // Survive from monitor destruction during observing sequence
    TBool destroyed = EFalse;
    iDestrPtr = &destroyed;
    
    if ( iStatus == KErrNone )
        {
        switch ( iConferenceStatus )
            {

            case RMobileConferenceCall::EConferenceIdle:
                {
                CSPLOGSTRING( CSPINT, "CSPETelConferenceStatusMonitor IDLE");
                iOwner.NotifyStateChange( 
                    MCSPConferenceStatusObserver::ECSPConferenceIdle );
                break;
                }

            case RMobileConferenceCall::EConferenceActive:
                {
                CSPLOGSTRING( CSPINT, "CSPETelConferenceStatusMonitor ACTIVE");
                iOwner.NotifyStateChange( 
                    MCSPConferenceStatusObserver::ECSPConferenceActive );
                break;
                }
            case RMobileConferenceCall::EConferenceHold:
                {
                CSPLOGSTRING( CSPINT, "CSPETelConferenceStatusMonitor HOLD");

                iOwner.NotifyStateChange( 
                    MCSPConferenceStatusObserver::ECSPConferenceHold );

                break;
                }

            default:
                {
                CSPLOGSTRING2( CSPINT, 
                   "CSP CSPEtelConferenceStatusMonitor::RunL: Unspecified/protocol \
                             specific call status: %d", iConferenceStatus );
                break;
                }
            }
            
        }
    else
        {
        CSPLOGSTRING2( CSPERROR, 
            "CSP CSPEtelConferenceStatusMonitor::RunL err %d", iStatus.Int() );
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
        // Already destroyed, do not touch members.
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceStatusMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( 
                          EMobileConferenceCallNotifyConferenceStatusChange );
    }

// ---------------------------------------------------------------------------
// Constructs the monitor..
// ---------------------------------------------------------------------------
//
CSPEtelConferenceStatusMonitor::CSPEtelConferenceStatusMonitor( 
                                   MCSPConferenceStatusObserver& aOwner,
                                   RMobileConferenceCall& aCall ) : 
                                   CActive( EPriorityStandard ),
                                   iOwner( aOwner ),
                                   iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceStatusMonitor::CSPEtelConferenceStatusMonitor()" );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceStatusMonitor::ConstructL()
    {
    // Implementation not required.
    }


// End of file
