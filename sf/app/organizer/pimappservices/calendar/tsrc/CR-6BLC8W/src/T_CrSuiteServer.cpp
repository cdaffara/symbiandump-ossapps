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
 
#include "T_CRSuiteServer.h"

#include "T_CrEntry.h"
#include "T_CrExportImport.h"
#include "T_CrAlarm.h"


_LIT(KServerName, "testcalinterimapiit");
	
CTestCalInterimApiSuite* CTestCalInterimApiSuite::NewL()
/**
 * @return - Instance of the test server
 * Same code for Secure and non-secure variants
 * Called inside the MainL() function to create and start the
 * CTestServer derived server.
 */
	{
	CTestCalInterimApiSuite * server = new (ELeave) CTestCalInterimApiSuite();
	CleanupStack::PushL(server);

	// CServer base class call which can be either StartL or ConstructL,
	// the later will permit Server Logging.
	server->StartL(KServerName);
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
	CActiveScheduler* sched = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CTestCalInterimApiSuite* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CTestCalInterimApiSuite::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);

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

CTestStep* CTestCalInterimApiSuite::CreateTestStep(const TDesC& aStepName)
/**
 * @return - A CTestStep derived instance
 * Secure and non-secure variants
 * Implementation of CTestServer pure virtual
 */
	{
	CTestStep* testStep = NULL;
	// They are created "just in time" when the worker thread is created
	// More test steps can be added below 

	if (aStepName == KTestCalInterimApiStoreFetch)
		{
		testStep = new CTestCalInterimApiStoreFetch();
		}
	else if (aStepName == KTestCalInterimApiStoreFetchRpt)
		{
		testStep = new CTestCalInterimApiStoreFetchRpt();
		}
	else if (aStepName == KTestCalInterimApiExportImport)
		{
		testStep = new CTestCalInterimApiExportImport();
		}
	else if (aStepName == KTestCalInterimApiImportMixed)
		{
		testStep = new CTestCalInterimApiImportMixed();
		}
	else if (aStepName == KTestCalInterimApiAlarmBasic)
		{
		testStep = new CTestCalInterimApiAlarmBasic();
		}
	else if (aStepName == KTestCalInterimApiAlarmMany)
		{
		testStep = new CTestCalInterimApiAlarmMany();
		}
	else if (aStepName == KTestCalInterimApiExportImportRpt)
		{
		testStep = new CTestCalInterimApiExportImportRpt();
		}
	else if (aStepName == KTestCalInterimApiExportImportExc)
		{
		testStep = new CTestCalInterimApiExportImportExc();
		}
	else if (aStepName == KTestCalInterimApiExportImportSpor)
		{
		testStep = new CTestCalInterimApiExportImportSpor();
		}
	else if (aStepName == KTestCalInterimApiExportRecur)
		{
		testStep = new CTestCalInterimApiExportRecur();
		}
	else if (aStepName == KTestCalInterimApiExportImportNegOffset)
		{
		testStep = new CTestCalInterimApiExportImportNegOffset();
		}	

	return testStep;
	}
