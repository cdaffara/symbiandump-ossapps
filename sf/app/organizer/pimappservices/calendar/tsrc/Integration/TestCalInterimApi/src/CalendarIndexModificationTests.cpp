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
 
#include <calcategory.h>
#include "CalendarIndexModificationTests.h"

#include <calrrule.h>

static const TInt64 KOneMinute = 1*60*1000000;//1 minute

CCalendarIndexDCTest::CCalendarIndexDCTest()
    {
    SetTestStepName(KCalendarIndexDCTest);
    }

//copy old agenda calendar file to private directory 
TVerdict CCalendarIndexDCTest::doTestStepPreambleL()
	{
	_LIT(KDataPath,	"calenderdatalocation");
	_LIT(KPrivatePath,	"calenderprivatelocation");
	TPtrC dataPath;
	TPtrC privatePath;
	GetStringFromConfig(ConfigSection(), KDataPath, dataPath);
	GetStringFromConfig(ConfigSection(), KPrivatePath, privatePath);
	
	HBufC* fromPath = HBufC::NewLC( dataPath.Length() + 2 );
	HBufC* toPath = HBufC::NewLC( privatePath.Length() + 2 );
#ifdef __WINS__
	fromPath->Des().Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	fromPath->Des().Append(parse.Drive());
#endif

	fromPath->Des().Append( dataPath );
	toPath->Des().Append( privatePath );
	
	User::LeaveIfError( iPIMTestServer.Connect() );
	
	//delete existing calendar file from calendar private directory
	iPIMTestServer.DeleteFileL( toPath->Des() );
	//copy test calendar file from testdata directory to calendar private directory
	iPIMTestServer.CopyFileL( fromPath->Des(), toPath->Des() );
	
	iPIMTestServer.Close();
	
	CleanupStack::PopAndDestroy( toPath );
	CleanupStack::PopAndDestroy( fromPath );
	
	return CCalendarIndexingTestBase::doTestStepPreambleL();
	}
	
void CCalendarIndexDCTest::PerformTestCaseL( )
    {
    RPointerArray< CCalEntry >* entryList = &GetEntryBuilderL().GetEntryList();
    CEntryProperty& entryProperty = GetEntryProperty();
    const TInt KEntryCount = entryList->Count();
    
    //validate content of calendar file,
	iUtility->TESTPRINT( CompareEntriesToViewsL( *entryList, CalCommon::TCalTimeRange(entryProperty.iStartDate, entryProperty.iEndDate) ) );
	iUtility->TESTPRINT( VerifyCategoriesL() );
    
    CloseWaitAndReopenSessionL();
   	entryList = &GetEntryBuilderL().GetEntryList();
	
	//validate content of calendar file, after server restarted
	iUtility->TESTPRINT( CompareEntriesToViewsL( *entryList, CalCommon::TCalTimeRange(entryProperty.iStartDate, entryProperty.iEndDate) ) );
	iUtility->TESTPRINT( VerifyCategoriesL() );
    }

CCalendarUniqueIndexTest::CCalendarUniqueIndexTest()
    {
    SetTestStepName(KCalendarUniqueIndexTest);
    }

CCalendarUniqueIndexTest::~CCalendarUniqueIndexTest()
    {
    iLocalUidArray.Close();
    }

void CCalendarUniqueIndexTest::PerformTestCaseL( )
    {
    //create entries using config data file        
    const TInt KEntryCount = GetEntryBuilderL().Count();
    
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    
    CCalEntry* entry = NULL;
    //cache local uids of entries
    for( TInt i = 0; i < KEntryCount; ++i )
    	{
        entry = GetEntryBuilderL().GetEntryList()[i];
        iLocalUidArray.Append( entry->LocalUidL() );
        entry = NULL;
    	}
    	
    DELETE(entry);
    
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    
    iUtility->TESTPRINT( ValidateUidsL() );
    }

