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

#include "../caltestlib.h"
#include "../caltestoom.h"

#include <e32test.h>
#include <e32math.h>
#include <caluser.h>
#include <calcommon.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calinstanceiterator.h>
#include <calsession.h>

_LIT(KTestName, "tcal_sortedinstanceiterator");
RTest test(KTestName);
_LIT(KPersonalCalendar, "Personal_calendar");

_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S.%C");

const TInt KInstanceCount(5);
const TInt KMultipleCalendarInstanceCount(10);

const TInt KFirstFileId = 1;
const TInt KSecondFileId = 2;

#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
class CInstanceIteratorTestManager : public CBase, public MCalTestOomCallBack
	{
public:
	enum TTestType
		{
		ESingleCalendarFileTest,
		EMultipleCalendarFilesTest
		};
public:
    static CInstanceIteratorTestManager* NewLC();
	~CInstanceIteratorTestManager();
		
	void RunTestsL();
	
private:
	void ConstructL();
	CCalEntry* CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount, TCalRRule::TType aRepeatType);
	CCalEntry* CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount);
	CCalEntry* CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TBool aFloatingTime);
	CCalEntry* CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime);
	
	void ValidateSortingL(RPointerArray<CCalInstance>& aFoundInstances, const CCalSortCriteria& aSortCriteria);
	void FindInstancesL(RPointerArray<CCalInstance>& aInstances, const CCalFindInstanceSettings& aSettings);
	void PrintInstanceDetailsL(CCalInstance& aInstance);
	void CheckInstancesL(CCalFindInstanceSettings& aFindInstanceSetting, TCalInstanceId aId, TInt aExpectedCount);
	TInt CompareL(const CCalInstance& aFirst, const CCalInstance& aSecond, const CCalSortCriterion& aSortCriterion);
	TInt CompareEntryTypeL(const CCalInstance& aFirst, const CCalInstance& aSecond, CalCommon::TCalSortDirection aSortDirection, const RArray<CCalEntry::TType>& aTypeOrders);
	TInt CompareShortFileIdL(const CCalInstance& aFirst, const CCalInstance& aSecond);
	void TestSimpleSortL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir);
	void TestMultipleSortL(CalCommon::TCalSortAttribute aFirstAttr, CalCommon::TCalSortDirection aFirstDir, CalCommon::TCalSortAttribute aSecondAttr, CalCommon::TCalSortDirection aSecondDir);	
	static TInt CompareTTime(TTime aFirst, TTime aSecond);
	
	void AddTestingDataL();
	void AddTestDataForMultipleFilesL();
	void TestFindInstanceL(CalCommon::TCalViewFilter aFilter, const TTime& aStart, const TTime& aEnd, TInt aExpectedCount);
	
	//Filter funtions testing.
	void TestRepeatingEventsL(TInt aExpectedInstanceCount);
	void TestRepeatingTodosL();
	void TestSingleEventL(TInt aExpectedInstanceCount);
	void TestSingleTodoL();
	void TestRepeatWithChildL();
	void TestAllTypesL();
	
	//Simple sort testing
	void TestSortByStartTimeL(); 
	void TestSortByEndTimeL();
	void TestSortByLastModifiedTimeL();
	void TestSortByPriorityL();
	void TestSortByTypesL();
	
	//Combined sort testing
	void TestSortByPriortiyThenStartTimeL();
	void TestSortByLastModifiedDateThenPriorityL();
	void TestSortByTypesThenPriorityL();

	//Test against default sort order
	void TestDefaultSortOrderL();
	void TestDefaultSortOrderWithInstanceIdL();
	
	//Test sorting with floating entries
	void TestSortOnFloatingEntriesL();
	
	//Iterator cache strategy test
	void TestCacheSizeL();
	
	//Test filtering on prioirty
	void TestFileringPriorityL();
	
	//Combined searching and sorting tests
	void TestCombinedSearchAndSortL();

	//Out Of Memory test
	void OomTestL(TType aType, TInt aFailAt);
	void TestSortedIteratorOOML();
	
private:
	CCalTestLibrary* iTestLibrary;
	RArray<CCalEntry::TType> iDefaultTypeOrder;
	CCalSession* iSecondSession;
	CCalEntryView* iSecondEntryView;
	CCalInstanceView* iInstanceView;
	RPointerArray<CCalSession> iSessions;
	TTestType iTestType;
	TCalInstanceId iId1;
	TCalInstanceId iId2;
	TCalFileId iFileId1;
	TCalFileId iFileId2;
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
	iDefaultTypeOrder.AppendL(CCalEntry::ETodo);
	iDefaultTypeOrder.AppendL(CCalEntry::EAnniv);
	iDefaultTypeOrder.AppendL(CCalEntry::EEvent);
	iDefaultTypeOrder.AppendL(CCalEntry::EAppt);
	iDefaultTypeOrder.AppendL(CCalEntry::EReminder);
	
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
	iDefaultTypeOrder.Close();
	iSessions.Close();
	delete iInstanceView;
	delete iTestLibrary;	
	delete iSecondEntryView;
	TRAP_IGNORE(iSecondSession->DeleteCalFileL(KPersonalCalendar));
	delete iSecondSession;
	}

