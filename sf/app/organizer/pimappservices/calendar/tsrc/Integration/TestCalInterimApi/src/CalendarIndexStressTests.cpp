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
  
#include "CalendarIndexStressTests.h"

_LIT(KViewFail, "View comparison failed on iteration: %d");


CCalendarIndexTZBoundaryTest::CCalendarIndexTZBoundaryTest()
    {
    SetTestStepName(KCalendarIndexTZBoundaryTest);
    }

void CCalendarIndexTZBoundaryTest::PerformTestCaseL( )
    {
    iNotifyProgress = EFalse;
    RPointerArray< CCalEntry >* entryList = &GetEntryBuilderL().GetEntryList();
    CEntryProperty& entryProperty = GetEntryProperty();
    const TInt KEntryCount = entryList->Count();

	CCalEntry* entry = NULL;
	const CTzId* tz = NULL;
	const CTzId* tz2 = NULL;

    //execute test operation on created entries
    for( TInt i = 0; i < KEntryCount; ++i )
        {
        entry = (*entryList)[i];
        CCalEntry::TType type = entry->EntryTypeL();
        
        //execute test operation 
        ExecuteTestCaseOperationL( *entry, entryProperty.iStartDate, entryProperty.iEndDate );
    	
    	//set tz to new time zone
    	tz = iNewTimeZone;
    	SetTimeZoneL( tz );
    	
    	//check view is still valid
        if( !ValidateViewsL( *entry, entryProperty.iStartDate, entryProperty.iEndDate ) )
        	{
        	ERR_PRINTF2( KViewFail, i );
        	}
    
	    //wait for calendar server to close
	    CloseWaitAndReopenSessionL();
    
       	entryList = &GetEntryBuilderL().GetEntryList();
       	iUtility->COMPAREI1( entryList->Count(), ==, KEntryCount, i );
       	entry = (*entryList)[i];
       	
       	//check view is still valid after server closes
        if( !ValidateViewsL( *entry, entryProperty.iStartDate, entryProperty.iEndDate ) )
        	{
        	ERR_PRINTF2( KViewFail, i );
        	}
        	
        tz2 = iInitialTimeZone;
        SetTimeZoneL( tz2 );
        
        //validate after timezone reset to initial value
        if( !ValidateViewsL( *entry, entryProperty.iStartDate, entryProperty.iEndDate ) )
        	{
        	ERR_PRINTF2( KViewFail, i );
        	}
        
        //revert to default properties for initial entries	
        DELETE( iInitialEntryProperties );
        iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries );
	     
		DeleteCalendarFile();
		//Updating the entrList 
		entryList = &GetEntryBuilderL().GetEntryList();

		entry = NULL;
		tz = NULL;
		tz2 = NULL;
        }
       
	DELETE(entry);
	DELETE(tz);
	DELETE(tz2);
    }

//set time zone if its presnt
void CCalendarIndexTZBoundaryTest::SetTimeZoneL( const CTzId*& aTz )
	{
    if( aTz )
        {
        //lazy initilization
        if( !iRealTimeZone )
        	{
           	iRealTimeZone = iTzServer.GetTimeZoneIdL();
           	}
    	iTzServer.SetTimeZoneL( const_cast<CTzId&>( *aTz ) );
        }
    else
    	{
    	PRINTFAIL( "Timezone not specified in config file", EFalse );
    	}
	}

CCalendarIndexDSTBoundaryTest::CCalendarIndexDSTBoundaryTest()
    {
    SetTestStepName(KCalendarIndexDSTBoundaryTest);
    }


void CCalendarIndexDSTBoundaryTest::PerformTestCaseL( )
    {
    const TTimeIntervalSeconds KOneSecondInterval( 1 );
    const TInt KOneSecond = 1000000;
    
    //set test time to dst time
    iTestTime = iDSTTime.Int64();        
    
    //create and store entries with dst start tiem
    CEntryProperty& entryProperty = GetEntryBuilderL().GetEntryProperty();
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    
	CalCommon::TCalTimeRange timeRange(entryProperty.iStartDate, entryProperty.iEndDate ); 
	
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), timeRange ) );
	
	//set to dst -1 second
    User::LeaveIfError( iTzServer.SetHomeTime( iTestTime - KOneSecondInterval ) );
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), timeRange ) );
	
	//set to dst
    User::LeaveIfError( iTzServer.SetHomeTime( iTestTime ) );
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), timeRange ) );
	
	//set to dst +1 second
    User::LeaveIfError( iTzServer.SetHomeTime( iTestTime + KOneSecondInterval ) );
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), timeRange ) );
	
	//set to dst -1 second
    User::LeaveIfError( iTzServer.SetHomeTime( iTestTime - KOneSecondInterval ) );
    //wait for 2 seconds
    User::After( KOneSecond * 2 );
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), timeRange ) );

    }
    
