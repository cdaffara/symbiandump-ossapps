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
//

#include "caltestlib.h"
#include <calentry.h>
#include <calentryview.h>
#include <calalarm.h>
#include <calattachment.h>
#include <calattachmentmanager.h>
#include <calcategory.h>
#include <calcategorymanager.h>
#include <calsession.h>
#include <calinstance.h>
#include <caliterator.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <e32test.h>
#include <s32file.h>

// Uncomment this line to test manually
// #define TEST_MANUALLY

static RTest test(_L("tcal_removable_media"));
_LIT(KRemovableMediaTestFile, "removable_media");

class CCalRemovableMediaTest : public CBase, public MCalProgressCallBack
	{
public:
	static CCalRemovableMediaTest* NewLC();
 	~CCalRemovableMediaTest();
	
	void DoTestsL();

	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();

private:
	CCalRemovableMediaTest();
	void ConstructL();
	
	void NextStepL(TInt aStep);
	
	void InitialiseFileL();
	void StoreEntriesL(TBool aTestStoreL);
	void UpdateEntriesL();
	void DeleteEntriesByUidL();
	void DeleteEntriesByLocalUidL();
	void DeleteEntriesAsynchronouslyByTimeL();
	void DeleteInstanceL();
	void AddCategoryL(TBool aTestAdd);
	void DeleteCategoryL();
	
	void GetNumEntriesL();
	void CheckUpdateL();
	void CheckDeleteAsynchronouslyByTime();
	void CheckNumEntriesL();
	void CheckNumAttachmentsL();
	void TestAfterMediaRemovedL();

	void CheckNumCategoriesL();
	void CheckDeleteCategory();
	void CheckDeleteInstanceL();
	void AddSecondCategoryL();
	
	void AddAttachmentL(TBool aTesting);
	void UpdateAttachmentL();
	void DeleteAttachmentL();
	void CheckDeleteAttachmentL();
	void ChangeAttachmentDriveL();
	
	TInt AttachmentFileCountL() const;
	
	void SetupTestLibL();
	void ClearTestLib();
	
	void FindEntriesWithTextL(RPointerArray<CCalInstance>& aInstances, const TDesC& aText);	
	void RemoveMedia();
	void ReplaceMedia();
	TBool MediaRemoved() const;

private:
	enum
		{
		ETestStore,
		ECheckStore,
		ETestUpdate,
		ECheckUpdate,
		ETestDeleteByTime,
		ECheckDeleteByTime,
		ETestDeleteByLocalUid,
		ECheckDeleteByLocalUid,
		ETestDeleteByUid,
		ECheckDeleteByUid,
		ETestAddCategory,
		ECheckAddCategory,
		ETestAddSecondCategory,
		ECheckAddSecondCategory,
		ETestDeleteCategory,
		ECheckDeleteCategory,
		ETestDeleteInstance,
		ECheckDeleteInstance,
		ETestAddAttachment,
		ECheckAddAttachment,
		ETestUpdateAttachment,
		ECheckUpdateAttachment,
		ETestDeleteAttachment,
		ECheckDeleteAttachment,
		EChangeAttachmentDrive,
		ETestAfterMediaRemoved,
		EFinishedTests,

		// edit this to change where the test starts
		EFirstStep = ETestStore,
		};
	
	CCalTestLibrary* iTestLib;
	CCalCategoryManager* iCatMan;
	CCalAttachmentManager* iAttMan;
	TBuf<32> iCalendarName;
	TInt iError;
	TInt iNumEntries;
	TInt iNumAttachments;
	TBool iMediaRemoved;
	};

void CCalRemovableMediaTest::Progress(TInt )
	{
	}

void CCalRemovableMediaTest::Completed(TInt aError)
	{
	iError = aError;
	test.Printf(_L("Completed with error %d \n"), aError);
	CActiveScheduler::Stop();
	}

TBool CCalRemovableMediaTest::NotifyProgress()
	{
	return EFalse;
	}

void CCalRemovableMediaTest::RemoveMedia()
	{
#ifdef TEST_MANUALLY
	test.Printf(_L("Remove the media and press any key.\n")); test.Getch();
#else
	iTestLib->FileSession().SetErrorCondition(KErrNotReady, 0);
	User::After(500000);
#endif
	iMediaRemoved = ETrue;
	}

