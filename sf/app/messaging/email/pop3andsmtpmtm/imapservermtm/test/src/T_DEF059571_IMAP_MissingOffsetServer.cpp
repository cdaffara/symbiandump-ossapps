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

#include "T_DEF059571_IMAP_MissingOffsetServer.h"
#include "T_DEF059571_IMAP_MissingOffsetStep.h"

_LIT(KServerName,"T_DEF059571_IMAP_MissingOffset");

CImapMissingOffsetServer* CImapMissingOffsetServer::NewL()
	{
	CImapMissingOffsetServer * server = new (ELeave) CImapMissingOffsetServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}

CImapMissingOffsetServer::CImapMissingOffsetServer()
	{}

CImapMissingOffsetServer::~CImapMissingOffsetServer()
	{}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CImapMissingOffsetServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CImapMissingOffsetServer::NewL());
	CleanupStack::PushL(server);
	if(!err)
		{
		// Sync with the client and enter the active scheduler
		RProcess::Rendezvous(KErrNone);
		sched->Start();
		}
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

RFs& CImapMissingOffsetServer::Fs()
	{
	return iFs;
	};	

CTestStep* CImapMissingOffsetServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTestImapMissingOffset)
		testStep = new CTestImapMissingOffsetStep();

	return testStep;
	}
