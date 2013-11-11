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
#include "caltestoom.h"

#include <calsession.h>
#include <e32test.h>
#include <calcalendarinfo.h>
#include <calcalendariterator.h>
#include <calentry.h>
#include <calentryview.h>
#include <calalarm.h>
#include <asclisession.h>
#include <asshdalarm.h>

RTest test(_L("tcal_calendarinfo"));

_LIT8(KGUID,  "A_Global_UID");
_LIT8(KGUID1, "A_Global_UID1");
_LIT8(KGUID2, "A_Global_UID2");
_LIT8(KGUID3, "A_Global_UID3");

_LIT(KCalendarFile, "tcal_calendarinfo");
_LIT(KCalendarFileExpected, "c:tcal_calendarinfo");
_LIT(KCalendarFilePrivatePath, "c:\\private\\10003a5b\\tcal_calendarinfo");

// calendar info values
_LIT(KCalendarName, "Calendar");
_LIT(KCalendarDescription, "This is a calendar");
const TRgb KCalendarColor(0xD7, 0x10, 0x01, 0xFF);
const TBool KCalendarEnabled(ETrue);

_LIT8(KPropertyTimeKey, "time");
const TTime KPropertyTimeValue(TDateTime(2007, EMarch, 5, 12, 30, 0, 0));
_LIT8(KPropertyStringKey, "string");
_LIT8(KPropertyStringValue, "Here is some 8 bit text.");
_LIT8(KPropertyIntKey, "int");
const TInt KPropertyIntValue(34);

class CTestManager : public CBase, public MCalTestOomCallBack
	{
private:
    enum
        {
        EOomTestCaseCreateCalFileWithInfo,
        EOomTestCaseSetInfoOnFileWithoutInfo,
        EOomTestCaseUpdateInfo,
        EOomTestCaseFetchInfo,
        EOomTestCaseIterateOverAllInfo,
        EOomTestCaseLast
        };
    
public:
	static CTestManager* NewLC();
	~CTestManager();

	void RunTestsL();

private:
	CTestManager();
	void ConstructL();
	
	static void CleanupDeleteCalFile(TAny* aRef);
	
private: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);
	void AddEntryWithAlarmL(const TTime& aStartTime, TInt aAlarmMinutes, HBufC8* aGuid, TBool aFloating);
		
private: // tests ...
    void TestCreateFileWithMetaDataL();
    void TestSetMetaDataL();
    void TestGetMetaDataL();
    void TestGetAllMetaDataL();
    void TestOomL();
    void TestAlarmStatusChangeL();
    void TestAlarmStatusWithAgendaShutDownL();
    void TestSetAndUpdatePropertyL();
    void TestGetPropertyL();
    void TestRemovePropertyL();
    void TestReplaceMetadataL();

private:
	CCalTestLibrary* iTestLibrary; // need a connection to a session to create a CCalEntry
	TCalFileId iFileId;
	RASCliSession      iAlarmServer;
	TInt iOomTestCase;
	};

CTestManager* CTestManager::NewLC()
	{
	CTestManager* self = new (ELeave) CTestManager;
	CleanupStack::PushL(self);
	self->ConstructL();
	// CleanupStack::Pop(self);
	return (self);
	}
	
CTestManager::CTestManager()
	{
	}

void CTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	test.Printf(_L("Make sure the file is deleted when we start the test"));
	TRAP_IGNORE(iTestLibrary->GetSession().DeleteCalFileL(KCalendarFile));
	
	User::LeaveIfError(iAlarmServer.Connect());
	}
	
CTestManager::~CTestManager()
	{
	delete iTestLibrary;
	iAlarmServer.Close();
	}

void CTestManager::CleanupDeleteCalFile(TAny* aRef)
    {
    CCalSession* session = static_cast<CCalSession*>(aRef);
    
    TRAPD(error, session->DeleteCalFileL(KCalendarFile));
    
    if (error != KErrNone)
        {
        test.Printf(_L("There was a problem deleting the file! %d"), error);
        }
    
    delete session;
    }

