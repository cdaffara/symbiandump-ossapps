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

/**
 @file
 @internalTechnology 
*/

#include "TestCalInterimApiInstanceSuite.h"
#include "TestCalInterimApiInstance.h"
#include "TestCalInterimApiInstanceData.h"

_LIT(KServerName, "TestCalInterimApiInstanceSuite");

/**
Static factory constructor. Creates and returns instance of the test server
@return A pointer to the newly created CTestCalInterimApiInstanceSuiteServer object
*/
CTestCalInterimApiInstanceSuiteServer* CTestCalInterimApiInstanceSuiteServer::NewL()
	{
	CTestCalInterimApiInstanceSuiteServer* server = new(ELeave) CTestCalInterimApiInstanceSuiteServer();
	CleanupStack::PushL(server);

	// CServer base class call
	// Name the server using the system-wide unique string
	// Clients use this to create server sessions.
	server->StartL(KServerName);
	
	CleanupStack::Pop(server);
	return server;
	}

LOCAL_C void MainL()
	{
	// For platform security
#if (defined __DATA_CAGING__)
	RProcess().DataCaging(RProcess::EDataCagingOn);	
	RProcess().SecureApi(RProcess::ESecureApiOn);	
#endif
	CActiveScheduler* sched = NULL;
	sched = new(ELeave) CActiveScheduler; 
	CActiveScheduler::Install(sched);
	CTestCalInterimApiInstanceSuiteServer* server = NULL;
	
	// Create the test-server
	TRAPD(err, server = CTestCalInterimApiInstanceSuiteServer::NewL());
	
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		
#ifdef __WINS__
		// To start the alarm server	
		server->StartAlarmAlertServer();
#endif
		
		sched->Start();
		}
	delete server;
	delete sched;	
	}

/**
Exe entry point code
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup *cleanup = CTrapCleanup::New();
	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAP_IGNORE(MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
Base class pure virtual
@return - Instance of the test step
*/
CTestStep* CTestCalInterimApiInstanceSuiteServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep *testStep = NULL;
	
	if (aStepName == KTestCalInterimApiInstance)
		{
		testStep = new (ELeave) CTestCalInterimApiInstance;
		}
	
	return testStep;
	}

#ifdef __WINS__
void CTestCalInterimApiInstanceSuiteServer::StartAlarmAlertServer()
	{

	// Start the Alarm Server
	TBuf<256> buffer;
	buffer.Append('p');
	buffer.Append('p');
	buffer.Append(EKeyEscape);
	RProcess alarmServer;
	// Create the Connection to the alarm alert server
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
	}
#endif

