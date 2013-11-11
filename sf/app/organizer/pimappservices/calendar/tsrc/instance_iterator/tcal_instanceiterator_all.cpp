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

#include "../caltestlib.h"
#include "../caltestoom.h"

#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceiterator.h>
#include <caliterator.h>

_LIT(KTestName, "tcal_instanceiterator_all");
_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S.%C");
_LIT(KPersonalCalendar, "personal_calendar");

const TTime KStartTimeOOMInstance(TTime(TDateTime(2006, ENovember, 12, 11, 30, 0, 0)));
const TInt KNumOfOOMInstances(10);

const TInt KFirstFileId = 1;

RTest test(KTestName);

class CTestData : public CBase
	{
public:
	struct TExpectedInstance
		{
		TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, CCalEntry::TType aType);
		TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, CCalEntry::TType aType, TUint8 aShortFileId);
		
		TCalTime iTime;
		TCalLocalUid iLocalUid;
		CCalEntry::TType iType;
		TUint8 iShortFileId;
		};
	enum TTestType
		{
		ESingleCalendarFileTest,
		EMultipleCalendarFilesTest
		};

public:
	CTestData();
	~CTestData();
	
	void InsertExpectedInstanceL(const TExpectedInstance& aExpectedInstance);
	
public:	
	CCalFindInstanceSettings* iFindInstanceSettings;
	RArray<TExpectedInstance> iExpectedInstances;
	TLinearOrder<TExpectedInstance> iLinearOrder;
	TTestType iTestType;
	};
	
CTestData::TExpectedInstance::TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, CCalEntry::TType aType)
	:iLocalUid(aLocalUid), iType(aType)
	{
	iTime.SetTimeLocalL(aTimeLocal);
	iShortFileId = KFirstFileId;
	}
	
CTestData::TExpectedInstance::TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, CCalEntry::TType aType, TUint8 aShortFileId)
 	:iLocalUid(aLocalUid), iType(aType), iShortFileId(aShortFileId)
 	{
 	iTime.SetTimeLocalL(aTimeLocal);
 	}	

TInt AnOrder(const CTestData::TExpectedInstance& aLeft, const CTestData::TExpectedInstance& aRight)
	{
	// This sort ordering does not account for instances at the same time instances should
	// therefore be added to the array in the correct order if they are that the same time
	TInt result(0);
	
	TTime now;
	now.HomeTime();
	
	TTime leftTime((aLeft.iTime.TimeLocalL() == Time::NullTTime()) ? now : aLeft.iTime.TimeLocalL());
	TTime rightTime((aRight.iTime.TimeLocalL() == Time::NullTTime()) ? now : aRight.iTime.TimeLocalL());
	
	if (leftTime < rightTime)
		{
		result = -1;
		}
	else if (leftTime > rightTime)
		{
		result = 1;
		}
	else if (aLeft.iType < aRight.iType)
		{
		result =1;
		}
	else if (aLeft.iType > aRight.iType)
		{
		result =-1; 
		}
	else
		{
		result = aLeft.iShortFileId - aRight.iShortFileId;
		}
		
	return result;
	}

	
CTestData::CTestData()
	:iLinearOrder(AnOrder)
	{
	}
	
CTestData::~CTestData()
	{
	delete iFindInstanceSettings;
	iExpectedInstances.Reset();
	}

void CTestData::InsertExpectedInstanceL(const TExpectedInstance& aExpectedInstance)
	{
	iExpectedInstances.InsertInOrderAllowRepeatsL(aExpectedInstance, iLinearOrder);
	}
		
class CInstanceIteratorTestManager : public CBase, public MCalTestOomCallBack
	{
public:
	enum
		{
		// Multiple files
		ERepeatingEventAndSingleTodoWithShortFileId,
		// Simple test cases
		ERepeatingEvent,
		ETwoRepeatingEvents,
		ERepeatingTodo,
		ETwoRepeatingTodos,
		ESingleEvent,
		ESingleTodo,
		
		// Mixtures of entries
		ERepeatingTodoAndSingleTodo,
		ERepeatingTodoAndSingleEvent,
		ERepeatingEventAndSingleTodo,
		ERepeatingEventAndSingleEvent,
		ERepeatingEventAndRepeatingTodo,
		
		// Entries with children RDates and ExDates
		ERepeatWithChild,
		ERepeatWithRDate,
		ERepeatWithExDate,
		
		// Time modes
		//EFloatingEntries,
		ENumOfTests
		};
		
public:
    static CInstanceIteratorTestManager* NewLC();
	~CInstanceIteratorTestManager();
		
	void RunTestsL();
	
private:
	void ConstructL();
		
	CTestData* CreateTestDataLC(TInt aTestCase);
	void TestIteratorL(CTestData& aTestData);
	
	// Add test data methods
	void AddRepeatingEventL(CTestData& aTestData);
	void AddTwoRepeatingEventsL(CTestData& aTestData);
	void AddRepeatingTodoL(CTestData& aTestData);
	void AddTwoRepeatingTodosL(CTestData& aTestData);
	void AddNonRepeatingEventL(CTestData& aTestData);
	void AddNonRepeatingTodoL(CTestData& aTestData);
	void AddRepeatingEventForMultipleFilesL(CTestData& aTestData);
	void AddNonRepeatingEventForMultipleFilesL(CTestData& aTestData);
	
	void AddRepeatWithChildL(CTestData& aTestData);
	void AddRepeatWithRDateL(CTestData& aTestData);
	void AddRepeatWithExDateL(CTestData& aTestData);
	
	// Negative tests
	void TestStartInstanceOutOfRangeL();
	void TestStartInstanceWrongSearchTextL();
	void TestStartInstanceWrongTypeL();
	void TestUseIncludeRptsNextInstanceOnlyL(); // Should fail with KErrNotSupported
	
	// Entries at the same time
	void TestInstancesAtTheSameTimeL();
	
	// Deleting entries
	void TestDeleteEntryBeforeIteratingL();
	void TestDeleteEntryWhilstIteratingL();
	
	// todos
	void TestTodosL();
	
	// stress tests
	void TestManyInstancesL();
	
	// Utility methods
	void PrintInstanceDetailsL(CCalInstance& aInstance);
	
	// Creating entries
	CCalEntry* CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount);
	CCalEntry* CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime);
	
	// OOM
	void OomTestL();
	void OomTestForMultipleFilesL();
	
	// Cleanup from second calendar files
	void ClearStoredDataL();
	
