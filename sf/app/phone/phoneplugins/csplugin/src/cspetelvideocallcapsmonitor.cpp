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
* Description:  Implementation of class CSPEtelVideoCallCapsMonitor which
*                monitors call status changes from ETel and notifies observer
*                according to call status change.
*
*/


#include <mccpcallobserver.h>

#include "cspetelvideocallcapsmonitor.h"
#include "cspvideocall.h"
#include "csplogger.h"


// ---------------------------------------------------------------------------
// CSPEtelCallEventMonitor::NewL.
// ---------------------------------------------------------------------------
//
CSPEtelVideoCallCapsMonitor* CSPEtelVideoCallCapsMonitor::NewL( 
                                                    CSPVideoCall& aOwner,
                                                    RMobileCall& aCall ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelCallEventMonitor::NewL()" );
    CSPEtelVideoCallCapsMonitor* self = 
                        new ( ELeave ) CSPEtelVideoCallCapsMonitor( 
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
CSPEtelVideoCallCapsMonitor::~CSPEtelVideoCallCapsMonitor( )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelVideoCallCapsMonitor::CSPEtelVideoCallCapsMonitor()" );
    Cancel();
    }

// ---------------------------------------------------------------------------
// Starts the monitoring.
// NOTE: This function reads the initial value of data call caps in the 
// beginning so that client can use GetMobileDataCallCaps to get up to date
// caps information.
// ---------------------------------------------------------------------------
//
void CSPEtelVideoCallCapsMonitor::StartMonitoring()
    {
    CSPLOGSTRING(CSPOBJECT, 
          "CSPEtelVideoCallCapsMonitor::StartMonitoring()" );

    iCall.GetMobileDataCallCaps( iDataCallCapsPckg );
    
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CSP: CSPEtelVideoCallCapsMonitor::StartMonitoring: Request \
                    RMobilePhone::NotifyCapsChange" );
        iCall.NotifyMobileDataCallCapsChange( iStatus, iDataCallCapsPckg );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
         "CSP: CSPEtelVideoCallCapsMonitor::StartMonitoring: Already active" );
        }
    }
    
// ---------------------------------------------------------------------------
// CSPEtelVideoCallCapsMonitor::GetMobileDataCallCaps
// ---------------------------------------------------------------------------
//
TInt CSPEtelVideoCallCapsMonitor::GetMobileDataCallCaps( TDes8& aCaps ) const
    {
    CSPLOGSTRING( CSPREQIN, "CSPEtelVideoCallCapsMonitor::GetMobileDataCallCaps" );
    // get data call caps from Etel
    aCaps = iDataCallCapsPckg;
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSPEtelVideoCallCapsMonitor::GetProtocolCaps
// ---------------------------------------------------------------------------
//
TUint CSPEtelVideoCallCapsMonitor::GetProtocolCaps()
    {
    iDataCallCaps = iDataCallCapsPckg();
    return iDataCallCaps.iProtocolCaps;
    }

// ---------------------------------------------------------------------------
// From CActive
// Handles call status notifying.
// ---------------------------------------------------------------------------
//
void CSPEtelVideoCallCapsMonitor::RunL()
    {
    CSPLOGSTRING2( CSPREQEND, 
        "CSPEtelVideoCallCapsMonitor::RunL: status: %d", iStatus.Int() );
    
    if ( iStatus == KErrNone )
        {
        iDataCallCaps = iDataCallCapsPckg();
        
        iOwner.DataCallCapsChanged( iDataCallCaps );
        
        iCall.NotifyMobileDataCallCapsChange( iStatus, iDataCallCapsPckg );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// Canceling functionality.
// ---------------------------------------------------------------------------
//
void CSPEtelVideoCallCapsMonitor::DoCancel()
    {
    iCall.CancelAsyncRequest( EMobileCallNotifyMobileDataCallCapsChange );
    }
    
// ---------------------------------------------------------------------------
// Constructs the monitor..
// ---------------------------------------------------------------------------
//
CSPEtelVideoCallCapsMonitor::CSPEtelVideoCallCapsMonitor( CSPVideoCall& aOwner,
                                                    RMobileCall& aCall ) : 
                                   CActive( EPriorityStandard ),
                                   iOwner( aOwner ),
                                   iCall ( aCall ),
                                   iDataCallCapsPckg( iDataCallCaps )
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelVideoCallCapsMonitor::CSPEtelCallCapsMonitor()" );
    CActiveScheduler::Add( this );
    }
    
// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CSPEtelVideoCallCapsMonitor::ConstructL()
    {
    // Implementation not required.    
    }

// ---------------------------------------------------------------------------
// CSPEtelVideoCallCapsMonitor::UpdateProtocolCaps
// ---------------------------------------------------------------------------
//
void CSPEtelVideoCallCapsMonitor::UpdateProtocolCaps()
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPEtelVideoCallCapsMonitor::UpdateProtocolCaps()" );

    iCall.GetMobileDataCallCaps( iDataCallCapsPckg );            
    }

// End of File
