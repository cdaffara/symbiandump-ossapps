// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiUserInt.h"
#include "TestCalInterimApiDefines.h"

#include <calentry.h>
#include <calsession.h>
#include <calinstance.h>
#include <caldataexchange.h>
#include <caldataformat.h>

_LIT( KUserIntTestCal, "c:userinttest.dat" );
_LIT( KExportFile,     "c:\\userintExportTest.vcs" );
_LIT( KInvalidVcsFile, "\\CITCalInterimApi\\InvalidUserInt.vcs" );
_LIT8( KEntryUid,  "EntryOne" );
_LIT8( KEntryUid2, "EntryTwo" );

TFileName GetFullFileName(const TDesC& aPath)
	{
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(aPath);
	return fileName;
	}

CUserIntTestStep::CUserIntTestStep()
 	{
 	SetTestStepName( KUserIntTestStepName );
 	}

CUserIntTestStep::~CUserIntTestStep()
	{
	delete iEntryView;
	delete iDataExchange;
	delete iSession;
	delete iScheduler;
	}

TVerdict CUserIntTestStep::doTestStepPreambleL()
    {
	TVerdict ret = CTestStep::doTestStepPreambleL();

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );
    
    iSession = CCalSession::NewL();
    OpenCalendarFileL();

    iEntryView = CCalEntryView::NewL( *iSession );
    iDataExchange = CCalDataExchange::NewL( *iSession );

    return ret;
	}	

TVerdict CUserIntTestStep::doTestStepL()
    {
    // Tests storage and retrieval, importing, exporting with positive user 
    // integer.
    TUint32 userInt1 = 0xffffffff;
    StoreAndRetrieveEntryL( KEntryUid(), userInt1 );
    ExportEntryL( KEntryUid() );
    ImportEntryL( KExportFile(), userInt1 );
    
    // Change the user integer of the entry just added.
    TInt newUserInt = 1000;
    StoreAndRetrieveEntryL( KEntryUid(), newUserInt );

    // Tests storage and retrieval, importing, exporting with negative user
    // integer. The exported value should be unsigned. Versit only supports 
    // signed ints so the user int is exported as a string instead. Versit 
    // doesn't support importing values with a '-' sign even though the type 
    // is a signed int, and if a negative value is imported, the value gets 
    // set to 0. The value is imported as an integer and implicitely converted
    // to a signed int. This tests that negative values aren't exported.
    TInt negUserInt = -500;
    StoreAndRetrieveEntryL( KEntryUid2(), negUserInt );
    ExportEntryL( KEntryUid2() );
    ImportEntryL( KExportFile(), negUserInt );
    
    // Imports an existing vcs with a negative user integer. Negative values
    // are invalid and the user int should remain the default value of 0.
    
    
    
    ImportEntryL( GetFullFileName(KInvalidVcsFile), 0 );

    // Test copying of entries.
    TestCopyEntriesL();

    // Test comparing entries.
    TestCompareL();

    return TestStepResult();
	}

TVerdict CUserIntTestStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

void CUserIntTestStep::OpenCalendarFileL( TBool aReplaceIfExists )
	{
	TBuf<256> buffer;
	TRAPD( err, iSession->CreateCalFileL( KUserIntTestCal ) );
	if (err == KErrAlreadyExists && aReplaceIfExists)
		{
		iSession->DeleteCalFileL( KUserIntTestCal );
		iSession->CreateCalFileL( KUserIntTestCal );	
		}
	iSession->OpenL( KUserIntTestCal() );
	}

/**
Store and retrieve user integer.  A calendar entry's user integer is set and 
the entry is stored. The same entry is then retrieved from the database. The 
user integer of the retrieved entry should match the entry that was set.

@param aUid The uid of the entry to store. To test changing an entry, the uid 
  of an already stored entry is specified for aUid.
@param aUserIntVal The user integer value for the entry.
*/
void CUserIntTestStep::StoreAndRetrieveEntryL( const TDesC8& aUid, 
    TUint32 aUserIntVal )
    {
    INFO_PRINTF2( _L("Creating and storing an entry. User int=%u"),
        aUserIntVal);

    CCalEntry* entry = CreateEntryL( aUid, aUserIntVal );
    CleanupStack::PushL( entry );

    // Store the entry.
    RPointerArray<CCalEntry> entriesToStore;
    entriesToStore.Append( entry );
    TInt numEntriesStored = 0;
    iEntryView->StoreL( entriesToStore, numEntriesStored );
    ASSERT( numEntriesStored == 1 );

    CleanupStack::PopAndDestroy( entry );
    
    ReOpenSession();

    // Verify entry was stored by retrieving it.    
    RetrieveEntryL( aUid, aUserIntVal );
    }

