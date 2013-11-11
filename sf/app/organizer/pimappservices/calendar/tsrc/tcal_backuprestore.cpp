// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <connect/sbeclient.h>
#include <e32test.h>
#include <caluser.h>
#include <calsession.h>
#include <calentry.h>
#include <calrrule.h>
#include <calentryview.h>
#include <calalarm.h>
#include <calcategory.h>
#include <calattachment.h>
#include <caliterator.h>
#include <coreappstest/testserver.h>

#include "caltestlib.h"

class CBackRestoreTestManager : public CBase, MCalChangeCallBack2, public MCalProgressCallBack
	{
public:
    static CBackRestoreTestManager* NewLC();
	~CBackRestoreTestManager();
	void ConstructL();
	
	// From MCalChangeCallBack2
    void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);

	void RunAllTestsL();
private:
    // From MCalProgressCallBack
    void Progress(TInt aPercentageCompleted);   
    void Completed(TInt aError);    
    TBool NotifyProgress();

    enum TBackupOrRestore
        {
        EBackup,
        ERestore
        };
 
    enum TRestoredFile
        {
        EFileVersion95,
        EFileVersion94,
        EFileNotSupported,
        EFileCurrupted
        };
	
	void TestBackupRestoreL(TBool aStoreEntry, TBackupOrRestore aBackupOrRestore);
	void TestMultipleCalendarBackupRestoreL(TBackupOrRestore aBackupOrRestore);
	void TestOperationsCanBePerformedL(TBackupOrRestore aBackupOrRestore);
	void TestRestoreEealyVersionCalFileL(TRestoredFile aRestoredFile);
	void TestAsynDeleteL();
	void TestNoCrashDuringRestoreL();
	
	void DeleteTestFilesL();
	void RegisterNotificationL();
	TBool CompareEntryL(RPointerArray<CCalEntry>& entriesSource, RPointerArray<CCalEntry>& entriesTocompare);

	void FetchAllEntryL(RPointerArray<CCalEntry>& aEntries);
	void StoreEntryL(TInt aNumber, CCalEntryView& aEntryView);
    CCalEntry* CreateEntryLC(HBufC8* aGulid, TBool aAddAttachment);
	void AddAlarmL(CCalEntry& aEntry);
	void AddCategoriesL(CCalEntry& aEntry);
	void AddAttendeesL(CCalEntry& aEntry);
	void AddFileAttachmentL(CCalEntry& aEntry);

private:
	CCalTestLibrary*                       iTestLib;
	TBool                                  iNotificationCallBackOccurred;
	MCalChangeCallBack2::TChangeType       iNotificationChangeType;
	TInt                                   iDeleteCompleteErrCode;
	};



static RTest test(_L("tcal_backrestore"));
static conn::CSBEClient* BackupClient;
static TDriveList DriveList;


_LIT8(KGuid, "A_Global_UID");
_LIT(KCalFile, "test_backrestore_calfile");
_LIT(KCalSecondFile, "test_backrestore_calSecondfile");
_LIT(KAttachFile, "c:\\calendar_attachments\\tcal_attach.jpg");
_LIT(KCalenderServerPath, "C:\\private\\10003a5b\\");
_LIT(KTestFilePath, "C:\\calendarbackuprestortest\\");

_LIT(KCalfile95, "z:\\private\\10003a5b\\tcal_dc_95");
_LIT(KCalfile94, "z:\\private\\10003a5b\\tcal_dc_94");
_LIT(KCalfileNotSupported, "z:\\private\\10003a5b\\tcal_dc_81");
_LIT(KCalfileCorrupt, "z:\\private\\10003a5b\\tcal_dc_92_corrupt");

const TInt KNumerOfEntry = 100;

// building block test backup and restore functions

static void StartBackupL()
    {
    //Use the Secure Backup Engine API to "start" a backup.  
    test.Printf(_L("Starting Backup\n"));
    BackupClient->SetBURModeL(DriveList, conn::EBURBackupFull, conn::EBackupBase);
    }

static void EndBackupL()
    {
    // Use the Secure Backup Engine API to "end" a backup.
    test.Printf(_L("Finishing Backup\n"));
    BackupClient->SetBURModeL(DriveList, conn::EBURNormal, conn::ENoBackup);
    }

static void StartRestoreL()
    {
    // Use the Secure Backup Engine API to "start" a restore.      
    test.Printf(_L("Starting restore\n"));
    BackupClient->SetBURModeL(DriveList, conn::EBURRestoreFull, conn::EBackupBase);
    }

static void EndRestoreL()
    {
    // Use the Secure Backup Engine API to "end" a restore.   
    test.Printf(_L("Finishing restore\n"));
    BackupClient->SetBURModeL(DriveList, conn::EBURNormal, conn::ENoBackup);
    }

