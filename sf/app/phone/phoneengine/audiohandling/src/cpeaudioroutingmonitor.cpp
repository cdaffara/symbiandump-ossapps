/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEAudioRoutingMonitor class 
*                member functions
*
*/


// INCLUDES
#include "talogger.h"
#include "cpeaudioroutingmonitor.h"
#include "cpeaudiodata.h"

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// ==================== LOCAL FUNCTIONS =====================================
//None.

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::CPEAudioRoutingMonitor
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEAudioRoutingMonitor::CPEAudioRoutingMonitor( 
    CPEAudioData& aOwner )
    : iOwner( aOwner )
    { 
    }

// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEAudioRoutingMonitor* CPEAudioRoutingMonitor::NewL( 
    CPEAudioData& aOwner )
    {
    CPEAudioRoutingMonitor* self = 
        new ( ELeave ) CPEAudioRoutingMonitor( aOwner );
    
    return self;
    }

CPEAudioRoutingMonitor::~CPEAudioRoutingMonitor()
    {
    }

// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::AvailableOutputsChanged
// AvailableOutput mode changed event received.
// -----------------------------------------------------------------------------
//
void CPEAudioRoutingMonitor::AvailableOutputsChanged( 
    CTelephonyAudioRouting& aTelephonyAudioRouting )
    {
    TEFLOGSTRING( KTAINT,
        "AUD CPEAudioRoutingMonitor::AvailableOutputsChanged" );
    iOwner.SendMessage( MEngineMonitor::EPEMessageAvailableAudioOutputsChanged, 
                        aTelephonyAudioRouting );
    }
        
// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::OutputChanged
// Output mode changed event received.
// -----------------------------------------------------------------------------
//
void CPEAudioRoutingMonitor::OutputChanged( 
    CTelephonyAudioRouting& aTelephonyAudioRouting )
    {
    TEFLOGSTRING( KTAINT, "AUD CPEAudioRoutingMonitor::OutputChanged" );
    iOwner.SendMessage( MEngineMonitor::EPEMessageAudioOutputChanged, 
                        aTelephonyAudioRouting.Output() );
    }
        
// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::SetOutputComplete
// OutputComplete mode changed event received.
// -----------------------------------------------------------------------------
//
void CPEAudioRoutingMonitor::SetOutputComplete( 
    CTelephonyAudioRouting& aTelephonyAudioRouting, 
    TInt aError )
    {
    TEFLOGSTRING2( KTAINT, 
        "AUD CPEAudioRoutingMonitor::SetOutputComplete, aError: %d",
        aError );
    if ( aError == KErrNone )
        {
        iOwner.SendMessage( MEngineMonitor::EPEMessageAudioOutputChanged, 
                            aTelephonyAudioRouting.Output() );
        }
    else
        {
        TEFLOGSTRING( KTAERROR,
            "AUD CPEAUDIOROUTINGMONITOR::SETOUTPUTCOMPLETE SETOUTPUTL FAILED" );
        }
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================
//None

//  End of File
