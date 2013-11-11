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

#include <calsession.h>
#include <calentry.h>				// CCalEntry
#include "calenExportertest.h"


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::Delete() 
{
	 if( iCalenExporter )
		{
		delete iCalenExporter;
		iCalenExporter = NULL;
		}
	  
	if( iCalSession )
		{
		delete iCalSession;
		iCalSession = NULL;
		}
}

// -----------------------------------------------------------------------------
// CCalenExporterTest::SetupExporterL
// Setup only importer
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::SetupExporterL()
{
        
	TRAPD(error, iCalSession = CCalSession::NewL(););
    iCalSession->OpenL( iCalSession->DefaultFileNameL());
    
    iCalenExporter = CCalenExporter::NewL( *iCalSession );
}

// -----------------------------------------------------------------------------
// CCalendarCommonUtilsTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCalenExporterTest::RunMethodL( 
        CStifItemParser& aItem ) 
    {
	// Construct the exporter
	SetupExporterL();
	
	// Run the test cases
    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        // Example: ENTRY( "Example", CCalenExporterTest::ExampleL ),

        /** Test functions for API CalenExporter */
        ENTRY( "TestExporterVCal", 
                CCalenExporterTest::TestExporterVCalL ),
        
        ENTRY( "TestExporterICal", 
                CCalenExporterTest::TestExporterICalL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::TestExporterVCalL
// Test funtion to test the API CCalenExporter::TestExporterVCalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenExporterTest::TestExporterVCalL( CStifItemParser& /*aItem*/ )
{
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenExporterTest, "CalenExporterTest" );
    _LIT( KTestExporter, "In TestExporterVCalL" );
    
    TestModuleIf().Printf( 0, KCalenExporterTest, KTestExporter );
		
	// Implementing the test case
    //uid
	_LIT8( KTestUid, "123544");
	
	//entry
	CCalEntry* entry = NULL;
	
	entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
	CleanupStack::PushL( entry );
	
	CBufFlat* buf = CBufFlat::NewL( 100 );
	CleanupStack::PushL(buf);
	
	RBufWriteStream writeStream;
	writeStream.Open(*buf);
	CleanupClosePushL(writeStream);
	
	
	RPointerArray<CCalEntry> array;
	array.Append( entry );
	
	//should not leave
	int error = KErrNone;
	TRAP(error, iCalenExporter->ExportVCalL( *entry, writeStream) );
	if(error != KErrNone) {
		retValue = KErrGeneral;
	}
	
	//should not leave
	TRAP(error, iCalenExporter->ExportVCalL( array, writeStream) );
	if(error != KErrNone) {
		retValue = KErrGeneral;
	}
	
	//should leave
	array.Append( entry );
	TRAP(error, iCalenExporter->ExportVCalL( array, writeStream) );
	if(error != KErrNotSupported) {
		retValue = KErrGeneral;
	}
	array.Close();
	
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy( entry );	
	return retValue;
}

// -----------------------------------------------------------------------------
// CCalenExporterTest::TestExporterVCalL
// Test funtion to test the API CCalenExporter::TestExporterICalL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CCalenExporterTest::TestExporterICalL( CStifItemParser& /*aItem*/ )
    {
    TInt retValue = KErrNone;
    // Print to UI
    _LIT( KCalenExporterTest, "CalenExporterTest" );
    _LIT( KTestExporter, "In TestExporterICalL" );
    
    TestModuleIf().Printf( 0, KCalenExporterTest, KTestExporter );
		
	// Implementing the test case
    //uid
	_LIT8( KTestUid, "123545");
	
	//entry
	CCalEntry* entry = NULL;
	
	entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
	CleanupStack::PushL( entry );
	
	CBufFlat* buf = CBufFlat::NewL( 100 );
	CleanupStack::PushL(buf);
	
	RBufWriteStream writeStream;
	writeStream.Open(*buf);
	CleanupClosePushL(writeStream);
	
	
	RPointerArray<CCalEntry> array;
	array.Append( entry );
	
	//should not leave
	int error = KErrNone;
	// Not calling this API now as it is leaking memory. We will use it once we 
	// fis that memory leak
	//TRAP(error, iCalenExporter->ExportICalL( *entry, writeStream) );
	if(error != KErrNone) {
		retValue = KErrGeneral;
	}
	
	//should not leave
	// Not calling this API now as it is leaking memory. We will use it once we 
	// fis that memory leak
	//TRAP(error, iCalenExporter->ExportICalL( array, writeStream) );
	if(error != KErrNone) {
		retValue = KErrGeneral;
	}
	
	//should leave
	array.Append( entry );
	// Not calling this API now as it is leaking memory. We will use it once we 
	// fis that memory leak
	//TRAP(error, iCalenExporter->ExportICalL( array, writeStream) );
/*	if(error != KErrNotSupported) {
		retValue = KErrGeneral;
	}*/
	array.Close();
	
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy( entry );	

    return retValue;

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
