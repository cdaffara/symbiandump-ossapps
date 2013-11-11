// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalIndexFileSuiteServer.h"
#include "TestCalIndexFileOpenStep.h"
#include "TestCalIndexFilePerfStep.h"
#include "TestCalIndexFileBandRStep.h"
#include "TestCalIndexFileCreateDbStep.h"
#include "TestCalIndexFileDeleteAllStep.h"
#include "TestCalIndexFileDeleteDbStep.h"
#include "TestCalIndexFileDeleteEntryStep.h"
#include "TestCalIndexFileModifyEntryStep.h"
#include "TestCalIndexFileOpenwithIndexStep.h"
#include "TestCalIndexFileAddEntryStep.h"


/**  Constants used by the ConsoleAlarmAlertServer*/
#ifdef __WINS__ 
_LIT(KConsoleAlarmAlertServerImg,"ConsoleAlarmAlertServer");
const TUid KServerUid2={0x1000008D};
const TUid KServerUid3={0x101F502A};
const TUidType serverUid(KNullUid,KServerUid2,KServerUid3);
#endif

#define	MAX_TEXT_MESSAGE	256

	
CTestCalIndexFileSuite* CTestCalIndexFileSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestCalIndexFileSuite *	server = new (ELeave) CTestCalIndexFileSuite();
	CleanupStack::PushL(server);
	// CServer base class call
	RProcess	handle = RProcess();
	TParsePtrC	serverName(handle.FileName());
	server->ConstructL(serverName.Name());

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
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().DataCaging(RProcess::ESecureApiOn);
	CActiveScheduler*	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestCalIndexFileSuite*	server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestCalIndexFileSuite::NewL());
	if(err == KErrNone)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);

		#ifdef __WINS__

		// Start the Alarm Server
		RProcess alarmServer;
		TInt err = alarmServer.Create(KConsoleAlarmAlertServerImg, KNullDesC, serverUid);
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
		#endif

		sched->Start();

		#ifdef __WINS__
		// If this is not done, the testserver process object is not being destroyed
		// immediately in the emulator, and hence problem arises when we try
		// to do another LOAD_SUITE immediately.
		alarmServer.Terminate(KErrNone);
		alarmServer.Close();
		#endif
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

CTestStep* CTestCalIndexFileSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 
	if(aStepName == KTestCalIndexFileOpenStep)
		{
		testStep = new CTestCalIndexFileOpenStep();
		}
	else if(aStepName == KTestCalIndexFileAddEntryStep)
		{
		testStep = new CTestCalIndexFileAddEntryStep();
		}
	else if(aStepName == KTestCalIndexFileBandRStep)
		{
		testStep = new CTestCalIndexFileBandRStep();
		}
	else if(aStepName == KTestCalIndexFileCreateDbStep)
		{
		testStep = new CTestCalIndexFileCreateDbStep();
		}
	else if(aStepName == KTestCalIndexFileDeleteAllStep)
		{
		testStep = new CTestCalIndexFileDeleteAllStep();
		}
	else if(aStepName == KTestCalIndexFileDeleteDbStep)
		{
		testStep = new CTestCalIndexFileDeleteDbStep();
		}
	else if(aStepName == KTestCalIndexFileDeleteEntryStep)
		{
		testStep = new CTestCalIndexFileDeleteEntryStep();
		}
	else if(aStepName == KTestCalIndexFileModifyEntryStep)
		{
		testStep = new CTestCalIndexFileModifyEntryStep();
		}
	else if(aStepName == KTestCalIndexFileOpenwithIndexStep)
		{
		testStep = new CTestCalIndexFileOpenwithIndexStep();
		}
	else if(aStepName == KTestCalIndexFilePerfStep)	
		{
		testStep = new CTestCalIndexFilePerfStep();
		}

	return testStep;
	}
	
	
