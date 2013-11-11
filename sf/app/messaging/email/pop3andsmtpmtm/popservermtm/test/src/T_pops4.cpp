// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// T_IMCM04.CPP
// 
//

#include "emailtestutils.h"
#include "miutlog.h"
#include "miutmsg.h"
#include <txtrich.h>
#include <logcli.h>
#include <logview.h>
#include <cemailaccounts.h>

// local variables etc //

_LIT(KPopTest, "T_POPS4 - Testing Progress Reporting In POPS");
RTest test(KPopTest);

LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;
LOCAL_D TMsvId newMessageId;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CActiveScheduler* scheduler;

//

LOCAL_C void Init()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);	testUtils = CEmailTestUtils::NewLC(test);
	CleanupStack::PushL(scheduler);

	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->CopyScriptFileL(_L("c:\\mailtest\\scripts\\popprog.scr"), _L("110"));
	testUtils->GoServerSideL();


	log = CImLog::NewL(_L("c:\\logs\\email\\t_pops4.log"), EAppend);
	CleanupStack::PushL(log);
	TBuf8<80> buf;
#if defined(__WINS__)
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_POPS4 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(2);  //testUtils, log
	delete scheduler;
	}

//

LOCAL_C void doMainL()
	{
	Init();
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

//	smtpService = testUtils->CreateSmtpServiceL();
	
	// Create the POP3 service and set the settings for this test.
	pop3Service = testUtils->CreatePopServiceL();

	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
	settings->Reset();
	_LIT(KPopServer, "internet-test.plc.psion.com");
	settings->SetServerAddressL(KPopServer);
	settings->SetLoginNameL(testUtils->MachineName());
	settings->SetPasswordL(testUtils->MachineName());
	settings->SetPort(110);
	
 	// Saving POP settings
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
 	TPopAccount popAccount;
 	accounts->GetPopAccountL(pop3Service, popAccount);						
 	accounts->SavePopSettingsL(popAccount, *settings);
	CleanupStack::PopAndDestroy(2, settings); // accounts/store, settings

	// Set the IAP to Demon (even though we're not going to dial up as such.)
	User::LeaveIfError(testUtils->iServerEntry->SetEntry(pop3Service));
	TMsvEntry entry = testUtils->iServerEntry->Entry();
	entry.SetMtmData1(3);
	User::LeaveIfError(testUtils->iServerEntry->ChangeEntry(entry));

	test.Printf(_L("\nPerforming Tests\n"));

	CTestActive* activeObjectTester = new (ELeave) CTestActive;
	CleanupStack::PushL(activeObjectTester);
	
	// Get server MTM.
	testUtils->InstantiatePopServerMtmL();
	CBaseServerMtm* pop3ServerMtm = testUtils->iPopServerMtm;

	// Get the POP3 server MTM to connect.
	log->AppendComment(_L8("Connect to POP3 server"));
	CMsvEntrySelection* serviceId = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(serviceId);
	serviceId->AppendL(pop3Service);
	TBuf8<1> parameters;
	activeObjectTester->StartL();
	User::LeaveIfError(testUtils->iServerEntry->SetEntry(pop3Service));

	pop3ServerMtm->StartCommandL(*serviceId, KPOP3MTMConnect, parameters, activeObjectTester->iStatus);
	CActiveScheduler::Start();

	// Get the progress
	TPop3Progress prog;	
	TPckgC<TPop3Progress> paramPack(prog);
	paramPack.Set(pop3ServerMtm->Progress());
	TPop3Progress progress=paramPack();

	// Check that the reported progress is EPopConnecting
	test(progress.iPop3Progress == TPop3Progress::EPopConnecting);

	CleanupStack::PopAndDestroy(); // serviceId

	// Cleanup
	CleanupStack::PopAndDestroy(); // activeObjectTester

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_POPS4 Testing Progress Reporting In POPS"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
