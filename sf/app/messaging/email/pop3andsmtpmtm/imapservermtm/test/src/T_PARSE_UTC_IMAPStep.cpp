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
// The sever contains emails with multiple or missing date header fields to ensure that the IAMP
// parser correctly parses the data. The first received header should be used where present, 
// the date header when its not. The current UTC time should be used if neither are present.
// 2. The client connects to the server and request a sync.
// 3. The server reads the protocol information from a file specified in the ini file (mapUTCTest.ini).
// 4. The client disconnects.
// 5. The message store is traversed to see if the correct timestamps have been given to the 
// message entries.
// 
//

/**
 @SYMTestCaseID MSG-IMAP-0243
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234
 @SYMTestCaseDesc Implements an IMAP server to facilitate offline testing of imap parsing. 
 @SYMTestActions  1. The Server is started and listens on port 143.
 @SYMTestExpectedResults The timestamps of the message entries should be the same as in the ini file.
*/

#include <testexecutelog.h>
#include "t_parse_utc_imapstep.h"


_LIT(KUTCTest,"UTC-TIME");
_LIT(KTestMessageDir,"rfc822\\parse\\");
RTest test(_L("UTC Parse Test"));

 
LOCAL_C void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}
		
CTestUTCParse::~CTestUTCParse()
	{
	delete iConsole;
	delete iImapServer;	
	delete iTestUtils;
	delete iImapClient;	
	delete iScheduler;
	iSession->CloseMessageServer();
	delete iSession;
	delete iSessionObserver;
	}

CTestUTCParse::CTestUTCParse()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestUTCParse);
	}
	


void CTestUTCParse::HandleEvent(TInt aEventCode)
	{
	CActiveScheduler::Stop();	
	
	if(aEventCode!=KErrNone && aEventCode!=KErrEof)
		{
		ERR_PRINTF2(_L("Error: %d"),aEventCode);	
		}
	
	}

		
void CTestUTCParse::PrintEntryTime(TTime* aTIme)
	{
	TBuf<45> lDateTimeString;
	FormatDateTime(lDateTimeString,*aTIme); 
	INFO_PRINTF2(_L("%S"),&lDateTimeString);	
	}
		
		
void CTestUTCParse::GetDetailsFromIniFileL()
	{
	
	//get the script file path
	if(!GetStringFromConfig(ConfigSection(),_L("ScriptFile"),iScriptFile))
	
		{
		ERR_PRINTF1(_L("Error reading script file path from ini file"));
		User::Leave(KErrArgument);	
		}

	TPtrC16 expectedTime;
	//get the expected time stamp
	if(!GetStringFromConfig(ConfigSection(),_L("ExpectedTime"),expectedTime))
	
		{
		ERR_PRINTF1(_L("Error reading expected time stamp file path from ini file"));
		User::Leave(KErrArgument);	
		}
	else
		{
		TInt pos=expectedTime.Find(KUTCTest);
		
		if(pos!=KErrNotFound)
			{
			iExpectedTimeStampIsNow=ETrue;
			iExpectedTimeStamp.UniversalTime();
			}
		else
			{
			iExpectedTimeStampIsNow=EFalse;
			iExpectedTimeStamp.Set(expectedTime);			
			}
		
		}
		
	//get the script file path
	if(!GetBoolFromConfig(ConfigSection(),_L("FetchWholeMessage"),iFetchWholeMessage))
	
		{
		ERR_PRINTF1(_L("Error reading script file path from ini file"));
		User::Leave(KErrArgument);	
		}
	//if the test contains an embedded test then get the expected timestamp for the 
	//embedded message
	if(iFetchWholeMessage==1)
		{		
		if(!GetStringFromConfig(ConfigSection(),_L("ExpectedTime2"),expectedTime))
			{
			ERR_PRINTF1(_L("Error reading expected time stamp file path from ini file"));
			User::Leave(KErrArgument);	
			}
		else
			{
			TInt pos=expectedTime.Find(KUTCTest);
		
			if(pos!=KErrNotFound)
				{
				iExpectedTimeStampIsNow2=ETrue;
				iExpectedTimeStamp2.UniversalTime();
				}
			else
				{
				iExpectedTimeStampIsNow2=EFalse;
				iExpectedTimeStamp2.Set(expectedTime);			
				}				
			}
		}
		
	}
	

