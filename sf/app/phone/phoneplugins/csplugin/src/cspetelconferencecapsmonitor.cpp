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
* Description:  Implementation of class CSPEtelConferenceCapsMonitor which
*                monitors call status changes from ETel and notifies observer
*                according to call status change.
*
*/


#include "cspetelconferencecapsmonitor.h"

#include "cspconferencecall.h"
#include "csplogger.h"



// ---------------------------------------------------------------------------
// CSPEtelConferenceCapsMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelConferenceCapsMonitor* CSPEtelConferenceCapsMonitor::NewL( 
                                                    CSPConferenceCall& aOwner,
                                                    RMobileConferenceCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceCapsMonitor::NewL()" );
    CSPEtelConferenceCapsMonitor* self = 
                        new ( ELeave ) CSPEtelConferenceCapsMonitor( 
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
CSPEtelConferenceCapsMonitor::~CSPEtelConferenceCapsMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceCapsMonitor::~CSPEtelConferenceCapsMonitor()" );
        
    if ( iDestrPtr )
        {
        *iDestrPtr = ETrue;
        iDestrPtr = NULL;
        }
        
    Cancel();
    }


// ---------------------------------------------------------------------------
// Starts the monitoring.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCapsMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPOBJECT, 
          "CSPEtelConferenceCapsMonitor::StartMonitoring()" );

    
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelConferenceCapsMonitor::StartMonitoring: Request \
                    RMobilePhone::NotifyCapsChange" );
        iCall.NotifyCapsChange( iStatus, iCaps );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPEtelConferenceCapsMonitor::StartMonitoring: Already active" );
        }
    }
    
// ---------------------------------------------------------------------------
// From CActive
// Handles call status notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCapsMonitor::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelConferenceCapsMonitor::RunL: status: %d", iStatus.Int() );
    
    // Survive from monitor destruction during observing sequence
    TBool destroyed = EFalse;
    iDestrPtr = &destroyed;

    
    if ( iStatus == KErrNone )
        {
        // Test caps and notify about the iOwner 
        iOwner.ConferenceCapsChanged( iCaps );

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
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCapsMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileConferenceCallNotifyCapsChange );
    }



    
// ---------------------------------------------------------------------------
// Constructs the monitor..
// ---------------------------------------------------------------------------
//
CSPEtelConferenceCapsMonitor::CSPEtelConferenceCapsMonitor( CSPConferenceCall& aOwner,
                                                    RMobileConferenceCall& aCall ) : 
                                   CActive( EPriorityStandard ),
                                   iOwner( aOwner ),
                                   iCall ( aCall )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelConferenceCapsMonitor::CSPEtelConferenceCapsMonitor()" );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPEtelConferenceCapsMonitor::ConstructL()
    {
    // Implementation not required.
    }


// End of file
