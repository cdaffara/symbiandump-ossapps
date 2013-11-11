/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Has the main entry point of clockserver.exe
*
*/

// System includes
#include <e32base.h>
#include <f32file.h>

// User includes
#include "clockservercommon.h"
#include "clockservermain.h"
#include "clockserver.h"

// Literals
_LIT( KLabelFault, "ClockServerThread-fault");

// ---------------------------------------------------------
// Fault(TSrvFault aFault)
// rest of the details are commented in the header
// ---------------------------------------------------------
//
GLDEF_C void Fault( TSrvFault aFault )
    {
    User::Panic( KLabelFault, aFault );
    }

// ---------------------------------------------------------
// TheServerThread( TAny* /*aPr*/ )
// rest of the details are commented in the header
// ---------------------------------------------------------
//
EXPORT_C TInt TheServerThread( TAny* /*aPr*/ )
    {
    RSemaphore semaphoreHandle;
	
	// Code for debug logs -START
	RFs fs;
	User::LeaveIfError(fs.Connect());
	_LIT(KLogPath,"C:\\logs\\clock\\");
	fs.MkDirAll(KLogPath);
	fs.Close();
    // Code for debug logs -END
	 
	// Check if a semaphore is already associated with ClockServer
    if( KErrNone != semaphoreHandle.OpenGlobal( KNameClockServerStartSemaphore ) )
        {
		// If no, then create one.
        if( KErrNone != semaphoreHandle.CreateGlobal( KNameClockServerStartSemaphore, 0 ) )
            {
			// There's a problem with creating the semaphore.
            Fault( EMainCreateSemaphore );
            }
        }
    else
        {
		// The global semaphore is already there.
        semaphoreHandle.Close();
        return KErrNone;
        }

    __UHEAP_MARK;		// Mark beginning of the HEAP
    
	TInt errCode = KErrNone ; 
	
	// Try renaming the current thread.
	errCode = User::RenameThread( KNameClockServer );
    __ASSERT_ALWAYS( errCode == KErrNone || errCode == KErrAlreadyExists, Fault( EMainClockServerThreadRename ) );

	// Construct and active scheduler
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    __ASSERT_ALWAYS( activeScheduler != NULL, Fault( EMainCreateScheduler ) );

	// Install the active scheduler
	CActiveScheduler::Install( activeScheduler );
	
    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    CClkSrvMain* clockServer = NULL;

	// Constructing the CClkSrvMain object, this inturn starts the server in CClkSrvMain::ConstructL -> StartL
    TRAPD( error, clockServer = CClkSrvMain::NewL() );
	__ASSERT_DEBUG( !error, Fault( EMainServerNotStarted ) );

    if( KErrNone == error )
        {
		// Rendezvous to the starter client to indicate successful start
		RProcess::Rendezvous( KErrNone );
		
		// Start the active scheduler
        CActiveScheduler::Start();
        }
	
	// Cleanup
    delete clockServer;
    delete CActiveScheduler::Current();
    delete trapCleanup;

    __UHEAP_MARKEND;	// Mark end of the HEAP

    // Close the handle to the RSemaphore
	semaphoreHandle.Close();

    return KErrNone;
    }

// ---------------------------------------------------------
// IsServerLoaded()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
LOCAL_C TBool IsServerLoaded()
    {
    TFindServer findServer( KNameClockServer );
    TFullName fullName;
    return( findServer.Next( fullName ) == KErrNone );
    }

// ---------------------------------------------------------
// E32Main()
// rest of the details are commented in the header
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return ( IsServerLoaded() ? KErrInUse : TheServerThread( NULL ) );
    }


// End of file
