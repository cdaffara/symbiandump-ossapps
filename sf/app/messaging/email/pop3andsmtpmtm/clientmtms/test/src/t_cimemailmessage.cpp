// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// To build: bldmake bldfiles
// abld test build wins udeb
// To run from windows command prompt:
// cd epoc32\release\wins\udeb 
// T_CIMEMAILMESSAGE -dtextshell --
// 
//
 

#include <e32base.h>
#include <e32std.h>
#include <e32cons.h>
#include <e32test.h>
#include <s32file.h>

// Message Server headers for linking to msgs.dll. 
#include <msvapi.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvstd.h>

// Class under test.
#include <miutmsg.h>

#include <mmsvattachmentmanager.h>

#include "emailtestutils.h"

typedef enum
	{
	EGetBodyTextL,
	EFindFirstHTMLPageL,
	EGetMessageDigestEntriesL,
	EGetAttachmentListsL,
	EDeleteAttachmentLId1,
	EDeleteAttachmentLId2,
	EFindUniversalResourceIdentifierL,
	EAddMessageAsAttachmentLId1,
	EAddMessageAsAttachmentLId2,
	EAddRelatedPartL,
	EStoreBodyTextL,
	EGetCharacterSetL,
	EDeleteAttachedMessageL
	} TMethodType;

// Forwards
class CMyScheduler;
class CObserver;


// Test harness constants and global variables...
_LIT(KTitle, "T_CIMEMAILMESSAGE");
_LIT(KFolderName, "DeleteMe");
CTrapCleanup* pcleanup = NULL;
CMyScheduler* pscheduler = NULL;
RTest test(KTitle);
LOCAL_D CEmailTestUtils* testUtils;
RFs gFs;
CMsvSession* pSession = NULL;
CObserver* pObserver = NULL;
CMsvEntry* pContext = NULL;
CTestActive* pTestActive = NULL;

// Global email message entry identifiers.
TMsvId gEmailMessageID;
TMsvId gEmailMessageID2;
TMethodType gMethodType;


// Test utility classes
class CMyScheduler : public CActiveScheduler
	{
private:
	void Error(TInt aError) const;
	};
void CMyScheduler::Error(TInt /*aError*/) const {}


class CObserver : public MMsvSessionObserver
	{
	public:
		void HandleSessionEvent(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};


void CObserver::HandleSessionEvent(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}
	
	
void CObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}

// Test Case Prototypes
LOCAL_C TBool TestAttachmentFlagL();
LOCAL_C void TestGetBodyTextL(CImEmailMessage* aEmailMessage, TRequestStatus& aStatus);
LOCAL_C void TestStoreBodyTextL(CImEmailMessage* aEmailMessage, TRequestStatus& aStatus);
LOCAL_C TBool TestPanicsL();

// Test Harness Prototypes
LOCAL_C void DoTestThreadWorkL();
static TInt DoTestThreadStart(TAny*);
LOCAL_C void InitL();
LOCAL_C void UninitL();
LOCAL_C void doTests();
LOCAL_C void doMainL();
GLDEF_C TInt E32Main();

_LIT(KTestThreadName, "CImEmailMessage Test Thread");

LOCAL_C void TestGetBodyTextL(CImEmailMessage* aEmailMessage, TRequestStatus& aStatus)
	{
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the store body text code.");
	bodyText->InsertL(0, bodyContents); 
	aEmailMessage->GetBodyTextL(
		aStatus,
		gEmailMessageID2, 
		CImEmailMessage::EThisMessageOnly,
		*bodyText, 
		*paraFormatLayer, 
		*charFormatLayer);

	CleanupStack::PopAndDestroy(3, paraFormatLayer);
	}

LOCAL_C void TestStoreBodyTextL(CImEmailMessage* aEmailMessage, TRequestStatus& aStatus)
	{
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the store body text code.");
	bodyText->InsertL(0, bodyContents); 
	aEmailMessage->StoreBodyTextL(gEmailMessageID2, *bodyText, aStatus);

	CleanupStack::PopAndDestroy(3, paraFormatLayer);
	}

