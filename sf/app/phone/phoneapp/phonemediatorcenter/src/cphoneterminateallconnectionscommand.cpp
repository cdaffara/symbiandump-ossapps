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
* Description: Implementation of CPhoneTerminateAllConnectionsCommand class.
*
*/


// INCLUDE FILES
#include "phonelogger.h"
#include "cphoneterminateallconnectionscommand.h"
#include "pevirtualengine.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CPhoneTerminateAllConnectionsCommand::CPhoneTerminateAllConnectionsCommand
// ---------------------------------------------------------
//
CPhoneTerminateAllConnectionsCommand::CPhoneTerminateAllConnectionsCommand( 
    MPhoneEngineMessageSender& aEngineMessageSender ) : 
    iEngineMessageSender( aEngineMessageSender )
    {
    }

// ---------------------------------------------------------
// CPhoneTerminateAllConnectionsCommand::ConstructL
// ---------------------------------------------------------
//
void CPhoneTerminateAllConnectionsCommand::ConstructL()
    {
    }

// ---------------------------------------------------------
// CPhoneTerminateAllConnectionsCommand::NewL
// ---------------------------------------------------------
//
EXPORT_C CPhoneTerminateAllConnectionsCommand* CPhoneTerminateAllConnectionsCommand::NewL( 
    MPhoneEngineMessageSender& aEngineMessageSender )
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, "CPhoneTerminateAllConnectionsCommand::NewL( ) ");
    CPhoneTerminateAllConnectionsCommand* self = new( ELeave ) 
        CPhoneTerminateAllConnectionsCommand( aEngineMessageSender );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ---------------------------------------------------------
// CPhoneTerminateAllConnectionsCommand::~CPhoneTerminateAllConnectionsCommand
// ---------------------------------------------------------
//
CPhoneTerminateAllConnectionsCommand::~CPhoneTerminateAllConnectionsCommand()
    {
    }

// ---------------------------------------------------------
// CPhoneTerminateAllConnectionsCommand::ExecuteL
// ---------------------------------------------------------
//
void CPhoneTerminateAllConnectionsCommand::ExecuteLD()
    {
    __LOGMETHODSTARTEND( EPhoneMediatorCenter, 
        "CPhoneTerminateAllConnectionsCommand::ExecuteL( ) ");
    iEngineMessageSender.SendPhoneEngineMessage( 
        MPEPhoneModel::EPEMessageTerminateAllConnections );
    delete this;
    }

//  End of File  
