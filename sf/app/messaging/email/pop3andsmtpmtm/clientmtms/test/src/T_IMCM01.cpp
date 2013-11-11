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
// Name of test harness: T_IMCM01
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Installs SMTP, POP3, IMAP4 MTMs, creates SMTP, POP3 IMAP4 emails and tests
// global find implementation in SMTP, POP3, IMAP4 client 
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM01\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM01.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM01.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM01.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM01.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <msvfind.h>
#include <miutlog.h>
#include <msvsearchsortquery.h>
#include <msvsearchsortoperation.h>


#define _L8(a) (TPtrC8((const TText8 *)(a)))

RTest test(_L("Find Testrig"));
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;

LOCAL_D TMsvId folderId;
LOCAL_D TMsvId entry1Id;
LOCAL_D TMsvId entry2Id;
LOCAL_D TMsvId entry3Id;


TBuf<32> ShortDescription(_L("find some text in here"));
TBuf<32> ShortDetails(_L("find some text in here"));

TBuf<32> MatchingDescription(_L("abcDEFghi Jkl mno"));
TBuf<32> MatchingDetails(_L("abcDEFghi Jkl mno"));

const TUid KUidFindTestPOP3Mtm = {0x10001029};
const TUid KUidFindTestSMTPMtm = {0x10001028};
const TUid KUidFindTestIMAP4Mtm = {0x1000102A};

_LIT(KMiutSubject, "short mail about something interesting");
_LIT8(KMiutMsgId, "0123456789AB.CdeFGHIj@symbian.com");
_LIT(KMiutFrom, "ralph-greenwell@psion.com");
_LIT(KMiutReplyTo, "ralph.greenwell@virgin.net");
_LIT(KMiutToRecipients1, "recipient1@address.com");
_LIT(KMiutToRecipients2, "recipient2@address.com");
_LIT(KMiutCcRecipients1, "cc1@address.com");
_LIT(KMiutCcRecipients2, "cc2@address.com");
_LIT(KMiutBccRecipients, "bcc@address.com");
_LIT8(KMiutResentMsgId, "0123456789AB.CdeFGHIj@symbian.com");
_LIT(KMiutResentFrom, "test@psion6.demon.co.uk");
_LIT(KMiutResentToRecipients1, "recipient1@address.com");
_LIT(KMiutResentToRecipients2, "recipient2@address.com");
_LIT(KMiutResentCcRecipients1, "cc1@address.com");
_LIT(KMiutResentCcRecipients2, "cc2@address.com");
_LIT(KMiutResentBccRecipients, "bcc@address.com");

#define KMiutRemoteSize 					123456
#define KMaxImHeaderStringLengthLong		KMaxImHeaderStringLength+1


LOCAL_D TBuf<KMaxImHeaderStringLengthLong> longValue;
LOCAL_D TBuf8<KMaxImHeaderStringLengthLong> longValue2;


//

//**********************************
// TMsvFindResultDateRange
//**********************************
//
// An enumeration that specifies the date comparison to perform
//

enum TMsvFindResultDateRange
	{
	EMsvOnDate,
	EMsvBeforeDate,
	EMsvAfterDate
	};

//**********************************
// CMsvFindDateOperation
//**********************************
//
// An operation that extends global find to restrict searching by date
//

class CMsvFindDateOperation : public CMsvFindOperation
	{
public:
	static CMsvFindDateOperation* FindInSelectionL(CMsvSession& aMsvSession, const TDesC& aTextToFind, const CMsvEntrySelection& aSel, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus);
	static CMsvFindDateOperation* FindInChildrenL(CMsvSession& aMsvSession, const TDesC& aTextToFind, TMsvId aId, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus);
	//
protected:
	CMsvFindDateOperation(CMsvSession& aMsvSession, const TDesC& aTextToFind, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus);
	//
private:
	TBool IsValid(const TMsvEntry& aEntry) const;
	//
private:
	TTime iDate;
	TMsvFindResultDateRange iRange;
	};

