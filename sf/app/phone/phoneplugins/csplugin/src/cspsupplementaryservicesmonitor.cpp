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
* Description:  This file contains the implementation of CSPSupplementaryServicesMonitor class 
*                member functions.
*
*/



#include "cspsupplementaryservicesmonitor.h"
#include "csprovider.h"
#include "csplogger.h"


// -----------------------------------------------------------------------------
// CSPSupplementaryServicesMonitor::CSPSupplementaryServicesMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSPSupplementaryServicesMonitor::CSPSupplementaryServicesMonitor
        ( 
        CSProvider& aOwner,     
        RMmCustomAPI& aCustomAPI        
        ) : CActive( EPriorityStandard ),
            iOwner( aOwner ),           
            iCustomAPI( aCustomAPI )
    {
    CSPLOGSTRING( CSPOBJECT, 
        "CSPSupplementaryServicesMonitor::CSPSupplementaryServicesMonitor: complete" );
    CActiveScheduler::Add( this );
    }

// Destructor
CSPSupplementaryServicesMonitor::~CSPSupplementaryServicesMonitor()
    {
    Cancel();
    CSPLOGSTRING( CSPOBJECT,
        "CALL CSPSupplementaryServicesMonitor::~CSPSupplementaryServicesMonitor: Complete." );
    }

// -----------------------------------------------------------------------------
// CSPSupplementaryServicesMonitor::StartMonitoring
// Starts supplementary services monitoring
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSPSupplementaryServicesMonitor::StartMonitoring()
    {
    if ( !IsActive() )
        {
        CSPLOGSTRING( CSPREQOUT, 
            "CALL CSPSupplementaryServicesMonitor::StartMonitoring: RMMCustomAPI::NotifySsNetworkEvent() called" );
        iCustomAPI.NotifySsNetworkEvent( iStatus, iSsTypeAndMode, iSsInfo );
        SetActive();
        }
    else
        {
        CSPLOGSTRING( CSPINT, 
            "CALL CSPSupplementaryServicesMonitor::StartMonitoring: Already active" );
        }
    }

// -----------------------------------------------------------------------------
// CSPSupplementaryServicesMonitor::DoCancel
// Cancels active object requests
// Method calls CancelAsyncRequest from the CustomaEtel object
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSPSupplementaryServicesMonitor::DoCancel()
    {
    iCustomAPI.CancelAsyncRequest( ECustomNotifySsNetworkEventIPC );
    CSPLOGSTRING( CSPINT, 
        "CALL CSPSupplementaryServicesMonitor::DoCancel: \
        RMMCustomAPI::CancelAsyncRequest( ECustomNotifySsRequestCompleteIPC ) called" );
    }

// -----------------------------------------------------------------------------
// CSPSupplementaryServicesMonitor::RunL
// CMethod gets notification from etel that asyncronous request is completed.
// -----------------------------------------------------------------------------
//
void CSPSupplementaryServicesMonitor::RunL()
    {
    CSPLOGSTRING( CSPINT, "CSPSupplementaryServicesMonitor::RunL" );
    if ( iStatus == KErrNone )
       {
       CSPLOGSTRING2( CSPINT, 
           "CSPSupplementaryServicesMonitor::RunL: iSsTypeAndMode.iSsType: %d",
           iSsTypeAndMode.iSsType );
        
       iOwner.NotifySsEvent( iSsTypeAndMode, iSsInfo );
       StartMonitoring();
       }
    }

// End of File 
