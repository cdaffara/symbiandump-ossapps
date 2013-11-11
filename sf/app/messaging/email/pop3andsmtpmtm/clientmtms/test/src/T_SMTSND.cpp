// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_SMTSND
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests SMTP SendAs functionality - creates SMTP messages; adds recipients; 
// store the body text; saves message; adds/removes attachments;
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// \MSG\IMCMTSRC\rfc82201.txt		<DRIVE>:\msgtest\imcm\rfc82201.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_SMTSND\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_SMTSND.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_SMTSND.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_SMTSND.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform 
// 4. Run T_SMTSND.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <e32uid.h>
#include <txtrich.h>
#include <msvids.h>
#include "smtcmtm.h"	// CSmtpClientMtm
#include "smtpset.h"	// CImSmtpSettings
#include <miutmsg.h>
#include "miutlog.h"	// CImLog
#include <mtmuids.h>
#include <mtmdef.h>
#include <cemailaccounts.h>

	#include <mmsvattachmentmanager.h>
	#include <sendas2.h>

RTest	test(_L("SMTP SendAs Testrig"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CImLog* log;

#define KRecipient _L("test@psion9.demon.co.uk")
#define KRecipientAlias _L("Test Account")


//


//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
 	testUtils->ClearEmailAccountsL();

	testUtils->GoServerSideL();
	
	test.Next(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();

	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("******** T_SMTSND Test (SMTP) Send As API ********"));
	TBuf8<80> buf;
#if defined(__WINS__)
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_SMTSND Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void TestSMTSendL(CMsvEntry& aEntry, TMsvId aDestination)
	{
	// Get count of children
	aEntry.SetEntryL(KMsvRootIndexEntryId);
	aEntry.SetEntryL(aDestination);
	TInt children = aEntry.Count();

	CTestTimer* timer = CTestTimer::NewL();
	timer->After(0x200000);
	CActiveScheduler::Start();
	delete timer;
	
	// rich text
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the SendAs API.  We \06 can try using a delimiter");
	bodyText->InsertL(0, bodyContents);
		
	
	RSendAs sa;
	TInt err = sa.Connect();
	User::LeaveIfError(err);
	CleanupClosePushL(sa);
	
	RSendAsMessage sa_msg;
	sa_msg.CreateL(sa, KUidMsgTypeSMTP);
	
	CleanupClosePushL(sa_msg);
	
	sa_msg.AddRecipientL(KRecipient, RSendAsMessage::ESendAsRecipientTo);
	sa_msg.AddRecipientL(KRecipient, KRecipientAlias, RSendAsMessage::ESendAsRecipientTo);
	
	sa_msg.SetSubjectL(_L("t_smtsnd message test 1"));
	sa_msg.SetBodyTextL(*bodyText);
	
	sa_msg.SaveMessageAndCloseL();
	
	CleanupStack::Pop(); // sa_msg
	CleanupStack::PopAndDestroy(); // sa
	

	// Check child count has increased
	aEntry.SetEntryL(KMsvRootIndexEntryId);
	aEntry.SetEntryL(aDestination);
	test(aEntry.Count() == children + 1);

	// Check the body text

	// ok, need to find the message entry
	TMsvId message = 0x00000000;
	TMsvEntry msgEntry;
	TBool found = EFalse;
	CMsvEntrySelection* sel = aEntry.ChildrenL();
	CleanupStack::PushL(sel);
	
	// Loop through the sub-entries
	for(TInt i=0;i<sel->Count();i++)
	{
		// Reset the entry to the child[i]
		aEntry.SetEntryL(sel->At(i));
		msgEntry = aEntry.Entry();
		if (msgEntry.iDescription.Compare(_L("t_smtsnd message test 1")) == 0)
		{
			message = aEntry.EntryId();
			found = ETrue;
			break;
		}
	}
	
	CleanupStack::PopAndDestroy(); // sel
	
	// make sure we actually found our message
	if (!found)
		User::Leave(KErrNotFound);


	// Check we've got what we wanted
	aEntry.SetEntryL(message);
	test(aEntry.Entry().Parent() == aDestination);

	CRichText* validRichText1=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(validRichText1);

	CMsvOperationWait* iWait=CMsvOperationWait::NewLC();

	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*testUtils->iMsvEntry);
	imEmailMessage->GetBodyTextL(iWait->iStatus, msgEntry.Id(), CImEmailMessage::EThisMessageOnly, *validRichText1, *paraFormatLayer, *charFormatLayer);
	iWait->Start();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	TBuf<100> bufa = bodyText->Read(0);
	TBuf<100> bufb = validRichText1->Read(0);

	test((bufa.Compare(bufb))==0);

	aEntry.SetEntryL(msgEntry.Id());
	CMsvStore* store = aEntry.ReadStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();
	header->RestoreL(*store);

	// check that there are 2 recipients
	test(header->ToRecipients().Count() == 2);

	// check that the alias in the second recipient is not lost!
	TBuf<50> recipient;
	recipient.Append('\"');
	recipient.Append(KRecipientAlias);
	recipient.Append('\"');
	recipient.Append(' ');
	recipient.Append('<');
	recipient.Append(KRecipient);
	recipient.Append('>');
	
	TPtrC toRecipient = header->ToRecipients()[1];
	test(toRecipient.CompareF(recipient)==0);

	CleanupStack::PopAndDestroy(header);
	CleanupStack::PopAndDestroy(store);

	CleanupStack::PopAndDestroy(2); //imEmailMessage, iWait

	CleanupStack::PopAndDestroy(4);	//bodyText, validRichText1, paraFormatLayer, charFormatLayer
	}

LOCAL_C void TestSMTSendL()
	{
	TMsvDummyObserver* ob = new(ELeave) TMsvDummyObserver;
	CleanupStack::PushL(ob);

	CMsvSession* session = CMsvSession::OpenSyncL(*ob);
	CleanupStack::PushL(session);

	CMsvEntry* cEntry = session->GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(cEntry);

	
	// with the new SendAs2 API messages are always created in Drafts
	TestSMTSendL(*cEntry, KMsvDraftEntryId);
	

	CleanupStack::PopAndDestroy(3); // cEntry, session, ob
	}

LOCAL_C void TestCapabilityL()
	{
	
	RSendAs sa;
	User::LeaveIfError(sa.Connect());
	CleanupClosePushL(sa);
	
	sa.FilterAgainstCapability(KUidMtmQuerySupportedBody);
	sa.FilterAgainstCapability(KUidMtmQuerySupportAttachments);
	sa.FilterAgainstCapability(KUidMtmQuerySupportSubject);
	sa.FilterAgainstCapability(KUidMtmQueryCanSendMsg);
	
	CSendAsMessageTypes* msgTypes = CSendAsMessageTypes::NewL();
	CleanupStack::PushL(msgTypes);
	
	sa.FilteredMessageTypesL(*msgTypes);
	
	// make sure SMTP is still in the list
	TInt found = 0;
	TInt count = msgTypes->Count();
	for (TInt i = 0; i < count; i++)
	{
		if (msgTypes->MessageTypeUid(i) == KUidMsgTypeSMTP)
		{
			found++;
			break;
		}
	}
	
	test(found > 0);
	
	// now set a capability not supported by SMTP
	msgTypes->Reset();
	sa.FilterAgainstCapability(KUidMtmQuerySupportsFolder);
	sa.FilteredMessageTypesL(*msgTypes);
	
	// make sure SMTP is no longer in the list
	found = EFalse;
	count = msgTypes->Count();
	for (TInt i = 0; i < count; i++)
	{
		if (msgTypes->MessageTypeUid(i) == KUidMsgTypeSMTP)
		{
			found = ETrue;
			break;
		}
	}
	
	test(found == 0);
	
	CleanupStack::PopAndDestroy(2); // sa, msgTypes
	
	}

LOCAL_C void TestValidationL()
	{
	
// Looks line validation functionality is not part of the new SendAs2 API, so we skip this test	
	}
	
LOCAL_C void TestAttachmentL(CMsvEntry& aEntry, TMsvId aDestination)
	{
	// Get count of children
	aEntry.SetEntryL(KMsvRootIndexEntryId);
	aEntry.SetEntryL(aDestination);
	TInt childCount = aEntry.Count();

	CTestTimer* timer = CTestTimer::NewL();
	timer->After(0x200000);
	CActiveScheduler::Start();
	delete timer;

	// rich text
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the SendAs API.  We \06 can try using a delimiter");
	bodyText->InsertL(0, bodyContents);
		

	RSendAs sa;
	User::LeaveIfError(sa.Connect());
	CleanupClosePushL(sa);
	
	RSendAsMessage sa_msg;
	sa_msg.CreateL(sa, KUidMsgTypeSMTP);
	CleanupClosePushL(sa_msg);
	
	sa_msg.AddRecipientL(KRecipient, RSendAsMessage::ESendAsRecipientTo);
	
	sa_msg.SetSubjectL(_L("t_smtsnd message test 2"));
	sa_msg.SetBodyTextL(*bodyText);
	
	TRequestStatus status;
	sa_msg.AddAttachment(_L("C:\\mailtest\\imcm\\rfc82201.txt"), status);
	User::WaitForRequest(status);
	
	sa_msg.SaveMessageAndCloseL();
	
	TMsvId message = 0x00000000;
	TMsvEntry msgEntry;
	TBool found = EFalse;
	
	aEntry.SetEntryL(KMsvRootIndexEntryId);
	aEntry.SetEntryL(aDestination);
	
	CMsvEntrySelection* sel = aEntry.ChildrenL();
	CleanupStack::PushL(sel);
	
	// Loop through the sub-entries
	for(TInt i=0;i<sel->Count();i++)
	{
		// Reset the entry to the child[i]
		aEntry.SetEntryL(sel->At(i));
		msgEntry = aEntry.Entry();
		if (msgEntry.iDescription.Compare(_L("t_smtsnd message test 2")) == 0)
		{
			message = aEntry.EntryId();
			found = ETrue;
			break;
		}
	}
	
	CleanupStack::PopAndDestroy(); // sel
	
	// make sure we actually found our message
	if (!found)
		User::Leave(KErrNotFound);

	// Check child count has increased
	aEntry.SetEntryL(KMsvRootIndexEntryId);
	aEntry.SetEntryL(aDestination);
	test(aEntry.Count() == childCount + 1);

	// Check we've got what we wanted
	aEntry.SetEntryL(message);
	test(aEntry.Entry().Parent() == aDestination);
	testUtils->iMsvEntry->SetEntryL(msgEntry.Id());


	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*testUtils->iMsvEntry);
	
	// Testing the message has one attachment using attachment manager
	TInt count = imEmailMessage->AttachmentManager().AttachmentCount();
	test(count == 1);
	
	// open the file for read, using index
	RFile file = imEmailMessage->AttachmentManager().GetAttachmentFileL(count-1);
	CleanupClosePushL(file);
	TInt size = 0;
	test(KErrNone == file.Size(size));
	HBufC8* buf8 = HBufC8::NewLC(size);
	TPtr8 des = buf8->Des();
	test(KErrNone == file.Read(des));
	CleanupStack::PopAndDestroy(2,&file); // buf8, file 

	// open the file for read, using attachment id
	
	// with the new API we don't know the attachment ID, so we skip this test for SendAs2

	_LIT8(KTestData,"Some test data");
	
	// open the file for write, using index
	file = imEmailMessage->AttachmentManager().GetAttachmentFileForWriteL(count-1);
	CleanupClosePushL(file);
	TInt filePosition = size;
	test(KErrNone == file.Write(filePosition,KTestData));
	file.Flush();
	
	
	// get the updated file size
	test(KErrNone == file.Size(size));
	CleanupStack::PopAndDestroy(&file); 
		
	// get the attachment info, using index
	CMsvAttachment* attachmentInfo = imEmailMessage->AttachmentManager().GetAttachmentInfoL(count-1);
	CleanupStack::PushL(attachmentInfo);


	// modify size information in attachment info
	attachmentInfo->SetSize(size);
		
	// Test modify the attachment info
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::Pop(attachmentInfo); // pass ownership to attachment manager
	CleanupStack::PushL(testActive);
	
	imEmailMessage->AttachmentManager().ModifyAttachmentInfoL(attachmentInfo,testActive->iStatus);	
	
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete
	TInt result = testActive->iStatus.Int(); 

	
	CleanupStack::PopAndDestroy(2,imEmailMessage); // testActive , imEmailMessage
	testUtils->iMsvEntry->SetEntryL(msgEntry.Id());

	
	// Check that the structure of message is:
	//	MESSAGE
	//		FOLDER_MIX
	//			TEXT
	//			ATTACHMENT

	CMsvEntrySelection* children = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(children);
	test(children->Count() == 1);

	TMsvId serviceId;
	TMsvEmailEntry entry;
	testUtils->iMsvEntry->Session().GetEntry(children->At(0), serviceId, entry);
	test(entry.MessageFolderType() == EFolderTypeMixed);

	testUtils->SetEntryL(children->At(0));
	
	CleanupStack::PopAndDestroy(); //children
	children = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(children);
	test(children->Count() == 2);
	testUtils->iMsvEntry->Session().GetEntry(children->At(0), serviceId, entry);
	test(entry.iType == KUidMsvAttachmentEntry);


	testUtils->iMsvEntry->Session().GetEntry(children->At(1), serviceId, entry);
	test(entry.iType == KUidMsvEmailTextEntry);

	CleanupStack::PopAndDestroy(children); //children
	CleanupStack::PopAndDestroy(2);	//sendAs, bodyText
	CleanupStack::PopAndDestroy(3); // ob, paraFormatLayer, charFormatLayer
	}

