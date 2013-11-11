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
* Description: Implementation of CPhoneStateCallSetupVoIP class.
*
*/


// INCLUDES
#include <mpeengineinfo.h>

#include "cphonestatecallsetupvoip.h"
#include "phonelogger.h"
#include "mphonestatemachine.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStateCallSetupVoIP::CPhoneStateCallSetupVoIP( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization ) : 
    CPhoneCallSetup( &aStateMachine, &aViewCommandHandle, &aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetupVoIP::~CPhoneStateCallSetupVoIP()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

CPhoneStateCallSetupVoIP::~CPhoneStateCallSetupVoIP()
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetupVoIP::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStateCallSetupVoIP::ConstructL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneStateCallSetupVoIP::ConstructL()");
    CPhoneCallSetup::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneStateCallSetupVoIP::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateCallSetupVoIP* CPhoneStateCallSetupVoIP::NewL( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization )
    {
    CPhoneStateCallSetupVoIP* self = new (ELeave) 
        CPhoneStateCallSetupVoIP( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateCallSetupVoIP::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneStateCallSetupVoIP::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateCallSetupVoIP::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnecting:
            HandleConnectingL( aCallId );
            break;
        
        default:
            CPhoneCallSetup::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateCallSetupVoIP::HandleConnectingL
// -----------------------------------------------------------
//
void CPhoneStateCallSetupVoIP::HandleConnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateCallSetupVoIP::HandleConnectingL() ");

    if( iStateMachine->PhoneEngineInfo()->CallType( aCallId ) == EPECallTypeVoIP )
        {
        //TODO
        //SetCallHeaderType( EPECallTypeVoIP );   
        }
        
    CPhoneCallSetup::HandleConnectingL( aCallId );      
    }

// End of File
