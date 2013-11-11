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
* Description:  CS Server Startup module
*
*/


// INCLUDE FILES
#include <ccsdefs.h>

#include "ccsdebug.h"
#include "ccsserver.h"

// ----------------------------------------------------------------------------
// Initialize and run the server
// ----------------------------------------------------------------------------
static void RunTheServerL()
{
    // creates log directory (Cs) and log file (CsServer.txt) on app startup.
    // prints starting banner...
           
    PRINT ( _L("Enter RunTheServerL") );

    // First create and install the active scheduler
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    CleanupStack::Pop(scheduler);
    
    // Create the server
    CCsServer* server = CCsServer::NewL();
  
    // Signal the client the startup is complete
    RProcess::Rendezvous(KErrNone);

    // Enter the wait loop
    CActiveScheduler::Start();

    // Exited cleanup scheduler and server
    delete server;
    delete scheduler;

    REComSession::FinalClose();
    
    PRINT ( _L("End RunTheServerL") );
}


// ----------------------------------------------------------------------------
// Main entry-point for the server thread/process
// ----------------------------------------------------------------------------
static TInt RunTheServer()
{
    PRINT ( _L("Enter RunTheServer") );
    
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r = KErrNoMemory;
    if (cleanup)
    {
		TRAP(r,RunTheServerL());
		delete cleanup;
    }
    
    PRINT ( _L("End RunTheServer") );
    
    return (r);
}


// ----------------------------------------------------------------------------
// Process entry point 
// ----------------------------------------------------------------------------
TInt E32Main()
{
    return RunTheServer();
}
