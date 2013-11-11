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

#include "T_ImapNewMsgDuringSyncIdleCancelStep.h"
#include "T_ImapNewMsgDuringSyncIdleCancelClient.h"

#include "emailtestutils.h"

#include <e32test.h>

RTest test(_L("IMAP receive new message while synch is cancelling idle"));

const TInt KSizeDateTimeBuffer = 45;
const TInt KImapPort = 143;
 
LOCAL_C void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}

CImapNewMsgDuringSyncIdleCancelStep::CImapNewMsgDuringSyncIdleCancelStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImapNewMsgDuringSyncIdleCancel);
	}

CImapNewMsgDuringSyncIdleCancelStep::~CImapNewMsgDuringSyncIdleCancelStep()
	{
	delete iConsole;
	delete iSpoofServer;	
	delete iTestUtils;
	delete iImapClient;	
	delete iScheduler;
	if(iSession!=NULL)
		iSession->CloseMessageServer();
	delete iSession;
	delete iSessionObserver;
	}

void CImapNewMsgDuringSyncIdleCancelStep::TestComplete(TInt aEventCode)
	{
	CActiveScheduler::Stop();

	if(aEventCode!=KErrNone && aEventCode!=KErrEof)
		{
		ERR_PRINTF2(_L("Error: %d"),aEventCode);	
		}
	}

void CImapNewMsgDuringSyncIdleCancelStep::PrintEntryTime(TTime* aTIme)
	{
	TBuf<KSizeDateTimeBuffer> dateTimeString;
	FormatDateTime(dateTimeString,*aTIme); 
	INFO_PRINTF2(_L("%S"),&dateTimeString);	
	}
		
void CImapNewMsgDuringSyncIdleCancelStep::GetDetailsFromIniFileL()
	{
	// get the script file path
	_LIT(KScriptFile,"ScriptFile");	
	if(!GetStringFromConfig(ConfigSection(),KScriptFile,iScriptFile))
		{
		_LIT(KErrorScriptFile, "Error reading script file path from ini file");
		ERR_PRINTF1(KErrorScriptFile);
		User::Leave(KErrArgument);
		}

	// get the expected number of mails
	_LIT(KExpectedCountString, "ExpectedEntryCount");
	if(!GetIntFromConfig(ConfigSection(),KExpectedCountString,iExpectedEntryCount))
		{
		_LIT(KErrorExpectedCount, "Error reading expected entry count from ini file");
		ERR_PRINTF1(KErrorExpectedCount);
		User::Leave(KErrArgument);	
		}
	}

TVerdict CImapNewMsgDuringSyncIdleCancelStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils->CleanMessageFolderL();
	iTestUtils->ClearEmailAccountsL();
	
	iTestUtils->GoClientSideL();
	
	// start the imap server
	GetDetailsFromIniFileL();
	iConsole=Console::NewL(_L("IMAP Receive New Msg During Sync Idle Cancel"),TSize(KConsFullScreen,KConsFullScreen));
	iSpoofServer = CSpoofServer::NewL(*this,iScriptFile);
	iSpoofServer->StartL(KImapPort);
		
	// start the imap client
	iImapClient = CImapNewMsgDuringSyncIdleCancelClient::NewL(this);
	iImapClient->StartL();
	
	CActiveScheduler::Start();
	return TestStepResult();
	}

TInt CImapNewMsgDuringSyncIdleCancelStep::GetEntryCountL()
	{
	TImapAccount imapAccount=iImapClient->GetImapAccount();
	
	TMsvSelectionOrdering ordering;	

	// open the imap service entry
	CMsvEntry* imapService = CMsvEntry::NewL(*iSession,imapAccount.iImapService,ordering);
	CleanupStack::PushL(imapService);
	// get its children
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=imapService->ChildrenL();
	// open its child inbox entry
	CMsvEntry* inboxEntry = CMsvEntry::NewL(*iSession, (*msvEntrySelection)[0],ordering);
	CleanupStack::PushL(inboxEntry);
    
   // get the childeren of the inbox
   delete msvEntrySelection;
   msvEntrySelection=NULL;
   msvEntrySelection=inboxEntry->ChildrenL();
   TInt count=msvEntrySelection->Count();

	delete msvEntrySelection;
	msvEntrySelection=NULL;	
	CleanupStack::PopAndDestroy(2,imapService);

	return count;
	}


TVerdict CImapNewMsgDuringSyncIdleCancelStep::doTestStepL()
	{
	TVerdict result;
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	// get entry count
	TInt entryCount=GetEntryCountL();
	result = (entryCount==iExpectedEntryCount? EPass:EFail);
	SetTestStepResult(result);
	return TestStepResult();
	}

TVerdict CImapNewMsgDuringSyncIdleCancelStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