CBackRestoreTestManager* CBackRestoreTestManager::NewLC()
	{
	CBackRestoreTestManager* self = new (ELeave) CBackRestoreTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}
	
void CBackRestoreTestManager::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalFile);
	iTestLib->OpenFileL(KCalFile);
	}	
	
CBackRestoreTestManager::~CBackRestoreTestManager()	
	{
	TRAP_IGNORE(iTestLib->DeleteFileL(KAttachFile, EFalse));
	TRAP_IGNORE(DeleteTestFilesL());
	delete iTestLib;
	}

void CBackRestoreTestManager::Progress(TInt aPercentageCompleted)
    {
    test.Printf(_L("DeleteL is %d%% complete\n"), aPercentageCompleted);
    }
    
void CBackRestoreTestManager::Completed(TInt aError)
    {
    test.Printf(_L("DeleteL has completed with error code %d\n"), aError);
    iDeleteCompleteErrCode = aError;
    }
    
TBool CBackRestoreTestManager::NotifyProgress()
    {
    return ETrue;
    }

void CBackRestoreTestManager::DeleteTestFilesL()
    {
    CFileMan* fileCopier=NULL;
    fileCopier = CFileMan::NewL(iTestLib->FileSession());
    fileCopier->RmDir(KTestFilePath());
    if(fileCopier)
         {
         delete fileCopier;
         }
    }

void CBackRestoreTestManager::RegisterNotificationL()
    {
    RDebug::Print(_L("Register Notication" ));
    TCalTime minTime;
    minTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime maxTime;
    maxTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange timeRange(minTime, maxTime);
    CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, timeRange);
    iTestLib->GetSession().StartChangeNotification(*this, *filter);
    delete filter;
    }

void CBackRestoreTestManager::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
    {
    test.Printf(_L("MCalChangeCallBack2 notification happened\n") );
    iNotificationCallBackOccurred = ETrue;

    //There is only one change in the array and it is backup\restore notification.
    test(aChangeItems.Count() == 1);
    TCalChangeEntry change = aChangeItems[0];
    iNotificationChangeType = aChangeItems[0].iChangeType;
    CActiveScheduler::Stop();
    }

void CBackRestoreTestManager::AddFileAttachmentL(CCalEntry& aEntry)
    {
//1. copy the attachment file from z:\\calendar_attachments\\tcal_attach.jpg tp c:\\calendar_attachments\\tcal_attach.jpg
    _LIT8(KMimeType, "image/jpeg");
    _LIT(KLabel, "snowman");
    _LIT(KOriginalDrive, "z:");
    
    TInt err = iTestLib->FileSession().MkDirAll(KAttachFile);
    if (err != KErrAlreadyExists)
         {
         User::LeaveIfError(err);
         }
         
    // required for passing attachment file handles to server
    User::LeaveIfError(iTestLib->FileSession().ShareProtected()); 
    
    CFileMan* fileCopier = CFileMan::NewL(iTestLib->FileSession());
    CleanupStack::PushL(fileCopier);
    
    TFileName initialLocationOfFile = KAttachFile();
    initialLocationOfFile.Replace(0, 2, KOriginalDrive());
    
    // get current time
    TTime now;
    now.HomeTime();
    
    // clear any read only attribute if the file is present such we avoid errors at copy time
    fileCopier->Attribs(KAttachFile, 0, KEntryAttReadOnly, now);
    
    // copy the file from Z: drive to C: drive (ROM to RAM)
    User::LeaveIfError(fileCopier->Copy(initialLocationOfFile, KAttachFile));
    
    // clear any read only attribute such we avoid errors at open time (true on real hardware)
    User::LeaveIfError(fileCopier->Attribs(KAttachFile, 0, KEntryAttReadOnly, now));   
    CleanupStack::PopAndDestroy(fileCopier);
 
//2. Set the file to the attachment
    RFile file;
    User::LeaveIfError(file.Open(iTestLib->FileSession(), KAttachFile(), EFileWrite));
    CleanupClosePushL(file);
    CCalAttachment* attachment = CCalAttachment::NewFileL(file);
    CleanupStack::PopAndDestroy(&file); // file.Close()
    CleanupStack::PushL(attachment);
    attachment->SetMimeTypeL(KMimeType());
    attachment->SetLabelL(KLabel());
 
//3. Add attachment to the entry
    aEntry.AddAttachmentL(*attachment);
    CleanupStack::Pop(attachment);
    }