void CInstanceIteratorTestManager::AddTestingDataL()
	{
	//Create a repeating event
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2007, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);

	//Create a repeating Todo
	entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2007, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);

	//Create a non-repeating event
	entry = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
									TTime(TDateTime(2006, EOctober, 16, 9, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);

	//Create a non-repeating Todo
	entry = CreateNonRepeatingEntryLC(CCalEntry::ETodo,
									TTime(TDateTime(2006, EOctober, 28, 9, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	//Create repeating event with children event
	CCalEntry* parentEntry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2005, EOctober, 16, 13, 36, 0, 0)),
												KInstanceCount);
	
	TCalTime recId;
	recId.SetTimeLocalL(TTime(TDateTime(2005, EOctober, 18, 13, 36, 0, 0)));
	
	HBufC8* guid = parentEntry->UidL().AllocLC();
	
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(childEntry);
	
	iTestLibrary->SetEntryStartAndEndTimeL(childEntry,
											TTime(TDateTime(2005, EOctober, 18, 17, 36, 0, 0)),
											TTime(TDateTime(2005, EOctober, 18, 17, 36, 0, 0)));
	
	iTestLibrary->StoreEntryL(*parentEntry);
	TCalLocalUid parentLocalUid(parentEntry->LocalUidL());
	
	iTestLibrary->StoreEntryL(*childEntry);
	TCalLocalUid childLocalUid(childEntry->LocalUidL());
	CleanupStack::PopAndDestroy(childEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	
	CCalEntry* repeatingEvent = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2002, ENovember, 16, 11, 30, 0, 0)),
											  KInstanceCount, TCalRRule::EYearly);
	iTestLibrary->StoreEntryL(*repeatingEvent);
	CleanupStack::PopAndDestroy(repeatingEvent);

	CCalEntry* singleEvent = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2003, EDecember, 16, 11, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*singleEvent);
	CleanupStack::PopAndDestroy(singleEvent);
	
	CCalEntry* repeatingTodo = CreateRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2002, ESeptember, 16, 11, 30, 0, 0)),
											  KInstanceCount, TCalRRule::EYearly);
	iTestLibrary->StoreEntryL(*repeatingTodo);
	CleanupStack::PopAndDestroy(repeatingTodo);
	
	CCalEntry* singleTodo = CreateNonRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2003, EAugust, 16, 11, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*singleTodo);
	CleanupStack::PopAndDestroy(singleTodo);

	CCalEntry* repeatingAppointment = CreateRepeatingEntryLC(CCalEntry::EAppt,
											  TTime(TDateTime(2002, EJuly, 16, 11, 30, 0, 0)),
											  KInstanceCount, TCalRRule::EYearly);
	iTestLibrary->StoreEntryL(*repeatingAppointment);
	CleanupStack::PopAndDestroy(repeatingAppointment);

	CCalEntry* singleAppointment = CreateNonRepeatingEntryLC(CCalEntry::EAppt,
											  TTime(TDateTime(2003, ENovember, 15, 11, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*singleAppointment);
	CleanupStack::PopAndDestroy(singleAppointment);
	
	CCalEntry* anniversary  = CreateRepeatingEntryLC(CCalEntry::EAnniv,
											  TTime(TDateTime(2002, EJune, 16, 11, 30, 0, 0)),
											  KInstanceCount, TCalRRule::EYearly);
	iTestLibrary->StoreEntryL(*anniversary);
	CleanupStack::PopAndDestroy(anniversary);

	CCalEntry* repeatingReminder = CreateRepeatingEntryLC(CCalEntry::EReminder,
											  TTime(TDateTime(2002, EApril, 16, 11, 30, 0, 0)),
											  KInstanceCount, TCalRRule::EYearly);
	iTestLibrary->StoreEntryL(*repeatingReminder);
	CleanupStack::PopAndDestroy(repeatingReminder);

	CCalEntry* singleReminder = CreateNonRepeatingEntryLC(CCalEntry::EReminder,
											  TTime(TDateTime(2003, EJune, 17, 11, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*singleReminder);
	CleanupStack::PopAndDestroy(singleReminder);
	}

void CInstanceIteratorTestManager::AddTestDataForMultipleFilesL()
	{
	iTestLibrary->CleanDatabaseL();
		
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	//Create a repeating event
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2007, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*entry);
	iId1.iEntryLocalId = entry->LocalUidL();
	iId1.iInstanceTime = entry->StartTimeL();
	iId1.iCollectionId = KFirstFileId;
	iTestLibrary->GetSession().FileIdL(iFileId1);
	
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numEntries(0);
	iSecondEntryView->StoreL(entries, numEntries);	
	test(numEntries == 1);
	iId2.iEntryLocalId = entries[0]->LocalUidL();
	iId2.iInstanceTime = entries[0]->StartTimeL();
	iId2.iCollectionId = KSecondFileId;
	iSecondSession->FileIdL(iFileId2);
	
	entries.ResetAndDestroy();

	//Create a non-repeating event
	entry = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
									TTime(TDateTime(2006, EOctober, 16, 9, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	iSecondEntryView->StoreL(entries, numEntries);	
	test(numEntries == 1);
	CleanupStack::PopAndDestroy(&entries);
	}

void CInstanceIteratorTestManager::TestFindInstanceL(CalCommon::TCalViewFilter aFilter, const TTime& aStart, const TTime& aEnd, TInt aExpectedCount)
	{
	TCalTime startRange;
	startRange.SetTimeLocalL(aStart); //1:00am 2007-10-16
	TCalTime endRange;
	endRange.SetTimeLocalL(aEnd); //1:00am 2007-10-23
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(aFilter, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EDescendingOrder);

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FindInstancesL(instances, *findInstanceSettings);
	
	if(aExpectedCount > 0)
		{
		test(aExpectedCount == instances.Count());
		}
	
	ValidateSortingL(instances, sortCriteria);
	
	CleanupStack::PopAndDestroy(&instances);	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestRepeatingEventsL(TInt aExpectedInstanceCount)
	{
	TestFindInstanceL(CalCommon::EIncludeEvents, 
			TTime(TDateTime(2007, EOctober, 16, 1, 0, 0, 0)), 
			TTime(TDateTime(2007, EOctober, 23, 1, 0, 0, 0)), 
			aExpectedInstanceCount);			
	}

void CInstanceIteratorTestManager::TestRepeatingTodosL()
	{
	TestFindInstanceL(CalCommon::EIncludeIncompletedTodos, 
			TTime(TDateTime(2007, EOctober, 16, 1, 0, 0, 0)), 
			TTime(TDateTime(2007, EOctober, 23, 1, 0, 0, 0)), 
			KInstanceCount);
	}

void CInstanceIteratorTestManager::TestSingleEventL(TInt aExpectedInstanceCount)
	{
	TestFindInstanceL(CalCommon::EIncludeEvents, 
			TTime(TDateTime(2006, EOctober, 16, 1, 0, 0, 0)), 
			TTime(TDateTime(2006, EOctober, 17, 1, 0, 0, 0)), 
			aExpectedInstanceCount);		
	}

void CInstanceIteratorTestManager::TestSingleTodoL()
	{
	TestFindInstanceL(CalCommon::EIncludeIncompletedTodos, 
			TTime(TDateTime(2006, EOctober, 28, 1, 0, 0, 0)), 
			TTime(TDateTime(2006, EOctober, 29, 1, 0, 0, 0)), 
			1);
	}

void CInstanceIteratorTestManager::TestRepeatWithChildL()
	{
	TestFindInstanceL(CalCommon::EIncludeEvents, 
			TTime(TDateTime(2005, EOctober, 16, 1, 0, 0, 0)), 
			TTime(TDateTime(2005, EOctober, 23, 1, 0, 0, 0)), 
			KInstanceCount);
	}

void CInstanceIteratorTestManager::TestAllTypesL()
	{
	TestFindInstanceL(CalCommon::EIncludeAll, 
			TTime(TDateTime(2004, EOctober, 16, 1, 0, 0, 0)), 
			TTime(TDateTime(2009, EOctober, 23, 1, 0, 0, 0)), 
			-1);
	}

void CInstanceIteratorTestManager::TestSortByStartTimeL()
	{
	TestSimpleSortL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
	//we don't need to test descending order for start time since the sort order has been used in filtering test
	}

void CInstanceIteratorTestManager::TestSortByEndTimeL()
	{
	TestSimpleSortL(CalCommon::ESortAttrEndTime, CalCommon::EAscendingOrder);
	TestSimpleSortL(CalCommon::ESortAttrEndTime, CalCommon::EDescendingOrder);
	}

void CInstanceIteratorTestManager::TestSortByLastModifiedTimeL()
	{
	TestSimpleSortL(CalCommon::ESortAttrLastModified, CalCommon::EAscendingOrder);
	TestSimpleSortL(CalCommon::ESortAttrLastModified, CalCommon::EDescendingOrder);
	}

void CInstanceIteratorTestManager::TestSortByPriorityL()
	{
	//All entries have been assigned a random priority when they were added.
	TestSimpleSortL(CalCommon::ESortAttrPriority, CalCommon::EAscendingOrder);
	TestSimpleSortL(CalCommon::ESortAttrPriority, CalCommon::EDescendingOrder);
	}

void CInstanceIteratorTestManager::TestSortByTypesL()
	{
	TCalTime startRange;
	startRange.SetTimeLocalL(TTime(TDateTime(2003, EJanuary, 1, 1, 0, 0, 0))); //1:00am 2004-01-01
	TCalTime endRange;
	endRange.SetTimeLocalL(TTime(TDateTime(2004, EJanuary, 1, 1, 0, 0, 0))); //1:00am 2005-01-01
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	
	RArray<CCalEntry::TType> typeOrder(5);
	CleanupClosePushL(typeOrder);
	
	typeOrder.AppendL(CCalEntry::EReminder);	
	typeOrder.AppendL(CCalEntry::ETodo);	
	typeOrder.AppendL(CCalEntry::EEvent);
	typeOrder.AppendL(CCalEntry::EAppt);
	typeOrder.AppendL(CCalEntry::EAnniv);
	
	sortCriteria.SetEntryTypeOrderL(typeOrder);
	CleanupStack::PopAndDestroy(&typeOrder);
	
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrType, CalCommon::EAscendingOrder);

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FindInstancesL(instances, *findInstanceSettings);
	
	ValidateSortingL(instances, sortCriteria);
	CleanupStack::PopAndDestroy(&instances);	
	CleanupStack::PopAndDestroy(findInstanceSettings);	
	}

void CInstanceIteratorTestManager::TestSortByPriortiyThenStartTimeL()
	{
	TestMultipleSortL(CalCommon::ESortAttrPriority, CalCommon::EAscendingOrder,
					  CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);	
	}

void CInstanceIteratorTestManager::TestSortByLastModifiedDateThenPriorityL()
	{
	TestMultipleSortL(CalCommon::ESortAttrLastModified, CalCommon::EAscendingOrder,
					  CalCommon::ESortAttrPriority, CalCommon::EAscendingOrder);	
	}

void CInstanceIteratorTestManager::TestSortByTypesThenPriorityL()
	{
	TestMultipleSortL(CalCommon::ESortAttrType, CalCommon::EAscendingOrder,
					  CalCommon::ESortAttrPriority, CalCommon::EAscendingOrder);	
	}

void CInstanceIteratorTestManager::TestDefaultSortOrderL()
	{
	//This is to test the default sort order of iterator generated by calling
	//findinstance without setting sort criteria.
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	CCalInstanceIterator* instanceIterator = NULL;
	if(iTestType == EMultipleCalendarFilesTest)
		{
		instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings);
		}
	else
		{
		instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
		}	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	
	CleanupStack::PushL(instanceIterator);
	TTime preTime(TCalTime::MinTime());
	
	// Call HasMore between each step to check the result
	while(instanceIterator->HasMore())
		{
		CCalInstance* instance = instanceIterator->NextL();
		TTime instanceTime = instance->Time().TimeLocalL();
		PrintInstanceDetailsL(*instance);
		test(instanceTime >= preTime);
		preTime = instanceTime;
		delete instance;
		}
	
	CleanupStack::PopAndDestroy(instanceIterator);
	}

void CInstanceIteratorTestManager::TestDefaultSortOrderWithInstanceIdL()
	{
	//This is to test the default sort order of iterator generated by calling
	//findinstance without setting sort criteria.
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	CheckInstancesL(*findInstanceSettings, iId2, 9);
	
	CheckInstancesL(*findInstanceSettings, iId1, 8);
	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestSortOnFloatingEntriesL()
	{
	_LIT8(KHongKong, "Asia/Hong_Kong"); //GMT +8:00
	_LIT8(KLondon, "Europe/London"); //GMT 0:00

	iTestLibrary->CleanDatabaseL();
	iTestLibrary->SetTimeZoneL(KLondon); //make sure it's london time
	
	CCalEntry* localTimeEvent = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2008, EOctober, 16, 10, 30, 0, 0)));
	iTestLibrary->StoreEntryL(*localTimeEvent);
	TCalLocalUid localTimeEvtUid(localTimeEvent->LocalUidL());
	CleanupStack::PopAndDestroy(localTimeEvent);
	
	CCalEntry* floatingTimeEvent = CreateNonRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2008, EOctober, 16, 12, 30, 0, 0)), 
											  ETrue);
	iTestLibrary->StoreEntryL(*floatingTimeEvent);
	TCalLocalUid floatingTimeEvtUid(floatingTimeEvent->LocalUidL());
	CleanupStack::PopAndDestroy(floatingTimeEvent);
	
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
	
	// Call HasMore between each step to check the result
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	CleanupStack::PushL(instanceIterator);
	
	test(instanceIterator->Count() == 2);
	CCalInstance* firstInstance = instanceIterator->NextL();
	test(firstInstance->Entry().LocalUidL() == localTimeEvtUid);
	delete firstInstance;
	CleanupStack::PopAndDestroy(instanceIterator);
	
	iTestLibrary->SetTimeZoneL(KHongKong); //change to hong kong time
	instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	CleanupStack::PushL(instanceIterator);
	
	test(instanceIterator->Count() == 2);
	firstInstance = instanceIterator->NextL();
	test(firstInstance->Entry().LocalUidL() == floatingTimeEvtUid);
	delete firstInstance;
	CleanupStack::PopAndDestroy(instanceIterator);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestCacheSizeL()
	{
	iTestLibrary->CleanDatabaseL();

	const TInt KInstanceCount(800);
	CCalEntry* repeatingEvents = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2008, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*repeatingEvents);
	CleanupStack::PopAndDestroy(repeatingEvents);
	
	CCalEntry* repeatingTodos = CreateRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2008, EOctober, 16, 18, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*repeatingTodos);
	CleanupStack::PopAndDestroy(repeatingTodos);
			
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
	
	// Call HasMore between each step to check the result
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	CleanupStack::PushL(instanceIterator);

	TInt count(0);
	while (instanceIterator->HasMore())
		{
		CCalInstance* instance = instanceIterator->NextL();
		delete instance;
		++count;
		}
	
	while (0 < count--)
		{
		CCalInstance* instance = instanceIterator->PreviousL();
		delete instance;
		}
	
	CleanupStack::PopAndDestroy(instanceIterator);
	}