void CTestUTCParse::WriteStoreL()
	{
	//create the entry_structure file
	TParse parsedFileName;
	TFileName lName(KTestMessageDir);
	iTestUtils->ResolveLogFile(lName, parsedFileName);
	iTestUtils->FindChildrenL(KMsvRootIndexEntryIdValue, parsedFileName.FullName(), ETrue, EFalse);	
	}
 	
void CTestUTCParse::TraverseL(const TMsvId aTMsvId)
	{
	//open the entry, dont sort
	TMsvSelectionOrdering ordering;	
	ordering.SetSorting(EMsvSortByNone);
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, aTMsvId,ordering);
	CleanupStack::PushL(entry);
	
	//retrieve the entries children
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	TInt count = entry->Count();
	
	CMsvEntry* child;
	TMsvEntry childDetails;

	//for each child test if its a message entry
	for(TInt i=0;i<count;i++)
		{
		child=iSession->GetEntryL((*msvEntrySelection)[i]);
		childDetails=child->Entry();
		
		if(childDetails.iType==KUidMsvMessageEntry)
			{
			iMessageEntryCount++;
			//if this is the first timestamp the us iActualTimeStamp, else use iActualTimeStamp2
			if(iActualTimeStamp==0)
				{
				iActualTimeStamp=childDetails.iDate;
				PrintEntryTime(&childDetails.iDate);	
				}
			else
				{
				iActualTimeStamp2=childDetails.iDate;
				PrintEntryTime(&childDetails.iDate);	
				}
			}
		//recursive call
		TraverseL(childDetails.Id());
		
		delete child;
		}
			
	CleanupStack::PopAndDestroy(msvEntrySelection);
	CleanupStack::PopAndDestroy(entry);
		
	}
	

TVerdict CTestUTCParse::doTestStepPreambleL()
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
	GetDetailsFromIniFileL();
	iConsole=Console::NewL(_L("IMAP UTC Parse Test"),TSize(KConsFullScreen,KConsFullScreen));
	iImapServer = CImapSpoofServer::NewL(this,iScriptFile);
	iImapServer->StartL();
		
	//start the imap client
	iImapClient = CActiveImapClient::NewL(this,iFetchWholeMessage);
	iImapClient->StartL();
	
	CActiveScheduler::Start();

	return TestStepResult();
	}

	
TVerdict CTestUTCParse::CheckTimeStamps()
	{
	TVerdict lResult;
	
	if(iExpectedTimeStampIsNow==EFalse)
		{
		lResult = (iActualTimeStamp==iExpectedTimeStamp? EPass:EFail);
		}
	else
		{
		TTimeIntervalMicroSeconds difference;
		difference=iExpectedTimeStamp.MicroSecondsFrom(iActualTimeStamp);
		lResult = (difference<5000000? EPass:EFail);	
		}
		
	// If there is an embedded message, check the times on that as well
	if ((lResult == EPass) && (iFetchWholeMessage))
		{
		if(iExpectedTimeStampIsNow2==EFalse)
			{
			lResult = (iActualTimeStamp2==iExpectedTimeStamp2 ? EPass:EFail);
			}
		else
			{
			TTimeIntervalMicroSeconds difference;
			difference=iExpectedTimeStamp2.MicroSecondsFrom(iActualTimeStamp2);
			lResult = (difference<5000000? EPass:EFail);
			}
		}

	return lResult;	
	}
		
TVerdict CTestUTCParse::doTestStepL()
	{
	TVerdict result;
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	// Client registry. Needed to get the MTM component
	TraverseL(iImapClient->GetAccount().iImapService);
	WriteStoreL();
	
	result=CheckTimeStamps();

	SetTestStepResult(result);
	
	return TestStepResult();
	}
	

TVerdict CTestUTCParse::doTestStepPostambleL()
	{
	return TestStepResult();
	}