void CBackRestoreTestManager::AddCategoriesL(CCalEntry& aEntry)
    {
    _LIT(KDummyCategoryName, "new category");
      
    // add categories and attendees to 1 in 10 entries
    CCalCategory* category1 = CCalCategory::NewL(KDummyCategoryName);
    CleanupStack::PushL(category1);   
    CCalCategory* category2 = CCalCategory::NewL(CCalCategory::ECalHoliday);
    CleanupStack::PushL(category2);     
    aEntry.AddCategoryL(category1); 
    aEntry.AddCategoryL(category2);    
    CleanupStack::Pop(category2);
    CleanupStack::Pop(category1);
    }

void CBackRestoreTestManager::AddAttendeesL(CCalEntry& aEntry)
    {
    _LIT(KDummyAddress1, "dummy address 1");
    _LIT(KDummyAddress2, "dummy address 2");
    _LIT(KDummyAddress3, "dummy address 3");
    _LIT(KDummyAddress4, "dummy address 4");
    _LIT(KDummyAddress5, "dummy address 5");
    _LIT(KDummySentBy1, "dummy sent by 1");
    _LIT(KDummySentBy2, "dummy sent by 2");
    
    // add attendees to 1 in 10 entries
    
    CCalAttendee* attendee1 = CCalAttendee::NewL(KDummyAddress1);
    CleanupStack::PushL(attendee1);  
    CCalAttendee* attendee2 = CCalAttendee::NewL(KDummyAddress2);
    CleanupStack::PushL(attendee2);  
    CCalAttendee* attendee3 = CCalAttendee::NewL(KDummyAddress3);
    CleanupStack::PushL(attendee3); 
    CCalAttendee* attendee4 = CCalAttendee::NewL(KDummyAddress4, KDummySentBy1);
    CleanupStack::PushL(attendee4);
    CCalAttendee* attendee5 = CCalAttendee::NewL(KDummyAddress5, KDummySentBy2);
    CleanupStack::PushL(attendee5);
    aEntry.AddAttendeeL(attendee5); 
    CleanupStack::Pop(attendee5);
    aEntry.AddAttendeeL(attendee4); 
    CleanupStack::Pop(attendee4);
    aEntry.AddAttendeeL(attendee3); 
    CleanupStack::Pop(attendee3);
    aEntry.AddAttendeeL(attendee2); 
    CleanupStack::Pop(attendee2);
    aEntry.AddAttendeeL(attendee1);
    CleanupStack::Pop(attendee1);
    }

void CBackRestoreTestManager::AddAlarmL(CCalEntry& aEntry)
    {
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(1);
         
    CCalContent* almContent = CCalContent::NewL();
    CleanupStack::PushL(almContent);
    // Add content and mimetype for the alarm.
    _LIT8(KContent, "C:\\test.jpg");
    _LIT8(KMimeType, "mime type");
    HBufC8* content = KContent().AllocLC();
    HBufC8* mimetype = KMimeType().AllocLC();
         
    // Takes ownership of content and mimetype.
    almContent->SetContentL(content, mimetype, CCalContent::EDispositionInline);
    CleanupStack::Pop(mimetype);
    CleanupStack::Pop(content);    
   
    alarm->SetAlarmAction(almContent); // Takes ownership of almContent.
    CleanupStack::Pop(almContent); 
        
    aEntry.SetAlarmL(alarm);
         
    CleanupStack::PopAndDestroy(alarm);
    }

CCalEntry* CBackRestoreTestManager::CreateEntryLC(HBufC8* aGulid, TBool aAddFileAttachment)
    {
    CleanupStack::PushL(aGulid);
    CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, aGulid);
    CleanupStack::Pop(aGulid);
    CleanupStack::PushL(entry); 
  
    //Set entry properties
    _LIT(KText, "bakuprestor_test_text");
    entry->SetSummaryL(KText());
    entry->SetLocationL(KText());
    entry->SetDescriptionL(KText());
    
    TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
    TCalTime calStartTime;
    calStartTime.SetTimeLocalL(startTime);
    TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
    TCalTime calEndTime;
    calEndTime.SetTimeLocalL(endTime);
    entry->SetStartAndEndTimeL(calStartTime, calEndTime);
    TCalRRule rpt(TCalRRule::EDaily);
    rpt.SetDtStart(calStartTime);
    rpt.SetCount(5);
    rpt.SetInterval(1);
    entry->SetRRuleL(rpt);//take a copy of the rule
    AddAlarmL(*entry);
    AddCategoriesL(*entry);
    AddAttendeesL(*entry);
    if(aAddFileAttachment)
        {
        AddFileAttachmentL(*entry);
        }
    return entry;
    }

