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

#include "caltestlib.h"

#include <e32std.h>
#include <e32test.h>
#include <calattachment.h>
#include <calattachmentmanager.h>
#include <calentry.h>
#include <calentryview.h>
#include <calsession.h>

RTest test(_L("tcal_attach"));

_LIT(KCalendarFile, "tcal_attach");

_LIT(KExpectedFileName1, "tcal_attach.jpg");
_LIT(KExpectedFileName2, "tcal_attach0.jpg");
_LIT(KFileLocation1, "c:\\calendar_attachments\\tcal_attach.jpg");
_LIT(KFileLocation2, "c:\\calendar_attachments\\tcal_attach2.jpg");
_LIT(KFileLocation3, "c:\\calendar_attachments\\tcal_attach3.txt");
_LIT(KFileLocation4, "c:\\calendar_attachments\\tcal_attach4.doc");
_LIT(KFileLocation5, "c:\\calendar_attachments\\tcal_attach5.bmp");
_LIT(KFileLocation6, "c:\\calendar_attachments\\tcal_attach6.gif");
_LIT(KFileLocation7, "c:\\calendar_attachments\\tcal_attach7.xls");
_LIT(KFileLocation8, "c:\\calendar_attachments\\tcal_attach_huge.txt");
_LIT(KOriginalDrive, "z:");
_LIT(KCalendarAttachmentsDir, "c:\\private\\10003a5b\\tcal_attach_a");
	
class CTestApp : public CBase
	{
public:
	static CTestApp* NewLC();
	~CTestApp();
	
	void RunTestsL();
	void RunNonShareProtectedTestsL();
	
	void SetFileSessionShareProtectedL();

private:
	CTestApp();
	void ConstructL();
	
	// test functions
	void TestURIAttachmentL();
	void TestBinaryAttachmentL();
	void TestFileAttachmentL(const TDesC& aExpectedFileName);
	void TestCIDAttachmentL();
	void TestSortAttachmentsL();
	void TestMultipleBinaryAttachmentsL();
	void TestMultipleFileAttachmentsL();
	void TestInvalidAttachmentsL(); 
	void TestManyBinaryAttachmentsL();
	void TestComparingAttachmentsL();
	void TestDeletingAttachmentL();
	void TestCopyingEntryWithAttachmentsL();
	void TestCopyingEntryWithAttachments2L();
	void TestAttachmentIndexPersistenceL();
	void TestFetchingHugeFileL();
	void TestDeleteCalendarFileL();
	void TestSameAttachmentWithDifferentDriveL();

	// utility functions
	TCalLocalUid StoreEntryWithAttachmentL(CCalAttachment* aAttachment);
	TCalLocalUid StoreEntryL(CCalEntry* aEntry);
	void ClearAttachmentsL(TCalLocalUid aLuid);
	void InitialiseFileL(CFileMan* aFileMan, const TDesC& aFileName);
	void TestFileHandleL(RFile& aFile, TInt aExpectedSize);
	void CheckDriveAndValueL(const TDesC8& aUid, TDriveName aDriveName);


private:
	CCalTestLibrary* iTestLib;
	CCalAttachmentManager* iAttachmentManager;
	RPIMTestServer iPIMTestServer;
	TInt iEntryCount;
	};

CTestApp::CTestApp()
	{
	}

CTestApp::~CTestApp()
	{
	
	// try to delete files
	TRAP_IGNORE(iTestLib->DeleteFileL(KCalendarFile, ETrue));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation1, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation2, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation3, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation4, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation5, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation6, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation7, EFalse));
	TRAP_IGNORE(iTestLib->DeleteFileL(KFileLocation8, EFalse));
	CFileMan* fileCopier=NULL;
	TRAP_IGNORE(fileCopier = CFileMan::NewL(iTestLib->FileSession()));
	TRAP_IGNORE(fileCopier->RmDir(KCalendarAttachmentsDir()));
	if(fileCopier)
		{
		delete fileCopier;
		}
	iPIMTestServer.Close();
	delete iAttachmentManager;
	delete iTestLib;
	}
	
void CTestApp::RunTestsL()
	{
	TestURIAttachmentL();
	TestBinaryAttachmentL();
	TestFileAttachmentL(KExpectedFileName1);

	//Save the same attachment file to an entry, the file name should be "filenamen" where 'n'is a trailing number 0,1,2,...to avoid redutancy.
    CFileMan* fileCopier = CFileMan::NewL(iTestLib->FileSession());
    CleanupStack::PushL(fileCopier);
	InitialiseFileL(fileCopier, KFileLocation1());
	CleanupStack::PopAndDestroy(fileCopier);
	TestFileAttachmentL(KExpectedFileName2);

	TestCIDAttachmentL();
	TestSortAttachmentsL();
	TestMultipleBinaryAttachmentsL();
	TestMultipleFileAttachmentsL();
	TestInvalidAttachmentsL();
	TestComparingAttachmentsL();
	TestDeletingAttachmentL();
	TestCopyingEntryWithAttachmentsL();
	TestCopyingEntryWithAttachments2L();
	TestAttachmentIndexPersistenceL();
	TestManyBinaryAttachmentsL();
	TestFetchingHugeFileL();
// 	This test need to add _EPOC_DRIVE_F in epoc.ini file
//  Example _EPOC_DRIVE_F = \epoc32\winscw\f_drive"
	TestSameAttachmentWithDifferentDriveL();
	TestDeleteCalendarFileL();
	}

CTestApp* CTestApp::NewLC()
	{
	CTestApp* self = new (ELeave) CTestApp;
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}

void CTestApp::SetFileSessionShareProtectedL()
	{
	// required for passing attachment file handles to server
	User::LeaveIfError(iTestLib->FileSession().ShareProtected()); 
	}

void CTestApp::ConstructL()
	{	
	User::LeaveIfError(iPIMTestServer.Connect());

	iTestLib = CCalTestLibrary::NewL(EFalse);
	
	iTestLib->ReplaceFileL(KCalendarFile());
	iTestLib->OpenFileL(KCalendarFile());
	
	CCalSession& session = iTestLib->GetSession();
	iAttachmentManager = CCalAttachmentManager::NewL(session, *iTestLib);
	CActiveScheduler::Start();
	
	CFileMan* fileCopier = CFileMan::NewL(iTestLib->FileSession());
	CleanupStack::PushL(fileCopier);
	
	InitialiseFileL(fileCopier, KFileLocation1());
	InitialiseFileL(fileCopier, KFileLocation2());
	InitialiseFileL(fileCopier, KFileLocation3());
	InitialiseFileL(fileCopier, KFileLocation4());
	InitialiseFileL(fileCopier, KFileLocation5());
	InitialiseFileL(fileCopier, KFileLocation6());
	InitialiseFileL(fileCopier, KFileLocation7());
	
	fileCopier->RmDir(KCalendarAttachmentsDir());

	CleanupStack::PopAndDestroy(fileCopier);
	}

void CTestApp::InitialiseFileL(CFileMan* aFileMan, const TDesC& aFileName)
	{
	TInt err = iTestLib->FileSession().MkDirAll(aFileName);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
		
	// symmetric loacation, just change of drive 
	TFileName initialLocationOfFile = aFileName;
	initialLocationOfFile.Replace(0, 2, KOriginalDrive());
	
	// get current time
	TTime now;
	now.HomeTime();
	
	// clear any read only attribute if the file is present such we avoid errors at copy time
	aFileMan->Attribs(aFileName, 0, KEntryAttReadOnly, now);
	
	// copy the file from Z: drive to C: drive (ROM to RAM)	
	User::LeaveIfError(aFileMan->Copy(initialLocationOfFile, aFileName));
	
	// clear any read only attribute such we avoid errors at open time (true on real hardware)
	User::LeaveIfError(aFileMan->Attribs(aFileName, 0, KEntryAttReadOnly, now));	
	}
	
