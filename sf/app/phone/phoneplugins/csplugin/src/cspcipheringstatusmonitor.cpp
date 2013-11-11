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
* Description:  Contains the implementation of class 
*                CSPCipheringStatusMonitor.
*
*/


//  INCLUDE FILES
#include <rmmcustomapi.h>
#include "cspcipheringstatusmonitor.h"
#include "csplogger.h"
#include "mcspsecuritysettingobserver.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSPCipheringStatusMonitor::CSPCipheringStatusMonitor( 
        RMmCustomAPI& aMmCustom, MCSPSecuritySettingObserver& aObs )
          : CActive( EPriorityStandard ),            
            iMmCustom( aMmCustom ),
            iObserver( aObs ),
            iIsInitialised( EFalse ),
            iCallsSecured( EFalse ), 
            iSecureSpecified( ETrue )
    {
    CActiveScheduler::Add( this );
    }

// Destructor
CSPCipheringStatusMonitor::~CSPCipheringStatusMonitor()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// CSPCipheringStatusMonitor::NewL
// ---------------------------------------------------------------------------
//
CSPCipheringStatusMonitor* CSPCipheringStatusMonitor::NewL(
     RMmCustomAPI& aMmCustom, MCSPSecuritySettingObserver& aObs ) 
    {
    CSPLOGSTRING(CSPOBJECT, 
        "CSPCipheringStatusMonitor::NewL()" );
    CSPCipheringStatusMonitor* self = new ( ELeave ) CSPCipheringStatusMonitor( 
                                        aMmCustom, aObs );
    return self;    
    }

// -----------------------------------------------------------------------------
// CSPCipheringStatusMonitor::StartMonitoring
// Starts ciphering status monitoring
// -----------------------------------------------------------------------------
//
void CSPCipheringStatusMonitor::StartMonitoring()
    {    
    if ( !IsActive() )
        {
        if ( !iIsInitialised )
            {
            // Monitor not initialized, get the initial ciphering status
            iMmCustom.GetCipheringInfo( iStatus, iCipheringInfo );
            }
        else
            {
            // Monitor already initialized, monitor changes
            iMmCustom.NotifyCipheringInfoChange( iStatus, iCipheringInfo ); 
            }
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPINT, 
            "CSP: CSPCipheringStatusMonitor::StartMonitoring: \
             not done, already active");
        }
    }
    
// -----------------------------------------------------------------------------
// CSPCipheringStatusMonitor::DoCancel
// Cancels active object requests
// Method calls CancelAsyncRequest from the CustomaEtel object
// -----------------------------------------------------------------------------
//
void CSPCipheringStatusMonitor::DoCancel()
    {
    CSPLOGSTRING( CSPINT, "CSP: CSPCipheringStatusMonitor::DoCancel" );
    
    if ( IsActive() )
        {
        if( iIsInitialised )
            {
            iMmCustom.CancelAsyncRequest( ECustomNotifyCipheringInfoChangeIPC );
            }
        else
            {
            iMmCustom.CancelAsyncRequest( ECustomGetCipheringInfoIPC );
            }
        }
    }

// -----------------------------------------------------------------------------
// CPESupplementaryServicesMonitor::RunL
// Method gets notification from etel that asyncronous request is completed.
// Method sends possible messages to the owner object and
// Method makes a new issue request to the CustomEtel.
// -----------------------------------------------------------------------------
//
void CSPCipheringStatusMonitor::RunL()
    {
    CSPLOGSTRING2(CSPINT,
        "CSP: CSPCipheringStatusMonitor::RunL: iStatus: %d", iStatus.Int() );

    if ( iStatus == KErrNone )
        {
        TBool secureSpecifiedChanged = iSecureSpecified != iCipheringInfo.iIndStatus; 
        
        // Notify only on secure specified status changes 
        if ( secureSpecifiedChanged )        
            {
            // Is set when secure indicator showing is allowed.
            iSecureSpecified = iCipheringInfo.iIndStatus; 
            
            if ( !iSecureSpecified )
                {                
                iObserver.SecuritySettingChanged( 
                        MCSPSecuritySettingObserver::ESecureNotSpecified ); 
                }
            }
        
        TBool callsSecuredChanged = ( iCallsSecured != iCipheringInfo.iCiphStatus ); 
        
        // Notify secure status changes only when secure has been specified
        if ( iSecureSpecified && callsSecuredChanged )
            {
            iCallsSecured = iCipheringInfo.iCiphStatus;
            
            if ( iCallsSecured )
                {
                iObserver.SecuritySettingChanged( MCSPSecuritySettingObserver::ESecureCall ); 
                }
            else 
                {                
                iObserver.SecuritySettingChanged( MCSPSecuritySettingObserver::ENotSecureCall ); 
                }
            }
        
        iIsInitialised = ETrue; 
        StartMonitoring();
        }
    else
        {
        CSPLOGSTRING( CSPINT, 
            "CSPCipheringStatusMonitor::RunL: Ciphering Off  MONITOR OFF" );                
        }
          
    if ( iStatus == KErrNotFound && !iIsInitialised )
        {
        // Network was not ready while fetching ciphering info
        iIsInitialised = ETrue;
        StartMonitoring();
        CSPLOGSTRING( CSPINT, 
            "CALL: CSPCipheringStatusMonitor::RunL: StartMonitoring()" );
        }
    }

// -----------------------------------------------------------------------------
// CSPCipheringStatusMonitor::NetworkSecurityStatus
// Network security status.
// -----------------------------------------------------------------------------
//
TBool CSPCipheringStatusMonitor::NetworkSecurityStatus() const
    {
    return iCallsSecured;
    }

// -----------------------------------------------------------------------------
// CSPCipheringStatusMonitor::SecureSpecified
// Secure specified status.
// -----------------------------------------------------------------------------
//
TBool CSPCipheringStatusMonitor::SecureSpecified() const
    {
    return iSecureSpecified;
    }

// End of File 

