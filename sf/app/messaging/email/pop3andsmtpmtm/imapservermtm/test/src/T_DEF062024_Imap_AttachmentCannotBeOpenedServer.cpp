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

#include "T_DEF062024_Imap_AttachmentCannotBeOpenedServer.h"
#include "t_DEF062024_IMAP21cnServer_AttachmentCannotBeOpenedStep.h"

_LIT(KServerName,"T_DEF062024_Imap_AttachmentCannotBeOpened");

CImapAttachmentServer* CImapAttachmentServer::NewL()
	{
	CImapAttachmentServer * server = new (ELeave) CImapAttachmentServer();
	CleanupStack::PushL(server);
	server-> ConstructL(KServerName);
	CleanupStack::Pop(server);
	return server;
	}

CImapAttachmentServer::CImapAttachmentServer()
	{}

CImapAttachmentServer::~CImapAttachmentServer()
	{}


LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CImapAttachmentServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CImapAttachmentServer::NewL());
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


RFs& CImapAttachmentServer::Fs()
	{
	return iFs;
	};	

CTestStep* CImapAttachmentServer::CreateTestStep(const TDesC& aStepName)
	{
		
	CTestStep* testStep = NULL;
	
	if(aStepName == KTestImapAttachment)
		testStep = new CTestImapAttachmentStep();


	return testStep;
	}