void CCalRemovableMediaTest::ReplaceMedia()
	{
#ifdef TEST_MANUALLY
	test.Printf(_L("Reinsert the media and press any key...\n")); test.Getch();
#else
	User::After(500000);
#endif
	iMediaRemoved = EFalse;
	}

TBool CCalRemovableMediaTest::MediaRemoved() const
	{
	return iMediaRemoved;
	}

void CCalRemovableMediaTest::NextStepL(TInt aStep)
	{
	if (MediaRemoved())
		{
		ReplaceMedia();
		}
	
	TRAPD(err, SetupTestLibL());
	if (err != KErrNone)
		{
		test.Printf(_L("Failed to re-open calendar file, error %d\n"), err);
		test(0);
		}
		
	switch (aStep)
		{
		case ETestStore:
			StoreEntriesL(ETrue);
			break;
		case ECheckStore:
			CheckNumEntriesL();
			break;
		case ETestUpdate:
			UpdateEntriesL();
			break;
		case ECheckUpdate:
			CheckUpdateL();
			CheckNumEntriesL();
			break;
		case ETestDeleteByTime:
			DeleteEntriesAsynchronouslyByTimeL();
			break;
		case ECheckDeleteByTime:
			CheckDeleteAsynchronouslyByTime();
			CheckNumEntriesL();
			break;
		case ETestDeleteByLocalUid:
			DeleteEntriesByLocalUidL();
			break;
		case ECheckDeleteByLocalUid:
			CheckNumEntriesL();
			break;
		case ETestDeleteByUid:
			DeleteEntriesByUidL();
			break;
		case ECheckDeleteByUid:
			CheckNumEntriesL();
			break;
		case ETestAddCategory:
			AddCategoryL(ETrue);
			break;
		case ECheckAddCategory:
			CheckNumCategoriesL();
			break;
		case ETestAddSecondCategory:
			AddSecondCategoryL();
			break;
		case ECheckAddSecondCategory:
			CheckNumCategoriesL();
			break;
		case ETestDeleteCategory:
			DeleteCategoryL();
			break;
		case ECheckDeleteCategory:
			CheckDeleteCategory();
			CheckNumCategoriesL();
			break;
		case ETestDeleteInstance:
			DeleteInstanceL();
			break;
		case ECheckDeleteInstance:
			CheckDeleteInstanceL();
			break;
		case ETestAddAttachment:
			AddAttachmentL(ETrue);
			break;
		case ECheckAddAttachment:
			CheckNumAttachmentsL();
			break;
		case ETestUpdateAttachment:
			UpdateAttachmentL();
			break;
		case ECheckUpdateAttachment:
			CheckNumEntriesL();
			CheckNumAttachmentsL();
			break;
		case ETestDeleteAttachment:
			DeleteAttachmentL();
			break;
		case ECheckDeleteAttachment:
			CheckDeleteAttachmentL();
			break;
		case EChangeAttachmentDrive:
			ChangeAttachmentDriveL();
			break;
		case ETestAfterMediaRemoved:
			TestAfterMediaRemovedL();
			break;
		}
	
	ClearTestLib();
	}

void CCalRemovableMediaTest::InitialiseFileL()
	{
	// ensure there are at least 10 entries in file
	GetNumEntriesL();
	if (iNumEntries < 10)
		{
		StoreEntriesL(EFalse);
		GetNumEntriesL();
		}
	}

