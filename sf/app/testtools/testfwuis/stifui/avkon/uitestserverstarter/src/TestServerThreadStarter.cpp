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
* Description: This module contains the implementation of 
* CTestServerThreadStarter class member functions.
*
*/

// INCLUDE FILES
#include "TestServerThreadStarter.h"
#include <stifinternal/TestServerClient.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
// None

// CONSTANTS
// None

// MACROS
// None

/**
 * CTestServerThreadStarter thread heap size.
 */
const TUint KDefaultHeapSize = 0x10000;     // 64 K

/**
 * CTestServerThreadStarter thread max heap size.
 */
const TUint KMaxHeapSize     = 0x20000;     // 128 K 

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
// None


// ================= MEMBER FUNCTIONS =========================================

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: TestServerStarterThreadFunction

     Description: CTestServerThreadStarter thread function.

	 CTestServerThreadStarter thread function in which testserver creation code
	 is executed.

     Parameters: TAny* aParameters: in: Pointer to CTestServerThreadStarter object
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestServerThreadStarter::TestServerStarterThreadFunction( TAny* aParameters )
	{
	RDebug::Printf( "UITESTING: CTestServerThreadStarter::TestServerStarterThreadFunction" );
    
    CTestServerThreadStarter* testServerStarter = (CTestServerThreadStarter*)aParameters;
    TThreadId mainThreadId = testServerStarter->iMainThreadId;
    CTestThreadContainerRunnerFactory* testThreadContainerRunnerFactory = 
    	testServerStarter->iTestThreadContainerRunnerFactory;
    
   	// Signal that all needed data is copied and main thread can continue its execution 
    testServerStarter->iServerThreadStartedSemaphore.Signal();
    	
    // Get module name from command line
	const TInt length = User().CommandLineLength();

    HBufC* cmdLine = HBufC::New( length );
    
    if ( cmdLine == NULL )
        {
        return KErrNoMemory;
        }

    TPtr moduleName = cmdLine->Des();

	User().CommandLine( moduleName );

    RDebug::Print(_L("CTestServerThreadStarter::TestServerStarterThreadFunction() Received data [%S]"), &moduleName);

    // Extract semaphore name passed in data
    TInt index = moduleName.Find(_L(" "));
    RDebug::Print(_L("CTestServerThreadStarter::TestServerStarterThreadFunction() Space separator found at position [%d]"), index);
    TPtrC semaphoreName = moduleName.Mid(index + 1);
    moduleName = moduleName.Left(index);

    RDebug::Print(_L("CTestServerThreadStarter::TestServerStarterThreadFunction() Extracted module name [%S] and sempahore name [%S]"), &moduleName, &semaphoreName);

    // Open start-up synchronization semaphore
    RSemaphore startup;
    RDebug::Print(_L(" Openingstart-up semaphore"));
    //TName semaphoreName = _L("startupSemaphore");
    //semaphoreName.Append( moduleName );
    
    TInt res = startup.OpenGlobal(semaphoreName);
    RDebug::Print(_L("Opening result %d"), res);    


    TFileName serverName;
    TInt r = StartNewServer ( moduleName, serverName, EFalse, startup, true, testThreadContainerRunnerFactory );

    if ( r ==   KErrAlreadyExists )
        {        
        // Ok, server was already started
        RDebug::Print(_L("UI TestServer already started, signaling semaphore and exiting"));
        startup.Signal();        

        delete cmdLine;

        return KErrNone;
        }
    else
        {       
        RDebug::Print(_L("UI TestServer is finished, code %d"), r);
        }

    delete cmdLine;
    
    //delete testThreadContainerRunnerFactory;
    testThreadContainerRunnerFactory = NULL;
    
    // Kill main thread to end UITestServerStarter application
    // when testserver is finished    
    RThread mainThread;
    TInt ret = mainThread.Open( mainThreadId );
	if( ret != KErrNone )
		{
		User::Panic( _L("ThreadHandleOpenError"), ret );
		}
    mainThread.Kill( KErrNone );
    
    return r;	
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: NewL

     Description: NewL is first phase of two-phased constructor.

	 NewL is first phase of two-phased constructor.

     Parameters: None.
     
     Return Values: Pointer to new CTestServerThreadStarter object.

     Errors/Exceptions: Leaves if new or ConstructL leaves.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestServerThreadStarter* CTestServerThreadStarter::NewL( )
	{
	CTestServerThreadStarter* self = new(ELeave) CTestServerThreadStarter();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: CTestServerThreadStarter

     Description: Default constructor.

	 Default constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestServerThreadStarter::CTestServerThreadStarter()
:CActive( EPriorityNormal )
	{
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: NewL

     Description: Default destructor.

	 Default destructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
CTestServerThreadStarter::~CTestServerThreadStarter()
	{
	Cancel();
	iTimer.Close();
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: ConstructL

     Description: Second phase of two-phased constructor.

	 Second phase of two-phased constructor.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestServerThreadStarter::ConstructL()
	{
	TInt ret = iTimer.CreateLocal();
	if ( ret != KErrNone )
		{
		User::Leave( ret );
		}

	CActiveScheduler::Add( this );
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: RunTestServerThread

     Description: Starts-up testserver.

	 Starts-up testserver.

     Parameters: None.
     
     Return Values: KErrNone when there was no error.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
TInt CTestServerThreadStarter::RunTestServerThread( CTestThreadContainerRunnerFactory* aTestThreadContainerRunnerFactory )
	{
	if ( IsActive() )
		{
		User::Panic( _L("E32USER-CBase"), 42 );
		}
		
	TInt ret = iServerThreadStartedSemaphore.CreateLocal( 0 );
	if ( ret != KErrNone )
		{
		User::Leave( ret );
		}
	
	iTestThreadContainerRunnerFactory = aTestThreadContainerRunnerFactory;
	iMainThreadId = RThread().Id();
	iReturnCode = KErrNone;
	
	iStatus = KRequestPending;

	SetActive();
	iTimer.After( iStatus, 0 );
	
	return KErrNone;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: RunL

     Description: RunL derived from CActive handles the completed requests.

	 RunL derived from CActive handles the completed requests.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: Leaves if one of the called method leavs.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestServerThreadStarter::RunL()
	{
	RThread testServerStarterThread;	
	TInt ret = testServerStarterThread.Create( _L("testserverstarterthread"), 
			TestServerStarterThreadFunction, 10 * KDefaultStackSize, 10 * KDefaultHeapSize, 10 * KMaxHeapSize, this );
	
	User::LeaveIfError( ret );
		
	testServerStarterThread.Resume();	
	testServerStarterThread.Close();
	
	iServerThreadStartedSemaphore.Wait();
	iServerThreadStartedSemaphore.Close();
	
	// Delete CTestServerThreadStarter after testserver is started
	delete this;
	}

/*
-------------------------------------------------------------------------------

     Class: CTestServerThreadStarter

     Method: DoCancel

     Description: DoCancel derived from CActive handles the Cancel.

	 DoCancel derived from CActive handles the Cancel.

     Parameters: None.
     
     Return Values: None.

     Errors/Exceptions: None.

     Status: Draft
    
-------------------------------------------------------------------------------
*/
void CTestServerThreadStarter::DoCancel()
	{
	if ( IsActive() )
		{
		iTimer.Cancel();
		}
	}

// End of File
