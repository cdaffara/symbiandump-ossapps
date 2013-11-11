// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TJapaneseSoundProperty.h"

// System includes
#include <vcard.h>
#include <f32file.h>
#include <e32test.h>

// Literal constants
_LIT(KTestName, "TJapaneseSoundProperty");
_LIT(KTestPath, "Z:\\VersitJapaneseVCards\\");
_LIT8(KSoundExtensionPropertyParameterName, "X-IRMC-N");

// Globals
static RFs					TheFsSession;
static RTest				TheTest(KTestName);

static void DoTestsL()
	{
	CVersitCardTest* test = CVersitCardTest::NewLC();
	test->StartTestsL();
    CleanupStack::PopAndDestroy(test);
	}
	
static void DriveSpaceReport()
	{
	TheTest.Printf(_L("Drive space report:\n"));
	
	TVolumeInfo volInfo;
	TheFsSession.Volume(volInfo, EDriveC);
	TheTest.Printf(_L("Volume C: Free Space: %D\n"), volInfo.iFree);
	TheTest.Printf(_L("Volume C: Total Size: %D\n"), volInfo.iSize);
	
	TheFsSession.Volume(volInfo, EDriveZ);
	TheTest.Printf(_L("Volume Z: Free Space: %D\n"), volInfo.iFree);
	TheTest.Printf(_L("Volume Z: Total Size: %D\n"), volInfo.iSize);	
	}

//@SYMTestCaseID PIM-TJAPANESESOUNDPROPERTY-0001

GLDEF_C TInt E32Main()
	{
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	if	(!theCleanup)
		return KErrNoMemory;

	if	(TheFsSession.Connect() < KErrNone)
		return KErrGeneral;
	
	// Test code to see how much space is left on c: when this is failing with 
	// -43 (KErrDirFull)
	DriveSpaceReport();
	// end of test code	

	TheTest.Start(_L("@SYMTestCaseID PIM-TJAPANESESOUNDPROPERTY-0001 TJapaneseSoundProperty"));
	TRAPD(error, DoTestsL());
	if (error != KErrNone)
		{
		TheTest.Printf(_L("Test failed with error code: %d"), error);
		
		// Test code to see how much space is left on c: when this is failing with 
		// -43 (KErrDirFull)
		DriveSpaceReport();
		// end of test code
		}
	TheTest(error == KErrNone);
	TheTest.End();
	TheTest.Close();
	TheFsSession.Close();

	delete theCleanup;
	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	TheTest.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return KErrNone;
	}

CVersitCardTest::~CVersitCardTest()
	{
	}

void CVersitCardTest::ConstructL()
	{
	}