void CInstanceIteratorTestManager::TestCombinedSearchAndSortL()
	{
	_LIT(KFormatSummary, "Summary%d");
	_LIT(KSummary, "Summary");
	_LIT(KDescription, "Description");
	
	iTestLibrary->CleanDatabaseL();

	TBuf<50> summary;
	
	const TInt KInstanceCount(5);
	//Create repeating events with summary being filled. 
	CCalEntry* repeatingEvents = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2008, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	summary.Format(KFormatSummary, 1);
	repeatingEvents->SetSummaryL(summary);
	iTestLibrary->StoreEntryL(*repeatingEvents);
	CleanupStack::PopAndDestroy(repeatingEvents);
	
	//Create repeating todo with summary being filled
	CCalEntry* repeatingTodo = CreateRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2008, EOctober, 16, 13, 30, 0, 0)),
											  KInstanceCount);
	summary.Format(KFormatSummary, 2);
	repeatingTodo->SetSummaryL(summary);
	iTestLibrary->StoreEntryL(*repeatingTodo);
	CleanupStack::PopAndDestroy(repeatingTodo);

	//Create time range to get all instances
	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	//finding setting including all types of instances 
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);

	//search with correct text and expect returning all instances
	CCalInstanceView::TCalSearchParams searchParams(KSummary, CalCommon::EFoldedTextSearch);
	findInstanceSettings->SetTextSearchL(searchParams);
	
	//set sort criteria so order the returned instances by start time then by entry types
	//(both sort directions are descending)
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EDescendingOrder);
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrType, CalCommon::EDescendingOrder);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FindInstancesL(instances, *findInstanceSettings);
	test(instances.Count() == KInstanceCount*2);
	ValidateSortingL(instances, sortCriteria);

	//create search parameters with incorrect text so there will be no instance found
	CCalInstanceView::TCalSearchParams notFoundSearchParams(KDescription, CalCommon::EFoldedTextSearch);
	findInstanceSettings->SetTextSearchL(notFoundSearchParams);
	
	instances.ResetAndDestroy();
	FindInstancesL(instances, *findInstanceSettings);
	test(instances.Count() == 0);
	ValidateSortingL(instances, sortCriteria);
	
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestFileringPriorityL()
	{
	iTestLibrary->CleanDatabaseL();
	const TInt KInstanceCount(5);
	TDateTime dateTime(2008, EJanuary, 01, 11, 30, 0, 0);

	for(TUint index = 0; index <= 255; ++index)
		{
		CCalEntry* rptEvents = CreateRepeatingEntryLC(CCalEntry::EEvent,dateTime,KInstanceCount,TCalRRule::EYearly);
		rptEvents->SetPriorityL(index);
		iTestLibrary->StoreEntryL(*rptEvents);
		CleanupStack::PopAndDestroy(rptEvents);
		}

	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	TUint randomPriority(Math::Random() & 0xFF);
	const TInt KExpectedNumofInstances =  KInstanceCount * (randomPriority + 1);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	findInstanceSettings->SetPriorityRange(CalCommon::TCalPriorityRange(0, randomPriority));
	CleanupStack::PushL(findInstanceSettings);
	
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	test(instanceIterator->Count() == KExpectedNumofInstances);
	delete instanceIterator;
	
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
	instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	test(instanceIterator->Count() == KExpectedNumofInstances);
	delete instanceIterator;

	//boudary test case: highest priority is larger than lowest
	findInstanceSettings->SetPriorityRange(CalCommon::TCalPriorityRange(255, 128));
	instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	test(instanceIterator->Count() == 0);
	delete instanceIterator;
	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	}

