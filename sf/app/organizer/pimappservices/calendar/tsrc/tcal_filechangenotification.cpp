// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <calsession.h>
#include <calentryview.h>
#include <e32test.h>
#include <calchangecallback.h>
#include <calfilechangenotification.h>
#include <calrrule.h>
#include <calcalendarinfo.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <tzusernames.h>
#include <tzuserdefineddata.h>
#endif

_LIT(KTestName,"tcal_filechangenotification");
RTest test(KTestName);
_LIT(KTestFileName,"c:tcal_filechangenotification");


class CCalFileChangeNotificationTest : public CBase, MCalChangeCallBack2, MCalFileChangeObserver
    {
public:
    static CCalFileChangeNotificationTest* NewLC();
    ~CCalFileChangeNotificationTest();
    
    // from MCalChangeCallBack2
    void CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);

    // from MCalChangeCallBack2
    void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
    //Test tasks
    void TestFileCreatedL();
    void TestFileDeletedL();
    void TestFileChangeWithCalendarInfoL();
    void TestUpdateCalendarInfoL();
    void TestMixedNotificationL();
 
private: 
    CCalFileChangeNotificationTest();
    void ConstructL();
    
    TCalLocalUid StoreEntryL();
    void RegisterEntryNotificationL();
    TBool CompareFileNotification(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries);
    TBool CompareEntryNotification(RArray<TCalChangeEntry>& aChangeItems);
  
private:
    TBool               iNotificationCallBackOccurred;
    TBool               iFileNotificationCallBackOccurred;
    CCalTestLibrary*    iTestLib;
    CCalSession*        iOtherSession;//It does the change such as create file, add calendar info...
    RArray<MCalFileChangeObserver::TChangeType> iExpectedCalendarInfoTypes;
    RArray<TCalChangeEntry> iExpectedEntryChange;
    };

CCalFileChangeNotificationTest::CCalFileChangeNotificationTest()
    :iNotificationCallBackOccurred(EFalse), iFileNotificationCallBackOccurred(EFalse)
    {
    }

void CCalFileChangeNotificationTest::CalendarInfoChangeNotificationL(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
    {
    test.Printf(_L("MCalFileChangeObserver notification happened\n") );
    iFileNotificationCallBackOccurred = ETrue;
    
    test(CompareFileNotification(aCalendarInfoChangeEntries));
    if(iExpectedCalendarInfoTypes.Count() == 0 && iExpectedEntryChange.Count() == 0)
        {
        CActiveScheduler::Stop();
        }
    }

TBool CCalFileChangeNotificationTest::CompareFileNotification(RPointerArray<CCalFileChangeInfo>& aCalendarInfoChangeEntries)
    {//return ETrue if all elements in aCalendarInfoChangeEntries are found in the iExpectedCalendarInfoTypes otherwise EFalse.
    TBool ret = EFalse;
    const TInt count = aCalendarInfoChangeEntries.Count();
    TInt jj = 0;
    do
        {
        CCalFileChangeInfo* calFileChangeInfo = aCalendarInfoChangeEntries[jj];
        ret = EFalse;
        TInt countExpected = iExpectedCalendarInfoTypes.Count();
        for(TInt ii = countExpected-1; ii >= 0; --ii)
            {
            if(calFileChangeInfo->FileNameL() == KTestFileName() && calFileChangeInfo->ChangeType() == iExpectedCalendarInfoTypes[ii])
                 {
                 iExpectedCalendarInfoTypes.Remove(ii);
                 ret = ETrue;
                 countExpected = iExpectedCalendarInfoTypes.Count();
                 break;
                 }
            }
        
        ++ jj;
        }while (ret && jj < count);
            
    return ret;
    }

TBool CCalFileChangeNotificationTest::CompareEntryNotification(RArray<TCalChangeEntry>& aChangeItems)
    {//return ETrue if all elements in aChangeItems are found in the iExpectedEntryChange otherwise EFalse.
    TBool ret = EFalse;
    const TInt count = aChangeItems.Count();
    TInt jj = 0;
    do
        {
        TCalChangeEntry entryChange = aChangeItems[jj];
        TBool found = EFalse;
         TInt countExpected = iExpectedEntryChange.Count();
        for(TInt ii = countExpected-1; ii >= 0; --ii)
            {
            if(entryChange.iEntryId == iExpectedEntryChange[ii].iEntryId &&
               entryChange.iChangeType == iExpectedEntryChange[ii].iChangeType &&
               entryChange.iEntryType == iExpectedEntryChange[ii].iEntryType)
                 {
                 iExpectedEntryChange.Remove(ii);
                 ret = ETrue;
                 countExpected = iExpectedEntryChange.Count();
                 break;
                 }
            }
        
        ++ jj;
        }while (ret && jj < count);
            
    return ret;
    }

void CCalFileChangeNotificationTest::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
    {
    test.Printf(_L("MCalChangeCallBack2 notification happened\n") );
    iNotificationCallBackOccurred = ETrue;
    test(CompareEntryNotification(aChangeItems));
    if(iExpectedCalendarInfoTypes.Count() == 0 && iExpectedEntryChange.Count() == 0)
        {
        CActiveScheduler::Stop();
        }
    }

CCalFileChangeNotificationTest* CCalFileChangeNotificationTest::NewLC()
    {
    CCalFileChangeNotificationTest* self = new (ELeave) CCalFileChangeNotificationTest();
    CleanupStack::PushL(self);
    self->ConstructL();
    return (self);
    }
    
CCalFileChangeNotificationTest::~CCalFileChangeNotificationTest()
    {
    delete iOtherSession;
    TRAP_IGNORE(iTestLib->DeleteFileL(KTestFileName));
    delete iTestLib;
    iExpectedCalendarInfoTypes.Close();
    iExpectedEntryChange.Close();
    }

void CCalFileChangeNotificationTest::ConstructL()
    {
    iTestLib = CCalTestLibrary::NewL();
    TRAP_IGNORE(iTestLib->DeleteFileL(KTestFileName));
    iOtherSession = CCalSession::NewL();
    }

TCalLocalUid CCalFileChangeNotificationTest::StoreEntryL()
    {
    test.Printf(_L("Add entry\n" ));
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    _LIT8(KUid, "UID_TzDb");
    HBufC8* guid = KUid().AllocLC();
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry);
    entries.AppendL(entry);
    CleanupStack::Pop(entry);
    
    //Set entry start and end time
    TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
    TCalTime calStartTime;
    calStartTime.SetTimeLocalL(startTime);
    TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
    TCalTime calEndTime;
    calEndTime.SetTimeLocalL(endTime);
    entry->SetStartAndEndTimeL(calStartTime, calEndTime);
    
    //Set repeating 
    TCalRRule repeat(TCalRRule::EDaily);
    repeat.SetDtStart(calStartTime);
    repeat.SetCount(3);
    entry->SetRRuleL(repeat);

    //Store the Entry
    TInt numStoredEntries;
    iOtherSession->OpenL(KTestFileName());
    CCalEntryView* otherView =CCalEntryView::NewL(*iOtherSession);
    CleanupStack::PushL(otherView);
    otherView->StoreL(entries, numStoredEntries);
    CleanupStack::PopAndDestroy(otherView);
    test (numStoredEntries == entries.Count());
    TCalLocalUid entryId = entries[0]->LocalUidL();

    CleanupStack::PopAndDestroy(&entries);
    return entryId; 
    }

