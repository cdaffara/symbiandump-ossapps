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
// Phonebook Sync Multiple Phonebook Unit Test server test code.
// 
//

/**
 @file
 @internalComponent
*/

#include "TE_PhBkSyncServer.h"
#include "TE_PhBkSyncBase.h"
#include "TestConnectSteps.h"
#include "TestDeleteContactSteps.h"
#include "TestDoSynchronisationSteps.h"
#include "TestGetContactFormatSteps.h"
#include "TestGetSyncModeSteps.h"
#include "TestOpenSteps.h"
#include "TestOtherSteps.h"
#include "TestSetSyncModeSteps.h"
#include "TestValidateContactSteps.h"
#include "TestVersionSteps.h"
#include "TestWriteContactSteps.h"
#include "TestReadingIniFile.h"

_LIT(KServerName,"TE_PhBkSync");


CPhBkSyncTestServer::~CPhBkSyncTestServer()
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
	} // CPhBkSyncTestServer::~CSyncTestServer


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CPhBkSyncTestServer* CPhBkSyncTestServer::NewL()
	{
	CPhBkSyncTestServer*  server = new(ELeave) CPhBkSyncTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CPhBkSyncTestServer::NewL


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
	CPhBkSyncTestServer* server = NULL;

	TRAPD(err, server = CPhBkSyncTestServer::NewL());
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
CTestStep* CPhBkSyncTestServer::CreateTestStep(const TDesC& aStepName)
	{
    //
	// Create the required test step...
	//
	CTestStep*  testStep = NULL;

	// OOM test
	if (aStepName == _L("PhbkSyncOOMConnectTest_01"))
		{
		testStep = new CPhbkSyncOOMConnectTest_01;
		}
	else if (aStepName == _L("PhbkSyncOOMDoSynchronisationTest_01"))
		{
		testStep = new CPhbkSyncOOMDoSynchronisationTest_01;
		}
	// step to test the delayed DoSync request. That is, following
	// execution sequence will be tested:
	// 1. ::DoSynchronisationL(...)
	// 2. ::CompleteGetPhoneStoreInfo(...)
	//
	else if (aStepName == _L("PhbkSyncSubscriberIdTest_01"))
		{
		testStep = new CPhbkSyncSubscriberIdTest_01;
		}
	// 1. API connect() related test steps
	else if (aStepName == _L("PhbkSyncConnectTest_01"))
		{
		testStep = new CPhbkSyncConnectTest_01;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_02"))
		{
		testStep = new CPhbkSyncConnectTest_02;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_03"))
		{
		testStep = new CPhbkSyncConnectTest_03;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_04"))
		{
		testStep = new CPhbkSyncConnectTest_04;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_05"))
		{
		testStep = new CPhbkSyncConnectTest_05;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_06"))
		{
		testStep = new CPhbkSyncConnectTest_06;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_07"))
		{
		testStep = new CPhbkSyncConnectTest_07;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_08"))
		{
		testStep = new CPhbkSyncConnectTest_08;
		}
	else if (aStepName == _L("PhbkSyncConnectTest_09"))
		{
		testStep = new CPhbkSyncConnectTest_09;
		}
	// 2. API Close() related test steps
	else if (aStepName == _L("PhbkSyncOpenTest_01"))
		{
		testStep = new CPhbkSyncOpenTest_01;
		}
	// 3. API Version() related test steps
	else if (aStepName == _L("PhbkSyncVersionTest_01"))
		{
		testStep = new CPhbkSyncVersionTest_01;
		}
	// 4. API DoSynchronisation() related test steps
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_01"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_01;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_02"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_02;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_03"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_03;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_04"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_04;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_05"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_05;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_06"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_06;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_07"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_07;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_08"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_08;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_09"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_09;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_10"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_10;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_11"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_11;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_12"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_12;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_13"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_13;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_14"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_14;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_15"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_15;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_16"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_16;  // removed due to unclear spec.
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_17"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_17;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_18"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_18;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_19"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_19;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_20"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_20;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_21"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_21;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_22"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_22;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_23"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_23;  // removed due to unrealistic spec.
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_24"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_24;  // removed due to unrealistic spec.
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_25"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_25;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_26"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_26;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_27"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_27;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_28"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_28;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_29"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_29;
		}
	else if (aStepName == _L("PhbkSyncDoSynchronisationTest_30"))
		{
		testStep = new CPhbkSyncDoSynchronisationTest_30;
		}
	// 5. API ValidateContact() related test steps
	else if (aStepName == _L("PhbkSyncValidateContactTest_01"))
		{
		testStep = new CPhbkSyncValidateContactTest_01;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_02"))
		{
		testStep = new CPhbkSyncValidateContactTest_02;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_03"))
		{
		testStep = new CPhbkSyncValidateContactTest_03;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_04"))
		{
		testStep = new CPhbkSyncValidateContactTest_04;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_05"))
		{
		testStep = new CPhbkSyncValidateContactTest_05;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_06"))
		{
		testStep = new CPhbkSyncValidateContactTest_06;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_07"))
		{
		testStep = new CPhbkSyncValidateContactTest_07;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_08"))
		{
		testStep = new CPhbkSyncValidateContactTest_08;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_09"))
		{
		testStep = new CPhbkSyncValidateContactTest_09;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_10"))
		{
		testStep = new CPhbkSyncValidateContactTest_10;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_11"))
		{
		testStep = new CPhbkSyncValidateContactTest_11;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_12"))
		{
		testStep = new CPhbkSyncValidateContactTest_12;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_13"))
		{
		testStep = new CPhbkSyncValidateContactTest_13;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_14"))
		{
		testStep = new CPhbkSyncValidateContactTest_14;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_15"))
		{
		testStep = new CPhbkSyncValidateContactTest_15;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_16"))
		{
		testStep = new CPhbkSyncValidateContactTest_16;
		}
	else if (aStepName == _L("PhbkSyncValidateContactTest_17"))
		{
		testStep = new CPhbkSyncValidateContactTest_17;
		}
	// 6. API GetContactFormat related test steps
	else if (aStepName == _L("PhbkSyncGetContactFormatTest_01"))
		{
		testStep = new CPhbkSyncGetContactFormatTest_01;
		}
	// 7. API WriteContact() related test steps
	else if (aStepName == _L("PhbkSyncWriteContactTest_01"))
		{
		testStep = new CPhbkSyncWriteContactTest_01;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_02"))
		{
		testStep = new CPhbkSyncWriteContactTest_02;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_03"))
		{
		testStep = new CPhbkSyncWriteContactTest_03;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_04"))
		{
		testStep = new CPhbkSyncWriteContactTest_04;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_05"))
		{
		testStep = new CPhbkSyncWriteContactTest_05;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_06"))
		{
		testStep = new CPhbkSyncWriteContactTest_06;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_07"))
		{
		testStep = new CPhbkSyncWriteContactTest_07;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_08"))
		{
		testStep = new CPhbkSyncWriteContactTest_08;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_09"))
		{
		testStep = new CPhbkSyncWriteContactTest_09;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_10"))
		{
		testStep = new CPhbkSyncWriteContactTest_10;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_11"))
		{
		testStep = new CPhbkSyncWriteContactTest_11;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_12"))
		{
		testStep = new CPhbkSyncWriteContactTest_12;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_13"))
		{
		testStep = new CPhbkSyncWriteContactTest_13;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_14"))
		{
		testStep = new CPhbkSyncWriteContactTest_14;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_15"))
		{
		testStep = new CPhbkSyncWriteContactTest_15;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_16"))
		{
		testStep = new CPhbkSyncWriteContactTest_16;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_17"))
		{
		testStep = new CPhbkSyncWriteContactTest_17;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_18"))
		{
		testStep = new CPhbkSyncWriteContactTest_18;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_19"))
		{
		testStep = new CPhbkSyncWriteContactTest_19;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_20"))
		{
		//testStep = new CPhbkSyncWriteContactTest_20;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_21"))
		{
		//testStep = new CPhbkSyncWriteContactTest_21;
		}
	else if (aStepName == _L("PhbkSyncWriteContactTest_22"))
		{
		testStep = new CPhbkSyncWriteContactTest_22;
		}
	// 8. API DeleteContact() related test steps
	else if (aStepName == _L("PhbkSyncDeleteContactTest_01"))
		{
		testStep = new CPhbkSyncDeleteContactTest_01;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_02"))
		{
		testStep = new CPhbkSyncDeleteContactTest_02;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_03"))
		{
		testStep = new CPhbkSyncDeleteContactTest_03;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_04"))
		{
		testStep = new CPhbkSyncDeleteContactTest_04;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_05"))
		{
		testStep = new CPhbkSyncDeleteContactTest_05;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_06"))
		{
		testStep = new CPhbkSyncDeleteContactTest_06;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_07"))
		{
		testStep = new CPhbkSyncDeleteContactTest_07;
		}
	else if (aStepName == _L("PhbkSyncDeleteContactTest_08"))
		{
		//testStep = new CPhbkSyncDeleteContactTest_08;
		}
	// 9. API GetSyncMode() related test steps
	else if (aStepName == _L("PhbkSyncGetSyncModeTest_01"))
		{
		testStep = new CPhbkSyncGetSyncModeTest_01;
		}
	// 10. API SetSyncMode() related test steps
	else if (aStepName == _L("PhbkSyncSetSyncModeTest_01"))
		{
		testStep = new CPhbkSyncSetSyncModeTest_01;
		}
	else if (aStepName == _L("PhbkSyncSetSyncModeTest_02"))
		{
		testStep = new CPhbkSyncSetSyncModeTest_02;
		}
	else if (aStepName == _L("PhbkSyncSetSyncModeTest_03"))
		{
		testStep = new CPhbkSyncSetSyncModeTest_03;
		}
	// 11. other test steps
	else if (aStepName == _L("PhbkSyncOtherTest_01"))
		{
		testStep = new CPhbkSyncOtherTest_01;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_02"))
		{
		testStep = new CPhbkSyncOtherTest_02;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_03"))
		{
		testStep = new CPhbkSyncOtherTest_03;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_04"))
		{
		testStep = new CPhbkSyncOtherTest_04;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_05"))
		{
		testStep = new CPhbkSyncOtherTest_05;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_06"))
		{
		testStep = new CPhbkSyncOtherTest_06;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_07"))
		{
		testStep = new CPhbkSyncOtherTest_07;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_08"))
		{
		testStep = new CPhbkSyncOtherTest_08;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_09"))
		{
		testStep = new CPhbkSyncOtherTest_09;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_10"))
		{
		testStep = new CPhbkSyncOtherTest_10;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_11"))
		{
		testStep = new CPhbkSyncOtherTest_11;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_12"))
		{
		testStep = new CPhbkSyncOtherTest_12;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_13"))
		{
		testStep = new CPhbkSyncOtherTest_13;
		}
	else if (aStepName == _L("PhbkSyncOtherTest_14"))
		{
		testStep = new CPhbkSyncOtherTest_14;
		}
	// 12.	Test Server ability to detect corrupt, missing, or zero length ini file
	else if (aStepName == _L("PhbkSyncIniFileTest"))
		{
		testStep = new CPhbkSyncIniFileTest;
		}
	
   	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	} // CPhBkSyncTestServer::CreateTestStep