private: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);
	
private:
	CCalTestLibrary* iTestLibrary;
	TCalLocalUid iOOMLocalUid;
	CCalSession* iSecondSession;
	CCalEntryView* iSecondEntryView;
	CCalInstanceView* iInstanceView;
	RPointerArray<CCalSession> iSessions;
	TBool iMultipleFilesTest;
	};

CInstanceIteratorTestManager* CInstanceIteratorTestManager::NewLC()
	{
	CInstanceIteratorTestManager* self = new (ELeave) CInstanceIteratorTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

void CInstanceIteratorTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);
	
	iSecondSession = CCalSession::NewL(iTestLibrary->GetSession());
	iSecondSession->CreateCalFileL(KPersonalCalendar);
	iSecondSession->OpenL(KPersonalCalendar);
	
	iSecondEntryView = CCalEntryView::NewL(*iSecondSession);
	
	iSessions.AppendL(&iTestLibrary->GetSession());
	iSessions.AppendL(iSecondSession);
	
	iInstanceView = CCalInstanceView::NewL(iSessions);
	}	


CInstanceIteratorTestManager::~CInstanceIteratorTestManager()	
	{
	iSessions.Close();
	delete iInstanceView;
	delete iTestLibrary;
	
	delete iSecondEntryView;
	
	TRAP_IGNORE(iSecondSession->DeleteCalFileL(KPersonalCalendar));
	delete iSecondSession;
	}

void CInstanceIteratorTestManager::PrintInstanceDetailsL(CCalInstance& aInstance)
	{
	TBuf<100> instanceDetails;
	
	TCalRRule rRule;
	if (aInstance.Entry().GetRRuleL(rRule))
		{
		instanceDetails.Append( _L("Repeating "));
		}
	
	instanceDetails.AppendFormat( _L("Entry %d "),  aInstance.Entry().LocalUidL());
	instanceDetails.AppendFormat( _L("Short File id %d "),  aInstance.InstanceIdL().iCollectionId);
	
	instanceDetails.Append( (aInstance.Entry().EntryTypeL() == CCalEntry::ETodo) ? _L("Todo ") : _L("Non-Todo ") );
	
	TBuf<50> timeBuf;
	aInstance.Time().TimeLocalL().FormatL(timeBuf, KFormatDateTime());
	
	instanceDetails.Append(timeBuf);
	
	test.Printf(instanceDetails);
	}

void CInstanceIteratorTestManager::ClearStoredDataL()
	{
	CDesC8ArrayFlat* guidArray = new (ELeave) CDesC8ArrayFlat(2);
	CleanupStack::PushL(guidArray);

	CCalIter* iter = CCalIter::NewL(*iSecondSession);
	CleanupStack::PushL(iter);

	for (TPtrC8 Uid(iter->FirstL()); Uid != KNullDesC8(); Uid.Set(iter->NextL()))
		{
		guidArray->AppendL(Uid);
		}

	if (guidArray->Count() > 0)
		{
  		iSecondEntryView->DeleteL(*guidArray);
		}
  	
	CleanupStack::PopAndDestroy(iter);
	CleanupStack::PopAndDestroy(guidArray);
	}

void CInstanceIteratorTestManager::OomTestL()
	{
	iTestLibrary->CleanDatabaseL();
	
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent, KStartTimeOOMInstance, KNumOfOOMInstances);
	iTestLibrary->StoreEntryL(*entry);
	iOOMLocalUid = entry->LocalUidL();
	CleanupStack::PopAndDestroy(entry);
	
	OomTestL(EClient, 0);
	CCalSession& calSession = iTestLibrary->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	}

void CInstanceIteratorTestManager::OomTestForMultipleFilesL()
	{
	iMultipleFilesTest = ETrue;
	
	//Cleanup data
	ClearStoredDataL();
	iTestLibrary->CleanDatabaseL();
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent, KStartTimeOOMInstance, KNumOfOOMInstances/2);
	iTestLibrary->StoreEntryL(*entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numEntries(0);
	iSecondEntryView->StoreL(entries, numEntries);
	test(numEntries == 1);	
	CleanupStack::PopAndDestroy(&entries);
	
	OomTestL(EClient, 0);
	CCalSession& calSession = iTestLibrary->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	}

