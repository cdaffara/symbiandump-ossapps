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

#include <calentry.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceiterator.h>

_LIT(KTestName, "tcal_instanceiterator_uid");
_LIT(KTestSecondFileName,"tcal_instanceiterator_uid_SecondFile");
_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S.%C");

const TTime KStartTimeOOMInstance(TTime(TDateTime(2006, ENovember, 12, 11, 30, 0, 0)));
const TInt KNumOfOOMInstances(10);
const TInt KFirstFileId = 1;

RTest test(KTestName);

class CTestData : public CBase
	{
public:
	struct TExpectedInstance
		{
		TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid);
		TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, TUint8 aFileId);
		
		TCalTime iTime;
		TCalLocalUid iLocalUid;
		TUint8 iFileId;
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
	HBufC8* iUid;
	RArray<TExpectedInstance> iExpectedInstances;
	TLinearOrder<TExpectedInstance> iLinearOrder;
	TTestType iTestType;
	};
	
CTestData::TExpectedInstance::TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid)
	:iLocalUid(aLocalUid)
	{
	iTime.SetTimeLocalL(aTimeLocal);
	iFileId = KFirstFileId;
	}

CTestData::TExpectedInstance::TExpectedInstance(TTime aTimeLocal, TCalLocalUid aLocalUid, TUint8 aFileId)
	:iLocalUid(aLocalUid), iFileId(aFileId)
	{
	iTime.SetTimeLocalL(aTimeLocal);
	}	
TInt AnOrder(const CTestData::TExpectedInstance& aLeft, const CTestData::TExpectedInstance& aRight)
	{
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
	else if (aLeft.iLocalUid < aRight.iLocalUid)
		{
		result =-1;
		}
	else if (aLeft.iLocalUid > aRight.iLocalUid)
		{
		result =1; 
		}
	else
		{
		result = aLeft.iFileId - aRight.iFileId;
		}
		
	return result;
	}

	
CTestData::CTestData()
	:iLinearOrder(AnOrder)
	{
	}
	
CTestData::~CTestData()
	{
	delete iUid;
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
		// Simple test cases
		ERepeatingEvent,
		ERepeatingTodo,
		EAddRepeatingTodoWithNonRepeatingChildrenTodo,
		
		// Entries with children RDates and ExDates
		ERepeatWithChild,
		ERepeatWithRDate,
		ERepeatWithExDate,
		ERepeatWithRepeatChild,
		
		//Multiple Files
		EAddRepeatingEventToMultipleFiles,
		EAddRepeatingTodoWithNonRepeatingChildToMultipleFiles,

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
	void AddRepeatingTodoL(CTestData& aTestData);
	void AddRepeatingTodoWithNonRepeatingChildrenTodoL(CTestData& aTestData);
	
	void AddRepeatWithChildL(CTestData& aTestData);
	void AddRepeatWithRDateL(CTestData& aTestData);
	void AddRepeatWithExDateL(CTestData& aTestData);
	void AddRepeatWithRepeatChildL(CTestData& aTestData);
	void AddRepeatingEventToMultipleFilesL(CTestData& aTestData);
	void AddRepeatingTodoWithNonRepeatingChildToMultipleFilesL(CTestData& aTestData);
	TUint8 StorEntryInSecondFileL(const CCalEntry& aEntry, TBool aCreateFile);
	// Deleting entries
	void TestDeleteEntryBeforeIteratingL();
	void TestExceptedEntryBeforeIteratingL();
	void TestDeleteEntryWhileIteratingL();
	
	// Utility methods
	void PrintInstanceDetailsL(CCalInstance& aInstance);
	
	// Creating entries
	CCalEntry* CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount);
	
	void TestIteratingInstanceL(const TInt aNumInstance, const TDesC& aSearchString);
	
	// OOM
	void OomTestL();

private: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);
	
private:
	CCalTestLibrary* iTestLibrary;
	HBufC8* iOOMUid;
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
	}	


CInstanceIteratorTestManager::~CInstanceIteratorTestManager()	
	{
	delete iOOMUid;
	delete iTestLibrary;
	}