LOCAL_C void DoTestThreadWorkL()
    {
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(scheduler);
    CleanupStack::PushL(scheduler);

	// Connect to the Message Server...
    CObserver* ob = new (ELeave) CObserver;
    CleanupStack::PushL(ob);

    CMsvSession* session = CMsvSession::OpenSyncL(*ob);
    CleanupStack::PushL(session);
	
	CMsvEntry* cEntry = CMsvEntry::NewL(*session, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(cEntry);
	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*cEntry);

	switch(gMethodType)
		{
		case EGetBodyTextL:
			TestGetBodyTextL(emailMessage, pTestActive->iStatus);
			break;

		case EFindFirstHTMLPageL:
			emailMessage->FindFirstHTMLPageL(gEmailMessageID2, pTestActive->iStatus);
			break;

		case EGetMessageDigestEntriesL:
			emailMessage->GetMessageDigestEntriesL(pTestActive->iStatus, gEmailMessageID2);
			break;

		case EGetAttachmentListsL:
			emailMessage->GetAttachmentsListL(
				gEmailMessageID2,
				CImEmailMessage::EAllAttachments,
				CImEmailMessage::EThisMessageOnly);
			break;

		case EDeleteAttachmentLId1:
			{
			TMsvAttachmentId attachmentId = gEmailMessageID;
			emailMessage->AttachmentManager().RemoveAttachmentL(attachmentId,pTestActive->iStatus);			
			}
			break;

		case EDeleteAttachmentLId2:
			{
			TMsvAttachmentId attachmentId = gEmailMessageID;
			emailMessage->AttachmentManager().RemoveAttachmentL(attachmentId,pTestActive->iStatus);			
			}
			break;

		case EFindUniversalResourceIdentifierL:
			emailMessage->FindUniversalResourceIdentifierL(
				gEmailMessageID2, 
				_L("a description"), 
				_L("http://someuri.com"), 
				pTestActive->iStatus);
			break;

		case EAddMessageAsAttachmentLId1:
			{
			CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
			CleanupStack::PushL(attachmentInfo);
			emailMessage->AttachmentManager().AddEntryAsAttachmentL(gEmailMessageID,attachmentInfo,pTestActive->iStatus);
			CleanupStack::Pop(attachmentInfo);
			}
			break;

		case EAddMessageAsAttachmentLId2:
			{
			CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
			CleanupStack::PushL(attachmentInfo);
			emailMessage->AttachmentManager().AddEntryAsAttachmentL(gEmailMessageID2,attachmentInfo,pTestActive->iStatus);
			CleanupStack::Pop(attachmentInfo);
			}
			break;

		case EAddRelatedPartL:
			emailMessage->AddRelatedPartL(gEmailMessageID2, _L("some text"), pTestActive->iStatus, gEmailMessageID, _L8("some text"));
			break;

		case EStoreBodyTextL:
			TestStoreBodyTextL(emailMessage, pTestActive->iStatus);
			break;

		case EGetCharacterSetL:
			{
			TUint charSetId = 0;
			TBool override  = EFalse;
			emailMessage->GetCharacterSetL(gEmailMessageID2, charSetId, override);
			break;
			}
		case EDeleteAttachedMessageL:
			{
			TMsvAttachmentId attachmentId = gEmailMessageID;
			emailMessage->AttachmentManager().RemoveAttachmentL(attachmentId,pTestActive->iStatus);
			}
			break;
		}

    CleanupStack::PopAndDestroy(5, scheduler);
    }

static TInt DoTestThreadStart(TAny*)
    {
    // Entry function for the child thread which is created then killed.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    test(cleanup != NULL);
    TRAPD(ret, DoTestThreadWorkL());
    delete cleanup;
    return 0;
    }

LOCAL_C TBool TestAttachmentFlagL()
	{
	TBool result = EFalse;
	TMsvId emailMessageID;
	TMsvId attachMessageID;

	// Create an email message entry.
	CImEmailOperation* emailOp = CImEmailOperation::CreateNewL(pTestActive->iStatus, *pSession, KMsvGlobalInBoxIndexEntryId, KMsvMessagePartBody, 0, KUidMsgTypeSMTP); 
	CleanupStack::PushL(emailOp);
	pTestActive->StartL(); // Active object which stops the active scheduler
	CActiveScheduler::Start();
	// Get the new message id.
	TPckg<TMsvId> param(emailMessageID);
	param.Copy(emailOp->FinalProgress());
	CleanupStack::PopAndDestroy(emailOp);

	emailOp = CImEmailOperation::CreateNewL(pTestActive->iStatus, *pSession, KMsvGlobalInBoxIndexEntryId, KMsvMessagePartBody, 0, KUidMsgTypeSMTP); 
	CleanupStack::PushL(emailOp);
	pTestActive->StartL(); // Active object which stops the active scheduler
	CActiveScheduler::Start();
	TPckg<TMsvId> param2(attachMessageID);
	param2.Copy(emailOp->FinalProgress());
	CleanupStack::PopAndDestroy(emailOp);

	CImEmailMessage* obj = CImEmailMessage::NewLC(*pContext);
	
	pContext->SetEntryL(emailMessageID);
	TMsvEntry entry = pContext->Entry();
	result = !(entry.Attachment());
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);	
	obj->AttachmentManager().AddEntryAsAttachmentL(attachMessageID,attachmentInfo,pTestActive->iStatus);
	CleanupStack::Pop(attachmentInfo);
	pTestActive->StartL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(obj);
	obj = NULL;

	// Want to test that the entry's attachment flag is true;
	pContext->SetEntryL(emailMessageID);
	entry = pContext->Entry();
	result &= (entry.Attachment() != EFalse);
	
	pContext->DeleteL(attachMessageID);
	pContext->DeleteL(emailMessageID);

	return result;
	}