void CInstanceIteratorTestManager::OomTestL(TType aType, TInt aFailAt)
	{
	test.Printf(_L("OOM Test %d %d"), aType, aFailAt);
	
	// Setup the standard find instance settings
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(KStartTimeOOMInstance);
	
	CCalInstanceIterator* instanceIterator;
	if(iMultipleFilesTest)
		{
		instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings);
		}
	else
		{
		instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, iOOMLocalUid, startingInstanceTime);
		}
	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	CleanupStack::PushL(instanceIterator);
	
	TInt foundInstanceCount = instanceIterator->Count();
	if (foundInstanceCount >= 0)
		{
		test(KNumOfOOMInstances == foundInstanceCount);
		}
	else
		{
		User::Leave(foundInstanceCount);		
		}
	
	// HasMore is unreliable in OOM testing it will return false if there is a memory error
	// instanceIterator->HasMore();
	
	CCalInstance* instance = instanceIterator->NextL();
	while (instance)
		{
		delete instance;
		instance = instanceIterator->NextL();
		}
	
	test(instanceIterator->NextL() == NULL);
	
	instance = instanceIterator->PreviousL();
	while (instance)
		{
		delete instance;
		instance = instanceIterator->PreviousL();
		}
	
	test(instanceIterator->PreviousL() == NULL);
	
	CleanupStack::PopAndDestroy(instanceIterator);
	}

CCalEntry* CInstanceIteratorTestManager::CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount)
	{
	TCalTime entryStart;
	entryStart.SetTimeLocalL(aLocalStartTime);
	TCalTime entryEnd;
	entryEnd.SetTimeLocalL(aLocalStartTime + TTimeIntervalHours(1));
	
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLibrary->CreateCalEntryL(aEntryType, guid);
	CleanupStack::PushL(entry);
	
	entry->SetStartAndEndTimeL(entryStart, entryEnd);
	
	// Add the RRule
	TCalRRule rRule(TCalRRule::EDaily);
	rRule.SetDtStart(entryStart);
	rRule.SetCount(aRepeatCount);
	rRule.SetInterval(1);
	entry->SetRRuleL(rRule);
	
	return entry;
	}

CCalEntry* CInstanceIteratorTestManager::CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime)
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLibrary->CreateCalEntryL(aEntryType, guid);
	CleanupStack::PushL(entry);
	
	if (aLocalStartTime != Time::NullTTime())
		{
		TCalTime entryStart;
		entryStart.SetTimeLocalL(aLocalStartTime);
		TCalTime entryEnd;
		entryEnd.SetTimeLocalL(aLocalStartTime + TTimeIntervalHours(1));
		entry->SetStartAndEndTimeL(entryStart, entryEnd);
		}
	
	return entry;
	}

void CInstanceIteratorTestManager::AddRepeatingEventL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Non-Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid, CCalEntry::EEvent));
	}

void CInstanceIteratorTestManager::AddRepeatingEventForMultipleFilesL(CTestData& aTestData)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	test.Printf(_L("Add Repeating Non-Todo For Short File ID test"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numEntries(0);
	iSecondEntryView->StoreL(entries, numEntries);
	test(numEntries == 1);
	CleanupStack::PopAndDestroy(&entries);	
		
	TCalCollectionId firstFileId = iTestLibrary->GetSession().CollectionIdL();
	TCalCollectionId secondFileId = iSecondSession->CollectionIdL();
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	}

void CInstanceIteratorTestManager::AddTwoRepeatingEventsL(CTestData& aTestData)
	{
	test.Printf(_L("Add 2 Repeating Non-Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry1 = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry1);
	TCalLocalUid localUid1(entry1->LocalUidL());
	CleanupStack::PopAndDestroy(entry1);
	
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	CCalEntry* entry2 = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 24, 10, 30, 0, 0)),
			2);
	iTestLibrary->StoreEntryL(*entry2);
	TCalLocalUid localUid2(entry2->LocalUidL());
	CleanupStack::PopAndDestroy(entry2);
	
	// Set up the expected instances
	// TODO
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid1, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid1, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 10, 30, 0, 0)), localUid2, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid1, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 10, 30, 0, 0)), localUid2, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid1, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid1, CCalEntry::EEvent));
	}

void CInstanceIteratorTestManager::AddRepeatingTodoL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 22, 14, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), localUid, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), localUid, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 15, 30, 0, 0)), localUid, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), localUid, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), localUid, CCalEntry::ETodo));
	}

void CInstanceIteratorTestManager::AddTwoRepeatingTodosL(CTestData& aTestData)
	{
	test.Printf(_L("Add 2 Repeating Todos"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry1 = CreateRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 22, 14, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry1);
	TCalLocalUid localUid1(entry1->LocalUidL());
	CleanupStack::PopAndDestroy(entry1);
	
	CCalEntry* entry2 = CreateRepeatingEntryLC(CCalEntry::ETodo,
			TTime(TDateTime(2006, ENovember, 23, 17, 45, 0, 0)),
			2);
	iTestLibrary->StoreEntryL(*entry2);
	TCalLocalUid localUid2(entry2->LocalUidL());
	CleanupStack::PopAndDestroy(entry2);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), localUid1, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), localUid1, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 18, 45, 0, 0)), localUid2, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 15, 30, 0, 0)), localUid1, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 18, 45, 0, 0)), localUid2, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), localUid1, CCalEntry::ETodo));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), localUid1, CCalEntry::ETodo));
	}

void CInstanceIteratorTestManager::AddNonRepeatingEventL(CTestData& aTestData)
	{
	test.Printf(_L("Add Non-Repeating Non-Todo"));
	
	// Daily repeating entry from 26 November
	CCalEntry* entry = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 25, 16, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 16, 30, 0, 0)), localUid, CCalEntry::EEvent));
	}