void CBackRestoreTestManager::StoreEntryL(TInt aNumber, CCalEntryView& aEntryView)
    {
    test.Printf(_L("Store Entries\n"));
    RPointerArray<CCalEntry> entriesToAdd;
    CleanupResetAndDestroyPushL(entriesToAdd);
    
    for(TInt ii=0; ii<aNumber; ++ii)
        { 
        TBool addAttachment = EFalse;
        TBuf8<64> uid;
        uid.Copy(KGuid());
        uid.AppendNum(ii);
        HBufC8* guid = uid.AllocL();
        if(ii%20 == 0)
             {
             addAttachment = ETrue;
             }
   
       CCalEntry* entry = CreateEntryLC(guid, addAttachment);
       entriesToAdd.AppendL(entry);
       CleanupStack::Pop(entry);        
       TInt success = 0;
       aEntryView.StoreL(entriesToAdd, success);
       entriesToAdd.ResetAndDestroy();
       }
    
    CleanupStack::PopAndDestroy(&entriesToAdd); 
    }
/*
@SYMTestCaseID              PIM-TCAL-backuprestore-0001
@SYMPreq                    CR2033
@SYMTestCaseDesc            Basic backup and restore test
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions              
  1. Make sure that the Calendar file can be copied when backup or restore happens
  2. Make sure that the observer  (this) is notified when backup or restore event happens
  3. Make sure that the restored file contains same entries as expected.
                  
@SYMTestExpectedResults     The above as described is satisfied
**/
void CBackRestoreTestManager::TestBackupRestoreL(TBool aStoreEntry, TBackupOrRestore aBackupOrRestore)
    {
    test.Printf(_L("Test Backup Restore\n"));
    if(aStoreEntry)
        {
        iTestLib->CleanDatabaseL();
        StoreEntryL(KNumerOfEntry, iTestLib->SynCGetEntryViewL());
        }
    //entriesBefore are those entries stored in the Calendar file before backup or restored happens
    RPointerArray<CCalEntry> entriesBefore;
    CleanupResetAndDestroyPushL(entriesBefore);   
    FetchAllEntryL(entriesBefore);
       
    //Register so that it is notified when backup\restore starts\ends
    RegisterNotificationL();
    
    //Start backup or restore process
   if(aBackupOrRestore ==  EBackup)
        {
        StartBackupL();
        }
    else
        {
        StartRestoreL();
        }
    CActiveScheduler::Start();
    
    // Test the notification has occured
    test(iNotificationCallBackOccurred);
    if(aBackupOrRestore ==  EBackup)
        {
        test(iNotificationChangeType==MCalChangeCallBack2::EBackupStart);
        //Simulate the backup process and make sure the file can be copied.
        iTestLib->PIMTestServer().CopyDirAndFilesL(KCalenderServerPath(), KTestFilePath());    
        }
    else
        {
        test(iNotificationChangeType==MCalChangeCallBack2::ERestoreStart);
        //Simulate the restore process and make sure the file can be copied.
        iTestLib->PIMTestServer().CopyDirAndFilesL( KTestFilePath(), KCalenderServerPath());    
        }
    
    iNotificationCallBackOccurred = EFalse;
    RPointerArray<CCalEntry> entriesAfter;
    CleanupResetAndDestroyPushL(entriesAfter);   

    // Test that the calendar file has been locked
    TRAPD(err, FetchAllEntryL(entriesAfter));
    test(err == KErrLocked);
    test(entriesAfter.Count()==0); 
   
    //Ends the backup or restore
    if(aBackupOrRestore ==  EBackup)
        {
        EndBackupL();
        }
    else
        {
        EndRestoreL();
        }
    CActiveScheduler::Start();
    
    // Test the notification has occured
    if(aBackupOrRestore ==  EBackup)
         {
         test(iNotificationChangeType==MCalChangeCallBack2::EBackupEnd);
         }
     else
         {
         test(iNotificationChangeType==MCalChangeCallBack2::ERestoreEnd);
         }
    
    //Test the file is unlocked and entries fetched are those stored in the restored file.
    FetchAllEntryL(entriesAfter);
    test(entriesAfter.Count()==KNumerOfEntry);
    test(CompareEntryL(entriesBefore,entriesAfter));
    CleanupStack::PopAndDestroy(&entriesAfter);
    CleanupStack::PopAndDestroy(&entriesBefore);
    iTestLib->GetSession().StopChangeNotification();
    }