void CTestManager::TestSetAndUpdatePropertyL()
    {
    test.Next(_L("Test setting some properties"));
    
    test.Printf(_L("create some meta data"));
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);

    test.Printf(_L("Add some properties to the metadata"));
    TTime now;
    now.HomeTime();
    TPckgC<TTime> timePckg1(now);
    calendarInfo->SetPropertyL(KPropertyTimeKey, timePckg1);
    
    TPckgC<TInt> intPckg1(KPropertyIntValue + 1);
    calendarInfo->SetPropertyL(KPropertyIntKey, intPckg1);
        
    calendarInfo->SetPropertyL(KPropertyStringKey, KPropertyStringValue);
    
    test.Printf(_L("Create a calendar file with the metadata"));
    iTestLibrary->GetSession().CreateCalFileL(KCalendarFile, *calendarInfo);
    
    test.Next(_L("Test updating some of the metadata properties"));
    TPckgC<TTime> timePckg2(KPropertyTimeValue);
    calendarInfo->SetPropertyL(KPropertyTimeKey, timePckg2);
    
    TPckgC<TTime> intPckg2(KPropertyIntValue);
    calendarInfo->SetPropertyL(KPropertyIntKey, intPckg2);
    
    test.Printf(_L("Open the file"));
    iTestLibrary->GetSession().OpenL(KCalendarFile);
    
    test.Printf(_L("Set the calendar info"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    }

void CTestManager::TestGetPropertyL()
    {
    test.Next(_L("Test fetching the metadata"));
    
    // fetch the calendar info from the session
    CCalCalendarInfo* calendarInfo = iTestLibrary->GetSession().CalendarInfoL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("Fetch the property keys"));
    CDesC8Array* propertyKeys = calendarInfo->PropertyKeysL();
    CleanupStack::PushL(propertyKeys);
    
    test.Printf(_L("Test that there are only three properties"));
    test(propertyKeys->Count() == 3);
    
    test.Printf(_L("Test that all three property keys are found"));
    TInt pos(KErrNotFound);
    test(propertyKeys->Find(KPropertyTimeKey, pos, ECmpNormal) != KErrNotFound);
    test(propertyKeys->Find(KPropertyIntKey, pos, ECmpNormal) != KErrNotFound);
    test(propertyKeys->Find(KPropertyStringKey, pos, ECmpNormal) != KErrNotFound);
    
    CleanupStack::PopAndDestroy(propertyKeys);
    
    test.Printf(_L("Test that the time property is set to the correct value"));
    TTime time;
    TPckgC<TTime> timeBuf(time);
    timeBuf.Set(calendarInfo->PropertyValueL(KPropertyTimeKey));
    time = timeBuf();
    test(time == KPropertyTimeValue);
    
    test.Printf(_L("Test that the int property is set to the correct value"));
    TInt intVal;
    TPckgC<TInt> intBuf(intVal);
    intBuf.Set(calendarInfo->PropertyValueL(KPropertyIntKey));
    intVal = intBuf();
    test(intVal == KPropertyIntValue);
    
    test.Printf(_L("Test that the string property is set to the correct value"));
    test(calendarInfo->PropertyValueL(KPropertyStringKey).Compare(KPropertyStringValue) == 0);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    }

void CTestManager::TestRemovePropertyL()
    {
    test.Next(_L("Test that we can remove a property from the properties"));
    
    test.Printf(_L("Fetch the calendar info"));
    CCalCalendarInfo* calendarInfo = iTestLibrary->GetSession().CalendarInfoL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("We should have 3 property keys before removing one"));
    CDesC8Array* array = calendarInfo->PropertyKeysL();
    test(array->Count() == 3);
    delete array;
    
    test.Printf(_L("Remove the property by key"));
    calendarInfo->RemovePropertyL(KPropertyTimeKey);
    
    test.Printf(_L("We should now have 2 properties after removing one"));
    array = calendarInfo->PropertyKeysL();
    test(array->Count() == 2);
    delete array;
    
    test.Printf(_L("Set the calendar info"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    
    calendarInfo = iTestLibrary->GetSession().CalendarInfoL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("Test that there are now only two keys"));
    CDesC8Array* propertyKeys = calendarInfo->PropertyKeysL();
    CleanupStack::PushL(propertyKeys);
    
    test(propertyKeys->Count() == 2);
    
    test.Printf(_L("Test that the correct two property keys are found"));
    TInt pos(KErrNotFound);
    test(propertyKeys->Find(KPropertyIntKey, pos, ECmpNormal) != KErrNotFound);
    test(propertyKeys->Find(KPropertyStringKey, pos, ECmpNormal) != KErrNotFound);
    
    CleanupStack::PopAndDestroy(propertyKeys);
    CleanupStack::PopAndDestroy(calendarInfo);
    }