void CInstanceIteratorTestManager::AddNonRepeatingTodoL(CTestData& aTestData)
	{
	test.Printf(_L("Add Non-Repeating Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateNonRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 26, 9, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 10, 30, 0, 0)), localUid, CCalEntry::ETodo));
	}

void CInstanceIteratorTestManager::AddNonRepeatingEventForMultipleFilesL(CTestData& aTestData)
	{
	test.Printf(_L("Add Non-Repeating Non-Todo"));
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);	
		
	// Daily repeating entry from 26 November
	CCalEntry* entry = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 25, 16, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numEntries(0);
	iSecondEntryView->StoreL(entries, numEntries);
	test(numEntries == 1);
	CleanupStack::PopAndDestroy(&entries);
	TCalCollectionId firstFileId = iTestLibrary->GetSession().CollectionIdL();
	TCalCollectionId secondFileId = iSecondSession->CollectionIdL();
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 16, 30, 0, 0)), localUid, CCalEntry::EEvent, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 16, 30, 0, 0)), localUid, CCalEntry::EEvent, secondFileId));
	}

void CInstanceIteratorTestManager::AddRepeatWithChildL(CTestData& aTestData)
	{
	test.Printf(_L("Add repeating event with a child"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* parentEntry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 13, 36, 0, 0)),
												5);
	
	TCalTime recId;
	recId.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 13, 36, 0, 0)));
	
	HBufC8* guid = parentEntry->UidL().AllocLC();
	
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(childEntry);
	
	iTestLibrary->SetEntryStartAndEndTimeL(childEntry,
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)),
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)));
	
	iTestLibrary->StoreEntryL(*parentEntry);
	TCalLocalUid parentLocalUid(parentEntry->LocalUidL());
	
	
	iTestLibrary->StoreEntryL(*childEntry);
	TCalLocalUid childLocalUid(childEntry->LocalUidL());
	CleanupStack::PopAndDestroy(childEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 13, 36, 0, 0)), parentLocalUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 13, 36, 0, 0)), parentLocalUid, CCalEntry::EEvent));
	
	//aTestData.InsertExpectedInstanceL(TTime(TDateTime(2006, ENovember, 24, 13, 36, 0, 0)));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)), childLocalUid, CCalEntry::EEvent));
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 13, 36, 0, 0)), parentLocalUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 13, 36, 0, 0)), parentLocalUid, CCalEntry::EEvent));
	}

void CInstanceIteratorTestManager::AddRepeatWithRDateL(CTestData& aTestData)
	{
	test.Printf(_L("Add repeating event with an rdate"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 9, 45, 0, 0)),
												5);
	
	TCalTime rDate;
	rDate.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 23, 17, 30, 0, 0)));
	
	RArray<TCalTime> rDates;
	CleanupClosePushL(rDates);
	rDates.AppendL(rDate);
	
	entry->SetRDatesL(rDates);
	CleanupStack::PopAndDestroy(&rDates);
	
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 9, 45, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 9, 45, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 9, 45, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 9, 45, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 9, 45, 0, 0)), localUid, CCalEntry::EEvent));
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 17, 30, 0, 0)), localUid, CCalEntry::EEvent)); // the rDate
	}

void CInstanceIteratorTestManager::AddRepeatWithExDateL(CTestData& aTestData)
	{
	test.Printf(_L("Add repeating event with an excepted date"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 18, 15, 0, 0)),
												5);
	
	TCalTime exDate;
	exDate.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 18, 15, 0, 0)));
	
	RArray<TCalTime> exDates;
	CleanupClosePushL(exDates);
	exDates.AppendL(exDate);
	
	entry->SetExceptionDatesL(exDates);
	CleanupStack::PopAndDestroy(&exDates);
	
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 18, 15, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 18, 15, 0, 0)), localUid, CCalEntry::EEvent));
	// aTestData.InsertExpectedInstanceL(TTime(TDateTime(2006, ENovember, 24, 18, 15, 0, 0))); // the exDate
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 18, 15, 0, 0)), localUid, CCalEntry::EEvent));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 18, 15, 0, 0)), localUid, CCalEntry::EEvent));
	}

CTestData* CInstanceIteratorTestManager::CreateTestDataLC(TInt aTestCase)
	{
	CTestData* testData = new (ELeave) CTestData;
	CleanupStack::PushL(testData);
	
	// Setup the standard find instance settings
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	testData->iFindInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	testData->iTestType = CTestData::ESingleCalendarFileTest;
	// create the test data
	switch (aTestCase)
		{
		case ERepeatingEvent:
			AddRepeatingEventL(*testData);
			break;
		case ETwoRepeatingEvents:
			AddTwoRepeatingEventsL(*testData);
			break;
		case ERepeatingTodo:
			AddRepeatingTodoL(*testData);
			break;
		case ETwoRepeatingTodos:
			AddTwoRepeatingTodosL(*testData);
			break;
		case ESingleEvent:
			AddNonRepeatingEventL(*testData);
			break;
		case ESingleTodo:
			AddNonRepeatingTodoL(*testData);
			break;
		case ERepeatingTodoAndSingleTodo:
			AddRepeatingTodoL(*testData);
			AddNonRepeatingTodoL(*testData);
			break;
		case ERepeatingTodoAndSingleEvent:
			AddRepeatingTodoL(*testData);
			AddNonRepeatingEventL(*testData);
			break;
		case ERepeatingEventAndSingleTodo:
			AddRepeatingEventL(*testData);
			AddNonRepeatingTodoL(*testData);
			break;
		case ERepeatingEventAndSingleEvent:
			AddRepeatingEventL(*testData);
			AddNonRepeatingEventL(*testData);
			break;
		case ERepeatingEventAndRepeatingTodo:
			AddRepeatingTodoL(*testData);
			AddRepeatingEventL(*testData);
			break;
		case ERepeatWithChild:
			AddRepeatWithChildL(*testData);
			break;
		case ERepeatWithRDate:
			AddRepeatWithRDateL(*testData);
			break;
		case ERepeatWithExDate:
			AddRepeatWithExDateL(*testData);
			break;
		case ERepeatingEventAndSingleTodoWithShortFileId:
			{
			testData->iTestType = CTestData::EMultipleCalendarFilesTest;
			AddRepeatingEventForMultipleFilesL(*testData);
			AddNonRepeatingEventForMultipleFilesL(*testData);			
			}
			break;
		default:
			{
			test.Printf(_L("Invalid test case!"));
			test(EFalse);			
			}
			break;
		};
	
	return testData;
	}

