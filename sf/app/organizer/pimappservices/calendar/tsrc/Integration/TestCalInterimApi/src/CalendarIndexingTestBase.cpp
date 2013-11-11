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
 
#include <calrrule.h>
#include <calinstance.h>
#include <calcategory.h>
#include "CalendarIndexingTestBase.h"
#include <calinstanceiterator.h>

CCalendarIndexingTestBase::CCalendarIndexingTestBase() 
:   CTestCalInterimApiSuiteStepBase(),
    iPresentTime( Time::NullTTime() ),
    iTestTime( Time::NullTTime() ),
    iParser(*this)
    {
    iHeapCheck = ETrue;//check heap(__UHEAP_MARKEND) in the CTestCalInterimApiSuiteStepBase destructor
    }

CCalendarIndexingTestBase::~CCalendarIndexingTestBase()
    {
    CloseViews();
	iFilteredEntries.ResetAndDestroy();
	iCategoryEntries.Reset();
	delete iCategoryManager;
    delete iUtility;
    delete iRealTimeZone;
	delete iInitialTimeZone;
	delete iNewTimeZone;
    delete iEntryBuilder;
    delete iInitialEntryProperties; 
    delete iTestEntryProperties;
    }

void CCalendarIndexingTestBase::OpenViewsL()
    {
	iInstanceView = CCalInstanceView::NewL( GetSession(), *this );
	CActiveScheduler::Start();
	iEntryView = CCalEntryView::NewL( GetSession(), *this );
	CActiveScheduler::Start();
    }

void CCalendarIndexingTestBase::CloseViews()
    {
    DELETE( iInstanceView );
    DELETE( iEntryView );
    }

//creates array of entries bases on entry properties
void CCalendarIndexingTestBase::AddEntriesFromConfigL( CEntryBuilder& aEntryBuilder )
    {
    CEntryProperty& entry = aEntryBuilder.GetEntryProperty();
    
    //generate entries if config section describing entry content exists
    if( entry.ContainsEntries() )
        {
        //create entries and populate
        aEntryBuilder.CreateEmptyEntriesL(CCalEntry::EMethodAdd, entry.iRepeatingEntryCount);
        aEntryBuilder.CreateEmptyEntriesL(CCalEntry::EMethodAdd, entry.iStandardEntryCount);
        UpdateEntriesFromConfigL( aEntryBuilder, entry );
        //create entries and populate
        aEntryBuilder.SetEntriesStartEndDatesL( entry.iStartDaysFromToday, entry.iEndDaysFromToday, entry.iEntryDateIncrement, 0 );
        aEntryBuilder.SetEntriesStartEndDatesL( entry.iStartDaysFromToday, entry.iEndDaysFromToday, entry.iEntryDateIncrement, entry.iRepeatingEntryCount );
        
        
        aEntryBuilder.SetEntriesRRulesL( entry.iEntryType , 0, entry.iRepeatingEntryCount );
        }
    }

//updates array of entries bases on update entry properties
void CCalendarIndexingTestBase::UpdateEntriesFromConfigL( 
		CEntryBuilder& aEntryBuilder,
		const CEntryProperty& aEntryUpdate )
    {
    CEntryProperty& entry = aEntryBuilder.GetEntryProperty();
    //update initial entry property with update preoperty
    if( &entry != &aEntryUpdate)
    	{
    	entry.Update( aEntryUpdate );
    	}
    
    //generate entries if config section describing entry content exists
    if( entry.ContainsEntries() )
        {
        if( (entry.iStandardcategory > 0 || entry.iCustomcategory > 0) )
        	{
	        if(  entry.iRepeatingEntryCount > 0 )
	        	{
	        	iUtility->COMPARE( entry.iStandardcategory + entry.iCustomcategory, ==, entry.iRepeatingEntryCount );
	        	//add categories for repeating entries
		        aEntryBuilder.AddDefaultCategoriesL( 0, entry.iStandardcategory, entry.iCategoryType);
		        aEntryBuilder.AddCustomCategoriesL( entry.iStandardcategory, entry.iCustomcategory, entry.iCategoryType);
	        	}
	        if( entry.iStandardEntryCount > 0 )
	        	{
	        	iUtility->COMPARE( entry.iStandardcategory + entry.iCustomcategory, ==, entry.iStandardEntryCount );
		        //add categories for non repeating entries
		        aEntryBuilder.AddDefaultCategoriesL( entry.iRepeatingEntryCount, entry.iStandardcategory, entry.iCategoryType);
		        aEntryBuilder.AddCustomCategoriesL( entry.iRepeatingEntryCount+entry.iStandardcategory, entry.iCustomcategory, entry.iCategoryType);
	        	}
        	}
        }
    }