CMsvFindDateOperation* CMsvFindDateOperation::FindInSelectionL(CMsvSession& aMsvSession, const TDesC& aTextToFind, const CMsvEntrySelection& aSel, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus)
//
//
//
	{
	CMsvFindDateOperation* self = new(ELeave)CMsvFindDateOperation(aMsvSession, aTextToFind, aPartList, aDate, aRange, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructFindInSelectionL(aSel);
	CleanupStack::Pop(); // self
	return self;	
	}

CMsvFindDateOperation* CMsvFindDateOperation::FindInChildrenL(CMsvSession& aMsvSession, const TDesC& aTextToFind, TMsvId aId, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus)
//
// 
//
	{
	CMsvFindDateOperation* self = new(ELeave)CMsvFindDateOperation(aMsvSession, aTextToFind, aPartList, aDate, aRange, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructFindInChildrenL(aId);
	CleanupStack::Pop(); // self
	return self;	
	}

CMsvFindDateOperation::CMsvFindDateOperation(CMsvSession& aMsvSession, const TDesC& aTextToFind, TMsvPartList aPartList, const TTime& aDate, TMsvFindResultDateRange aRange, TRequestStatus& aObserverRequestStatus)
: CMsvFindOperation(aMsvSession, aTextToFind, aPartList, aObserverRequestStatus), iDate(aDate), iRange(aRange)
	{
	}

TBool CMsvFindDateOperation::IsValid(const TMsvEntry& aEntry) const
//
//
//
	{
	TDateTime dt;
		
	dt = aEntry.iDate.DateTime();
	TTime date1(TDateTime(dt.Year(), dt.Month(), dt.Day(), 0, 0, 0, 0));

	dt = iDate.DateTime();
	TTime date2(TDateTime(dt.Year(), dt.Month(), dt.Day(), 0, 0, 0, 0));

	switch (iRange)
		{
		case EMsvOnDate:
			return date1 == date2;
		case EMsvBeforeDate:
			return date1 < date2;
		case EMsvAfterDate:
			return date1 > date2;
		}
	return EFalse;
	}

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
	testUtils->GoServerSideL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("******* T_IMCM01 Test Client MTM Global Find *******"));
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
	log->AppendComment(_L8("**********    T_IMCM01 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler 
	}

LOCAL_C void CreateMessageHeader(CImHeader& imHeader)
	{
	// a bunch of data for the header object....
	imHeader.Reset();
	imHeader.SetSubjectL(KMiutSubject);

	imHeader.SetImMsgIdL(KMiutMsgId);

	imHeader.SetFromL(KMiutFrom);
	imHeader.SetReplyToL(KMiutReplyTo);
	imHeader.SetRemoteSize(KMiutRemoteSize);

	imHeader.ToRecipients().AppendL(KMiutToRecipients1);
	imHeader.ToRecipients().AppendL(KMiutToRecipients2);
	imHeader.CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader.CcRecipients().AppendL(KMiutCcRecipients2);
	imHeader.BccRecipients().AppendL(KMiutBccRecipients);
	}

LOCAL_C void CreateMessageHeader2(CImHeader& imHeader)
	{
	// a bunch of data for the header object....
	imHeader.Reset();
	imHeader.SetSubjectL(KMiutSubject);

	imHeader.SetImMsgIdL(KMiutMsgId);

	imHeader.SetFromL(KMiutFrom);
	imHeader.SetReplyToL(KMiutReplyTo);
	imHeader.SetRemoteSize(KMiutRemoteSize);

	imHeader.ToRecipients().AppendL(KMiutToRecipients1);
	imHeader.ToRecipients().AppendL(KMiutToRecipients2);
	imHeader.CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader.CcRecipients().AppendL(KMiutCcRecipients2);
	imHeader.BccRecipients().AppendL(KMiutBccRecipients);
	
	imHeader.SetResentMsgIdL(KMiutResentMsgId);
	imHeader.SetResentFromL(KMiutResentFrom);
	imHeader.ResentToRecipients().AppendL(KMiutResentToRecipients1);
	imHeader.ResentToRecipients().AppendL(KMiutResentToRecipients2);
	imHeader.ResentCcRecipients().AppendL(KMiutResentCcRecipients1);
	imHeader.ResentCcRecipients().AppendL(KMiutResentCcRecipients2);
	imHeader.ResentBccRecipients().AppendL(KMiutResentBccRecipients);
	}

LOCAL_C void CreateLongMessageHeader(CImHeader& imHeader)
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	imHeader.Reset();
	longValue.Fill('x',KMaxImHeaderStringLengthLong);
	longValue2.Fill('x',KMaxImHeaderStringLengthLong);

	imHeader.SetSubjectL(longValue);

	imHeader.SetImMsgIdL(longValue2);

	imHeader.SetFromL(longValue);
	imHeader.SetReplyToL(longValue);
	imHeader.SetRemoteSize(KMiutRemoteSize);

	imHeader.ToRecipients().AppendL(longValue);
	imHeader.ToRecipients().AppendL(longValue);
	imHeader.CcRecipients().AppendL(longValue);
	imHeader.CcRecipients().AppendL(longValue);
	imHeader.BccRecipients().AppendL(longValue);

	imHeader.SetResentMsgIdL(longValue2);
	imHeader.SetResentFromL(longValue);
	}

LOCAL_C void CreateMessagesL(TUid aMtm)
	{
	CParaFormatLayer* paraLayer = CParaFormatLayer::NewL();
	CleanupStack::PushL(paraLayer);
	CCharFormatLayer* charLayer = CCharFormatLayer::NewL();
	CleanupStack::PushL(charLayer);

	// create entry to work under
	TMsvEntry folder;
	folder.iType = KUidMsvFolderEntry;
	folder.iMtm = KUidMsvLocalServiceMtm;
	folder.iServiceId = KMsvLocalServiceIndexEntryId;
	testUtils->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(folder);
	testUtils->iMsvEntry->SetEntryL(folder.Id());
	folderId=folder.Id();

	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = aMtm;
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(entry1);
	TRAPD(error,testUtils->iMsvEntry->SetEntryL(entry1.Id()));
	entry1Id=entry1.Id();

	CMsvStore* fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);

	CImHeader* header = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CreateMessageHeader(*header);
	header->StoreL(*fileStore);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(2); // filestore, header

	TMsvEmailEntry ementry1;
	ementry1.iMtm = aMtm;
	ementry1.iServiceId = testUtils->iMsvEntry->Entry().iServiceId;
	ementry1.iType = KUidMsvEmailTextEntry;
	testUtils->iMsvEntry->CreateL(ementry1);
	TRAP(error,testUtils->iMsvEntry->SetEntryL(ementry1.Id()));

	fileStore=NULL;
	fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	CRichText* text1=CRichText::NewL(paraLayer, charLayer);
	CleanupStack::PushL(text1);
	text1->InsertL(0,_L("Some Rich Text 1"));
	fileStore->StoreBodyTextL(*text1);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(2); // filestore


	fileStore=NULL;
	TMsvEntry entry2;
	entry2.iType = KUidMsvMessageEntry;
	entry2.iMtm = aMtm;
	entry2.iServiceId = KMsvLocalServiceIndexEntryId;
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(entry2);
	TRAP(error,testUtils->iMsvEntry->SetEntryL(entry2.Id()));
	entry2Id=entry2.Id();

	fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	CImHeader* header1 = CImHeader::NewLC(); 
	CreateMessageHeader2(*header1);
	header1->StoreL(*fileStore);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(2); // filestore,header


	TMsvEmailEntry ementry2;
	ementry2.iMtm = aMtm;
	ementry2.iServiceId = testUtils->iMsvEntry->Entry().iServiceId;
	ementry2.iType = KUidMsvEmailTextEntry;
	testUtils->iMsvEntry->CreateL(ementry2);
	TRAP(error,testUtils->iMsvEntry->SetEntryL(ementry2.Id()));

	fileStore=NULL;
	fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	CRichText* text2=CRichText::NewL(paraLayer, charLayer);
	CleanupStack::PushL(text2);
	text2->InsertL(0,_L("Some Rich Text 2"));
	fileStore->StoreBodyTextL(*text2);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(2); // filestore,text


	fileStore=NULL;
	TMsvEntry entry3;
	entry3.iType = KUidMsvMessageEntry;
	entry3.iMtm = aMtm;
	entry3.iServiceId = KMsvLocalServiceIndexEntryId;
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(entry3);
	TRAP(error,testUtils->iMsvEntry->SetEntryL(entry3.Id()));
	entry3Id=entry3.Id();
	
	fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	CImHeader* header2 = CImHeader::NewLC(); 
	CreateLongMessageHeader(*header2);
	header2->StoreL(*fileStore);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(2); // filestore,header
	
	
	TMsvEmailEntry ementry3;
	ementry3.iMtm = aMtm;
	ementry3.iServiceId = testUtils->iMsvEntry->Entry().iServiceId;
	ementry3.iType = KUidMsvEmailTextEntry;
	testUtils->iMsvEntry->CreateL(ementry3);
	TRAP(error,testUtils->iMsvEntry->SetEntryL(ementry3.Id()));

	fileStore=NULL;
	fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	CRichText* text3=CRichText::NewL(paraLayer, charLayer);
	CleanupStack::PushL(text3);
	text3->InsertL(0,_L("Some Rich Text 3"));
	fileStore->StoreBodyTextL(*text3);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(4); // filestore,header,text,layers
	}


