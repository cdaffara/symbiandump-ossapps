// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_imapmailstoreplainbodydata.h"

const TInt KBodySize=0x0001DE2C;

/**
@SYMTestCaseID MSG-IMAP-MAILSTORE-0007
@SYMTestType UT
@SYMTestPriority High
@SYMPREQ 1432
@SYMTestCaseDesc Tests storage of plaintext 16 bit objects used to store body data using CImapMailStore..
@SYMTestActions  1. Creates a list of index entries into which the body and its CImMimeHeader header will be stored.
				 2. Creates CImMimeHeader populated with test values and reads test body data from a file.
				 3. Copies the data into 3 chunks, appending an identifying string to each chunk. 
				 4. Uses CImapMailStore to store the chunks out of order.
				 5. Uses CMsvStore to read back the body data.
				 6. Tests the body was stored correctly.

@SYMTestExpectedResults The chunks should have all been stored and the order should have been corrected.
*/


_LIT(KTesting16BitBodyData,"Testing Storage of plain body data...");
	
CTestImapMailStorePlainBodyData::CTestImapMailStorePlainBodyData()
	{	
	}
	
/**
Calls the base classes SetupL() to clean the message store and initialise test.
*/
	
void CTestImapMailStorePlainBodyData::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();	
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	CleanupStack::Pop(mimeHeader); 
	INFO_PRINTF1(KStoringCImMimeHeader);
	iImapMailStore->StorePartL(mimeHeader,iBodyMessageEntry,*this);
	++iRequestCount;
	CActiveScheduler::Start();	
	
	INFO_PRINTF1(KTesting16BitBodyData);
	//store the body data in the wrong order	
	TImEncodingType encoding = EEncodingTypeNone;
	
	iInfo=CFetchBodyInfo::NewL(iBodyMessageEntry);
	iInfo->SetContentTransferEncoding(encoding);
	iInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iSettings = CImapSettings::NewL(*iServerEntry);	
	}

void CTestImapMailStorePlainBodyData::TestInitialiseBodyDataL()
	{
	//read body text form a file
	TBuf<KMaxFilePathLength>emailbodyfile(KEmail40kBodyFile);
	RFs fsSession;
	RFile infile; 
		
	User::LeaveIfError(fsSession.Connect()); 
 	User::LeaveIfError(infile.Open(fsSession, emailbodyfile, EFileRead));
	TInt size;
	infile.Size(size);
 
 	//read the 8 bit data from the file
	HBufC8* filebuf1 = HBufC8::NewLC(size+KTestStringLength);  
	TPtr8 ptr1=filebuf1->Des();	  
	infile.Read(ptr1);
	CleanupStack::Pop(filebuf1);
	
	//read the 2nd chunk
	HBufC8* filebuf2 = HBufC8::NewLC(size+KTestStringLength);  
	TPtr8 ptr2=filebuf2->Des();	  
	ptr2.Copy(ptr1);
	CleanupStack::Pop(filebuf2);
	
	//read the 3rd chunk
	HBufC8* filebuf3 = HBufC8::NewLC(size+KTestStringLength);  
	TPtr8 ptr3=filebuf3->Des();	  
	ptr3.Copy(ptr1);
	CleanupStack::Pop(filebuf3);
	
	//we dont need to read from the file anymore
	infile.Close();
	fsSession.Close();
	
	//append test strings to identify each chunk
	ptr1.Append(KTestTextString1);
	ptr2.Append(KTestTextString2);	
	ptr3.Append(KTestTextString3);		

	
	iImapMailStore->InitialiseStorePlainBodyTextL(3,*iSettings,*iInfo,KLogId,*this,*this,iBinaryCap);
	
	INFO_PRINTF1(KStoringChunk1);
	
	iImapMailStore->StorePlainBodyTextL(filebuf2, iBodyMessageEntry, 1);
	
	//this is to exercise CBodyQueueEntry16 where not all data is available when first activated. 
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(K2Seconds);
	CActiveScheduler::Start();
	delete timer;
	

	
	INFO_PRINTF1(KStoringChunk2);
	iImapMailStore->StorePlainBodyTextL(filebuf3 ,iBodyMessageEntry, 2);
	
	INFO_PRINTF1(KStoringChunk0);
	iImapMailStore->StorePlainBodyTextL(filebuf1, iBodyMessageEntry, 0);

	++iRequestCount;
	CActiveScheduler::Start();
		
	}

/**
Reads back the body data using CRichText and test chunks where stored correctly.
*/
void CTestImapMailStorePlainBodyData::TestBodyData1L()
	{
	TestInitialiseBodyDataL();
	INFO_PRINTF1(KRestoringBodyData);
	
	iServerEntry->SetEntry(iBodyMessageEntry);
	CMsvStore* store=iServerEntry->ReadStoreL();
	CleanupStack::PushL(store);   	
   	// Create a rich text object...
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);
	
	CRichText* richText=CRichText::NewL(paraLayer, charLayer, CEditableText::EFlatStorage, KBufferGranularity);
	CleanupStack::PushL(richText);
	
	store->RestoreBodyTextL(*richText);	

	//load the body text
   	TPtrC view(KNullDesC);
	TCharFormat format;
	richText->GetChars(view, format, 0);	

	INFO_PRINTF1(KCheckingBodyData);
	
	TInt cc = richText->DocumentLength();
	ASSERT_EQUALS(richText->DocumentLength(), KBodySize);

	TInt chunk1Pos=view.Find(KTestTextString1);
	TInt chunk2Pos=view.Find(KTestTextString2);
	TInt chunk3Pos=view.Find(KTestTextString3);
	
	//check that chunks were stored in the correct order
	INFO_PRINTF1(KCheckingData);
	ASSERT_TRUE( (chunk1Pos<chunk2Pos) && (chunk2Pos<chunk3Pos));
	
	CleanupStack::PopAndDestroy(4,store);
	}

#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
void CTestImapMailStorePlainBodyData::TestBodyData2L()
	{
	iBinaryCap = ETrue;
	TestBodyData1L();
	}
#endif

CTestSuite* CTestImapMailStorePlainBodyData::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestBodyData1L);
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	ADD_ASYNC_TEST_STEP(TestBodyData2L);
#endif
	END_SUITE;
	}
