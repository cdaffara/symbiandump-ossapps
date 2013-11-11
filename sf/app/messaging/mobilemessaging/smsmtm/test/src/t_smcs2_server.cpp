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

#include "t_smcs2_server.h"
#include "t_testsmsentry_step.h"
#include "t_testSmsSettings_Step.h"
#include "t_testsmsnumber_step.h"
#include "t_testSmsEmailFields_Step.h"
#include "t_testsmsHeader_step.h"
#include "t_testSmsUtilities_Step.h"
#include "t_testSmsEmsExtensions_step.h"
 
_LIT(KServerName,"t_smcs2_server");

CSchSendUTCServer* CSchSendUTCServer::NewL()
	{
	CSchSendUTCServer * server = new (ELeave) CSchSendUTCServer();
	CleanupStack::PushL(server);
	server-> ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}
	
LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CleanupStack::PushL(sched);
	CSchSendUTCServer* server = server = CSchSendUTCServer::NewL();
	CleanupStack::PushL(server);
	
	RProcess::Rendezvous(KErrNone);
	sched->Start();

	CleanupStack::PopAndDestroy(2,sched);
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(cleanup == NULL)
		{
		return KErrNoMemory;
		}
	TRAPD(err,MainL());
	delete cleanup;
	__UHEAP_MARKEND;
	return err;
    }
    
RFs& CSchSendUTCServer::Fs()
	{
	return iFs;
	};	
	
CTestStep* CSchSendUTCServer::CreateTestStep(const TDesC& aStepName)
	{
	
	CTestStep* testStep = NULL; 	
	
	if(aStepName == KTestSmsEntry)
		testStep = new CTestSmsEntryStep();	
	
	if(aStepName == KTestSmsSettings)
		testStep = new CTestSmsSettingsStep();	
	
	if(aStepName == KTestSmsNumber)
		testStep = new CTestSmsNumberStep();
	
	if(aStepName == KTestSmsEmailFields)
		testStep = new CTestSmsEmailFieldsStep();
	
	if(aStepName == KTestSmsHeader)
		testStep = new CTestSmsHeaderStep();
	
	if(aStepName == KTestSmsUtilities)
		testStep = new CTestSmsUtilitiesStep();
	
	if(aStepName == KTestSmsEmsExtensions)
		testStep = new CTestSmsEmsExtensionsStep();
		
	return testStep;
	}