// create an appointment for tomorrow with this attachment
// dont push the attachment passed into this function - entry takes ownership
TCalLocalUid CTestApp::StoreEntryWithAttachmentL(CCalAttachment* aAttachment)
	{
	test.Printf(_L("Creating entry with attachment\n"));

	CleanupStack::PushL(aAttachment);
	_LIT8(KBaseGuid, "uid_");
	TBuf8<16> guid;
	guid.Copy(KBaseGuid);
	guid.AppendNum(iEntryCount++);
	
	HBufC8* guidHBuf = guid.AllocLC();
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guidHBuf);
	CleanupStack::Pop(guidHBuf);
	
	CleanupStack::PushL(entry);
	
	TTime now;
	now.HomeTime();
	now += TTimeIntervalDays(8);
	
	TCalTime calTime;
	calTime.SetTimeLocalL(now);
	
	entry->SetStartAndEndTimeL(calTime, calTime);
	
	entry->AddAttachmentL(*aAttachment);
	CleanupStack::Pop(entry);
	CleanupStack::Pop(aAttachment);
	CleanupStack::PushL(entry);
	
	TCalLocalUid localUid = StoreEntryL(entry);

	CleanupStack::PopAndDestroy(entry);
	
	return localUid;
	}

// store an entry
TCalLocalUid CTestApp::StoreEntryL(CCalEntry* aEntry)
	{
	test.Printf(_L("Storing entry\n"));

	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(aEntry);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(success == entryArray.Count());
	
	TCalLocalUid localUid = aEntry->LocalUidL();
	
	test.Printf(_L("entry local uid=%d\n"), localUid);
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.Close()
	return localUid;
	}