void CInstanceIteratorTestManager::PrintInstanceDetailsL(CCalInstance& aInstance)
	{
	TBuf<50> instanceDetails;
	
	instanceDetails.AppendFormat( _L("Entry %d "),  aInstance.Entry().LocalUidL());
	
	instanceDetails.Append( (aInstance.Entry().EntryTypeL() == CCalEntry::ETodo) ? _L("Todo ") : _L("Non-Todo ") );
	
	TBuf<50> timeBuf;
	aInstance.Time().TimeLocalL().FormatL(timeBuf, KFormatDateTime());
	
	instanceDetails.Append(timeBuf);
	
	test.Printf(instanceDetails);
	}

void CInstanceIteratorTestManager::OomTestL()
	{
	iTestLibrary->CleanDatabaseL();
	
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent, KStartTimeOOMInstance, KNumOfOOMInstances);
	iTestLibrary->StoreEntryL(*entry);
	iOOMUid = entry->UidL().AllocL();
	CleanupStack::PopAndDestroy(entry);
	
	OomTestL(EClient, 0);
	// TODO
	CCalSession& calSession = iTestLibrary->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	delete iOOMUid;
	iOOMUid = NULL;
	}
	
void CInstanceIteratorTestManager::OomTestL(TType aType, TInt aFailAt)
	{
	test.Printf(_L("OOM Test %d %d"), aType, aFailAt);
		
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(KStartTimeOOMInstance);
	
	CCalInstanceIterator* instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*iOOMUid, startingInstanceTime);
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

TUint8 CInstanceIteratorTestManager::StorEntryInSecondFileL(const CCalEntry& aEntry, TBool aCreateFile)
	{
	CCalSession* secondSession = CCalSession::NewL(iTestLibrary->GetSession());
	CleanupStack::PushL(secondSession);
	if(aCreateFile)
		{
		secondSession->CreateCalFileL(KTestSecondFileName);
		}
	secondSession->OpenL(KTestSecondFileName);
	CCalEntryView* secondView =CCalEntryView::NewL(*secondSession);
	CleanupStack::PushL(secondView);
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	entries.AppendL(&aEntry);
	TInt entriesStored(0);
	secondView->StoreL(entries, entriesStored);
	TCalCollectionId fileId = secondSession->CollectionIdL();
	CleanupStack::PopAndDestroy(3, secondSession);
	return fileId;
	}

