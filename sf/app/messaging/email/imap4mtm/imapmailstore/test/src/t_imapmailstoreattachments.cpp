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
#include "t_imapmailstoreattachments.h"

const TInt KAttachmentSize=0x0001DF16;
_LIT8(KTestText8String1,"testchunk1");
_LIT8(KTestText8String2,"testchunk2");
_LIT8(KTestText8String3,"testchunk3");
_LIT(KCheckingAttachmentSize,"Checking the attachment size...");
_LIT(RestoringAttachment,"Restoring the attachment...");
_LIT(KTestingStorageAttachment,"Testing Storage of attachment data...");

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

CTestImapMailStoreAttachments::CTestImapMailStoreAttachments()
	{	
	}
	
/**
Calls the base classes SetupL() to clean the message store and initialise test.
Creates CImHeader and CImMimeHeader objects.
*/
void CTestImapMailStoreAttachments::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();	
	
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	
	CleanupStack::Pop(mimeHeader); 
	iImapMailStore->StorePartL(mimeHeader,iAttMessageEntry,*this);
	++iRequestCount;
	CActiveScheduler::Start();
	
	CImMimeHeader* mimeHeader2=CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader2);
	CleanupStack::Pop(mimeHeader2); 
	
	iImapMailStore->StorePartL(mimeHeader2,iBodyMessageEntry,*this);
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
	iSettings = CImapSettings::NewL(*iServerEntry);
	
	iImapMailStore->InitialiseStoreAttachmentL(3,*iSettings,*iInfo,KLogId,*this);
	INFO_PRINTF1(KStoringChunk1);
	iImapMailStore->StoreAttachmentChunkL(filebuf2,iAttMessageEntry,1);
	
	//this is to exercise CAttachmentQueueEntry where not all data is available when first activated. 
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(K2Seconds);
	CActiveScheduler::Start();
	delete timer;
	
	INFO_PRINTF1(KStoringChunk0);
	iImapMailStore->StoreAttachmentChunkL(filebuf1,iAttMessageEntry,0);	

	INFO_PRINTF1(KStoringChunk2);
	iImapMailStore->StoreAttachmentChunkL(filebuf3,iAttMessageEntry,2);	
	
	++iRequestCount;
	CActiveScheduler::Start();		
	}
	
/**
Reads back the downloaded attachment stored in using CMsvAttachment and test chunks where stored correctly.
*/
void CTestImapMailStoreAttachments::TestAttachment1L()
	{
	
	//test the attachment information stored in 
	INFO_PRINTF1(RestoringAttachment);
	
	//get the file path
	iServerEntry->SetEntry(iAttMessageEntry);
	CMsvStore* store=iServerEntry->ReadStoreL();
	
	CleanupStack::PushL(store);

	MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();
	ASSERT_EQUALS(attachmentMgr.AttachmentCount(), 1);
	
	//get the file path
	TFileName filePath;
	CMsvAttachment* attachment = attachmentMgr.GetAttachmentInfoL(0);
	filePath = attachment->FilePath();
	CleanupStack::PushL(attachment);
		
	//open the file
	INFO_PRINTF1(RestoringAttachment);
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile attachmentfile;	
	ASSERT_EQUALS(attachmentfile.Open(fsSession,filePath,EFileRead),KErrNone);
	
	//test the size of the attachment
	INFO_PRINTF1(KCheckingAttachmentSize);
	TInt attachmentSize;
	attachmentfile.Size(attachmentSize);
	ASSERT_EQUALS(attachmentSize,KAttachmentSize);

	//read the data	
	HBufC8* buffer8 = HBufC8::NewLC(attachmentSize);
	TPtr8 ptr = buffer8->Des();
	attachmentfile.Read(ptr);
	
	TInt chunk1Pos=buffer8->Find(KTestText8String1);
	TInt chunk2Pos=buffer8->Find(KTestText8String2);
	TInt chunk3Pos=buffer8->Find(KTestText8String3);	
	
	//check that chunks were stored in the correct order
	INFO_PRINTF1(KCheckingData);
	ASSERT_TRUE( (chunk1Pos<chunk2Pos) && (chunk2Pos<chunk3Pos));
	
	CleanupStack::PopAndDestroy(3,store);

	}
	
CTestSuite* CTestImapMailStoreAttachments::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestAttachment1L);
	END_SUITE;
	}
