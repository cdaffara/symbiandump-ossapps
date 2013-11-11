// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Phonebook Sync Single Phonebook Unit Test server test code.
// 
//

/**
 @file
 @internalComponent
*/

#include "TE_SyncServer.h"
#include "TE_SyncBase.h"
#include "TE_Sync.h"

_LIT(KServerName,"TE_Sync");


CSyncTestServer::~CSyncTestServer()
	{
	//
	// Shutdown the server if possible (only works in debug mode).
	//
	RPhoneBookSession  phoneBookSession;
	TInt  ret;
	
	ret = phoneBookSession.Connect();
	if (ret == KErrNone)
		{
		phoneBookSession.ShutdownServer(EFalse);
		phoneBookSession.Close();
		}
	} // CSyncTestServer::~CSyncTestServer


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CSyncTestServer* CSyncTestServer::NewL()
	{
	CSyncTestServer*  server = new(ELeave) CSyncTestServer();
	CleanupStack::PushL(server);
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CSyncTestServer::NewL


/**
 *  Uses the new Rendezvous() call to sync with the client.
 */
LOCAL_C void MainL()
	{
	//
	// Start an active scheduler...
	//
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	//
	// Ensure the SIMTSY config number is reset prior to starting...
	//
	TInt  result;
	
	result = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (result != KErrNone  &&  result != KErrNotFound)
		{
		User::Leave(result);
		}

	//
	// Create the CTestServer derived server...
	//
	CSyncTestServer* server = NULL;

	TRAPD(err, server = CSyncTestServer::NewL());
	if (err == KErrNone)
		{
		//
		// Sync with the client and enter the active scheduler
		//
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	//
	// Clean up...
	//
	CleanupStack::Pop(scheduler);
	delete server;
	delete scheduler;
	} // MainL

/**
 *  @return Standard Epoc error code on exit.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup*  cleanup = CTrapCleanup::New();

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;

	return err;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CSyncTestServer::CreateTestStep(const TDesC& aStepName)
	{
    //
	// Create the required test step...
	//
	CTestStep*  testStep = NULL;

	if (aStepName == _L("TestOpen"))
		{
		testStep = new CTestOpen();
		}

	if (aStepName == _L("TestDoSynchronisation"))
		{
		testStep = new CTestDoSynchronisation();
		}

	if (aStepName == _L("TestWriteContactToICCTest"))
		{
		testStep = new CTestWriteContactToICCTest();
		}

	if (aStepName == _L("TestWriteContactToICCOverwriteTest"))
		{
		testStep = new CTestWriteContactToICCOverwriteTest();
		}

	if (aStepName == _L("TestWriteContactToICCNoSlotTest"))
		{
		testStep = new CTestWriteContactToICCNoSlotTest();
		}

	if (aStepName == _L("TestWriteContactToICCInvalidSlotTest"))
		{
		testStep = new CTestWriteContactToICCInvalidSlotTest();
		}

	if (aStepName == _L("TestWriteContactToICCBlankNumTest"))
		{
		testStep = new CTestWriteContactToICCBlankNumTest();
		}

	if (aStepName == _L("TestWriteContactToICCBlankNameTest"))
		{
		testStep = new CTestWriteContactToICCBlankNameTest();
		}

	if (aStepName == _L("TestWriteContactToICCExceedMaxNumTest"))
		{
		testStep = new CTestWriteContactToICCExceedMaxNumTest();
		}

	if (aStepName == _L("TestWriteContactToICCExceedMaxNameTest"))
		{
		testStep = new CTestWriteContactToICCExceedMaxNameTest();
		}

	if (aStepName == _L("TestWriteContactToICCMaxNumTest"))
		{
		testStep = new CTestWriteContactToICCMaxNumTest();
		}

	if (aStepName == _L("TestWriteContactToICCMaxNameTest"))
		{
		testStep = new CTestWriteContactToICCMaxNameTest();
		}

	if (aStepName == _L("TestWriteContactToICCNumLettersTest"))
		{
		testStep = new CTestWriteContactToICCNumLettersTest();
		}

	if (aStepName == _L("TestWriteContactToICCObsecureTest"))
		{
		testStep = new CTestWriteContactToICCObsecureTest();
		}

	if (aStepName == _L("TestValidateContact"))
		{
		testStep = new CTestValidateContact();
		}

	if (aStepName == _L("TestValidateContactAccessLockedTest"))
		{
		testStep = new CTestValidateContactAccessLockedTest();
		}

	if (aStepName == _L("TestValidateContactInvalidUIDTest"))
		{
		testStep = new CTestValidateContactInvalidUIDTest();
		}

	if (aStepName == _L("TestUpdateLookUpTableFailure"))
		{
		testStep = new CTestUpdateLookUpTableFailure();
		}

	if (aStepName == _L("TestUpdateLookUpTableSuccess"))
		{
		testStep = new CTestUpdateLookUpTableSuccess();
		}

	if (aStepName == _L("TestGetSyncMode"))
		{
		testStep = new CTestGetSyncMode();
		}

	if (aStepName == _L("TestSetSyncMode"))
		{
		testStep = new CTestSetSyncMode();
		}

	if (aStepName == _L("TestDeleteContactFromICC"))
		{
		testStep = new CTestDeleteContactFromICC();
		}

	if (aStepName == _L("TestDeleteContactInvalidIdFromICC"))
		{
		testStep = new CTestDeleteContactInvalidIdFromICC();
		}

	if (aStepName == _L("TestGetContactFormat"))
		{
		testStep = new CTestGetContactFormat();
		}

	if (aStepName == _L("TestGetPhoneBookCacheState"))
		{
		testStep = new CTestGetPhoneBookCacheState();
		}

	if (aStepName == _L("TestVersionTest"))
		{
		testStep = new CTestVersionTest();
		}

	if (aStepName == _L("TestGetLastSyncError"))
		{
		testStep = new CTestGetLastSyncError();
		}

	if (aStepName == _L("TestNotifyCacheStateChange"))
		{
		testStep = new CTestNotifyCacheStateChange();
		}

	if (aStepName == _L("TestGetPhoneBookId"))
		{
		testStep = new CTestGetPhoneBookId();
		}

	if (aStepName == _L("TestReset"))
		{
		testStep = new CTestReset();
		}

	if (aStepName == _L("TestSlotHandlingTest"))
		{
		testStep = new CTestSlotHandlingTest();
		}

	if (aStepName == _L("TestWriteToSlotTest"))
		{
		testStep = new CTestWriteToSlotTest();
		}

	if (aStepName == _L("TestCancelWriteRequest"))
		{
		testStep = new CTestCancelWriteRequest();
		}

	if (aStepName == _L("TestCancelDeleteRequest"))
		{
		testStep = new CTestCancelDeleteRequest();
		}

	if (aStepName == _L("TestCancelDoSyncRequest"))
		{
		testStep = new CTestCancelDoSyncRequest();
		}

	if (aStepName == _L("TestConnectionTest"))
		{
		testStep = new CTestConnectionTest();
		}

	if (aStepName == _L("TestFailedPhoneStartup"))
		{
		testStep = new CTestFailedPhoneStartup();
		}

	return testStep;
	} // CSyncTestServer::CreateTestStep
