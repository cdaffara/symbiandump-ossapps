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
* Description: Implementation of CPhoneStateIncomingVoIP class.
*
*/


// INCLUDES
#include <mpeengineinfo.h>
#include "cphonestateincomingvoip.h"
#include "phonelogger.h"
#include "cphonecustomizationvoip.h"
#include "phonerssbase.h"
#include "mphonestatemachine.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStateIncomingVoIP::CPhoneStateIncomingVoIP( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization ) : 
    CPhoneIncoming( &aStateMachine, &aViewCommandHandle, &aPhoneCustomization )
    {
    }


// -----------------------------------------------------------
// CPhoneStateIncomingVoIP::~CPhoneStateIncomingVoIP()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

CPhoneStateIncomingVoIP::~CPhoneStateIncomingVoIP()
    {
    }


// -----------------------------------------------------------
// CPhoneStateIncomingVoIP::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStateIncomingVoIP::ConstructL()
    {
    __LOGMETHODSTARTEND( 
        PhoneUIVoIPExtension, "CPhoneStateIncomingVoIP::ConstructL()");
    
    CPhoneIncoming::ConstructL();
    }


// -----------------------------------------------------------
// CPhoneStateIncomingVoIP::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateIncomingVoIP* CPhoneStateIncomingVoIP::NewL( 
    MPhoneStateMachine& aStateMachine, 
    MPhoneViewCommandHandle& aViewCommandHandle,
    MPhoneCustomization& aPhoneCustomization )
    {
    CPhoneStateIncomingVoIP* self = new (ELeave) CPhoneStateIncomingVoIP( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateIncomingVoIP::HandleAudioPlayStoppedL
// -----------------------------------------------------------
//
void CPhoneStateIncomingVoIP::HandleAudioPlayStoppedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncomingVoIP::HandleAudioPlayStoppedL()" );
    
    // Update the CBA
    CPhoneIncoming::HandleAudioPlayStoppedL();    
    }

// End of File
