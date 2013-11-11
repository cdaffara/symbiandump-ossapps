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
// t_parse_def059571_missingoffsetstep.cpp
// 
//

#include <testexecutelog.h>
#include "t_def059571_imap_missingoffsetstep.h"

RTest test(_L("IMAP Missing Offset and HTML Test"));

const TInt KSizeDateTimeBuffer = 45;
const TInt KImapPort = 143;
 
LOCAL_C void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}
		
CTestImapMissingOffsetStep::~CTestImapMissingOffsetStep()
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

CTestImapMissingOffsetStep::CTestImapMissingOffsetStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImapMissingOffset);
	}

void CTestImapMissingOffsetStep::TestComplete(TInt aEventCode)
	{
	CActiveScheduler::Stop();	
	
	if(aEventCode!=KErrNone && aEventCode!=KErrEof)
		{
		ERR_PRINTF2(_L("Error: %d"),aEventCode);	
		}
	}

void CTestImapMissingOffsetStep::PrintEntryTime(TTime* aTIme)
	{
	TBuf<KSizeDateTimeBuffer> dateTimeString;
	FormatDateTime(dateTimeString,*aTIme); 
	INFO_PRINTF2(_L("%S"),&dateTimeString);	
	}
		
void CTestImapMissingOffsetStep::GetDetailsFromIniFileL()
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

TVerdict CTestImapMissingOffsetStep::doTestStepPreambleL()
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
	iConsole=Console::NewL(_L("IMAP Missing Offset Test"),TSize(KConsFullScreen,KConsFullScreen));
	iSpoofServer = CSpoofServer::NewL(*this,iScriptFile);
	iSpoofServer->StartL(KImapPort);
		
	// start the imap client
	iImapClient = CActiveImapClient::NewL(this);
	iImapClient->StartL();
	
	CActiveScheduler::Start();
	return TestStepResult();
	}

TInt CTestImapMissingOffsetStep::GetEntryCountL()
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
    // the count should be 1
    TInt count=msvEntrySelection->Count();
    
    delete msvEntrySelection;
    msvEntrySelection=NULL;	
	CleanupStack::PopAndDestroy(2,imapService);
	
	return count;
	}


TVerdict CTestImapMissingOffsetStep::doTestStepL()
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

TVerdict CTestImapMissingOffsetStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}