void CCalRemovableMediaTest::StoreEntriesL(TBool aTestStoreL)
	{
	if (aTestStoreL)
		{
		test.Next(_L("Test StoreL"));		
		}
	
	const TInt KNumEntries = 10;
	test.Printf(_L("Creating %d non-repeating entries \n"), KNumEntries);
	TTime time;
	time.HomeTime();
	
	GetNumEntriesL();
	test.Printf(_L("Number of entries before add is %d\n"), iNumEntries);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	for (TInt i = 0; i < KNumEntries; ++i)
		{
		time -= TTimeIntervalDays(1);
		TBuf8<64> buf;
		iTestLib->RandomText8(buf);
		CCalEntry::TType type = CCalEntry::EAppt;
		if (i % 2 == 0)
			{
			type = CCalEntry::ETodo;
			}
		HBufC8* bufAlloc = buf.AllocLC();
		CCalEntry* entry = iTestLib->CreateCalEntryL(type, bufAlloc);
		CleanupStack::Pop(); // buf.AllocLC()
		CleanupStack::PushL(entry);
		iTestLib->SetEntryStartAndEndTimeL(entry, time, time);
		entryArray.AppendL(entry);
		CleanupStack::Pop(entry);
		}
	
	if (aTestStoreL)
		{
		RemoveMedia();
		}

	TInt success(0);
	test.Printf(_L("Adding entries..."));	
	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(entryArray, success));
	test.Printf(_L("StoreL gave error %d, num successfully added=%d\n"), err, success);

	if (aTestStoreL)
		{
		test(err == KErrNotReady);
		test(success == 0);
		}
	else
		{
		User::LeaveIfError(err);
		test(success == KNumEntries);
		}
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CCalRemovableMediaTest::FindEntriesWithTextL(RPointerArray<CCalInstance>& aInstances, const TDesC& aText)
	{
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll | CalCommon::EIncludeRptsNextInstanceOnly;
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	
	CCalInstanceView::TCalSearchParams searchParams(aText, CalCommon::EFoldedTextSearch);
	
	iTestLib->SynCGetInstanceViewL().FindInstanceL(aInstances, filter, timeRange, searchParams);
	}

void CCalRemovableMediaTest::UpdateEntriesL()
	{
	test.Next(_L("Test UpdateL\n"));	
	InitialiseFileL();
			
	test.Printf(_L("Updating %d entries \n"), iNumEntries);
	
	RPointerArray<CCalInstance> instArray;
	CleanupResetAndDestroyPushL(instArray);

	FindEntriesWithTextL(instArray, KNullDesC);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	for (TInt i = 0; i < instArray.Count(); ++i)
		{
		CCalEntry* entry = &instArray[i]->Entry();
		entry->SetSummaryL(_L("updated summary"));
		entryArray.AppendL(entry);
		}
		
	RemoveMedia();

	TInt success(0);
	test.Printf(_L("Updating entries..."));
	TRAPD(err, iTestLib->SynCGetEntryViewL().UpdateL(entryArray, success));
	test.Printf(_L("UpdateL gave error %d, num successfully updated=%d\n"), err, success);
	
	test(err == KErrNotReady);
	test(success == 0);

	CleanupStack::PopAndDestroy(&entryArray); // Close()
	CleanupStack::PopAndDestroy(&instArray); // ResetAndDestroy()
	}

void CCalRemovableMediaTest::CheckUpdateL()
	{
	RPointerArray<CCalInstance> instArray;
	CleanupResetAndDestroyPushL(instArray);

	// check that no entries were updated
	test.Printf(_L("Checking no entries were updated..."));
	FindEntriesWithTextL(instArray, _L("updated summary"));
	test.Printf(_L("%d entries were updated\n"), instArray.Count());
	test(instArray.Count() == 0);
	
	CleanupStack::PopAndDestroy(&instArray); // ResetAndDestroy
	}

void CCalRemovableMediaTest::GetNumEntriesL()
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(minTime, localUids);
	iNumEntries = localUids.Count();
	CleanupStack::PopAndDestroy(&localUids); // Close()
	
	iNumAttachments = AttachmentFileCountL();
	}
	
TInt CCalRemovableMediaTest::AttachmentFileCountL() const
	{
	CCalAttachmentIterator* attachments = iAttMan->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	TInt count = attachments->Count();
	delete attachments;
	return count;
	}

void CCalRemovableMediaTest::CheckNumEntriesL()
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	test.Printf(_L("Checking number of entries: expected=%d..."), iNumEntries);
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(minTime, localUids);
	test.Printf(_L("Number of entries found=%d\n"), localUids.Count());
	test(iNumEntries == localUids.Count());
	CleanupStack::PopAndDestroy(&localUids); // Close()
	}
	