LOCAL_C void TestFindInSelectionL()
	{
	CMsvOperationWait* active = CMsvOperationWait::NewLC();
	CMsvFindOperation* find = NULL;
	CMsvEntrySelection* selection = new(ELeave)CMsvEntrySelection;
	CleanupStack::PushL(selection);

	selection->AppendL(folderId);
	selection->AppendL(entry1Id);
	selection->AppendL(entry2Id);
	selection->AppendL(entry3Id);

	// Test invalid arguments
	TRAPD(error, CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus));
	test(error == KErrArgument);

	selection->Delete(0);
	selection->InsertL(1, folderId);

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();
	
	// Test the progress
    TPckgBuf<TMsvFindOperationProgress> progress;
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrArgument);
	test(progress().iCompleted == 1);
	test(progress().iRemaining == 3);

	selection->Delete(1);
	selection->InsertL(2, folderId);

	// Test results
	test(find->GetFindResult().Count() == 1);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();
	
	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrArgument);
	test(progress().iCompleted == 2);
	test(progress().iRemaining == 2);

	selection->Delete(2);
	selection->InsertL(3, folderId);

	// Test results
	test(find->GetFindResult().Count() == 2);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();
	
	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrArgument);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 1);

	selection->Delete(3);

	// Test results
	test(find->GetFindResult().Count() == 2);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();
	
	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results
	test(find->GetFindResult().Count() == 2);

	test(selection->At(0) == find->GetFindResult().At(0).iId);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(3); // find, selection, active, testUtils->iMsvEntry, testUtils->iMsvSession, ob
	}