//compare uid and local uid from entry and retrieved entry
TBool CCalendarUniqueIndexTest::ValidateUidsL()
	{
	TBool uidsMatchStoredUids = ETrue;
	const TInt KEntryCount = GetEntryBuilderL().Count();
	CCalEntry* localEntry = NULL;
	CCalEntry* storedEntry = NULL;
    for( TInt i = 0; i < KEntryCount; ++i )
    	{
    	//retrieve entry by local uid
        storedEntry = GetEntryBuilderL().GetEntryList()[i];
        localEntry = iEntryView->FetchL( iLocalUidArray[i] );
        CleanupStack::PushL( localEntry );
        uidsMatchStoredUids = iUtility->TESTPRINT( localEntry != NULL ) && uidsMatchStoredUids;
        uidsMatchStoredUids = uidsMatchStoredUids && iUtility->COMPARE( iLocalUidArray[i], ==, localEntry->LocalUidL() );
        uidsMatchStoredUids = uidsMatchStoredUids && iUtility->COMPARE( storedEntry->UidL(), ==, localEntry->UidL() );
        
        HBufC* uID = HBufC::NewLC(localEntry->UidL().Length());
  		uID->Des().Copy(localEntry->UidL());
  		INFO_PRINTF2(KInfoGUIDEntry, uID);
  		CleanupStack::PopAndDestroy( uID );
  		
        CleanupStack::PopAndDestroy( localEntry );
        
  		uID = HBufC::NewLC(storedEntry->UidL().Length());
  		uID->Des().Copy(storedEntry->UidL());
  		INFO_PRINTF2(KInfoGUIDEntry, uID);
  		CleanupStack::PopAndDestroy( uID );
     	
        //retrieve entry by uid
        RPointerArray< CCalEntry > entryList;
        iEntryView->FetchL( storedEntry->UidL(), entryList );
        CleanupResetDestroyPushL( entryList );
        INFO_PRINTF2(_L("number of entries %d"),entryList.Count());
        uidsMatchStoredUids = uidsMatchStoredUids && iUtility->COMPARE( entryList.Count(), ==, 1 );
        uidsMatchStoredUids = uidsMatchStoredUids && iUtility->COMPARE( entryList[0]->LocalUidL(), ==, iLocalUidArray[i] );
        uidsMatchStoredUids = uidsMatchStoredUids && iUtility->COMPARE( storedEntry->UidL(), ==, entryList[0]->UidL() );
        CleanupStack::PopAndDestroy( &entryList );
        storedEntry = NULL;
        localEntry = NULL;
    	}
    	
    DELETE( storedEntry );
    DELETE( localEntry );
    return ( KEntryCount > 0 && uidsMatchStoredUids );
	}

void CCalendarIndexCategoryTest::PerformTestCaseL( )
    {
    //create entries using config data file        
    iNotifyProgress = EFalse;
    TInt numberOfEntries = 0;
    //store entries with retrieved categories
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    
    VerifyCategoriesL();
    
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    
    VerifyCategoriesL();
    }
		
CCalendarIndexModificationTest::CCalendarIndexModificationTest()
    {
    SetTestStepName(KCalendarIndexModificationTest);
    }