void CTestManager::TestReplaceMetadataL()
    {
    _LIT8(KDifferentKey, "different key");
    _LIT8(KDifferentValue, "different value");
    
    test.Next(_L("Test that we can replace the metadata"));
    
    test.Printf(_L("create some new metadata"));
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);

    test.Printf(_L("Add a property to the metadata"));
    calendarInfo->SetPropertyL(KDifferentKey, KDifferentValue);
    
    test.Printf(_L("set the metadata"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    
    test.Printf(_L("fetch the metadata"));
    calendarInfo = iTestLibrary->GetSession().CalendarInfoL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("Test that there is only the one new key"));
    CDesC8Array* propertyKeys = calendarInfo->PropertyKeysL();
    CleanupStack::PushL(propertyKeys);
    
    test(propertyKeys->Count() == 1);
    
    test.Printf(_L("Test that the correct two property keys are found"));
    TInt pos(KErrNotFound);
    test(propertyKeys->Find(KDifferentKey, pos, ECmpNormal) != KErrNotFound);
    
    CleanupStack::PopAndDestroy(propertyKeys);
    CleanupStack::PopAndDestroy(calendarInfo);
    }

/* 
@SYMTestCaseID				PIM-TCAL-CALENDAR-INFO-0001
@SYMPreq					CR1985
@SYMTestCaseDesc			Test that we can create a file with metadata.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1. Create a calendar info instance and set some values on it
							2. Create a file and specify that the calendar info is associated with it
							
@SYMTestExpectedResults		The calendar file should be created and the metadata set on it. 
**/
void CTestManager::TestCreateFileWithMetaDataL()
    {
    test.Next(_L("Test that a file can be created with meta data"));
    
    test.Printf(_L("Make sure the file is deleted when we start the test"));
    TRAP_IGNORE(iTestLibrary->GetSession().DeleteCalFileL(KCalendarFile));
    
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);

    test.Printf(_L("Set some properties on the meta data"));
    calendarInfo->SetNameL(KCalendarName);
    calendarInfo->SetDescriptionL(KCalendarDescription);
    calendarInfo->SetColor(KCalendarColor);
    calendarInfo->SetEnabled(KCalendarEnabled);
    
    test.Printf(_L("Use the create cal file overload method that takes calendar info"));
    iTestLibrary->GetSession().CreateCalFileL(KCalendarFile, *calendarInfo);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    }

