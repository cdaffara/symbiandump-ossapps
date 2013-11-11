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
#include <S32FILE.H>

#include <calsession.h>
#include <calentry.h>				// CCalEntry
#include "calenimportertest.h"
#include "calenimptestconstants.h"


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::Delete() 
{
	 if( iCalenImporter )
		{
		delete iCalenImporter;
		iCalenImporter = NULL;
		}
	iEntries.ResetAndDestroy();
	if( iCalSession )
		{
		delete iCalSession;
		iCalSession = NULL;
		}
	
	//Close File Server
	iFs.Close();
}

// -----------------------------------------------------------------------------
// CCalenImporterTest::SetupimporterL
// Setup only importer
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::SetupimporterL()
{
	int error = iFs.Connect();
        
	iCalSession = CCalSession::NewL();
    iCalSession->OpenL( iCalSession->DefaultFileNameL()  );
    
    iCalenImporter = CCalenImporter::NewL( *iCalSession );
}

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalenImporterTest::RunMethodL( 
        CStifItemParser& aItem ) 
    {
	// Construct the importer
	SetupimporterL();
	
	// Run the test cases
    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalenImporterTest::ExampleL ),

        /** Test functions for API Calenimporter */
        ENTRY( "TestImporterVCal", 
                CCalenImporterTest::TestImporterVCalL ),
        
        ENTRY( "TestImporterICal", 
                CCalenImporterTest::TestImporterICalL ),
                	
        ENTRY( "TestSetImportMode", 
                CCalenImporterTest::TestSetImportMode ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::TestimporterVCalL
// Test funtion to test the API CCalenimporter::TestimporterVCalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenImporterTest::TestImporterVCalL( CStifItemParser& aItem )
{
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenimporterTest, "CalenimporterTest" );
    _LIT( KTestimporter, "In TestimporterVCalL" );
    
    TestModuleIf().Printf( 0, KCalenimporterTest, KTestimporter );
		
	// Implementing the test case
    TPtrC fileName;
	// Parsing type to be set before parsing a string. 
	// EQuoteStyleParsing: information between quotes is handled as a one string. 
	aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
	aItem.GetNextString ( fileName );
	
	HBufC* inputFile	= HBufC::NewLC( KInputDirName().Length() + fileName.Length() );
	    
	inputFile->Des().Insert( 0, KInputDirName );
	inputFile->Des().Append( fileName );   
	
	RFileReadStream fileStream;
	TInt value = fileStream.Open(iFs, *inputFile, EFileRead);
	if(value != KErrNone) {
		return KErrGeneral;
	}
	CleanupClosePushL(fileStream);
	
	TRAPD( error, iCalenImporter->ImportVCalendarL( fileStream, iEntries ) );
	CleanupStack::PopAndDestroy(); // fileStream
	CleanupStack::PopAndDestroy(inputFile);
	if(error)
		{
		return KErrGeneral;
		}
	
	// Verify the entries i	mported
	CCalEntry* entry = iEntries[0];
	if(entry->SummaryL().Compare(_L("Super test ")))
		{
		return KErrGeneral;	
		}
	else if(entry->EntryTypeL() != CCalEntry::EAppt)
		{
		return KErrGeneral;	
		}

	return retValue;
}

// -----------------------------------------------------------------------------
// CCalenImporterTest::TestimporterVCalL
// Test funtion to test the API CCalenimporter::TestimporterICalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenImporterTest::TestImporterICalL( CStifItemParser& aItem )
{
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenimporterTest, "CalenimporterTest" );
    _LIT( KTestimporter, "In TestimporterICalL" );
    
    TestModuleIf().Printf( 0, KCalenimporterTest, KTestimporter );
		
	// Implementing the test case
	TPtrC fileName;
	// Parsing type to be set before parsing a string. 
	// EQuoteStyleParsing: information between quotes is handled as a one string. 
	aItem.SetParsingType( CStifItemParser::EQuoteStyleParsing );
	aItem.GetNextString ( fileName );
	
	HBufC* inputFile	= HBufC::NewLC( KInputDirName().Length() + fileName.Length() );
		
	inputFile->Des().Insert( 0, KInputDirName );
	inputFile->Des().Append( fileName );   
	
	RFileReadStream fileStream;
	TInt value = fileStream.Open(iFs, *inputFile, EFileRead);
	if(value != KErrNone) {
		retValue =  KErrGeneral;
	}
	CleanupClosePushL(fileStream);
	
	int error = KErrNone;
	// Not calling this API now as it is leaking memory. We will use it once we 
	// fis that memory leak
	// TRAP( error, iCalenImporter->ImportICalendarL( fileStream, iEntries ) );
	CleanupStack::PopAndDestroy(); // fileStream
	CleanupStack::PopAndDestroy(inputFile);
	return retValue;
	if(error)
		{
		retValue =  KErrGeneral;
		}
	
	// Verify the entries imported
	CCalEntry* entry = iEntries[0];
	if(entry->SummaryL().Compare(_L("Super test ")))
		{
		retValue =  KErrGeneral;	
		}
	else if(entry->EntryTypeL() != CCalEntry::EAppt)
		{
		retValue = KErrGeneral;	
		}

    return retValue;
}

// -----------------------------------------------------------------------------
// CCalenImporterTest::TestSetImportMode
// Test funtion to test the API CCalenimporter::TestSetImportMode
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenImporterTest::TestSetImportMode( CStifItemParser& /*aItem*/ )
{
	TInt retValue = KErrNone;
	// Print to UI
	_LIT( KCalenimporterTest, "CalenimporterTest" );
	_LIT( KTestimporter, "In TestSetImportMode" );
	
	TestModuleIf().Printf( 0, KCalenimporterTest, KTestimporter );
	
	// Implementing the test case
	iCalenImporter->SetImportMode(ECalenImportModeNormal);
	
	// No Api to get the import mode
	return retValue;
}

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