void CInstanceIteratorTestManager::TestMultipleSortL(CalCommon::TCalSortAttribute aFirstAttr, CalCommon::TCalSortDirection aFirstDir,
		CalCommon::TCalSortAttribute aSecondAttr, CalCommon::TCalSortDirection aSecondDir)
	{
	TCalTime startRange;
	startRange.SetTimeLocalL(TTime(TDateTime(2001, EOctober, 16, 1, 0, 0, 0))); //1:00am 2001-10-16
	TCalTime endRange;
	endRange.SetTimeLocalL(TTime(TDateTime(2009, EOctober, 23, 1, 0, 0, 0))); //1:00am 2009-10-23
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(aFirstAttr, aFirstDir);
	sortCriteria.AppendCriterionL(aSecondAttr, aSecondDir);

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	FindInstancesL(instances, *findInstanceSettings);
	ValidateSortingL(instances, sortCriteria);
	
	CleanupStack::PopAndDestroy(&instances);	
	CleanupStack::PopAndDestroy(findInstanceSettings);	
	}


void CInstanceIteratorTestManager::TestSimpleSortL(CalCommon::TCalSortAttribute aAttr, CalCommon::TCalSortDirection aDir)
	{
	TCalTime startRange;
	startRange.SetTimeLocalL(TTime(TDateTime(2004, EOctober, 16, 1, 0, 0, 0))); //1:00am 2004-10-16
	TCalTime endRange;
	endRange.SetTimeLocalL(TTime(TDateTime(2009, EOctober, 23, 1, 0, 0, 0))); //1:00am 2009-10-23
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(aAttr, aDir);

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FindInstancesL(instances, *findInstanceSettings);
	ValidateSortingL(instances, sortCriteria);
	CleanupStack::PopAndDestroy(&instances);
	
	
	CleanupStack::PopAndDestroy(findInstanceSettings);	
	}

