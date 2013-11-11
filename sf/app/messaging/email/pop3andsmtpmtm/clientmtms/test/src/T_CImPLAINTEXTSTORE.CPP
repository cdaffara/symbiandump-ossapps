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
// T_PLAINTEXTSTORE.CPP
// 
//

#include "t_CImplaintextstore.h"
#include "cactivewaiter.h"

LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;
LOCAL_D TMsvId newMessageId;
LOCAL_D CTrapCleanup* theCleanup;


#define KTestMessageDir			_L("")

RTest test(KCImPlainBodyTest);

class CImPlainBodyText;
	
CTestCImPlainTextStore::CTestCImPlainTextStore()
	{
		iCImPlainBodyTextStore= NULL;
		iRequestCount = 0;
		iActiveWaiter = NULL;
	}
	

/**
Initialises the tests.
*/	
void CTestCImPlainTextStore::SetupL()
	{
	

	iActiveWaiter = new (ELeave) CActiveWaiter();
	
	testUtils = CEmailTestUtils::NewL(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	TBuf<KMaxFilePathLength>emailbodyfile(KEmail40kBodyFile);
	RFs fsSession;
	RFile infile; 
	
	
	
	User::LeaveIfError(fsSession.Connect()); 
 	User::LeaveIfError(infile.Open(fsSession, emailbodyfile, EFileRead));
	
	TInt size = 0;
	infile.Size(size);
 
 	//read the 8 bit data from the file
	TInt chunkLength = size / 3; 
	
 	HBufC8* tempBuf1 = HBufC8::NewL(chunkLength);	
 	TPtr8 tempBufPtr1(tempBuf1->Des());
 	infile.Read(tempBufPtr1,chunkLength);
	iFilebuf1 = HBufC16::NewL(chunkLength);  
	TPtr16 ptr1=iFilebuf1->Des();	  
	ptr1.Copy(tempBufPtr1);
	
	HBufC8* tempBuf2 = HBufC8::NewL(chunkLength);	
 	TPtr8 tempBufPtr2(tempBuf2->Des());
 	infile.Read(tempBufPtr2,chunkLength);
	iFilebuf2 = HBufC16::NewL(chunkLength);  
	TPtr16 ptr2=iFilebuf2->Des();	  
	ptr2.Copy(tempBufPtr2);
	
	HBufC8* tempBuf3 = HBufC8::NewL(chunkLength);	
 	TPtr8 tempBufPtr3(tempBuf3->Des());
	infile.Read(tempBufPtr3,chunkLength);
	iFilebuf3 = HBufC16::NewL(chunkLength);  
	TPtr16 ptr3=iFilebuf3->Des();	  
	ptr3.Copy(tempBufPtr3);
	
	infile.Close();
	
	
	testUtils->GoClientSideL();
	testUtils->ClearEmailAccountsL();
	smtpService = testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();
	testUtils->GoServerSideL();

	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	test.Printf(_L("\nPerforming Tests\n"));
	
	testUtils->GoClientSideL();

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
	CleanupStack::PopAndDestroy(accounts);

	TParse parsedFileName;
	TFileName name(KTestMessageDir);
	testUtils->ResolveLogFile(name, parsedFileName);

	
	// Get the default charset
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter,fsSession );
	CleanupStack::PushL(charConv);

	iDefaultCharset =	charConv->SystemDefaultCharset();
	CleanupStack::PopAndDestroy(2);
	fsSession.Close();
	
	}


void CTestCImPlainTextStore::TearDownL()
//virtual 
	{
	delete iCImPlainBodyTextStore;
	delete iFilebuf1;
	delete iFilebuf2;
	delete iFilebuf3;
	delete testUtils;
	delete iActiveWaiter;
	test.Close();
	
	}



/**
Create new Plaintext message
*/

void CTestCImPlainTextStore::CreateNewPlaintextMessageL()
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(iActiveWaiter->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId,KMsvNullIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP, CActive::EPriorityStandard, ETrue);
	CleanupStack::PushL(emailOperation);
	iActiveWaiter->WaitActive();
	
	//check progres
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();	
	paramPack.Set(progBuf);
	newMessageId=paramPack();
	CleanupStack::PopAndDestroy(emailOperation); 
	}
		
void CTestCImPlainTextStore::TestStoreRestore16BitChunkSynchL()
	{
	CreateNewPlaintextMessageL();
	CMsvEntry* entry = testUtils->iMsvSession->GetEntryL(newMessageId);
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*entry);
	iCImPlainBodyTextStore = imEmailMessage->OpenPlainBodyTextForWriteL();
	iCImPlainBodyTextStore->StoreChunkL(iFilebuf1->Des());

	TInt bodyTextSize = iFilebuf1->Length();
		
	iCImPlainBodyTextStore->StoreChunkL(iFilebuf2->Des());
	bodyTextSize += iFilebuf2->Length();
	
	iCImPlainBodyTextStore->StoreChunkL(iFilebuf3->Des());
	bodyTextSize += iFilebuf3->Length();
	
	iCImPlainBodyTextStore->CommitL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	delete iCImPlainBodyTextStore;
	iCImPlainBodyTextStore = NULL;
	
	//Restore the data in chunks.	
	HBufC16* filebuf11 = HBufC16::NewL(100+KTestStringLength );

	
	TInt origSize = bodyTextSize;
	TInt sizeOfRestoredText = 0;

	TPtr bufPtr = filebuf11->Des();
	TInt pos = 0;
	
	iCImPlainBodyTextStore = imEmailMessage->OpenPlainBodyTextForReadL(CImEmailMessage::EThisMessageOnly,100);
	iCImPlainBodyTextStore->NextChunkL(bufPtr);
	
	RArray<TPtr> chunkArray;
	chunkArray.Append(bufPtr);
	
	sizeOfRestoredText += bufPtr.Length();
	
	while(bufPtr.Length() > 0 && bufPtr.Length() == 100)
		{
		iCImPlainBodyTextStore->NextChunkL(bufPtr);
		// put retrived buffer in a array.
		chunkArray.Append(bufPtr);
		sizeOfRestoredText += bufPtr.Length();
		}

	ASSERT_EQUALS(bodyTextSize, sizeOfRestoredText);

	for(TInt j=chunkArray.Count() - 1;j >0; --j )
		{
		bufPtr.SetLength(0);
		iCImPlainBodyTextStore->PreviousChunkL(bufPtr);
		// Compare chunk retrieved with chunk retrieved by NextChunkL
		TInt comp = bufPtr.Compare(chunkArray[j-1]);
		ASSERT_EQUALS(comp, 0);
		}

	CleanupStack::PopAndDestroy(imEmailMessage); 
	}
			
