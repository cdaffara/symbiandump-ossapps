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


 
#ifndef CALENDAR_TEST_ENTRY_PARSER__INCLUDED_
#define CALENDAR_TEST_ENTRY_PARSER__INCLUDED_

#include <calcategorymanager.h>
#include "TestCalInterimApiSuiteStepBase.h"
#include "CalendarTestUtility.h"

class CEntryProperty : public CBase
	{
public:	
    enum TIntervalType
    	{
		ESecond = 0,
		EMinute,
		EHour,
		EDay,
		EMonth,
		EYear,
    	};
    	
    enum TCategoryType
    	{
    	ESingleCategory,
    	EMultipleCategory,
    	};
    	
    enum TEntryRepeatType
        {
        EEntryEnds,
        EEntryRepeatsForever,
        EEntrySingleInstance,
        };
    	
public:
	static CEntryProperty* NewLC();
	~CEntryProperty();
	TBool ContainsEntries() const;
	void Update( const CEntryProperty& aPropertyUpdate );

private:
	CEntryProperty();		
	
public:
	//number of standard entries
    TInt 										iStandardEntryCount; 
    //number of repeating entries		
    TInt 										iRepeatingEntryCount; 
    //number of days from today, when entry starts +/-		
    TInt 										iStartDaysFromToday; 
    //number of days from today, when entry end +/-				
    TInt 										iEndDaysFromToday; 	
    //number of time units to in/decreas date of entries		
    TInt 										iEntryDateIncrement; 
    //number of standard categories to add to entry		
    TInt 										iStandardcategory; 
    //number of custom/extended categories to add to entry			
    TInt 										iCustomcategory; 
	//list of custom categories, retrieve from config file			
    TPtrC 										iCustomCategoryString;
    //add a single or multiple categories to each entry
    TCategoryType 								iCategoryType;
    //time unit to increase date of entry iwth
    TIntervalType								iIntervalType;
    //entry repeats indefinitely or not or single instance
    TEntryRepeatType 							iEntryType;
	//list of custom categories, retrieve from config file
	RArray< TPtrC > 							iCustomCategoriesList;
	//earliest start time of an entry generated from config file
	TCalTime                					iStartDate;
	//latest end time of an entry generated from config file
	TCalTime                					iEndDate;
	//seconds offset from entry start time
	TInt 										iAlarmOffset;
	// delete the nth isntance
	TInt										iExceptionNumber;
	//add iChildCount children
	TInt										iChildCount;
	//set to floating time
	TBool										iFloating;
	};
	
class TEntryParser
	{
public:
	TEntryParser( CTestCalInterimApiSuiteStepBase& aTestStep );
	CEntryProperty* ParseEntryL( const TDesC& aSection );
	
private:
	CTestCalInterimApiSuiteStepBase& iTestStep;
	};
	
class CEntryBuilder  : public CBase
	{
public:
	static CEntryBuilder* NewL( 
		CTestCalInterimApiSuiteStepBase& aTestStep, 
		CEntryProperty& aEntry,
		TTime& 	aTestTime,
		TTime& 	aPresentTime
		);
	~CEntryBuilder();
	
	void 			CreateEmptyEntriesL(
         			   const CCalEntry::TMethod aEntryMethodType,
         			   const TInt aNumberOfEntries
         			   );
    void 			SetEntriesStartEndDatesL(
         			   const TInt aStartDaysFromToday,
         			   const TInt aEndDaysFromToday,
         			   const TInt aEntryDateIncrement,
        			   const TInt aFirstEntry
         			   );	   
    void 			SetEntriesRRulesL(  
         			   const CEntryProperty::TEntryRepeatType aRepeatType, 
         			   const TInt aFirstEntry, 
         			   const TInt aNumberOfEntries
         			   );
	void 			AddDefaultCategoriesL(
	     			   const TInt aFirstEntry, 
	     			   const TInt aNumberOfEntries,
	     			   const CEntryProperty::TCategoryType aCategoryType
	     			   );
         			
	void 			AddCustomCategoriesL(
	     			   const TInt aFirstEntry, 
	     			   const TInt aNumberOfEntries,
	     			   const CEntryProperty::TCategoryType aCategoryType
	     			   );
	void 			AddAlarmsToEntriesL( const TInt aMinutesFromNow );
	RPointerArray< CCalEntry >& GetEntryList();
	CEntryProperty& GetEntryProperty();
	TInt 			Count() const;
	void 			SetSingleUpdate( const CCalEntry& aUpdateEntry );
	void 			SetMultipleUpdate();
	void 			SetEntryLocalTime( 
						const CCalEntry& aEntry,  
						const CEntryProperty& aProperty
						);
						         			   
private:
					CEntryBuilder( 
						CTestCalInterimApiSuiteStepBase& aTestStep, 
						CEntryProperty& aEntry,
						TTime& 	aTestTime,
						TTime& 	aPresentTime
						);
	TTime 			ApplyTimeInterval( 
						const TInt aInterval,
						const TTime aInitialTime 
						);	
	TBool 			IsUpdatableL( const CCalEntry& aUpdateEntry );
	void 			SetLocalTimeL( 
						TCalTime& aCalTime, 
						const TTime aTTime
						);							
		
private:
	CTestCalInterimApiSuiteStepBase& 	iTestStep;
	CEntryProperty& 				 	iEntry;
	//this is the actual current time, reset to this value after test is complete
	TTime& 								iPresentTime;
	//set time to this value when performing test case
	TTime& 								iTestTime;
	//number of entries created by builder
    TInt                          	    iEntryCount;
	RPointerArray< CCalEntry >  		iEntryList;
	CCalEntry* 							iSingleEntry;
	CCalendarTestUtility*   			iUtility;
	};
	

#endif//CALENDAR_TEST_ENTRY_PARSER__INCLUDED_