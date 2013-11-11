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

#include "tcal_instance_testdata.h"
#include "caltestlib.h"

#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <calprogresscallback.h>
#include <calrrule.h>
#include <calsession.h>
#include <e32test.h>
#include <tz.h>

#include <calinstanceiterator.h>

RTest test(_L("tcal_instance"));

_LIT(KFormatDate, "%D%M%Y%/0%1%/1%2%/2%3%/3");
_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S.%C");
_LIT(KCalendarFile, "tcal_instance");

class CInstanceTestManager : public CBase
	{
public:
	enum TTestAPI
		{
		EFindInstance,
		EIteratorNextInstance,
		ETotalTestAPI
		};
public:
	static CInstanceTestManager* NewLC();
	~CInstanceTestManager();
	
	inline void CleanDatabaseL();
	void TestPopulateAndCheckL(TInt aTestAPI);
	void TestEditAndCheckL(TInt aTestAPI);	


private:

	// avoid any method but NewL instancing the class
	CInstanceTestManager() {	}
	// no copy constructor and assignment operator
	CInstanceTestManager(CInstanceTestManager& );
	CInstanceTestManager& operator = (const CInstanceTestManager& );

	void ConstructL();

	void ExtractFromDatabaseL(CTestDataStruct& aTestData, TInt aTestAPI);
	void EditEntriesL();


private: // member data
	CCalTestLibrary*  iTestLibrary;
	
	CCalEntryView*    iEntryView;	// not owned
	CCalInstanceView* iInstanceView;// not owned
	};


CInstanceTestManager* CInstanceTestManager::NewLC()
	{
	CInstanceTestManager* self = new (ELeave) CInstanceTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}
	

CInstanceTestManager::~CInstanceTestManager()
	{
	TRAP_IGNORE(iTestLibrary->DeleteFileL(KCalendarFile));
	delete iTestLibrary;
	}


void CInstanceTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();

	iTestLibrary->ReplaceFileL(KCalendarFile());
	iTestLibrary->OpenFileL(KCalendarFile());

	iInstanceView = &iTestLibrary->SynCGetInstanceViewL();
	iEntryView = &iTestLibrary->SynCGetEntryViewL();
	}
	
	
inline void CInstanceTestManager::CleanDatabaseL()
	{
	iTestLibrary->CleanDatabaseL();		
	}


void CInstanceTestManager::TestPopulateAndCheckL(TInt aTestAPI)
	{
	TInt testStart = 0; 
	TInt testEnd = CTestData::ENumberOfTests;


	for(TInt i = testStart; i < testEnd; ++i)
		{
		CTestDataStruct* testData = CTestData::GenerateTestDataLC((CTestData::TTestNumber)i, iInstanceView, iEntryView);

		ExtractFromDatabaseL(*testData, aTestAPI);

		CleanupStack::PopAndDestroy(testData);

		CleanDatabaseL();
		}		
	}


void CInstanceTestManager::TestEditAndCheckL(TInt aTestAPI)
	{
	TInt testStart = 0; 
	TInt testEnd = CTestData::ENumberOfTests;
	
	
	for(TInt j = testStart; j < testEnd; ++j)
		{
		CTestDataStruct* testData = CTestData::GenerateTestDataLC((CTestData::TTestNumber)j, iInstanceView, iEntryView);
		EditEntriesL();
		ExtractFromDatabaseL(*testData, aTestAPI);

		CleanupStack::PopAndDestroy(testData);

		CleanDatabaseL();
		}			
	}


// Edit entries
void CInstanceTestManager::EditEntriesL()
	{
	CCalIter& iter = iTestLibrary->GetIterL();
	
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()) )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);

		iEntryView->FetchL(Uid, entryList);
		// can't update child entries, so only the first entry in entryList can be used
		while (entryList.Count() > 1)
			{
			delete entryList[1];
			entryList.Remove(1);
			}
		
		entryList[0]->SetSequenceNumberL( 0 );
		// don't remove the original summary as it is used in text search tests 
		TBuf<256> summary;
		summary.Copy(entryList[0]->SummaryL());
		summary.Append( _L("0123456789 qwertyuiopasdfghjklzxcvbnm") );
		entryList[0]->SetSummaryL(summary);
		
		TInt updated = 0;

		iEntryView->UpdateL(entryList, updated);

		test( entryList.Count() == updated );		

		CleanupStack::PopAndDestroy(&entryList);		
		}
	}