CVersitCardTest* CVersitCardTest::NewLC()
	{
	CVersitCardTest* self = new(ELeave) CVersitCardTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//********************************************************************************************
void CVersitCardTest::StartTestsL()
	{
	CParserVCard* parser;


    // TEST 0
    _LIT(KTestFileName0, "escaped.vcf"); //SOUND;X-IRMC-N:This \;Is The \;First\;\;;\;Second\; Bit \;
	parser = InternalizeCardLC(KTestPath, KTestFileName0);
    CheckNumberOfSoundPropertiesL(KTestFileName0, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 1
    _LIT(KTestFileName1, "amano.vcf");  //SOUND;X-IRMC-N:Amano;Yoshitaka
	parser = InternalizeCardLC(KTestPath, KTestFileName1);
    CheckNumberOfSoundPropertiesL(KTestFileName1, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 2
    _LIT(KTestFileName2, "aoki.vcf");  //SOUND;X-IRMC-N;CHARSET=SHIFT_JIS:?A?I?L;?C?T?I
	parser = InternalizeCardLC(KTestPath, KTestFileName2);
    CheckNumberOfSoundPropertiesL(KTestFileName2, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 3
    _LIT(KTestFileName3, "barbara.vcf"); //SOUND;X-IRMC-N;CHARSET=SHIFT_JIS:?o?[?o?‰;?J?^?I?J
	parser = InternalizeCardLC(KTestPath, KTestFileName3);
    CheckNumberOfSoundPropertiesL(KTestFileName3, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 4
    _LIT(KTestFileName4, "Digit1.vcf");	 //SOUND;X-IRMC-N:999999;99999
	parser = InternalizeCardLC(KTestPath, KTestFileName4);
    CheckNumberOfSoundPropertiesL(KTestFileName4, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 5
    _LIT(KTestFileName5, "Digit2.vcf"); //SOUND;X-IRMC-N:‚R‚R‚R‚R‚R‚R;‚T‚T‚T‚T‚T‚T
	parser = InternalizeCardLC(KTestPath, KTestFileName5);
    CheckNumberOfSoundPropertiesL(KTestFileName5, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 6
    _LIT(KTestFileName6, "haga.vcf");  //SOUND;X-IRMC-N;CHARSET=SHIFT_JIS:?n?K;?„?X?V
	parser = InternalizeCardLC(KTestPath, KTestFileName6);
    CheckNumberOfSoundPropertiesL(KTestFileName6, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 7
    _LIT(KTestFileName7, "kasai.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName7);
    CheckNumberOfSoundPropertiesL(KTestFileName7, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 8
    _LIT(KTestFileName8, "kitajima.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName8);
    CheckNumberOfSoundPropertiesL(KTestFileName8, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 9
    _LIT(KTestFileName9, "Longest.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName9);
    CheckNumberOfSoundPropertiesL(KTestFileName9, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 10
    _LIT(KTestFileName10, "matsui.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName10);
    CheckNumberOfSoundPropertiesL(KTestFileName10, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 11
    _LIT(KTestFileName11, "miyazaki.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName11);
    CheckNumberOfSoundPropertiesL(KTestFileName11, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 12
    _LIT(KTestFileName12, "nakai.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName12);
    CheckNumberOfSoundPropertiesL(KTestFileName12, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 13
    _LIT(KTestFileName13, "powell.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName13);
    CheckNumberOfSoundPropertiesL(KTestFileName13, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 14
    _LIT(KTestFileName14, "prolongedsound.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName14);
    CheckNumberOfSoundPropertiesL(KTestFileName14, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 15
    _LIT(KTestFileName15, "SpecialCharacter1.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName15);
    CheckNumberOfSoundPropertiesL(KTestFileName15, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 16
    _LIT(KTestFileName16, "SpecialCharacter1.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName16);
    CheckNumberOfSoundPropertiesL(KTestFileName16, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 17
    _LIT(KTestFileName17, "SpecialCharacter2.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName17);
    CheckNumberOfSoundPropertiesL(KTestFileName17, *parser, 2);
    CleanupStack::PopAndDestroy(parser);


    // TEST 18
    _LIT(KTestFileName18, "takashiro.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName18);
    CheckNumberOfSoundPropertiesL(KTestFileName18, *parser, 2);
    CleanupStack::PopAndDestroy(parser);
 
    // TEST 19
    _LIT(KTestFileName19, "MultipleSounds.vcf");
	parser = InternalizeCardLC(KTestPath, KTestFileName19);
    CheckNumberOfSoundPropertiesL(KTestFileName19, *parser, 2, 2);
    CleanupStack::PopAndDestroy(parser);


    // Create vCard with sound property and export it.
    CreateVCardTestL();
	}


//********************************************************************************************
CParserVCard* CVersitCardTest::InternalizeCardLC(const TDesC& aFile)
//
//	Internalize the specified file
//
	{
    CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);
    //
    TInt pos = 0;
	RFile file;
	User::LeaveIfError(file.Open(TheFsSession, aFile, EFileRead));
	CleanupClosePushL(file);

	// Read from the file
	static_cast<CVersitParser*>(parser)->InternalizeL(file, pos);

	CleanupStack::PopAndDestroy(); // file
    //
    return parser;
    }


//********************************************************************************************
CParserVCard* CVersitCardTest::InternalizeCardLC(const TDesC& aPath, const TDesC& aFileName)
//
//	Internalize the specified file from the specified path
//
	{
    TFileName name(aPath);
    name += aFileName;
    return InternalizeCardLC(name);
    }


//********************************************************************************************
void CVersitCardTest::CheckNumberOfSoundPropertiesL(const TDesC& aFileName, const CParserVCard& aParser, TInt aExpectedArrayElementCount, TInt aExpectedPropertyCount)
//
//	Check that the number of SOUND sub-elements are as expected
//
	{
    TheTest.Printf(_L("%S\n"), &aFileName);
    (void) aFileName;
    //
    TBool passedTest = EFalse;
    CArrayPtr<CParserProperty>* properties = aParser.PropertyL(KVersitTokenSOUND, TUid::Uid(KVersitPropertyHBufCUid), EFalse);
    TheTest(properties != NULL);
    if  (properties)
        {
        //
        CleanupStack::PushL(properties);
        const TInt count = properties->Count();
        TheTest(count == aExpectedPropertyCount);
        //
        for(TInt i=0; i<count; i++)
            {
            const CParserProperty* property = properties->At(i);

            // Check that the X-IRMC-N property parameter exists
            const TBool foundParam = property->Param(KSoundExtensionPropertyParameterName) != NULL;
	        TheTest(foundParam);

            // Check property value
            TheTest(property->Value() != NULL);

            CParserPropertyValueHBufC* propertyAsHBufC = static_cast<CParserPropertyValueHBufC*>(property->Value());

            // Check property value as an array
            CParserPropertyValueCDesCArray* valueAsArray = propertyAsHBufC->TreatAsArrayPropertyLC(*property);
            TheTest(valueAsArray->Value() != NULL);
            CDesCArray& array = *valueAsArray->Value();
            //
            const TInt arrayItemCount = array.Count();
            TheTest(arrayItemCount == aExpectedArrayElementCount);
            for(TInt j=0; j<arrayItemCount; j++)
                {
                const TPtrC pValue(array[j]);
                TheTest.Printf(_L("%S\n"), &pValue);
                }
            TheTest.Printf(_L("\n"));
            CleanupStack::PopAndDestroy(valueAsArray);
            //
            passedTest = ETrue;
            }

        CleanupStack::PopAndDestroy(properties);
        }
    TheTest(passedTest);
	}


//********************************************************************************************
void CVersitCardTest::CreateVCardTestL()
//
//
//
    {
	CVersitParser* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);

    // Add Name
    {
    CDesCArrayFlat* nameArray = new(ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(nameArray);
    nameArray->AppendL(_L("Wibble"));
    nameArray->AppendL(_L("Wobble"));
    //
    CParserPropertyValue* propNameValue = new(ELeave) CParserPropertyValueCDesCArray(nameArray);
    CleanupStack::Pop(nameArray);
    CleanupStack::PushL(propNameValue);
    CParserProperty* propName = CParserProperty::NewL(*propNameValue, KVersitTokenN, NULL);
    CleanupStack::Pop(propNameValue);
    parser->AddPropertyL(propName);
    }

    // Add Sound
    {
    CDesCArrayFlat* soundArray = new(ELeave) CDesCArrayFlat(2);
    CleanupStack::PushL(soundArray);
    soundArray->AppendL(_L("Sound1"));
    soundArray->AppendL(_L("Sound2"));
    //
    CParserPropertyValue* propSoundValue = new(ELeave) CParserPropertyValueCDesCArray(soundArray);
    CleanupStack::Pop(soundArray);
    CleanupStack::PushL(propSoundValue);
    CParserProperty* propSound = CParserProperty::NewL(*propSoundValue, KVersitTokenSOUND, NULL);
    CleanupStack::Pop(propSoundValue);
    //
	CParserParam* param = CParserParam::NewL(KSoundExtensionPropertyParameterName, KNullDesC8);
	CleanupStack::PushL(param);
    propSound->AddParamL(param);
	CleanupStack::Pop(param);
    //
    parser->AddPropertyL(propSound);
    }

    // Export vCard
    _LIT(KFileName, "C:\\JapaneseSound.vcf");
	RFile file;
	User::LeaveIfError(file.Replace(TheFsSession, KFileName, EFileWrite));
	CleanupClosePushL(file);
    parser->ExternalizeL(file);
	CleanupStack::PopAndDestroy(&file);
    
    // Test
    CParserVCard* importedCard = CVersitCardTest::InternalizeCardLC(KFileName);
    CheckNumberOfSoundPropertiesL(KFileName, *importedCard, 2);
    CleanupStack::PopAndDestroy(importedCard);

    // Tidy up
    CleanupStack::PopAndDestroy(parser);    
    }