/** 
Tests that the user integer was stored in the calendar database and
can be retrieved again. The entry is first stored using 
CreateAndStoreEntryL(). The user integer is checked against the expected
value once retrieved.

@param aUid The uid of the entry to retrieve and was previously stored.
@param aUserIntVal The expected user integer value of the entry.
*/
void CUserIntTestStep::RetrieveEntryL( const TDesC8& aUid, 
    TUint32 aExpectedUserInt )
    {
    INFO_PRINTF2( _L("Retrieving entry. Expected user int = %u"), 
        aExpectedUserInt);

    // Retrieve the entry.
    RPointerArray<CCalEntry> entriesStored;
    CleanupStack::PushL( &entriesStored );
    iEntryView->FetchL( aUid, entriesStored );
    ASSERT( entriesStored.Count() == 1 );

    // Check the user integer.
    CCalEntry* entry = entriesStored[0];
    TUint32 restoredUserInt = entry->UserIntL();
    if (restoredUserInt != aExpectedUserInt )
        {
        INFO_PRINTF1( _L("Test failed: Retrieved user int not expected \
            value." ) );
        SetTestStepResult( EFail );
        }

    CleanupStack::Pop( &entriesStored );
    entriesStored.ResetAndDestroy();
    }

/**
An entry from the calendar database is retrieved and then exported.This is used
with ImportEntryL() to fully verify the entry was exported.

@param aUid The uid of an entry existing in the database to be retrieved
    and then exported.
*/
void CUserIntTestStep::ExportEntryL( const TDesC8& aUid )
    {
    INFO_PRINTF1( _L("Exporting entry.") );
    
    // Retrieve the entry.
    RPointerArray<CCalEntry> entriesStored;
    CleanupStack::PushL( &entriesStored );
    iEntryView->FetchL( aUid, entriesStored );
    ASSERT( entriesStored.Count() == 1 );

    // Create the export file.
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
	RFile outFile;
	TInt err = outFile.Replace( fs, KExportFile, EFileWrite );
	User::LeaveIfError( err );

    // Export the entry that was retrieved.
    RFileWriteStream writeStream( outFile );
    CleanupClosePushL( writeStream );
    iDataExchange->ExportL( KUidVCalendar, writeStream, entriesStored );
    writeStream.CommitL();

    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( &fs );
    CleanupStack::Pop( &entriesStored );
    entriesStored.ResetAndDestroy();
    }

/**
An exported vCal that has an X-SYMBIAN-USERINT property is  imported and the 
user integer of the entry is checked. The user integer of the entry should 
match the value of the exported X-SYMBIAN-USERINT property.
 
@param aExpectedUserInt The expected value of the imported user integer.
*/
void CUserIntTestStep::ImportEntryL( const TDesC& aFileName, 
	TUint32 aExpectedUserInt )
    {
    INFO_PRINTF2( _L("Importing entry. Expected user int = %u"), 
        aExpectedUserInt );
    
    RFs fs;
    User::LeaveIfError( fs.Connect() );	
    CleanupClosePushL( fs );
    
    // Open the file to import.
	RFile inFile;
	inFile.Open( fs, aFileName, EFileRead );
    RFileReadStream readStream( inFile );
    CleanupClosePushL( readStream );
    RPointerArray<CCalEntry> importedEntries;
    CleanupStack::PushL( &importedEntries );

    // Import the file that was previously exported.
    iDataExchange->ImportL( KUidVCalendar, readStream, importedEntries );
    ASSERT( importedEntries.Count() == 1 );
    
    // Verify that the user integer is the expected value.
    CCalEntry* importedEntry = importedEntries[0];
    TUint32 userInt = importedEntry->UserIntL();
    if (userInt != aExpectedUserInt)
        {
        INFO_PRINTF1( _L("Test failed: Imported user int not expected \
            value." ) );
        SetTestStepResult( EFail );
        }

    CleanupStack::Pop( &importedEntries );
    importedEntries.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PopAndDestroy( &fs );
    }

