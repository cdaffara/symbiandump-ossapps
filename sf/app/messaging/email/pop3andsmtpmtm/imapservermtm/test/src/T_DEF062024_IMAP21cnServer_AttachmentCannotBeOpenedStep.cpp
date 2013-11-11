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
// The sever contains and retrieves an email with an attachment (a v-card), the aim of th test is to check that this
// attachmenet is fetched correctly by comparing its size and centent to the size and content of the source file attached.
// 2. The client connects to the server, requests a sync and a populate
// 3. The server reads the protocol information from a file specified in the ini file (ImapAttachmentTest.ini).
// 4. The client disconnects.
// 5. The attachment of the mail fetched is compared with the corresponding source file. The test passes if its size and content is exactly the same as the size and content of the file originally attached to this email, otherwise itv fails.
// 
//

/**
 @SYMTestCaseID DEF062024   
 @SYMTestType UT					
 @SYMTestPriority High			
 @SYMDEF DEF062024					
 @SYMTestCaseDesc Implements an IMAP server to facilitate offline testing of imap parsing. 
 @SYMTestActions  1. The Server is started and listens on port 143.
 @SYMTestExpectedResults The attachment file should have the same size and content as the source file originally attached.
*/

#include <testexecutelog.h>
#include "t_DEF062024_IMAP21cnServer_AttachmentCannotBeOpenedStep.h"


RTest test(_L("IMAP Get and check attachment Test"));
const TInt KImapPort = 143;
const TInt KChunkReadFile = 512;
const TInt KSizeDateTimeBuffer = 45;

 
LOCAL_C void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}
		
CTestImapAttachmentStep::~CTestImapAttachmentStep()
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

CTestImapAttachmentStep::CTestImapAttachmentStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestImapAttachment);
	}

void CTestImapAttachmentStep::TestComplete(TInt aErrorCode)
	{
	CActiveScheduler::Stop();	
	
	if(aErrorCode!=KErrNone && aErrorCode!=KErrEof)
		{
		ERR_PRINTF2(_L("Error: %d"),aErrorCode);	
		}
	}
 
		
void CTestImapAttachmentStep::PrintEntryTime(TTime* aTIme)
	{
	TBuf<KSizeDateTimeBuffer> dateTimeString;
	FormatDateTime(dateTimeString,*aTIme); 
	_LIT(KDateTimeString, "%S");
	INFO_PRINTF2(KDateTimeString,&dateTimeString);	
	}
		
		
void CTestImapAttachmentStep::GetDetailsFromIniFileL()
	{
	//get the script file path
	_LIT(KScriptFile,"ScriptFile");	
	if(!GetStringFromConfig(ConfigSection(),KScriptFile,iScriptFile))
		{
		_LIT(KErrorScriptFile, "Error reading script file path from ini file");
		ERR_PRINTF1(KErrorScriptFile);
		User::Leave(KErrArgument);	
		}
	// get the source path of the file attached to the mail
	_LIT(KSourceAttach,"AttachmentSourcePath");
	if(!GetStringFromConfig(ConfigSection(),KSourceAttach,iAttachmentSourcePath))
		{
		_LIT(KErrorSourceAttach, "Error reading attachment source file path from ini file");
		ERR_PRINTF1(KErrorSourceAttach);
		User::Leave(KErrArgument);	
		}
	// get the index of the attachment in this mail
	_LIT(KAttachIndex, "AttachmentIndex");
	if(!GetIntFromConfig(ConfigSection(),KAttachIndex,iAttachmentIndex))
		{
		_LIT(KErrorAttachIndex, "Error reading attachment index from ini file");
		ERR_PRINTF1(KErrorAttachIndex);
		User::Leave(KErrArgument);	
		}
	}	

TVerdict CTestImapAttachmentStep::doTestStepPreambleL()
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
	_LIT(KIMAPTest, "IMAP Email Attachment Test");
	iConsole=Console::NewL(KIMAPTest,TSize(KConsFullScreen,KConsFullScreen));
	iSpoofServer = CSpoofServer::NewL(*this,iScriptFile);
	iSpoofServer->StartL(KImapPort);
	//start the imap client
	iImapClient = CActiveImapClient::NewL(*this);
	iImapClient->StartL();
	
	CActiveScheduler::Start();
	return TestStepResult();
	}

TVerdict CTestImapAttachmentStep::CheckAttachmentL() 
	{
	TVerdict result = EPass;
	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	CMsvEntry* entry = iSession->GetEntryL(iImapClient->GetMessageId());
	CleanupStack::PushL(entry);
	CImEmailMessage* emailMsg = CImEmailMessage::NewL(*entry);
	CleanupStack::PushL(emailMsg);
	MMsvAttachmentManager& manager = emailMsg->AttachmentManager();
	TInt attachmentCount = manager.AttachmentCount();
	// Get the file attachment
	RFile fileAttachment = manager.GetAttachmentFileL(iAttachmentIndex);
	CleanupClosePushL(fileAttachment);
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile dataFile;
	// Open the source file attached to the email
	User::LeaveIfError(dataFile.Open(fs, iAttachmentSourcePath, EFileRead|EFileShareReadersOnly));
	CleanupClosePushL(fs);
	TInt fileSize = 0;
	TInt attachmentSize = 0;
	// Get the size of the source file
	User::LeaveIfError(dataFile.Size(fileSize));
	// Get the size of the attachment file
	User::LeaveIfError(fileAttachment.Size(attachmentSize));
	
	// Check the sizes first, the source and attachment files must have the same size
	if(fileSize!=attachmentSize)
		{
		result = EFail;				
		}
	else
		{
		// We compare the contents
		TInt compare;
		TBuf8<KChunkReadFile> dataBuffer;
		TBuf8<KChunkReadFile> attachmentData;
		if(fileSize < KChunkReadFile)
			{			
			dataFile.Read(0,dataBuffer);
			fileAttachment.Read(0,attachmentData);
			
			compare = attachmentData.Compare(dataBuffer);
			if(compare!=0)
				{
				result = EFail;
				}
			else
				{
				result = EPass;
				}				
			}
		else
			{
			// The size of the files is big, we read them by chunks and compare them
			TInt bytesRead = KChunkReadFile;
			TInt ii = 0;
			while (bytesRead < fileSize)	
				{
				// Get the i-th chunk of data of the source file
				dataFile.Read(ii*KChunkReadFile,dataBuffer);
				// Get the i-th chunk of data of the attachment file
				fileAttachment.Read(ii*KChunkReadFile,attachmentData);
				
				// Compare the chunks
				compare = attachmentData.Compare(dataBuffer);
				if(compare != 0)
					{
					result = EFail;
					break;
					}
				else
					result = EPass;
				bytesRead += KChunkReadFile;
				++ii;
				}
			}		
		}
		
	CleanupStack::PopAndDestroy(5, entry);
	
	return result;	
	}
		
TVerdict CTestImapAttachmentStep::doTestStepL()
	{
	TVerdict result;	
	
	result = CheckAttachmentL();
	
	SetTestStepResult(result);
	
	return TestStepResult();
	}
	

TVerdict CTestImapAttachmentStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}
	
	


