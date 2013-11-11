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
 
#include <calinstance.h>
#include "CalendarIndexingPerformanceTests.h"

//print string, then print heap size
#define HEAPSIZE( string )					\
				{							\
				_LIT(KHeapPrint, string );	\
				INFO_PRINTF1( KHeapPrint );	\
				PrintHeap();				\
				}

//print string and view count, then print heap size				
#define HEAPSIZEVIEW( string, count )				\
				{									\
				_LIT(KHeapPrint, string );			\
				INFO_PRINTF2( KHeapPrint, count );	\
				PrintHeap();						\
				}				
	
_LIT(KDailyView, "load a daily view");
_LIT(KWeeklyView, "load a weekly view");
_LIT(KMonthlyView, "load a monthly view");
const static TInt KViewStartYearsFromNow = -3;
const static TInt KViewEndYearsFromNow = 4;
const static TInt KViewYearsFromNowIncrement = 3;

CCalendarIndexingStartupPerformanceTest::CCalendarIndexingStartupPerformanceTest()
    {
    SetTestStepName(KCalendarIndexingStartupPerformanceTest);
    }

void CCalendarIndexingStartupPerformanceTest::PerformTestCaseL( )
    {
    iNotifyProgress = EFalse;
    //create entries using config data file        
    const TInt KEntryCount = GetEntryBuilderL().Count();
    
    //store test entries
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    //close server
    CloseSessions();
    iUtility->FindAndCloseCalendarServer( CCalendarTestUtility::EWaitForServerToClose, EFalse );
    //mesaure lenght of time to open calendar server, session, views.
    StartOperationMeasurement();
    
	iSession = CCalSession::NewL();
	iSession->OpenL(iCalenderFileName);
	OpenViewsL();
	_LIT(KSessionOpen, "open a session, default calendar file, an entry view and an instance view");
    EndOperationMeasurement( KSessionOpen() );
    //print measurement
    
	CloseViews();
	DELETE( iSession );
	
    OpenSessionsL();
    
    TTime startDayDate;
    TTime endDayDate;
    TTime startWeekDate;
    TTime endWeekDate;
    TTime startMonthDate;
    TTime endMonthDate;
	
	iUtility->SetPeriodStartAndEndDates( startDayDate, endDayDate, CCalendarTestUtility::EDay);  
	iUtility->SetPeriodStartAndEndDates( startWeekDate, endWeekDate, CCalendarTestUtility::EWeek);        
	iUtility->SetPeriodStartAndEndDates( startMonthDate, endMonthDate, CCalendarTestUtility::EMonth);
	
	//measure length of time to  create daily, weeakly and monthly view, iterates through a decade range
    for( TInt yearsFromNow = -3; yearsFromNow < 4; yearsFromNow+=3 )
    	{
		MeasureViewCreationL( startDayDate, endDayDate, yearsFromNow, KDailyView);
		MeasureViewCreationL( startWeekDate, endWeekDate, yearsFromNow, KWeeklyView);
		MeasureViewCreationL( startMonthDate, endMonthDate, yearsFromNow, KMonthlyView);
    	}              
    
    }
	
CCalendarIndexingPerformanceDegradationTest::CCalendarIndexingPerformanceDegradationTest()
    {
    SetTestStepName(KCalendarIndexingPerformanceDegradationTest);
    }
    
void CCalendarIndexingPerformanceDegradationTest::PerformTestCaseL( )
    {
    iNotifyProgress = EFalse;

    TTime startDayDate;
    TTime endDayDate;
    TTime startWeekDate;
    TTime endWeekDate;
    TTime startMonthDate;
    TTime endMonthDate;
	
	iUtility->SetPeriodStartAndEndDates( startDayDate, endDayDate, CCalendarTestUtility::EDay);  
	iUtility->SetPeriodStartAndEndDates( startWeekDate, endWeekDate, CCalendarTestUtility::EWeek);        
	iUtility->SetPeriodStartAndEndDates( startMonthDate, endMonthDate, CCalendarTestUtility::EMonth);
        	
    RPointerArray< CCalEntry >* entryList = &GetEntryBuilderL().GetEntryList();
    CEntryProperty& entryProperty = GetEntryProperty();
    const TInt KEntryCount = entryList->Count();
	
	//measure length of time taken to perform operation
	StartOperationMeasurement();
	
    //execute test operation on created entries
    for( TInt i = 0; i < KEntryCount; ++i )
        {
        CCalEntry* entry = (*entryList)[i];
        CCalEntry::TType type = entry->EntryTypeL();
                
        ExecuteTestCaseOperationL( *entry, entryProperty.iStartDate, entryProperty.iEndDate );
        }
        
    _LIT(KTimeOperation, "Perform operation on instance/entry");
    EndOperationMeasurement( KTimeOperation() );
    //end measurement
            
	//measure length of time to  create daily, weeakly and monthly view, iterates through a decade range
    for( TInt yearsFromNow = KViewStartYearsFromNow; yearsFromNow < KViewEndYearsFromNow; yearsFromNow+=KViewYearsFromNowIncrement )
    	{	
		MeasureViewCreationL( startDayDate, endDayDate, yearsFromNow, KDailyView);
		MeasureViewCreationL( startWeekDate, endWeekDate, yearsFromNow, KWeeklyView);
		MeasureViewCreationL( startMonthDate, endMonthDate, yearsFromNow, KMonthlyView);
    	}         
    
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    
    //measure length of time to  create daily, weeakly and monthly view, iterates through a decade range
    for( TInt yearsFromNow = KViewStartYearsFromNow; yearsFromNow < KViewEndYearsFromNow; yearsFromNow+=KViewYearsFromNowIncrement )
    	{	
		MeasureViewCreationL( startDayDate, endDayDate, yearsFromNow, KDailyView);
		MeasureViewCreationL( startWeekDate, endWeekDate, yearsFromNow, KWeeklyView);
		MeasureViewCreationL( startMonthDate, endMonthDate, yearsFromNow, KMonthlyView);
    	}           
    }

