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
* Description:   test agnversit2 api
*
*/


//  CLASS HEADER
#include "testversit2.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>


//  INTERNAL INCLUDES
#include "AgnVersit2.h"				// CAgnVersit2
#include "AgnVersit2TestConstants.h"// Constants
#include "ICalParser.h"
#include "ICal.h"

// -----------------------------------------------------------------------------
// CCVersit2Test::NewL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCVersit2Test* CCVersit2Test::NewL()
    {
    CCVersit2Test* self = CCVersit2Test::NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::NewLC
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
CCVersit2Test* CCVersit2Test::NewLC()
    {
    CCVersit2Test* self = new( ELeave ) CCVersit2Test();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::~CCVersit2Test()
// Destructor (virtual by CBase).
// -----------------------------------------------------------------------------
//
CCVersit2Test::~CCVersit2Test()
    {
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::CCVersit2Test
// Default constructor
// -----------------------------------------------------------------------------
//
CCVersit2Test::CCVersit2Test()
    {
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::ConstructL
// Second phase construction
// -----------------------------------------------------------------------------
//
void CCVersit2Test::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::SetupL
// Setup.
// -----------------------------------------------------------------------------
//
void CCVersit2Test::SetupL()
    {
	//Create file server session
	EUNIT_ASSERT_EQUALS(iFs.Connect(), KErrNone);

	//Get the input and output directories to use from the test ini file
	iInputDir 	= HBufC::NewL(KFileNameLength);
	iOutputDir	= HBufC::NewL(KFileNameLength);

	iInputDir->Des().Append(_L("E://testing//data//unittest//agnversit2//input//"));
	iOutputDir->Des().Append(_L("E://testing//data//unittest//agnversit2//output//"));

	//Make an output directory if needed
	iFs.MkDirAll(*iOutputDir);

	iImportFile = HBufC::NewL(KFileNameLength);
	iExportFile = HBufC::NewL(KFileNameLength);
	iCheckFile	= HBufC::NewL(KFileNameLength);

    iString = HBufC::NewL(KICalMaxContentLineLength);

	//Create a CICalParser object
	iCalParser = CICalParser::NewL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::Teardown
// Teardown.
// -----------------------------------------------------------------------------
//
void CCVersit2Test::Teardown()
    {
    delete iString;
    iString = NULL;
	delete iCalParser;
	iCalParser = NULL;
	delete iInputDir;
	iInputDir = NULL;
	delete iOutputDir;
	iOutputDir = NULL;
	delete iImportFile;
	iImportFile = NULL;
	delete iExportFile;
	iExportFile = NULL;
	delete iCheckFile;
	iCheckFile = NULL;
	iFs.Close();
    }
    
// -----------------------------------------------------------------------------
// CCVersit2Test::ImportL
// Import the entry
// -----------------------------------------------------------------------------
//
TBool CCVersit2Test::ImportL()
    {
	RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *iImportFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);
	
	iCalParser->InternalizeL(fileStream);
	
	CleanupStack::PopAndDestroy(&fileStream);
	return ETrue;
    }
 
// -----------------------------------------------------------------------------
// CCVersit2Test::ExportL
// Export the entry
// -----------------------------------------------------------------------------
//    
TBool CCVersit2Test::ExportL()
    {
	RFileWriteStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Replace(iFs, *iExportFile, EFileWrite), KErrNone);
	CleanupClosePushL(fileStream);
	
	TInt count = iCalParser->CalCount();
	
	for (TInt x = 0; x < count; ++x)
		{
		iCalParser->Cal(x).ExternalizeL(fileStream);	
		}
	
	CleanupStack::PopAndDestroy(&fileStream);
	return ETrue;
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::CompareFilesL
// Compares the contents of iImportFile against the contents of iExportFile line
// by line. Ignores certain lines containing properties which are expected to be
// different (eg time stamp)
// -----------------------------------------------------------------------------
//
TBool CCVersit2Test::CompareFilesL()
	{
	TBool identical = ETrue;

	// Open a read stream on both files
	RFileReadStream fileStream1;
	RFileReadStream fileStream2;
	EUNIT_ASSERT_EQUALS(fileStream1.Open(iFs, *iExportFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream1);
	EUNIT_ASSERT_EQUALS(fileStream2.Open(iFs, *iCheckFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream2);

	// Buffer to hold each line of a file
	HBufC8* buf1 = HBufC8::NewLC(KICalMaxContentLineLength);
	HBufC8* buf2 = HBufC8::NewLC(KICalMaxContentLineLength);

	TPtr8 buf1Ptr = buf1->Des();
	TPtr8 buf2Ptr = buf2->Des();

	TInt error1(KErrNone);
	TInt error2(KErrNone);

	// Compare the files line by line, excluding certain
	// properties we expect to be different (eg the time stamp)
	while (error1 == KErrNone && error2 == KErrNone && identical)
		{
		TRAP(error1, GetLineL(fileStream1, buf1Ptr));
		TRAP(error2, GetLineL(fileStream2, buf2Ptr));
		
		if (error1 != error2)
			{
			identical = EFalse;
			}
		else if (error1 == KErrNone)
			{
			if (buf1Ptr.Find(KDtStamp) == 0)
				{
				EUNIT_ASSERT_EQUALS(buf2Ptr.Find(KDtStamp), 0);
				identical = (buf2Ptr.Find(KDtStamp) == 0);
				}
			else if (buf1Ptr.Find(KCreated) == 0)
				{
				EUNIT_ASSERT_EQUALS(buf2Ptr.Find(KCreated), 0);
				identical = (buf2Ptr.Find(KCreated) == 0);
				}
			else
				{
				EUNIT_ASSERT_EQUALS(buf1Ptr.CompareF(buf2Ptr), 0);
				identical = (buf1Ptr.CompareF(buf2Ptr) == 0);
				}
			}
		}
		
	CleanupStack::PopAndDestroy(buf2);
	CleanupStack::PopAndDestroy(buf1);
	CleanupStack::PopAndDestroy(&fileStream2);
	CleanupStack::PopAndDestroy(&fileStream1);
		
	return identical;
	}

// -----------------------------------------------------------------------------
// CCVersit2Test::GetLineL
// Helper function used by CompareFilesL.
// Reads from a RFileReadStream into a descriptor up to the next new line
// -----------------------------------------------------------------------------
//
void CCVersit2Test::GetLineL(RFileReadStream& aStream,TPtr8& aPtr)
	{
	aStream.ReadL(aPtr, TChar(KNewline));	// Read up to the next newline.
	}

// -----------------------------------------------------------------------------
// CCVersit2Test::GetStringFromFile
// Get String From File.
// -----------------------------------------------------------------------------
//
void CCVersit2Test::GetStringFromFile(const TDesC8& aSectName, const TDesC8& aKeyName, TPtrC& aResult)
    {
    HBufC* configFile	= HBufC::NewLC(KFileNameLength);

    configFile->Des().Insert(0, *iInputDir);
	configFile->Des().Append(_L("AgnVersit2Test.ini"));    

    RFileReadStream fileStream;
	EUNIT_ASSERT_EQUALS(fileStream.Open(iFs, *configFile, EFileRead), KErrNone);
	CleanupClosePushL(fileStream);

	// Buffer to hold each line of a file
	HBufC8* buf = HBufC8::NewLC(KICalMaxContentLineLength);
	TPtr8 bufPtr = buf->Des();
	
	TInt error(KErrNone);
	TBool found = EFalse;
    aResult.Set(NULL, 0); 
    
    while (error == KErrNone && !found)
		{
        TRAP(error, GetLineL(fileStream, bufPtr));
		
		if (bufPtr.Find(aSectName) == 1)
		    {
		    while(error == KErrNone && !found)
		        {
		        if(bufPtr.Find(aKeyName) == 0)
		            {
		            TInt i;

		            TPtrC8 ptr(bufPtr);
		            i = ptr.Find(KSpace2)+1;
		            ptr.Set(ptr.Right(ptr.Length() - i));
                    i = ptr.Find(KEndofLine);
                    
                    RDebug::RawPrint( ptr );
                    RDebug::RawPrint( ptr.Left(i) );
                    
                    ptr.Set(ptr.Left(i));

                    delete iString;
                    iString = NULL;
                    
                    iString = Convert8To16BitL(ptr);
                    aResult.Set( iString->Des().PtrZ(), ptr.Length());
                    RDebug::RawPrint( aResult );
                	found = ETrue;
		            }
		        else
		            {
		            TRAP(error, GetLineL(fileStream, bufPtr)); 
		            }
		        }
		    }
		}
	CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(&fileStream);
    CleanupStack::PopAndDestroy(configFile);
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::Convert8To16BitL
// Conversion.
// -----------------------------------------------------------------------------
//
HBufC16* CCVersit2Test::Convert8To16BitL(const TDesC8& aDes8)
    {
    HBufC16* myBuf = HBufC16::NewL(aDes8.Length()+1);
    TPtr16 ptr = myBuf->Des();
    ptr.Copy(aDes8);
    return myBuf;
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::GetFilesL
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
void CCVersit2Test::GetFilesL(const TDesC8& aDes8)
    {
    TPtrC fileName(NULL, KICalMaxContentLineLength);
    TPtr test(NULL, 0);

    GetStringFromFile(aDes8, KImportFileName, fileName);
	iImportFile->Des().Insert(0, *iInputDir);
  	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iImportFile->Des().Append(test);

    GetStringFromFile(aDes8, KExportFileName1, fileName);
	iExportFile->Des().Insert(0, *iOutputDir);
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iExportFile->Des().Append(test);
	
    GetStringFromFile(aDes8, KCheckFileName, fileName);
	iCheckFile->Des().Insert(0,*iInputDir);
	test.Set(const_cast<TUint16*>(fileName.Ptr()), fileName.Length(), fileName.Length());
	test.TrimAll();
	iCheckFile->Des().Append(test);  
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::doTestStepL
// Execute test steps
// -----------------------------------------------------------------------------
//
void CCVersit2Test::doTestStepL()
    {
    EUNIT_ASSERT( ImportL() );
    EUNIT_ASSERT( ExportL() );
    EUNIT_ASSERT( CompareFilesL() );
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::PropertyValidationL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::PropertyValidationL()
    {
	GetFilesL(KVersit2PropertyValidation);
	
	doTestStepL();    
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::EmbeddingValidationL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::EmbeddingValidationL()
    {
	GetFilesL(KVersit2EmbeddingValidation);
	
	doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::EscapeSequenceL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::EscapeSequenceL()
    {
	GetFilesL(KVersit2EscapeSequence);
	
	doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::VJournalL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::VJournalL()
    {
	GetFilesL(KVersit2VJournal);
	
	doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::VFreeBusyL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::VFreeBusyL()
    {
	GetFilesL(KVersit2VFreeBusy);
	
	doTestStepL();
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::Corrupt1L
// -----------------------------------------------------------------------------
//
void CCVersit2Test::Corrupt1L()
    {
	GetFilesL(KVersit2Corrupt1);
	EUNIT_PRINT(_L("This test case should leave with KErrCorrupt"));
    EUNIT_ASSERT_SPECIFIC_LEAVE( ImportL(), KErrCorrupt );
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::Corrupt2L
// Symbian OS 2 phased constructor.
// -----------------------------------------------------------------------------
//
void CCVersit2Test::Corrupt2L()
    {
    GetFilesL(KVersit2Corrupt2);
	
	EUNIT_PRINT(_L("This test case should leave with KErrCorrupt"));
    EUNIT_ASSERT_SPECIFIC_LEAVE( ImportL(), KErrCorrupt );
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::Corrupt3L
// -----------------------------------------------------------------------------
//
void CCVersit2Test::Corrupt3L(  )
    {
    GetFilesL(KVersit2Corrupt3);
	
	EUNIT_PRINT(_L("This test case should leave with KErrCorrupt"));
    EUNIT_ASSERT_SPECIFIC_LEAVE( ImportL(), KErrCorrupt );
    }

// -----------------------------------------------------------------------------
// CCVersit2Test::NotCorruptL
// -----------------------------------------------------------------------------
//
void CCVersit2Test::NotCorruptL(  )
    {
    GetFilesL(KVersit2NotCorrupt);
	
	doTestStepL();
    }
  
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    CCVersit2Test,
    "Versit2 unit tests",
    "UNIT" )

EUNIT_TEST(
    "PropertyValidation",
    "Versit2",
    "PropertyValidationL",
    "FUNCTIONALITY",
    SetupL, PropertyValidationL, Teardown)

EUNIT_TEST(
    "EmbeddingValidation",
    "Versit2",
    "EmbeddingValidationL",
    "FUNCTIONALITY",
    SetupL, EmbeddingValidationL, Teardown)
    
EUNIT_TEST(
    "EscapeSequence",
    "Versit2",
    "EscapeSequenceL",
    "FUNCTIONALITY",
    SetupL, EscapeSequenceL, Teardown)

EUNIT_TEST(
    "VJournal",
    "Versit2",
    "VJournalL",
    "FUNCTIONALITY",
    SetupL, VJournalL, Teardown)

EUNIT_TEST(
    "VFreeBusy",
    "Versit2",
    "VFreeBusyL",
    "FUNCTIONALITY",
    SetupL, VFreeBusyL, Teardown)
    
 EUNIT_TEST(
    "Corrupt1",
    "Versit2",
    "Corrupt1L",
    "FUNCTIONALITY",
    SetupL, Corrupt1L, Teardown)

 EUNIT_TEST(
    "Corrupt2",
    "Versit2",
    "Corrupt2L",
    "FUNCTIONALITY",
    SetupL, Corrupt2L, Teardown)
    
 EUNIT_TEST(
    "Corrupt3",
    "Versit2",
    "Corrupt3L",
    "FUNCTIONALITY",
    SetupL, Corrupt3L, Teardown)

 EUNIT_TEST(
    "NotCorrupt",
    "Versit2",
    "NotCorruptL",
    "FUNCTIONALITY",
    SetupL, NotCorruptL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