CCalEntry* CInstanceIteratorTestManager::CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount, TCalRRule::TType aRepeatType)
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
	TCalRRule rRule(aRepeatType);
	rRule.SetDtStart(entryStart);
	rRule.SetCount(aRepeatCount);
	rRule.SetInterval(1);
	entry->SetRRuleL(rRule);
	
	//assign a random priorty to the entry 
	entry->SetPriorityL(Math::Random() & 0xFF);
	return entry;
	}


CCalEntry* CInstanceIteratorTestManager::CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount)
	{
	return CreateRepeatingEntryLC(aEntryType, aLocalStartTime, aRepeatCount, TCalRRule::EDaily);
	}

CCalEntry* CInstanceIteratorTestManager::CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime)
	{
	return CreateNonRepeatingEntryLC(aEntryType, aLocalStartTime, EFalse);
	}

CCalEntry* CInstanceIteratorTestManager::CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TBool aFloatingTime)
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLibrary->CreateCalEntryL(aEntryType, guid);
	CleanupStack::PushL(entry);
	
	if (aLocalStartTime != Time::NullTTime())
		{
		TCalTime entryStart;
		if(aFloatingTime)
			{
			entryStart.SetTimeLocalFloatingL(aLocalStartTime);
			}
		else
			{
			entryStart.SetTimeLocalL(aLocalStartTime);
			}
			
		TCalTime entryEnd;
		if(aFloatingTime)
			{
			entryEnd.SetTimeLocalFloatingL(aLocalStartTime + TTimeIntervalHours(1));
			}
		else
			{
			entryEnd.SetTimeLocalL(aLocalStartTime + TTimeIntervalHours(1));
			}
		
		entry->SetStartAndEndTimeL(entryStart, entryEnd);
		}

	//assign a random priorty to the entry 
	entry->SetPriorityL(Math::Random() & 0xFF);
	return entry;
	}