/**
Creates a new entry. The only configurable field is the user integer.

@param aUserInt The value of the user integer.
@return The entry that was created.
*/   
CCalEntry* CUserIntTestStep::CreateEntryL( const TDesC8& aUid, TUint32 aUserInt )
    {
	HBufC8* guid = aUid.AllocLC();

    CCalEntry* entry = CCalEntry::NewL( CCalEntry::EEvent, guid,
        CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop( guid );
	CleanupStack::PushL( entry );

    TTime startTime(TDateTime(2007, EFebruary, 1, 9, 0, 0, 0));
    TCalTime calStartTime;
    calStartTime.SetTimeLocalL( startTime );
    entry->SetStartAndEndTimeL( calStartTime, calStartTime );
    entry->SetSummaryL( _L("user int test entry") );

    // Set the user integer.
    entry->SetUserIntL( aUserInt );

    CleanupStack::Pop( entry );
    return entry;
    }

/**
Copy an entry. An entry is created and the user integer set. The entry is 
then copied to a new entry.The entries should not be considered the same.
*/
void CUserIntTestStep::TestCopyEntriesL()
    {
    TUint32 userInt = 9999999;
    INFO_PRINTF2( _L("Testing CopyFromL. User int = %u"), userInt );
    
    // Create two entries with different user integer values.
    CCalEntry* srcEntry = CreateEntryL( KEntryUid(), userInt );
    CleanupStack::PushL( srcEntry );
    CCalEntry* destEntry = CreateEntryL( KEntryUid(), 0 );
    CleanupStack::PushL( destEntry );

    // User integer values should be different.
    TUint32 srcUserInt = srcEntry->UserIntL();
    TUint32 destUserInt = destEntry->UserIntL();
    ASSERT( srcUserInt != destUserInt );

    // Copy destination entry to the source and compare the values. They
    // should now be the same.
    destEntry->CopyFromL( *srcEntry );
    srcUserInt = srcEntry->UserIntL();
    destUserInt = destEntry->UserIntL();
    
    if (srcUserInt != destUserInt)
        {
        INFO_PRINTF1( _L("Compare entries failed. User int values on src/dest \
            entries not equal." ) );
        SetTestStepResult( EFail );
        }

    CleanupStack::PopAndDestroy( destEntry );
    CleanupStack::PopAndDestroy( srcEntry );
    }

/**
Compare entries with different user integer values. An entry with a previously 
set user integer value is copied to another entry. The user integer of the 
destination entry is then changed and the entries are compared. The entries 
should not be considered the same.
*/
void CUserIntTestStep::TestCompareL()
    {
    TUint32 srcUserInt = 999999;    
    INFO_PRINTF2( _L("Testing CompareL: User int=%u"),  srcUserInt );

    // Create two entries with different user integer values.
    CCalEntry* srcEntry = CreateEntryL( KEntryUid(), srcUserInt );
    CleanupStack::PushL( srcEntry );
    CCalEntry* destEntry = CreateEntryL( KEntryUid(), 0 );
    CleanupStack::PushL( destEntry );
    
    // Compare src and dest. They should not be equal.
    TBool isDestEqual = srcEntry->CompareL( *destEntry );
    if (isDestEqual)
        {
        INFO_PRINTF1( _L("Compare test failed. Entries should not be equal") );
        SetTestStepResult( EFail );
        }

    // Make user integer same in both src and dest entries then compare.
    destEntry->SetUserIntL( srcUserInt );
    isDestEqual = srcEntry->CompareL( *destEntry );
    if (!isDestEqual)
        {
        INFO_PRINTF1( _L("Compare test failed. Entries should be equal") );
        SetTestStepResult( EFail );
        }

    CleanupStack::PopAndDestroy( destEntry );
    CleanupStack::PopAndDestroy( srcEntry );
    }
    
/**
Re-opens the session to the agenda server to ensure that the server restarts.
*/
void CUserIntTestStep::ReOpenSession()
	{
	delete iEntryView;
	delete iDataExchange;
	delete iSession;
	iEntryView = NULL;
	iDataExchange = NULL; 
	iSession = NULL;
	
    iSession = CCalSession::NewL();
    
    // Open calendar file again but don't delete it.
    OpenCalendarFileL( EFalse );

    iEntryView = CCalEntryView::NewL( *iSession );
    iDataExchange = CCalDataExchange::NewL( *iSession );
	}