void CCalendarIndexDSTBoundaryTest::ParseConfigSection(const TDesC& aSection)
    {
    CCalendarIndexingTestBase::ParseConfigSection(aSection);
    
    //get dst start time
    _LIT( KDSTTime, "DstTime" );
    iUtility->GetTimeFromConfig( aSection, KDSTTime, iDSTTime );
    }

CCalendarIndexFSErrorTest::CCalendarIndexFSErrorTest()
    {
    SetTestStepName(KCalendarIndexFSErrorTest);
    }

void CCalendarIndexFSErrorTest::PerformTestCaseL( )
    {
#ifndef __WINSCW__
    iNotifyProgress = EFalse;
    
    //fail test if no entries created
    if( !iUtility->COMPARE( GetEntryBuilderL().GetEntryList().Count(), >, 0 ) )
    	{
    	PRINTFAIL( "No entries defined", EFalse );
    	return;
    	}	
    
    const TInt expectedError = KErrDiskFull;
    TInt error = KErrNone;
    
  
    //transition to full disk mode
    iUtility->FillDiskL();
    TRAP( error, PerformOperationL() );
    //expect low disk error
    iUtility->COMPARE( expectedError, ==, error );
    //transition into normal disk mode
    iUtility->HighDiskModeL();
	
	//check that operation failed
	ValidateOperationL( ETrue );

    //wait for calendar server to close
    CloseWaitAndReopenSessionL();

	//check taht operation failed
   	ValidateOperationL( ETrue );
   	
   	ResetEnvironmentL();
   
   	error = KErrNone;
    TRAP( error, PerformOperationL() );
    iUtility->COMPARE( KErrNone, ==, error );
	
	//check operation was successful
	ValidateOperationL( EFalse );
	//transition to full disk mode
    iUtility->RefillL();

    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    //transition to normal disk mode
    iUtility->HighDiskModeL();

   	ValidateOperationL( EFalse );
   	ResetEnvironmentL();
   
   	error = KErrNone;
    TRAP( error, PerformOperationL() );
    iUtility->COMPARE( KErrNone, ==, error );
	
	//check operation was successful
	ValidateOperationL( EFalse );

    //wait for calendar server to close
    CloseSessions();
    iUtility->FindAndCloseCalendarServer( CCalendarTestUtility::EWaitForServerToClose, EFalse );
    //transition to full disk mode before reopening server
    iUtility->RefillL();
    OpenSessionsL();
    
    //check operation was successful
   	ValidateOperationL( EFalse );
   	ResetEnvironmentL();
	iUtility->ClearDiskL();
	//transition into normal disk mode
#endif
    } 

#ifndef __WINSCW__
//perform test case operation
void CCalendarIndexFSErrorTest::PerformOperationL()
	{
	CEntryProperty& entryProperty = GetEntryProperty();
    
    ExecuteTestCaseOperationL( *GetEntryBuilderL().GetEntryList()[0], entryProperty.iStartDate, entryProperty.iEndDate );
	}

//validate successful/failed operations	
void CCalendarIndexFSErrorTest::ValidateOperationL( const TBool aFailedOperation)
	{
	CEntryProperty& entryProperty = GetEntryProperty();
    
    if( aFailedOperation )
    	{    
    	//reset entry properties and entry list to default values
    	DELETE( iInitialEntryProperties );
    	iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries ); 
	    CloseSessions();
	    OpenSessionsL();
		//operation should be unsuccessful
	    if( !ValidateFailureL( *GetEntryBuilderL().GetEntryList()[0], entryProperty.iStartDate, entryProperty.iEndDate ) )
	    	{
	    	ERR_PRINTF2( KViewFail, 0 );
	    	}
    	}
	else
		{
		//operation should be successful
	    if( !ValidateViewsL( *GetEntryBuilderL().GetEntryList()[0], entryProperty.iStartDate, entryProperty.iEndDate ) )
	    	{
	    	ERR_PRINTF2( KViewFail, 0 );
	    	}
		}
	}