void CCalRemovableMediaTest::CheckNumAttachmentsL()
	{
	if (iNumAttachments > 0)
		{
		test.Printf(_L("Checking number of attachments: expected=%d...Number of entries found=%d\n"), iNumAttachments, AttachmentFileCountL());
		test(iNumAttachments == AttachmentFileCountL());
		}
	}

void CCalRemovableMediaTest::DeleteEntriesByLocalUidL()
	{
	test.Next(_L("Test DeleteL by local UID\n"));	
	InitialiseFileL();
	
	test.Printf(_L("Deleting all entries by local uid \n"));

	test.Printf(_L("Number of entries before delete is %d\n"), iNumEntries);
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(minTime, localUids);
	test(localUids.Count() == iNumEntries); // check all local uids found
	
	RemoveMedia();

	TInt success = 0;
	test.Printf(_L("Doing delete..."));
	TRAPD(err, iTestLib->SynCGetEntryViewL().DeleteL(localUids, success));
	test.Printf(_L("DeleteL gave err %d, num successfully deleted=%d\n"), err, success);
	test(err == KErrNotReady);
	test(success == 0);

	CleanupStack::PopAndDestroy(&localUids); // Close()
	}
void CCalRemovableMediaTest::DeleteEntriesByUidL()
	{
	test.Next(_L("Test DeleteL by GUID\n"));
	InitialiseFileL();

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CDesC8Array* uidArray = new (ELeave) CDesC8ArraySeg(8);
	CleanupStack::PushL(uidArray);
	
	CCalIter& iter = iTestLib->GetIterL();
	
	TPtrC8 uid = iter.FirstL();
	while (uid.Length() > 0)
		{
		uidArray->AppendL(uid);
		uid.Set(iter.NextL());
		}
	
	test(iNumEntries == uidArray->Count());
	test.Printf(_L("Found %d entries before delete\n"), iNumEntries);
	
	RemoveMedia();
			
	test.Printf(_L("Deleting entries..."));
	TRAPD(err, iTestLib->SynCGetEntryViewL().DeleteL(*uidArray));
	test(err == KErrNotReady);
	test.Printf(_L("DeleteL gave error %d...\n"), err);

	CleanupStack::PopAndDestroy(uidArray);
	CleanupStack::PopAndDestroy(&entries);
	}

void CCalRemovableMediaTest::DeleteEntriesAsynchronouslyByTimeL()
	{
	test.Next(_L("Test DeleteL by time\n"));	
	InitialiseFileL();
	
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	
	RemoveMedia();
	
	test.Printf(_L("Deleting entries...\n"));
	TRAPD(err, iTestLib->SynCGetEntryViewL().DeleteL(timeRange, filter, *this));
	test.Printf(_L("DeleteL gave error %d...\n"), err);
	test(err == KErrNone);

	CActiveScheduler::Start();
	}

void CCalRemovableMediaTest::CheckDeleteAsynchronouslyByTime()
	{
	test(iError == KErrNotReady);
	}

void CCalRemovableMediaTest::AddCategoryL(TBool aTestAdd)
	{
	if (aTestAdd)
		{
		test.Next(_L("Test AddCategoryL\n"));		
		}
	
	iNumEntries = iCatMan->CategoryCountL();
	
	test.Printf(_L("Adding a category...\n"));
	TBuf<24> categoryName;
	TTime now;
	now.UniversalTime();
	categoryName.AppendNum(now.Int64());
	CCalCategory* category = CCalCategory::NewL(categoryName);
	CleanupStack::PushL(category);
	
	if (aTestAdd)
		{
		RemoveMedia();
		}

	TRAPD(err, iCatMan->AddCategoryL(*category));
	CleanupStack::PopAndDestroy(category);
	if (aTestAdd)
		{
		test.Printf(_L("Adding category gave error %d...\n"), err);
		test(err == KErrNotReady);
		}
	else
		{
		test(err == KErrNone);
		}
	}

