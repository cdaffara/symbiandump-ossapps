/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        ?Description
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "BCTestCalendar2.h"



// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
_LIT(KCreateSessionMsg, "Creating Cal Session\n");
_LIT(KCreateEViewMsg,   "Creating Entry view\n");
_LIT(KCreateIViewMsg,   "Creating Instance view\n");
_LIT(KCreateUtilsMsg,   "Creating InterimUtils\n");
_LIT(KNewLine,          "\n");
_LIT(KUidGenMsg,        "Generating %d Uids\n");
_LIT(KUidGenOk,         "UID generation OK\n\n");
_LIT(KUidGenErrorMsg,   "ERROR: Duplicate uid generated\n");
_LIT(KMREnabledMsg,     "\nMeeting Request support enabled\n\n");
_LIT(KMRDisabledMsg,    "\nMeeting Request support disabled\n\n");
_LIT(KApptSummary,      "Summary");
_LIT(KExText,           "EXEXEX");
_LIT(KOrganizer,        "a@b.com");
_LIT(KIsMrTestOk,       "Is Meeting Request Tests OK\n");
_LIT(KStaticMrTest,     "Static Is Meeting Request Test OK\n");
_LIT(KIsMrTestFail,     "Is Meeting Request Tests Failed\n");
_LIT(KPopTestOk,        "Populate child from parent ok\n\n");
_LIT(KPopTestFail,      "Populate child from parent failed\n\n");
_LIT(KStoreParentMsg,   "Storing Parent Entry...");
_LIT(KStoreChildMsg,    "Storing Child Entry...");
_LIT(KStoreFail,        "\nStoreL Test Fail\n");
_LIT(KOkMsg,            " OK\n");