void CInstanceIteratorTestManager::FindInstancesL(RPointerArray<CCalInstance>& aInstances,
												 const CCalFindInstanceSettings& aSettings)
	{	
	test.Printf(_L("Fetching instances"));
	
	// Call HasMore between each step to check the result
	CCalInstanceIterator* instanceIterator = NULL;
	
	if(iTestType == EMultipleCalendarFilesTest)
		{
		instanceIterator = iInstanceView->FindInstanceL(aSettings);
		}
	else
		{
		instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(aSettings);
		}
	CleanupStack::PushL(instanceIterator);
	
	while (instanceIterator->HasMore())
		{
		CCalInstance* instance = instanceIterator->NextL();
		CleanupStack::PushL(instance);
		PrintInstanceDetailsL(*instance);
		aInstances.AppendL(instance);
		CleanupStack::Pop(instance);
		}
	CleanupStack::PopAndDestroy(instanceIterator);
	}

void CInstanceIteratorTestManager::ValidateSortingL(RPointerArray<CCalInstance>& aFoundInstances, const CCalSortCriteria& aSortCriteria)
	{
	test.Printf(_L("Validating sorting order"));
	const TInt KCount(aFoundInstances.Count());
	const TInt KNumOfSortCriteria(aSortCriteria.Count());
	for(TInt instanceIndex = 0; instanceIndex < KCount - 1; instanceIndex++)
		{
		for(TInt criteriaIndex = 0; criteriaIndex < KNumOfSortCriteria; criteriaIndex++)
			{
			const CCalSortCriterion& sortCriterion = aSortCriteria.AtL(criteriaIndex);
			TInt result;
			if(sortCriterion.SortAttribute() != CalCommon::ESortAttrType)
				{
				result = CompareL(*aFoundInstances[instanceIndex], *aFoundInstances[instanceIndex + 1], sortCriterion);
				}
			else
				{
				if(aSortCriteria.EntryTypeOrder().Count() != 0)
					{
					result = CompareEntryTypeL(*aFoundInstances[instanceIndex], *aFoundInstances[instanceIndex + 1], sortCriterion.SortDirection(), aSortCriteria.EntryTypeOrder());
					}
				else
					{
					result = CompareEntryTypeL(*aFoundInstances[instanceIndex], *aFoundInstances[instanceIndex + 1], sortCriterion.SortDirection(), iDefaultTypeOrder);
					}
				}
			
			if(iTestType == EMultipleCalendarFilesTest)
				{
				result = CompareShortFileIdL(*aFoundInstances[instanceIndex], *aFoundInstances[instanceIndex + 1]);
				}
			test(result >= 0);
			if(result > 0)
				{
				//order has been decided and validated
				break;
				}
			}
		}
	}