void CBackRestoreTestManager::TestNoCrashDuringRestoreL()
    {
    test.Printf(_L("Test calendar does not crash if a second session tries to open the file being restored\n"));
    iTestLib->CleanDatabaseL();
    StoreEntryL(KNumerOfEntry, iTestLib->SynCGetEntryViewL());
    
    //Register so that it is notified when backup\restore starts\ends
    RegisterNotificationL();
    
    //Start restore process
    StartRestoreL();
    
    CActiveScheduler::Start();
    
    // Test the notification has occured
    test(iNotificationCallBackOccurred);
    
    CCalSession* secondSession = CCalSession::NewL();
    CleanupStack::PushL(secondSession);
    TRAPD(err, secondSession->OpenL(KCalFile));
    if(err != KErrLocked)
        {
        User::Leave(err);
        }
       
    EndRestoreL();
    CActiveScheduler::Start();
    
    TRAP(err, secondSession->OpenL(KCalFile));
    if(err != KErrNone)
        {
        User::Leave(err);
        }
    CleanupStack::PopAndDestroy(secondSession);
    iTestLib->GetSession().StopChangeNotification();
    }
/**
@SYMTestCaseID              PIM-TCAL-backuprestore-0002
@SYMPreq                    CR2033
@SYMTestCaseDesc            Test that asynchronous deleting is being carried out when restore is in progress
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions              
  1. Make sure that the asynchronous operation can be interruppted by restore event.
  2. Make sure that the file can be restored when asnchronous deleting is being carried out.
                  
@SYMTestExpectedResults     TThe above as described is satisfied
**/
void CBackRestoreTestManager::TestAsynDeleteL()
    {
    test.Printf(_L("Test Asynchronous Delete\n"));
  
    //Prepare the fro the test
    // - Store some entries 
    // - Fetch entries for comparison later.
    // - Save the file so that it can be restored later.
    iTestLib->CleanDatabaseL();
    StoreEntryL(KNumerOfEntry, iTestLib->SynCGetEntryViewL());
    iTestLib->CloseAgendaServer();
    iTestLib->PIMTestServer().CopyDirAndFilesL(KCalenderServerPath(), KTestFilePath());
    iTestLib->OpenFileL(KCalFile);
    RPointerArray<CCalEntry> entriesBefore;
    CleanupResetAndDestroyPushL(entriesBefore);   
    FetchAllEntryL(entriesBefore);
 
    // Carry out the asynchronous deletion operation
    TCalTime startTime;
    TCalTime endTime;
    startTime.SetTimeLocalL(TCalTime::MinTime());
    endTime.SetTimeLocalL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange range(startTime,endTime);
    iTestLib->SynCGetEntryViewL().DeleteL(range, CalCommon::EIncludeAll, *this); 

    //Start the restore  
    RegisterNotificationL();
    StartRestoreL();
    CActiveScheduler::Start();
    test(iNotificationCallBackOccurred);
    test(iNotificationChangeType==MCalChangeCallBack2::ERestoreStart);
    //Deletion could be completed or aborted.
    test(iDeleteCompleteErrCode == KErrCancel || iDeleteCompleteErrCode == KErrNone);
    // Retore the original calendar file (with KNumerOfEntry entries)
    iTestLib->PIMTestServer().CopyDirAndFilesL( KTestFilePath(), KCalenderServerPath());

    RPointerArray<CCalEntry> entriesAfter;
    CleanupResetAndDestroyPushL(entriesAfter);   
   
    TRAPD(err, FetchAllEntryL(entriesAfter));
    test(err == KErrLocked);
    test(entriesAfter.Count()==0);
   
    //Ends the backup and test the notification
    EndRestoreL();
    CActiveScheduler::Start();
    test(iNotificationChangeType==MCalChangeCallBack2::ERestoreEnd);
    
    //test the file is unlocked.
    FetchAllEntryL(entriesAfter);
    test(entriesAfter.Count()==KNumerOfEntry);
    //The entries should be those in restored file.
    test(CompareEntryL(entriesBefore,entriesAfter));
    CleanupStack::PopAndDestroy(&entriesAfter);
    CleanupStack::PopAndDestroy(&entriesBefore);
    iTestLib->GetSession().StopChangeNotification();
    iNotificationCallBackOccurred = EFalse;
    }