/** Test the file change notification is send and received when the other client has created a file.
*/
void CCalFileChangeNotificationTest::TestFileCreatedL()
    {//maybe combine with delete
    //Register for Calendar entry change notification.
    test.Next(_L("Test notification when file is created"));
    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    iOtherSession->CreateCalFileL(KTestFileName());
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarFileCreated);
    
    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback
        
    test(iFileNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
 
    iFileNotificationCallBackOccurred = EFalse;
    iTestLib->GetSession().StopFileChangeNotification();
    }

/** Test the file change notifications are send and received when the other client has created a file with a metadata.
*/  
void CCalFileChangeNotificationTest::TestFileChangeWithCalendarInfoL()
    {
    //Register for Calendar entry change notification.
    test.Next(_L("Test notification when file is created"));
    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);  
    iOtherSession->CreateCalFileL(KTestFileName(), *calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarFileCreated);
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarInfoCreated);
    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback       
    test(iFileNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
    iFileNotificationCallBackOccurred = EFalse;
    
    iOtherSession->DeleteCalFileL(KTestFileName());
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarFileDeleted);
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarInfoDeleted);
    
    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback       
    test(iFileNotificationCallBackOccurred);
    iFileNotificationCallBackOccurred = EFalse;
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
 
    iTestLib->GetSession().StopFileChangeNotification();
    }

/** Test the file change notifications are send and received when the other client has updated the metadata.
*/
void CCalFileChangeNotificationTest::TestUpdateCalendarInfoL()
    {
    test.Next(_L("Test notification when calendar info is updated"));
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);  
    iOtherSession->CreateCalFileL(KTestFileName(), *calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);
    iOtherSession->OpenL(KTestFileName);

    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    
    const TInt numUpdates = 10;
    for(TInt ii= 0; ii < numUpdates; ++ii)
        {
        calendarInfo = CCalCalendarInfo::NewL();
        CleanupStack::PushL(calendarInfo);  
        iOtherSession->SetCalendarInfoL(*calendarInfo);
        CleanupStack::PopAndDestroy(calendarInfo);
        iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarInfoUpdated);
        }
    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback       
    test(iFileNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
    iFileNotificationCallBackOccurred = EFalse;

    iTestLib->GetSession().StopFileChangeNotification();
    iOtherSession->DeleteCalFileL(KTestFileName());
    }

