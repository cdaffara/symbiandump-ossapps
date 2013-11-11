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
//
 
#include "TestCalInterimApiDCSuiteServer.h"

#include "TestCalInterimApiDefines.h"
#include "TestCalInterimApiDCStep.h"
#include "TestCalInterimApiDCCreateEntries.h"

_LIT(KServerName,"calinterimapiunittestserver");
	
CTestCalInterimApiDCSuite* CTestCalInterimApiDCSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestCalInterimApiDCSuite * server = new (ELeave) CTestCalInterimApiDCSuite();
	CleanupStack::PushL(server);

	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	server->StartL(KServerName);
	//server->ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}

// Secure variants much simpler
// For EKA2, just an E32Main and a MainL()
LOCAL_C void MainL()
/**
 * Secure variant
 * Much simpler, uses the new Rendezvous() call to sync with the client
 */
	{
	// Leave the hooks in for platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
#endif
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestCalInterimApiDCSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestCalInterimApiDCSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
	#ifdef __WINS__ 
		// Start the Alarm Server
		TBuf<MAX_TEXT_MESSAGE> buffer;
		buffer.Append('p');
		buffer.Append('p');
		buffer.Append(EKeyEscape);
		RProcess alarmServer;
		alarmServer.Create(KConsoleAlarmAlertServerImg, buffer, serverUid);	
		TRequestStatus stat;
		alarmServer.Rendezvous(stat);
		if (stat != KRequestPending)
			{
			alarmServer.Kill(0);
			}
			else
			{
			alarmServer.Resume();
			}
		
		User::WaitForRequest(stat);
		TInt result = stat.Int();
		if(result != KErrNone && result != KErrAlreadyExists)
			{
			User::LeaveIfError(result);	
			}
	#endif //WINS
		sched->Start();
		}

	delete server;
	delete sched;
	}

GLDEF_C TInt E32Main()
/**
 * @return - Standard Epoc error code on process exit
 * Secure variant only
 * Process entry point. Called by client using RProcess API
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
    }

CTestStep* CTestCalInterimApiDCSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	if(aStepName == KTestCalInterimApiDCCreateEntries)
		{
		testStep = new CTestCalInterimApiDCCreateEntries();
		}
	else if(aStepName == KTestCalInterimApiDCTest)
		{
		testStep = new CTestCalInterimApiDCTest();
		}
	return testStep;
	}