void CInstanceIteratorTestManager::AddRepeatingEventToMultipleFilesL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Non-Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	TUint8 secondFileId = StorEntryInSecondFileL(*entry, ETrue);
	TCalCollectionId firstFileId = iTestLibrary->GetSession().CollectionIdL();
	
	aTestData.iUid = entry->UidL().AllocL();
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid, secondFileId));
	
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 24, 10, 30, 0, 0)),
			5);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CInstanceIteratorTestManager::AddRepeatingTodoWithNonRepeatingChildToMultipleFilesL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Todo with Non-Repeating Children Todos"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* parentEntry = CreateRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 22, 14, 30, 0, 0)),
												5);
	
	TCalTime recId;
	recId.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 14, 30, 0, 0)));
	HBufC8* uid = parentEntry->UidL().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::ETodo, uid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(childEntry);
	
	iTestLibrary->SetEntryStartAndEndTimeL(childEntry,
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)),
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)));
	TCalTime calTime;
	calTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)));
	childEntry->SetCompletedL(ETrue, calTime);
	
	iTestLibrary->StoreEntryL(*parentEntry);
	aTestData.iUid = parentEntry->UidL().AllocL();
	TCalLocalUid parentLocalUid(parentEntry->LocalUidL());
	
	iTestLibrary->StoreEntryL(*childEntry);
	
	TCalLocalUid childLocalUid(childEntry->LocalUidL());
	
	TUint8 secondFileId = StorEntryInSecondFileL(*parentEntry, ETrue);
	StorEntryInSecondFileL(*childEntry, EFalse);
	TCalCollectionId firstFileId = iTestLibrary->GetSession().CollectionIdL();

	CleanupStack::PopAndDestroy(childEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), parentLocalUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), parentLocalUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), parentLocalUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), parentLocalUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), childLocalUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), childLocalUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), parentLocalUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), parentLocalUid, secondFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), parentLocalUid, firstFileId));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), parentLocalUid, secondFileId));
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
			TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)),
			3);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CInstanceIteratorTestManager::AddRepeatingEventL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Non-Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	aTestData.iUid = entry->UidL().AllocL();
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 11, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 11, 30, 0, 0)), localUid));
	
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 24, 10, 30, 0, 0)),
			5);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CInstanceIteratorTestManager::AddRepeatingTodoL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Todo"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 22, 14, 30, 0, 0)),
												5);
	iTestLibrary->StoreEntryL(*entry);
	aTestData.iUid = entry->UidL().AllocL();
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 15, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), localUid));
	
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
			TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)),
			3);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CInstanceIteratorTestManager::AddRepeatingTodoWithNonRepeatingChildrenTodoL(CTestData& aTestData)
	{
	test.Printf(_L("Add Repeating Todo with Non-Repeating Children Todos"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* parentEntry = CreateRepeatingEntryLC(CCalEntry::ETodo,
												TTime(TDateTime(2006, ENovember, 22, 14, 30, 0, 0)),
												5);
	
	TCalTime recId;
	recId.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 14, 30, 0, 0)));
	HBufC8* uid = parentEntry->UidL().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::ETodo, uid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(childEntry);
	
	iTestLibrary->SetEntryStartAndEndTimeL(childEntry,
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)),
											TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)));
	TCalTime calTime;
	calTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)));
	childEntry->SetCompletedL(ETrue, calTime);
	
	iTestLibrary->StoreEntryL(*parentEntry);
	aTestData.iUid = parentEntry->UidL().AllocL();
	TCalLocalUid parentLocalUid(parentEntry->LocalUidL());
	
	iTestLibrary->StoreEntryL(*childEntry);
	TCalLocalUid childLocalUid(childEntry->LocalUidL());
	CleanupStack::PopAndDestroy(childEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 15, 30, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 15, 30, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 11, 30, 0, 0)), childLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 15, 30, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 15, 30, 0, 0)), parentLocalUid));
	
	// Create another entry to have some entries that should actually not be found as they are
	// not part of the entry we are interested in
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::ETodo,
			TTime(TDateTime(2006, ENovember, 23, 11, 30, 0, 0)),
			3);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
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
	
	aTestData.iUid = parentEntry->UidL().AllocL();
	
	HBufC8* uid = parentEntry->UidL().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, uid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(uid);
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
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 13, 36, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 13, 36, 0, 0)), parentLocalUid));
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)), childLocalUid));
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 13, 36, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 13, 36, 0, 0)), parentLocalUid));
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
	aTestData.iUid = entry->UidL().AllocL();
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 9, 45, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 9, 45, 0, 0)), localUid));
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 17, 30, 0, 0)), localUid)); // the rDate
	
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 9, 45, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 9, 45, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 9, 45, 0, 0)), localUid));
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
	aTestData.iUid = entry->UidL().AllocL();
	TCalLocalUid localUid(entry->LocalUidL());
	CleanupStack::PopAndDestroy(entry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 18, 15, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 23, 18, 15, 0, 0)), localUid));
	// aTestData.InsertExpectedInstanceL(TTime(TDateTime(2006, ENovember, 24, 18, 15, 0, 0))); // the exDate
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 18, 15, 0, 0)), localUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 26, 18, 15, 0, 0)), localUid));
	}

void CInstanceIteratorTestManager::AddRepeatWithRepeatChildL(CTestData& aTestData)
	{
	test.Printf(_L("Add repeating event with a repeating child"));
	
	// Daily repeating entry from 23 November to 27 November
	CCalEntry* parentEntry = CreateRepeatingEntryLC(CCalEntry::EEvent,
												TTime(TDateTime(2006, ENovember, 22, 13, 36, 0, 0)),
												5);
	
	TCalTime recId;
	recId.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 23, 13, 36, 0, 0)));
	
	aTestData.iUid = parentEntry->UidL().AllocL();
	
	// Add child entry
	HBufC8* uid = parentEntry->UidL().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, uid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(childEntry);
	TCalTime startTime;
	startTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)));
	TCalTime endTime;
	endTime.SetTimeLocalL(TTime(TDateTime(2006, ENovember, 24, 18, 36, 0, 0)));
	childEntry->SetStartAndEndTimeL(startTime, endTime);
	// Add the RRule
	TCalRRule rRule(TCalRRule::EDaily);
	rRule.SetDtStart(startTime);
	rRule.SetCount(2);
	rRule.SetInterval(1);
	childEntry->SetRRuleL(rRule);
	iTestLibrary->StoreEntryL(*parentEntry);
	TCalLocalUid parentLocalUid(parentEntry->LocalUidL());
	iTestLibrary->StoreEntryL(*childEntry);
	TCalLocalUid childLocalUid(childEntry->LocalUidL());
	CleanupStack::PopAndDestroy(childEntry);
	
	CleanupStack::PopAndDestroy(parentEntry);
	
	// Set up the expected instances
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 22, 13, 36, 0, 0)), parentLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 24, 17, 36, 0, 0)), childLocalUid));
	aTestData.InsertExpectedInstanceL(CTestData::TExpectedInstance(TTime(TDateTime(2006, ENovember, 25, 17, 36, 0, 0)), childLocalUid));
	}