void CInstanceIteratorTestManager::TestIteratorL(CTestData& aTestData)
	{
	const TInt KExpectedInstanceCount( aTestData.iExpectedInstances.Count() );
	
	TCalInstanceId instanceId;
	
	for (TInt startingInstanceIndex(0) ; startingInstanceIndex < KExpectedInstanceCount ; ++startingInstanceIndex)
		{
		// We will create an iterator for every instance in the test data
		// then test iterating forwards and backwards
		
		test.Printf(_L("Create iterator and iterate forwards using NextL()"));
		
		CTestData::TExpectedInstance startInstance = aTestData.iExpectedInstances[startingInstanceIndex];
		instanceId.iEntryLocalId = startInstance.iLocalUid;
		instanceId.iInstanceTime = startInstance.iTime;
		instanceId.iCollectionId = startInstance.iShortFileId;
		
		// Call HasMore between each step to check the result
		CCalInstanceIterator* instanceIterator = NULL;
		if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
			{
			instanceIterator = iInstanceView->FindInstanceL(*aTestData.iFindInstanceSettings, instanceId);
			}
		else
			{
			instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*aTestData.iFindInstanceSettings, startInstance.iLocalUid, startInstance.iTime);
			}
			
		CleanupStack::PushL(instanceIterator);
		
		test.Printf(_L("test that count returned the correct number of instances"));
		TInt foundInstanceCount = instanceIterator->Count();
		test(foundInstanceCount == KExpectedInstanceCount);
		
		TInt expectedInstancesForwards(KExpectedInstanceCount - startingInstanceIndex - 1);
		TInt foundInstancesForwards(0);
		
		while (instanceIterator->HasMore())
			{
			CCalInstance* instance = instanceIterator->NextL();
			CleanupStack::PushL(instance);
			
			PrintInstanceDetailsL(*instance);
			
			if (aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iTime.TimeLocalL() == Time::NullTTime())
				{
				test.Printf(_L("This instance is an undated todo"));
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iLocalUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iShortFileId);
				}
			else
				{
				test.Printf(_L("Test this instance is the correct expected one"));
				test(instance->Time().TimeLocalL() == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iTime.TimeLocalL());
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iLocalUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iShortFileId);
				}
			
			CleanupStack::PopAndDestroy(instance);
			foundInstancesForwards++;
			}
		
		test.Printf(_L("test NextL() returns NULL when at the end"));
		test(instanceIterator->NextL() == NULL);
		
		test.Printf(_L("test the correct number of instances were found when iterating forwards"));
		test(foundInstancesForwards == expectedInstancesForwards);
		
		CleanupStack::PopAndDestroy(instanceIterator);
		
		test.Printf(_L("Create iterator and iterate backwards using PreviousL()"));
		if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
			{
			instanceIterator = iInstanceView->FindInstanceL(*aTestData.iFindInstanceSettings, instanceId);
			}
		else
			{
			instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*aTestData.iFindInstanceSettings, startInstance.iLocalUid, startInstance.iTime);
			}
					
		CleanupStack::PushL(instanceIterator);
		
		TInt expectedInstancesBackwards(startingInstanceIndex);
		TInt foundInstancesBackwards(0);
		
		while (CCalInstance* instance = instanceIterator->PreviousL())
			{
			CleanupStack::PushL(instance);
			
			PrintInstanceDetailsL(*instance);
			
			if (aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iTime.TimeLocalL() == Time::NullTTime())
				{
				test.Printf(_L("This instance is an undated todo"));
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iLocalUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iShortFileId);
				}
			else
				{
				test.Printf(_L("Test this instance is the correct expected one"));
				test(instance->Time().TimeLocalL() == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iTime.TimeLocalL());
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iLocalUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iShortFileId);
				}
		
			CleanupStack::PopAndDestroy(instance);
			foundInstancesBackwards++;
			}
		
		test.Printf(_L("test PreviousL() returns NULL when at the beginning"));
		test(instanceIterator->PreviousL() == NULL);
		
		test.Printf(_L("test the corret number of instances were found when iterating backwards"));
		test(foundInstancesBackwards == expectedInstancesBackwards);
		
		CleanupStack::PopAndDestroy(instanceIterator);
		}
	}