TInt CInstanceIteratorTestManager::CompareL(const CCalInstance& aFirst, const CCalInstance& aSecond, const CCalSortCriterion& aSortCriterion)
	{
	TInt ret(0);
	TInt direction = (aSortCriterion.SortDirection() == CalCommon::EAscendingOrder) ? 1 : -1;
	switch(aSortCriterion.SortAttribute())
		{
		case CalCommon::ESortAttrStartTime:
			ret = CompareTTime(aFirst.StartTimeL().TimeUtcL(), aSecond.StartTimeL().TimeUtcL());  
			break;
		case CalCommon::ESortAttrEndTime:
			ret = CompareTTime(aFirst.EndTimeL().TimeUtcL(), aSecond.EndTimeL().TimeUtcL());  
			break;
		case CalCommon::ESortAttrLastModified:
			ret = CompareTTime(aFirst.Entry().LastModifiedDateL().TimeUtcL(), aSecond.Entry().LastModifiedDateL().TimeUtcL());  
			break;
		case CalCommon::ESortAttrPriority:
			ret = aSecond.Entry().PriorityL() - aFirst.Entry().PriorityL(); 
			break;
		case CalCommon::ESortAttrType:
		default:
			//should not be called here
			test(0);
			break;
		}
	
	return ret * direction;
	}

TInt CInstanceIteratorTestManager::CompareEntryTypeL(const CCalInstance& aFirst, const CCalInstance& aSecond, CalCommon::TCalSortDirection aSortDirection, const RArray<CCalEntry::TType>& aTypeOrders)
	{
	TInt firstIndex = aTypeOrders.Find(aFirst.Entry().EntryTypeL());
	TInt secondIndex = aTypeOrders.Find(aSecond.Entry().EntryTypeL());
	
	TInt direction = (aSortDirection == CalCommon::EAscendingOrder) ? 1 : -1;
	
	return direction * (secondIndex - firstIndex);
	}

TInt CInstanceIteratorTestManager::CompareShortFileIdL(const CCalInstance& aFirst, const CCalInstance& aSecond)
	{
	TUint8 firstId = aFirst.InstanceIdL().iCollectionId;
	TUint8 secondId = aSecond.InstanceIdL().iCollectionId;
	
	if(secondId > firstId)
		{
		return (secondId - firstId);
		}
	else if (firstId > secondId)
		{
		return (firstId - secondId);
		}
	else
		{
		return -1;
		}
	}

TInt CInstanceIteratorTestManager::CompareTTime(TTime aFirst, TTime aSecond)
	{
	if(aFirst > aSecond)
		{
		return -1;
		}
	else if(aFirst == aSecond)
		{
		return 0;
		}
	else
		{
		return 1;
		}
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
	instanceDetails.AppendFormat( _L("Priority %d "),  aInstance.Entry().PriorityL());
	instanceDetails.AppendFormat( _L("Short File Id %d "), aInstance.InstanceIdL().iCollectionId);
	
	switch(aInstance.Entry().EntryTypeL())
		{
		case CCalEntry::ETodo:
			instanceDetails.Append(_L("Todo "));
			break;
		case CCalEntry::EAppt:
			instanceDetails.Append(_L("Appt "));
			break;
		case CCalEntry::EEvent:
			instanceDetails.Append(_L("Event "));
			break;
		case CCalEntry::EReminder:
			instanceDetails.Append(_L("Rmdr "));
			break;
		case CCalEntry::EAnniv:
			instanceDetails.Append(_L("Anniv "));
			break;
		default:
			instanceDetails.Append(_L("Unkown "));
			break;
		}
	
	TBuf<50> timeBuf;
	aInstance.Time().TimeLocalL().FormatL(timeBuf, KFormatDateTime());
	
	instanceDetails.Append(timeBuf);
	
	test.Printf(instanceDetails);
	}

void CInstanceIteratorTestManager::CheckInstancesL(CCalFindInstanceSettings& aFindInstanceSetting, TCalInstanceId aId, TInt aExpectedCount)
	{	
	CCalInstanceIterator* instanceIterator = iInstanceView->FindInstanceL(aFindInstanceSetting, aId);
	CleanupStack::PushL(instanceIterator);
	TTime preTime(TCalTime::MinTime());
	TInt count(0);
	//Call HasMore between each step to check if more results are available
	while(instanceIterator->HasMore())
		{
		CCalInstance* instance = instanceIterator->NextL();
		TTime instanceTime = instance->Time().TimeLocalL();
		PrintInstanceDetailsL(*instance);
		test(instanceTime >= preTime);
		preTime = instanceTime;
		delete instance;
		++count;
		}
	test(count == aExpectedCount);
	CleanupStack::PopAndDestroy(instanceIterator);
	}

