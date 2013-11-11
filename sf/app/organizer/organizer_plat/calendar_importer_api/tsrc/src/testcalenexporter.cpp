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
* Description:   test calenexporter api
*
*/



//  CLASS HEADER
#include "testcalenexporter.h"


//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <calsession.h>
//#include <CalEntry.h>				// CCalEntry

#include <DelimitedPathSegment8.h>
#include <UTF.H>

#include <CalenExporter.h>

//  INTERNAL INCLUDES

// -----------------------------------------------------------------------------
// CCalenExporterTest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenExporterTest* CCalenExporterTest::NewL()
    {
    CCalenExporterTest* self = CCalenExporterTest::NewLC();
    CleanupStack::Pop();

    return self;
    }
    
// -----------------------------------------------------------------------------
// CCalenExporterTest::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCalenExporterTest* CCalenExporterTest::NewLC()
    {
    CCalenExporterTest* self = new( ELeave ) CCalenExporterTest();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::~CCalenExporterTest
// Destructor (virtual by CBase)
// -----------------------------------------------------------------------------
//
CCalenExporterTest::~CCalenExporterTest()
    {
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::NewL
// Default constructor
// -----------------------------------------------------------------------------
//
CCalenExporterTest::CCalenExporterTest()
    {
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::ConstructL
// Second phase construct
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }
    
// specific export function test cases

// -----------------------------------------------------------------------------
// CCalenExporterTest::SetupExporterL
// Setup only importer
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::SetupExporterL()
    {
        
	iCalSession = CCalSession::NewL();
    iCalSession->OpenL( iCalSession->DefaultFileNameL()  );
    
    iCalenExporter = CCalenExporter::NewL( *iCalSession );
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::TeardownExporter
// Teardown only importer
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::TeardownExporter()
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
// CCalenExporterTest::GetExportFlagsTestL
// Get export flags
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::GetExportFlagsTestL()
    {
    //uid
    _LIT8( KTestUid, "123544");
    
    //entry
    CCalEntry* entry = NULL;
    
    //=========================================
    // Type == EEvent
    //=========================================
    //Method = EMethodNone
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodNone, 0 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodPublish
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodPublish, 1 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodRequest
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodRequest, 2 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodReply
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodReply, 3 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodDeclineCounter
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodDeclineCounter, 4 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodCancel
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodCancel, 5 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodRefresh
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodRefresh, 6 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodCounter
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodCounter, 7 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodDeclineCounter
    entry = CCalEntry::NewL(CCalEntry::EEvent, KTestUid().AllocL(), CCalEntry::EMethodDeclineCounter, 8 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //=========================================
    // Type == ETodo
    //=========================================
    //Method = EMethodNone
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodNone, 9 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodPublish
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodPublish, 10 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodRequest
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodRequest, 11 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodReply
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodReply, 12 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodDeclineCounter
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodDeclineCounter, 13 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodCancel
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodCancel, 14 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodRefresh
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodRefresh, 15 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodCounter
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodCounter, 16 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    
    //Method = EMethodDeclineCounter
    entry = CCalEntry::NewL(CCalEntry::ETodo, KTestUid().AllocL(), CCalEntry::EMethodDeclineCounter, 17 );
    CleanupStack::PushL( entry );
    GetExportFlagsTestL( *entry ); 
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::GetExportFlagsTestL
// Get export flags test
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::GetExportFlagsTestL( CCalEntry& aEntry )
    {
    CBufFlat* buf = CBufFlat::NewL( 100 );
    CleanupStack::PushL(buf);
    
    RBufWriteStream writeStream;
    writeStream.Open(*buf);
    CleanupClosePushL(writeStream);
    
    iCalenExporter->ExportICalL( aEntry, writeStream);
    
    CleanupStack::PopAndDestroy(&writeStream);
    CleanupStack::PopAndDestroy(buf);
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::ExportICalTestL
// Test exporting ICAL
// -----------------------------------------------------------------------------
//
void CCalenExporterTest::ExportICalTestL()
    {
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
    EUNIT_ASSERT_NO_LEAVE( iCalenExporter->ExportICalL( array, writeStream) );
    
    array.Append( entry );
    
    //should leave with KErrNotSupported
    EUNIT_ASSERT_SPECIFIC_LEAVE( iCalenExporter->ExportICalL( array, writeStream), KErrNotSupported );
    
    array.Close();
    
    CleanupStack::PopAndDestroy(&writeStream);
    CleanupStack::PopAndDestroy(buf);
    
    
    CleanupStack::PopAndDestroy( entry );
    }

// -----------------------------------------------------------------------------
// CCalenExporterTest::ExportVCalTestL
// Test exporting ICAL
// -----------------------------------------------------------------------------
//    
void CCalenExporterTest::ExportVCalTestL()
    {
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
    EUNIT_ASSERT_NO_LEAVE( iCalenExporter->ExportVCalL( array, writeStream) );
    
    array.Append( entry );
    
    //should leave with KErrNotSupported
    EUNIT_ASSERT_SPECIFIC_LEAVE( iCalenExporter->ExportVCalL( array, writeStream), KErrNotSupported );
    
    array.Close();
    
    CleanupStack::PopAndDestroy(&writeStream);
    CleanupStack::PopAndDestroy(buf);
    
    
    CleanupStack::PopAndDestroy( entry );
    }


//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    CCalenExporterTest,
    "Add test suite description here.",
    "MODULE" )
    
//Specific export test cases
EUNIT_TEST(
    "Test getting right export flag",
    "CCalenExporter",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupExporterL, GetExportFlagsTestL, TeardownExporter )

EUNIT_TEST(
    "Test exporting of iCal",
    "CCalenExporter",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupExporterL, ExportICalTestL, TeardownExporter )

EUNIT_TEST(
    "Test exporting of iCal",
    "CCalenExporter",
    "Add tested function name here",
    "FUNCTIONALITY",
    SetupExporterL, ExportVCalTestL, TeardownExporter )
          
EUNIT_END_TEST_TABLE

//  END OF FILE