// clear all attachments on this entry
void CTestApp::ClearAttachmentsL(TCalLocalUid aLuid)
	{
	test.Printf(_L("Clearing attachments on entry %d\n"), aLuid);
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(aLuid);
	CleanupStack::PushL(entry);
	
	while (entry->AttachmentCountL() > 0)
		{
		CCalAttachment* attach = entry->AttachmentL(0);
		entry->DeleteAttachmentL(*attach);
		}
	
	StoreEntryL(entry);
		
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestFileHandleL(RFile& aFile, TInt aExpectedSize)
	{
	test.Printf(_L("Verifying file handle returned correctly\n"));

	// check file name correct
	TFileName name;
	User::LeaveIfError(aFile.FullName(name));
	name.LowerCase();
	test(name.Find(KCalendarAttachmentsDir()) != KErrNotFound);
	
	// check file size correct
	TInt actualSize;
	User::LeaveIfError(aFile.Size(actualSize));
	test(actualSize == aExpectedSize);
	
	// check file cannot be moved
	_LIT(KDummy, "c:\\tcal_attach.dummy");
	test(KErrAccessDenied == aFile.Rename(KDummy()));
	}

//
void CTestApp::TestURIAttachmentL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("URI attachment\n"));
	test.Printf(_L("==============\n"));

	_LIT8(KTestUri1, "ftp://ftp.bar.com/pub/docs/foo.doc");
	_LIT8(KMimeType1, "application/msword");
	_LIT(KName1, "document name");
	
	// set up attachment properties
	CCalAttachment* attachment = CCalAttachment::NewUriL(KTestUri1());
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	// fetch the attachments on the entry
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	// check the attachment has been stored correctly 
	test.Printf(_L("Checking attachment details\n"));
	test(fetchedAttachment->Type() == CCalAttachment::EUri);
	test(fetchedAttachment->Value() == KTestUri1());
	test(fetchedAttachment->MimeType() == KMimeType1());
	test(fetchedAttachment->Label() == KName1());
	test(fetchedAttachment->FileAttachment() == NULL);
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestBinaryAttachmentL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Binary attachment\n"));
	test.Printf(_L("=================\n"));
	
	_LIT8(KBinaryData1, "fvgbuygbrvgbrfguyrt4fg67463t23875rt87ty8754yt89y96y0u6908yu896uyrtuyirfuygbcfyugyuvertgvrwsrv67er847rt87ei7rektyiufyhosiuydgdryiu");
	_LIT8(KMimeType1, "dummy/gibberish");
	_LIT(KName1, "binary data attachment name");
	
	// set up attachment properties
	HBufC8* data = KBinaryData1().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	// fetch the attachments on the entry
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	// check the attachment has been stored correctly 
	test.Printf(_L("Checking attachment details\n"));
	test(fetchedAttachment->Type() == CCalAttachment::EFile);
	test(fetchedAttachment->MimeType() == KMimeType1());
	test(fetchedAttachment->Label() == KName1());
	test(fetchedAttachment->IsAttributeSet(CCalAttachment::EExportInline));
	test(fetchedAttachment->FileAttachment() != NULL);
	test(fetchedAttachment->FileAttachment()->Size() == KBinaryData1().Length());
	
	_LIT(KExpectedDrive, "C:");
	TDriveName driveName;
	driveName.Copy(fetchedAttachment->FileAttachment()->Drive());
	driveName.UpperCase();
	test(driveName == KExpectedDrive());
	
	test(fetchedAttachment->Value().Length() == 0);
	fetchedAttachment->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment->Value() == KBinaryData1());
	
	RFile file;
	CleanupClosePushL(file);
	fetchedAttachment->FileAttachment()->FetchFileHandleL(file);
	TestFileHandleL(file, KBinaryData1().Length());
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestFileAttachmentL(const TDesC& aExpectedFileName)
	{
	test.Printf(_L("\n"));
	test.Printf(_L("File attachment\n"));
	test.Printf(_L("===============\n"));

	_LIT8(KMimeType1, "image/jpeg");
	_LIT(KName1, "snowman");
	
	RFile file;
	TInt err = file.Open(iTestLib->FileSession(), KFileLocation1(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestFileAttachmentL, failed to open file %S, err=%d\n"), &KFileLocation1(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	// set up attachment properties
	CCalAttachment* attachment = CCalAttachment::NewFileL(file);
	TTime lastModifiedUtc;
	file.Modified(lastModifiedUtc); // gets time in UTC
    TInt size;
	file.Size(size);

    CleanupStack::PopAndDestroy(&file); // file.Close()

	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());
	test(attachment->FileAttachment() != NULL);
	attachment->FileAttachment()->SetLastModifiedTimeUtc(lastModifiedUtc);
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	// fetch the attachments on the entry
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	
	// check the attachment has been stored correctly 
	test.Printf(_L("Checking attachment details\n"));
	test(fetchedAttachment->Type() == CCalAttachment::EFile);
	test(fetchedAttachment->MimeType() == KMimeType1());
	test(fetchedAttachment->Label() == KName1());
	test(fetchedAttachment->IsAttributeSet(CCalAttachment::EExportInline) == EFalse);
	test(fetchedAttachment->FileAttachment() != NULL);
	test(fetchedAttachment->FileAttachment()->Size() == size);
	test(fetchedAttachment->FileAttachment()->LastModifiedTimeUtc() == lastModifiedUtc);
	
	_LIT(KExpectedDrive, "C:");
	TDriveName driveName;
	driveName.Copy(fetchedAttachment->FileAttachment()->Drive());
	driveName.UpperCase();
	test(driveName == KExpectedDrive());

	test(fetchedAttachment->Value().Length() == 0);
	fetchedAttachment->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment->Value().Length() == size);

	RFile file2;
	CleanupClosePushL(file2);
	fetchedAttachment->FileAttachment()->FetchFileHandleL(file2);
	TBuf<100> name;
	file2.Name(name);
	test (aExpectedFileName == name);
	TestFileHandleL(file2, size);
	CleanupStack::PopAndDestroy(&file2);
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestCIDAttachmentL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("CID attachment\n"));
	test.Printf(_L("==============\n"));

	_LIT8(KContentId1, "&£$rg$*£$r*(reuf(%ehtg*(e%$y*tgeght*e%y*(t£$*(rt*^85643856438765");
	_LIT8(KMimeType1, "image/jpeg");
	_LIT(KName1, "rodent");
	
	RFile file;
	TInt err = file.Open(iTestLib->FileSession(), KFileLocation2(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestCIDAttachmentL, failed to open file %S, err=%d\n"), &KFileLocation2(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	// set up attachment properties
	CCalAttachment* attachment = CCalAttachment::NewFileByContentIdL(KContentId1());
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());

	test(attachment->FileAttachment() != NULL);
	attachment->FileAttachment()->SetResourceL(file);
	TTime lastModifiedUtc;
	file.Modified(lastModifiedUtc); // gets time in UTC
	attachment->FileAttachment()->SetLastModifiedTimeUtc(lastModifiedUtc);
	TInt size;
	file.Size(size);
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	CleanupStack::PopAndDestroy(&file); // file.Close()
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	// fetch the attachments on the entry
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	
	// check the attachment has been stored correctly 
	test.Printf(_L("Checking attachment details\n"));
	test(fetchedAttachment->Type() == CCalAttachment::EFile);
	test(fetchedAttachment->MimeType() == KMimeType1());
	test(fetchedAttachment->Label() == KName1());
	test(fetchedAttachment->IsAttributeSet(CCalAttachment::EExportInline) == EFalse);
	test(fetchedAttachment->FileAttachment() != NULL);
	test(fetchedAttachment->FileAttachment()->Size() == size); 
	test(fetchedAttachment->FileAttachment()->LastModifiedTimeUtc() == lastModifiedUtc); 
	
	_LIT(KExpectedDrive, "C:");
	TDriveName driveName;
	driveName.Copy(fetchedAttachment->FileAttachment()->Drive());
	driveName.UpperCase();
	test(driveName == KExpectedDrive()); 
	
	test(fetchedAttachment->Value().Length() == 0);
	fetchedAttachment->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment->Value().Length() == size);

	RFile file2;
	CleanupClosePushL(file2);
	fetchedAttachment->FileAttachment()->FetchFileHandleL(file2);
	TestFileHandleL(file2, size);
	CleanupStack::PopAndDestroy(&file2);
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestSortAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Sorting attachments\n"));
	test.Printf(_L("==================\n"));
	
	// this test assumes two attachments are already present in the calendar database
	test.Printf(_L("Fetch all attachments sorted by size\n"));
	CCalAttachmentIterator* attachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);
	
	CCalAttachment* att1 = attachments->NextL();
	test(att1 != NULL);
	CleanupStack::PushL(att1);

	CCalAttachment* att2 = attachments->NextL();
	test(att2 != NULL);
	CleanupStack::PushL(att2);

	// make sure the items are not the same (different pointers)
	test(att1 != att2);

	test.Printf(_L("Check correct order\n"));
	test(att1->FileAttachment() != NULL);
	test(att2->FileAttachment() != NULL);
	test(att1->FileAttachment()->Size() >= att2->FileAttachment()->Size());
	
	test.Printf(_L("Fetch entry with this attachment\n"));
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	iAttachmentManager->EntriesReferencingFileAttachmentL(localUids, *att1);
	test(localUids.Count() == 1);
	
	test.Printf(_L("Check correct entry fetched\n"));
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(localUids[0]);
	CleanupStack::PopAndDestroy(&localUids); // localUids.Close()
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	test(fetchedAttachment->Label() == att1->Label());
	test(fetchedAttachment->MimeType() == att1->MimeType());
	test(fetchedAttachment->Type() == att1->Type());
	test(fetchedAttachment->IsAttributeSet(CCalAttachment::EExportInline) == att1->IsAttributeSet(CCalAttachment::EExportInline));
	test(fetchedAttachment->FileAttachment() != NULL);
	test(fetchedAttachment->FileAttachment()->Size() == att1->FileAttachment()->Size());
	test(fetchedAttachment->FileAttachment()->LastModifiedTimeUtc() == att1->FileAttachment()->LastModifiedTimeUtc());
	CleanupStack::PopAndDestroy(entry);
	
	TInt lastSize = att2->FileAttachment()->Size();
	
	CleanupStack::PopAndDestroy(att2);
	att2 = NULL;	
	CleanupStack::PopAndDestroy(att1);
	att1 = NULL;
	
	// check all attachments in correct size order
	while (attachments->HasMore())
		{
		CCalAttachment* nextAttachment = attachments->NextL();
		CleanupStack::PushL(nextAttachment);
		test(nextAttachment->FileAttachment() != NULL);
		test(nextAttachment->FileAttachment()->Size() <= lastSize);
		lastSize = nextAttachment->FileAttachment()->Size();
		CleanupStack::PopAndDestroy(nextAttachment);
		}
	
	CleanupStack::PopAndDestroy(attachments);
	
	test.Printf(_L("Fetch all attachments sorted by last modified date\n"));
	attachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortByDateModifiedNewestFirst);
	CleanupStack::PushL(attachments);
	
	att1 = attachments->NextL();
	test(att1 != NULL);
	CleanupStack::PushL(att1);

	att2 = attachments->NextL();
	test(att2 != NULL);
	CleanupStack::PushL(att2);

	// make sure the items are not the same (different pointers)
	test(att1 != att2);	
	
	test.Printf(_L("Check correct order\n"));
	test(att1->FileAttachment() != NULL);
	test(att2->FileAttachment() != NULL);
	test(att1->FileAttachment()->LastModifiedTimeUtc() >= att2->FileAttachment()->LastModifiedTimeUtc());
	
	TTime lastTime = att2->FileAttachment()->LastModifiedTimeUtc();
	
	CleanupStack::PopAndDestroy(att2);
	CleanupStack::PopAndDestroy(att1);	
	
	// check all attachments in correct time order
	while (attachments->HasMore())
		{
		CCalAttachment* nextAttachment = attachments->NextL();
		CleanupStack::PushL(nextAttachment);
		test(nextAttachment->FileAttachment() != NULL);
		test(nextAttachment->FileAttachment()->LastModifiedTimeUtc() <= lastTime);
		lastTime = nextAttachment->FileAttachment()->LastModifiedTimeUtc();
		CleanupStack::PopAndDestroy(nextAttachment);
		}
		
	CleanupStack::PopAndDestroy(attachments);
	}