/*
@SYMTestCaseID              PIM-TCAL-backuprestore-0003
@SYMPreq                    CR2033
@SYMTestCaseDesc            Test the backup or restore when there are more than one Calendar files held in the calendar server
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions              
 * 1. Make sure that the Calendar file can be copied when backup or restore happens
 * 2. Make sure that  observer  (this) is notified when backup or restore event happens
 * 3. Make sure that the restored file contains same entries as expected.
                  
@SYMTestExpectedResults     The above as described is satisfied
**/
void CBackRestoreTestManager::TestMultipleCalendarBackupRestoreL(TBackupOrRestore aBackupOrRestore)
    {
    test.Printf(_L("Test there are multiple files when backup or restore is in progress\n"));
  
  //Prepare for the test
    //Create the second file and open it
    CCalSession* secondSession = CCalSession::NewL(iTestLib->GetSession()); 
    CleanupStack::PushL(secondSession);
    TRAPD(err, secondSession->CreateCalFileL(KCalSecondFile()));
    if (err == KErrAlreadyExists)
        {
        secondSession->DeleteCalFileL(KCalSecondFile());    
        secondSession->CreateCalFileL(KCalSecondFile());
        }
    secondSession->OpenL(KCalSecondFile());
    CCalEntryView* secondView =CCalEntryView::NewL(*secondSession);
    CleanupStack::PushL(secondView);
   
    //Store some entries in both calendar files
    iTestLib->CleanDatabaseL();
    StoreEntryL(KNumerOfEntry/2, iTestLib->SynCGetEntryViewL());
    StoreEntryL(KNumerOfEntry/2, *secondView);
    
    //Fetch entries in both calendar for comparison later
    RPointerArray<CCalEntry> entriesBefore1;
    CleanupResetAndDestroyPushL(entriesBefore1);   
    FetchAllEntryL(entriesBefore1);
    
    RPointerArray<CCalEntry> entriesBefore2;
    CleanupResetAndDestroyPushL(entriesBefore2);   
    FetchAllEntryL(entriesBefore2);
    
    //Start backup or restore and test the notification
    RegisterNotificationL();
    if(aBackupOrRestore ==  EBackup)
        {
        StartBackupL();
        }
    else
        {
        StartRestoreL();
        }
    CActiveScheduler::Start();
    test(iNotificationCallBackOccurred);
    if(aBackupOrRestore == EBackup)
        {
        test(iNotificationChangeType==MCalChangeCallBack2::EBackupStart);
        // Make sure the file can be copied.
        iTestLib->PIMTestServer().CopyDirAndFilesL(KCalenderServerPath(), KTestFilePath());    
        }
    else
        {
        test(iNotificationChangeType==MCalChangeCallBack2::ERestoreStart);
        iTestLib->PIMTestServer().CopyDirAndFilesL(KTestFilePath(), KCalenderServerPath());    
        }
    iNotificationCallBackOccurred = EFalse;
    
    // test that the calendar file has been loced
    RPointerArray<CCalEntry> entriesAfter1;
    CleanupResetAndDestroyPushL(entriesAfter1);   
    RPointerArray<CCalEntry> entriesAfter2;
    CleanupResetAndDestroyPushL(entriesAfter2);   

    TRAP(err, FetchAllEntryL(entriesAfter1));
    test(err == KErrLocked);
    TRAP(err, FetchAllEntryL(entriesAfter2));
    test(err == KErrLocked);
    test(entriesAfter1.Count()==0);
    test(entriesAfter2.Count()==0);   
      
    //Ends the backup or restore and test the notification
    if(aBackupOrRestore ==  EBackup)
        {
        EndBackupL();
        }
    else
        {
        EndRestoreL();
        }
    CActiveScheduler::Start();
    if(aBackupOrRestore ==  EBackup)
         {
         test(iNotificationChangeType==MCalChangeCallBack2::EBackupEnd);
         }
     else
         {
         test(iNotificationChangeType==MCalChangeCallBack2::ERestoreEnd);
         }
    
    //Test the file are unlocked.
    FetchAllEntryL(entriesAfter1);
    FetchAllEntryL(entriesAfter2);
    //Entries are those in the current file (backup test) or restored file (retore test).
    test(entriesAfter1.Count()==KNumerOfEntry/2);
    test(CompareEntryL(entriesBefore1,entriesAfter1));
    test(entriesAfter2.Count()==KNumerOfEntry/2);
    test(CompareEntryL(entriesBefore2,entriesAfter2));
 
    CleanupStack::PopAndDestroy(&entriesAfter2);
    CleanupStack::PopAndDestroy(&entriesAfter1);
    CleanupStack::PopAndDestroy(&entriesBefore2);
    CleanupStack::PopAndDestroy(&entriesBefore1);
    CleanupStack::PopAndDestroy(secondView);
    secondSession->DeleteCalFileL(KCalSecondFile()); 
    CleanupStack::PopAndDestroy(secondSession);
    iTestLib->GetSession().StopChangeNotification();
    }
