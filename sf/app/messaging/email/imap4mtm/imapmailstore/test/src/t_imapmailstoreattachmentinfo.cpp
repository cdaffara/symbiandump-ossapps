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

#include <mmsvstoremanager.h>
#include "t_imapmailstoreattachmentinfo.h"

const TInt KEmptyAttachmentSize=0;
_LIT(KCheckingAttachmentSize,"Checking the attachment size...");
_LIT(RestoringAttachment,"Restoring the attachment...");
_LIT(KTestingAttachmentNotDownloaded,"Testing the info of the attachment that was not downloaded...");

/**
@SYMTestCaseID MSG-IMAP-MAILSTORE-0006
@SYMTestType UT
@SYMTestPriority High
@SYMPREQ 1160
@SYMTestCaseDesc Tests storage of attachments using CImapMailStore..
@SYMTestActions  1. Creates a list of index entries into which the body and its CImMimeHeader header will be stored.
				 2. Creates CImMimeHeader populated with test values and reads test attachment data from a file.
				 3. Creates attacment info for an attacment being synced but not downloaded. 
				 4. Tests that an empty file was ceated for the attachment.

@SYMTestExpectedResults The chunks should have all been stored and the order should have been corrected.
*/

CTestImapMailStoreAttachmentInfo::CTestImapMailStoreAttachmentInfo()
	{	
	}
	
/**
Calls the base classes SetupL() to clean the message store and initialise test.
Creates CImHeader and CImMimeHeader objects.
*/
void CTestImapMailStoreAttachmentInfo::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();	
	
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	
	CleanupStack::Pop(mimeHeader); 
	iImapMailStore->StorePartL(mimeHeader,iAttMessageEntry,*this);
	++iRequestCount;
	CActiveScheduler::Start();
	
	//just create attachment info synchronously
	iImapMailStore->CreateAttachmentInfoL(iAttMessageEntry);		
	}


void CTestImapMailStoreAttachmentInfo::TestAttachmentInfoL()
	{
	//open the file
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	
	//now check the info of the attachment that was not downloaded
	INFO_PRINTF1(KTestingAttachmentNotDownloaded);
	iServerEntry->SetEntry(iAttMessageEntry);
	CMsvStore* store=iServerEntry->ReadStoreL();
	
	CleanupStack::PushL(store);
	
	MMsvAttachmentManager& attachmentMgr2 = store->AttachmentManagerL();
	ASSERT_EQUALS(attachmentMgr2.AttachmentCount(), 1);
	
	//get the file path
	CMsvAttachment* attachment = attachmentMgr2.GetAttachmentInfoL(0);
	CleanupStack::PushL(attachment);
	TFileName filePath = attachment->FilePath();
	
	//open the file
	INFO_PRINTF1(RestoringAttachment);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile attachmentfile;		
	ASSERT_EQUALS(attachmentfile.Open(fsSession,filePath,EFileRead),KErrNone);
	
	//test the size of the attachment
	INFO_PRINTF1(KCheckingAttachmentSize);
	TInt attachmentSize;
	attachmentfile.Size(attachmentSize);
	ASSERT_EQUALS(attachmentSize,KEmptyAttachmentSize);
	
	CleanupStack::PopAndDestroy(2,store);
	}

CTestSuite* CTestImapMailStoreAttachmentInfo::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestAttachmentInfoL);
	END_SUITE;
	}