void CInstanceIteratorTestManager::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));

	TCalTime startRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endRange;
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	CCalSortCriteria& sortCriteria = findInstanceSettings->SortCriteria();
	sortCriteria.AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
	
	// Call HasMore between each step to check the result
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	CleanupStack::PushL(instanceIterator);

	TInt count(0);
	while (instanceIterator->HasMore())
		{
		CCalInstance* instance = instanceIterator->NextL();
		delete instance;
		++count;
		}
	
	CleanupStack::PopAndDestroy(instanceIterator);
	}

void CInstanceIteratorTestManager::TestSortedIteratorOOML()
	{
	iTestLibrary->CleanDatabaseL();

	const TInt KInstanceCount(10);
	CCalEntry* repeatingEvents = CreateRepeatingEntryLC(CCalEntry::EEvent,
											  TTime(TDateTime(2008, EOctober, 16, 11, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*repeatingEvents);
	CleanupStack::PopAndDestroy(repeatingEvents);
	
	CCalEntry* repeatingTodos = CreateRepeatingEntryLC(CCalEntry::ETodo,
											  TTime(TDateTime(2008, EOctober, 16, 18, 30, 0, 0)),
											  KInstanceCount);
	iTestLibrary->StoreEntryL(*repeatingTodos);
	CleanupStack::PopAndDestroy(repeatingTodos);
	
	CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EServer, iTestLibrary->GetSession());
	CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EClient, iTestLibrary->GetSession());
	
	iTestLibrary->CleanDatabaseL();
	}

void CInstanceIteratorTestManager::RunTestsL()
	{
	switch( iTestType )
		{
		case ESingleCalendarFileTest:
			{
	test.Next(_L("Initializing database"));
	AddTestingDataL();	
	
	test.Next(_L("Test filtering on TestRepeatingEventsL"));
			TestRepeatingEventsL(KInstanceCount);
	
	test.Next(_L("Test filtering on Repeating Todos"));
	TestRepeatingTodosL();

	test.Next(_L("Test filtering on single events"));
			TestSingleEventL(1);
	
	test.Next(_L("Test filtering on single todo"));
	TestSingleTodoL();
	
	test.Next(_L("Test filtering on repeating events with child"));
	TestRepeatWithChildL();

	test.Next(_L("Test filtering on all type instances"));
	TestAllTypesL();
	
	//Simple sorting test
	test.Next(_L("Test sorting by start time"));
	TestSortByStartTimeL();
	
	test.Next(_L("Test sorting by end time"));
	TestSortByEndTimeL();
	
	test.Next(_L("Test sorting by last modified time"));
	TestSortByLastModifiedTimeL();
	
	test.Next(_L("Test sorting by priority"));
	TestSortByPriorityL();
	
	test.Next(_L("Test sorting by entry types"));
	TestSortByTypesL();
	
	test.Next(_L("Test sorting by priority then start time"));
	TestSortByPriortiyThenStartTimeL();
	
	test.Next(_L("Test sorting by last modified time then priority"));
	TestSortByLastModifiedDateThenPriorityL();

	test.Next(_L("Test sorting by entry types then priority"));
	TestSortByTypesThenPriorityL();

	test.Next(_L("Test against default sort order"));
	TestDefaultSortOrderL();
	
	test.Next(_L("Test sorting on floating entry"));
	TestSortOnFloatingEntriesL();	
	
	test.Next(_L("Test cache strategy used by sorted iterator"));
	TestCacheSizeL();

	test.Next(_L("Test filtering on priority"));	
	TestFileringPriorityL();
	
	test.Next(_L("Test combined searching on text and sorting"));	
	TestCombinedSearchAndSortL();

	test.Next(_L("OOM Test for calendar server instance iterator"));
	TestSortedIteratorOOML();
	
			break;
			}
		case EMultipleCalendarFilesTest:
			{
	test.Printf(_L("Starting with multiple calendar tests"));
	test.Next(_L("Initialising database for multiple files test"));
	AddTestDataForMultipleFilesL();
	
			test.Next(_L("Test filtering on repeating events for multiple calendar files"));
			TestRepeatingEventsL(KMultipleCalendarInstanceCount);
			
			test.Next(_L("Test filtering on single events for multiple calendar files"));
			TestSingleEventL(2);
			
			test.Next(_L("Test default sort order for multiple calendar file"));
	TestDefaultSortOrderL();
	

			test.Next(_L("Test default sort order with instance id for multiple calendar file"));
	TestDefaultSortOrderWithInstanceIdL();
	
			test.Next(_L("Test sorting by start time for multiple calendar file"));
	TestSortByStartTimeL();
	
			test.Next(_L("Test sorting by end time for multiple calendar file"));
	TestSortByEndTimeL();
			break;
			}
		default:
			break;
		}	
	}

static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	
	testManager->RunTestsL();
	
	CleanupStack::PopAndDestroy(testManager);
	}
#endif

/**
@SYMTestCaseID     PIM-TCAL-SORTEDINSTANCEITERATOR-0001
*/
TInt E32Main()
    {
	__UHEAP_MARK;
	
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-SORTEDINSTANCEITERATOR-0001 tcal_sortedinstanceiterator"));

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (!scheduler)
		{
		test(EFalse);
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	
	TInt ret(KErrNone);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TRAP(ret, DoTestL());
#endif
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return ret;
    }