void CCalendarIndexModificationTest::PerformTestCaseL( )
    {
    iNotifyProgress = EFalse;
   	_LIT(KViewFail, "View comparison failed on iteration: %d");
    //create entries using config data file 
        	
    RPointerArray< CCalEntry >* entryList = &GetEntryBuilderL().GetEntryList();
    CEntryProperty* entryProperty = &GetEntryProperty();
    const TInt KEntryCount = entryList->Count();
	CCalEntry* entry = NULL;

    //execute test operation on created entries, iterates through all lists
    for( TInt i = 0; i < KEntryCount; ++i )
        {
       	entryList = &GetEntryBuilderL().GetEntryList();
       	entryProperty = &GetEntryProperty();
        entry = (*entryList)[i];
        TInt catcount = iCategoryManager->CategoryCountL();
        
       HBufC* uID = HBufC::NewLC(entry->UidL().Length());
  		uID->Des().Copy(entry->UidL());
  		INFO_PRINTF2(KInfoGUIDEntry, uID);
  		CleanupStack::PopAndDestroy( uID );

        ExecuteTestCaseOperationL( *entry, entryProperty->iStartDate, entryProperty->iEndDate );
    	catcount = iCategoryManager->CategoryCountL();
    	//validate view after operation executed
        if( !ValidateViewsL( *entry, entryProperty->iStartDate, entryProperty->iEndDate ) )
        	{
        	ERR_PRINTF2( KViewFail, i );
        	}
    	if( iOperationType == EUpdate )
    		{
	        //revert to default properties for initial entries	
	        DELETE( iInitialEntryProperties );
	        iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries );       
    		}
    		
    	catcount = iCategoryManager->CategoryCountL();
	    //wait for calendar server to close
	    CloseWaitAndReopenSessionL();
    		
    	if( iOperationType == EUpdate )
    		{
			//set single entry to be updated
			GetEntryBuilderL().SetSingleUpdate( *GetEntryBuilderL().GetEntryList()[i] );
			//update entry
			UpdateEntriesFromConfigL(GetEntryBuilderL(), *iTestEntryProperties);
			//reset multiple updates
			GetEntryBuilderL().SetMultipleUpdate();
    		}
    	
    	entryList = NULL;
       	entryList = &GetEntryBuilderL().GetEntryList();
       	entryProperty = &GetEntryProperty();
       	iUtility->COMPAREI1( entryList->Count(), ==, KEntryCount, i );
       	
       	entry = NULL;
       	entry = (*entryList)[i];
       	
       	catcount = iCategoryManager->CategoryCountL();
       	//validate view after server re-opened
        if( !ValidateViewsL( *entry, entryProperty->iStartDate, entryProperty->iEndDate ) )
        	{
        	ERR_PRINTF2( KViewFail, i );
        	}
        	catcount = iCategoryManager->CategoryCountL();
        //revert to default properties for initial entries	
        DELETE( iInitialEntryProperties );
        iInitialEntryProperties = iParser.ParseEntryL( iInitialEntries ); 
        
        if( iOperationType == EStore )
        	{
        	//if performing store operation, delete calendar after each store
		    DeleteCalendarFile();
        	}
        	
        else if( iOperationType == EUpdate )
        	{
        	//if performing update operation, delete calendar after each store
        	
		    DeleteCalendarFile();
		    entryList = &GetEntryBuilderL().GetEntryList();
		    //recreate database from scratch
		    if( iInitialEntryProperties && iInitialEntryProperties->ContainsEntries() )
		    	{
			    //store entries in db here:
			    TInt numberOfEntries = 0;
			    iEntryView->StoreL( *entryList, numberOfEntries);
			    //check that all generated entries have been stored
			    iUtility->COMPARE( numberOfEntries, ==, entryList->Count() );
		    	iStoredEntryCount += numberOfEntries;
		    	}
        	}
        else
        	{
        	//recreates entry builder with fresh list
		    CloseSessions();
		    OpenSessionsL();
        	}
        	
        entry = NULL;
        entryList = NULL;
        }
        
    DELETE(entry);
    DELETE(entryList);
    }

CCalendarIndexAlarmEntriesTest::CCalendarIndexAlarmEntriesTest()
    {
    SetTestStepName(KCalendarIndexAlarmEntriesTest);
    }
    
CCalendarIndexAlarmEntriesTest::~CCalendarIndexAlarmEntriesTest()
	{
	iAlarmSession.Close();
	}

TVerdict CCalendarIndexAlarmEntriesTest::doTestStepPreambleL()
	{
	CCalendarIndexingTestBase::doTestStepPreambleL();
	User::LeaveIfError( iAlarmSession.Connect() );
	return TestStepResult();
	}

void CCalendarIndexAlarmEntriesTest::PerformTestCaseL( )
    {    
    //verify alarms generated before server closes, and verify no alarms after server closes
    TInt numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
	VerifyAlarmsL( GetEntryBuilderL().Count() );
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
	VerifyAlarmsL( 0 );
	
    //empty database
    DeleteCalendarFile();
    
    //verify alarms generated after server closes
    numberOfEntries = 0;
    iEntryView->StoreL( GetEntryBuilderL().GetEntryList(), numberOfEntries);
    //check that all generated entries have been stored
    iUtility->COMPARE( numberOfEntries, ==, GetEntryBuilderL().Count() );
    
    //wait for calendar server to close
    CloseWaitAndReopenSessionL();
    
    VerifyAlarmsL( GetEntryBuilderL().Count() );
    //empty database
    DeleteCalendarFile();
    }

