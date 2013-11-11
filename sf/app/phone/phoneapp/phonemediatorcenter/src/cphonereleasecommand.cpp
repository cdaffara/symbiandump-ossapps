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
* Description: Implementation of CPhoneReleaseCommand class.
*
*/


// INCLUDE FILES
#include "phonelogger.h"
#include "cphonereleasecommand.h"
#include "pevirtualengine.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneReleaseCommand::CPhoneReleaseCommand
// ---------------------------------------------------------
//
CPhoneReleaseCommand::CPhoneReleaseCommand( 
    MPhoneEngineMessageSender& aEngineMessageSender ) : 
    iEngineMessageSender( aEngineMessageSender )
    {
    }

// ---------------------------------------------------------
// CPhoneReleaseCommand::ConstructL
// ---------------------------------------------------------
//
void CPhoneReleaseCommand::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneReleaseCommand::NewL
// ---------------------------------------------------------
//
EXPORT_C CPhoneReleaseCommand* CPhoneReleaseCommand::NewL( 
    MPhoneEngineMessageSender& aEngineMessageSender )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneReleaseCommand::NewL( ) ");
    CPhoneReleaseCommand* self = new( ELeave ) 
        CPhoneReleaseCommand( aEngineMessageSender );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneReleaseCommand::~CPhoneReleaseCommand
// ---------------------------------------------------------
//
CPhoneReleaseCommand::~CPhoneReleaseCommand()
    {
    }

// ---------------------------------------------------------
// CPhoneReleaseCommand::ExecuteL
// ---------------------------------------------------------
//
void CPhoneReleaseCommand::ExecuteLD()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, 
        "CPhoneReleaseCommand::ExecuteL( ) ");
    iEngineMessageSender.SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageRelease );
    delete this;
    }

//  End of File  