//update single entry within entry builder
void CCalendarIndexingTestBase::UpdateEntryL(
		CEntryBuilder& aEntryBuilder, 
		const CEntryProperty& aEntryPropertyUpdate, 
		const CCalEntry& aUpdateEntry )
	{
	//set single entry to be updated
	aEntryBuilder.SetSingleUpdate( aUpdateEntry );
	//update entry
	UpdateEntriesFromConfigL(aEntryBuilder, aEntryPropertyUpdate);
	//reset multiple updates
	aEntryBuilder.SetMultipleUpdate();
	
	//update entry within calendar server
    RPointerArray< CCalEntry > entryList;
    CleanupClosePushL( entryList );
    entryList.Append( &aUpdateEntry );
    TInt numOfEntries(0);
    
    iEntryView->UpdateL( entryList, numOfEntries );
    iUtility->COMPARE( numOfEntries, ==, 1 );
    
    CleanupStack::PopAndDestroy( &entryList );
   
   	}

//retrieve test data from config file    
void CCalendarIndexingTestBase::ParseConfigSection(const TDesC& aSection)
    {
    _LIT( KTimezone, "Timezone" );
    _LIT( KNewTimeZone, "NewTimeZone" );
    _LIT( KPresentTime, "PRESENTTIME" );
    _LIT( KSetupEntriesProfile, "SetupEntriesProfile" );
    _LIT( KTestEntriesProfile, "TestEntriesProfile" );
    _LIT( KEntryaction, "entryaction" );
    
    
    TInt entryAction = -1;

    iInitialTimeZone = iUtility->GetTZFromConfigL( aSection, KTimezone);
    iNewTimeZone = iUtility->GetTZFromConfigL(     aSection, KNewTimeZone);
    iUtility->GetTimeFromConfig(                   aSection, KPresentTime,         iTestTime);
    GetStringFromConfig(                           aSection, KSetupEntriesProfile, iInitialEntries);
    GetStringFromConfig(                           aSection, KTestEntriesProfile,  iTestCaseEntries);
    GetIntFromConfig(                              aSection, KEntryaction,         entryAction );
    iOperationType = static_cast< TOperationType >( entryAction );
    }

//set up environment using test data
void CCalendarIndexingTestBase::ProcessConfigDataL()
    {   	    
   
   	//set home time to consistant value
   	iPresentTime.HomeTime();
	if( iTestTime != Time::NullTTime() )
	    {
    	User::LeaveIfError( iTzServer.SetHomeTime(iTestTime) );
	    }
	//set tz to consistent value
    if( iInitialTimeZone )
        {
    	iRealTimeZone = iTzServer.GetTimeZoneIdL();
    	iTzServer.SetTimeZoneL(*iInitialTimeZone);
        }
    
    //process entry data from config section
    iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries ); 
    iTestEntryProperties = iParser.ParseEntryL( iTestCaseEntries );
    //build entries based on entry properties
    GetEntryBuilderL();
    
    //if initial entry properties defined, store in calendar server
    if( iInitialEntryProperties && iInitialEntryProperties->ContainsEntries() )
    	{
	    //store entries in db here:
	    TInt numberOfEntries = 0;
	if( iTestTime != Time::NullTTime() )
	    {   
        User::LeaveIfError( iTzServer.SetHomeTime(iPresentTime) );
	    }
	    iEntryView->StoreL( iEntryBuilder->GetEntryList(), numberOfEntries);
	    //check that all generated entries have been stored
	    iUtility->COMPARE( numberOfEntries, ==, iEntryBuilder->GetEntryList().Count() );
    	iStoredEntryCount += numberOfEntries;
    	}
    }

TVerdict CCalendarIndexingTestBase::doTestStepPreambleL()
    {
    //mark end in the CTestCalInterimApiSuiteStepBase destructor
   	__UHEAP_MARK;
    iUtility = CCalendarTestUtility::NewLC( *this );
    CleanupStack::Pop( iUtility );
    //close server before CTestCalInterimApiSuiteStepBase preamble opens calendar session
    iUtility->FindAndCloseCalendarServer( CCalendarTestUtility::EForceCloseServer, ETrue );
    
    CTestCalInterimApiSuiteStepBase::doTestStepPreambleL();
    OpenViewsL();
    User::LeaveIfError( iTzServer.Connect() );
    iCategoryManager = CCalCategoryManager::NewL( GetSession() );
	return TestStepResult();
    }

TVerdict CCalendarIndexingTestBase::doTestStepL()
    {
    ParseConfigSection( ConfigSection());
    ProcessConfigDataL();
    PerformTestCaseL();
    return TestStepResult();
    }

//reset environement to default values
TVerdict CCalendarIndexingTestBase::doTestStepPostambleL()
    {
    if( iRealTimeZone )
        {
    	iTzServer.SetTimeZoneL(*iRealTimeZone);
        }
	if( iTestTime != Time::NullTTime() )
	    {   
        User::LeaveIfError( iTzServer.SetHomeTime(iPresentTime) );
	    }
    iTzServer.Close();
    return CTestCalInterimApiSuiteStepBase::doTestStepPostambleL();
    }

