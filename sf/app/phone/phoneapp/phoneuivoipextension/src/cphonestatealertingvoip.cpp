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
* Description: Implementation of CPhoneStateAlertingVoIP class.
*
*/


// INCLUDES
#include <mpeengineinfo.h>

#include "cphonestatealertingvoip.h"
#include "phonelogger.h"
#include "mphonestatemachine.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStateAlertingVoIP::CPhoneStateAlertingVoIP( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization ) : 
    CPhoneAlerting( &aStateMachine, &aViewCommandHandle, &aPhoneCustomization )
    {
    
    }

// -----------------------------------------------------------
// CPhoneStateAlertingVoIP::~CPhoneStateAlertingVoIP()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

CPhoneStateAlertingVoIP::~CPhoneStateAlertingVoIP()
    {
    }

// -----------------------------------------------------------
// CPhoneStateAlertingVoIP::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStateAlertingVoIP::ConstructL()
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, "CPhoneStateAlertingVoIP::ConstructL()");
    CPhoneAlerting::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneStateAlertingVoIP::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateAlertingVoIP* CPhoneStateAlertingVoIP::NewL( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization )
    {
    CPhoneStateAlertingVoIP* self = new (ELeave) 
        CPhoneStateAlertingVoIP( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateAlertingVoIP::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneStateAlertingVoIP::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateAlertingVoIP::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
        
        default:
            CPhoneAlerting::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateAlertingVoIP::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneStateAlertingVoIP::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( PhoneUIVoIPExtension, 
        "CPhoneStateAlertingVoIP::HandleConnectedL() ");

    if( iStateMachine->PhoneEngineInfo()->CallType( aCallId ) == EPECallTypeVoIP )
        {
        //TODO
        //SetCallHeaderType( EPECallTypeVoIP );
        }

    CPhoneAlerting::HandleConnectedL( aCallId );        
    }

// End of File