void CTestApp::TestMultipleBinaryAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Multiple binary attachment\n"));
	test.Printf(_L("==========================\n"));
	
	_LIT8(KBinaryData1, "zxzxzxzxxzxzxzzxzxzxzxzxzxzxzxzxzzzzxzxzxxzxzzxzxzxxzzxzxzx");
	_LIT8(KMimeType1, "dummy/gibberish");
	_LIT(KName1, "multiple attachments 1");
	
	_LIT8(KBinaryData2, "nmnmnmnmnmnmnmmnmnmnmnmnmnmnmmnmnmnmmnmnmnmnmnmnmnmnmnmmnmnmmn");
	_LIT8(KMimeType2, "something/else");
	_LIT(KName2, "multiple attachments 2");
	
	_LIT8(KBinaryData3, "aoaoaoaoooaoaoaoaoaoaoaoaoaoaoaoaoaoaoaoaaa");
	_LIT8(KMimeType3, "other/");
	_LIT(KName3, "multiple attachments 3");
	
	// set up attachment properties
	HBufC8* data = KBinaryData1().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	test.Printf(_L("Add another two binary attachments to this entry\n"));
	// add another two binary attachments to this entry
	HBufC8* data2 = KBinaryData2().AllocLC();
	CCalAttachment* attachment2 = CCalAttachment::NewFileL(data2);
	CleanupStack::Pop(data2);
	CleanupStack::PushL(attachment2);
	
	attachment2->SetMimeTypeL(KMimeType2());
	attachment2->SetLabelL(KName2());


	HBufC8* data3 = KBinaryData3().AllocLC();
	CCalAttachment* attachment3 = CCalAttachment::NewFileL(data3);
	CleanupStack::Pop(data3);
	CleanupStack::PushL(attachment3);
	
	attachment3->SetMimeTypeL(KMimeType3());
	attachment3->SetLabelL(KName3());
	
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	entry->AddAttachmentL(*attachment2);
	entry->AddAttachmentL(*attachment3);
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::Pop(attachment3);
	CleanupStack::Pop(attachment2);
	
	CCalEntry* entry2 = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry2);

	test.Printf(_L("Checking attachment details\n"));
	test(entry2->AttachmentCountL() == 3);
	
	// check the attachment has been stored correctly 
	CCalAttachment* fetchedAttachment1 = entry2->AttachmentL(0);	
	test(fetchedAttachment1->Type() == CCalAttachment::EFile);
	test(fetchedAttachment1->FileAttachment() != NULL);
	fetchedAttachment1->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment1->Value().Length() > 0);
	test(fetchedAttachment1->MimeType() == KMimeType1());
	test(fetchedAttachment1->Label() == KName1());
	test(fetchedAttachment1->IsAttributeSet(CCalAttachment::EExportInline));
	test(fetchedAttachment1->FileAttachment()->Size() == KBinaryData1().Length());
	
	CCalAttachment* fetchedAttachment2 = entry2->AttachmentL(1);
	test(fetchedAttachment2->Type() == CCalAttachment::EFile);
	test(fetchedAttachment2->FileAttachment() != NULL);
	fetchedAttachment2->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment2->Value().Length() > 0);
	test(fetchedAttachment2->MimeType() == KMimeType2());
	test(fetchedAttachment2->Label() == KName2());
	test(fetchedAttachment2->IsAttributeSet(CCalAttachment::EExportInline));
	test(fetchedAttachment2->FileAttachment()->Size() == KBinaryData2().Length());
	
	CCalAttachment* fetchedAttachment3 = entry2->AttachmentL(2);	
	test(fetchedAttachment3->Type() == CCalAttachment::EFile);
	test(fetchedAttachment3->FileAttachment() != NULL);
	fetchedAttachment3->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment3->Value().Length() > 0);
	test(fetchedAttachment3->MimeType() == KMimeType3());
	test(fetchedAttachment3->Label() == KName3());
	test(fetchedAttachment3->IsAttributeSet(CCalAttachment::EExportInline));
	test(fetchedAttachment3->FileAttachment()->Size() == KBinaryData3().Length());
	
	CleanupStack::PopAndDestroy(entry2);
	}

void CTestApp::TestMultipleFileAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Multiple file attachment\n"));
	test.Printf(_L("========================\n"));

	_LIT8(KContentId1, "1235472154238423674");
	_LIT8(KMimeType1, "text/plain");
	_LIT(KName1, "txt attachment");

	_LIT8(KContentId2, "437236874623874234");
	_LIT8(KMimeType2, "application/msword");
	_LIT(KName2, "word attachment");
	
	_LIT8(KContentId3, "032948782396748935");
	_LIT8(KMimeType3, "image/bmp");
	_LIT(KName3, "bmp attachment");
	
	
	RFile file;
	TInt err = file.Open(iTestLib->FileSession(), KFileLocation3(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestMultipleFileAttachmentsL, failed to open file %S, err=%d\n"), &KFileLocation3(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	// set up attachment properties
	CCalAttachment* attachment = CCalAttachment::NewFileByContentIdL(KContentId1());
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());
	
	test(attachment->FileAttachment() != NULL);
	attachment->FileAttachment()->SetResourceL(file);
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	CleanupStack::PopAndDestroy(&file); // file.Close()
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);

	test.Printf(_L("Add another two file attachments to this entry\n"));
	
	CCalAttachment* attachment2 = CCalAttachment::NewFileByContentIdL(KContentId2());
	CleanupStack::PushL(attachment2);
	
	attachment2->SetMimeTypeL(KMimeType2());
	attachment2->SetLabelL(KName2());
	test(attachment2->FileAttachment() != NULL);
	
	RFile file2;
	err = file2.Open(iTestLib->FileSession(), KFileLocation4(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestMultipleFileAttachmentsL 2, failed to open file %S, err=%d\n"), &KFileLocation4(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file2);
	attachment2->FileAttachment()->SetResourceL(file2);
	CleanupStack::PopAndDestroy(&file2);
	
	CCalAttachment* attachment3 = CCalAttachment::NewFileByContentIdL(KContentId3());
	CleanupStack::PushL(attachment3);
	
	attachment3->SetMimeTypeL(KMimeType3());
	attachment3->SetLabelL(KName3());
	test(attachment3->FileAttachment() != NULL);
	
	RFile file3;
	err = file3.Open(iTestLib->FileSession(), KFileLocation5(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestMultipleFileAttachmentsL, failed to open file %S, err=%d\n"), &KFileLocation5(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file3);
	attachment3->FileAttachment()->SetResourceL(file3);
	CleanupStack::PopAndDestroy(&file3);
	
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	entry->AddAttachmentL(*attachment2);
	entry->AddAttachmentL(*attachment3);
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::Pop(attachment3);
	CleanupStack::Pop(attachment2);
	
	// fetch the attachments on the entry
	CCalEntry* entry2 = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry2);
	test(entry2->AttachmentCountL() == 3);
	
	// check the attachment has been stored correctly 
	test.Printf(_L("Checking attachment details\n"));
	
	CCalAttachment* fetchedAttachment1 = entry2->AttachmentL(0);	
	test(fetchedAttachment1->FileAttachment() != NULL);
	test(fetchedAttachment1->Type() == CCalAttachment::EFile);
	fetchedAttachment1->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment1->Value().Length() > 0);
	test(fetchedAttachment1->MimeType() == KMimeType1());
	test(fetchedAttachment1->Label() == KName1());
	test(fetchedAttachment1->IsAttributeSet(CCalAttachment::EExportInline) == EFalse);
	test(fetchedAttachment1->FileAttachment()->Size() > 0);
	
	CCalAttachment* fetchedAttachment2 = entry2->AttachmentL(1);
	test(fetchedAttachment2->Type() == CCalAttachment::EFile);
	test(fetchedAttachment2->FileAttachment() != NULL);
	fetchedAttachment2->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment2->Value().Length() > 0);
	test(fetchedAttachment2->MimeType() == KMimeType2());
	test(fetchedAttachment2->Label() == KName2());
	test(fetchedAttachment2->IsAttributeSet(CCalAttachment::EExportInline) == EFalse);
	test(fetchedAttachment2->FileAttachment()->Size() > 0);
	
	CCalAttachment* fetchedAttachment3 = entry2->AttachmentL(2);
	test(fetchedAttachment3->Type() == CCalAttachment::EFile);
	test(fetchedAttachment3->FileAttachment() != NULL);
	fetchedAttachment3->FileAttachment()->LoadBinaryDataL();
	test(fetchedAttachment3->Value().Length() > 0);
	test(fetchedAttachment3->MimeType() == KMimeType3());
	test(fetchedAttachment3->Label() == KName3());
	test(fetchedAttachment3->IsAttributeSet(CCalAttachment::EExportInline) == EFalse);
	test(fetchedAttachment3->FileAttachment()->Size() > 0);
	
	CleanupStack::PopAndDestroy(entry2);
	}