/* 
@SYMTestCaseID				PIM-TCAL-CALENDAR-INFO-0002
@SYMPreq					CR1985
@SYMTestCaseDesc			Test that we can set metadata on a file that already exists.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1. Create a file without metadata
							2. Create a calendar info instance and set some values on it
							2. Set the calendar info is associated with it
							
@SYMTestExpectedResults		The calendar file should be created and the metadata set on it. 
**/
void CTestManager::TestSetMetaDataL()
    {
    test.Next(_L("Test that you can set meta data on a file that already exists"));
    
    test.Printf(_L("Make sure the file is deleted when we start the test"));
    TRAP_IGNORE(iTestLibrary->GetSession().DeleteCalFileL(KCalendarFile));
    
    test.Printf(_L("Create the cal file without meta data"));
    iTestLibrary->GetSession().CreateCalFileL(KCalendarFile);
    
    test.Printf(_L("open the file"));
    iTestLibrary->GetSession().OpenL(KCalendarFile);
    iTestLibrary->GetSession().FileIdL(iFileId);

    test.Printf(_L("create the meta data object"));
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("Set some properties on the meta data"));
    calendarInfo->SetNameL(KCalendarName);
    calendarInfo->SetDescriptionL(KCalendarDescription);
    calendarInfo->SetColor(KCalendarColor);
    calendarInfo->SetEnabled(KCalendarEnabled);
    
    test.Printf(_L("Set the meta data on the file"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    }
	
/* 
@SYMTestCaseID				PIM-TCAL-CALENDAR-INFO-0003
@SYMPreq					CR1985
@SYMTestCaseDesc			Test that we are able to fetch metadata that has been set on a file.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1. Open a file with metadata
							2. fetch the properties of the metadata
							
@SYMTestExpectedResults		The metadata should be there and the properties should have the correct values.
**/
void CTestManager::TestGetMetaDataL()
    {
    test.Next(_L("Test that you can set meta data on a file that already exists"));
    
    test.Printf(_L("open the file"));
    iTestLibrary->GetSession().OpenL(KCalendarFile);
    iTestLibrary->GetSession().FileIdL(iFileId);
    
    test.Printf(_L("get the meta data"));
    CCalCalendarInfo* calendarInfo = iTestLibrary->GetSession().CalendarInfoL();
    
    if (calendarInfo)
        {
        test.Printf(_L("There is meta data so test that is set correctly"));
        
        CleanupStack::PushL(calendarInfo);
        test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
        test(calendarInfo->NameL().Compare(KCalendarName) == 0);
        test(calendarInfo->DescriptionL().Compare(KCalendarDescription) == 0);
        test(calendarInfo->Color() == KCalendarColor);
        test(calendarInfo->Enabled() == KCalendarEnabled);
    
        CleanupStack::PopAndDestroy(calendarInfo);
        }
    else
        {
        test.Printf(_L("There is no meta data"));
        test(EFalse);
        }
    }

/* 
@SYMTestCaseID				PIM-TCAL-CALENDAR-INFO-0004
@SYMPreq					CR1985
@SYMTestCaseDesc			Test that we can fetch metadata for all files through the calendar iterator.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1. Have 1 file that has metadata set on it
							2. Have many other files that do not have metadata (calendar files from other tests). 
							3. Create a calendar iterator
							4. iterate through all calendars fetching metadata of it exitsts
							
@SYMTestExpectedResults		Only one file should have valid metadata and it should have the correct values.
**/
void CTestManager::TestGetAllMetaDataL()
    {
    test.Next(_L("Test iterating through all meta data set on the calendar server"));

    test.Printf(_L("create the iterator"));
    CCalCalendarIterator* calendarIterator = CCalCalendarIterator::NewL(iTestLibrary->GetSession());
    CleanupStack::PushL(calendarIterator);
    
    CDesCArray* calFiles = iTestLibrary->GetSession().ListCalFilesL();
    CleanupStack::PushL(calFiles);
    
    test.Printf(_L("Test the meta data count is the same as the list calendar info count"));
    const TInt KCalendarInfoCount = calendarIterator->Count();
    const TInt KListFilesCount = calFiles->Count();
    test(KCalendarInfoCount == KListFilesCount);
    
    test.Printf(_L("Iterate through the calendar info collection"));
    
    TInt validCount(0);
    TInt totalCount(0);
    
    for (CCalCalendarInfo* calendarInfo = calendarIterator->FirstL();
            calendarInfo != NULL ;
            calendarInfo = calendarIterator->NextL())
        {
        CleanupStack::PushL(calendarInfo);
        
        test.Printf(_L("File %d = %S\n"), ++totalCount, &calendarInfo->FileNameL());
        
        if (calendarInfo->IsValid())
            {
            test.Printf(_L("The file is valid so test that the properties are as expected"));
            
            ++validCount;
            
            test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
            test(calendarInfo->NameL().Compare(KCalendarName) == 0);
            test(calendarInfo->DescriptionL().Compare(KCalendarDescription) == 0);
            test(calendarInfo->Color() == KCalendarColor);
            test(calendarInfo->Enabled() == KCalendarEnabled);
            }
        else
            {
            test.Printf(_L("The file is invalid, but make sure it is one of the files from ListCalFilesL"));
            
            TInt pos(KErrNotFound);
            TInt findError = calFiles->Find(calendarInfo->FileNameL(), pos);
            
            test(findError == KErrNone);
            
            // Remove the file from the list so that we test we don't get duplicates
            calFiles->Delete(pos);
            }
        
        test.Printf(_L("Test that the CurrentL API works"));
        CCalCalendarInfo* currentCalendarInfo = calendarIterator->CurrentL();
        CleanupStack::PushL(currentCalendarInfo);
        
        test(currentCalendarInfo->IsValid() == calendarInfo->IsValid());
        test(currentCalendarInfo->FileNameL().Compare(calendarInfo->FileNameL()) == 0 );
        
        if (currentCalendarInfo->IsValid())
            {
            test.Printf(_L("The info is valid so test that all the other properties are the same"));
            
            test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
            test(calendarInfo->NameL().Compare(KCalendarName) == 0);
            test(calendarInfo->DescriptionL().Compare(KCalendarDescription) == 0);
            test(calendarInfo->Color() == KCalendarColor);
            test(calendarInfo->Enabled() == KCalendarEnabled);
            }
        
        CleanupStack::PopAndDestroy(currentCalendarInfo);
        CleanupStack::PopAndDestroy(calendarInfo);
        }
    
    test.Printf(_L("Test that we only found one file with valid meta data"));
    test(validCount == 1);
    
    CleanupStack::PopAndDestroy(calFiles);
    CleanupStack::PopAndDestroy(calendarIterator);
    }
	
/* 
@SYMTestCaseID				PIM-TCAL-CALENDAR-INFO-0005
@SYMPreq					CR1985
@SYMTestCaseDesc			Test that all the metadata APIs work under OOM conditions.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1. Call all new APIs under OOM conditions
							
@SYMTestExpectedResults		APIs should be transactional and fail without panicking.
**/
void CTestManager::OomTestL(TType aType, TInt aFailAt)
    {
    test.Printf(_L("Oom test callback %d %d\n"), aType, aFailAt);
    
    // create file with calendar info
    CCalSession* session = CCalSession::NewL();
    CleanupStack::PushL(TCleanupItem(CleanupDeleteCalFile, session));
    
    TRAPD(err, session->DeleteCalFileL(KCalendarFile));
    if (err != KErrNotFound)
        {
        User::LeaveIfError(err);
        }
    
    CCalCalendarInfo* info = CCalCalendarInfo::NewL();
    CleanupStack::PushL(info);
    
    info->SetNameL(KCalendarName);
    info->SetDescriptionL(KCalendarDescription);
    info->SetColor(KCalendarColor);
    info->SetEnabled(KCalendarEnabled);
    
    switch (iOomTestCase)
        {
        case EOomTestCaseCreateCalFileWithInfo:
            {
            session->CreateCalFileL(KCalendarFile, *info);
            }
            break;
        case EOomTestCaseSetInfoOnFileWithoutInfo:
            {
            // create file without calendar info and then set
            session->CreateCalFileL(KCalendarFile);
            session->OpenL(KCalendarFile);
            session->SetCalendarInfoL(*info);
            }
            break;
        case EOomTestCaseUpdateInfo:
            {
            // create cal file with impl and then update it
            session->CreateCalFileL(KCalendarFile, *info);
            session->OpenL(KCalendarFile);
            session->SetCalendarInfoL(*info);
            }
            break;
        case EOomTestCaseFetchInfo:
            {
            session->CreateCalFileL(KCalendarFile, *info);
            session->OpenL(KCalendarFile);
        
            // get calendar info
            
            CCalCalendarInfo* fetchedInfo = session->CalendarInfoL();
            CleanupStack::PushL(fetchedInfo);
            
            // check that all the details are correct
            test(fetchedInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
            test(fetchedInfo->NameL().Compare(KCalendarName) == 0);
            test(fetchedInfo->DescriptionL().Compare(KCalendarDescription) == 0);
            test(fetchedInfo->Color() == KCalendarColor);
            test(fetchedInfo->Enabled() == KCalendarEnabled);
            
            CleanupStack::PopAndDestroy(fetchedInfo);
            }
            break;
        case EOomTestCaseIterateOverAllInfo:
            {
            session->CreateCalFileL(KCalendarFile, *info);
            
            // Iterate through the calendar info
            CCalCalendarIterator* calendarIterator = CCalCalendarIterator::NewL(*session);
            CleanupStack::PushL(calendarIterator);
            
            for (CCalCalendarInfo* calendarInfo = calendarIterator->FirstL();
                    calendarInfo != NULL ;
                    calendarInfo = calendarIterator->NextL())
                {
                CleanupStack::PushL(calendarInfo);
                
                if (calendarInfo->IsValid())
                    {
                    test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
                    test(calendarInfo->NameL().Compare(KCalendarName) == 0);
                    test(calendarInfo->DescriptionL().Compare(KCalendarDescription) == 0);
                    test(calendarInfo->Color() == KCalendarColor);
                    test(calendarInfo->Enabled() == KCalendarEnabled);
                    }
                else
                    {
                    // the filename should not be that of the file we expect to have metadata
                    test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) != 0);
                    }
                
                CCalCalendarInfo* currentCalendarInfo = calendarIterator->CurrentL();
                CleanupStack::PushL(currentCalendarInfo);
                
                test(currentCalendarInfo->IsValid() == calendarInfo->IsValid());
                test(currentCalendarInfo->FileNameL().Compare(calendarInfo->FileNameL()) == 0 );
                
                if (currentCalendarInfo->IsValid())
                    {
                    test(calendarInfo->FileNameL().Compare(KCalendarFileExpected) == 0);
                    test(calendarInfo->NameL().Compare(KCalendarName) == 0);
                    test(calendarInfo->DescriptionL().Compare(KCalendarDescription) == 0);
                    test(calendarInfo->Color() == KCalendarColor);
                    test(calendarInfo->Enabled() == KCalendarEnabled);
                    }
                
                CleanupStack::PopAndDestroy(currentCalendarInfo);
                CleanupStack::PopAndDestroy(calendarInfo);
                }
            
            CleanupStack::PopAndDestroy(calendarIterator);
            }
            break;
        default:
            break;
        };
    
    CleanupStack::PopAndDestroy(info);
    CleanupStack::PopAndDestroy(session);
    }

