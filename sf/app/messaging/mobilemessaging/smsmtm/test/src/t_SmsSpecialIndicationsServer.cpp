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

#include "t_smsspecialindicationsserver.h"
#include "t_smsspecialindicationsstep.h"
 
_LIT(KServerName,"t_SmsSpecialIndicationsServer");

CSmsSpecialIndicationsServer* CSmsSpecialIndicationsServer::NewL()
	{
	CSmsSpecialIndicationsServer * server = new (ELeave) CSmsSpecialIndicationsServer();
	CleanupStack::PushL(server);
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}
	
LOCAL_C void MainL()
	{
	RProcess().DataCaging(RProcess::EDataCagingOn);
	RProcess().SecureApi(RProcess::ESecureApiOn);
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(sched);
	CSmsSpecialIndicationsServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CSmsSpecialIndicationsServer::NewL());
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
	delete server;
	server = NULL;
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
    
RFs& CSmsSpecialIndicationsServer::Fs()
	{
	return iFs;
	};	

CTestStep* CSmsSpecialIndicationsServer::CreateTestStep(const TDesC& aStepName)
	{
		
	CSmsSpecialIndicationsStep* testStep = NULL; 	
	
	if(aStepName == KSmsSpecialIndicationsStep1)
	{
		testStep = new CSmsSpecialIndicationsStep();	
		testStep->InitialiseSpecialIndicationsStepL();		
	}
	
	return (CTestStep*)testStep;
	}