LOCAL_C void TestFindDescriptionSelectionL()
	{
	CMsvOperationWait* active = CMsvOperationWait::NewLC();

	CMsvEntrySelection* selection = new(ELeave)CMsvEntrySelection;
	CleanupStack::PushL(selection);

	CMsvFindOperation* find = NULL;

	selection->AppendL(entry1Id);
	selection->AppendL(entry2Id);
	selection->AppendL(entry3Id);

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("short mail"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    TPckgBuf<TMsvFindOperationProgress> progress;
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("something"), *selection, KMsvMessagePartDescription, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("mail"), *selection, KMsvMessagePartDescription | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("mai"), *selection, KMsvMessagePartDescription | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("interesting"), *selection, KMsvMessagePartDescription | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("Mail"), *selection, KMsvMessagePartDescription | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("something"), *selection, KMsvMessagePartDescription | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartDescription);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartDescription);

	CleanupStack::PopAndDestroy(); // find

	CleanupStack::PopAndDestroy(2); // selection, active, 
	}

LOCAL_C void TestFindOriginatorSelectionL()
	{
	CMsvOperationWait* active = CMsvOperationWait::NewLC();

	CMsvEntrySelection* selection = new(ELeave)CMsvEntrySelection;
	CleanupStack::PushL(selection);

	CMsvFindOperation* find = NULL;

	selection->AppendL(entry1Id);
	selection->AppendL(entry2Id);
	selection->AppendL(entry3Id);

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("psion.com"), *selection, KMsvMessagePartOriginator, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    TPckgBuf<TMsvFindOperationProgress> progress;
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartOriginator);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartOriginator);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("PSION.COM"), *selection, KMsvMessagePartOriginator, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartOriginator);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartOriginator);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("psion"), *selection, KMsvMessagePartOriginator | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartOriginator);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("ps"), *selection, KMsvMessagePartOriginator | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("ralph-greenwell@psion.com"), *selection, KMsvMessagePartOriginator | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartOriginator);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartOriginator);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("ralph-greenwell@psio"), *selection, KMsvMessagePartOriginator | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("ralph-greenwell@psion.com"), *selection, KMsvMessagePartOriginator | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartOriginator);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartOriginator);

	CleanupStack::PopAndDestroy(); // find

	CleanupStack::PopAndDestroy(2); // selection, active, 
	}