_LIT( KCalendarDatabaseFilePath, "c:Calendar" );

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBCTestCalendar2::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CBCTestCalendar2::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CBCTestCalendar2::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCTestCalendar2::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", CBCTestCalendar2::ExampleL ),
        ENTRY( "TestNew", CBCTestCalendar2::TestNewL ),
        ENTRY( "TestMRViewEnabled", CBCTestCalendar2::TestMRViewEnabledL ),
        ENTRY( "TestGlobalUidGen", CBCTestCalendar2::TestGlobalUidGenL ),
        ENTRY( "TestIsMeetingRequest", CBCTestCalendar2::TestIsMeetingRequestL ),
        ENTRY( "TestPopulateChild", CBCTestCalendar2::TestPopulateChildL ),
        ENTRY( "TestStaticPopulateChild", CBCTestCalendar2::TestStaticPopulateChildL ),
        ENTRY( "TestStore", CBCTestCalendar2::TestStoreL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCTestCalendar2::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCTestCalendar2::ExampleL( CStifItemParser& aItem )
    {

	    // Print to UI
	    _LIT( KBCTestCalendar2, "BCTestCalendar2" );
	    _LIT( KExample, "In Example" );
	    TestModuleIf().Printf( 0, KBCTestCalendar2, KExample );
	    // Print to log file
	    iLog->Log( KExample );
	
	    TInt i = 0;
	    TPtrC string;
	    _LIT( KParam, "Param[%i]: %S" );
	    while ( aItem.GetNextString ( string ) == KErrNone )
	        {
	        TestModuleIf().Printf( i, KBCTestCalendar2, 
	                                KParam, i, &string );
	        i++;
	        }
	
	    return KErrNone;

    }

TInt CBCTestCalendar2::TestNewL( CStifItemParser& aItem )
    {
				CCalenInterimUtils2*		calUtils = CCalenInterimUtils2::NewL();
				
	    	if(calUtils)
	    		{
	    				delete calUtils;
	    				calUtils = NULL;
	    		}
	
	    	return KErrNone;

    }
    
TInt CBCTestCalendar2::TestMRViewEnabledL( CStifItemParser& aItem )
    {
			
			TBool enabled = iUtils->MRViewersEnabledL();
			enabled = iUtils->MRViewersEnabledL(ETrue);
    	return KErrNone;

    }
    
TInt CBCTestCalendar2::TestGlobalUidGenL( CStifItemParser& aItem )
    {
			TInt aUidCount = 10;
			TBuf<50> msg;
			msg.Format(KUidGenMsg, aUidCount);
			
			CDesC8ArrayFlat* uidArray = new (ELeave) CDesC8ArrayFlat(aUidCount);

	    HBufC8* uid8;   //for uid
	    HBufC* uid16;   //for output to console
	
			for(TInt count=0; count < 2; count++)
			{
	    for (TInt x = 0; x < aUidCount; ++x)
	        {
	        
	            uid8 = iUtils->GlobalUidL();
	        
	        CleanupStack::PushL(uid8);
	        
	        //Output the new uid to console
	        uid16 = HBufC::NewL(uid8->Length());
	        CleanupStack::PushL(uid16);
	        
	        TPtr ptr = uid16->Des();
	        ptr.Copy(*uid8);
	        
	        CleanupStack::PopAndDestroy(uid16);
	        uid16 = NULL;
	        
	        // Check the uid doesn't already exist in the array
	        TInt index;
	        TInt findResult = uidArray->Find(*uid8, index);
	        if (findResult != 0 )   //0 indicates a match
	            {
	            uidArray->AppendL(*uid8);
	            }
				  else
				  	{
				  		uidArray->Reset();
	            CleanupStack::PopAndDestroy(uid8);
	            uid8 = NULL;
	            User::Leave(KErrAlreadyExists);
	          }
	        CleanupStack::PopAndDestroy(uid8);
	        uid8 = NULL;
	        
	        uidArray->Reset();
	        }  
	     }
			
   		 return KErrNone;

    }
    

TInt CBCTestCalendar2::TestIsMeetingRequestL( CStifItemParser& aItem )
    {
			
			CCalEntry* entry = CreateNonRepeatingApptL();
    	 	CleanupStack::PushL(entry);

    	TBool isMeetingRequest = iUtils->IsMeetingRequestL(*entry);
    	 
    	if (isMeetingRequest)
    			User::Leave(KErrCorrupt);

			CCalUser* organizer = CCalUser::NewL(KOrganizer);
	    CleanupStack::PushL(organizer);
	    entry->SetOrganizerL(organizer); //Passing ownership
	    CleanupStack::Pop(organizer);
	    
	    isMeetingRequest = iUtils->IsMeetingRequestL(*entry);
	    
	    if (!isMeetingRequest)
	    		User::Leave(KErrCorrupt);
	    
	    isMeetingRequest = CCalenInterimUtils2::IsMeetingRequestL(*entry);
	    
	    if (!isMeetingRequest)
	    		User::Leave(KErrCorrupt);
	    	 
	    CleanupStack::PopAndDestroy(entry);
	  	return KErrNone;

    }  
    
TInt CBCTestCalendar2::TestPopulateChildL( CStifItemParser& aItem )
    {
			
			for(TInt count=0; count < 2; count++)
			{
			// Create an entry (parent).  This should really be a repeating appointment,
	    // but as we're not storing the entries, we can use CCalEntry::CompareL for
	    // verification.  This would fail if the parent was repeating however.
	    CCalEntry* parent = CreateNonRepeatingApptL();
	    CleanupStack::PushL(parent);
	    
	    // Create an entry (child)
			CCalEntry* child = CCalEntry::NewL( parent->EntryTypeL(),
	                                        parent->UidL().AllocL(),
	                                        parent->MethodL(),
	                                        parent->SequenceNumberL());//,
	                                        //parent->StartTimeL(),
	                                        //CalCommon::EThisOnly);
	    CleanupStack::PushL(child);
	    
	    iUtils->PopulateChildFromParentL(*child, *parent);
	    
	    child->SetStartAndEndTimeL(parent->StartTimeL(), parent->EndTimeL());
	    
	    if (!(parent->CompareL(*child)))
	        {
	           User::Leave(KErrCorrupt);
	        }
	        CleanupStack::PopAndDestroy(child);
	    		CleanupStack::PopAndDestroy(parent);
	    }
	    		return KErrNone;
	  }
    
TInt CBCTestCalendar2::TestStaticPopulateChildL( CStifItemParser& aItem )
    {
			
			for(TInt count=0; count < 2; count++)
			{
			// Create an entry (parent).  This should really be a repeating appointment,
	    // but as we're not storing the entries, we can use CCalEntry::CompareL for
	    // verification.  This would fail if the parent was repeating however.
	    CCalEntry* parent = CreateNonRepeatingApptL();
	    CleanupStack::PushL(parent);
	    
	    // Create an entry (child)
			CCalEntry* child = CCalEntry::NewL( parent->EntryTypeL(),
	                                        parent->UidL().AllocL(),
	                                        parent->MethodL(),
	                                        parent->SequenceNumberL());//,
	                                        //parent->StartTimeL(),
	                                        //CalCommon::EThisOnly);
	    CleanupStack::PushL(child);
	    

	        CCalenInterimUtils2::PopulateChildFromParentL(*child, *parent);
	    
	    child->SetStartAndEndTimeL(parent->StartTimeL(), parent->EndTimeL());
	    
	    if (!(parent->CompareL(*child)))
	        {
	           User::Leave(KErrCorrupt);
	        }
	        CleanupStack::PopAndDestroy(child);
	    		CleanupStack::PopAndDestroy(parent);
	    }
			return KErrNone;
    }

TInt CBCTestCalendar2::TestStoreL( CStifItemParser& aItem )
    {
			for(TInt count=0; count < 2; count++)
			{
				// Create a repeating entry
	    CCalEntry* repEntry = CreateRepeatingApptL();
	    CleanupStack::PushL(repEntry);
	       
	    //Keep the UID hanging around, we need this
	    HBufC8* uid = repEntry->UidL().AllocLC();
	    
	    TBool aUseStatic = EFalse;
	    
	    //Store the entry
	        if (!aUseStatic)
	        {
	        iUtils->StoreL(*iEntryView, *repEntry);
	        }
	    else
	        {
	        CCalenInterimUtils2::StoreL(*iEntryView, *repEntry);
	        }
	    
	    //Fetch the entry by uid.  We should see exactly 1 entry returned.
	    iEntryView->FetchL(*uid, iEntries);
	    
	    TInt entryCount = iEntries.Count();
	    if (entryCount != 1)
	        {
	          User::Leave(KErrCorrupt);
	        }
	
	    //Create an exception to our repeating entry
	    CCalEntry* exception = CreateExceptionL(*repEntry);
	    CleanupStack::PushL(exception);
	    
	    //Store the exception in the database
	    if (!aUseStatic)
	        {
	        iUtils->StoreL(*iEntryView, *exception);
	        }
	    else
	        {
	        CCalenInterimUtils2::StoreL(*iEntryView, *exception);
	        }
	    
	    //Clear the entry array
	    iEntries.ResetAndDestroy();  
	    entryCount = iEntries.Count();
	        
	    //Fetch the entry by uid.  We should see exactly 2 entries returned (one parent, one child)
	    iEntryView->FetchL(*uid, iEntries);
	         
	    entryCount = iEntries.Count();
	    if (entryCount != 2)
	        {
	         User::Leave(KErrCorrupt);
	        }
	    
	    //Clear the entry array
	    iEntries.ResetAndDestroy();  
	    entryCount = iEntries.Count();
	    iEntryView->DeleteL(*repEntry);
	    //Fetch the entry by uid.  
	    iEntryView->FetchL(*uid, iEntries);
	         
	    entryCount = iEntries.Count();
	    
	    CleanupStack::PopAndDestroy(exception);
	    CleanupStack::PopAndDestroy(uid);
	    CleanupStack::PopAndDestroy(repEntry);
	    aUseStatic = ETrue;
		  }	
    return KErrNone;

   }
        
CCalEntry* CBCTestCalendar2::CreateNonRepeatingApptL()
	{
	//Start times for Appt
	TTime now;
	now.UniversalTime();
	TCalTime startTime;
	startTime.SetTimeUtcL(now);
	User::After(100000);
    HBufC8* uid = iUtils->GlobalUidL();
    CleanupStack::PushL(uid); 
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(uid);  
    CleanupStack::PushL(entry);
    entry->SetStartAndEndTimeL(startTime,startTime);
	entry->SetSummaryL(KApptSummary);
	entry->CategoryListL();
	CleanupStack::Pop(entry);
	
	return entry;
	}

CCalEntry* CBCTestCalendar2::CreateRepeatingApptL()
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
	
void CBCTestCalendar2::CreateSessionAndViewsL()
	{
	iAsWait = new (ELeave) CActiveSchedulerWait();
	
  iCalSession = CCalSession::NewL();

    TRAPD( err, iCalSession->OpenL(KCalendarDatabaseFilePath));
    if ( err == KErrNotFound )
        {
        iCalSession->CreateCalFileL(KCalendarDatabaseFilePath);
        iCalSession->OpenL(KCalendarDatabaseFilePath);
        }
    else
        {
        User::LeaveIfError( err );
        }
    
//    iCalProg = new (ELeave) CBCTestCalProg;
	iEntryView = CCalEntryView::NewL( *iCalSession, *this );
    iAsWait->Start();   // Blocks until Complete is called
    User::LeaveIfError(iError);
	iUtils = CCalenInterimUtils2::NewL();
	  	}

CCalEntry* CBCTestCalendar2::CreateExceptionL(const CCalEntry& aParent)
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
	
/**
CBCTestCalProg::Completed
From MCalProgressCallback.
*/
void CBCTestCalendar2::Completed(TInt aStatus)
    {
    
    iError = aStatus;
    iAsWait->AsyncStop();
    }

/**
CBCTestCalProg::Progress
From MCalProgressCallback. Intentionally empty.
*/
void CBCTestCalendar2::Progress( TInt /*aPercentageCompleted*/ )
    {}

/**
CBCTestCalProg::NotifyProgress
From MCalProgressCallback. Don't notify us about progress updates.
*/
TBool CBCTestCalendar2::NotifyProgress()
    {
    return EFalse;
    }
    
// End of file

// -----------------------------------------------------------------------------
// CBCTestCalendar2::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCTestCalendar2::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