void CCalRemovableMediaTest::AddSecondCategoryL()
	{
	test.Next(_L("Test AddSecondCategoryL\n"));	
	AddCategoryL(EFalse);
	
	iNumEntries = iCatMan->CategoryCountL();
	
	test.Printf(_L("Adding a second category...\n"));
	TBuf<24> categoryName;
	TTime now;
	now.UniversalTime();
	categoryName.AppendNum(now.Int64()+1);
	CCalCategory* category = CCalCategory::NewL(categoryName);
	CleanupStack::PushL(category);
	
	RemoveMedia();
	
	TRAPD(err, iCatMan->AddCategoryL(*category));
	CleanupStack::PopAndDestroy(category);
	test.Printf(_L("Adding second category gave error %d...\n"), err);
	test(err == KErrNotReady);
	}

void CCalRemovableMediaTest::CheckNumCategoriesL()
	{
	TInt newCategoryCount = iCatMan->CategoryCountL();
	test.Printf(_L("Number of categories expected=%d, number found=%d\n"), iNumEntries, newCategoryCount);
	test(newCategoryCount == iNumEntries);
	}

void CCalRemovableMediaTest::DeleteCategoryL()
	{
	test.Next(_L("Test DeleteCategoryL\n"));	
	AddCategoryL(EFalse);
			
	iNumEntries = iCatMan->CategoryCountL();
	test(iCatMan->CategoryCountL() > 12); // 12 is the number of default categories, this checks at least one has been added
	
	test.Printf(_L("Deleting category...\n"));
	CCalCategory* cat = iCatMan->CategoryL(iNumEntries-1);
	CleanupStack::PushL(cat);
		
	RemoveMedia();
	
	TRAPD(err, iCatMan->DeleteCategoryL(*cat, *this));
	test.Printf(_L("DeleteCategoryL gave error %d\n"), err);

	test(err == KErrNone);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(cat);
	}

void CCalRemovableMediaTest::CheckDeleteCategory()
	{
	test(iError == KErrNotReady);
	}

void CCalRemovableMediaTest::AddAttachmentL(TBool aTesting)
	{
	if (aTesting)
		{
		test.Next(_L("Test add attachment\n"));		
		}
	GetNumEntriesL();
	
	_LIT8(KBinaryData, "001010110100001010010001010110100001010010001010110100001010010001010110100001010010...is this binary?");
	_LIT8(KGuid, "AddAttachmentL_guid");
	
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	HBufC8* binaryData = KBinaryData().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(binaryData);
	CleanupStack::Pop(binaryData);
	CleanupStack::PushL(attachment);
#ifdef TEST_MANUALLY
	_LIT(KDrive, "d:");
	TDriveName drive = KDrive();
	attachment->FileAttachment()->SetDriveL(drive);
#endif
	entry->AddAttachmentL(*attachment);
	CleanupStack::Pop(attachment);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	TInt success = 0;
	entryArray.AppendL(entry);
	
	if (aTesting)
		{
		RemoveMedia();
		}

	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(entryArray, success));

	if (aTesting)
		{
		test.Printf(_L("AddAttachmentL gave error %d\n"), err);
		test(success == 0);
		test(err == KErrNotReady);
		}
	else
		{
		test(success == entryArray.Count());
		test(err == KErrNone);
		}
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.Close()
	CleanupStack::PopAndDestroy(entry);
	}

void CCalRemovableMediaTest::UpdateAttachmentL()
	{
	test.Next(_L("Test update attachment\n"));	
	AddAttachmentL(EFalse);
	
	GetNumEntriesL();
	
	CCalAttachmentIterator* attachments = iAttMan->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);

	test(attachments->HasMore());
	CCalAttachment* currentAttachment = attachments->NextL();
	CleanupStack::PushL(currentAttachment);

	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	iAttMan->EntriesReferencingFileAttachmentL(localUids, *currentAttachment);
	test(localUids.Count() == 1);
	
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(localUids[0]);
	test(entry != NULL);
	
	CleanupStack::PopAndDestroy(&localUids); // localUids.Close()
	CleanupStack::PushL(entry);
	
	entry->DeleteAttachmentL(*currentAttachment);

	_LIT8(KBinaryData, "zxcvbnmzxcvbnmzxcvbnzmxcvbnmzxcvbznxmcvbnmzxcvbzxcvbnmzxvcbmn");
	HBufC8* data = KBinaryData().AllocLC();
	CCalAttachment* newAttachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	CleanupStack::PushL(newAttachment);
	entry->AddAttachmentL(*newAttachment);
	CleanupStack::Pop(newAttachment);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);

	RemoveMedia();

	entryArray.AppendL(entry);
	TInt success = 0;
	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(entryArray, success));
	test.Printf(_L("UpdateAttachmentL gave error %d\n"), err);
	test(success == 0);
	test(err == KErrNotReady);
	
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.Close()

	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(currentAttachment);
	CleanupStack::PopAndDestroy(attachments);
	}