LOCAL_C void TestAttachmentL()
	{
	TMsvDummyObserver* ob = new(ELeave) TMsvDummyObserver;
	CleanupStack::PushL(ob);

	CMsvSession* session = CMsvSession::OpenSyncL(*ob);
	CleanupStack::PushL(session);

	CMsvEntry* cEntry = session->GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(cEntry);


	// with the new SendAs2 API messages are always created in Drafts
	TestAttachmentL(*cEntry, KMsvDraftEntryId);

	CleanupStack::PopAndDestroy(3); // cEntry, session, ob
	}

LOCAL_C void doMainL()
	{
	InitL();

	testUtils->GoClientSideL();

	TMsvId smtpService = testUtils->CreateSmtpServiceL();

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
	CleanupStack::PopAndDestroy(accounts);

	testUtils->TestStart(1);
	test.Start(_L("Send SMTP message"));
	TestSMTSendL();
	log->AppendComment(_L8("Test 1 OK\n"));
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	test.Next(_L("Query message capability"));
	TestCapabilityL();
	log->AppendComment(_L8("Test 2 OK\n"));
	testUtils->TestFinish(2);
	
	testUtils->TestStart(3);
	test.Next(_L("Message validation"));
	TestValidationL();
	log->AppendComment(_L8("Test 3 OK\n"));
	testUtils->TestFinish(3);
	
	testUtils->TestStart(4);
	test.Next(_L("Add attachment Test"));
	TestAttachmentL();
	log->AppendComment(_L8("Test 4 OK\n"));
	testUtils->TestFinish(4);

	log->AppendComment(_L8("Tests completed\n"));
	testUtils->TestHarnessCompleted();

	Closedown();		
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
