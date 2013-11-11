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
#include "t_imapmailcancelstoreattachments.h"

const TInt KEmptyAttachmentSize=0;
_LIT8(KTestText8String1,"testchunk1");
_LIT8(KTestText8String2,"testchunk2");
_LIT8(KTestText8String3,"testchunk3");
_LIT(KCheckingAttachmentSize,"Checking the attachment size...");
_LIT(RestoringAttachment,"Restoring the attachment...");
_LIT(KTestingStorageAttachment,"Testing Storage of attachment data...");
_LIT(KTestingCancelledAttachment,"Testing the info of the attachment that was cancelled during download...");

/**
@SYMTestCaseID MSG-IMAP-MAILSTORE-0004
@SYMTestType UT
@SYMTestPriority High
@SYMPREQ 1160
@SYMTestCaseDesc Tests storage of attachments using CImapMailStore..
@SYMTestActions  1. Creates a list of index entries into which the body and its CImMimeHeader header will be stored.
				 2. Creates CImMimeHeader populated with test values and reads test attachment data from a file.
				 3. Copies the data into 3 chunks, appending an identifying string to each chunk. 
				 4. Uses CImapMailStore to store the chunks out of order.
				 5. Uses CMsvAttachment to read back the attachment.
				 6. Tests the body was stored correctly.

@SYMTestExpectedResults The chunks should have all been stored and the order should have been corrected.
*/

CTestImapMailCancelStoreAttachments::CTestImapMailCancelStoreAttachments()
	{	
	}
	
/**
Calls the base classes SetupL() to clean the message store and initialise test.
Creates CImHeader and CImMimeHeader objects.
*/
void CTestImapMailCancelStoreAttachments::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();	
	
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	
	CleanupStack::Pop(mimeHeader); 
	iImapMailStore->StorePartL(mimeHeader,iAttMessageEntry,*this);
	++iRequestCount;
	CActiveScheduler::Start();
	
 	//read attchment text form a file
	TBuf<KMaxFilePathLength>emailbodyfile(KEmail40kBodyFile);
	RFs fsSession;
	RFile infile; 
		
	User::LeaveIfError(fsSession.Connect()); 
 	User::LeaveIfError(infile.Open(fsSession, emailbodyfile, EFileRead));
	TInt size;
	infile.Size(size);
 
 	//create 1st chunk
	HBufC8* filebuf1 = HBufC8::NewLC(size+KTestStringLength);  
	TPtr8 ptr1=filebuf1->Des();	  
	infile.Read(ptr1);
	CleanupStack::Pop(filebuf1);

	//we dont need to read from the file anymore
	infile.Close();
	fsSession.Close();

	//create a 2nd chunk
	HBufC8* filebuf2 = HBufC8::NewLC(size+KTestStringLength);
	CleanupStack::Pop(filebuf2);
	TPtr8 ptr2=filebuf2->Des();	
	ptr2.Copy(ptr1);	
	
	//create a 3rd chunk
	HBufC8* filebuf3 = HBufC8::NewLC(size+KTestStringLength);
	CleanupStack::Pop(filebuf3);
	TPtr8 ptr3=filebuf3->Des();	
	ptr3.Copy(ptr1);	
	
	//append test strings to identify each chunk
	ptr1.Append(KTestText8String1);
	ptr2.Append(KTestText8String2);	
	ptr3.Append(KTestText8String3);	
	
	INFO_PRINTF1(KTestingStorageAttachment);
	//store the body data in the wrong order
	TImEncodingType encoding = EEncodingTypeNone;
	
	iInfo=CFetchBodyInfo::NewL(iAttMessageEntry);
	iInfo->SetContentTransferEncoding(encoding);
	iInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iSettings = CImapSettings::NewL(*iServerEntry);
	
	iImapMailStore->InitialiseStoreAttachmentL(3,*iSettings,*iInfo,KLogId,*this);
	
	INFO_PRINTF1(KStoringChunk0);
	iImapMailStore->StoreAttachmentChunkL(filebuf1,iAttMessageEntry,0);	
	
	//this is to allow the store to store chunk 0 before we cancel the request. 
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(K2Seconds);
	CActiveScheduler::Start();
	delete timer;
	
	iImapMailStore->CancelRequest(*this);
		
	}
	
/**
Reads back the downloaded attachment stored in using CMsvAttachment and test chunks where stored correctly.
*/
void CTestImapMailCancelStoreAttachments::TestAttachment1L()
	{
	
	//open the file
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	
	//now check the info of the attachment that was not downloaded
	INFO_PRINTF1(KTestingCancelledAttachment);
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
	
CTestSuite* CTestImapMailCancelStoreAttachments::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestAttachment1L);
	END_SUITE;
	}