/*
@SYMTestCaseID              PIM-TCAL-backuprestore-0004
@SYMPreq                    CR2033
@SYMTestCaseDesc            Test the operations which are not affected by a backup or restore event
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions              
 * Most of operations are not permitted when backup or restore happens. However, some of operations
 * such as notification request, enabling broadcast and closing calenda should
 * be permitted when backup or restore happens.
                  
@SYMTestExpectedResults     The above as described is satisfied
**/
void CBackRestoreTestManager::TestOperationsCanBePerformedL(TBackupOrRestore aBackupOrRestore)
    {
    test.Printf(_L("Test the operations which can be performed when backup or restore happens\n"));
    
    RegisterNotificationL();
    if(aBackupOrRestore == EBackup)
        {
        StartBackupL();
        }
    else
        {
        StartRestoreL();
        }
    CActiveScheduler::Start();
    
    RegisterNotificationL(); 
    //The following operation should be successful when backup or restore happens
    iTestLib->GetSession().StopChangeNotification();
    iTestLib->GetSession().EnableChangeBroadcast();
    iTestLib->GetSession().DisableChangeBroadcast();
    iTestLib->CloseAgendaServer();
    
    if(aBackupOrRestore == EBackup)
        {
        EndBackupL();
        }
    else
        {
        EndRestoreL();
        }
    
    DeleteTestFilesL();
    User::After(1000000);//Make sure backup\restore ends before opening the file. We can't use the notification because the file is closed.
    iTestLib->OpenFileL(KCalFile);//Open the file so that it comes back the test condition before it starts
    }

/*
@SYMTestCaseID              PIM-TCAL-backuprestore-0005
@SYMPreq                    CR2033
@SYMTestCaseDesc            Test that an earlier version Calendar file is retored.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions              
 * 1. Make sure that a compatible early version Calendar file can be restored and opened.
 * 2. Make sure that a correct notification is received when restoring an incompatible early version of Calendar file.
 * 3. Make sure that a correct notification is received when the restored file is corrupted.
 * 4. Make sure that an non-usable session can be closed and an non-usable file can be deleted.
                  
@SYMTestExpectedResults     The above as described is satisfied
**/
void CBackRestoreTestManager::TestRestoreEealyVersionCalFileL(TRestoredFile aRestoredFile)
    {
    test.Printf(_L("Test Restore an Eealy Version Calendar FileL\n"));
    
    //Prepare the test: Save the current file to the testing directory so that it can be copied back.
    DeleteTestFilesL();
    iTestLib->CloseAgendaServer();
    iTestLib->PIMTestServer().CopyDirAndFilesL(KCalenderServerPath(), KTestFilePath());
    TRAPD(err,iTestLib->OpenFileL(KCalFile));
    iTestLib->SynCGetEntryViewL();
    TFileName currentFile = KCalenderServerPath();
    currentFile.Append(KCalFile());
    
    //Start the restore event
    RegisterNotificationL();
    StartRestoreL();
    CActiveScheduler::Start();
    //copy an earlly version calendar file to the current calendar file
    TBool restoredGoodFile = ETrue;
    switch (aRestoredFile)
        {
        case EFileVersion95:
            iTestLib->PIMTestServer().CopyFileL(KCalfile95, currentFile);          
            break;
        case EFileVersion94:
            iTestLib->PIMTestServer().CopyFileL(KCalfile94, currentFile);
            break;
        case EFileNotSupported:
            iTestLib->PIMTestServer().CopyFileL(KCalfileNotSupported, currentFile);
            restoredGoodFile = EFalse;
            break;
        case EFileCurrupted:
            iTestLib->PIMTestServer().CopyFileL(KCalfileCorrupt, currentFile);
            restoredGoodFile = EFalse;
            break;
        default:
            break;
        }
      
    EndRestoreL();
    CActiveScheduler::Start();
    test(iNotificationCallBackOccurred);
    
    //Test the correct notification is received depend on whether the file is valid or not
    if(restoredGoodFile)
        {
        test(iNotificationChangeType==MCalChangeCallBack2::ERestoreEnd); 
        RPointerArray<CCalEntry> entries;
        CleanupResetAndDestroyPushL(entries);   
        FetchAllEntryL(entries);
        test(entries.Count()>0);
        CleanupStack::PopAndDestroy(&entries);
        }
    else
        {
        test(iNotificationChangeType==MCalChangeCallBack2::ERestoredFileCanNotBeOpened); 
        }
    
    // Make sure the file can be deleted and server can be closed whatever the restored file is good one or not.
    iTestLib->GetSession().DeleteCalFileL(KCalFile);
    iTestLib->CloseAgendaServer();
    
    //Copied the original file to the current file.
    iTestLib->PIMTestServer().CopyDirAndFilesL(KTestFilePath, KCalenderServerPath());
    iTestLib->OpenFileL(KCalFile);//Open the file so that it comes back the test condition before it starts
    iNotificationCallBackOccurred = EFalse;
    }

