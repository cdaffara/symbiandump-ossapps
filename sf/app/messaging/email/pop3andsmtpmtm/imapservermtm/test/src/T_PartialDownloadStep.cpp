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

#include <testexecutelog.h>
#include "t_partialdownloadstep.h"

RTest test(_L("Partial Download Test"));
const TInt KImapPort = 143;
const TInt KEntryCount=2; 

CPartialDownloadStep::~CPartialDownloadStep()
	{
	delete iConsole;
	delete iSpoofServer;	
	delete iTestUtils;
	delete iImapClient;	
	delete iScheduler;
	iSession->CloseMessageServer();
	delete iSession;
	delete iSessionObserver;
	}

CPartialDownloadStep::CPartialDownloadStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestPartialDownLoad);
	}

void CPartialDownloadStep::TestComplete(TInt aErrorCode)
	{
	CActiveScheduler::Stop();	
	
	if(aErrorCode!=KErrNone && aErrorCode!=KErrEof)
		{
		ERR_PRINTF2(_L("Error: %d"),aErrorCode);	
		}
	}

	
TVerdict CPartialDownloadStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils->CleanMessageFolderL();
	iTestUtils->ClearEmailAccountsL();
	
	iTestUtils->GoClientSideL();	
	//start the imap server
	iConsole=Console::NewL(_L("IMAP UTC Parse Test"),TSize(KConsFullScreen,KConsFullScreen));
	iSpoofServer = CSpoofServer::NewL(*this,_L("c:\\msgtest\\imap\\partialDownload.txt"));
	iSpoofServer->StartL(KImapPort);
	//start the imap client
	iImapClient = CPartialDownLoadImapClient::NewL(*this,ETrue);
	iImapClient->StartL();
	
	CActiveScheduler::Start();
	return TestStepResult();
	}

TInt CPartialDownloadStep::GetEntryCountL()
	{
	
	TImapAccount imapAccount=iImapClient->GetImapAccount();
	
	TMsvSelectionOrdering ordering;	

	//open the imap service entry
	CMsvEntry* imapService = CMsvEntry::NewL(*iSession,imapAccount.iImapService,ordering);
	CleanupStack::PushL(imapService);
	//get its children
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=imapService->ChildrenL();
	//open its child inbox entry
	CMsvEntry* inboxEntry = CMsvEntry::NewL(*iSession, (*msvEntrySelection)[0],ordering);
    CleanupStack::PushL(inboxEntry);
    
    //get the childeren of the inbox
    delete msvEntrySelection;
    msvEntrySelection=NULL;
    msvEntrySelection=inboxEntry->ChildrenL();
    //the count should be 2
    TInt count=msvEntrySelection->Count();
    
    delete msvEntrySelection;
    msvEntrySelection=NULL;	
	CleanupStack::PopAndDestroy(2,imapService);
	
	return count;
	}

TVerdict CPartialDownloadStep::doTestStepL()
	{
	TVerdict result;
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	//get entry count
	TInt entryCount=GetEntryCountL();
	result = (entryCount==KEntryCount? EPass:EFail);
	SetTestStepResult(result);
	return TestStepResult();
	}
	

TVerdict CPartialDownloadStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
	
	


