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
* Description:
*
*/


#include "irdebug.h"
#include "irnetworkobserver.h"
#include "irnetworkcontroller.h"


// ---------------------------------------------------------------------------
// CIRNetworkObserver::NewL
// Creates an Instance of CIRNetworkObserver
// ---------------------------------------------------------------------------
//
EXPORT_C CIRNetworkObserver *CIRNetworkObserver::NewL(
                                CIRNetworkController* aNetworkController )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::NewL - Entering" );
    CIRNetworkObserver *self = NewLC( aNetworkController );
    CleanupStack::Pop( self );
    IRLOG_DEBUG( "CIRNetworkObserver::NewL - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::~CIRNetworkObserver()
// Default Destructor
// ---------------------------------------------------------------------------
//
CIRNetworkObserver::~CIRNetworkObserver()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::~CIRNetworkObserver - Entering" );
    Cancel();
    iIRConnectionMonitor.CancelNotifications();
    iIRConnectionMonitor.Close();
    IRLOG_DEBUG( "CIRNetworkObserver::~CIRNetworkObserver - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::InitializeNetworkObserver()
//  Initializes the Connection monitor
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::InitializeNetworkObserver()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::InitializeNetworkObserver - Entering" );
    iConnectionId = 0;
    if ( !IsActive() )
        {
        iObserverState = EInitializing;
        iIRConnectionMonitor.GetConnectionCount( iConnectionCount,iStatus );
        SetActive();
        }
    IRLOG_DEBUG( "CIRNetworkObserver::InitializeNetworkObserver - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::SetObserver( MIRNetworkController* aObserver )
// Set the observer used to communicate with the IRNetworkController
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::SetObserver( MIRNetworkController* aObserver )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::SetObserver - Entering" );
    iMonitorObserver = aObserver;
    iMonitoringRequired = ETrue;  
    IRLOG_DEBUG( "CIRNetworkObserver::SetObserver - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::CIRNetworkObserver()
// Default Constructor
// ---------------------------------------------------------------------------
//
CIRNetworkObserver::CIRNetworkObserver( CIRNetworkController* aNetworkController ):
 CActive( CActive::EPriorityStandard ), iNetworkController( aNetworkController )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::CIRNetworkObserver - Entering" );
    // Add the AO to the ActiveScheduler
    CActiveScheduler::Add( this );
    IRLOG_DEBUG( "CIRNetworkObserver::CIRNetworkObserver - Exiting." );
    }

// ---------------------------------------------------------------------------
// Creates an Instance of CIRNetworkObserver
// ---------------------------------------------------------------------------
//
CIRNetworkObserver *CIRNetworkObserver::NewLC(
                            CIRNetworkController* aNetworkController )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::NewLC - Entering " );
    CIRNetworkObserver *self = new( ELeave )CIRNetworkObserver( 
                                                aNetworkController );
    CleanupStack::PushL( self );
    self->ConstructL();
    IRLOG_DEBUG( "CIRNetworkObserver::NewLC - Exiting." );
    return self;
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::ConstructL() 
// Second Phase construction.
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::ConstructL()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::ConstructL - Entering" );
    iIRConnectionMonitor.ConnectL();
    iIRConnectionMonitor.NotifyEventL( *this );
    iMonitoringRequired = EFalse;  
    IRLOG_DEBUG( "CIRNetworkObserver::ConstructL - Exiting." );    
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::RunL()
// The function is called by the active scheduler when a request completion event occurs,
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::RunL()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::RunL - Entering" );
    IRNetworkObserverRunL();
    IRLOG_DEBUG( "CIRNetworkObserver::RunL - Exiting" );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::DoCancel()
// Cancels the pending requests on the CIRNetworkObserver Active object
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::DoCancel()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::DoCancel" );
    }

// ---------------------------------------------------------------------------    
// CIRNetworkObserver::RunError()
// Handles a leave occurring in the request completion event handler RunL()
// ---------------------------------------------------------------------------
//    
TInt CIRNetworkObserver::RunError( TInt /*aError*/ )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::RunError" );
    iNetworkController->ResetHandingOverConnection();
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::EventL( const CConnMonEventBase &aConnMonEvent )
// Derived from MConnectionMonitorObserver
// Implements the EventL method which is called when there is a network event
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::EventL(const CConnMonEventBase &aConnMonEvent)
    {
    IRLOG_INFO4( "CIRNetworkObserver::EventL - Event type %d for connection %d, iConnectionId=%d", 
                 aConnMonEvent.EventType(), aConnMonEvent.ConnectionId(), iConnectionId );
    switch(aConnMonEvent.EventType())
        {
        case EConnMonCreateConnection:
            {
            //Triggered when a new connection has been been created
            // for future implementation
            }
        break;
        
        case EConnMonDeleteConnection:
            {
            //Triggered when a connection has been been deleted.
            if( aConnMonEvent.ConnectionId() == iConnectionId )
                {
                if(iMonitoringRequired)
                    {
                    if (iMonitorObserver)
                         {
                         iMonitorObserver->IRNetworkEventL(ENetworkConnectionDisconnected);
                         }
                    else
                        {
                        iNetworkController->NotifyActiveNetworkObserversL(ENetworkConnectionDisconnected);
                        }
                    }
                }     
            }
        break;
        
        case EConnMonCreateSubConnection:       
            {
            //Triggered when a new subconnection has been been created
            // for future implementation
            }
        break;
        
        case EConnMonDeleteSubConnection:       
            {
            //Triggered when a subconnection has been been deleted
            // for future implementation
            }
        break;
        
        case EConnMonDownlinkDataThreshold:     
            {
            // for future implementation
            }
        break;
        
        case EConnMonUplinkDataThreshold:       
            {
            // for future implementation
            }
        break;
        
        case EConnMonNetworkStatusChange:       
            {
            // for future implementation
            }
        break;
        
        case EConnMonConnectionStatusChange:    
            {
            //Triggered when the status of some connection changes.
            // for future implementation
            }
        break;
        
        case EConnMonConnectionActivityChange:  
            {
            //Triggered when some connection changes from active to idle or vice versa
            // for future implementation
            }
        break;
        
        case EConnMonNetworkRegistrationChange: 
            {
            // for future implementation
            }
        break;
        
        case EConnMonBearerChange:              
            {
            //Triggered when bearer type (GPRS / Edge GPRS / WCDMA) changes
            // for future implementation
            }
        break;
        
        case EConnMonSignalStrengthChange:      
            {
            // for future implementation
            }
        break;
        
        case EConnMonBearerAvailabilityChange:  
            {
            //Triggered when the availability of some bearer changes
            // for future implementation
            }
        break;
        
        case EConnMonIapAvailabilityChange:     
            {
            // for future implementation
            }
        break;
        
        case EConnMonTransmitPowerChange:    
            {
            // for future implementation
            }
        break;
        default:
            {
            // no implementation
            }
        break;    
        }
    IRLOG_DEBUG( "CIRNetworkObserver::EventL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::IdentifyConnection()
// Identifies the type of connection we have used to get connected to network
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::IdentifyConnection()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::IdentifyConnection - Entering" );
    if ( !IsActive() )
        {
        iObserverState = EGettingConnectionInfo;
        iIRConnectionMonitor.GetConnectionInfo(
            iConnectionCount,iConnectionId,iSubConnectionCount );
        // O Indicates method applies to connection
        iIRConnectionMonitor.GetIntAttribute(
            iConnectionId,0,KBearer,iConnectionType,iStatus );
        SetActive();
        }
    IRLOG_DEBUG( "CIRNetworkObserver::IdentifyConnection - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::GetIAPId()
// Retrieves the IAP Id of the connection
// ---------------------------------------------------------------------------
//
void CIRNetworkObserver::GetAPId()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::GetIAPId - Entering" );
    if ( !IsActive() )
        {
        iObserverState = EGettingIAPId;
        iIRConnectionMonitor.GetConnectionInfo(
            iConnectionCount, iConnectionId, iSubConnectionCount );
        // O in RConnectionMonitor::GetIntAttribute indicates method applies to connection
        iIRConnectionMonitor.GetUintAttribute(
            iConnectionId, 0, KIAPId, iIAPId, iStatus );
        SetActive();
        }
    IRLOG_DEBUG( "CIRNetworkObserver::GetIAPId - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::IRNetworkObserverRunL()
// Utility function used just to keep RunL() small
// ---------------------------------------------------------------------------
// 
void CIRNetworkObserver::IRNetworkObserverRunL()
    {
    IRLOG_DEBUG( "CIRNetworkObserver::IRNetworkObserverRunL - Entering" );
    switch(iObserverState)
        {
        case EInitializing:
            {
            if( iStatus.Int() == KErrNone )
                {
                // Initializing the Connection Monitor sucessful    
                IdentifyConnection();
                }
            else
                {
                // Error initializing the connection monitor
                iNetworkController->ResetHandingOverConnection();
                }    
            }
        break;
        
        case EGettingConnectionInfo:
            {
            if( iStatus.Int() == KErrNone )
                {
                iIsIAPIdAvailable = EFalse;
                GetAPId();  
                }
            else
                {
                // Error initializing the connection monitor
                iNetworkController->ResetHandingOverConnection();
                }    
            }
        break;   
        
        case EGettingIAPId:
            {
            iIsIAPIdAvailable = ETrue;   
            switch(iConnectionType)
            {
            case EBearerGPRS:
                {
                iIRConnectionType = EGprs;
                }
            break;
                  
            case EBearerEdgeGPRS:
                {
                iIRConnectionType = EEdge;
                }
            break;
                  
            case EBearerWLAN:
                {
                iIRConnectionType = EWiFi;    
                }
            break;
                  
            case EBearerWCDMA:
                {
                iIRConnectionType = EWcdma;
                }
                  break;
                
            case EBearerCDMA2000:
                {
                iIRConnectionType = ECdma2000;
                }
            break;
            
            default:
                {
                #ifdef __WINS__
                iIRConnectionType = EGprs;
                #endif
                }
            break;
            }
            
                  
            if (iMonitoringRequired)
                {
                if (iMonitorObserver)    
                    {
                    // Intimate the connection established event
                    iMonitorObserver->IRNetworkEventL(ENetworkConnectionEstablished);
                    }
                else
                    {
                    iNetworkController->NotifyActiveNetworkObserversL(ENetworkConnectionEstablished);
                    }
                }
                  
            iNetworkController->ResetHandingOverConnection();
            }
        break;
          
        default:
            {
            // no implementation
            }
        break;
        }
    IRLOG_DEBUG( "CIRNetworkObserver::IRNetworkObserverRunL - Exiting." );
    }

// ---------------------------------------------------------------------------
// CIRNetworkObserver::SetNetworkMonitoring()
// Sets network monitoring observer to decide whether network monitoring is required.
// ---------------------------------------------------------------------------
// 
void CIRNetworkObserver::SetNetworkMonitoring( TBool aValue )
    {
    IRLOG_DEBUG( "CIRNetworkObserver::SetNetworkMonitoring - Entering" );

    iMonitoringRequired = aValue;    
    
    IRLOG_DEBUG( "CIRNetworkObserver::SetNetworkMonitoring - Exiting" );

    }
