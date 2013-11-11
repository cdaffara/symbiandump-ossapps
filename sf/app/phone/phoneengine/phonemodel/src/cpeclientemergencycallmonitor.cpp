/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
*                CPEClientEmergencyCallMonitor class 
*
*/


// INCLUDE FILES
#include "cpeclientemergencycallmonitor.h"
#include "cpemessagehandler.h"
#include "mpephonemodelinternal.h"
#include "pevirtualengine.h"
#include <cphcltcallnotify.h>
#include <mpedatastore.h>
#include <talogger.h>


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::CPEClientEmergencyCallMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEClientEmergencyCallMonitor::CPEClientEmergencyCallMonitor( 
        MPEPhoneModelInternal& aModel,
        CPhCltCallNotify& aCallNotifier )
        :   iModel( aModel ),
            iCallNotifier( aCallNotifier ),
            iClientOriginatedCall( EFalse )
    {
    TEFLOGSTRING( KTAOBJECT, 
        "PE CPEClientEmergencyCallMonitor::CPEClientEmergencyCallMonitor complete");
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CPEClientEmergencyCallMonitor::ConstructL()
    {
    }
    
// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEClientEmergencyCallMonitor* CPEClientEmergencyCallMonitor::NewL( 
    MPEPhoneModelInternal& aModel,
    CPhCltCallNotify& aCallNotifier )
    {
    CPEClientEmergencyCallMonitor* self = new( ELeave ) 
        CPEClientEmergencyCallMonitor( aModel, aCallNotifier );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPEClientEmergencyCallMonitor::~CPEClientEmergencyCallMonitor()
    {
    Cancel();    
    }

// -----------------------------------------------------------------------------
// CPhCltDialData* CPEClientEmergencyCallMonitor::ClientDialData
// -----------------------------------------------------------------------------
//    
CPhCltDialData* CPEClientEmergencyCallMonitor::ClientDialData()
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::SendRespond
// Sends respond to the phone client.
// -----------------------------------------------------------------------------
//
void CPEClientEmergencyCallMonitor::SendRespond( 
        const TInt aRespond )
    {
    if ( iClientOriginatedCall )
        {
        TEFLOGSTRING2( KTAMESOUT, 
            "PE CPEClientEmergencyCallMonitor::SendRespond, CPhCltCallNotify::RespondClient( %d )"
            ,aRespond );
        iCallNotifier.RespondEmergencyToClient( aRespond );
        iClientOriginatedCall = EFalse;
       // StartMonitoring();
        }    
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::StartMonitoring
// Starts monitoring phone client for client dials.
// -----------------------------------------------------------------------------
//
void CPEClientEmergencyCallMonitor::StartMonitoring() 
    {
    TEFLOGSTRING( KTAMESOUT, 
        "PE CPEClientEmergencyCallMonitor::StartMonitoring, CPhCltCallNotify::NotifyEmergencyCall( )" );
    iCallNotifier.NotifyEmergencyCall( this, iEmergencyNumber );
    iMonitoring = ETrue;
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::Cancel
// Cancel active object request
// -----------------------------------------------------------------------------
//
void CPEClientEmergencyCallMonitor::Cancel()
    {
    TEFLOGSTRING( KTAREQEND, "PE CPEClientEmergencyCallMonitor::Cancel" );
    iCallNotifier.CancelNotifyEmergencyCall();
    iMonitoring = EFalse;
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::DialEmergencyRequest
// -----------------------------------------------------------------------------
//
void CPEClientEmergencyCallMonitor::DialEmergencyRequest()
    {
    iMonitoring = EFalse;
    TEFLOGSTRING( KTAREQEND, 
        "PE CPEClientEmergencyCallMonitor::DialEmergencyRequest" );
    iClientOriginatedCall = ETrue;
    iModel.DataStore()->SetPhoneNumber( 
        static_cast<TPEPhoneNumber>( iEmergencyNumber ) );
    iModel.HandleInternalMessage( MPEPhoneModel::EPEMessageClientDialEmergency );
    StartMonitoring();
    }

// -----------------------------------------------------------------------------
// CPEClientEmergencyCallMonitor::IsActive
// -----------------------------------------------------------------------------
//    
TBool CPEClientEmergencyCallMonitor::IsActive()
    {
    return iMonitoring;
    }
        
// End of File