CTestData* CInstanceIteratorTestManager::CreateTestDataLC(TInt aTestCase)
	{
	CTestData* testData = new (ELeave) CTestData;
	CleanupStack::PushL(testData);
	// create the test data
	testData->iTestType = CTestData::ESingleCalendarFileTest;
	switch (aTestCase)
		{
		case ERepeatingEvent:
			AddRepeatingEventL(*testData);
			break;
		case ERepeatingTodo:
			AddRepeatingTodoL(*testData);
			break;
		case EAddRepeatingTodoWithNonRepeatingChildrenTodo:
			AddRepeatingTodoWithNonRepeatingChildrenTodoL(*testData);
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
		case ERepeatWithRepeatChild:
			AddRepeatWithRepeatChildL(*testData);
			break;
		case EAddRepeatingEventToMultipleFiles:
			testData->iTestType = CTestData::EMultipleCalendarFilesTest;
			AddRepeatingEventToMultipleFilesL(*testData);
			break;
		case EAddRepeatingTodoWithNonRepeatingChildToMultipleFiles:
			testData->iTestType = CTestData::EMultipleCalendarFilesTest;
			AddRepeatingTodoWithNonRepeatingChildToMultipleFilesL(*testData);
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
	CCalInstanceView* multiInstanceView = NULL;
	CCalSession* secondSession = NULL;
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	
	if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
		{
		secondSession = CCalSession::NewL(iTestLibrary->GetSession());
		CleanupStack::PushL(secondSession);
		secondSession->OpenL(KTestSecondFileName);
		sessions.AppendL(&iTestLibrary->GetSession());
		sessions.AppendL(secondSession);
		multiInstanceView = CCalInstanceView::NewL(sessions);
		CleanupStack::PushL(multiInstanceView);
		}

	for (TInt startingInstanceIndex(0) ; startingInstanceIndex < KExpectedInstanceCount ; ++startingInstanceIndex)
		{
		// We will create an iterator for every instance in the test data
		// then test iterating forwards and backwards
		
		test.Printf(_L("Create iterator and iterate forwards using NextL()"));
		
		CTestData::TExpectedInstance startInstance = aTestData.iExpectedInstances[startingInstanceIndex];
		
		// Call HasMore between each step to check the result
		CCalInstanceIterator* instanceIterator = NULL;
		if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
			{
			instanceIterator = multiInstanceView->FindInstanceByUidL(aTestData.iExpectedInstances[startingInstanceIndex].iFileId, *aTestData.iUid, startInstance.iTime);
			}
		else
			{
			instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*aTestData.iUid, startInstance.iTime);
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
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iFileId);
				test(instance->Entry().UidL() == *aTestData.iUid);
				}
			else
				{
				test.Printf(_L("Test this instance is the correct expected one"));
				test(instance->Time().TimeLocalL() == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iTime.TimeLocalL());
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iLocalUid);
				test(instance->Entry().UidL() == *aTestData.iUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex + foundInstancesForwards + 1].iFileId);
				}
			
			CleanupStack::PopAndDestroy(instance);
			foundInstancesForwards++;
			}
		
		test.Printf(_L("test NextL() returns NULL when at the end"));
		test(instanceIterator->NextL() == NULL);
		
		test.Printf(_L("test the corret number of instances were found when iterating forwards"));
		test(foundInstancesForwards == expectedInstancesForwards);
		
		CleanupStack::PopAndDestroy(instanceIterator);
		
		test.Printf(_L("Create iterator and iterate backwards using PreviousL()"));
		
		if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
			{
			instanceIterator = multiInstanceView->FindInstanceByUidL(aTestData.iExpectedInstances[startingInstanceIndex].iFileId, *aTestData.iUid, startInstance.iTime);
			}
		else
			{
			instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*aTestData.iUid, startInstance.iTime);
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
				test(instance->Entry().UidL() == *aTestData.iUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iFileId);
				}
			else
				{
				test.Printf(_L("Test this instance is the correct expected one"));
				test(instance->Time().TimeLocalL() == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iTime.TimeLocalL());
				test(instance->Entry().LocalUidL() == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iLocalUid);
				test(instance->Entry().UidL() == *aTestData.iUid);
				test(instance->InstanceIdL().iCollectionId == aTestData.iExpectedInstances[startingInstanceIndex - foundInstancesBackwards - 1].iFileId);
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
	
	if(aTestData.iTestType == CTestData::EMultipleCalendarFilesTest)
		{
		CleanupStack::PopAndDestroy(multiInstanceView);
		secondSession->DeleteCalFileL(KTestSecondFileName);
		CleanupStack::PopAndDestroy(secondSession);
		}
	CleanupStack::PopAndDestroy(&sessions);

	}

