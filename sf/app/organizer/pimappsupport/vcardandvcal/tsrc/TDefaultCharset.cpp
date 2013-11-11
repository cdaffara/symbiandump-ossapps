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

#include "TDefaultCharset.h"

// System includes
#include <versit.h>
#include <f32file.h>
#include <e32test.h>

// Literal constants
_LIT(KTestName, "TDefaultCharSet");

_LIT(KTestInputFile, "Z:\\TestFiles\\DefaultCharSetInput.vcf");
_LIT(KTestOutputFile, "C:\\DefaultCharSetOutput.vcf");
_LIT8(KShiftJis, "Shift_JIS");

// Globals
static RFs					TheFsSession;
static RTest				TheTest(KTestName);

static void FileCleanup(TAny * aFileMan)
	{
	CFileMan* FileMan = (CFileMan*)aFileMan;
	FileMan->Delete(_L("C:\\DefaultCharSetOutput.vcf"));
	}

static void DoTestsL()
	{
	CFileMan* FileMan = CFileMan::NewL(TheFsSession);
	CleanupStack::PushL(FileMan);
	CleanupStack::PushL(TCleanupItem(FileCleanup, FileMan));
	CVersitCardTest* test = CVersitCardTest::NewLC();
	test->StartTestsL();
	CleanupStack::PopAndDestroy(3, FileMan); // filecleanup, test
	}

/**
@SYMTestCaseID PIM-TDEFAULTCHARSET-0001
*/
GLDEF_C TInt E32Main()
	{
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;
	CTrapCleanup* TheCleanup = CTrapCleanup::New();
	if	(!TheCleanup)
		return KErrNoMemory;

	if	(TheFsSession.Connect() < KErrNone)
		return KErrGeneral;

	TheTest.Start(_L("@SYMTestCaseID PIM-TDEFAULTCHARSET-0001 TDefaultCharSet"));
	TRAPD(error, DoTestsL());
	TheTest(error == KErrNone);
	TheTest.End();
	TheTest.Close();
	TheFsSession.Close();

	delete TheCleanup;
	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	TheTest.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return KErrNone;
	}

//
// ------> CVersitCardTest (source)
//

//********************************************************************************************
CVersitCardTest* CVersitCardTest::NewLC()
	{
	CVersitCardTest* self = new(ELeave) CVersitCardTest;
	CleanupStack::PushL(self);
	return self;
	}

//********************************************************************************************
void CVersitCardTest::StartTestsL()
	{
	/*	
	This test is created to ensure that the new TVersitParserFlags enumeration 
	EUseDefaultCharSetForAllProperties works correctly and it gives the expected behaviour. 
	
	1.	Create parser and set default language from US-ASCII->SHIFT-JIS
	2.	Set EUseDefaultCharSetForAllProperties flag
	3.	Internalize a card with default SHIFT-JIS fields with no explicit charsets
	4.	Externalise the same contact (should put shift-jis charset explicitly into vcf)
	5.	Check to ensure that the exported vcf contains explict charset of SHIFT-JIS
		NOTE:: cannot check with internalize due to stripping of charset in AnalysesEncodingCharset()
	
	*/
	
	
	TheTest.Start(_L("Set up Parser"));
	
	CParserVCard* cardParser = CParserVCard::NewL();
	CleanupStack::PushL(cardParser);
		
	cardParser->SetDefaultCharSet(Versit::EShiftJISCharSet);					//1
	cardParser->SetFlags(CVersitParser::EUseDefaultCharSetForAllProperties);	//2
	
	TheTest.Next(_L("Start Internalization of Card"));
	TRAPD(err, InternalizeCardL(KTestInputFile, *cardParser));					//3
	TheTest(err==KErrNone);
	
	TheTest.Next(_L("Externalize the Card"));
	TRAP(err, ExternalizeCardL(KTestOutputFile, *cardParser));					//4
	TheTest(err==KErrNone);
	
	
	TheTest.Next(_L("Check that resulting card is correct"));
	TRAP(err, TheTest(CheckResultL(KTestOutputFile)));							//5
	TheTest(err==KErrNone);
	
	TheTest.End();	
	
	CleanupStack::PopAndDestroy(cardParser);	
	}
//********************************************************************************************

void CVersitCardTest::InternalizeCardL(const TDesC& aFile, CParserVCard& aParser) const
//
//	Internalize the specified file
//
	{
	TInt pos = 0;
	RFile file;
	User::LeaveIfError(file.Open(TheFsSession, aFile, EFileRead));
	CleanupClosePushL(file);

	// Read from the file
	static_cast<CVersitParser&>(aParser).InternalizeL(file, pos);

	CleanupStack::PopAndDestroy(); // file
	}
//********************************************************************************************

void CVersitCardTest::ExternalizeCardL(const TDesC& aFile, CParserVCard& aParser) const
//
//	Externalize the specified file
//
	{
	RFile file;
	User::LeaveIfError(file.Replace(TheFsSession, aFile, EFileWrite));
	CleanupClosePushL(file);

	// Write from the file
	static_cast<CVersitParser&>(aParser).ExternalizeL(file);

	CleanupStack::PopAndDestroy(); // file
	}
//********************************************************************************************

TBool CVersitCardTest::CheckResultL(const TDesC& aFile) const
	{
	TBool result=EFalse;
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	RFile myFile;
	CleanupClosePushL(myFile);

	TInt err = myFile.Open(fs, aFile, EFileRead|EFileShareReadersOnly);
	if (err != KErrNone)
		User::Leave(err);	
		
	TBuf8<256> buffer;
	myFile.Read(buffer, 256);
	
	//	check to ensure that shift-jis is explicitly set.
	if(KErrNotFound != buffer.Find(KShiftJis))
		result=ETrue;
	else
		result=EFalse;

	CleanupStack::PopAndDestroy(2);	//	myFile, fs
	return result;
	}
