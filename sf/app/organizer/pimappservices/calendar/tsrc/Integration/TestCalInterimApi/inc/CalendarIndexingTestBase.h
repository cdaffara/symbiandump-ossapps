/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


 
#ifndef CALENDAR_INDEXING_TEST_BASE__INCLUDED_
#define CALENDAR_INDEXING_TEST_BASE__INCLUDED_

#include <calcategorymanager.h>
#include <calsession.h>
#include <calinstanceview.h>
#include <calentryview.h>
#include "TestCalInterimApiSuiteStepBase.h"
#include "CalendarTestUtility.h"
#include "entryparser.h"

_LIT(KInfoGUIDEntry,						"GUID of entry that instance belongs to: %S");

class CCalendarIndexingTestBase : public CTestCalInterimApiSuiteStepBase
{	
protected:
	enum TOperationType
		{
		EStore = 0,
		EDeleteEntry,
		EDeleteInstance,
		EUpdate,
		EAddChild,
		ESetTimeType,
		};

public:	
	virtual ~CCalendarIndexingTestBase();
	virtual TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();
	void MeasureViewCreationL(
			TTime aStartTime,
			TTime aEndTime,
			const TInt aYearsFromNow,
			const TDesC& aDescription
			);
	
protected:
					CCalendarIndexingTestBase();
	void 			OpenViewsL();
	void 			CloseViews();
	virtual void 	AddEntriesFromConfigL( CEntryBuilder& aEntryBuilder );
	void 			UpdateEntriesFromConfigL( 
						CEntryBuilder& aEntryBuilder,
						const CEntryProperty& aEntryUpdate 
						);
	virtual void 	PerformTestCaseL( ) = 0;
	virtual void 	ParseConfigSection(const TDesC& aSection);
	virtual void 	ProcessConfigDataL();
	void 			ExecuteTestCaseOperationL(
         			   const CCalEntry& aEntry, 
         			   const TCalTime& aStartDate, 
         			   const TCalTime& aEndDate
         			   );                         
    TBool			ValidateViewsL(
         			  const CCalEntry& aEntry, 
         			  const TCalTime aStartDate, 
         			  const TCalTime aEndDate
         			  );
    void 			CloseWaitAndReopenSessionL();
    virtual void 	CloseSessions();
    virtual void 	OpenSessionsL();

	TBool 			CompareEntriesToViewsL(
				        const RPointerArray< CCalEntry >& aEntryList,
				        const CalCommon::TCalTimeRange& aTimeRange 
				        );
				        
	CEntryProperty& GetEntryProperty();	
	CEntryBuilder& 	GetEntryBuilderL();	
	void 			UnloadEntryBuilder();
	void 			DeleteCalendarFile();
	TBool 			VerifyCategoriesL();
	void 			StartOperationMeasurement();
	TTimeIntervalMicroSeconds EndOperationMeasurement(
								const TDesC& aOperationDescription
								);	
    TBool			CompareEntryToViewsL( 
	    				const CCalEntry& aEntry,
	    				const CalCommon::TCalTimeRange& aTimeRange 
	    				);
	TBool 			CompareDeletedEntryToViewsL(
		      			  const CCalEntry& aEntry,
		      			  const CalCommon::TCalTimeRange& aTimeRange
		      			  );   
	TBool 			ValidateChildren(
				        const CCalEntry& aEntry,
				        const TInt aChildCount
				        );		     
	            
private:
	TBool 			CompareEntryToInstances(
	      			  const CCalEntry& aEntry,
	      			  const CalCommon::TCalTimeRange& aTimeRange
	      			  );
	TBool 			CompareEntryToInstanceL( 
						const CCalInstance& aInstance, 
						const CCalEntry& aEntry
						);	
	TBool 			CompareRepeatEntryToInstanceTimes(
						const CCalInstance& aInstance, 
						const CCalEntry& aEntry
						);
	void 			GetMatchingEntriesL( const CCalCategory& aCategory );
	void 			UpdateEntryL(
						CEntryBuilder& aEntryBuilder, 
						const CEntryProperty& aEntryPropertyUpdate, 
						const CCalEntry& aUpdateEntry 
						);
	void 			DeleteInstanceL(
				        const CCalEntry& aEntry,
				        const TInt aInstanceNumber,
        				const CalCommon::TCalTimeRange& aTimeRange 
				        );
	void 			AddChildL(
				        const CCalEntry& aEntry,
				        const TInt aChildCount,
				        const CalCommon::TCalTimeRange& aTimeRange 
				        );
	            		
protected:
	//instance view
	CCalInstanceView*       		iInstanceView;
	//entry view
	CCalEntryView*          		iEntryView;
	//utility class
	CCalendarTestUtility*   		iUtility;
	//type of operations to execute within test case
	TOperationType          		iOperationType;
	//this is the actual current time, reset to this value after test is complete
	TTime                   		iPresentTime;
	//set time to this value when performing test case
	TTime                   		iTestTime;
	//the actual timezone prior to testing
	CTzId*                  		iRealTimeZone;
	//timezone to be used by all tests
	CTzId*                  		iInitialTimeZone;
	//timezone value to be used by a specific test case
	CTzId*                  		iNewTimeZone;
	//config section storing data for initial entries list
	TPtrC                   		iInitialEntries;
	//config section storing data for test case entries list	
	TPtrC                   		iTestCaseEntries;
	RTz                     		iTzServer;
	//number of entries stored in calendar server by test
    TInt                            iStoredEntryCount;
    //entry parsing objects
    CEntryProperty*					iInitialEntryProperties;
    CEntryProperty*					iTestEntryProperties;
    TEntryParser					iParser;
    //todo:: make private
	CCalCategoryManager*			iCategoryManager;

private:   
	//creates a list of entries to be used by the test
	CEntryBuilder*  				iEntryBuilder;
	RPointerArray< CCalEntry >		iFilteredEntries;
	RPointerArray< CCalEntry >		iCategoryEntries;
	TTime							iOperationStart;

};
#endif // ifndef CALENDAR_INDEXING_TEST_BASE__INCLUDED_
