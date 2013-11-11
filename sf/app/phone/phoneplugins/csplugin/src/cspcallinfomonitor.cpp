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
* Description:  Monitors changes in remote party info.
*  Interface   : 
*
*/



//  INCLUDE FILES

#include    "cspcallinfomonitor.h"       
#include    "mcspcallobserver.h"
#include    "csppanic.pan"
#include    "csplogger.h" //Debug


// ============================ MEMBER FUNCTIONS =============================

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::NewL()
//
//  1st phase constructor
//----------------------------------------------------------------------------
//
CSPCallInfoMonitor* CSPCallInfoMonitor::NewL
    ( MCSPCallObserver& aObserver,
      RMobileCall& aCall  )   
    {
    CSPCallInfoMonitor* self = new (ELeave) CSPCallInfoMonitor( aObserver,
                                                                aCall ); 
    return self;
    }

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::~CSPCallInfoMonitor()
//
//  d'tor.
//----------------------------------------------------------------------------
//
CSPCallInfoMonitor::~CSPCallInfoMonitor()
    {    
    Cancel();
    CSPLOGSTRING( CSPINT, "CSP: CSPCallInfoMonitor::~CSPCallInfoMonitor" );
    }

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::StartMonitor()
//
//  Starts monitoring.
//----------------------------------------------------------------------------
//
void CSPCallInfoMonitor::StartMonitoring()
    {    
    // error ignored (starting notification doesn't leave)
    if (!IsActive()) 
        {
        iCall.NotifyRemotePartyInfoChange( 
            iStatus,  
            iRemotePartyInfoPckg ); 
        SetActive();             
        }
    else
        {
        CSPLOGSTRING( CSPERROR, 
            "CSP: CSPCallInfoMonitor::StartMonitoring: Already active" );
        
        }
    }

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::RunL()
//
//  Handling of the status changes.
//----------------------------------------------------------------------------
//
void CSPCallInfoMonitor::RunL()
    {
    CSPLOGSTRING2( CSPINT, 
        "CSP: CSPCallInfoMonitor::RunL iStatus: %d", iStatus.Int() );

    RMobileCall::TMobileCallRemoteIdentityStatus idStatus = 
        iRemotePartyInfo.iRemoteIdStatus;
    CSPLOGSTRING2( CSPINT, 
        "CSP: CSPCallInfoMonitor::RunL id status: %d", idStatus );

    switch ( iStatus.Int() )
        {
        case KErrNone:
            {                
            if ( idStatus == RMobileCall::ERemoteIdentityAvailable) 
                {
                iObserver.NotifyRemotePartyInfoChanged(
                    iRemotePartyInfo.iCallingName,
                    KNullDesC());
                }
            else
                {
                CSPLOGSTRING2( CSPERROR, 
                "CSP: CSPCallInfoMonitor::RunL DISCARD: %d", idStatus );
                }            
            break;
            }
        default:
            CSPLOGSTRING( CSPERROR, 
                "CSP: CSPCallInfoMonitor::RunL error" );
            break;
        }
        
    if ( KErrNotSupported != iStatus.Int() )
        {        
        StartMonitoring();
        }
    }

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::DoCancel()
//
//  Implementation of cancel.
//----------------------------------------------------------------------------
//
void CSPCallInfoMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSP: CSPCallInfoMonitor::DoCancel" );
    
    iCall.CancelAsyncRequest( EMobileCallNotifyRemotePartyInfoChange );
    }

//----------------------------------------------------------------------------
//  CSPCallInfoMonitor::CSPCallInfoMonitor()
//
//  constructor.
//----------------------------------------------------------------------------
//
CSPCallInfoMonitor::CSPCallInfoMonitor( MCSPCallObserver &aObserver, 
                                        RMobileCall& aCall ) : 
        CActive( EPriorityStandard ),
        iObserver( aObserver ),
        iCall( aCall ),
        iRemotePartyInfoPckg( iRemotePartyInfo )
    {
    CActiveScheduler::Add( this );
    }

// End of file