CCalendarIndexingRamPerformanceTest::CCalendarIndexingRamPerformanceTest()
    {
    SetTestStepName(KCalendarIndexingRamPerformanceTest);
    }

TVerdict CCalendarIndexingRamPerformanceTest::doTestStepPreambleL()
	{
	HEAPSIZE( "Heap size prior to test data loading" );
	return CCalendarIndexingTestBase::doTestStepPreambleL();
	}
	
TVerdict CCalendarIndexingRamPerformanceTest::doTestStepPostambleL()
	{
	CCalendarIndexingTestBase::doTestStepPostambleL();
	UnloadEntryBuilder();
	
	HEAPSIZE( "Heap size after entries unloaded in preamble" );
	return TestStepResult();
	}
    
void CCalendarIndexingRamPerformanceTest::PerformTestCaseL( )
    {
    
    TTime startDayDate;
    TTime endDayDate;
    TTime startWeekDate;
    TTime endWeekDate;
    TTime startMonthDate;
    TTime endMonthDate;
	
	iUtility->SetPeriodStartAndEndDates( startDayDate, endDayDate, CCalendarTestUtility::EDay);  
	iUtility->SetPeriodStartAndEndDates( startWeekDate, endWeekDate, CCalendarTestUtility::EWeek);        
	iUtility->SetPeriodStartAndEndDates( startMonthDate, endMonthDate, CCalendarTestUtility::EMonth);
    
    UnloadEntryBuilder();
    HEAPSIZE( "Heap size prior to entry creation" );
    
    //create entries using config data file        
    GetEntryBuilderL();
    HEAPSIZE( "Heap size after entry creation" );
    
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    
    HEAPSIZE( "Heap size after entries are stored" );
    
    //measure client ram usage to create daily, weeakly and monthly view, iterates through a decade range
    for( TInt yearsFromNow = KViewStartYearsFromNow; yearsFromNow < KViewEndYearsFromNow; yearsFromNow+=KViewYearsFromNowIncrement )
    	{
		InstanceViewRamUsageL( startDayDate, endDayDate, yearsFromNow);
		InstanceViewRamUsageL( startWeekDate, endWeekDate, yearsFromNow);
		InstanceViewRamUsageL( startMonthDate, endMonthDate, yearsFromNow);
    	}
    	
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    
    HEAPSIZE( "Heap size after server restarted" );
    
    //measure client ram usage to create daily, weeakly and monthly view, iterates through a decade range
    for( TInt yearsFromNow = KViewStartYearsFromNow; yearsFromNow < KViewEndYearsFromNow; yearsFromNow+=KViewYearsFromNowIncrement )
    	{
		InstanceViewRamUsageL( startDayDate, endDayDate, yearsFromNow);
		InstanceViewRamUsageL( startWeekDate, endWeekDate, yearsFromNow);
		InstanceViewRamUsageL( startMonthDate, endMonthDate, yearsFromNow);
    	}	
    }

//print client ram usage	
void CCalendarIndexingRamPerformanceTest::PrintHeap()
	{
	_LIT(KHeapPrint, "Client Heap cells allocated: %d, Heap Size: %d bytes\n");
	INFO_PRINTF3( KHeapPrint, User::Heap().Count(), User::Heap().Size() );
	}
	
void CCalendarIndexingRamPerformanceTest::InstanceViewRamUsageL(
		TTime aStartTime,
		TTime aEndTime,
		const TInt aYearsFromNow)
	{
	TTimeIntervalYears yearsFromNow( aYearsFromNow );
	
	//inc/decrement years form start/end times
	aStartTime += yearsFromNow;
	aEndTime += yearsFromNow;
	
	//set start/end times
	TCalTime tcalStartTime;
	TCalTime tcalEndTime;
	tcalStartTime.SetTimeLocalL( aStartTime );
	tcalEndTime.SetTimeLocalL( aEndTime );
	
	_LIT(KStartTime, "Instance View StartTime: ");
	_LIT(KEndTime, "Instance View EndTime: ");
	
	//print start/end tiems
	INFO_PRINTF1(KStartTime);
	PrintTimeL( aStartTime );
	INFO_PRINTF1(KEndTime);
	PrintTimeL( aEndTime );
	
	HEAPSIZE( "Heap size before instance view generated" );
    RPointerArray< CCalInstance  > viewInstances;
    CleanupResetDestroyPushL( viewInstances );
    
    iInstanceView->FindInstanceL( viewInstances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(tcalStartTime, tcalEndTime) );
	HEAPSIZEVIEW( "Heap size after instance view generated %d instances", viewInstances.Count() );
	
	CleanupStack::PopAndDestroy( &viewInstances );
	HEAPSIZE( "Heap size after instance view array destroyed" );
	}


