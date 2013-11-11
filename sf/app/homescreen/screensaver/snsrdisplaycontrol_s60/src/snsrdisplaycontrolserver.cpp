/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of Screensaver Display Control Server
*
*/


#include "snsrdisplaycontrolserver.h"
#include "snsrdisplaycontrolsession.h"
#include "snsrdisplaycontrolcommon.h"

// =========== CONSTANTS =========== 


// ======== LOCAL FUNCTIONS ========
 
// ----------------------------------------------------------------------------
// Initialize and run the server
// ----------------------------------------------------------------------------
//
static void RunTheServerL()
    {
    // First create and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TInt err = User::RenameThread( KSnsrDispCtrlSrvName );
    User::LeaveIfError( err );
    
    // Create the server
    CSnsrDisplayControlServer* server = CSnsrDisplayControlServer::NewLC();
    server->StartL( KSnsrDispCtrlSrvName );
    
    // Signal the client the startup is complete
    RProcess::Rendezvous(KErrNone);

    // Enter the wait loop
    CActiveScheduler::Start();

    // Exited cleanup scheduler and server
    CleanupStack::PopAndDestroy( server );
    CleanupStack::PopAndDestroy( scheduler );
    }

// ----------------------------------------------------------------------------
// Main entry-point for the server thread/process
// ----------------------------------------------------------------------------
//
static TInt RunTheServer()
    {
    CTrapCleanup* cleanup = CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
        {
        TRAP( r, RunTheServerL() );
        delete cleanup;
        }
    
    return (r);
    }


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlServer::CSnsrDisplayControlServer() :
    CServer2( EPriorityStandard, CServer2::TServerType( EIpcSession_Sharable ) )
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CSnsrDisplayControlServer::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlServer* CSnsrDisplayControlServer::NewLC()
    {
    CSnsrDisplayControlServer* self =  new (ELeave) CSnsrDisplayControlServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSnsrDisplayControlServer::~CSnsrDisplayControlServer()
    {
    }

// ---------------------------------------------------------------------------
// Create server side session object
// ---------------------------------------------------------------------------
//
CSession2* CSnsrDisplayControlServer::NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage ) const
    {
    TVersion version( KSnsrDispCtrlSrvVerMajor, 
                      KSnsrDispCtrlSrvVerMinor, 
                      KSnsrDispCtrlSrvVerBuild );
    if ( !User::QueryVersionSupported( version, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    
    // Allow connections only from HbDeviceDialogAppServer. Essentially, we are a
    // capability proxy which grants access to API normally needing WriteDeviceData
    // capability to a process which doesn't have it. Use access control
    // of our own to prevent maluse. This module is not part of SDK or even platform API
    // and only supposed to be used internally by Screensaver, so preventing access
    // from other processes should not be a problem.
    static _LIT_SECURITY_POLICY_S0(hbDeviceDialogSidPolicy, 0x20022FC5);
    TBool passed = hbDeviceDialogSidPolicy().CheckPolicy(aMessage);
    if ( !passed )
        {
        User::Leave( KErrPermissionDenied );
        }

    CSession2* session;
    session = CSnsrDisplayControlSession::NewL();
    session->SetServer( this );
    return session;
    }


// ======== GLOBAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// Process entry point 
// ----------------------------------------------------------------------------
TInt E32Main()
    {
    return RunTheServer();
    }