void CTestManager::TestOomL()
    {
    // make sure the agenda is not running when we start
    iTestLibrary->CloseAgendaServer();
    iTestLibrary->ConnectAgendaServerL();
    
    for (iOomTestCase = 0 ; iOomTestCase < EOomTestCaseLast ; ++iOomTestCase)
        {
        test.Printf(_L("Oom test case %d"), iOomTestCase);
        
        test.Printf(_L("Test the client side"));
        TRAP_IGNORE(iTestLibrary->GetSession().DeleteCalFileL(KCalendarFile));
        CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EClient, iTestLibrary->GetSession());
        
        test.Printf(_L("Test the server side"));
        TRAP_IGNORE(iTestLibrary->GetSession().DeleteCalFileL(KCalendarFile));
        CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EServer, iTestLibrary->GetSession());
        }
    }
	
void CTestManager::AddEntryWithAlarmL(const TTime& aStartTime, TInt aAlarmMinutes, HBufC8* aGuid, TBool aFloating)
    {  
    TDateTime dt = (aStartTime - TTimeIntervalMinutes(aAlarmMinutes)).DateTime();
    test.Printf(_L("Adding alarmed entry, alarm set to: %d/%d/%d, %d:%d\n"),
                dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute());
    
    HBufC8* guid = aGuid;        
    if (!guid)
        {
        guid = KGUID().AllocLC();
        }
        
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry);
    
    TCalTime calStartTime;
    TCalTime calEndTime;    
    if (aFloating)
        {
        calStartTime.SetTimeLocalFloatingL(aStartTime);
        calEndTime.SetTimeLocalFloatingL(aStartTime + TTimeIntervalHours(1));
        }
    else
        {
        calStartTime.SetTimeLocalL(aStartTime);
        calEndTime.SetTimeLocalL(aStartTime + TTimeIntervalHours(1));
        }        
    entry->SetStartAndEndTimeL(calStartTime, calEndTime);    
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(aAlarmMinutes);
    entry->SetAlarmL(alarm);
    CleanupStack::PopAndDestroy(alarm);
    RPointerArray<CCalEntry> entriesToStore;
    CleanupClosePushL(entriesToStore);    
    entriesToStore.AppendL(entry);    
    TInt entriesStored = 0;
    iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
    test(entriesToStore.Count() == entriesStored);    
    CleanupStack::PopAndDestroy(2); //&entriesToStore, entry
    }