void CTestApp::TestInvalidAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Negative tests\n"));
	test.Printf(_L("========================\n"));

	test.Printf(_L("Test adding file with invalid permissions (read only)\n"));
	RFile file;
	TInt err = file.Open(iTestLib->FileSession(), KFileLocation6(), EFileRead);
	if (err != KErrNone)
		{
		test.Printf(_L("TestInvalidAttachmentsL, failed to open file %S, err=%d\n"), &KFileLocation6(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	CCalAttachment* attachment = CCalAttachment::NewFileL(file);
	CleanupStack::PopAndDestroy(&file);
	
	TRAP(err, StoreEntryWithAttachmentL(attachment));
	test.Printf(_L("Store entry failed with error %d\n"), err);
	test(err == KErrAccessDenied);
	// 
	test.Printf(_L("Test adding file with invalid permissions (read and sharable only)\n"));
	err = file.Open(iTestLib->FileSession(), KFileLocation6(), EFileRead|EFileShareExclusive);
	if (err != KErrNone)
		{
		test.Printf(_L("TestInvalidAttachmentsL, failed to open file %S, err=%d\n"), &KFileLocation6(), err);
		User::Leave(err);
		}	
	CleanupClosePushL(file);
	attachment = CCalAttachment::NewFileL(file);
	CleanupStack::PopAndDestroy(&file);
	
	TRAP(err, StoreEntryWithAttachmentL(attachment));
	test.Printf(_L("Store entry failed with error %d\n"), err);
	test(err == KErrAccessDenied);

	//
	test.Printf(_L("Test storing an entry with a content ID attachment\n"));

	_LIT8(KContentId, "12345612435612435612");
	CCalAttachment* contentIdAttachment = CCalAttachment::NewFileByContentIdL(KContentId());	
	TRAP(err, StoreEntryWithAttachmentL(contentIdAttachment));
	test(err == KErrArgument);
	
	//
	test.Printf(_L("Test adding an attachment with empty binary data\n"));

	HBufC8* binaryData = KNullDesC8().AllocL();
	CCalAttachment* emptyAttachment = NULL;
	TRAP(err, emptyAttachment = CCalAttachment::NewFileL(binaryData));
	delete emptyAttachment;
	delete binaryData;
	test(err == KErrArgument);
	
	//
	test.Printf(_L("Test adding an attachment with very long URI, name and MIME Type\n"));

	_LIT8(KLongURI, "uriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuriuri");
	_LIT8(KLongMimeType, "mimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetypemimetype");
	_LIT(KLongName, "namenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamenamename");

	CCalAttachment* longAttachment = CCalAttachment::NewUriL(KLongURI());
	CleanupStack::PushL(longAttachment);

	longAttachment->SetMimeTypeL(KLongMimeType());
	
	TRAP(err, longAttachment->SetLabelL(KLongName()));
	test(err == KErrArgument);
	
	TBuf<KCalAttachmentMaxLabelLength> shortenedName;
	shortenedName.Copy(KLongName().Left(KCalAttachmentMaxLabelLength));
	longAttachment->SetLabelL(shortenedName);

	CleanupStack::Pop(longAttachment);
	TCalLocalUid luid = StoreEntryWithAttachmentL(longAttachment);
	
	CCalEntry* longEntry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(longEntry);
	test(longEntry->AttachmentCountL() == 1);
	
	CCalAttachment* fetchedLongAttachment = longEntry->AttachmentL(0);
	test(fetchedLongAttachment->Type() == CCalAttachment::EUri);
	test(fetchedLongAttachment->Value() == KLongURI());
	test(fetchedLongAttachment->MimeType() == KLongMimeType());
	test(fetchedLongAttachment->Label() == shortenedName);
	//
	test.Printf(_L("Test accessing an out-of-range attachment\n"));
	test(longEntry->AttachmentL(-1) == NULL);
	test(longEntry->AttachmentL(100) == NULL);
	
	CleanupStack::PopAndDestroy(longEntry);
	//
	test.Printf(_L("test fetching binary data for unsaved attachment\n"));
	_LIT8(KBinaryData, "zzzzzzzzzzzzzzzzzzzzz");
	HBufC8* binaryData2 = KBinaryData().AllocLC();
	CCalAttachment* unsavedAttachment = CCalAttachment::NewFileL(binaryData2);
	CleanupStack::Pop(binaryData2);
	CleanupStack::PushL(unsavedAttachment);
	
	test(unsavedAttachment->FileAttachment() != NULL);
	TRAP(err, unsavedAttachment->FileAttachment()->LoadBinaryDataL());
	test.Printf(_L("fetch binary data gave error %d\n"), err);
	test(err == KErrNone);
	test(unsavedAttachment->Value() == KBinaryData());
	
	test.Printf(_L("test fetching binary data for deleted attachment\n"));
	CleanupStack::Pop(unsavedAttachment);
	luid = StoreEntryWithAttachmentL(unsavedAttachment);

	CCalEntry* fetchedEntry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(fetchedEntry);
	test(fetchedEntry->AttachmentCountL() == 1);
	CCalAttachment* fetchedAttachment = fetchedEntry->AttachmentL(0);
	
	RArray<TCalLocalUid> uids;
	CleanupClosePushL(uids);
	uids.AppendL(luid);
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().DeleteL(uids, success);
	CleanupStack::PopAndDestroy(&uids); // uids.Close()

	test(fetchedAttachment->FileAttachment() != NULL);
	TRAP(err, fetchedAttachment->FileAttachment()->LoadBinaryDataL());
	test.Printf(_L("fetch binary data failed with error %d\n"), err);
	test(err == KErrNotFound);
	test(fetchedAttachment->Value().Length() == 0);
	CleanupStack::PopAndDestroy(fetchedEntry);
	}

void CTestApp::TestComparingAttachmentsL()
	{
	CCalAttachmentIterator* attachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);

	CCalAttachment* att1 = attachments->NextL();
	CleanupStack::PushL(att1);
	test(att1 != NULL);
	
	// fetch the same entry twice
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	iAttachmentManager->EntriesReferencingFileAttachmentL(localUids, *att1);
	TCalLocalUid luid = localUids[0];
	CleanupStack::PopAndDestroy(&localUids);
	CleanupStack::PopAndDestroy(att1);

	CCalEntry* entry1 = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry1);
	test(entry1 != NULL);

	CCalEntry* entry2 = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry2);
	test(entry2 != NULL);

	test(entry1->CompareL(*entry2)); // check the entries match	
	
	// edit attachment	
	CCalAttachment* attach1 = entry1->AttachmentL(0);
	CCalAttachment* attach2 = entry2->AttachmentL(0);
	
	// change MIME type and check Compare fails
	_LIT8(KDummyString, "dummy");
	attach1->SetMimeTypeL(KDummyString());
	test(entry1->CompareL(*entry2) == EFalse);

	attach1->SetMimeTypeL(attach2->MimeType());
	test(entry1->CompareL(*entry2));
	
	// change last modified date and check Compare fails
	TTime time(TDateTime(2003, EJanuary, 23, 9, 0, 0, 0));
	test(attach1->FileAttachment() != NULL);
	attach1->FileAttachment()->SetLastModifiedTimeUtc(time);
	test(entry1->CompareL(*entry2) == EFalse);

	test(attach2->FileAttachment() != NULL);
	attach1->FileAttachment()->SetLastModifiedTimeUtc(attach2->FileAttachment()->LastModifiedTimeUtc());
	test(entry1->CompareL(*entry2));

	// change name and check Compare fails
	_LIT(KName, "dummy name");
	attach1->SetLabelL(KName);
	test(entry1->CompareL(*entry2) == EFalse);

	attach1->SetLabelL(attach2->Label());
	test(entry1->CompareL(*entry2));

	// change drive and check Compare fails
	_LIT(KDriveName, "e:");
	attach1->FileAttachment()->SetDriveL(KDriveName());
	test(entry1->CompareL(*entry2) == EFalse);

	attach1->FileAttachment()->SetDriveL(attach2->FileAttachment()->Drive());
	test(entry1->CompareL(*entry2));

	// change attributes and check Compare fails
	attach1->SetAttribute(CCalAttachment::EExportInline);
	test(entry1->CompareL(*entry2) == EFalse);

	attach1->ClearAttribute(CCalAttachment::EExportInline);
	test(entry1->CompareL(*entry2));

	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	CleanupStack::PopAndDestroy(attachments);
	}