void CCalRemovableMediaTest::DeleteAttachmentL()
	{
	test.Next(_L("Test delete attachment\n"));	
	// ensure at least one entry exists
	iNumEntries = AttachmentFileCountL();
	if (iNumEntries == 0)
		{
		AddAttachmentL(EFalse);
		iNumEntries = AttachmentFileCountL();
		test(iNumEntries != 0);
		}
	
	CCalAttachmentIterator* attachments = iAttMan->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);

	test(attachments->HasMore());
	CCalAttachment* currentAttachment = attachments->NextL();
	CleanupStack::PushL(currentAttachment);

	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	iAttMan->EntriesReferencingFileAttachmentL(localUids, *currentAttachment);
	test(localUids.Count() == 1);
	
	RemoveMedia();
	TInt success = 0;
	TRAPD(err, iTestLib->SynCGetEntryViewL().DeleteL(localUids, success));
	test.Printf(_L("DeleteAttachmentL gave error %d\n"), err);
	test(err == KErrNotReady);
	test(success == 0);
	
	CleanupStack::PopAndDestroy(&localUids); // localUids.Close()
	
	CleanupStack::PopAndDestroy(currentAttachment);
	CleanupStack::PopAndDestroy(attachments);
	}

void CCalRemovableMediaTest::CheckDeleteAttachmentL()
	{
	test.Printf(_L("Checking number of attachments: expected=%d...Number of entries found=%d\n"), iNumAttachments, AttachmentFileCountL());
	test(iNumAttachments == AttachmentFileCountL());
	}