LOCAL_C void TestFindRecipientSelectionL()
	{
	CMsvOperationWait* active = CMsvOperationWait::NewLC();

	CMsvEntrySelection* selection = new(ELeave)CMsvEntrySelection;
	CleanupStack::PushL(selection);

	selection->AppendL(entry1Id);
	selection->AppendL(entry2Id);
	selection->AppendL(entry3Id);

	CMsvFindOperation* find = NULL;

	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("address"), *selection, KMsvMessagePartRecipient, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    TPckgBuf<TMsvFindOperationProgress> progress;
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartRecipient);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartRecipient);

	CleanupStack::PopAndDestroy(); // find

	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("xxx"), *selection, KMsvMessagePartRecipient, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 1);
	test(find->GetFindResult().At(0).iId == entry3Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartRecipient);

	CleanupStack::PopAndDestroy(); // find

	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("Address"), *selection, KMsvMessagePartRecipient | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("addr"), *selection, KMsvMessagePartRecipient | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("recipient2@address.com"), *selection, KMsvMessagePartRecipient | KMsvFindCaseSensitive | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 2);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartRecipient);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartRecipient);

	CleanupStack::PopAndDestroy(); // find

	CleanupStack::PopAndDestroy(2); // selection, active, testUtils->iMsvEntry, testUtils->iMsvSession, ob
	}


LOCAL_C void TestFindBodySelectionL()
	{
	CMsvOperationWait* active = CMsvOperationWait::NewLC();

	CMsvEntrySelection* selection = new(ELeave)CMsvEntrySelection;
	CleanupStack::PushL(selection);

	CMsvFindOperation* find = NULL;

	selection->AppendL(entry1Id);
	selection->AppendL(entry2Id);
	selection->AppendL(entry3Id);

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("rich"), *selection, KMsvMessagePartBody, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    TPckgBuf<TMsvFindOperationProgress> progress;
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 3);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(2).iId == entry3Id);
	test(find->GetFindResult().At(2).iPartList == KMsvMessagePartBody);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("RICH"), *selection, KMsvMessagePartBody, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 3);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(2).iId == entry3Id);
	test(find->GetFindResult().At(2).iPartList == KMsvMessagePartBody);


	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("rich"), *selection, KMsvMessagePartBody | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("ric"), *selection, KMsvMessagePartBody | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("rich"), *selection, KMsvMessagePartBody | KMsvFindWholeWord, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 3);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(2).iId == entry3Id);
	test(find->GetFindResult().At(2).iPartList == KMsvMessagePartBody);


	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("rich"), *selection, KMsvMessagePartBody | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 0);

	CleanupStack::PopAndDestroy(); // find

	// Test find in selection - match whole word and case sensitive
	active->Start();
	find = CMsvFindOperation::FindInSelectionL(*testUtils->iMsvSession, _L("Rich"), *selection, KMsvMessagePartBody | KMsvFindWholeWord | KMsvFindCaseSensitive, /*TMsvFindResultAny(),*/ active->iStatus);
	CleanupStack::PushL(find);
	CActiveScheduler::Start();

	// Test the progress
    progress.Copy(find->FinalProgress());
	test(progress().iError == KErrNone);
	test(progress().iCompleted == 3);
	test(progress().iRemaining == 0);

	// Test results

	test(find->GetFindResult().Count() == 3);
	test(find->GetFindResult().At(0).iId == entry1Id);
	test(find->GetFindResult().At(0).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(1).iId == entry2Id);
	test(find->GetFindResult().At(1).iPartList == KMsvMessagePartBody);
	test(find->GetFindResult().At(2).iId == entry3Id);
	test(find->GetFindResult().At(2).iPartList == KMsvMessagePartBody);

	CleanupStack::PopAndDestroy(3); // selection, find,active
	}


