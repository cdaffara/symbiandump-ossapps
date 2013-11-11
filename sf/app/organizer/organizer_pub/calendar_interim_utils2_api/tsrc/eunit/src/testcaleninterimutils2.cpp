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
* Description:   test caleninterimutils2 api
*
*/



//  CLASS HEADER
#include "testcaleninterimutils2.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <calentry.h>               //CCalEntry
#include <CalUser.h>				//CCalUser
#include <CalRRule.h>				//TCalRRule
#include <CalEntryView.h>			//CCalEntryView
#include <CalSession.h>				//CCalSession
#include <S32MEM.H>                 //RDesReadStream
#include "CleanupResetAndDestroy.h"
#include <caldataexchange.h>        //CCalDataExchange
#include <caldataformat.h>          //KUidVCalendar

//  INTERNAL INCLUDES
#include <CalenInterimUtils2.h>

// Constants
_LIT(KApptSummary,      "Summary");
_LIT(KExText,           "EXEXEX");
_LIT(KOrganizer,        "a@b.com");
_LIT(KAttendee,         "test@test.com");
_LIT(KDescription,      "Test description");
_LIT(KLocation,         "Nokia");
_LIT( KCalendarDatabaseFilePath, "c:Calendar" );

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test* CCalenInterimUtils2Test::NewL()
    {
    CCalenInterimUtils2Test* self = CCalenInterimUtils2Test::NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test* CCalenInterimUtils2Test::NewLC()
    {
    CCalenInterimUtils2Test* self = new( ELeave ) CCalenInterimUtils2Test();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }
// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::~CCalenInterimUtils2Test
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test::~CCalenInterimUtils2Test()
    {
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::CCalenInterimUtils2Test
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenInterimUtils2Test::CCalenInterimUtils2Test()
    {
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::ConstructL
// Performs the second phase construction.
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::SetupL
// Setup 
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::SetupL(  )
    {
    iAsWait = new (ELeave) CActiveSchedulerWait();
    
    EUNIT_ASSERT_NO_LEAVE( iCalSession = CCalSession::NewL() );

    EUNIT_ASSERT_NO_LEAVE( iCalSession->OpenL(KCalendarDatabaseFilePath) );

    EUNIT_ASSERT_NO_LEAVE( iEntryView = CCalEntryView::NewL( *iCalSession, *this ) );
    
    EUNIT_ASSERT_NO_LEAVE( iAsWait->Start() );   // Blocks until Complete is called
    
    EUNIT_ASSERT_NO_LEAVE( iUtils = CCalenInterimUtils2::NewL() );
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::Teardown
// Teardown
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Teardown(  )
    {
    delete iUtils;
    iUtils = NULL;
    
    delete iEntryView;
    iEntryView = NULL;
    
    delete iCalSession;
    iCalSession = NULL;

    //clean up active scheduler wait
    delete iAsWait;
    iAsWait = NULL;
    }
    
// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::CCalenInterimUtils2TestNewL
// 
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::CCalenInterimUtils2TestNewL()
    {
    }
    
// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::MRViewersEnabledL
// Check MR Viewers Enabling
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::MRViewersEnabledL(  )
    {
    TBool enabled = iUtils->MRViewersEnabledL( ETrue );

    // When Meeting request flag is disabled return value should be EFalse
     EUNIT_ASSERT_EQUALS( enabled, EFalse );
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::GlobalUidL
// Test global uid
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::GlobalUidL(  )
    {
    HBufC8* uid = iUtils->GlobalUidL();
    CleanupStack::PushL(uid);
    EUNIT_ASSERT( uid );
    CleanupStack::Pop(uid);
    delete uid;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::PopulateChildFromParentL
// Populate child from parent
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::PopulateChildFromParentL(  )
    {    
    // Create an entry (parent).  This should really be a repeating appointment,
    // but as we're not storing the entries, we can use CCalEntry::CompareL for
    // verification.  This would fail if the parent was repeating however.
    CCalEntry* parent = CreateNonRepeatingApptL();
    CleanupStack::PushL(parent);

    CCalUser* organizer = CCalUser::NewL(KOrganizer);
    CleanupStack::PushL(organizer);
    parent->SetOrganizerL(organizer); //Passing ownership
    CleanupStack::Pop(organizer);
    
    CCalAttendee* attendee = CCalAttendee::NewL(KAttendee);
    CleanupStack::PushL(attendee);
    parent->AddAttendeeL(attendee);
    CleanupStack::Pop(attendee);
    
    // Create an entry (child)
	CCalEntry* child = CCalEntry::NewL( parent->EntryTypeL(),
                                        parent->UidL().AllocL(),
                                        CCalEntry::EMethodNone,
                                        parent->SequenceNumberL(),
                                        parent->StartTimeL(),
                                        CalCommon::EThisOnly);
    CleanupStack::PushL(child);
    

    EUNIT_ASSERT_NO_LEAVE( iUtils->PopulateChildFromParentL(*child, *parent) );
    
    EUNIT_ASSERT_NO_LEAVE( child->SetStartAndEndTimeL(parent->StartTimeL(), parent->EndTimeL()) );
    
    EUNIT_ASSERT( parent->CompareL(*child) );
    
    EUNIT_ASSERT_NO_LEAVE( iUtils->PopulateChildFromParentL(*child, *parent) );
    
    CleanupStack::PopAndDestroy(child);
    CleanupStack::PopAndDestroy(parent);
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::StoreL
// Store the entry
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::StoreL(  )
    {
    // Create a repeating entry
    CCalEntry* repEntry = CreateRepeatingApptL();
    CleanupStack::PushL(repEntry);
  
    //Keep the UID hanging around, we need this
    HBufC8* uid = repEntry->UidL().AllocLC();
    
    //Store the entry
    EUNIT_ASSERT_NO_LEAVE( iUtils->StoreL(*iEntryView, *repEntry) );

     //Fetch the entry by uid.  We should see exactly 1 entry returned.
    EUNIT_ASSERT_NO_LEAVE( iEntryView->FetchL(*uid, iEntries) );
    
    TInt entryCount = iEntries.Count();
    EUNIT_ASSERT_EQUALS( entryCount, 1 );
    
    //Create an exception to our repeating entry
    CCalEntry* exception = CreateExceptionL(*repEntry);
    CleanupStack::PushL(exception);
    
    //Store the exception in the database
    EUNIT_ASSERT_NO_LEAVE( iUtils->StoreL(*iEntryView, *exception) );
    
    //Clear the entry array
    iEntries.ResetAndDestroy();  
    entryCount = iEntries.Count();
        
    //Fetch the entry by uid.  We should see exactly 2 entries returned (one parent, one child)
    EUNIT_ASSERT_NO_LEAVE( iEntryView->FetchL(*uid, iEntries) );
         
    entryCount = iEntries.Count();   
    EUNIT_ASSERT_EQUALS( entryCount, 2 );
    
    //Clear the entry array
    iEntries.ResetAndDestroy();  
    entryCount = iEntries.Count();
    EUNIT_ASSERT_NO_LEAVE( iEntryView->DeleteL(*repEntry) );
    
    //Fetch the entry by uid.  
    EUNIT_ASSERT_NO_LEAVE( iEntryView->FetchL(*uid, iEntries) );
         
    entryCount = iEntries.Count();
    
    EUNIT_ASSERT_EQUALS( entryCount, 0);
    
    CleanupStack::PopAndDestroy(exception);
    CleanupStack::PopAndDestroy(uid);
    CleanupStack::PopAndDestroy(repEntry);
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::Store2L
// Store the entry
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Store2L(  )
    {
    // Create a repeating entry
    CCalEntry* repEntry = CreateRepeatingApptL();
    CleanupStack::PushL(repEntry);
    
    //Keep the UID hanging around, we need this
    HBufC8* uid = repEntry->UidL().AllocLC();
    
    //Store the entry
    EUNIT_ASSERT_NO_LEAVE( iUtils->StoreL(*iEntryView, *repEntry) );

    CCalEntry* ex1 = CreateExceptionL(*repEntry);
    CleanupStack::PushL(ex1);
    
    EUNIT_ASSERT_NO_LEAVE( iUtils->StoreL(*iEntryView, *ex1) );

    CleanupStack::PopAndDestroy(ex1);
    CleanupStack::PopAndDestroy(uid);
 	
 	CleanupStack::PopAndDestroy(repEntry);
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::IsMeetingRequestL
// Check for Meeting Request
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::IsMeetingRequestL(  )
    {
    // Create meeting request
    CCalEntry* entry = CreateNonRepeatingApptL();
    CleanupStack::PushL(entry);

    TBool isMeetingRequest = iUtils->IsMeetingRequestL(*entry);
    
    EUNIT_ASSERT_EQUALS( isMeetingRequest, EFalse );


    CCalUser* organizer = CCalUser::NewL(KOrganizer);
    CleanupStack::PushL(organizer);
    entry->SetOrganizerL(organizer); //Passing ownership
    CleanupStack::Pop(organizer);
    
    isMeetingRequest = iUtils->IsMeetingRequestL(*entry);
    
    EUNIT_ASSERT_EQUALS( isMeetingRequest, ETrue );

    CleanupStack::PopAndDestroy(entry);
    }

//Helper methods

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::CreateNonRepeatingApptL
// Creates a non repeating appointment and returns a pointer to the
// new entry
// -----------------------------------------------------------------------------
//
CCalEntry* CCalenInterimUtils2Test::CreateNonRepeatingApptL()
	{
	//Start times for Appt
	TTime now;
	now.UniversalTime();
	TCalTime startTime;
	startTime.SetTimeUtcL(now);
	User::After(100000);
    
    HBufC8* uid = iUtils->GlobalUidL();
    CleanupStack::PushL(uid); 
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodPublish, 0);
    CleanupStack::Pop(uid);  
    CleanupStack::PushL(entry);
        
    entry->SetStartAndEndTimeL(startTime,startTime);
	entry->SetSummaryL(KApptSummary);
	entry->SetDescriptionL(KDescription);
	entry->SetLocationL(KLocation);
	entry->SetPriorityL(1); //High priority
	entry->SetStatusL(CCalEntry::EConfirmed);
	entry->CategoryListL();
	CleanupStack::Pop(entry);
	
	return entry;
	}

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::CreateRepeatingApptL
// Creates a repeating appointment and returns a pointer to the
// new entry
// -----------------------------------------------------------------------------
//	
CCalEntry* CCalenInterimUtils2Test::CreateRepeatingApptL()
    {
    CCalEntry* entry = CreateNonRepeatingApptL();
    CleanupStack::PushL(entry);
   	//Add a daily repeat
	TCalRRule rule;
	rule.SetType(TCalRRule::EDaily);
	rule.SetDtStart(entry->StartTimeL());
	rule.SetCount(10);	
	entry->SetRRuleL(rule);
	CleanupStack::Pop(entry);
    return entry;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::CreateExceptionL
// Creates an exception to the passed in parent entry.
// -----------------------------------------------------------------------------	
//
CCalEntry* CCalenInterimUtils2Test::CreateExceptionL(const CCalEntry& aParent)
    {
	CCalEntry* child = CCalEntry::NewL(aParent.EntryTypeL(), aParent.UidL().AllocL(),
                                        aParent.MethodL(), aParent.SequenceNumberL(),
                                        aParent.StartTimeL(), CalCommon::EThisOnly);

    CleanupStack::PushL(child);
	iUtils->PopulateChildFromParentL(*child, aParent);
    child->SetSummaryL(KExText);
	child->SetLocationL(KExText);
	child->SetStartAndEndTimeL(aParent.StartTimeL(), aParent.EndTimeL());
    CleanupStack::Pop(child);
    return child;
    }
// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::ImportSingleEntryL
// Imports one vCal file
// -----------------------------------------------------------------------------
//
CCalEntry*  CCalenInterimUtils2Test::ImportSingleEntryL( CCalSession& aSession,
                                    const TDesC8& aVCal )
    {
    RDesReadStream stream( aVCal );
    CCalEntry* entry = ImportSingleEntryL( aSession, stream );
    return entry;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::ImportSingleEntryL
// Import single entry
// -----------------------------------------------------------------------------
//
CCalEntry* CCalenInterimUtils2Test::ImportSingleEntryL( CCalSession& aSession, 
                               RReadStream& aStream )
    {
    CCalEntry* entry = NULL;

    CCalDataExchange* exchange = CCalDataExchange::NewL( aSession );
    CleanupStack::PushL( exchange );
    
    RPointerArray< CCalEntry > entries;
    CleanupResetAndDestroyPushL( entries );

    exchange->ImportL( KUidVCalendar, aStream, entries );
    
    if ( entries.Count() == 1 )
        {
        entry = entries[0]; 
        entries.Remove(0);            
        }
    else if ( entries.Count() == 0 )
        {
        User::Leave( KErrArgument);
        }
    else
        {
        User::Leave( KErrTooBig );
        }
    CleanupStack::PopAndDestroy( &entries ); 
    CleanupStack::PopAndDestroy( exchange );
    return entry;
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::Completed
// From MCalProgressCallback.
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Completed(TInt /*aStatus*/)
    {
    //iError = aStatus;
    iAsWait->AsyncStop();
    }

// -----------------------------------------------------------------------------
// CCalenInterimUtils2Test::Progress
// From MCalProgressCallback. Intentionally empty.
// -----------------------------------------------------------------------------
//
void CCalenInterimUtils2Test::Progress( TInt /*aPercentageCompleted*/ )
    {
    }

// -----------------------------------------------------------------------------
//CCalenInterimUtils2Test::NotifyProgress
//From MCalProgressCallback. Don't notify us about progress updates.
// -----------------------------------------------------------------------------
//
TBool CCalenInterimUtils2Test::NotifyProgress()
    {
    return EFalse;
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    CCalenInterimUtils2Test,
    "Unit tests for CalenInterimUtils2",
    "UNIT" )

EUNIT_TEST(
    "NewL - test ",
    "CCalenInterimUtils2",
    "NewL",
    "FUNCTIONALITY",
    SetupL, CCalenInterimUtils2TestNewL, Teardown)

EUNIT_TEST(
    "MRViewersEnabledL - test ",
    "CCalenInterimUtils2",
    "MRViewersEnabledL",
    "FUNCTIONALITY",
    SetupL, MRViewersEnabledL, Teardown)

EUNIT_TEST(
    "GlobalUidL - test ",
    "CCalenInterimUtils2",
    "GlobalUidL",
    "FUNCTIONALITY",
    SetupL, GlobalUidL, Teardown)

EUNIT_TEST(
    "PopulateChildFromParentL - test ",
    "CCalenInterimUtils2",
    "PopulateChildFromParentL",
    "FUNCTIONALITY",
    SetupL, PopulateChildFromParentL, Teardown)

EUNIT_TEST(
    "StoreL - test ",
    "CCalenInterimUtils2",
    "StoreL",
    "FUNCTIONALITY",
    SetupL, StoreL, Teardown)

EUNIT_TEST(
    "Store2L - test ",
    "CCalenInterimUtils2",
    "StoreL",
    "FUNCTIONALITY",
    SetupL, Store2L, Teardown)

EUNIT_TEST(
    "IsMeetingRequestL - test ",
    "CCalenInterimUtils2",
    "IsMeetingRequestL",
    "FUNCTIONALITY",
    SetupL, IsMeetingRequestL, Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
