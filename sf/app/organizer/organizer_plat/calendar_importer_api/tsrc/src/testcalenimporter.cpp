/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   test calenimporter api
*
*/



//  CLASS HEADER
#include "testcalenimporter.h"
#include "calenimptestconstants.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <calsession.h>
#include <CalEntry.h>				// CCalEntry
#include <DelimitedPathSegment8.h>
#include <UTF.H>
#include <CalenImporter.h>

// -----------------------------------------------------------------------------
// CCalenImporterTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenImporterTest* CCalenImporterTest::NewL()
    {
    CCalenImporterTest* self = CCalenImporterTest::NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenImporterTest* CCalenImporterTest::NewLC()
    {
    CCalenImporterTest* self = new( ELeave ) CCalenImporterTest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::~CCalenImporterTest()
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCalenImporterTest::~CCalenImporterTest()
    {
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::CCalenImporterTest()
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenImporterTest::CCalenImporterTest(): iImportFile( NULL ),
                          iExportFile( NULL ),
                          iCheckFile( NULL ),
                          iType( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    
    }

//============================================================================================
// Import/export ical and vcal
//============================================================================================
         

// -----------------------------------------------------------------------------
// CCalenImporterTest::SetupL
// Setup & Teardown
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::SetupL()
    {
    //Create file server session
	EUNIT_ASSERT_EQUALS(iFs.Connect(), KErrNone);
    
	//Make an output directory if needed
	iFs.MkDirAll( KOutputDirName );
    
	iCalSession = CCalSession::NewL();
    iCalSession->OpenL( iCalSession->DefaultFileNameL()  );
    
    iCalenImporter = CCalenImporter::NewL( *iCalSession );    
    }


// -----------------------------------------------------------------------------
// CCalenImporterTest::Teardown
// Teardown
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::Teardown()
    {
    if( iType )
        {
        delete iType;
        iType = NULL;
        }
    if( iImportFile )
        {
        delete iImportFile;	// File to import from
        iImportFile = NULL;
        }
    
    if( iExportFile )
        {
        delete iExportFile;	// File to export to
        iExportFile = NULL;
        }
    
    if( iCheckFile )
        {
        delete iCheckFile;	// File to verify export against
        iCheckFile = NULL;
        }
  
    iEntries.ResetAndDestroy();
      
    if( iCalenImporter )
        {
        delete iCalenImporter;
        iCalenImporter = NULL;
        }
         
    if( iCalSession )
        {
        delete iCalSession;
        iCalSession = NULL;
        }
    
    //Close File Server
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::ImportL
// Imports an iCalendar entry from iImportFile.
// @param aExportFile the file to import from. Default 0 means take from ini file.
// @internalTechnology
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::ImportL()
	{
	//iEntries.ResetAndDestroy();
	
	RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *iImportFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);
	
	if( iType->Match( KTypeVCal ) != KErrNotFound )
        {
        TRAPD( error, iCalenImporter->ImportVCalendarL( fileStream, iEntries ) );
        if(error)
        	{
        	//to avoid warning
        	}
        }
    else if( iType->Match( KTypeICal ) != KErrNotFound )
        {
        TRAPD( error, iCalenImporter->ImportICalendarL( fileStream, iEntries ) );
        if(error)
        	{
        	//to avoid warning
        	}
        }
	
	CleanupStack::PopAndDestroy(); // fileStream
	}

// -----------------------------------------------------------------------------
// CCalenImporterTest::ParseFileL
// Parses the ini file and sends the content to ParseLineL.
// @param aFile the ini file.
// @internalTechnology
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::ParseFileL( const TDesC16& aFile )	
    {
    HBufC* configFile	= HBufC::NewLC( KInputDirName().Length() + aFile.Length() );
    
    configFile->Des().Insert( 0, KInputDirName );
	configFile->Des().Append( aFile );    

    RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *configFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);

	// Buffer to hold each line of a file
	HBufC8* buf = HBufC8::NewLC(KICalMaxContentLineLength);
	TPtr8 bufPtr = buf->Des();
	
	TInt error(KErrNone);
	TBool found = EFalse;
    
    while (error == KErrNone && !found)
		{
		
		TRAP( error, fileStream.ReadL( bufPtr, TChar(KNewline) ) );
        
        if( error == KErrNone )
            {
            ParseLineL( bufPtr );
			
			RunTestL();
		    }
		}
	CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(&fileStream);
    CleanupStack::PopAndDestroy(configFile);
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::ParseLineL
// Parses a RFC2396 specified line.
// @param aLine the line to parse.
// @internalTechnology
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::ParseLineL( const TDesC8& aLine )
	{
	EUNIT_PRINT( aLine );
	/*
	TDelimitedPathSegmentParser8 Parses path segments whose 
	components are delimited by the ';' character, 
	as defined in RFC2396.
	*/
    
   	TDelimitedPathSegmentParser8 parser;
	parser.Parse( aLine );
	TPtrC8 segment;
    TPtr8 test(NULL, 0);
    
    //The line parsed from the file is expected to have the following format:
    //type;import_file_name;check_file_name;export_file_name;
    
   
   //Get type vcal/ical
    if( parser.GetNext( segment ) != KErrNotFound )
	    {
	    if( iType )
	        {
	        delete iType;
	        iType = NULL;
	        }
	    
	    iType = CnvUtfConverter::ConvertToUnicodeFromUtf8L( segment);
	    }
      	
    //Get Import file name
    if( parser.GetNext( segment ) != KErrNotFound )
	    {
	    if( iImportFile )
	        {
	        delete iImportFile;
	        iImportFile = NULL;
	        }
	    
	    iImportFile = CnvUtfConverter::ConvertToUnicodeFromUtf8L( segment);
	    iImportFile = iImportFile->ReAlloc( iImportFile->Length() + KInputDirName().Length() );
	    iImportFile->Des().Insert( 0, KInputDirName );
      	}
	
	//Get Check file name
	if( parser.GetNext( segment ) != KErrNotFound )
	    {
	    if( iCheckFile )
	        {
	        delete iCheckFile;
	        iCheckFile = NULL;
	        }
	    
	    iCheckFile = CnvUtfConverter::ConvertToUnicodeFromUtf8L( segment);
	    iCheckFile = iCheckFile->ReAlloc( iCheckFile->Length() + KInputDirName().Length() );
	    iCheckFile->Des().Insert( 0, KInputDirName);
      	}
	
	//Get Export file name
	if( parser.GetNext( segment ) != KErrNotFound )
	    {
	    if( iExportFile )
	        {
	        delete iExportFile;
	        iExportFile = NULL;
	        }
	        
	    iExportFile = CnvUtfConverter::ConvertToUnicodeFromUtf8L( segment);
	    iExportFile = iExportFile->ReAlloc( iExportFile->Length() + KOutputDirName().Length() );
	    iExportFile->Des().Insert( 0, KOutputDirName );
      	}
	}

// -----------------------------------------------------------------------------
// CCalenImporterTest::
// This function is called by ParseFileL for each 
// line in the ini file
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::RunTestL()
    {
    ImportL();    
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::ImportTestL
// 
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::ImportTestL(  )
    {
    /*
    Starts the whole test

    ImportExportTestL -->
                        ParseFileL --> (Loops through the whole file)
                                     ParseLineL
                                     RunTest    -->
                                                  ImportL();
                                                  ExportL();
                                                  CompareFilesL();
    */
    EUNIT_ASSERT_NO_LEAVE( ParseFileL( KTestIniFile ) );
    }

//============================================================================================
// specific import function test cases
//============================================================================================

// -----------------------------------------------------------------------------
// CCalenImporterTest::SetupImporterL
// Setup only importer
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::SetupImporterL()
    {
        
	  iCalSession = CCalSession::NewL();
    iCalSession->OpenL( iCalSession->DefaultFileNameL()  );
    
    iCalenImporter = CCalenImporter::NewL( *iCalSession );
    }

// -----------------------------------------------------------------------------
// CCalenImporterTest::TeardownImporter
// Teardown only importer
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::TeardownImporter()
    {
    if( iCalenImporter )
        {
        delete iCalenImporter;
        iCalenImporter = NULL;
        }
      
    if( iCalSession )
        {
        delete iCalSession;
        iCalSession = NULL;
        }
    }
    
// -----------------------------------------------------------------------------
// CCalenImporterTest::SetImportModeTestL
// SetImportMode
// -----------------------------------------------------------------------------
//
void CCalenImporterTest::SetImportModeTestL()
    {
    iCalenImporter->SetImportMode( ECalenImportModeNormal );
    iCalenImporter->SetImportMode( ECalenImportModeExtended );
    }

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    CCalenImporterTest,
    "Add test suite description here.",
    "MODULE" )

//Import and Export
EUNIT_TEST(
    "Test a bunch of iCal and vCal",
    "CCalenImporter",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupL, ImportTestL, Teardown )

//Specific import test cases
EUNIT_TEST(
    "Test setting the import mode",
    "CCalenImporter",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupImporterL, SetImportModeTestL, TeardownImporter )
          
EUNIT_END_TEST_TABLE

//  END OF FILE