void CInstanceIteratorTestManager::TestStartInstanceOutOfRangeL()
	{
	iTestLibrary->CleanDatabaseL();
	
	TTime startingInstanceTimeLocal(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												startingInstanceTimeLocal,
												5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// create a time range which does not include the first instance
	TCalTime startRange;
	startRange.SetTimeLocalL(startingInstanceTimeLocal + TTimeIntervalDays(1));
	TCalTime endRange;
	endRange.SetTimeLocalL(startingInstanceTimeLocal + TTimeIntervalDays(17));
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	// Find the first instance using the normal FindInstanceL
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(startingInstanceTimeLocal);
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, localUid, startingInstanceTime);
	CleanupStack::PushL(instanceIterator);
	TRAPD(err, instanceIterator->NextL());
	
	test(err == KErrNotFound);
	
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestStartInstanceWrongSearchTextL()
	{
	_LIT(KEntryText, "This is some text for he description");
	_LIT(KSearchText, "This is some text to match on");
	
	iTestLibrary->CleanDatabaseL();
	
	TTime startingInstanceTimeLocal(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												startingInstanceTimeLocal,
												5);
	
	entry->SetDescriptionL(KEntryText);
	
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// create some filter settings that has a different search text
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);

	CCalInstanceView::TCalSearchParams searchParams(KSearchText, CalCommon::EFoldedTextSearch);
	findInstanceSettings->SetTextSearchL(searchParams);
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(startingInstanceTimeLocal);

	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, localUid, startingInstanceTime);
	CleanupStack::PushL(instanceIterator);
	TRAPD(err, instanceIterator->NextL());
	
	test(err == KErrNotFound);
	
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}
	
void CInstanceIteratorTestManager::TestStartInstanceWrongTypeL()
	{
	iTestLibrary->CleanDatabaseL();
	
	TTime startingInstanceTimeLocal(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												startingInstanceTimeLocal,
												5);
	
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// create some filter settings that has a different search text
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll ^ CalCommon::EIncludeEvents, timeRange);
	CleanupStack::PushL(findInstanceSettings);

	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(startingInstanceTimeLocal);

	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, localUid, startingInstanceTime);
	CleanupStack::PushL(instanceIterator);
	TRAPD(err, instanceIterator->NextL());
	
	test(err == KErrNotFound);
	
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestUseIncludeRptsNextInstanceOnlyL()
	{
	// Should fail with KErrNotSupported
	iTestLibrary->CleanDatabaseL();
	
	TTime startingInstanceTimeLocal(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent, startingInstanceTimeLocal, 5);
	
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Create some filter settings that has a different search text
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll ^ CalCommon::EIncludeRptsNextInstanceOnly, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(startingInstanceTimeLocal);
	
	CCalInstanceIterator* instanceIterator = NULL;
	TRAPD(err, instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, localUid, startingInstanceTime));
	CleanupStack::PushL(instanceIterator);
	
	test(err == KErrNotSupported);
	
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

// Entries at the same time
void CInstanceIteratorTestManager::TestInstancesAtTheSameTimeL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// Create a repeating todo, a repeating event, a single todo, a single event
	TTime startingInstanceTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
	// Daily repeating entry from 23 November for 10 instances
	CCalEntry* entry1 = CreateRepeatingEntryLC(CCalEntry::EEvent, startingInstanceTime, 10);
	iTestLibrary->StoreEntryL(*entry1);
	TCalLocalUid localUid1(entry1->LocalUidL());
	CleanupStack::PopAndDestroy(entry1);
	
	// a single event at the same time as one of the repeats
	CCalEntry* entry2 = CreateNonRepeatingEntryLC(CCalEntry::EEvent, startingInstanceTime + TTimeIntervalDays(3));
	iTestLibrary->StoreEntryL(*entry2);
	TCalLocalUid localUid2(entry2->LocalUidL());
	CleanupStack::PopAndDestroy(entry2);
	
	// a single todo at the same time as one of the repeats
	CCalEntry* entry3 = CreateNonRepeatingEntryLC(CCalEntry::ETodo, startingInstanceTime + TTimeIntervalDays(5) - TTimeIntervalHours(1));
	iTestLibrary->StoreEntryL(*entry3);
	TCalLocalUid localUid3(entry3->LocalUidL());
	CleanupStack::PopAndDestroy(entry3);
	
	// Daily repeating entry from 3 December for 10 instances
	CCalEntry* entry4 = CreateRepeatingEntryLC(CCalEntry::EEvent, startingInstanceTime + TTimeIntervalDays(8), 10);
	iTestLibrary->StoreEntryL(*entry4);
	TCalLocalUid localUid4(entry4->LocalUidL());
	CleanupStack::PopAndDestroy(entry4);
	
	
	// Create the test data, including findinstance settings and expected instances
	CTestData* testData = new(ELeave) CTestData;
	CleanupStack::PushL(testData);
	
	TCalTime startRange;
	startRange.SetTimeLocalL(startingInstanceTime - TTimeIntervalDays(5));
	TCalTime endRange;
	endRange.SetTimeLocalL(startingInstanceTime + TTimeIntervalDays(20));
	CalCommon::TCalTimeRange searchTimeRange(startRange, endRange);
	testData->iFindInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, searchTimeRange);
	
	// instances of entry 2
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(3), localUid2, CCalEntry::EEvent));
	
	// instances of entry 3
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(5), localUid3, CCalEntry::ETodo));
	
	// instances of entry 1
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(0), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(1), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(2), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(3), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(4), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(5), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(6), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(7), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(8), localUid1, CCalEntry::EEvent));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(9), localUid1, CCalEntry::EEvent));
	
		// instances of entry 4
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(8), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(9), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(10), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(11), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(12), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(13), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(14), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(15), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(16), localUid4, CCalEntry::ETodo));
	testData->InsertExpectedInstanceL(CTestData::TExpectedInstance(startingInstanceTime + TTimeIntervalDays(17), localUid4, CCalEntry::ETodo));
	
	// Test that the expected instances come up in the correct order
	// Should be able to iterate through all the instances, even if they appear at the same time
	TestIteratorL(*testData);
	
	CleanupStack::PopAndDestroy(testData);
	}

