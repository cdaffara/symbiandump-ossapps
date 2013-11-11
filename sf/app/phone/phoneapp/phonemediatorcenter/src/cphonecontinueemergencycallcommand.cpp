/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneContinueEmergencyCallCommand.
*
*/


// INCLUDE FILES
#include "phonelogger.h"
#include "cphonecontinueemergencycallcommand.h"
#include "pevirtualengine.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneContinueEmergencyCallCommand::CPhoneContinueEmergencyCallCommand
// ---------------------------------------------------------
//
CPhoneContinueEmergencyCallCommand::CPhoneContinueEmergencyCallCommand( 
    MPhoneEngineMessageSender& aEngineMessageSender ) : 
    iEngineMessageSender( aEngineMessageSender )
    {
    }

// ---------------------------------------------------------
// CPhoneContinueEmergencyCallCommand::ConstructL
// ---------------------------------------------------------
//
void CPhoneContinueEmergencyCallCommand::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneContinueEmergencyCallCommand::NewL
// ---------------------------------------------------------
//
EXPORT_C CPhoneContinueEmergencyCallCommand* CPhoneContinueEmergencyCallCommand::NewL( 
    MPhoneEngineMessageSender& aEngineMessageSender )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneContinueEmergencyCallCommand::NewL( ) ");
    CPhoneContinueEmergencyCallCommand* self = new( ELeave ) 
        CPhoneContinueEmergencyCallCommand( aEngineMessageSender );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneContinueEmergencyCallCommand::~CPhoneContinueEmergencyCallCommand
// ---------------------------------------------------------
//
CPhoneContinueEmergencyCallCommand::~CPhoneContinueEmergencyCallCommand()
    {
    }

// ---------------------------------------------------------
// CPhoneContinueEmergencyCallCommand::ExecuteLD
// ---------------------------------------------------------
//
void CPhoneContinueEmergencyCallCommand::ExecuteLD()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, 
        "CPhoneContinueEmergencyCallCommand::ExecuteL( ) ");
    iEngineMessageSender.SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageContinueEmergencyCallInitialization );
    delete this;
    }

//  End of File  