/** Test the file change notification is send and received when the other client has deleted a file.
*/
void CCalFileChangeNotificationTest::TestFileDeletedL()
    {
    test.Next(_L("TestFileDeletedL"));

    //Register for Calendar entry change notification.
    test.Next(_L("Test notification when file is deleted"));
    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    iOtherSession->DeleteCalFileL(KTestFileName());
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarFileDeleted);
     
    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback
         
    test(iFileNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
    iFileNotificationCallBackOccurred = EFalse;
     
    iTestLib->GetSession().StopFileChangeNotification();
    }

void CCalFileChangeNotificationTest::RegisterEntryNotificationL()
    {//requeste for the change notification
    RDebug::Print(_L("Register Notication" ));
    iTestLib->ReplaceFileL(KTestFileName);
    iTestLib->GetSession().OpenL(KTestFileName);

    TCalTime minTime;
    minTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime maxTime;
    maxTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange timeRange(minTime, maxTime);
    CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, timeRange);
    iTestLib->GetSession().StartChangeNotification(*this, *filter);
    delete filter;
    }

/** Test the file change notifications and entry change notification are received when the other client change the file and\or calendar entries.
*/
void CCalFileChangeNotificationTest::TestMixedNotificationL()
    {
// (1) Session 1 request both file and entry notification
    //Session 1 Register for Calendar entry change notification.
    test.Next(_L("Test entry and file notification are mixed"));
    //Session 1 Register for Calendar file change notification.
    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    RegisterEntryNotificationL();
   
    //Session 2 set and store the calendar info
    iOtherSession->OpenL(KTestFileName());
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);  
    iOtherSession->SetCalendarInfoL(*calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);

    //one file change notification is expected
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarInfoCreated);
    //Session 2 also add an entry
    TCalLocalUid entryId = StoreEntryL();//Add an entry by iOtherSession
    TCalChangeEntry change;
    change.iEntryId = entryId;
    change.iChangeType = MCalChangeCallBack2::EChangeAdd;
    change.iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
    //one entry change notification is expected
    iExpectedEntryChange.AppendL(change);

    CActiveScheduler::Start();
    //CActiveScheduler is stopped in Callback       
    test(iFileNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
    test(iNotificationCallBackOccurred);
    test(iExpectedEntryChange.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.

    iFileNotificationCallBackOccurred = EFalse;
    iNotificationCallBackOccurred = EFalse;

// (2) Session 1 stoped file change notification only request for entry notification
    iTestLib->GetSession().StopFileChangeNotification();   
    calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);  
    iOtherSession->SetCalendarInfoL(*calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);
    StoreEntryL();
    change.iChangeType = MCalChangeCallBack2::EChangeModify;    
    iExpectedEntryChange.AppendL(change);
    CActiveScheduler::Start();
    test(!iFileNotificationCallBackOccurred);
    test(iNotificationCallBackOccurred);
    test(iExpectedEntryChange.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.
    iNotificationCallBackOccurred = EFalse;
    
// (3) Session 1 start the entry change notification and stoped file change notification
    iTestLib->GetSession().StartFileChangeNotificationL(*this);
    iTestLib->GetSession().StopChangeNotification();
    StoreEntryL();
    calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);  
    iOtherSession->SetCalendarInfoL(*calendarInfo);
    CleanupStack::PopAndDestroy(calendarInfo);
    iExpectedCalendarInfoTypes.AppendL(MCalFileChangeObserver::ECalendarInfoUpdated);
    CActiveScheduler::Start();
    test(iFileNotificationCallBackOccurred);
    test(!iNotificationCallBackOccurred);
    test(iExpectedCalendarInfoTypes.Count() == 0);//In callback function elements in iExpectedCalendarInfoTypes has been remove when it callback item is equel to the expoected.

    iTestLib->GetSession().StopFileChangeNotification(); 
    delete iOtherSession;
    iTestLib->DeleteFileL(KTestFileName());
    iOtherSession = CCalSession::NewL();
    }
  
/**
*/
void DoTestsL()
    {
    test.Title();
    test.Start(_L("@SYMTESTCaseID:PIM-TCAL-TZDBCHANGE-0001 Calendar TZ Rules Data Change Awareness Test Suite"));
    CCalFileChangeNotificationTest* testManager = CCalFileChangeNotificationTest::NewLC();
    
    // Run the test suite
    testManager->TestFileCreatedL();
    testManager->TestFileDeletedL();
    testManager->TestFileChangeWithCalendarInfoL();
    testManager->TestUpdateCalendarInfoL();
    testManager->TestMixedNotificationL();
    
    CleanupStack::PopAndDestroy(testManager);
    test.End();
    }

TInt E32Main()
    {
    __UHEAP_MARK;

    CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if(!trapCleanup)
        {
        return KErrNoMemory;
        }

    CActiveScheduler* scheduler = new CActiveScheduler;
    if(!scheduler)
        {
        return KErrNoMemory;
        }
    CActiveScheduler::Install(scheduler);   
    
    TRAPD(ret, DoTestsL());
    test.Printf(_L("Trapped return value from DoTestsL(): %d\n"), ret);
    test(ret == KErrNone);

    test.Close();   

    delete scheduler;
    delete trapCleanup; 

    __UHEAP_MARKEND;

    return (KErrNone);
    }


