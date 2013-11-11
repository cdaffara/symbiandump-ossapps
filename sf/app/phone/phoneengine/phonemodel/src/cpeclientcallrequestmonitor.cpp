/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of 
*                CPEClientCallRequestMonitor class 
*
*/


// INCLUDE FILES
#include "cpeclientcallrequestmonitor.h"
#include "cpemessagehandler.h"
#include "mpephonemodelinternal.h"
#include "pevirtualengine.h"
#include <cphcltcallnotify.h>
#include <cphcltdialdata.h>
#include <mpedatastore.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::CPEClientCallRequestMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEClientCallRequestMonitor::CPEClientCallRequestMonitor( 
        MPEPhoneModelInternal& aModel,
        CPhCltCallNotify& aCallNotifier ): 
            iModel( aModel ),
            iCallNotifier( aCallNotifier )
    {
    TEFLOGSTRING( KTAOBJECT, 
        "PE CPEClientCallRequestMonitor::CPEClientCallRequestMonitor complete");
    }

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CPEClientCallRequestMonitor::ConstructL()
    {
    iDialData = NULL;    
    }
    
// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEClientCallRequestMonitor* CPEClientCallRequestMonitor::NewL( 
    MPEPhoneModelInternal& aModel,
    CPhCltCallNotify& aCallNotifier )
    {
    CPEClientCallRequestMonitor* self = new( ELeave ) 
        CPEClientCallRequestMonitor( aModel, aCallNotifier  );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPEClientCallRequestMonitor::~CPEClientCallRequestMonitor()
    {
    Cancel();
    if( iDialData )
        {
        delete iDialData;
        iDialData = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::SendRespond
// Sends respond to the phone client.
// -----------------------------------------------------------------------------
//
void CPEClientCallRequestMonitor::SendRespond( 
        const TInt aRespond )
    {
    if ( iClientOriginatedCall )
        {
        TEFLOGSTRING2( KTAMESOUT, 
            "PE CPEClientCallRequestMonitor::SendRespond, CPhCltCallNotify::RespondClient( %d )"
            ,aRespond );
        iCallNotifier.RespondClient( aRespond );
        iClientOriginatedCall = EFalse;
        StartMonitoring();
        }    
    }


// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::DialRequest
// -----------------------------------------------------------------------------
//
void CPEClientCallRequestMonitor::DialRequest( CPhCltDialData* aDialData )
    {
    TEFLOGSTRING( KTAMESOUT, 
        "PE CPEClientCallRequestMonitor::DialRequest" );
    iMonitoring = EFalse;
    iDialData = aDialData;
    iClientOriginatedCall = ETrue;
    iModel.DataStore()->SetPhoneNumber( 
        static_cast<TPEPhoneNumber>( iDialData->TelephoneNumber() ) );
        
    if ( aDialData->SATCall() )
        {
        iModel.DataStore()->SetCallOriginCommand( EPECallOriginSAT );
        }
    else
        {
        iModel.DataStore()->SetCallOriginCommand( EPECallOriginClient );
        }

    iModel.HandleInternalMessage( MPEPhoneModel::EPEMessageClientDial );
    
    // Clear data so that following dials during created call does not
    // use these values.
    iModel.DataStore()->SetUserToUserInformation( KNullDesC() );
    iModel.DataStore()->SetCallOriginCommand( EPECallOriginPhone );
    }

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::StartMonitoring
// Starts monitoring phone client for client dials.
// -----------------------------------------------------------------------------
//
void CPEClientCallRequestMonitor::StartMonitoring()
    {
    TEFLOGSTRING( KTAMESOUT, 
        "PE CPEClientCallRequestMonitor::StartMonitoring, RPhCltCallNotify::NotifyDialRequest( )" );
    
    iCallNotifier.NotifyDialRequest( this );
    if( iDialData )
        {
        delete iDialData;
        iDialData = NULL;
        }
    iMonitoring = ETrue;
    }

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::DoCancel
// Cancel active object request
// -----------------------------------------------------------------------------
//
void CPEClientCallRequestMonitor::Cancel()
    {
    TEFLOGSTRING( KTAREQEND, "PE CPEClientCallRequestMonitor::DoCancel" );
    iCallNotifier.CancelNotifyDialRequest( );
    iMonitoring = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::ClientDialData
// -----------------------------------------------------------------------------
//
CPhCltDialData* CPEClientCallRequestMonitor::ClientDialData()
    {
    return iDialData;
    }
    
// -----------------------------------------------------------------------------
// CPEClientCallRequestMonitor::IsActive
// -----------------------------------------------------------------------------
//    
TBool CPEClientCallRequestMonitor::IsActive()
    {
    return iMonitoring;
    }
        
// End of File