void CTestApp::TestDeletingAttachmentL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Deleting attachment\n"));
	test.Printf(_L("===================\n"));
	
	test.Printf(_L("Creating an attachment with binary data\n"));

	_LIT8(KBinaryData1, "abcdefghijklmnopqrstuvwxyz");
	
	HBufC8* data = KBinaryData1().AllocL();
	CCalAttachment* binaryAttachment = CCalAttachment::NewFileL(data);
	TCalLocalUid luid = StoreEntryWithAttachmentL(binaryAttachment);
	
	test.Printf(_L("Fetching entry and removing attachment\n"));
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	
	test(entry->AttachmentCountL() == 1);
	CCalAttachment* entryAttachment = entry->AttachmentL(0);
	test(entryAttachment->FileAttachment() != NULL);

	RFile file;
	CleanupClosePushL(file);
	entryAttachment->FileAttachment()->FetchFileHandleL(file);
	TFileName fileName;
	file.FullName(fileName);
	test.Printf(_L("attachment stored in file %S\n"), &fileName);
	CleanupStack::PopAndDestroy(&file);

	entry->DeleteAttachmentL(*entryAttachment);

	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);

	TInt err = iPIMTestServer.CheckForFile(fileName);
	test(err == KErrNotFound);
	
	//
	entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 0);
	
	_LIT8(KURI, "dummy uri");
	CCalAttachment* uriAttachment = CCalAttachment::NewUriL(KURI());
	CleanupStack::PushL(uriAttachment);
	
	entry->AddAttachmentL(*uriAttachment);
	test(entry->AttachmentCountL() == 1);
	entry->DeleteAttachmentL(*uriAttachment);
	test(entry->AttachmentCountL() == 0);
	
	CleanupStack::Pop(uriAttachment);
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestCopyingEntryWithAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Copy entry with attachment after storing\n"));
	test.Printf(_L("========================================\n"));
	
	_LIT8(KBinaryData1, "zzzzzzzzzzzzzzzzzz");
	
	test.Printf(_L("Store an entry with a binary data attachment\n"));
	// set up attachment properties
	HBufC8* data = KBinaryData1().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	
	// store an entry with this attachment
	TCalLocalUid luid = StoreEntryWithAttachmentL(attachment);
	
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	
	CCalAttachment* originalAttachment = entry->AttachmentL(0);
	test(originalAttachment->FileAttachment() != NULL);
	RFile file;
	CleanupClosePushL(file);
	originalAttachment->FileAttachment()->FetchFileHandleL(file);
	TFileName fileName;
	file.FullName(fileName); // get the full name of the attachment
	test.Printf(_L("original entry stored attachment at %S\n"), &fileName);

	CleanupStack::PopAndDestroy(&file); // file.Close()
	
	test.Printf(_L("fetch the entry and copy it\n"));
	_LIT8(KCopy, "copy");
	HBufC8* copyGuid = KCopy().AllocLC();
	CCalEntry* entryCopy = CCalEntry::NewL(entry->EntryTypeL(), copyGuid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(copyGuid);
	CleanupStack::PushL(entryCopy);
	entryCopy->CopyFromL(*entry, CCalEntry::EDontCopyId);
	
	test.Printf(_L("store the copy then delete the original entry\n"));
	TCalLocalUid copyLuid = StoreEntryL(entryCopy);
	CleanupStack::PopAndDestroy(entryCopy);
	
	iTestLib->SynCGetEntryViewL().DeleteL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	CCalEntry* fetchedCopy = iTestLib->SynCGetEntryViewL().FetchL(copyLuid);
	CleanupStack::PushL(fetchedCopy);
	test(fetchedCopy->AttachmentCountL() == 1);

	CCalAttachment* fetchedCopyAttachment = fetchedCopy->AttachmentL(0);
	test(fetchedCopyAttachment->FileAttachment() != NULL);
	
	RFile file2;
	CleanupClosePushL(file2);
	fetchedCopyAttachment->FileAttachment()->FetchFileHandleL(file2);
	TFileName fileName2;
	file2.FullName(fileName2); // get the full name of the attachment
	test.Printf(_L("copied entry stored attachment at %S\n"), &fileName2);
	test(fileName == fileName2);

	CleanupStack::PopAndDestroy(&file2);
	CleanupStack::PopAndDestroy(fetchedCopy);
	}

void CTestApp::TestCopyingEntryWithAttachments2L()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Copy entry with attachment before storing\n"));
	test.Printf(_L("=========================================\n"));
	
	_LIT8(KBinaryData1, "zzzzzzzzzzzzzzzzzz");
	_LIT8(KGuid, "copytest");
	_LIT8(KGuid2, "copytest2");
	
	test.Printf(_L("Create an entry with a binary data attachment\n"));

	// set up attachment properties
	HBufC8* data = KBinaryData1().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	CleanupStack::PushL(attachment);

	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->AddAttachmentL(*attachment);
	
	test.Printf(_L("Create a copy of the entry\n"));
	HBufC8* copyGuid = KGuid2().AllocLC();
	CCalEntry* entryCopy = CCalEntry::NewL(entry->EntryTypeL(), copyGuid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(copyGuid);
	CleanupStack::PushL(entryCopy);
	entryCopy->CopyFromL(*entry, CCalEntry::EDontCopyId);
	
	test.Printf(_L("Store both entries\n"));
	TCalLocalUid luid = StoreEntryL(entry);
	TCalLocalUid luidCopy = StoreEntryL(entryCopy);

	// entries are the same
	test(entry->CompareL(*entryCopy)); 
	
	CleanupStack::PopAndDestroy(entryCopy);
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::Pop(attachment);
	
	//
	
	CCalEntry* fetchedEntry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(fetchedEntry);

	CCalEntry* fetchedEntryCopy = iTestLib->SynCGetEntryViewL().FetchL(luidCopy);
	CleanupStack::PushL(fetchedEntryCopy);
		
	// entries are still the same - the fact the attachments are stored in different files shouldn't matter
	// since this data can't be compared here anyway - their size is the same
	test(fetchedEntry->CompareL(*fetchedEntryCopy)); 
	
	CCalAttachment* originalAttachment = fetchedEntry->AttachmentL(0);
	test(originalAttachment->FileAttachment() != NULL);
	RFile file;
	CleanupClosePushL(file);
	originalAttachment->FileAttachment()->FetchFileHandleL(file);
	TFileName fileName;
	file.FullName(fileName); // get the full name of the attachment
	test.Printf(_L("original entry stored attachment at %S\n"), &fileName);

	CleanupStack::PopAndDestroy(&file); // file.Close()
	
	CCalAttachment* copiedAttachment = fetchedEntryCopy->AttachmentL(0);
	test(copiedAttachment->FileAttachment() != NULL);
	RFile file2;
	CleanupClosePushL(file2);
	copiedAttachment->FileAttachment()->FetchFileHandleL(file2);
	TFileName fileName2;
	file2.FullName(fileName2); // get the full name of the attachment
	test.Printf(_L("copied entry stored attachment at %S\n"), &fileName2);
	test(fileName != fileName2);

	CleanupStack::PopAndDestroy(&file2);
	
	CleanupStack::PopAndDestroy(fetchedEntryCopy);
	CleanupStack::PopAndDestroy(fetchedEntry);
	}