void CTestCImPlainTextStore::TestStoreRestore16BitChunkAsyncL()
	{
	CreateNewPlaintextMessageL();
	
	CMsvEntry* entry = testUtils->iMsvSession->GetEntryL(newMessageId);
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*entry);
	
	iCImPlainBodyTextStore = imEmailMessage->OpenPlainBodyTextForWriteL();

	iCImPlainBodyTextStore->StoreChunkL(iFilebuf1->Des(),iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	TInt bodyTextSize = iFilebuf1->Length();
		
	iCImPlainBodyTextStore->StoreChunkL(iFilebuf2->Des(),iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	bodyTextSize += iFilebuf2->Length();
	
	iCImPlainBodyTextStore->StoreChunkL(iFilebuf3->Des(),iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	bodyTextSize += iFilebuf3->Length();
	
	iCImPlainBodyTextStore->CommitL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	delete iCImPlainBodyTextStore;
	iCImPlainBodyTextStore = NULL;
	
	//Restore the data in chunks.	
	HBufC16* filebuf11 = HBufC16::NewL(100+KTestStringLength);

	
	TInt origSize = bodyTextSize;
	TInt sizeOfRestoredText = 0;

	TPtr bufPtr = filebuf11->Des();
	TInt pos = 0;
	
	iCImPlainBodyTextStore = imEmailMessage->OpenPlainBodyTextForReadL(CImEmailMessage::EThisMessageOnly,100);
	iCImPlainBodyTextStore->NextChunkL(bufPtr,iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	RArray<TPtr> chunkArray;
	chunkArray.Append(bufPtr);
	
	sizeOfRestoredText += bufPtr.Length();
	
	while(bufPtr.Length() > 0 && bufPtr.Length() == 100)
		{
		iCImPlainBodyTextStore->NextChunkL(bufPtr,iActiveWaiter->iStatus);
		iActiveWaiter->WaitActive();
		// put retrived buffer in a array.
		chunkArray.Append(bufPtr);
		sizeOfRestoredText += bufPtr.Length();
		}

	ASSERT_EQUALS(bodyTextSize, sizeOfRestoredText);

	for(TInt j=chunkArray.Count() - 1;j >0; --j )
		{
		bufPtr.SetLength(0);
		iCImPlainBodyTextStore->PreviousChunkL(bufPtr,iActiveWaiter->iStatus);
		iActiveWaiter->WaitActive();
		// Compare chunk retrieved with chunk retrieved by NextChunkL
		TInt comp = bufPtr.Compare(chunkArray[j-1]);
		ASSERT_EQUALS(comp, 0);
		}
	CleanupStack::PopAndDestroy(imEmailMessage); 
	}

void CTestCImPlainTextStore::TestStoreRichTextAsPlainTextL()
	{	
	CreateNewPlaintextMessageL();
	
	CMsvEntry* entry = testUtils->iMsvSession->GetEntryL(newMessageId);
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*entry);
	
	iCImPlainBodyTextStore = imEmailMessage->OpenPlainBodyTextForWriteL();

	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
		
	CRichText* richText=CRichText::NewL(paraLayer, charLayer, CEditableText::EFlatStorage, KBufferGranularity);


	TBuf<50> theMessageContents = _L("Test message contents");
	richText->InsertL(0, theMessageContents);
	TInt textLength = richText->DocumentLength();
	
	iCImPlainBodyTextStore->StoreRichTextAsPlainTextL(*richText);
	iCImPlainBodyTextStore->CommitL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	delete richText;

	richText=CRichText::NewL(paraLayer, charLayer, CEditableText::EFlatStorage, KBufferGranularity);
	imEmailMessage->GetBodyTextL(newMessageId, CImEmailMessage::EThisMessageOnly, *richText, *paraLayer, *charLayer);	
	richText->DocumentLength();
	ASSERT_EQUALS(richText->DocumentLength(), textLength);

	delete richText;
	delete charLayer;
	delete paraLayer;
	CleanupStack::PopAndDestroy(imEmailMessage); 
	}	

CTestSuite* CTestCImPlainTextStore::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestStoreRestore16BitChunkSynchL);
	ADD_ASYNC_TEST_STEP(TestStoreRestore16BitChunkAsyncL);
	ADD_ASYNC_TEST_STEP(TestStoreRichTextAsPlainTextL);
	END_SUITE;
	}
	

	
	