//get initial entry properties if it exists, otherwise get test entry properties    
CEntryProperty& CCalendarIndexingTestBase::GetEntryProperty()
	{
	if( iInitialEntryProperties && iInitialEntryProperties->ContainsEntries())
		{
		return *iInitialEntryProperties;
		}
	else
		{
		return *iTestEntryProperties;
		}
	}

//get entry builder, lazy initilization	
CEntryBuilder& CCalendarIndexingTestBase::GetEntryBuilderL()
	{
	if( !iEntryBuilder )
		{
	    iEntryBuilder = CEntryBuilder::NewL( *this, GetEntryProperty(), iTestTime, iPresentTime);
	    AddEntriesFromConfigL( *iEntryBuilder );
		}
	return *iEntryBuilder;
	}

//unload entry builder from memory	
void CCalendarIndexingTestBase::UnloadEntryBuilder()
	{
	DELETE( iEntryBuilder );
	}

//execute test operation, store, delete... 
//configured via operation type retrieved from config section	
void CCalendarIndexingTestBase::ExecuteTestCaseOperationL(
        const CCalEntry& aEntry, 
        const TCalTime& aStartDate, 
        const TCalTime& aEndDate
        )
    {
	CalCommon::TCalTimeRange timeRange(aStartDate, aEndDate);
    switch( iOperationType )
        {
		case EStore:
		    {
            //store entry in db
            StoreCalendarEntryL(iEntryView, const_cast<CCalEntry*>( &aEntry ) );
            ++iStoredEntryCount;
   		    break;
		    }
		case EDeleteEntry:
		    {
		    iUtility->DeleteEntryL( *iEntryView, aEntry );
		    --iStoredEntryCount;
		    break;
		    }
		case EDeleteInstance:
		    {
		    DeleteInstanceL( aEntry, iTestEntryProperties->iExceptionNumber, timeRange );
		    break;
		    }
		case EUpdate:
		    {
		    UpdateEntryL( GetEntryBuilderL(), *iTestEntryProperties, aEntry );
		    break;
		    }
		case EAddChild:
		    {
		    AddChildL( aEntry, iTestEntryProperties->iChildCount, timeRange );
		    break;
		    }
		case ESetTimeType:
		    {
		    GetEntryBuilderL().SetEntryLocalTime( aEntry, *iTestEntryProperties );
		    break;
		    }
		default:
		    {
		    break;
		    }
        }
    }