void CTestApp::TestAttachmentIndexPersistenceL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Attachment index persistence\n"));
	test.Printf(_L("============================\n"));
	
	RArray<TUint> attachmentSizes;
	CleanupClosePushL(attachmentSizes);
	
	CCalAttachmentIterator* initialAttachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(initialAttachments);
	
	CCalAttachment* initAttach = initialAttachments->NextL();
	CleanupStack::PushL(initAttach);
	
	TInt attachmentCount = 0;
	while (initAttach)
		{
		test(initAttach->FileAttachment() != NULL);
		attachmentSizes.AppendL(initAttach->FileAttachment()->Size());
		CleanupStack::PopAndDestroy(initAttach);
		initAttach = NULL;
		initAttach = initialAttachments->NextL();
		CleanupStack::PushL(initAttach);
		attachmentCount++;
		}
	CleanupStack::Pop(initAttach);
	test(attachmentCount == attachmentSizes.Count());
	test(initialAttachments->Count() == attachmentCount);
	CleanupStack::PopAndDestroy(initialAttachments);
	
	delete iAttachmentManager;
	iAttachmentManager = NULL;
	delete iTestLib;
	iTestLib = NULL;
	
	iTestLib = CCalTestLibrary::NewL(EFalse);
	iTestLib->OpenFileL(KCalendarFile());
	
	CCalSession& session = iTestLib->GetSession();
	iAttachmentManager = CCalAttachmentManager::NewL(session, *iTestLib);
	CActiveScheduler::Start();

	CCalAttachmentIterator* latestAttachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(latestAttachments);
	test(latestAttachments->Count() == attachmentCount);

	CCalAttachment* latestAttach = latestAttachments->NextL();
	CleanupStack::PushL(latestAttach);
	TUint attachmentCount2 = 0;
	while (latestAttach)
		{
		test(latestAttach->FileAttachment() != NULL);
		test(attachmentSizes[attachmentCount2] == latestAttach->FileAttachment()->Size());
		attachmentCount2++;
		CleanupStack::PopAndDestroy(latestAttach);
		latestAttach = NULL;
		latestAttach = latestAttachments->NextL();
		CleanupStack::PushL(latestAttach);
		}
	test(attachmentCount == attachmentCount2);
	CleanupStack::Pop(latestAttach);
	CleanupStack::PopAndDestroy(latestAttachments);
	
	CleanupStack::PopAndDestroy(&attachmentSizes); // attachmentSizes.Close()	
	}

void CTestApp::TestManyBinaryAttachmentsL()
	{
	test.Printf(_L("\n"));
	test.Printf(_L("Many Binary attachments\n"));
	test.Printf(_L("=======================\n"));
	
	_LIT8(KBinaryData1, "abcdefghijklmnopqrstuvwxyz");
	
	CCalAttachmentIterator* attIter1 = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	TInt numExistingAttachments  = attIter1->Count();
	delete attIter1;
	
	const TInt KNumAttachments = 500;
	test.Printf(_L("Adding %d entries with attachments\n"), KNumAttachments);
	// set up attachment properties
	for (TInt i = 0; i < KNumAttachments; i++)
		{
		HBufC8* data = KBinaryData1().AllocL();
		CCalAttachment* attachment = CCalAttachment::NewFileL(data);
		
		// store an entry with this attachment
		StoreEntryWithAttachmentL(attachment);
		}

	CCalAttachmentIterator* attIter = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attIter);
	
	test(attIter->Count() == numExistingAttachments + KNumAttachments);
	
	CCalAttachment* att = attIter->NextL();
	CleanupStack::PushL(att);
	test(att->FileAttachment() != NULL);

	RFile file;
	CleanupClosePushL(file);
	att->FileAttachment()->FetchFileHandleL(file);
	TFileName name;
	file.FullName(name);
	CleanupStack::PopAndDestroy(&file); // file.Close()
	CleanupStack::PopAndDestroy(att);
	CleanupStack::PopAndDestroy(attIter);
	
	iTestLib->CleanDatabaseL();
	
	TInt err = iPIMTestServer.CheckForFile(name);
	test(err == KErrNotFound);
	
	CCalAttachmentIterator* attIter2 = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	test(attIter2->Count() == 0);
	delete attIter2;
	}

