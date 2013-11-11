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
* Description: Application startup main implementation.
*
*/

//  Include Files  
#include "mylocationlogger.h"
#include "appmain.h"
#include "mylocationsengine.h"
#include <e32base.h>
#include <e32std.h>

//  Local Functions
// -----------------------------------------------------------------------------
// DoExecuteL()
// 
// -----------------------------------------------------------------------------
//
static void DoExecuteL()
{
    __TRACE_CALLSTACK;// Create and install active scheduler
    CActiveScheduler* scheduler = NULL;

    scheduler = new (ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);
    
    // create Mylocations engine object
    CMyLocationsEngine *myLocationsEngine = CMyLocationsEngine::NewL();
    CleanupStack::PushL( myLocationsEngine );

    MYLOCLOGSTRING("myLocationsEngine created.");
    
    RProcess::Rendezvous(KErrNone);
    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy(myLocationsEngine);
    // Pop and destroy the scheduler
    CleanupStack::PopAndDestroy(scheduler);
   
}

// -----------------------------------------------------------------------------
// Execute()
// 
// -----------------------------------------------------------------------------
//
static TInt Execute()
{  
    TInt error = KErrNoMemory;
    // Create the cleanup stack
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if (cleanup)
    {
        // Initialize and start the server
        TRAP( error, DoExecuteL() );
        delete cleanup;
    }
    return error;
}

GLDEF_C TInt E32Main()
{
    return Execute();
}