LOCAL_C TBool TestPanicsL()
	{
	const TInt KMinTestHeapSize = 0x10000;
    const TInt KMaxTestHeapSize = 0x100000;
    
	RThread testThread;
	TRequestStatus requestStatus;

	TBool result = ETrue;
	TMsvEntry entry;

	// Make sure paniced threads don't cause us trouble.
	User::SetJustInTime(EFalse);

	//
	//--------------------------------------------------------------------------
	// Create email message entries.
	//--------------------------------------------------------------------------
	//
	CImEmailOperation* emailOp = CImEmailOperation::CreateNewL(pTestActive->iStatus, *pSession, KMsvGlobalInBoxIndexEntryId, KMsvMessagePartBody, 0, KUidMsgTypeSMTP); 
	CleanupStack::PushL(emailOp);
	pTestActive->StartL(); // Active object which stops the active scheduler
	CActiveScheduler::Start();
	// Get the new message id.
	TPckg<TMsvId> param(gEmailMessageID);
	param.Copy(emailOp->FinalProgress());
	CleanupStack::PopAndDestroy(emailOp);

	emailOp = CImEmailOperation::CreateNewL(pTestActive->iStatus, *pSession, KMsvGlobalInBoxIndexEntryId, KMsvMessagePartBody, 0, KUidMsgTypeSMTP); 
	CleanupStack::PushL(emailOp);
	pTestActive->StartL(); // Active object which stops the active scheduler
	CActiveScheduler::Start();
	TPckg<TMsvId> param2(gEmailMessageID2);
	param2.Copy(emailOp->FinalProgress());
	CleanupStack::PopAndDestroy(emailOp);

	CImEmailMessage* emailMessage = CImEmailMessage::NewLC(*pContext);
	
	pContext->SetEntryL(gEmailMessageID2);
	entry = pContext->Entry();
	// Set to something inappropriate
	entry.iType = KUidMsvAttachmentEntry;
	pContext->ChangeL(entry);
	
	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::GetBodyTextL -------------------------------------
	//--------------------------------------------------------------------------
	//
	gMethodType = EGetBodyTextL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();
	
	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::FindFirstHTMLPageL -------------------------------
	//--------------------------------------------------------------------------
	//
	gMethodType = EFindFirstHTMLPageL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	
	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::GetMessageDigestEntriesL -------------------------
	//--------------------------------------------------------------------------
	//
	gMethodType = EGetMessageDigestEntriesL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	
	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::GetAttachmentListsL ------------------------------
	//--------------------------------------------------------------------------
	//
	gMethodType = EGetAttachmentListsL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	
	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::DeleteAttachmentL --------------------------------
	//--------------------------------------------------------------------------
	//

	gMethodType = EDeleteAttachmentLId1;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	pContext->SetEntryL(gEmailMessageID2);
	entry = pContext->Entry();
	entry.iType = KUidMsvMessageEntry;
	pContext->ChangeL(entry);

	// The first message ID points to an entry of type KUidMsvMessageEntry.
	// The second message ID will cause a leave because it isn't a KUidMsvAttachmentEntry
	gMethodType = EDeleteAttachmentLId2;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL,
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	pContext->SetEntryL(gEmailMessageID2);
	entry = pContext->Entry();
	entry.iType = KUidMsvAttachmentEntry;
	pContext->ChangeL(entry);

	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::FindUniversalResourceIdentifierL -----------------
	//--------------------------------------------------------------------------
	//

	gMethodType = EFindUniversalResourceIdentifierL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();


	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::AddMessageAsAttachmentL --------------------------
	//--------------------------------------------------------------------------
	//
	
	gMethodType = EAddMessageAsAttachmentLId1;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize,
		KMaxTestHeapSize,
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	// The first message ID points to an entry of type KUidMsvMessageEntry.
	// The second message ID will cause a leave because it isn't a KUidMsvMessageEntry
	gMethodType = EAddMessageAsAttachmentLId2;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::AddRelatedPartL ----------------------------------
	//--------------------------------------------------------------------------
	//
	pContext->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	TMsvId folderId = 0;
	TMsvEmailEntry folderEntry;
	folderEntry.iType = KUidMsvFolderEntry;
	folderEntry.iMtm = KUidMsvLocalServiceMtm;
	folderEntry.iServiceId = KMsvLocalServiceIndexEntryId;
	folderEntry.iDetails.Set(KFolderName);
	// Set the relatedPartID's parent folder type to something acceptable
	
	TMsvLocalOperationProgress details;
	CMsvOperation* opert = pContext->CreateL(folderEntry, pTestActive->iStatus);
	CleanupStack::PushL(opert);
	pTestActive->StartL(); // Active object which stops the active scheduler
	CActiveScheduler::Start();
	TPckg<TMsvLocalOperationProgress> folderParam(details);
	folderParam.Copy(opert->FinalProgress());
	folderId = details.iId;
	pContext->SetEntryL(folderId);
	CleanupStack::PopAndDestroy(opert);

	// Set the relatedPartID's parent folder to the one just created
	opert = NULL;
	pContext->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	pTestActive->StartL(); // Active object which stops the active scheduler
	opert = pContext->MoveL(gEmailMessageID2, folderId, pTestActive->iStatus);
	CleanupStack::PushL(opert);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(opert);

	// emailMessageID2 is not of KUidMsvMessageEntry.
	gMethodType = EAddRelatedPartL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::StoreBodyTextL -----------------
	//--------------------------------------------------------------------------
	//

	gMethodType = EStoreBodyTextL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::GetCharacterSetL -----------------
	//--------------------------------------------------------------------------
	//

	gMethodType = EGetCharacterSetL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	//
	//--------------------------------------------------------------------------
	//- Test CImEmailMessage::DeleteAttachedMesageL -----------------
	//--------------------------------------------------------------------------
	//

	gMethodType = EDeleteAttachedMessageL;
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, 
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);
	result &= 
		((testThread.ExitType() == EExitPanic) && 
		(testThread.ExitReason() == EEntryIsNotMessage));
    testThread.Close();

	User::SetJustInTime(ETrue);

	CleanupStack::PopAndDestroy(emailMessage);
	emailMessage = NULL;
	pContext->SetEntryL(gEmailMessageID2);
	entry = pContext->Entry();
	entry.iType = KUidMsvMessageEntry;
	pContext->ChangeL(entry);

	pContext->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	pContext->DeleteL(gEmailMessageID);
	pContext->DeleteL(gEmailMessageID2);
	pContext->DeleteL(folderId);

	return result;
	}