void CTestApp::RunNonShareProtectedTestsL()
	{
	// this test panics!
	test.Printf(_L("Test adding file from session with invalid permissions (not sharable)\n"));
	RFs fs;
	User::LeaveIfError(fs.Connect());
	RFile file;
	TInt err = file.Open(fs, KFileLocation7(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("RunNonShareProtectedTestsL, failed to open file %S, err=%d\n"), &KFileLocation7(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	CCalAttachment* attachment = CCalAttachment::NewFileL(file);
	CleanupStack::PopAndDestroy(&file);
	
	TRAP(err, StoreEntryWithAttachmentL(attachment));
	test(err == KErrBadHandle);
	}

void CTestApp::TestFetchingHugeFileL()
	{
	SetFileSessionShareProtectedL();
	test.Printf(_L("TestFetchingHugeFileL\n"));
	
	RFile file;
	TInt err = file.Replace(iTestLib->FileSession(), KFileLocation8(), EFileWrite);
	if (err != KErrNone)
		{
		test.Printf(_L("TestFetchingHugeFileL, failed to open file %S, err=%d\n"), &KFileLocation8(), err);
		User::Leave(err);
		}
	CleanupClosePushL(file);
	
	_LIT8(KBasicString, "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz");
	_LIT(KName, "huge file");
	const TInt KOneMillion = 1000000;
	const TInt KHugeFileSize = KOneMillion;
	const TInt KNumStrings = KHugeFileSize / KBasicString().Length(); 
	// Create a file of size 1,000,000 bytes
	// Note that creating a file much bigger than this will not fit into RAM.
	
	for (TInt i = 0; i < KNumStrings && err == KErrNone; ++i)
		{
		err = file.Write(KBasicString());
		}
	
	if (err != KErrNone)
		{
		test.Printf(_L("TestFetchingHugeFileL, ignored error while writing to file, err=%d\n"), err);
		}
	
	TInt size;
	file.Size(size);
	test.Printf(_L("Created file of size %d\n"), size);
	
	// set up attachment properties
	CCalAttachment* attachment = CCalAttachment::NewFileL(file);
	CleanupStack::PopAndDestroy(&file); // file.Close()
	CleanupStack::PushL(attachment);
	
	attachment->SetLabelL(KName());
	
	// store an entry with this attachment
	CleanupStack::Pop(attachment);
	TCalLocalUid luid = 0;
	TRAP(err, luid = StoreEntryWithAttachmentL(attachment));
	if (err != KErrNone)
		{
		test.Printf(_L("Storing attachment failed with err %d\n"), err);
		User::Leave(err);
		}
	test.Printf(_L("Stored attachment using file of size %d\n"), size);
	
	//
	
	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);

	CCalAttachment* fetchedAttachment = entry->AttachmentL(0);
	
	TTime t1, t2;
	t1.UniversalTime();

	test(fetchedAttachment->FileAttachment() != NULL);
	TRAP(err, fetchedAttachment->FileAttachment()->LoadBinaryDataL());
	if (err != KErrNone)
		{
		test.Printf(_L("TestFetchingHugeFileL, failed to fetch binary data, err=%d\n"), err);
		User::Leave(err);
		}
	
	t2.UniversalTime();
	
	TTimeIntervalMicroSeconds ms = t2.MicroSecondsFrom(t1);
	TInt milliseconds = ms.Int64() / 1000;
	test.Printf(_L("Time to fetch binary data: %d milliseconds\n"), milliseconds);
	
	test(fetchedAttachment->Value().Length() == size);
	
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::TestDeleteCalendarFileL()
	{
	// check folder exists
	TInt err = iTestLib->PIMTestServer().CheckForFile(KCalendarAttachmentsDir);
	test(err == KErrNone);
	
	// open a file and attempt to delete the calendar file - should fail
	CCalAttachmentIterator* attachments = iAttachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
	CleanupStack::PushL(attachments);
	
	CCalAttachment* att1 = attachments->NextL();
	CleanupStack::PushL(att1);
	test(att1 != NULL);
	test(att1->FileAttachment() != NULL);

	RFile file;
	CleanupClosePushL(file);
	att1->FileAttachment()->FetchFileHandleL(file); // open file handle
	test.Printf(_L("Attempting to delete cal file with open attachment\n"));
	TRAP(err, iTestLib->GetSession().DeleteCalFileL(KCalendarFile));
	test(err == KErrInUse);

	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(att1);
	CleanupStack::PopAndDestroy(attachments);

	test.Printf(_L("Attempting to delete cal file\n"));
	iTestLib->GetSession().DeleteCalFileL(KCalendarFile);

	err = iTestLib->PIMTestServer().CheckForFile(KCalendarAttachmentsDir);
	test(err == KErrNotFound);
	}
	
void CTestApp::CheckDriveAndValueL(const TDesC8& aUid, TDriveName aDriveName)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	iTestLib->SynCGetEntryViewL().FetchL(aUid, entries);
	test(aDriveName == entries[0]->AttachmentL(0)->FileAttachment()->Drive());
	entries[0]->AttachmentL(0)->FileAttachment()->LoadBinaryDataL();
	test(entries[0]->AttachmentL(0)->Value().Length()>0);
	
	CleanupStack::PopAndDestroy(&entries);
	}
	
void CTestApp::TestSameAttachmentWithDifferentDriveL()
	{
	_LIT8(KAttachEntry, 
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"DTSTART:20000221T070000Z\r\n"
	"DTEND:20000221T071500Z\r\n"
	"SUMMARY:TEST ATTACHMENT\r\n"
	"UID:AttachTest\r\n"
	"ATTACH;VALUE=CID:jsmith.part3.960817T083000.xyzMail@host1.com\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n");

	_LIT(KDriveC, "c:");
	TBuf16<2> driveX;
	
	// search for a removeable drive
	TDriveList  driveList;
	TDriveInfo  driveInfo;
	TInt nDrv=-1;
	
	//-- get drives list
	if( KErrNone!=iTestLib->FileSession().DriveList(driveList) )
      {
      test.Printf(_L("\nError in TestSameAttachmentWithDifferentDriveL (failed to get drives list)") );
      return;
      }
	
    for (nDrv=0; nDrv < KMaxDrives; nDrv++)
        {
        if(!driveList[nDrv])
            continue;   //-- skip unexisting drive

        //-- get drive info
        if( KErrNone!=iTestLib->FileSession().Drive(driveInfo, nDrv) )
            {
            test.Printf(_L("\nError: in TestSameAttachmentWithDifferentDriveL (failed to get drive info for %d)"), nDrv);
            return;   //-- can't get information about the drive
            }
        if( driveInfo.iDriveAtt & KDriveAttRemovable )
        	{
        	//-- found the first available drive to use
        	break; // for cricle
        	}
        } //-- end for circle
    
    if( nDrv >= KMaxDrives )
    	{
    	//-- we didn't found the drive
    	test.Printf(_L("Warning! Found NOT suitable drive for this step - skipping TestSameAttachmentWithDifferentDriveL\n"));
		return;
    	}

    //-- get the letter
    TChar driveChar;
    if( KErrNone != iTestLib->FileSession().DriveToChar(nDrv, driveChar) )
        {
        test.Printf(_L("\nError in TestSameAttachmentWithDifferentDriveL (failed to get drives letter from a number)") );
        return;
        }
    driveX.Append(driveChar);
    driveX.Append(_L(":"));
    
	//Copy the attach file from z: to c:
	CFileMan* fileCopier = CFileMan::NewL(iTestLib->FileSession());
	CleanupStack::PushL(fileCopier);
	InitialiseFileL(fileCopier, KFileLocation1());
	CleanupStack::PopAndDestroy(fileCopier);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iTestLib->ImportL(KAttachEntry, entries);
	RFile file;
	CCalAttachment* attach = entries[0]->AttachmentL(0);
	User::LeaveIfError(file.Open(iTestLib->FileSession(), KFileLocation1(), EFileWrite));
	CleanupClosePushL(file);
	attach->FileAttachment()->SetResourceL(file);
	CleanupStack::PopAndDestroy(&file);
	TInt success=0;
	//Store the entry with a fresh new attachment
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	_LIT8(KUid,"AttachTest");
	CheckDriveAndValueL(KUid,KDriveC());

	//Update the entry with the same attachment but changed drive
	//The attachment file should be moved to x: drive
	
	//clean all files possiblly left from the previous tests
    _LIT(KCalFolder,"\\private\\10003a5b\\");
  	TBuf<100> folder;
	folder = driveX;
	folder.Append(KCalFolder);
	CFileMan* fileMan = CFileMan::NewL(iTestLib->FileSession());
	CleanupStack::PushL(fileMan);
	TRAPD(err, fileMan->Delete(folder, CFileMan::ERecurse));
	CleanupStack::PopAndDestroy(fileMan);

	attach = entries[0]->AttachmentL(0);
	attach->FileAttachment()->SetDriveL(driveX);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	CheckDriveAndValueL(KUid,driveX);
	
	//Copy the entry into a new one and store it
	//Two entries should share the same attachment
	_LIT8(KUid1,"AttachTest1");
	HBufC8* guid1 = KUid1().AllocLC();
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid1);
	CleanupStack::Pop(guid1);
	CleanupStack::PushL(entry);
	entry->CopyFromL(*entries[0], CCalEntry::EDontCopyId);
	entries.ResetAndDestroy();
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	CheckDriveAndValueL(KUid1,driveX);

	//Copy the entry into a new one but with a different drive name of the attachment and store it
	//A new attachment should be created in a different drive
	_LIT8(KUid2,"AttachTest2");
	HBufC8* guid2 = KUid2().AllocLC();
	entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid2);
	CleanupStack::Pop(guid2);
	CleanupStack::PushL(entry);
	entry->CopyFromL(*entries[0], CCalEntry::EDontCopyId);
	entries.ResetAndDestroy();
	entries.AppendL(entry);
	CleanupStack::Pop(entry);		
	attach = entry->AttachmentL(0);
	attach->FileAttachment()->SetDriveL(KDriveC());
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	CheckDriveAndValueL(KUid2,KDriveC());
	CleanupStack::PopAndDestroy(&entries);		
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()

	{
	CTestApp* testManager = CTestApp::NewLC();
	testManager->SetFileSessionShareProtectedL();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	testManager->RunTestsL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	
	testManager = NULL;
	testManager = CTestApp::NewLC();
	testManager->RunNonShareProtectedTestsL();
	CleanupStack::PopAndDestroy(testManager);
	}

/**

@SYMTestCaseID     PIM-TCAL-ATTACH-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ATTACH-0001 Calendar Interim API Attach test suite"));

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

