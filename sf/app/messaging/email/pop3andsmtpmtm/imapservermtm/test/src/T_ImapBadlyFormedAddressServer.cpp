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

#include "T_ImapBadlyFormedAddressServer.h"
#include "T_ImapBadlyFormedAddressStep.h"

_LIT(KServerName,"T_ImapBadlyFormedAddress");

CImapBadlyFormedAddressServer* CImapBadlyFormedAddressServer::NewL()
	{
	CImapBadlyFormedAddressServer * server = new (ELeave) CImapBadlyFormedAddressServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}

CImapBadlyFormedAddressServer::CImapBadlyFormedAddressServer()
	{}

CImapBadlyFormedAddressServer::~CImapBadlyFormedAddressServer()
	{}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CImapBadlyFormedAddressServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CImapBadlyFormedAddressServer::NewL());
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

RFs& CImapBadlyFormedAddressServer::Fs()
	{
	return iFs;
	};	

CTestStep* CImapBadlyFormedAddressServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTestImapBadlyFormedAddress)
		testStep = new CImapBadlyFormedAddressStep();

	return testStep;
	}
