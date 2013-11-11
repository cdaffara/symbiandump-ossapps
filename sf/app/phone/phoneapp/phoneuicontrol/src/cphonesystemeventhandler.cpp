/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CPhoneSystemEventHandler class.
*
*/


// INCLUDE FILES
#include <telinformationpskeys.h>
#include <telephonyvariant.hrh>
#include <startupdomainpskeys.h>
#include <hwrmdomainpskeys.h>

#include "cphonesystemeventhandler.h"
#include "mphonestate.h"
#include "mphonestatemachine.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneSystemEventHandler::CPhoneSystemEventHandler
// C++ default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPhoneSystemEventHandler::CPhoneSystemEventHandler(
    MPhoneStateMachine* aStateMachine ) :
    iStateMachine( aStateMachine )
    {
    }

// -----------------------------------------------------------
// CPhoneSystemEventHandler::NewL()
// Two-phased constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSystemEventHandler* CPhoneSystemEventHandler::NewL(
    MPhoneStateMachine* aStateMachine )
    {
    CPhoneSystemEventHandler* self =
        new (ELeave) CPhoneSystemEventHandler( aStateMachine );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// CPhoneSystemEventHandler::ConstructL()
// EPOC default constructor can leave.
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPhoneSystemEventHandler::ConstructL()
    {
    // Set up notifications for call state values
    CPhonePubSubProxy::Instance()->NotifyChangeL(
        KPSUidCtsyCallInformation,
        KCTsyCallState,
        this );
    
    // Set up notifications for Sim security state values.
    CPhonePubSubProxy::Instance()->NotifyChangeL(
        KPSUidStartup,
        KStartupSimSecurityStatus,
        this );
    }

// ---------------------------------------------------------
// CPhoneSystemEventHandler::CPhoneSystemEventHandler
// C++ default constructor
// (other items were commented in a header).
// ---------------------------------------------------------
//
CPhoneSystemEventHandler::~CPhoneSystemEventHandler()
    {
    }

// ---------------------------------------------------------
// CPhoneSystemEventHandler::HandlePhoneStartupL
// ---------------------------------------------------------
//
void CPhoneSystemEventHandler::HandlePhoneStartupL()
    {
    iStateMachine->State()->HandlePhoneStartupL();
    }

// -----------------------------------------------------------
// CPhoneSystemEventHandler::HandlePropertyChangedL
// -----------------------------------------------------------
//
void CPhoneSystemEventHandler::HandlePropertyChangedL(
    const TUid& aCategory,
    const TUint aKey,
    const TInt aValue )
    {
    iStateMachine->State()->HandlePropertyChangedL( aCategory, aKey, aValue );
    }

//  End of File