LOCAL_C void doMainL()
	{
	InitL();

	testUtils->GoClientSideL();
	CreateMessagesL(KUidFindTestPOP3Mtm);
	testUtils->TestStart(1);
	test.Start(_L("POP3 - Finding text in selection"));
	TestFindInSelectionL();
	log->AppendComment(_L8("Test 1 OK"));
	testUtils->TestFinish(1);
	testUtils->TestStart(2);
	test.Next(_L("POP3 - Finding text in description"));
	TestFindDescriptionSelectionL();
	log->AppendComment(_L8("Test 2 OK"));
	testUtils->TestFinish(2);

	testUtils->TestStart(3);
	test.Next(_L("POP3 - Finding text in recipient"));
	TestFindRecipientSelectionL();
	log->AppendComment(_L8("Test 3 OK"));
	testUtils->TestFinish(3);

	testUtils->TestStart(4);
	test.Next(_L("POP3 - Finding text in originator"));
	TestFindOriginatorSelectionL();
	log->AppendComment(_L8("Test 4 OK"));
	testUtils->TestFinish(4);

	testUtils->TestStart(5);
	test.Next(_L("POP3 - Finding text in body"));
	TestFindBodySelectionL();
	log->AppendComment(_L8("Test 5 OK"));
	testUtils->TestFinish(5);

	testUtils->TestStart(6);
	CreateMessagesL(KUidFindTestSMTPMtm);
	test.Next(_L("SMTP - Finding text in selection"));
	TestFindInSelectionL();
	log->AppendComment(_L8("Test 6 OK"));
	testUtils->TestFinish(6);

	testUtils->TestStart(7);
	test.Next(_L("SMTP - Finding text in description"));
	TestFindDescriptionSelectionL();
	log->AppendComment(_L8("Test 7 OK"));
	testUtils->TestFinish(7);

	testUtils->TestStart(8);
	test.Next(_L("SMTP - Finding text in recipient"));
	TestFindRecipientSelectionL();
	log->AppendComment(_L8("Test 8 OK"));
	testUtils->TestFinish(8);

	testUtils->TestStart(9);
	test.Next(_L("SMTP - Finding text in originator"));
	TestFindOriginatorSelectionL();
	log->AppendComment(_L8("Test 9 OK"));
	testUtils->TestFinish(9);

	testUtils->TestStart(10);
	test.Next(_L("SMTP - Finding text in body"));
	TestFindBodySelectionL();
	log->AppendComment(_L8("Test 10 OK"));
	testUtils->TestFinish(10);

	testUtils->TestStart(11);
	CreateMessagesL(KUidFindTestIMAP4Mtm);
	test.Next(_L("IMAP4 - Finding text in selection"));
	TestFindInSelectionL();
	log->AppendComment(_L8("Test 11 OK"));
	testUtils->TestFinish(11);

	testUtils->TestStart(12);
	test.Next(_L("IMAP4 - Finding text in description"));
	TestFindDescriptionSelectionL();
	log->AppendComment(_L8("Test 12 OK"));
	testUtils->TestFinish(12);

	testUtils->TestStart(13);
	test.Next(_L("IMAP4 - Finding text in recipient"));
	TestFindRecipientSelectionL();
	log->AppendComment(_L8("Test 13 OK"));
	testUtils->TestFinish(13);

	testUtils->TestStart(14);
	test.Next(_L("IMAP4 - Finding text in originator"));
	TestFindOriginatorSelectionL();
	log->AppendComment(_L8("Test 14 OK"));
	testUtils->TestFinish(14);

	testUtils->TestStart(15);
	test.Next(_L("IMAP4 - Finding text in body"));
	TestFindBodySelectionL();
	log->AppendComment(_L8("Test 15 OK"));
	testUtils->TestFinish(15);

	log->AppendComment(_L8("TESTS PASSED"));
	testUtils->TestHarnessCompleted();
	Closedown();		
	}
	

LOCAL_C void doFindTestL()
	{
	InitL();

	testUtils->GoClientSideL();
	CreateMessagesL(KUidFindTestPOP3Mtm);
	testUtils->TestStart(1);
	test.Start(_L("POP3 - Finding text in Header"));
	log->AppendComment(_L8("Test 1 OK"));
	testUtils->TestFinish(1);
	
	log->AppendComment(_L8("TESTS PASSED"));
	testUtils->TestHarnessCompleted();
	Closedown();		
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