//validates unsuccessful operations
TBool CCalendarIndexFSErrorTest::ValidateFailureL(
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
		    //check that entry is not present
		    validComparison = CompareDeletedEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EDeleteEntry:
		    {
		    //check that entry has not been deleted
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EDeleteInstance:
		    {
		    //compare un-updated entry to instance/entry views
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    break;
		    }
		case EUpdate:
		    {
		    //compare un-updated entry to instance/entry views
		    validComparison = CompareEntryToViewsL( aEntry, timeRange );
		    validComparison = VerifyCategoriesL() && validComparison;
		    break;
		    }
		case EAddChild:
		    {
		    //no children should be added
		    ValidateChildren( aEntry, 0 );
		    break;
		    }
		case ESetTimeType:
		    {
		    //compare un-updated entry to instance/entry views
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

//reset environment to status prior to operation execution    
void CCalendarIndexFSErrorTest::ResetEnvironmentL()
	{
    //revert to default properties for initial entries	
    DELETE( iInitialEntryProperties );
    iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries ); 
    
    if( iOperationType == EStore )
    	{
	    DeleteCalendarFile();
    	}
    else
    	{
	    CloseSessions();
	    OpenSessionsL();
    	}
	}
#endif

//this step cleans up after CCalendarIndexFSErrorTest step       
CClearDisk::CClearDisk()
    {
    SetTestStepName(KClearDisk);
    }

//cleanup test step, should only cleanup files created in CCalendarIndexFSErrorTest     
TVerdict CClearDisk::doTestStepPreambleL()
	{
	//empty
	return TestStepResult();
	}

//delete all empty files that were generated by CCalendarIndexFSErrorTest	
TVerdict CClearDisk::doTestStepL()
	{
#ifndef __WINSCW__
    iUtility = CCalendarTestUtility::NewLC( *this );
    CleanupStack::Pop( iUtility );
    iUtility->ClearDiskL();
#endif
    return TestStepResult();
	}
	
TVerdict CClearDisk::doTestStepPostambleL()
	{
	//empty
	return TestStepResult(); 
	}
	
void CClearDisk::PerformTestCaseL( )
	{
	//empty
	}

CCalendarIndexCrashRecoveryTest::CCalendarIndexCrashRecoveryTest()
    {
    SetTestStepName(KCalendarIndexCrashRecoveryTest);
    }

void CCalendarIndexCrashRecoveryTest::PerformTestCaseL( )
    {
    //create entries using config data file        
    CEntryProperty& entryProperty = GetEntryBuilderL().GetEntryProperty();
    
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
	
	//check that entries were successfuly stored
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), CalCommon::TCalTimeRange(entryProperty.iStartDate, entryProperty.iEndDate ) ) );

    //forcefully panics calendar server, then closes and reopens server sessions
    PanicAndReopenSessionL();
   		
	//check that entries were successfuly stored after server panic
    iUtility->TESTPRINT( CompareEntriesToViewsL( GetEntryBuilderL().GetEntryList(), CalCommon::TCalTimeRange(entryProperty.iStartDate, entryProperty.iEndDate ) ) );

    }

//forcefully panics calendar server, then closes and reopens server sessions    
void CCalendarIndexCrashRecoveryTest::PanicAndReopenSessionL()
    {
    iUtility->FindAndCloseCalendarServer( CCalendarTestUtility::EForceCloseServer, EFalse );
    CloseSessions();
    OpenSessionsL();
    }


CCalendarIndexOOMTest::CCalendarIndexOOMTest()
    {
    SetTestStepName(KCalendarIndexOOMTest);
    }

void CCalendarIndexOOMTest::PerformTestCaseL( )
    {    
    iNotifyProgress = EFalse;
    CEntryProperty& entryProperty = GetEntryProperty();
    
    if( !iUtility->COMPARE( GetEntryBuilderL().GetEntryList().Count(), >, 0 ) )
    	{
    	return;
    	}
    
    TInt err = KErrNone;
    TInt failAt = 0;

	UnloadEntryBuilder();
    do
		{
		++failAt;
		
		GetEntryBuilderL();
		CCalEntry* theEntry = GetEntryBuilderL().GetEntryList()[0];
		
		//set heap failure on client
		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt);
		__UHEAP_MARK;		
	
		// all entries are loaded and unloaded here, to avoid any test related memory leaks
		//perform operation
		TRAP(err, ExecuteTestCaseOperationL( *theEntry, entryProperty.iStartDate, entryProperty.iEndDate ); 
			);
			
		//reset heap failure on client, check that there is no memory leak on client	
		__UHEAP_RESET;
		
		__UHEAP_MARKEND;//check for memory leaks
		
		//unload array of entries
		UnloadEntryBuilder();
		theEntry = NULL;   
		
		//if no memory error, continue test
		if( err == KErrNoMemory )
			{
			continue;
			}
		else if( err == KErrNone )
			{
			//if no error, test must be complete, verify entr
			GetEntryBuilderL();//load list of entries    	
		    
		    //validate operation was executed
	        if( !ValidateViewsL( *GetEntryBuilderL().GetEntryList()[0], entryProperty.iStartDate, entryProperty.iEndDate ) )
	        	{
	        	ERR_PRINTF2( KViewFail, failAt );
	        	}
		    //wait for calendar server to close
		    CloseWaitAndReopenSessionL();
	    	       	
	       	//validate operation was executed
	        if( !ValidateViewsL( *GetEntryBuilderL().GetEntryList()[0], entryProperty.iStartDate, entryProperty.iEndDate ) )
	        	{
	        	ERR_PRINTF2( KViewFail, failAt );
	        	}
			break;
			}
		else
			{
			//if other eror, fail and end test.
			iUtility->COMPAREI1( err, ==, KErrNoMemory, failAt );
			break;
			}

		}
	while(err != KErrNone);
    
    }


