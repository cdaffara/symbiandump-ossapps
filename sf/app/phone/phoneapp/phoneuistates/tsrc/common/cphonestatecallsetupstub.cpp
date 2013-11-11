/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDES
#include <FeatMgr.h>
#include <StringLoader.h>
#include <AknUtils.h>
#include <mpeengineinfo.h>
#include <mpeclientinformation.h>

#include "CPhonePubSubProxy.h"
#include "CPhoneStateCallSetupstub.h"
#include "CPhoneStateInCall.h"
#include "MPhoneStateMachine.h"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamCallStateData.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamGlobalNote.h"
#include "PhoneStateDefinitions.h"
#include "PhoneUI.hrh"
#include "PhoneRssBase.h"
#include "CPhoneMainResourceResolver.h"
#include "PhoneLogger.h"
#include "PhoneUI.pan"
#include "TPhoneCmdParamAudioOutput.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateCallSetup::CPhoneStateCallSetup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aCustomization ) : 
    CPhoneState( aStateMachine, aViewCommandHandle, aCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::~CPhoneStateCallSetup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateCallSetup::~CPhoneStateCallSetup()
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::ConstructL()
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateCallSetup* CPhoneStateCallSetup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateCallSetup* self = new (ELeave) CPhoneStateCallSetup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleCommandL()
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneStateCallSetup::HandleCommandL( TInt aCommand )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleKeyEventL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandleKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aEventCode )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aScanCode )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleConnectedL( TInt aCallId )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleIdleL( TInt aCallId )
    {
    }
    
// -----------------------------------------------------------------------------
// CPhoneStateCallSetup::HandleAudioOutputChangedL
// -----------------------------------------------------------------------------
//    
void CPhoneStateCallSetup::HandleAudioOutputChangedL()
    {
    }
     
void CPhoneStateCallSetup::HandleCreateNumberEntryL(struct TKeyEvent const &, enum TEventCode) 
    {}

void CPhoneStateCallSetup::OpenMenuBarL(void)
    {}

void CPhoneStateCallSetup::UpdateCbaL(int)
    {}

void CPhoneStateCallSetup::UpdateInCallCbaL(void) 
    {}

void CPhoneStateCallSetup::HandleNumberEntryClearedL(void)
    {}
