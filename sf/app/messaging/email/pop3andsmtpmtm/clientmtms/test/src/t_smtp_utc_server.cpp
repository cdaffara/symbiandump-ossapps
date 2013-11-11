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

#include "t_smtp_utc_server.h"
#include "t_smtp_utc_step.h"
 
_LIT(KServerName,"t_smtp_utc_server");

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
	CSchSendUTCServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSchSendUTCServer::NewL());
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
	
	if(aStepName == KSmtpUtcEntryStep)
		testStep = new CSmtpUtcEntryStep();	
	
	return testStep;
	}