LOCAL_C void InitL()
	{
	// Connect to the file system...
	gFs.Connect();

	// Connect to the Message Server...
	pObserver = new (ELeave) CObserver();
	CleanupStack::PushL(pObserver);
	pSession = CMsvSession::OpenSyncL(*pObserver);
	CleanupStack::PushL(pSession);
	
	pContext = CMsvEntry::NewL(*pSession, KMsvGlobalInBoxIndexEntryId, TMsvSelectionOrdering());
	CleanupStack::PushL(pContext);
	
	pTestActive = new (ELeave) CTestActive();
	CleanupStack::PushL(pTestActive);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	}


LOCAL_C void UninitL()
	{
	CleanupStack::PopAndDestroy(testUtils);
	CleanupStack::PopAndDestroy(pTestActive);
	CleanupStack::PopAndDestroy(pContext);
	CleanupStack::PopAndDestroy(pSession);
	CleanupStack::PopAndDestroy(pObserver);
	gFs.Close();
	}


LOCAL_C void doTests()
	{
	testUtils->TestStart(1);

	test.Start(_L("Testing CImEmailMessage attachment flag"));
	test(TestAttachmentFlagL());
	test.End();
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	test.Start(_L("Testing CImEmailMessage panics"));
	test(TestPanicsL());
	test.End();
	testUtils->TestFinish(2);
	testUtils->TestHarnessCompleted();
	}


LOCAL_C void doMainL()
	{
	InitL();
	doTests();
	UninitL();
	}


GLDEF_C TInt E32Main()
	{
	pcleanup = CTrapCleanup::New();
	pscheduler = new (ELeave) CMyScheduler();
	CActiveScheduler::Install(pscheduler);
	TRAPD(ret,doMainL());		
	delete pcleanup;
	delete pscheduler;
	return(KErrNone);
	}