//check that operation was performed successful by ExecuteTestCaseOperationL
//configured via operation type retrieved from config section	
TBool CCalendarIndexingTestBase::ValidateViewsL(
        const CCalEntry& aEntry, 
        const TCalTime aStartDate, 
        const TCalTime aEndDate
        )
    {
    TBool validComparison = EFalse;
	CalCommon::TCalTimeRange timeRange(aStartDate, aEndDate);
	
    switch( iOperationType )
        {
		case EStore:
		    {
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EDeleteEntry:
		    {
		    validComparison = CompareDeletedEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EDeleteInstance:
		    {
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EUpdate:
		    {
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    validComparison = VerifyCategoriesL() && validComparison;
		    break;
		    }
		case EAddChild:
		    {
		    validComparison = ValidateChildren( aEntry, iTestEntryProperties->iChildCount );
		    break;
		    }
		case ESetTimeType:
		    {
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		default:
		    {
		    break;
		    }
        }
    return validComparison;
    }


/**
closes views, calendar session and resets entry arrays, 
waits for clanedar server to close
then reopens session, views and reloads entry arrays
*/
void CCalendarIndexingTestBase::CloseWaitAndReopenSessionL()
    {
    CloseSessions();
    iUtility->FindAndCloseCalendarServer( CCalendarTestUtility::EWaitForServerToClose, EFalse );
    OpenSessionsL();
    }

//closes all open sessions to the calendar server
void CCalendarIndexingTestBase::CloseSessions()
	{
	DELETE( iCategoryManager );
    CloseViews();
	UnloadEntryBuilder();
    DELETE( iSession );
	}

//re-opens sessions/views to calendar server 
void CCalendarIndexingTestBase::OpenSessionsL()
	{
	iSession = CCalSession::NewL();
	iSession->OpenL(iCalenderFileName);
	iCategoryManager = CCalCategoryManager::NewL( *iSession );
	OpenViewsL();
	GetEntryBuilderL();
	}

//closes all sessions to calendar, deletes calendar file and reopens session	
void CCalendarIndexingTestBase::DeleteCalendarFile()
	{
    CloseSessions();
	iSession = CCalSession::NewL();
	
    //replaces calendar with empty/new calendar
    CreateAndOpenCalenderFileL( iCalenderFileName, ETrue );
    
    DELETE( iSession );
	OpenSessionsL();
	}

//check that deleted entry has no instances associated with  it,
//check that deleted entry is not retrieved from entry view	
TBool CCalendarIndexingTestBase::CompareDeletedEntryToViewsL(
        const CCalEntry& aEntry,
        const CalCommon::TCalTimeRange& aTimeRange )
    {
    //check that deleted entry has no instances associated with  it,
    TBool viewsMatchesEntry = ETrue;
    RPointerArray< CCalInstance  > viewInstances;
    CleanupResetDestroyPushL( viewInstances );
    iInstanceView->FindInstanceL( viewInstances, CalCommon::EIncludeAll, aTimeRange );
    
    const TInt KInstanceCount = viewInstances.Count();
    
    CCalInstance* instance = NULL;
    for(TInt i = 0; i < KInstanceCount; ++i )
        {
        instance = viewInstances[ i ];
        //deleted entry should not have any matching instances, fail if match
        if( instance->Entry().UidL() == aEntry.UidL() )
            {
            iUtility->COMPAREI1( instance->Entry().UidL(), != , aEntry.UidL(), i );
            viewsMatchesEntry = EFalse;
            break;
            }
        instance = NULL;
        }
    
    //deleted entry should not generates any instances
    viewsMatchesEntry = iUtility->COMPARE( KInstanceCount, ==, iStoredEntryCount )  && viewsMatchesEntry;
    CleanupStack::PopAndDestroy();//viewInstances
    
    RPointerArray< CCalEntry > calEntryArray;
    CleanupResetDestroyPushL( calEntryArray );
    iEntryView->FetchL( aEntry.UidL(), calEntryArray );
    //deleted entry should not be retrieved by entry view
    viewsMatchesEntry = iUtility->TESTPRINT( calEntryArray.Count() == 0 ) && viewsMatchesEntry;
    CleanupStack::PopAndDestroy( &calEntryArray );
    
    return viewsMatchesEntry;
    }

//deletes the nth instance matching aInstanceNumber and entry from instance view	
void CCalendarIndexingTestBase::DeleteInstanceL(
        const CCalEntry& aEntry,
        const TInt aInstanceNumber,
        const CalCommon::TCalTimeRange& aTimeRange )
    {
    RPointerArray< CCalInstance  > viewInstances;
    CleanupResetDestroyPushL( viewInstances );
    iInstanceView->FindInstanceL( viewInstances, CalCommon::EIncludeAll, aTimeRange );
    
    const TInt KInstanceCount = viewInstances.Count();
    
    TInt matchingInstances = 0;
    CCalInstance* instance = NULL;
    for(TInt i = viewInstances.Count() - 1; i >= 0 ; --i )
        {
        instance = viewInstances[ i ];
        if( instance->Entry().UidL() == aEntry.UidL() )
            {
            ++matchingInstances;
            //deletes aInstanceNumber instance matching entry
            if( matchingInstances  == aInstanceNumber )
            	{
            	iInstanceView->DeleteL( instance, CalCommon::EThisOnly );
            	viewInstances.Remove( i );
            	}
            }
        instance = NULL;
        }
        
    DELETE(instance);
    CleanupStack::PopAndDestroy( &viewInstances );
    }

//adds 	aChildCount many child entries to entry
void CCalendarIndexingTestBase::AddChildL(
        const CCalEntry& aEntry,
        const TInt aChildCount,
        const CalCommon::TCalTimeRange& aTimeRange )
    {
    RPointerArray< CCalInstance  > viewInstances;
    CleanupResetDestroyPushL( viewInstances );
    iInstanceView->FindInstanceL( viewInstances, CalCommon::EIncludeAll, aTimeRange );
    
    const TInt KInstanceCount = viewInstances.Count();
    
    TInt addedChildren = 0;
    CCalInstance* instance = NULL;
    CCalEntry* child = NULL;
    
    for(TInt i = 0; i < viewInstances.Count() && addedChildren < aChildCount; ++i )
        {
        instance = viewInstances[ i ];
        if( instance->Entry().UidL() == aEntry.UidL() )
            {
		    HBufC8* guid = instance->Entry().UidL().AllocLC();
			child = CCalEntry::NewL(instance->Entry().EntryTypeL(), guid, CCalEntry::EMethodNone, 
									addedChildren + 1, instance->StartTimeL(), CalCommon::EThisOnly);
			CleanupStack::Pop(guid);
			CleanupStack::PushL( child );
			child->SetStartAndEndTimeL( instance->StartTimeL(), instance->EndTimeL() );
			
			RPointerArray< CCalEntry > calEntryArray;
	   		CleanupResetDestroyPushL( calEntryArray );
	    
	    	HBufC* uID = HBufC::NewLC(instance->Entry().UidL().Length());
  			uID->Des().Copy(instance->Entry().UidL());
  			INFO_PRINTF2(KInfoGUIDEntry, uID);
  			CleanupStack::PopAndDestroy( uID );
	    
	    	iEntryView->FetchL( instance->Entry().UidL(), calEntryArray );
			
			INFO_PRINTF2(_L("count of array %d"),calEntryArray.Count());
			 
			CleanupStack::PopAndDestroy(&calEntryArray);
			
			StoreCalendarEntryL(iEntryView, child );
			CleanupStack::PopAndDestroy();
			++addedChildren;
			child = NULL;
            }
             
        instance = NULL;
        }
        
    DELETE(instance);
    DELETE(child);
    CleanupStack::PopAndDestroy( &viewInstances );
        
    }

//check that entry has aChildCount many children
TBool CCalendarIndexingTestBase::ValidateChildren(
        const CCalEntry& aEntry,
        const TInt aChildCount
        )
	{
    RPointerArray< CCalEntry > calEntryArray;
    CleanupResetDestroyPushL( calEntryArray );
    iEntryView->FetchL( aEntry.UidL(), calEntryArray );
    //deleted entry should not be retrieved by entry view
    TBool viewsMatchesChildren = iUtility->COMPARE( calEntryArray.Count(), ==, aChildCount + 1);
    CleanupStack::PopAndDestroy( &calEntryArray );
    return viewsMatchesChildren;
    
	}

//compare a list of entries to instance view and entry views
//all instances should match an entry in the list
//all entries should contain a matching entry in entry view
TBool CCalendarIndexingTestBase::CompareEntriesToViewsL(
        const RPointerArray< CCalEntry >& aEntryList,
        const CalCommon::TCalTimeRange& aTimeRange )
    {
	const TInt KEntryCount = aEntryList.Count();
    TBool viewsMatchEntries = ETrue;
  	for( TInt i = 0; i < KEntryCount; ++i )
		{
    	//entries should match all instances
		viewsMatchEntries = iUtility->TESTPRINTI1( CompareEntryToInstances( *(aEntryList[i]), aTimeRange ), i) && viewsMatchEntries; 
		}
		
    const CCalEntry* entry = NULL;
    //assume no child entries
	for( TInt i = 0; i < KEntryCount; ++i )
		{
		entry = aEntryList[i];
				
	    RPointerArray< CCalEntry > calEntryArray;
	    CleanupResetDestroyPushL( calEntryArray );
	    
	    HBufC* uID = HBufC::NewLC(entry->UidL().Length());
  		uID->Des().Copy(entry->UidL());
  		INFO_PRINTF2(KInfoGUIDEntry, uID);
  		CleanupStack::PopAndDestroy( uID );
	    
	    iEntryView->FetchL( entry->UidL(), calEntryArray );
	    
	    INFO_PRINTF2(_L("count of array %d"),calEntryArray.Count());
	    
	    viewsMatchEntries = iUtility->TESTPRINT( calEntryArray.Count() == 1 ) && viewsMatchEntries;
	    if( calEntryArray.Count() > 0 )
	    	{
	    	const TBool KMatchingEntries = iUtility->CompareEntriesL( *(calEntryArray[0]), *entry );
	    	viewsMatchEntries = iUtility->TESTPRINTI1( KMatchingEntries, i )  && viewsMatchEntries;
	    	}
	    	
	    CleanupStack::PopAndDestroy( &calEntryArray );
	    entry = NULL;
		}
		
	DELETE( entry );
    return viewsMatchEntries;
    }

//compares single entry to all views.
//check that views contain matching instances/entries
TBool CCalendarIndexingTestBase::CompareEntryToViewsL(
        const CCalEntry& aEntry,
        const CalCommon::TCalTimeRange& aTimeRange )
    {
    TBool viewsMatchEntry = EFalse;
    RPointerArray< CCalEntry > entryList;
    CleanupClosePushL( entryList );
    entryList.Append( &aEntry );
    viewsMatchEntry = CompareEntriesToViewsL( entryList, aTimeRange );
    CleanupStack::PopAndDestroy( &entryList );
    return viewsMatchEntry;
    }

//removes instance from array if it matches entry
TBool CCalendarIndexingTestBase::CompareEntryToInstances(
        const CCalEntry& aEntry,
        const CalCommon::TCalTimeRange& aTimeRange
        )
    {
    
    TBool entryMatchesInstances = EFalse;
    
    //if entry not within timerange, ignore
    if( !iUtility->TESTPRINT( iUtility->EntryWithinTimeRange( aEntry, aTimeRange) ) )
    	{
    	PRINTFAIL( "Entry start date", ETrue );
		PrintTimeL(aEntry.StartTimeL().TimeLocalL());
    	PRINTFAIL( "Range start date", ETrue );
		PrintTimeL(aTimeRange.StartTime().TimeLocalL());
    	PRINTFAIL( "Entry end date", ETrue );
		PrintTimeL(aEntry.EndTimeL().TimeLocalL());
    	PRINTFAIL( "Range end date", ETrue );
		PrintTimeL(aTimeRange.EndTime().TimeLocalL());
    	return EFalse;
    	}
    	
    TCalRRule entryRRule;
    const TBool repeatingEntry = aEntry.GetRRuleL( entryRRule );

    //iterate through instances, if instance matches entry, remove instance from list and increment number of matching instances
    
    TInt matchingInstances = 0;
   	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, aTimeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	CCalInstanceIterator* iterator = iInstanceView->FindInstanceL(*findInstanceSettings);
	CleanupStack::PopAndDestroy(findInstanceSettings);
	CleanupStack::PushL(iterator);
	
	TInt i= 0;
	while (CCalInstance* instance = iterator->NextL())
		{
		CleanupStack::PushL(instance);
	    if( instance->Entry().UidL() == aEntry.UidL() )
	         {
	         if( iUtility->TESTPRINTI1( CompareEntryToInstanceL( *instance, aEntry ), i ) )
	            {
		        ++matchingInstances;
	            }
	         else
	           {
	            return EFalse;
	            }
	         }
	     else
	         {
	       	// do nothing
	         }
	    CleanupStack::PopAndDestroy(instance);
	    ++i;
		}
	
	CleanupStack::PopAndDestroy(iterator);
    
 //if repeating entry, expect many instances
    if( repeatingEntry )
    	{
    	TInt numberOfRepeats = KErrNotFound;
    	const TTime KRepeatStartTime = entryRRule.DtStart().TimeLocalL();
    	const TTime KRepeatEndTime = entryRRule.Until().TimeLocalL();
	    switch( entryRRule.Type() )
	    	{
	    	case TCalRRule::EDaily:
	    		{
	    		numberOfRepeats = KRepeatStartTime.DaysFrom( KRepeatEndTime ).Int();
				break;
	    		}
			case TCalRRule::EWeekly:
	    		{
	    		numberOfRepeats = KRepeatStartTime.DaysFrom( KRepeatEndTime ).Int() / 7;
				break;
	    		}
			case TCalRRule::EMonthly:
	    		{
	    		numberOfRepeats = KRepeatStartTime.MonthsFrom( KRepeatEndTime ).Int();
				break;
	    		}
			case TCalRRule::EYearly:
	    		{
	    		numberOfRepeats = KRepeatStartTime.YearsFrom( KRepeatEndTime ).Int();
				break;
	    		}
	    	default:
	    		{
	    		//unknown repeat type, fail test
	    		TEST1( EFalse, ETrue );
	    		return EFalse;
	    		}
	    	}
	    
	    //number of instances plus start instance
	    numberOfRepeats = Abs(numberOfRepeats) + 1;
	    //if exception has been added, expect one less instance
	    if( iTestEntryProperties && iTestEntryProperties->iExceptionNumber > 0 )
	    	{
	    	--numberOfRepeats;
	    	}
		//expect many instances for repeating entry
		entryMatchesInstances = iUtility->COMPARE( numberOfRepeats , ==, matchingInstances );
    	}
    //if non repeating entry
	else
		{
		//expect only one instance for non-repeating entry
		entryMatchesInstances = iUtility->COMPARE( matchingInstances, ==, 1 );
		}
	
	return entryMatchesInstances;
    
    }

//compares entry to matching instances,
//checks that start end time are valid as determined by type and rpt rule
TBool CCalendarIndexingTestBase::CompareEntryToInstanceL( 
		const CCalInstance& aInstance, 
		const CCalEntry& aEntry
		)
	{
	TBool entryMatchesInstance = ETrue;
	const TTime KEntryStartTime = aEntry.StartTimeL().TimeLocalL();
	const TTime KEntryEndTime = aEntry.EndTimeL().TimeLocalL();
	const TTime KInstanceStartTime = aInstance.StartTimeL().TimeLocalL();
	const TTime KInstanceEndTime = aInstance.EndTimeL().TimeLocalL();
	
	const TDateTime KEntryStartDTime = KEntryStartTime.DateTime();
	const TDateTime KInstanceStartDTime = KInstanceStartTime.DateTime();
	
	const TBool KNullEntryEndTime = KEntryEndTime == Time::NullTTime();
	const TBool KNullInstanceEndTime = KInstanceEndTime == Time::NullTTime();
	
    TCalRRule entryRRule;
    const TBool repeatingEntry = aEntry.GetRRuleL( entryRRule );
    
    if( repeatingEntry )
    	{
	    switch( entryRRule.Type() )
	    	{
	    	case TCalRRule::EDaily:
	    		{
	    		entryMatchesInstance = CompareRepeatEntryToInstanceTimes( aInstance, aEntry ) && entryMatchesInstance;
			    break;
	    		}
			case TCalRRule::EWeekly:
	    		{
	    		//dont check end time, as that can change to match rpt rule
			    entryMatchesInstance = iUtility->COMPARE( KEntryStartTime.DayNoInWeek() , ==,  KInstanceStartTime.DayNoInWeek() ) && entryMatchesInstance;
	    		entryMatchesInstance = CompareRepeatEntryToInstanceTimes( aInstance, aEntry ) && entryMatchesInstance;
			    break;
	    		}
			case TCalRRule::EMonthly:
	    		{
	    		//dont check end time, as that can change to match rpt rule
			    entryMatchesInstance = iUtility->COMPARE( KEntryStartTime.DayNoInMonth() , ==,  KInstanceStartTime.DayNoInMonth() ) && entryMatchesInstance;    		
	    		entryMatchesInstance = CompareRepeatEntryToInstanceTimes( aInstance, aEntry ) && entryMatchesInstance;
			    break;
	    		}
			case TCalRRule::EYearly:
	    		{
	    		//dont check end times, as that can change to match rpt rule
			    entryMatchesInstance = iUtility->COMPARE( KEntryStartDTime.Month() , ==,  KInstanceStartDTime.Month() ) && entryMatchesInstance;
			    entryMatchesInstance = iUtility->COMPARE( KEntryStartTime.DayNoInMonth() , ==,  KInstanceStartTime.DayNoInMonth() ) && entryMatchesInstance;
	    		entryMatchesInstance = CompareRepeatEntryToInstanceTimes( aInstance, aEntry ) && entryMatchesInstance;
			    break;
	    		}
	    	default:
	    		{
	    		//unknown repeat type, fail test
	    		_LIT(KRTypeError, "Unknown repeat rule type %d");
	    		ERR_PRINTF2(KRTypeError, entryRRule.Type() );
	    		TEST1( EFalse, ETrue );
	    		return EFalse;
	    		}
	    	}
    	}
	else
		{
	    entryMatchesInstance = iUtility->TESTPRINT( KEntryStartTime == KInstanceStartTime ) && entryMatchesInstance;
	    entryMatchesInstance = iUtility->TESTPRINT( KEntryEndTime == KInstanceEndTime || KNullEntryEndTime || KNullInstanceEndTime ) && entryMatchesInstance;
	    if( !entryMatchesInstance )
	    	{
	    	PRINTFAIL( "Entry start date", ETrue );
			PrintTimeL(KEntryStartTime);
	    	PRINTFAIL( "Instance start date", ETrue );
			PrintTimeL(KInstanceStartTime);
	    	PRINTFAIL( "Entry end date", ETrue );
			PrintTimeL(KEntryEndTime);
	    	PRINTFAIL( "Instance end date", ETrue );
			PrintTimeL(KInstanceEndTime);
	    	}
		}
	return entryMatchesInstance;
	}

//check that the start and end times for instance and entry	are equal
TBool CCalendarIndexingTestBase::CompareRepeatEntryToInstanceTimes(
		const CCalInstance& aInstance, 
		const CCalEntry& aEntry
		)
	{
	TBool entryMatchesInstance = ETrue;
	
	const TDateTime entryStartDTime = aEntry.StartTimeL().TimeLocalL().DateTime();
	const TDateTime entryEndDTime = aEntry.EndTimeL().TimeLocalL().DateTime();
	const TDateTime instanceStartDTime = aInstance.StartTimeL().TimeLocalL().DateTime();
	const TDateTime instanceEndDTime = aInstance.EndTimeL().TimeLocalL().DateTime();
	
    entryMatchesInstance = iUtility->COMPARE( entryStartDTime.Hour() , ==,  instanceStartDTime.Hour() ) && entryMatchesInstance;
    entryMatchesInstance = iUtility->COMPARE( entryStartDTime.Minute() , ==,  instanceStartDTime.Minute() ) && entryMatchesInstance;
    entryMatchesInstance = iUtility->COMPARE( entryStartDTime.Second() , ==,  instanceStartDTime.Second() ) && entryMatchesInstance;
    
    //if end time for instance or entry is null, dont compare
    if( ( aEntry.EndTimeL().TimeLocalL() != Time::NullTTime() ) && ( aInstance.EndTimeL().TimeLocalL() != Time::NullTTime() ) )
    	{
	    entryMatchesInstance = iUtility->COMPARE( entryEndDTime.Hour() , ==,  instanceEndDTime.Hour() ) && entryMatchesInstance;
	    entryMatchesInstance = iUtility->COMPARE( entryEndDTime.Minute() , ==,  instanceEndDTime.Minute() ) && entryMatchesInstance;
	    entryMatchesInstance = iUtility->COMPARE( entryEndDTime.Second() , ==,  instanceEndDTime.Second() ) && entryMatchesInstance;
    	}
    
    return entryMatchesInstance;
	}

//filter entries using category manager, check that retrieved entries match expected entries    
TBool CCalendarIndexingTestBase::VerifyCategoriesL()
	{
	TBool categoriesEqual = ETrue;
    //ignore ECalExtended, as it appears to be invalid, cause leave error -6, not documented
    const TInt KNumberOfDefaultCategories = CCalCategory::ECalVacation + 1;
    const TInt KNumberOfCustomCategories = GetEntryProperty().iCustomCategoriesList.Count();
    const TInt KNumberOfCategories = KNumberOfDefaultCategories + KNumberOfCustomCategories;
    //repeat for all categories used by test
    for(TInt i = 0; i < KNumberOfCategories ; ++i)
        {
        CCalCategory* category = NULL;
        if( i < KNumberOfDefaultCategories )
        	{
	        //cycle through all possible entry types
	        const CCalCategory::TCalCategoryType KCategoryType = 
	        			static_cast < CCalCategory::TCalCategoryType >( i % KNumberOfDefaultCategories );
			category = CCalCategory::NewL( KCategoryType );
        	}
        else
        	{
	    	const TInt KCategoryIndex = i - KNumberOfDefaultCategories;
			category = CCalCategory::NewL( GetEntryProperty().iCustomCategoriesList[ KCategoryIndex ] );
        	}
		CleanupStack::PushL( category );
		//leaves with -1 if custom category is not found??	
		//filter entries using category manager	        			
		TRAPD( error,
			iCategoryManager->FilterCategoryL( *category, iFilteredEntries, *this );
			);
		if( !error )
			{
			CActiveScheduler::Start();
			}
		//generate list of entries matching the category
		GetMatchingEntriesL( *category );
		//check that retrieved entries match expected entries
		categoriesEqual = iUtility->TESTPRINTI1( iUtility->CompareEntryListsL( iFilteredEntries, iCategoryEntries ), i ) && categoriesEqual;
		iFilteredEntries.ResetAndDestroy();
		iCategoryEntries.Reset();
		CleanupStack::PopAndDestroy( category );
    	}
    return categoriesEqual;
	}

//scans entry builder list to find all entries with matching categories
//adds entry to category entries array
void CCalendarIndexingTestBase::GetMatchingEntriesL( const CCalCategory& aCategory )
	{
	const TInt KEntryCount = GetEntryBuilderL().Count();
	CCalEntry* entry = NULL;
	for( TInt i = 0; i < KEntryCount; ++i )
		{
		entry = GetEntryBuilderL().GetEntryList()[i];
		
		//CCalEntry& entry = *GetEntryBuilderL().GetEntryList()[i];
		const RPointerArray< CCalCategory >& entryCategories = entry->CategoryListL();
		const TInt KCategoryCount = entryCategories.Count();
		for( TInt j = 0; j < KCategoryCount; ++j )
			{
			const CCalCategory::TCalCategoryType KCategoryType = entryCategories[j]->Category();
			if ( KCategoryType == aCategory.Category() )
				{
				//if name of extended category doesn not match, these are not matching entries
				if( ( KCategoryType == CCalCategory::ECalExtended ) && 
					( entryCategories[j]->ExtendedCategoryName() != aCategory.ExtendedCategoryName() )
				   )
					{
					continue;
					}
				else
					{
					//entry contains category, add to category list
					iCategoryEntries.Append( entry );
					break;
					}
				}
			}
		}
	}

//measure in micro seconds the length of time it takes to find the instances
//for specified range
void CCalendarIndexingTestBase::MeasureViewCreationL(
		TTime aStartTime,
		TTime aEndTime,
		const TInt aYearsFromNow,
		const TDesC& aDescription)
	{
	TTimeIntervalYears yearsFromNow( aYearsFromNow );
	_LIT(KStartTime, "Instance View StartTime: ");
	_LIT(KEndTime, "Instance View EndTime: ");
	_LIT(KInstanceCount, "Loaded %d instances");
	
	//increase/dcrease start time by aYearsFromNow
	aStartTime += yearsFromNow;
	aEndTime += yearsFromNow;
	
	//print start/end time
	INFO_PRINTF1(KStartTime);
	PrintTimeL( aStartTime );
	INFO_PRINTF1(KEndTime);
	PrintTimeL( aEndTime );
	
	TCalTime startTcal;
	TCalTime endTcal;
	startTcal.SetTimeLocalL( aStartTime );
	endTcal.SetTimeLocalL( aEndTime );
	
    RPointerArray< CCalInstance  > viewInstances;
    CleanupResetDestroyPushL( viewInstances );
    //start time measurement
    StartOperationMeasurement();
    //find all instances in specified time region
    iInstanceView->FindInstanceL( viewInstances, 
    	CalCommon::EIncludeAll, CalCommon::TCalTimeRange(startTcal, endTcal) );
	INFO_PRINTF2(KInstanceCount, viewInstances.Count() );
	
	//end and print time measurement
    EndOperationMeasurement( aDescription );
    
    CleanupStack::PopAndDestroy( &viewInstances );
	}

//set operation start time to current time
void CCalendarIndexingTestBase::StartOperationMeasurement()
	{
	iOperationStart.HomeTime();
	}

//set operation end time to current time and subtract from start time
//print time diffrence	
TTimeIntervalMicroSeconds CCalendarIndexingTestBase::EndOperationMeasurement(
		const TDesC& aOperationDescription
		)
	{
	TTimeIntervalMicroSeconds operationLength;
	TTime operationEnd;
	operationEnd.HomeTime();
	
	operationLength = operationEnd.MicroSecondsFrom( iOperationStart );

    _LIT( KSessionTime, "Time taken to %S is: %LD microseconds\n" );
    INFO_PRINTF3( KSessionTime, &aOperationDescription, operationLength.Int64() );
    
    return operationLength;
	}

