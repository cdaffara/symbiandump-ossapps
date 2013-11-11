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
* Description:  This module contains the implementation of CPECallAudioRoutingHandler class 
*                member functions
*
*/


// INCLUDES
#include <talogger.h>
#include "cpecallaudioroutinghandler.h"
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
// CPECallAudioRoutingHandler::CPECallAudioRoutingHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPECallAudioRoutingHandler::CPECallAudioRoutingHandler( 
    CPEAudioData& aOwner )
    : iOwner( aOwner )
    { 
    }

// -----------------------------------------------------------------------------
// CPEAudioRoutingMonitor::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPECallAudioRoutingHandler* CPECallAudioRoutingHandler::NewL( 
    CPEAudioData& aOwner )
    {
    CPECallAudioRoutingHandler* self = 
        new ( ELeave ) CPECallAudioRoutingHandler( aOwner );
    
    return self;
    }

CPECallAudioRoutingHandler::~CPECallAudioRoutingHandler()
    {
    }
 
 
// -----------------------------------------------------------------------------
// CPECallAudioRoutingHandler::HandleAudioRoutingCallInit
// -----------------------------------------------------------------------------
//
TInt CPECallAudioRoutingHandler::HandleAudioRoutingCallInit( TBool aVideoCall )
    {
  
    TInt error = KErrNone;
    CTelephonyAudioRouting::TAudioOutput curOutput = iOwner.Output();
    CTelephonyAudioRouting::TAudioOutput newOutput = curOutput;

    TEFLOGSTRING2( KTAINT, 
        "AUD CPECallAudioRoutingHandler::HandleAudioRoutingCallInit > : isVideo = %d", aVideoCall );
    iAnyVideoCall = aVideoCall;
    
    // Wired headset has top priority
    if ( iOwner.IsWiredAvailable() )
        {
        newOutput = CTelephonyAudioRouting::EWiredAudioAccessory;
        }
    else if ( iOwner.IsBTAvailable() ) 
        {
        if ( iAnswerToHandset )  
            {
            newOutput = iOwner.RoutePreference();
            }
        else
            {
            newOutput = CTelephonyAudioRouting::EBTAudioAccessory;
            }            
        }
    else if ( iOwner.IsTTYAvailable() )
        {
        newOutput = CTelephonyAudioRouting::ETTY;
        }
    else
        {
        if ( iAnyVideoCall )
            {
            if ( iAnswerToHandset )
                {
                newOutput = CTelephonyAudioRouting::EHandset;
                }
            else
                {
                newOutput = CTelephonyAudioRouting::ELoudspeaker;        
                }               
            }
        else
            {
            newOutput = iOwner.RoutePreference();
            } 
        }
    SetAnswerToHandset( EFalse );
    
    if ( newOutput != curOutput )
        {
        iOwner.SetTAROutput( newOutput, EFalse );    
        TEFLOGSTRING2( KTAINT, 
        "AUD CPECallAudioRoutingHandler::HandleAudioRoutingCallInit: new output = %d", newOutput );
        }
    else
        {
        TEFLOGSTRING2( KTAINT, 
        "AUD CPECallAudioRoutingHandler::HandleAudioRoutingCallInit: Already output = %d", curOutput );
        }        
 
    return error;
    }
    