// Deleting entries
void CInstanceIteratorTestManager::TestDeleteEntryBeforeIteratingL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), 5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
		
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)));
	CCalInstanceIterator* instanceIterator = 0;
	TRAPD(err, instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings,
			localUid, startingInstanceTime));
	CleanupStack::PushL(instanceIterator);
	test(err == KErrNone);
	
	// Delete the entry from the database
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry);
	
	// NextL should leave with KErrNotFound
	TRAP(err, instanceIterator->NextL());
	test(err == KErrNotFound);
		
	CleanupStack::PopAndDestroy(3, entry);
	}

void CInstanceIteratorTestManager::TestDeleteEntryWhilstIteratingL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// Daily repeating entry from 23 Novemeber to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), 5);
	iTestLibrary->StoreEntryL(*entry);
	TCalLocalUid localUid(entry->LocalUidL());
		
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)));
	CCalInstanceIterator* instanceIterator = 0;
	TRAPD(err, instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings,
			localUid, startingInstanceTime));
	CleanupStack::PushL(instanceIterator);
	test(err == KErrNone);
	
	// Get one instance then delete the entry from database
	CCalInstance* instance = 0;
	TRAP(err, instance = instanceIterator->NextL());
	test(err == KErrNone);
	delete instance;	
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry);
	TRAP(err, instance = instanceIterator->NextL());
	test(err == KErrNotFound);
		
	CleanupStack::PopAndDestroy(3);
	}

void CInstanceIteratorTestManager::TestManyInstancesL()
	{
	iTestLibrary->CleanDatabaseL();
	
	const TInt KNumOfEntries(100);
	const TInt KNumOfInstancesPerEntry(100);
	
	TTime startingInstanceTimeLocal(TTime(TDateTime(1996, ENovember, 12, 11, 30, 0, 0)));
	
	// Create the test data, including findinstance settings and expected instances
	CTestData* testData = new(ELeave) CTestData;
	CleanupStack::PushL(testData);
	
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange searchTimeRange(startRange, endRange);
	testData->iFindInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, searchTimeRange);
	
	TCalLocalUid localUid;
	
	test.Printf(_L("Start storing the entries"));
	for (TInt i(0) ; i < KNumOfEntries ; ++i)
		{		
		CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent, startingInstanceTimeLocal + TTimeIntervalMinutes(i), KNumOfInstancesPerEntry);
		iTestLibrary->StoreEntryL(*entry);
		
		if (i == 0)
			{
			localUid = entry->LocalUidL();			
			}
		
		CleanupStack::PopAndDestroy(entry);
		
		test.Printf(_L("."));
		}
	
	// create the iterator
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(startingInstanceTimeLocal);
	
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*testData->iFindInstanceSettings, localUid, startingInstanceTime);
	CleanupStack::PushL(instanceIterator);
	
	test.Printf(_L("test that count returned the correct number of instances"));
	TInt foundInstanceCount = instanceIterator->Count();
	
	TInt foundInstancesForwards(0);
	
	test.Printf(_L("iterating forwards"));
	while (instanceIterator->HasMore())
		{
		delete instanceIterator->NextL();
		foundInstancesForwards++;
		test.Printf(_L("."));
		}
	
	test.Printf(_L("test the correct number of instances were found when iterating forwards"));
	test.Printf(_L("foundInstancesForwards = %d"), foundInstancesForwards);
	test( foundInstancesForwards == ((KNumOfEntries * KNumOfInstancesPerEntry) - 1) );
		
	TInt foundInstancesBackwards(0);
	
	test.Printf(_L("iterating backwards"));
	while (CCalInstance* instance = instanceIterator->PreviousL())
		{
		delete instance;
		foundInstancesBackwards++;
		test.Printf(_L("."));
		}
	
	test.Printf(_L("test the corret number of instances were found when iterating backwards"));
	test.Printf(_L("foundInstancesBackwards = %d"), foundInstancesBackwards);
	test( foundInstancesBackwards == ((KNumOfEntries * KNumOfInstancesPerEntry) - 1) );
	
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(testData);
	}

