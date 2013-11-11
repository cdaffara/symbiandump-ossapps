/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "caleninterimutils2test.h"
#include <calentry.h>

//  LOCAL CONSTANTS AND MACROS
const TInt KDefaultStartTime( 8 );

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Delete() 
{
    // Nothing.
	if(iCalenInterimUtils)
	{
		delete iCalenInterimUtils;
		iCalenInterimUtils = NULL;
	}
	
	if(iCalEntryView)
	{
		delete iCalEntryView;
		iCalEntryView = NULL;
	}
	
	if(iWait)
	{
		delete iWait;
		iWait = NULL;
	}
	
	if(iCalSession)
	{
		delete iCalSession;
		iCalSession = NULL;
	}
}

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::RunMethodL( 
        CStifItemParser& aItem ) 
    {
	TRAP(iError, iCalSession = CCalSession::NewL(););
	const TDesC& file = iCalSession->DefaultFileNameL(); 
	TRAP(iError, iCalSession->OpenL(file));
	iCalenInterimUtils = CCalenInterimUtils2::NewL();
    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalenInterimUtils2Test::ExampleL ),

        /** Test functions for API CalenDateUtils */
        ENTRY( "TestGlobalUUID", 
                CCalenInterimUtils2Test::TestGlobalUUIDL ),
		ENTRY( "TestPopulateChildEntry", 
					   CCalenInterimUtils2Test::TestPopulateChildEntryL ),
	    ENTRY( "TestStore", 
					  CCalenInterimUtils2Test::TestStoreL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::TestGlobalUUIDL
// Test funtion to test the API CCalenInterimUtils2::GlobalUidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::TestGlobalUUIDL( CStifItemParser& /*aItem*/ )
    {
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenInterimUtils2Test, "CalenInterimUtils2Test" );
    _LIT( KDuplicateUids, "Uids are duplicate" );
    _LIT( KTestGlobalUUID, "In TestGlobalUUIDL" );
    _LIT( KTestGlobalUUIDs, "TestGlobalUUIDs are " );
    
    TestModuleIf().Printf( 0, KCalenInterimUtils2Test, KTestGlobalUUID );
    // Print to log file
    iLog->Log( KTestGlobalUUID );
    iLog->Log( KTestGlobalUUIDs );
    // Make the call to the API.
    // Create unique ID.
    RPointerArray<HBufC8> guids;
    TInt i = 0;
    for( ; i<20; i++ )
        {
        HBufC8* guid = iCalenInterimUtils->GlobalUidL();
        iLog->Log( *guid );
        guids.AppendL(guid);
        }
    
    // Validate the guids so that none of them will be duplicates
    for(i=1; i < 20; i++)
        {
        for(TInt j = 0; j < i; j++)
            {
            if( guids[j] == guids[i] )
                {
                iLog->Log( KDuplicateUids );
                retValue = KErrNotFound;
                break;
                }
            }
        }
    guids.ResetAndDestroy();

    return retValue;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::TestPopulateChildEntryL
// Test funtion to test the API CCalenInterimUtils2::PopulateChildEntryL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::TestPopulateChildEntryL( CStifItemParser& /*aItem*/ )
{
	TInt retValue = KErrNone;
	_LIT8( KTestUid, "123547");
	// Print to UI
	_LIT( KCalenInterimUtils2Test, "CalenInterimUtils2Test" );
	_LIT( KTestGlobalUUID, "In TestPopulateChildEntryL" );
	
	TestModuleIf().Printf( 0, KCalenInterimUtils2Test, KTestGlobalUUID );
	
	// Create a parent entry
	//entry
	CCalEntry* parentEntry = NULL;
	
	parentEntry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
	// Set necessary field
	parentEntry->SetSummaryL(_L("test Entry"));
	parentEntry->SetLocationL(_L("test Location"));
	parentEntry->SetStatusL(CCalEntry::EConfirmed);
	CleanupStack::PushL( parentEntry );
	
	// Create the child entry
	CCalEntry* childEntry = NULL;
		
	childEntry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
	CleanupStack::PushL( childEntry );
	// Call the api so that all the parent attributes gets transfered to child 
	iCalenInterimUtils->PopulateChildFromParentL(*childEntry, *parentEntry);
	
	// Validation
	if(childEntry->SummaryL().Compare(parentEntry->SummaryL()))
	{
		retValue = KErrGeneral;
	}
	else if(childEntry->LocationL().Compare(parentEntry->LocationL()))
	{
		retValue = KErrGeneral;
	}
	else if(childEntry->StatusL() != parentEntry->StatusL())
	{
		retValue = KErrGeneral;
	}
	CleanupStack::PopAndDestroy(childEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	return retValue;
}

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::TestStoreL
// Test funtion to test the API CCalenInterimUtils2::StoreL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenInterimUtils2Test::TestStoreL( CStifItemParser& /*aItem*/ )
{
	TInt retValue = KErrNone;
	// Print to UI
	_LIT( KCalenInterimUtils2Test, "CalenInterimUtils2Test" );
	_LIT( KTestGlobalUUID, "In TestStoreL" );
	TestModuleIf().Printf( 0, KCalenInterimUtils2Test, KTestGlobalUUID );
	
	// Create the entry view
	TRAPD(error, iCalEntryView = CCalEntryView::NewL(*iCalSession, *this);) 
	if(!iWait)
	{
		TRAP(error, iWait = new (ELeave) CActiveSchedulerWait;);
		if(!iWait->IsStarted())
		{
			iWait->Start();
		}
	}
	if(iError != KErrNone)
	{
		return KErrGeneral;
	} 
	else
	{
		// Entry view creation is complete
		// Create the CcalEntry and store it
		_LIT8( KTestUid, "123548");
		CCalEntry* parentEntry = NULL;
			
		parentEntry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
		// Set necessary field
		parentEntry->SetSummaryL(_L("test Entry"));
		parentEntry->SetLocationL(_L("test Location"));
		TCalTime startTime;
		TDateTime dateTime;
		dateTime.Set(2010, TMonth(1), 1, 10, 0, 0, 0);
		TTime time(dateTime);
		startTime.SetTimeLocalL(time);
		parentEntry->SetStartAndEndTimeL(startTime, startTime);
		parentEntry->SetStatusL(CCalEntry::EConfirmed);
		CleanupStack::PushL( parentEntry );
		
		// Store it
		iCalenInterimUtils->StoreL(*iCalEntryView, *parentEntry, false);
		CleanupStack::PopAndDestroy(parentEntry);
	}
	return retValue;
}

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::Completed
// Callback function for entry view creation.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Completed(TInt aError)
{
	iError = aError;
	// Stop the wait timer
	if( iWait && iWait->IsStarted())
	{
		iWait->AsyncStop();
	}
}
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