void CBackRestoreTestManager::FetchAllEntryL(RPointerArray<CCalEntry>& aEntries)
    {
    CCalIter& iter = iTestLib->GetIterL();
    for (TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()))
        {
        iTestLib->SynCGetEntryViewL().FetchL(Uid, aEntries);
        }
    
   const TInt count = aEntries.Count();
   for (TInt ii=0; ii< count; ii++)
       {//Make sure offline properties are loaded 
       aEntries[ii]->SummaryL();
       aEntries[ii]->DescriptionL();
       aEntries[ii]->LocationL();
       CCalAlarm* alarm = aEntries[ii]->AlarmL();
       delete alarm;
       }
   }

TBool CBackRestoreTestManager::CompareEntryL(RPointerArray<CCalEntry>& entriesSource, RPointerArray<CCalEntry>& entriesTocompare)
    {
    TInt sourceCount = entriesSource.Count();
    TInt CompareCount = entriesTocompare.Count();
    if( sourceCount!= CompareCount)
        {
        return EFalse;
        }
    //Make a copy of two entry arraies to make the comparison easier without changing the arraies past in.
    RPointerArray<CCalEntry> sourceCopy;
    CleanupClosePushL(sourceCopy);
   
    RPointerArray<CCalEntry> compareCopy;
    CleanupClosePushL(compareCopy);
   
    for (TInt count = 0; count < sourceCount; count++)
        {
        sourceCopy.AppendL( entriesSource[count]);
        compareCopy.AppendL( entriesSource[count]);
        }
   
    TBool same = EFalse;
     
    for (TInt i = 0; i < sourceCount; i++)
        {
        same = EFalse;
        TInt j = 0;
        CompareCount = compareCopy.Count();
        while (!same && j<CompareCount)
          {
          if (compareCopy[j]->CompareL(*sourceCopy[i]))
              {
              same = ETrue;
              compareCopy.Remove(j);   
              }
          ++j;
          }
        
        if(!same)
            {
            break;
            }
        }
      
    CleanupStack::PopAndDestroy(&compareCopy);
    CleanupStack::PopAndDestroy(&sourceCopy);

    return same;
    }

void CBackRestoreTestManager::RunAllTestsL()
    { 
    //Basic tests - Store some entries and test that the entries can not be fetched when backup\retore is in progress.
    test.Printf(_L("Basic tests - test backup\n"));
    TestBackupRestoreL(ETrue, EBackup);
    test.Printf(_L("Basic tests - test restore\n"));
    TestBackupRestoreL(ETrue, ERestore);
 
    //Smilar to basic tests but both Calendar file and it's index file exist when backup\retore is in progress.
    test.Printf(_L("close the calendar server\n"));
    iTestLib->CloseAgendaServer();
    DeleteTestFilesL();
    iTestLib->OpenFileL(KCalFile);
    test.Printf(_L("test backup after close and reopen a file\n"));
    TestBackupRestoreL(EFalse, EBackup);
    test.Printf(_L("test restore after close and reopen a file\n"));
    TestBackupRestoreL(EFalse, ERestore);
    
    //Test that some operations are not affected by backup\restore.
    TestOperationsCanBePerformedL(EBackup);
    TestOperationsCanBePerformedL(ERestore);
 
    //Test that asynchronous deletion is being carried out when backup\restore is in progress.
    DeleteTestFilesL();
    TestAsynDeleteL();
 
    //Test that an earlier version Calendar file is retored.
    TestRestoreEealyVersionCalFileL(EFileVersion95);
    TestRestoreEealyVersionCalFileL(EFileVersion94);
    TestRestoreEealyVersionCalFileL(EFileCurrupted);
    TestRestoreEealyVersionCalFileL(EFileNotSupported);
    
    // Similar to the basic tests but there are more than one Calendar files are involved.
    DeleteTestFilesL();
    TestMultipleCalendarBackupRestoreL(EBackup); 
    TestMultipleCalendarBackupRestoreL(ERestore);
    TestNoCrashDuringRestoreL();
    }
	
static void DoTestL()
	{
	BackupClient = conn::CSBEClient::NewL();
	CleanupStack::PushL(BackupClient);

	CBackRestoreTestManager* testManager = CBackRestoreTestManager::NewLC();
	TPerformanceTimer timer(test);

	testManager->RunAllTestsL();	

	test.Printf(_L("Done\n"));
	
	CleanupStack::PopAndDestroy(testManager);
	CleanupStack::PopAndDestroy(BackupClient);
	}

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-BACKUPRESTORE-0001 Calendar tcal_backuprestore test suite"));
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
	test.Printf(_L("ret is:%d\n"),ret);
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
