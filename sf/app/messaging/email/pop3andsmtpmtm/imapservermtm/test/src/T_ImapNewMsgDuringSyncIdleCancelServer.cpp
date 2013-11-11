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
// The server waits for the idle read to be cancelled by the sync command, and then sends a '* 2 EXISTS' command to indicate that
// a new message has arrived.
// 2. The client connects to the server, requests a sync and a populate.
// 3. Client / server interact to complete the sync and populate. One message is fetched.
// 4. The client requests a full sync (causing the idle read cancel).
// 5. The server indicates a new message has arrived
// 6. Client / server interact to complete the full sync. The new message is fetched
// 7. The client requests a full sync (causing the idle read cancel).
// 8. The server indicates a new message has arrived
// 9. Client / server interact to complete the full sync. The new message is fetched
// 10. The client disconnects.
// 
//

/**
 @SYMTestCaseID DEF067159
 @SYMTestType UT
 @SYMTestPriority High
 @SYMDEF DEF067159
 @SYMTestCaseDesc IMAP spoof server test that checks that a new message indication is correctly handled if idle read is being cancelled during a sync
 @SYMTestActions 1. The Server is started and listens on port 143.
 @SYMTestExpectedResults Client should have fetched three messages
*/

#include "T_ImapNewMsgDuringSyncIdleCancelServer.h"
#include "T_ImapNewMsgDuringSyncIdleCancelStep.h"

_LIT(KServerName,"T_ImapNewMsgDuringSyncIdleCancel");

CImapNewMsgDuringSyncIdleCancelServer* CImapNewMsgDuringSyncIdleCancelServer::NewL()
	{
	CImapNewMsgDuringSyncIdleCancelServer * server = new (ELeave) CImapNewMsgDuringSyncIdleCancelServer();
	CleanupStack::PushL(server);
	server->ConstructL(KServerName);

	CleanupStack::Pop(server);
	return server;
	}

CImapNewMsgDuringSyncIdleCancelServer::CImapNewMsgDuringSyncIdleCancelServer()
	{}

CImapNewMsgDuringSyncIdleCancelServer::~CImapNewMsgDuringSyncIdleCancelServer()
	{}

LOCAL_C void MainL()
	{
	CActiveScheduler* sched=NULL;
	sched=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);
	CImapNewMsgDuringSyncIdleCancelServer* server = NULL;
	// Create the CTestServer derived server
	TRAPD(err,server = CImapNewMsgDuringSyncIdleCancelServer::NewL());
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

RFs& CImapNewMsgDuringSyncIdleCancelServer::Fs()
	{
	return iFs;
	};	

CTestStep* CImapNewMsgDuringSyncIdleCancelServer::CreateTestStep(const TDesC& aStepName)
	{
	CTestStep* testStep = NULL;
	
	if(aStepName == KTestImapNewMsgDuringSyncIdleCancel)
		testStep = new CImapNewMsgDuringSyncIdleCancelStep();

	return testStep;
	}