void CCalRemovableMediaTest::ChangeAttachmentDriveL()
	{
#ifdef TEST_MANUALLY
	_LIT(KDriveC, "C:");
	_LIT(KDriveD, "D:");
	_LIT8(KGuidCopy, "guid_copy");
	
	test.Next(_L("Create an attachment, copy it then set the drive to be different"));	
	// add an attachment 
	AddAttachmentL(EFalse);
	
	// fetch the entry containing this attachment
	CCalAttachmentIterator* attachments = iAttMan->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);

	test(attachments->HasMore());
	CCalAttachment* currentAttachment = attachments->NextL();
	CleanupStack::PushL(currentAttachment);

	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	iAttMan->EntriesReferencingFileAttachmentL(localUids, *currentAttachment);
	test(localUids.Count() == 1);
	
	TInt luid1 = localUids[0];
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid1);
	test(entry != NULL);
	CleanupStack::PopAndDestroy(&localUids);
	CleanupStack::PopAndDestroy(currentAttachment);
	CleanupStack::PopAndDestroy(attachments);
	
	CleanupStack::PushL(entry);
	
	// create a copy of the entry
	HBufC8* guid = KGuidCopy().AllocLC();
	CCalEntry* copyOfEntry = CCalEntry::NewL(entry->EntryTypeL(), guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(copyOfEntry);
	copyOfEntry->CopyFromL(*entry, CCalEntry::EDontCopyId);

	// edit the copy to place the attachment on a different drive
	test(copyOfEntry->AttachmentCountL() == 1);
	CCalAttachment* attach = copyOfEntry->AttachmentL(0);
	test(attach != NULL);
	attach->FileAttachment()->SetDriveL(KDriveC());

	iTestLib->StoreEntryL(*copyOfEntry);
	TCalLocalUid luid2 = copyOfEntry->LocalUidL();
	CleanupStack::PopAndDestroy(copyOfEntry);
	CleanupStack::PopAndDestroy(entry);

	// fetch the copied entry and check its attachment
	CCalEntry* fetchedCopiedEntry = iTestLib->SynCGetEntryViewL().FetchL(luid2);
	test(fetchedCopiedEntry != NULL);
	CleanupStack::PushL(fetchedCopiedEntry);
	
	test(fetchedCopiedEntry->AttachmentCountL() == 1);
	attach = fetchedCopiedEntry->AttachmentL(0);
	test(attach != NULL);
	attach->FileAttachment()->LoadBinaryDataL();
	test(attach->Value().Length() > 10);
	TDriveName drive = attach->FileAttachment()->Drive();
	drive.UpperCase();
	test(drive == KDriveC());
	CleanupStack::PopAndDestroy(fetchedCopiedEntry);
	
	
	
	// fetch the original entry
	entry = iTestLib->SynCGetEntryViewL().FetchL(luid1);
	CleanupStack::PushL(entry);
	
	// create a copy of the entry
	guid = KGuidCopy().AllocLC();
	copyOfEntry = CCalEntry::NewL(entry->EntryTypeL(), guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(copyOfEntry);
	copyOfEntry->CopyFromL(*entry, CCalEntry::EDontCopyId);

	// edit the copy to place the attachment on the same drive (but change the flag)
	test(copyOfEntry->AttachmentCountL() == 1);
	attach = copyOfEntry->AttachmentL(0);
	test(attach != NULL);
	attach->FileAttachment()->SetDriveL(KDriveC());
	attach->FileAttachment()->SetDriveL(KDriveD());

	iTestLib->StoreEntryL(*copyOfEntry);
	TCalLocalUid luid3 = copyOfEntry->LocalUidL();
	CleanupStack::PopAndDestroy(copyOfEntry);
	CleanupStack::PopAndDestroy(entry);

	// fetch the copied entry and check its attachment
	fetchedCopiedEntry = iTestLib->SynCGetEntryViewL().FetchL(luid3);
	test(fetchedCopiedEntry != NULL);
	CleanupStack::PushL(fetchedCopiedEntry);
	
	test(fetchedCopiedEntry->AttachmentCountL() == 1);
	attach = fetchedCopiedEntry->AttachmentL(0);
	test(attach != NULL);
	attach->FileAttachment()->LoadBinaryDataL();
	test(attach->Value().Length() > 10);
	drive = attach->FileAttachment()->Drive();
	drive.UpperCase();
	test(drive == KDriveD());
	CleanupStack::PopAndDestroy(fetchedCopiedEntry);
#endif
	}
void CCalRemovableMediaTest::DeleteInstanceL()
	{
	test.Next(_L("Test Delete Instance\n"));	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	iTestLib->FetchAllInstancesL(instances);
	if (instances.Count() == 0)
		{
		InitialiseFileL();
		iTestLib->FetchAllInstancesL(instances);
		test(instances.Count() > 0);
		}
	
	iNumEntries = instances[0]->Entry().LocalUidL();
	
	RemoveMedia();
	test.Printf(_L("Deleting instance...\n"));
	TRAPD(err, iTestLib->SynCGetInstanceViewL().DeleteL(instances[0], CalCommon::EThisAndAll));
	test.Printf(_L("Deleting instance gave error %d...\n"), err);
	test(err == KErrNotReady);
	
	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy()
	}

void CCalRemovableMediaTest::CheckDeleteInstanceL()
	{
	test.Printf(_L("Checking instance not deleted...\n"));
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(iNumEntries);
	test(entry != NULL);
	delete entry;
	}

void CCalRemovableMediaTest::TestAfterMediaRemovedL()
	{
	test.Printf(_L("Testing read-only operations after media removed\n"));
	
	StoreEntriesL(EFalse); // add some entries to begin with
	
	StoreEntriesL(ETrue); // remove media
	
	// test find instances after media removed
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	TRAPD(err, FindEntriesWithTextL(instances, KNullDesC()));
	test(err == KErrNotReady);
	CleanupStack::PopAndDestroy(&instances);
	
	// test iterator
	TRAP(err, iTestLib->GetIterL().FirstL());
	test(err == KErrNotReady);

	// test next instance
	TCalTime startTime;
	startTime.SetTimeLocalL(TCalTime::MinTime());
	TRAP(err, iTestLib->SynCGetInstanceViewL().NextInstanceL(CalCommon::EIncludeAll, startTime));
	test(err == KErrNotReady);
	}

CCalRemovableMediaTest* CCalRemovableMediaTest::NewLC()
	{
	CCalRemovableMediaTest* app = new (ELeave) CCalRemovableMediaTest();
	CleanupStack::PushL(app);
	app->ConstructL();
	return app;
	}

void CCalRemovableMediaTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	
#ifdef TEST_MANUALLY
#ifdef __WINS__
	iCalendarName.Copy(_L("d:"));
	iCalendarName.Append(KRemovableMediaTestFile);
#else
	// find the first drive letter after drive c which exists
	TDriveList list;
	iTestLib->FileSession().DriveList(list);

	TChar driveLetter;
	TInt i = EDriveD;
	while (i < EDriveZ)
		{
		TVolumeInfo volInfo;
		TInt err = iTestLib->FileSession().Volume(volInfo, i);
		if (err == KErrNone)
			{
			break;
			}
		++i;
		}
	
	User::LeaveIfError(iTestLib->FileSession().DriveToChar(i, driveLetter));
	iCalendarName.Zero();
	iCalendarName.Append(driveLetter);
	test.Printf(_L("Using drive %S as external drive\n"), &iCalendarName);
	test(iCalendarName.Length() == 1);
	iCalendarName.Append(_L(":"));
	iCalendarName.Append(KRemovableMediaTestFile);
#endif
#else
	iCalendarName.Copy(_L("c:"));
	iCalendarName.Append(KRemovableMediaTestFile);
#endif

	TRAP_IGNORE(iTestLib->ReplaceFileL(iCalendarName));
	}