void CInstanceIteratorTestManager::TestTodosL()
	{
	iTestLibrary->CleanDatabaseL();
	
	TTime now;
	now.HomeTime();
	
	// a single undated todo 
	CCalEntry* entry1 = CreateNonRepeatingEntryLC(CCalEntry::ETodo, Time::NullTTime());
	iTestLibrary->StoreEntryL(*entry1);
	TCalLocalUid localUid1(entry1->LocalUidL());
	CleanupStack::PopAndDestroy(entry1);
	
	// a single undated todo 
	CCalEntry* entry2 = CreateNonRepeatingEntryLC(CCalEntry::ETodo, Time::NullTTime());
	iTestLibrary->StoreEntryL(*entry2);
	TCalLocalUid localUid2(entry2->LocalUidL());
	CleanupStack::PopAndDestroy(entry2);
	
	// a single completed todo 
	CCalEntry* entry3 = CreateNonRepeatingEntryLC(CCalEntry::ETodo, now + TTimeIntervalDays(3));
	TCalTime completedTime3;
	completedTime3.SetTimeLocalL(now + TTimeIntervalDays(3) + TTimeIntervalHours(3));
	entry3->SetCompletedL(ETrue, completedTime3);
	iTestLibrary->StoreEntryL(*entry3);
	TCalLocalUid localUid3(entry3->LocalUidL());
	CleanupStack::PopAndDestroy(entry3);
	
	// a single todo 
	CCalEntry* entry4 = CreateNonRepeatingEntryLC(CCalEntry::ETodo, now + TTimeIntervalDays(4));
	iTestLibrary->StoreEntryL(*entry4);
	TCalLocalUid localUid4(entry4->LocalUidL());
	CleanupStack::PopAndDestroy(entry4);
	
	// test that undated todos are found when the search range include 'now'
	CTestData* testData1 = new(ELeave) CTestData;
	CleanupStack::PushL(testData1);
	
	TCalTime startRange1;
	startRange1.SetTimeLocalL(now - TTimeIntervalDays(7));
	TCalTime endRange1;
	endRange1.SetTimeLocalL(now + TTimeIntervalDays(7));
	CalCommon::TCalTimeRange searchTimeRange1(startRange1, endRange1);
	testData1->iFindInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, searchTimeRange1);
	
	// the expected undated todos
	testData1->InsertExpectedInstanceL(CTestData::TExpectedInstance(Time::NullTTime(), localUid1, CCalEntry::ETodo));
	testData1->InsertExpectedInstanceL(CTestData::TExpectedInstance(Time::NullTTime(), localUid2, CCalEntry::ETodo));
	
	// the expected dated todos
	testData1->InsertExpectedInstanceL(CTestData::TExpectedInstance(now + TTimeIntervalDays(3) + TTimeIntervalHours(3), localUid3, CCalEntry::ETodo));
	testData1->InsertExpectedInstanceL(CTestData::TExpectedInstance(now + TTimeIntervalDays(4) + TTimeIntervalHours(1), localUid4, CCalEntry::ETodo));
	
	TestIteratorL(*testData1);
	CleanupStack::PopAndDestroy(testData1);
	
	// test that undated todos are not found when the search range does not include now
	CTestData* testData2 = new(ELeave) CTestData;
	CleanupStack::PushL(testData2);
	
	TCalTime startRange2;
	startRange2.SetTimeLocalL(now + TTimeIntervalDays(2));
	TCalTime endRange2;
	endRange2.SetTimeLocalL(now + TTimeIntervalDays(7));
	CalCommon::TCalTimeRange searchTimeRange2(startRange2, endRange2);
	testData2->iFindInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, searchTimeRange2);
	
	// the expected dated todos
	testData2->InsertExpectedInstanceL(CTestData::TExpectedInstance(now + TTimeIntervalDays(3) + TTimeIntervalHours(3), localUid3, CCalEntry::ETodo));
	testData2->InsertExpectedInstanceL(CTestData::TExpectedInstance(now + TTimeIntervalDays(4) + TTimeIntervalHours(1), localUid4, CCalEntry::ETodo));
	
	TestIteratorL(*testData2);
	CleanupStack::PopAndDestroy(testData2);
	}

void CInstanceIteratorTestManager::RunTestsL()
	{
	// Run all the normal tests
	for (TInt testCase(0) ; testCase < ENumOfTests ; ++testCase)
		{

		test.Next(_L("Testing ordinary instances"));
		
		iTestLibrary->CleanDatabaseL();
		CTestData* testData = CreateTestDataLC(testCase);
		TestIteratorL(*testData);
		CleanupStack::PopAndDestroy(testData);
		}	
		
	// Some negative tests
	test.Next(_L("TestStartInstanceOutOfRangeL"));
	TestStartInstanceOutOfRangeL();

	test.Next(_L("TestStartInstanceWrongSearchTextL"));
	TestStartInstanceWrongSearchTextL();

	test.Next(_L("TestStartInstanceWrongTypeL"));
	TestStartInstanceWrongTypeL();

	test.Next(_L("TestUseIncludeRptsNextInstanceOnlyL"));
	TestUseIncludeRptsNextInstanceOnlyL();
	
	// Test instances that appear at the same time

	test.Next(_L("TestInstancesAtTheSameTimeL"));
	TestInstancesAtTheSameTimeL();
		
	// Test deleting entries before and during iterations

	test.Next(_L("TestDeleteEntryBeforeIteratingL"));
	TestDeleteEntryBeforeIteratingL();
	
	test.Next(_L("TestDeleteEntryWhilstIteratingL"));
	TestDeleteEntryWhilstIteratingL();
	
	test.Next(_L("TestManyInstancesL"));
	TestManyInstancesL();

	test.Next(_L("TestTodosL"));
	TestTodosL();
	
#ifdef _DEBUG
	// Out of memory testing

	test.Next(_L("OOM test"));
	OomTestL();
	
	test.Next(_L("OOM test for multiple files"));
	OomTestForMultipleFilesL();
#endif
	}
	
static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	
	testManager->RunTestsL();
	
	CleanupStack::PopAndDestroy(testManager);
	}

/**
@SYMTestCaseID     PIM-TCAL-INSTANCEITERATOR-ALL-0001
*/

TInt E32Main()
    {
	__UHEAP_MARK;
	
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-INSTANCEITERATOR-ALL-0001 tcal_instanceiterator_all"));

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (!scheduler)
		{
		test(EFalse);
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	
	TInt ret(KErrNone);
	TRAP(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return ret;
    }
