/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
 *
*/


//  CLASS HEADER
#include "t_glxlogging.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <EUnitDecorators.h>

//  INTERNAL INCLUDES
#include "glxlogger.h"
#include "glxlogchunk.h"

// CONSTRUCTION
T_GlxLogging* T_GlxLogging::NewL()
    {
    T_GlxLogging* self = T_GlxLogging::NewLC();
    CleanupStack::Pop();

    return self;
    }

T_GlxLogging* T_GlxLogging::NewLC()
    {
    T_GlxLogging* self = new( ELeave ) T_GlxLogging;
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
T_GlxLogging::~T_GlxLogging()
    {
    }

// Default constructor
T_GlxLogging::T_GlxLogging()
    {
    }

// Second phase construct
void T_GlxLogging::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void T_GlxLogging::SetupL()
    {
    }

void T_GlxLogging::Teardown()
    {
    }

// local stub version of RDebug
TPtrC8 gPrintBuffer;
EXPORT_C void RDebug::RawPrint( const TDesC8& aDes )
	{
	gPrintBuffer.Set( aDes );
	}

_LIT8( KStrContents, "abcdefghijklmnopqrstuwxyz" );
void T_GlxLogging::T_TestWithoutChunkL()
    {
    // make a print
    _LIT8( KPrintStr, "FooBar");
    GlxLogger::WriteFormat( KPrintStr );
	// check it went through to RDebug
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr(), gPrintBuffer, "test that strings match");

	// test variables
    _LIT8( KPrintStr2, "FooBar %d,%.1f");
    _LIT8( KPrintStr2Result, "FooBar 1,2.5");
    GlxLogger::WriteFormat( KPrintStr2, 1, 2.45 );
	// check it went through to RDebug
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr2Result(), gPrintBuffer, "test that strings match");

	// test buffer overflow, 300 chars long buffer
	HBufC8* bigbuffer = HBufC8::NewL( 300 );
	TPtr8 ptr = bigbuffer->Des();
	for( TInt i = 0; i < 300; i++ )
		{
		TInt index = (i % KStrContents().Length() );
		TChar ch = KStrContents()[ index ];
		ptr.Append( ch );
		}

	// write big buffer alone
    GlxLogger::WriteFormat( *bigbuffer );
    // take a ptr to verify the content
    TPtr8 check = bigbuffer->Des();
    check.SetLength( 256 );
    EUNIT_ASSERT_EQUALS_DESC( check, gPrintBuffer, "test that strings match");

    // with data
    GlxLogger::WriteFormat( *bigbuffer, 1, 2, 3 );
    EUNIT_ASSERT_EQUALS_DESC( check, gPrintBuffer, "test that strings match");

	// release the buffer
	delete bigbuffer;
    }

// these are copied from glxlogchunk.cpp
// name for chunk 1
_LIT( KLogChunk1Name, "MC_Photos_Logchunk1" );
// name for chunk 2
_LIT( KLogChunk2Name, "MC_Photos_Logchunk2" );

// local helper to read a TPtrC from memory
inline TPtrC8 ReadTPtrC( TUint8*& aAddr )
	{
	TInt32* int_ptr = reinterpret_cast< TInt32* >( aAddr );
	// read size
	TInt32 size = *int_ptr;
    // adjust given addres and create the TPtrC
    aAddr += sizeof( TInt32 );
	TPtrC8 des( aAddr, size );
	// then adjust address to point to the next byte after this descriptor 
	aAddr += size;
	return des;
	}
	
void T_GlxLogging::T_TestWithChunk1L()
	{
	// create the chunks with the manager
	RGlxLogManager manager;
	manager.CreateL();

    // make a print
    _LIT8( KPrintStr, "FooBar");
    GlxLogger::WriteFormat( KPrintStr );

	// open chunk1 in read only
	RGlxLogUtility chunk;
	TInt err = chunk.Open( KLogChunk1Name(), ETrue );
    EUNIT_ASSERT_EQUALS_DESC( KErrNone, err, "no error in chunk open");
	// check that the descriptor is really there
	// read first descriptor
	TUint8* addres =  chunk.BaseAddress();
	TPtrC8 des = ReadTPtrC( addres );

    EUNIT_ASSERT_EQUALS_DESC( KPrintStr().Size(), des.Size(), "all bytes written");
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr(), des, "data matches");

    // make another print
    _LIT8( KPrintStr2, "FooBar %d, %.2f");
    _LIT8( KPrintStr2Result, "FooBar 99, 1.23");
    GlxLogger::WriteFormat( KPrintStr2, 99, 1.23 );
	// now we should have two descriptors there
	addres =  chunk.BaseAddress();
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr(), ReadTPtrC( addres ), "descriptor 1 matches");
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr2Result(), ReadTPtrC( addres ), "descriptor 2 matches");

	manager.Release();
	}