// -----------------------------------------------------------------------------
// CPECallAudioRoutingHandler::HandleAudioRoutingAvailableChanged
// Handles accessory mode changed message from AudioHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPECallAudioRoutingHandler::HandleAudioRoutingAvailableChanged()
    {
    TInt error = KErrNone;
       
    CTelephonyAudioRouting::TAudioOutput curOutput = iOwner.Output();
    CTelephonyAudioRouting::TAudioOutput newOutput = curOutput;
    
    
    switch ( curOutput )
        {
        case CTelephonyAudioRouting::ENotActive:
            // None
             break;
        case CTelephonyAudioRouting::EHandset:
        case CTelephonyAudioRouting::ELoudspeaker:
            // Wired audio accessory connected during call
            if ( iOwner.IsWiredAvailable() )
                {
                newOutput = CTelephonyAudioRouting::EWiredAudioAccessory;
                }
            // BT comes available during call   
            else if ( iOwner.IsBTAvailable() )
                {
                newOutput = CTelephonyAudioRouting::EBTAudioAccessory;
                }
            else if ( iOwner.IsTTYAvailable() )                 
                {
                newOutput = CTelephonyAudioRouting::ETTY;
                }
            else
                {
                // No changes
                }   
            break;
        case CTelephonyAudioRouting::EWiredAudioAccessory:
            if ( iOwner.IsBTAvailable() )
                {
                // Not allowed direct from wired to BTA
                // or BTA comes available during wired, No changes
                }
            if ( iOwner.IsWiredAvailable() )
                {
                // No changes
                }
            else if ( iAnyVideoCall )
                {
                newOutput = CTelephonyAudioRouting::ELoudspeaker;
                }
            else                
                {
                // Wired detach
                newOutput = iOwner.RoutePreference();
                }   
            break;
        case CTelephonyAudioRouting::EBTAudioAccessory:
            if ( iOwner.IsWiredAvailable() )
                {
                newOutput = CTelephonyAudioRouting::EWiredAudioAccessory;
                }
            else if ( iOwner.IsBTAvailable() )  
                {
                // No changes
                }
            else
                {
                newOutput = iOwner.RoutePreference();
                }           
            break;
        case CTelephonyAudioRouting::ETTY:
            if ( iOwner.IsTTYAvailable()  )
                {
                // No changes
                }
            else
                {
                newOutput = iOwner.RoutePreference();
                }
            break;
       default:
            break;     
        }
    
   if ( newOutput != curOutput )
        {
        iOwner.SetTAROutput( newOutput, ETrue );    
        TEFLOGSTRING2( KTAINT, 
        "PE CPECallAudioRoutingHandler::HandleAudioRoutingAvailableChanged: new output = %d", newOutput );
        }
    else
        {
        TEFLOGSTRING2( KTAINT, 
        "PE CPECallAudioRoutingHandler::HandleAudioRoutingAvailableChanged: Already output = %d", curOutput );
        }     
      
    return error;
    }    

// -----------------------------------------------------------------------------
// CPECallAudioRoutingHandler::HandleAudioRoutePreferenceChanged
// Handles route preference mode changed message from AudioHandling subsystem.
// -----------------------------------------------------------------------------
//
TInt CPECallAudioRoutingHandler::HandleAudioRoutePreferenceChanged()
    {
    TInt error = KErrNone;
    CTelephonyAudioRouting::TAudioOutput curOutput = iOwner.Output();
    CTelephonyAudioRouting::TAudioOutput newOutput = curOutput;
    
    if ( !iAnyVideoCall &&
         curOutput != CTelephonyAudioRouting::EWiredAudioAccessory &&
         curOutput != CTelephonyAudioRouting::EBTAudioAccessory )
        {
        newOutput = iOwner.RoutePreference();
        }
    if ( newOutput != curOutput )
        {
        iOwner.SetTAROutput( newOutput, ETrue );    
        TEFLOGSTRING2( KTAINT, 
        "PE CPECallAudioRoutingHandler::HandleAudioRoutePreferenceChanged: new output = %d", newOutput );
        }
    else
        {
        TEFLOGSTRING2( KTAINT, 
        "PE CPECallAudioRoutingHandler::HandleAudioRoutePreferenceChanged: Already output = %d", curOutput );
        }        
    return error;
    }
// -----------------------------------------------------------------------------
// CPECallAudioRoutingHandler::SetAnswerToHandset
//  
// -----------------------------------------------------------------------------
//
void  CPECallAudioRoutingHandler::SetAnswerToHandset( TBool aStatus )
    {
    iAnswerToHandset = aStatus;
    }
 
 
// ================= OTHER EXPORTED FUNCTIONS ===============================
//None

//  End of File