// Call FindInstanceL and check the results
void CInstanceTestManager::ExtractFromDatabaseL(CTestDataStruct& aTestData, TInt aTestAPI)
	{
	TCalTime startDate;
	startDate.SetTimeLocalL(aTestData.iStartDate);
	TCalTime endDate;
	endDate.SetTimeLocalL(aTestData.iEndDate);

	CalCommon::TCalTimeRange timeRange(startDate, endDate);

	test.Printf(_L("TEST: %S\n"), &aTestData.iDescription);

	RPointerArray<CCalInstance> instanceArray;
	CleanupResetAndDestroyPushL(instanceArray);

	switch (aTestAPI)
		{
		case EFindInstance:
			{
			test.Printf(_L("EFindInstance\n"));
			
			if (aTestData.iSearchString)
				{
				CCalInstanceView::TCalSearchParams searchParams(*aTestData.iSearchString, aTestData.iSearchBehaviour);
				
				iInstanceView->FindInstanceL(instanceArray, aTestData.iFilter, timeRange, searchParams);
				}
			else
				{
				iInstanceView->FindInstanceL(instanceArray, aTestData.iFilter, timeRange);		
				}
			}
			break;
		case EIteratorNextInstance:
			{
			test.Printf(_L("EIteratorNextInstance\n"));
			
			if (aTestData.iFilter & CalCommon::EIncludeRptsNextInstanceOnly)
				{
				// the iterator does not support this filter flag so return
				CleanupStack::PopAndDestroy(&instanceArray);
				return;			
				}
			
			CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(aTestData.iFilter, timeRange);
			CleanupStack::PushL(findInstanceSettings);
			
			if (aTestData.iSearchString)
				{
				CCalInstanceView::TCalSearchParams searchParams(*aTestData.iSearchString, aTestData.iSearchBehaviour);
				findInstanceSettings->SetTextSearchL(searchParams);
				}
			
			CCalInstanceIterator* iterator = iInstanceView->FindInstanceL(*findInstanceSettings);
			CleanupStack::PopAndDestroy(findInstanceSettings);
			CleanupStack::PushL(iterator);
			
			while (CCalInstance* instance = iterator->NextL())
				{
				instanceArray.AppendL(instance);
				}
			
			CleanupStack::PopAndDestroy(iterator);
			}
			break;
		default:
			{
			test(EFalse);
			}
		}
	
	

	TBuf<50> startBuf;
	TBuf<50> endBuf;
	startDate.TimeLocalL().FormatL(startBuf,KFormatDate());
	endDate.TimeLocalL().FormatL(endBuf,KFormatDate());

	test.Printf(_L("Checking instances between %S and %S\n"), &startBuf, &endBuf);

	if (aTestData.iFunction)
		{
		ASSERT(aTestData.iFunction->CallBack() == 0); // this fails if test function returns non-zero
		aTestData.iFunction->iPtr = NULL;	// the callback is a one shot function that deletes the object..
		}
	
	if (instanceArray.Count() == 0)
		{
		test.Printf(_L("No instances found\n"));
		}
	else
		{
		TTime instTime;
		TTime instStartTime;
		TTime instEndTime;
		
		TBuf<50> instBuf;
		TBuf<50> instStartBuf;
		TBuf<50> instEndBuf;
		
		for (TInt instanceCount = 0; instanceCount < instanceArray.Count(); ++instanceCount)
			{
			CCalInstance* inst = instanceArray[instanceCount];
			instTime = inst->Time().TimeLocalL();
			instStartTime = inst->StartTimeL().TimeLocalL();
			instEndTime = inst->EndTimeL().TimeLocalL();
			
			instTime.FormatL(instBuf,KFormatDate());
			instStartTime.FormatL(instStartBuf,KFormatDateTime());
			instEndTime.FormatL(instEndBuf,KFormatDateTime());
			
			instBuf.Append(_L(" "));
			instBuf.AppendNum(instTime.DateTime().Hour());
			instBuf.Append(_L(":"));
			instBuf.AppendNum(instTime.DateTime().Minute());
			
			TBuf<32>  rptType;
			TCalRRule rptDef;
			if (!inst->Entry().GetRRuleL(rptDef))
				{
				rptType.Copy(_L("One off"));
				}
			else
				{
				switch (rptDef.Type())
					{
					case TCalRRule::EDaily:
						rptType.Format(_L("Daily every %d days"), rptDef.Interval());
						break;
					case TCalRRule::EWeekly:
						rptType.Format(_L("Weekly every %d weeks"), rptDef.Interval());
						break;
					case TCalRRule::EMonthly:
						rptType.Format(_L("MonthlyByDates every %d months"), rptDef.Interval());
						break;
					case TCalRRule::EYearly:
						rptType.Format(_L("YearlyByDate every %d years"), rptDef.Interval());
						break;
					default:
						test(0); // should never get here unless somehow a repeat rule has an invalid type!
						break;
					}
				}

			TBool instanceNotExpected = EFalse;
				
			if (inst->Entry().EntryTypeL() == CCalEntry::ETodo)
				{
				// For a todo the 'end time' is the 'time' of the instance
				TDateTime dateTime = instEndTime.DateTime();
								
				//**NOTE**//
				//In case of undated ToDo's the instance time is current time, it may change slightly as
				//the FindInstance() and other operations take some amount of time thereby changing the
				//seconds and microseconds, which will cause the test condition below to fail.
				//So,while adding Expected instance time to aTestData.iTimes, we set the seconds
				//and microseconds to 0.
				//Now, to match that time we need to set the seconds and microseconds
				//of retrieved instance time to 0 as well.
					
				dateTime.SetMicroSecond(0);
				dateTime.SetSecond(0);
				instEndTime = TTime(dateTime);

				if (aTestData.iTimes.Find(instEndTime) == KErrNotFound)
					{
					instanceNotExpected = ETrue;
					}
				}
			else
				{
				if (aTestData.iTimes.Find(instTime) == KErrNotFound && instStartTime == instTime)
					{
					instanceNotExpected = ETrue;
					}
				}
				
#ifdef __WINS__
			test.Printf(_L("One instance is on %S to %S - %S\n"), &instStartBuf, &instEndBuf, &rptType);
			if (instanceNotExpected)
				{
				test.Printf(_L("THIS ENTRY WAS NOT EXPECTED"));
				test(0);
				}
#else
// don't output every instance on non-WINS environment because the log size is too great
			if (instanceNotExpected)
				{
				test.Printf(_L("Unexpected instance at %S to %S - %S\n"), &instStartBuf, &instEndBuf, &rptType);
				test(0);
				}
#endif
			}
		}
		
	if (instanceArray.Count() != aTestData.iTimes.Count())
		{
		test.Printf(_L("Some expected entries were not found!"));
		test(0);	
		}
		
	CleanupStack::PopAndDestroy(&instanceArray);
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
LOCAL_C void DoTestL()
	{
	CInstanceTestManager* testManager = CInstanceTestManager::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	for (TInt i(0) ; i < CInstanceTestManager::ETotalTestAPI ; ++i)
		{
		testManager->CleanDatabaseL();
		testManager->TestPopulateAndCheckL(i);
		testManager->TestEditAndCheckL(i);
		}


	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();	


	CleanupStack::PopAndDestroy(testManager);	
	}


/**
@SYMTestCaseID     PIM-TCAL-INSTANCE-0001
*/
TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-INSTANCE-0001 Calendar Interim API Instance test suite"));
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