// second thread function
_LIT8( KPrintStrThread2, "T2 FooBar");
_LIT8( KPrintStr2Thread2, "T2 %d FooBar %.4f");
_LIT8( KPrintStr2Thread2Verify, "T2 -987 FooBar 0.0012");
TInt SecondThread( TAny* )
	{
    // make a print
    GlxLogger::WriteFormat( KPrintStrThread2 );

    // make another print
    GlxLogger::WriteFormat( KPrintStr2Thread2(), -987, 0.0012 );

	// signal that we are done
	RThread::Rendezvous( KErrNone );

	return 0;
	}

void T_GlxLogging::T_TestWithBothChunksL()
	{
	// create the chunks with the manager
	RGlxLogManager manager;
	manager.CreateL();

    // make a print from this thread
    _LIT8( KPrintStr3, "T1 FooBar");
    GlxLogger::WriteFormat( KPrintStr3 );

	// create another thread to write the log entries
	RThread thread2;
	User::LeaveIfError( 
		thread2.Create( _L("SecondThread"), &SecondThread, 1000, NULL, NULL ) );
	// request status to sync the test
	TRequestStatus requestStatus;
	// logon to the other thread
	thread2.Rendezvous( requestStatus );
	// run the created thread
	thread2.Resume();
	// wait untill the thread is complete
	User::WaitForRequest( requestStatus );

    // make another print from this thread
    _LIT8( KPrintStr4, "T1 FooBar %.4f %d,");
    _LIT8( KPrintStr4Verify, "T1 FooBar 0.0012 -11,");
    GlxLogger::WriteFormat( KPrintStr4(), 0.0012, -11 );

	// validate both the chunks
	// open chunk1 in read only
	RGlxLogUtility chunk1;
	TInt err = chunk1.Open( KLogChunk1Name(), ETrue );
    EUNIT_ASSERT_EQUALS_DESC( KErrNone, err, "no error in chunk open");
	// check that the descriptor is really there
	// read first descriptor
	TUint8* addres =  chunk1.BaseAddress();
	// we should have two descriptors in chunk 1
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr3(), ReadTPtrC( addres ), "descriptor 1 matches");
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr4Verify(), ReadTPtrC( addres ), "descriptor 2 matches");

	// open chunk2 in read only
	RGlxLogUtility chunk2;
	err = chunk2.Open( KLogChunk2Name(), ETrue );
    EUNIT_ASSERT_EQUALS_DESC( KErrNone, err, "no error in chunk open");
	// check that the descriptor is really there
	// read first descriptor
	addres =  chunk2.BaseAddress();
	// we should have two descriptors in chunk 2
    EUNIT_ASSERT_EQUALS_DESC( KPrintStrThread2(), ReadTPtrC( addres ), "descriptor 1 matches");
    EUNIT_ASSERT_EQUALS_DESC( KPrintStr2Thread2Verify(), ReadTPtrC( addres ), "descriptor 2 matches");

	manager.Release();
	}

//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE(
    T_GlxLogging,
    "Test suite for logging",
    "UNIT" )

EUNIT_TEST(
    "Test printing to RDebug",
    "GlxLogger",
    "WriteFormat",
    "FUNCTIONALITY",
    SetupL, T_TestWithoutChunkL, Teardown )

EUNIT_TEST(
    "Test printing to chunk 1",
    "GlxLogger",
    "WriteFormat",
    "FUNCTIONALITY",
    SetupL, T_TestWithChunk1L, Teardown )

EUNIT_TEST(
    "Test printing to chunk 2",
    "GlxLogger",
    "WriteFormat",
    "FUNCTIONALITY",
    SetupL, T_TestWithBothChunksL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