/* 
@SYMTestCaseID              PIM-TCAL-CALENDAR-INFO-0006
@SYMPreq                    CR2066
@SYMTestCaseDesc            Test that alarm status changes is consistent with agenda shut down.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions             1. Create a calendar file and set the metadata
                            2. Create a few alarmed entries
                            3. Get the alarm details
                            4. Close agenda server
                            5. Open agenda and check the alarm status
                            6. Disable calendar and check the alarm status
                            
@SYMTestExpectedResults     Alarms should be enabled when calendar metadata is enabled. Alarms 
                            should be disabled when calendar metadata is disabled.
**/
void CTestManager::TestAlarmStatusWithAgendaShutDownL()
    {
    test.Next(_L("Test the alarm status while closing and re-opening agenda"));
    
    test.Printf(_L("Make sure the file is deleted when we start the test"));
    iTestLibrary->DeleteFileL(KCalendarFile);      
    iTestLibrary->CreateFileL(KCalendarFile);
    iTestLibrary->OpenFileL(KCalendarFile);
   
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);
    
    test.Printf(_L("Set some properties on the meta data"));
    calendarInfo->SetNameL(KCalendarFile);
    calendarInfo->SetDescriptionL(KCalendarDescription);
    calendarInfo->SetColor(KCalendarColor);
    calendarInfo->SetEnabled(KCalendarEnabled);
    
    test.Printf(_L("Set the meta data on the file"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
        
    const TInt offSet = 30;// alarm offset is 30 seconds
    TTime alarmTime(TDateTime(2009, EJuly, 5, 12, 0, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(alarmTime - TTimeIntervalSeconds(offSet)); 

    HBufC8* guid1 = KGUID1().AllocLC();
    AddEntryWithAlarmL(alarmTime, 0,  guid1, EFalse); 
   
    HBufC8* guid2 = KGUID2().AllocLC();
    AddEntryWithAlarmL(alarmTime + TTimeIntervalSeconds(offSet), 0,  guid2, EFalse); 
    
    HBufC8* guid3 = KGUID3().AllocLC();
    AddEntryWithAlarmL(alarmTime + TTimeIntervalMinutes(offSet), 0,  guid3, EFalse); 
        
    TAlarmId alarmId;
    TASShdAlarm alarm;
    iAlarmServer.GetNextDueAlarmId(alarmId);
    iAlarmServer.GetAlarmDetails(alarmId, alarm);
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
        
    test.Printf(_L("Calendar server shut down\n")); 
    iTestLibrary->CloseAgendaServer();    

    // Three alarm should be queued
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 3);
    
    iTestLibrary->OpenFileL(KCalendarFile());    
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 3);
    
    iTestLibrary->SynCGetEntryViewL();//force the entry view is build so that the alarms will be deleted.
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
    
    test.Printf(_L("Wait for an alarm to notify\n")); 
    test(iTestLibrary->WaitForAlarmEvent(KNullAlarmId, EAlarmChangeEventState, EAlarmStateNotifying, offSet*1000000));
        
    // One alarm should be queued
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
    iAlarmServer.GetNextDueAlarmId(alarmId);
    iAlarmServer.GetAlarmDetails(alarmId, alarm);
    TAlarmStatus status;
    iAlarmServer.GetAlarmStatus(alarmId, status);
    test.Printf(_L("Check the alarm status - it should be enabled\n")); 
    test(status == EAlarmStatusEnabled);
    
    test.Printf(_L("Disable calendar metadata\n")); 
    calendarInfo->SetEnabled(EFalse);
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    iAlarmServer.GetAlarmDetails(alarmId, alarm);
    iAlarmServer.GetAlarmStatus(alarmId, status);
    test.Printf(_L("Check the alarm status - it should be disabled\n")); 
    test(status == EAlarmStatusDisabled);
       
    // Restore settings back to how those were before start of the test
    iTestLibrary->CleanDatabaseL();
    TTime now;
    now.HomeTime();
    iTestLibrary->PIMTestServer().SetHomeTime(now);
    
    CleanupStack::PopAndDestroy(calendarInfo);
    }

/* 
@SYMTestCaseID              PIM-TCAL-CALENDAR-INFO-0007
@SYMPreq                    CR2066
@SYMTestCaseDesc            Test that alarm status changes with calendar metadata.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus              Implemented
@SYMTestActions             1. Create a calendar file and add metadata to it
                            2. Add an alarmed entry
                            3. Check the status of the alarm
                            4. Disable calendar and check alarm status
                            
@SYMTestExpectedResults     Alarm status should be enabled/disabled depending on calendar metadata status.
**/
void CTestManager::TestAlarmStatusChangeL()
    {
    test.Next(_L("Test the change in alarm status with respect to calendar status"));
        
    test.Printf(_L("Make sure the file is deleted when we start the test\n"));
    iTestLibrary->DeleteFileL(KCalendarFile);
    iTestLibrary->CreateFileL(KCalendarFile);
    iTestLibrary->OpenFileL(KCalendarFile);
         
    CCalCalendarInfo* calendarInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calendarInfo);

    test.Printf(_L("Set some properties on the meta data\n"));
    calendarInfo->SetNameL(KCalendarFile);
    calendarInfo->SetDescriptionL(KCalendarDescription);
    calendarInfo->SetColor(KCalendarColor);
    calendarInfo->SetEnabled(KCalendarEnabled);
    
    test.Printf(_L("Set the meta data on the file\n"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    TTime time(TDateTime(2008, EJuly, 17, 0, 0, 0, 0));     
    iTestLibrary->PIMTestServer().SetHomeTime(time); 
        
    HBufC8* guid1 = KGUID1().AllocLC();
    test.Printf(_L("Add an alarmed entry\n"));
    AddEntryWithAlarmL(time + TTimeIntervalSeconds(2), 0,  guid1, EFalse); 
    
    TAlarmId alarmId;
    TASShdAlarm alarmDetails;
    iAlarmServer.GetNextDueAlarmId(alarmId);
    iAlarmServer.GetAlarmDetails(alarmId, alarmDetails);
   
    TAlarmStatus status;
    iAlarmServer.GetAlarmStatus(alarmId, status);
    test.Printf(_L("Check if the status of the alarm is enabled\n"));
    test(status == EAlarmStatusEnabled);
    
    calendarInfo->SetEnabled(EFalse);
    test.Printf(_L("Disable the meta data on the file\n"));
    iTestLibrary->GetSession().SetCalendarInfoL(*calendarInfo);
    
    iAlarmServer.GetAlarmDetails(alarmId, alarmDetails);
    test.Printf(_L("Get alarm status - alarm should be disabled\n")); 
    TRAPD(err, iAlarmServer.GetAlarmStatus(alarmId, status));
    test(err == KErrNone);
    test(status == EAlarmStatusDisabled); 
    
    CleanupStack::PopAndDestroy(calendarInfo);
    
    time.HomeTime();
    iTestLibrary->PIMTestServer().SetHomeTime(time);    
    iTestLibrary->CleanDatabaseL();
    }

void CTestManager::RunTestsL()
	{
	TestSetAndUpdatePropertyL();
    TestGetPropertyL();
    TestRemovePropertyL();
    TestReplaceMetadataL();
    TestCreateFileWithMetaDataL();
    TestSetMetaDataL();
    TestGetMetaDataL();
    TestGetAllMetaDataL();
    TestAlarmStatusChangeL();
    TestAlarmStatusWithAgendaShutDownL();
    TestOomL();    
	}
	
static void DoTestL()
	{
	CTestManager* testManager = CTestManager::NewLC();

	TPerformanceTimer timer(test);
	timer.Start();

	// Run the test suite
	testManager->RunTestsL();

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	CleanupStack::PopAndDestroy(testManager);
	}

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(KCalendarFile);
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
	
	if (ret != KErrNone)
	    {
	    test.Printf(_L("test failed with %d"), ret);
	    }
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