// Deleting entries
void CInstanceIteratorTestManager::TestDeleteEntryBeforeIteratingL()
	{
	iTestLibrary->CleanDatabaseL();
	
	test.Printf(_L("test FindInstanceByUidL returns KErrNotFound when starting instance not found"));
	// Create a new entry and store it
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
			5);
	iTestLibrary->StoreEntryL(*entry);
	HBufC8* uid = entry->UidL().AllocLC();
		
	// Clean the database
	iTestLibrary->CleanDatabaseL();
	TCalTime calTime;
	calTime.SetTimeLocalL(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	TRAPD(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*uid, calTime));
	test(err == KErrNotFound);
	
	CleanupStack::PopAndDestroy(2);
	}

void CInstanceIteratorTestManager::TestExceptedEntryBeforeIteratingL()
	{
	iTestLibrary->CleanDatabaseL();
	
	test.Printf(_L("test FindInstanceByUidL returns KErrNotFound when starting instance has been excepted"));
	
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
	HBufC8* uid = entry->UidL().AllocLC();
			
	TCalTime calTime;
	calTime.SetTimeLocalL(TDateTime(2006, ENovember, 24, 18, 15, 0, 0));
	TRAPD(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*uid, calTime));
	test(err == KErrNotFound);
	
	CleanupStack::PopAndDestroy(2);
	}

void CInstanceIteratorTestManager::TestDeleteEntryWhileIteratingL()
	{
	iTestLibrary->CleanDatabaseL();
	
	test.Printf(_L("test the iterator if the entry is deleted after we start iterating"));
	
	// Create a new entry and store it
	CCalEntry* entry = CreateRepeatingEntryLC(CCalEntry::EEvent,
			TTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0)),
			5);
	iTestLibrary->StoreEntryL(*entry);
	HBufC8* uid = entry->UidL().AllocLC();
	
	// Create the iterator
	TCalTime calTime;
	calTime.SetTimeLocalL(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	CCalInstanceIterator* instanceIterator = 0;
	TRAPD(err, instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceByUidL(*uid, calTime));
	test(err == KErrNone);
	CleanupStack::PushL(instanceIterator);
	
	// Delete the entry
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry);
	
	CCalInstance* instance = 0;
	// Verify that we can continue iterating
	do
		{
		TRAPD(err, instance = instanceIterator->NextL());
		delete instance;
		instance = 0;
		// In the current implementation this will actually work but leaving with KErrNotFound would be acceptable too.
		test((err == KErrNotFound) || (err == KErrNone));
		if (err == KErrNotFound)
			{
			break;
			}
		}
	while (instance);
	
	CleanupStack::PopAndDestroy(3);
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
	
	// Test deleting entries before and during iterations

	test.Next(_L("TestDeleteEntryBeforeIteratingL"));

	TestDeleteEntryBeforeIteratingL();
	
	// Test iterating from an excepted instance

	test.Next(_L("TestExceptedEntryBeforeIteratingL"));

	TestExceptedEntryBeforeIteratingL();
	
	// Test iterating through a deleted entry

	test.Next(_L("TestDeleteEntryWhileIteratingL"));

	TestDeleteEntryWhileIteratingL();
	
	// Out of memory testing
	test.Next(_L("OOM test"));

	OomTestL();
	}
	
static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	
	testManager->RunTestsL();
	
	CleanupStack::PopAndDestroy(testManager);
	}

/**

@SYMTestCaseID     PIM-TCAL-INSTANCEITERATOR-UID-0001

*/


TInt E32Main()
    {
	__UHEAP_MARK;
	
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-INSTANCEITERATOR-UID-0001 tcal_instanceiterator_uid"));


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