void CCalendarIndexAlarmEntriesTest::AddEntriesFromConfigL( CEntryBuilder& aEntryBuilder )
	{
	CCalendarIndexingTestBase::AddEntriesFromConfigL( aEntryBuilder );
	
	CEntryProperty& entryProperty = GetEntryBuilderL().GetEntryProperty();
	
    if( entryProperty.iAlarmOffset > 0 )
    	{
    	//if valid alarm offset, add alarms to entries
    	GetEntryBuilderL().AddAlarmsToEntriesL( entryProperty.iAlarmOffset );
    	}
	else
		{
		PRINTFAIL( "This is not an alarm test", EFalse );
		}
	}

//all entries should have an alarm associated
void CCalendarIndexAlarmEntriesTest::VerifyAlarmsL( const TInt aAlarmCount )
	{
	//array of unique entries with correspending alarms generated, does not own entries, stored in builder array
	RPointerArray< CCalEntry > entriesWithExpiredAlarms;
	CleanupClosePushL( entriesWithExpiredAlarms );
	
	TTime startTime;
	TTime currentTime;
	startTime.HomeTime();
	
	TInt iteration = 0;
	for( ;( entriesWithExpiredAlarms.Count() != aAlarmCount ); ++iteration  )
		{
		TRequestStatus status;
		TAlarmId alarmId;
		iAlarmSession.NotifyChange( status, alarmId );
		//wait for an alarm to be generated
		User::WaitForRequest( status );
		//check that generated alarm matches entry
		VerifyAlarmStatusL( status, alarmId, entriesWithExpiredAlarms );
		
		currentTime.HomeTime();
		//if having waited more than 3 minutes, stop waiting
		if( currentTime.MicroSecondsFrom( startTime ).Int64() > (KOneMinute * 3) )
			{
			PRINTFAIL( "Timeout: Waited for more than 3 minutes", ETrue );
			break;
			}
		}
	
	//all alarms associated with entries should have been activated
	iUtility->COMPAREI1( entriesWithExpiredAlarms.Count(), ==, aAlarmCount, iteration );
	CleanupStack::PopAndDestroy( &entriesWithExpiredAlarms );
	
	}

//check that expected alarm status generated
//generate entry based on alarm data
//compare entry to entry builder list, if it matches an entry
//and not within existing list of alarm entries, add to entry list
void CCalendarIndexAlarmEntriesTest::VerifyAlarmStatusL( 
		const TRequestStatus& aStatus,
		const TAlarmId aAlarmId,
		RPointerArray< CCalEntry >& aEntryList)
	{
	//check that expected alarm status generated
	if( aStatus.Int() == EAlarmChangeEventTimerExpired )
		{
		HBufC8* entryData = NULL;
		iAlarmSession.GetAlarmData( aAlarmId, entryData );
		
		if( entryData == NULL )
			{
			return;
			}
		
		//generate entry based on alarm data	
		CleanupStack::PushL( entryData );
		CCalEntryId* entryId = CCalEntryId::NewL( entryData );//take ownership
		CleanupStack::Pop( entryData );
		CleanupStack::PushL( entryId );
		TCalLocalUid entryUid = entryId->LocalUidL();
		CleanupStack::PopAndDestroy( entryId );
		CCalEntry* alarmEntry = iEntryView->FetchL( entryUid );
		
		if( alarmEntry == NULL )
			{
			return;
			}
			
		CleanupStack::PushL( alarmEntry );
		TIdentityRelation< CCalEntry > entryComparisonFunc( CCalendarIndexAlarmEntriesTest::CompareEntryGuidsL );
		//compare entry to entry builder list, 
		const TInt entryLocation = GetEntryBuilderL().GetEntryList().Find( alarmEntry, entryComparisonFunc );
		const TInt existingEntryLocation = aEntryList.Find( alarmEntry, entryComparisonFunc );
		CleanupStack::PopAndDestroy( alarmEntry );
		
		//if it doesn't match an entry
		if( entryLocation < 0 )
			{
			return;
			}
		//if it matches an entry and not within existing list of alarm entries, add to entry list
		else if( existingEntryLocation < 0 )
			{
			aEntryList.Append( GetEntryBuilderL().GetEntryList()[ entryLocation ] );
			}
		}
	}

TBool CCalendarIndexAlarmEntriesTest::CompareEntryGuidsL(
		const CCalEntry& aEntry1, 
		const CCalEntry& aEntry2 )
	{
	
	return aEntry1.UidL() == aEntry2.UidL();
	
	}