void CCalRemovableMediaTest::SetupTestLibL()
	{
#ifndef TEST_MANUALLY
	iTestLib->FileSession().SetErrorCondition(0);
	User::After(500000);
#endif
	test.Printf(_L("Re-opening calendar file\n"));
	iTestLib->OpenFileL(iCalendarName);
	if (!iCatMan)
		{
		iCatMan = CCalCategoryManager::NewL(iTestLib->GetSession());
		}
	if (!iAttMan)
		{
		iAttMan = CCalAttachmentManager::NewL(iTestLib->GetSession(), *iTestLib);
		CActiveScheduler::Start();
		}
	}

void CCalRemovableMediaTest::ClearTestLib()
	{
	delete iCatMan;
	iCatMan = NULL;
	delete iAttMan;
	iAttMan = NULL;
	}

CCalRemovableMediaTest::CCalRemovableMediaTest()
	{
	}
	
CCalRemovableMediaTest::~CCalRemovableMediaTest()
	{
	ClearTestLib();
	delete iTestLib;
	}
	
void CCalRemovableMediaTest::DoTestsL()
	{
	TInt step = EFirstStep;
	while (step != EFinishedTests)
		{
// this test suite can work only under debug mode because the file server failure 
// simulation feature
#ifdef _DEBUG
		NextStepL(step);
#endif		
		++step;
		}
	// clean up files
	iTestLib->FileSession().SetErrorCondition(0);
	User::After(500000);
	iTestLib->DeleteFileL(KRemovableMediaTestFile, 1);
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
#ifdef TEST_MANUALLY
	test.Printf(_L("Manual test\n"));
#ifdef __WINS__
	test.Printf(_L("\n"));
	test.Printf(_L("Ensure the epoc.ini file is set up with the D: drive mapped to an external drive:\n"));
	test.Printf(_L("  _epoc_drive_d f:\n"));
#endif
	test.Printf(_L("\n"));
	test.Printf(_L("Put an MMC card in the external drive and remove/replace it when prompted.\n"));
	test.Printf(_L("Press any key.\n"));
	test.Getch();
#endif


	CCalRemovableMediaTest* testManager = CCalRemovableMediaTest::NewLC();
	

	TPerformanceTimer timer(test);
	timer.Start();
	

	// Run the test suite

	testManager->DoTestsL();


	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}
	

/**@SYMTestCaseID     PIM-TCAL-REMOVABLE-MEDIA-0001*/TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-REMOVABLE-MEDIA-0001 Calendar Interim API Removable Media test suite"));	
	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }

