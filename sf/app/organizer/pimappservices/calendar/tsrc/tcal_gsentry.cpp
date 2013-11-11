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
#include "tcal_gsentry_testdata.h"

#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <e32test.h>

_LIT(KFormatDate, "%D%M%Y%/0%1%/1%2%/2%3%/3");
_LIT(KCalendarFile, "tCal_GsEntry");

class CDummyCalendarApp : public CBase
	{
public:
	static CDummyCalendarApp* NewLC();
	~CDummyCalendarApp();
	void DoTestL();
	
private:
	CDummyCalendarApp();
	void ConstructL();
	
	void OpenCleanFileL();
    void FillDefaultDatabaseL(TInt aTestCount, TBool aFloating, TBool aSummerTime = EFalse);
	void ExtractFromDatabaseL();
	
    void ExtractEntriesL(TBool aCompareWithExpectedNum = EFalse, TInt aExpectedNum = 0);
	void ExtractInstancesL();
	void ExtractNonExistentEntryL();


private: // member data
	CCalTestLibrary* iTestLib;
	CCalInstanceView* iInstanceView;
	CCalEntryView* iEntryView;
	TTestDataStruct* iCurrentTestData;
	};

CDummyCalendarApp* CDummyCalendarApp::NewLC()
	{
	CDummyCalendarApp* self = new (ELeave) CDummyCalendarApp();
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}
	
CDummyCalendarApp::~CDummyCalendarApp()
	{
	delete iTestLib;
	delete iCurrentTestData;
	}

CDummyCalendarApp::CDummyCalendarApp()
	{
	}

void CDummyCalendarApp::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	}

void CDummyCalendarApp::OpenCleanFileL()
	{
	iTestLib->ReplaceFileL(KCalendarFile);
	iTestLib->OpenFileL(KCalendarFile);
	
	iInstanceView = &iTestLib->SynCGetInstanceViewL();
	iEntryView = &iTestLib->SynCGetEntryViewL();
	}
	
void CDummyCalendarApp::FillDefaultDatabaseL(TInt aTestCount, TBool aFloating, TBool aSummerTime)
	{
	if (iCurrentTestData)
		{
		delete iCurrentTestData;
		iCurrentTestData = NULL;
		}
   	iCurrentTestData = CTestData::GetTestDataL((CTestData::TTestNumber)aTestCount, iInstanceView, iEntryView, aFloating, aSummerTime);
	}

static void ResetAndDestroyCalEntryArray(TAny* aPtr)
	{
	if (aPtr)
		{
		RPointerArray<CCalEntry>* array = static_cast<RPointerArray<CCalEntry>*>(aPtr);
		array->ResetAndDestroy();
		}
	}
	
// Helper method
// Extract all entries given by Guids in iCurrentTestData
void CDummyCalendarApp::ExtractEntriesL(TBool aCompareWithExpectedNum, TInt aExpectedNum)
	{
	// Extract entries by the GUIDs given in current-test-data's iGuids
	for(TInt a = 0; a < iCurrentTestData->iGuidsToExtract.Count(); a++)
		{
		TPtrC8 guid = iCurrentTestData->iGuidsToExtract[a];
		
		RPointerArray<CCalEntry> entryArray;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyCalEntryArray, &entryArray));
		iEntryView->FetchL(guid, entryArray);
		
		test.Printf(_L("%d GS entries fetched\n"), entryArray.Count());

        if (aCompareWithExpectedNum)
            {
            test.Printf(_L("%d entries were expected"), aExpectedNum);
            test(entryArray.Count() == aExpectedNum);
            }

		CCalEntry* entryPtr;
		for(TInt a = 0; a < entryArray.Count(); a++)
			{
			entryPtr = entryArray[a];
			if(a==0)
				{
				test.Printf(_L("Parent -> SeqNum:%d\n"), entryPtr->SequenceNumberL());
				}
			else
				{
				TBuf<16> recIdTimeBuf;
				TTime recIdTime = entryPtr->RecurrenceIdL().TimeLocalL();
				recIdTime.FormatL(recIdTimeBuf, KFormatDate());
				
				test.Printf(_L("Child  -> SeqNum:%d, RecID: %S\n"), entryPtr->SequenceNumberL(), &recIdTimeBuf);
				}
			}
		
		CleanupStack::Pop(); // TCleanupItem(ResetAndDestroyCalEntryArray, &entryArray)
		entryArray.ResetAndDestroy();
		}
	}
	
// Helper method
// Extract all instances given by Guids in iCurrentTestData
void CDummyCalendarApp::ExtractInstancesL()
	{
	TCalTime startDate;
	startDate.SetTimeLocalL(iCurrentTestData->iStartDate);
	TCalTime endDate;
	endDate.SetTimeLocalL(iCurrentTestData->iEndDate);

	CalCommon::TCalTimeRange timeRange(startDate, endDate);
	
	RPointerArray<CCalInstance> instanceArray;
	
	if (iCurrentTestData->iSearchString)
		{
		CCalInstanceView::TCalSearchParams searchParams(*iCurrentTestData->iSearchString, iCurrentTestData->iSearchBehaviour);
		
		iInstanceView->FindInstanceL(instanceArray, 
									(CalCommon::TCalViewFilter)iCurrentTestData->iFilter, 
									timeRange, 
									searchParams);
		}
	else
		{
		iInstanceView->FindInstanceL(instanceArray, 
							(CalCommon::TCalViewFilter)iCurrentTestData->iFilter, 
							timeRange);		
		}
	
	TBuf<26> startBuf;
	TBuf<26> endBuf;
	startDate.TimeLocalL().FormatL(startBuf,KFormatDate());
	endDate.TimeLocalL().FormatL(endBuf,KFormatDate());
	
	test.Printf(_L("Checking instances between %S and %S\n"), &startBuf, &endBuf);
	
	if (instanceArray.Count() == 0)
		{
		test.Printf(_L("No instances found\n"));
		}
	else
		{
		TInt instanceCount = 0;
		TTime instTime;
		TBuf<26> instBuf;

		while (instanceCount < instanceArray.Count())
			{
			CCalInstance* inst = instanceArray[instanceCount];
			CleanupStack::PushL(inst);

			instTime = inst->Time().TimeLocalL();
			instTime.FormatL(instBuf,KFormatDate());
	
			TBuf<62> rptType;
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
						ASSERT(0);
						break;
					}
				}
				
			test.Printf(_L("One instance is on %S, %d:%d - %S"), &instBuf, instTime.DateTime().Hour(), instTime.DateTime().Minute(), &rptType);
			
			if (iCurrentTestData->iTimes.Find(instTime) == KErrNotFound)
				{
				test.Printf(_L("THIS ENTRY WAS NOT EXPECTED\n"));
				test(0);
				}
				
			instanceCount++;
			CleanupStack::Pop(inst);
			}
			
		if (iCurrentTestData->iFunction)
			ASSERT(iCurrentTestData->iFunction->CallBack() == 0);
		}

	if (instanceArray.Count() != iCurrentTestData->iTimes.Count())
		{
		test.Printf(_L("Some expected entries were not found!\n"));
		test(0);	
		}
		
	instanceArray.ResetAndDestroy();
	}
	
void CDummyCalendarApp::ExtractFromDatabaseL()
	{
	// Print test's description
	// Check if description field is longer than 124 characters and split the print if it is.
	// (printf does not seem to handle string length > 124)
	TInt len(iCurrentTestData->iDescription.Length());
	if(len > 124)
		{
		TInt num(len / 124);
		TBuf<TTestDataStruct::KDescriptionLen> tmpBuff(iCurrentTestData->iDescription);
		for(TInt i = 0; i < num; ++i)
			{
			test.Printf(_L("%.124S"), &tmpBuff);
			tmpBuff.Delete(0, 124);
			}
		if(tmpBuff.Length())
			{
			test.Printf(_L("%S\n"), &tmpBuff);
			}
		}
	else
		{
		test.Printf(_L("TEST: %S\n"), &iCurrentTestData->iDescription);
		}	

	if(iCurrentTestData->iTestType == TTestDataStruct::EEntryTest)
		{
		ExtractEntriesL();
		}
		
	else if(iCurrentTestData->iTestType == TTestDataStruct::EInstanceTest)
		{
		ExtractInstancesL();
		}
	}

void CDummyCalendarApp::ExtractNonExistentEntryL()
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyCalEntryArray, &entryArray));
	
	// Passing string type UID parameter and expecting entryArray to be empty
	iEntryView->FetchL( _L8("myuid"), entryArray);
	test(entryArray.Count() == 0);
	entryArray.Reset();
	
	// Passing integer type UID parameter and expecting entryArray to be empty
	iEntryView->FetchL( _L8("10"), entryArray);
	test(entryArray.Count() == 0);
	
	CleanupStack::Pop(); // TCleanupItem(ResetAndDestroyCalEntryArray, &entryArray)
	entryArray.ResetAndDestroy();
	}


void CDummyCalendarApp::DoTestL()
	{
	TInt testCount(0);
	
	while (testCount < CTestData::ENumberOfTestsForFixedAndFloating)
		{
		OpenCleanFileL();
		FillDefaultDatabaseL(testCount, EFalse);
		ExtractFromDatabaseL();
		testCount++;
		iTestLib->CleanDatabaseL();
		}

	// repeat the above test for floating entry
	/**
	@SYMTestCaseID PIM-TCAL-GSENTRY-Floating_Parent_And_Child_Entry-0001
	@SYMTestCaseDesc Create floating parent and child entries
	@SYMFssID App-Engines/CalInterimAPI/Data.007
	@SYMTestStatus Implemented
	@SYMTestPriority Medium
	@SYMTestActions Create floating parent and child entries with various combination of repeating rule, excption dates, and RDates. 
	Store the entries, then check that the expected instances can be retrieved.
	@SYMTestExpectedResults The check shows that the instances defined by the repeated rule and exception dates and RDates are found
	@SYMTestType CT	
	*/
	testCount = 0;
	while (testCount < CTestData::ENumberOfTests)
		{
		OpenCleanFileL();
		FillDefaultDatabaseL(testCount, ETrue);
		ExtractFromDatabaseL();
		testCount++;
		iTestLib->CleanDatabaseL();
		}

    //Test with Local floating time the replacement of existing child entry
    OpenCleanFileL();
    //Fill the database with entries in LocalFloating format and with August dates
    //to get difference between UTC and Local time
    FillDefaultDatabaseL(CTestData::EReplaceExistingChild, ETrue, ETrue);
    ExtractEntriesL(ETrue, 3);//check the entries count, it should be 3

	//Test added for DEF069038
	ExtractNonExistentEntryL();
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CDummyCalendarApp* app = CDummyCalendarApp::NewLC();
	

	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	app->DoTestL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	

	CleanupStack::PopAndDestroy(app);
	}


/**

@SYMTestCaseID     PIM-TCAL-GSENTRY-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-GSENTRY-0001 Calendar Interim API Group Scheduling test suite"));

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

