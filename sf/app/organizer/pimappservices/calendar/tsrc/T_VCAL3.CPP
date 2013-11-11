// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Auto test VCal import functionality
// 
//

#include <e32test.h>
#include <s32file.h>
#include <txtrich.h>
#include <e32math.h>

#include <caldataexchange.h>
#include <caldataformat.h>
#include <calentryview.h>
#include <caliterator.h>
#include <calentry.h>

#include <coreappstest/testserver.h>

#include "caltestlib.h"

LOCAL_D RTest test(_L("T_VCAL3"));

_LIT(KTestCalendarFile, "c:t_vcal3");
_LIT(KTestVCalFile, "t_vcal3.vcs");

const TInt numNames = 5;
const TInt numParams = 2;
const TInt numValues = 7;

TBuf8<40> names[numNames];
TBuf8<40> params[numParams];
TBuf8<40> values[numValues];
TBuf8<1> charstring;

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void RunTestsL();
	
private:
	void ConstructL();

	void PrintValue(RFile& outfile, TInt64& seed);
	void PrintParam(RFile& outfile, TInt64& seed);
	void PrintName(RFile& outfile, TInt64& seed);

private:
	CCalTestLibrary* iCalTestLib;
	CCalDataExchange* iCalDataExchange;

	};
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new(ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->ReplaceFileL(KTestCalendarFile);
	iCalTestLib->OpenFileL(KTestCalendarFile);
	iCalDataExchange = CCalDataExchange::NewL(iCalTestLib->GetSession());
	}
	
CTestApp::~CTestApp()
	{
	TRAP_IGNORE(iCalTestLib->DeleteFileL(KTestCalendarFile, ETrue));
	delete iCalDataExchange;
	delete iCalTestLib;
	}
	
void CTestApp::PrintValue(RFile& outfile, TInt64& seed)
	{
	TInt valueIndex = static_cast<TInt>(Math::FRand(seed) * numValues * 1.1);
	if(valueIndex < numValues)
		{
		outfile.Write(values[valueIndex]);
		}
	else
		{
		if(static_cast<TInt>(Math::FRand(seed) + 0.5))
			{
			TInt repeats = static_cast<TInt>(Math::FRand(seed) * 50);
			for(TInt p = 0; p < repeats; p++)
				{
				charstring.Fill(static_cast<TInt>(Math::FRand(seed) * 255), 1);
				}
			outfile.Write(charstring);
			}
		}
	}

void CTestApp::PrintParam(RFile& outfile, TInt64& seed)
	{
	TInt paramIndex = static_cast<TInt>(Math::FRand(seed) * numParams * 1.1);
	if(paramIndex < numParams)
		{
		outfile.Write(_L8(";"));
		outfile.Write(params[paramIndex]);
		outfile.Write(_L8("="));
		PrintValue(outfile, seed);
		}
	else
		{
		TInt repeats = static_cast<TInt>(Math::FRand(seed) * 50);
		for(TInt p = 0; p < repeats; p++)
			{
			charstring.Fill(static_cast<TInt>(Math::FRand(seed) * 255), 1);
			}
		outfile.Write(charstring);
		}
	}

void CTestApp::PrintName(RFile& outfile, TInt64& seed)
	{
	TInt nameIndex = static_cast<TInt>(Math::FRand(seed) * numNames * 1.1);
	if(nameIndex < numNames)
		{
		outfile.Write(names[nameIndex]);

		//write param
		if(static_cast<TInt>(Math::FRand(seed) + 0.4))
			PrintParam(outfile, seed);
		outfile.Write(_L8(":"));

		//write value
		if(static_cast<TInt>(Math::FRand(seed) + 0.8))
			PrintValue(outfile, seed);

		if(static_cast<TInt>(Math::FRand(seed) + 0.8))
			outfile.Write(_L8("\r\n"));
		}
	else
		{
		TInt repeats = static_cast<TInt>(Math::FRand(seed) * 10);
		for(TInt p = 0; p < repeats; p++)
			{
			charstring.Fill(static_cast<TInt>(Math::FRand(seed) * 255), 1);
			}
		outfile.Write(charstring);
		}				
		if(static_cast<TInt>(Math::FRand(seed) + 0.8))
			outfile.Write(_L8("\r\n"));
	}

void CTestApp::RunTestsL()
	{	
	TTime time;
	time.UniversalTime();
	TInt64 seed = time.Int64();

	names[0] = _L8("BEGIN");
	names[1] = _L8("END");
	names[2] = _L8("\r\n");
	names[3] = _L8("DTSTART:");
	names[4] = _L8("DESCRIPTION:");
	
	params[0] = _L8("ENCODING");
	params[1] = _L8("LANGUAGE");

	values[0] = _L8("VCALENDAR");
	values[1] = _L8("VEVENT");
	values[2] = _L8("VTODO");
	values[3] = _L8("BASE64");
	values[4] = _L8("QUOTED-PRINTABLE");
	values[5] = _L8("8-bit");
	values[6] = _L8("fr-CA");

	for(TInt i = 0; i < 100; i++)
		{
		iCalTestLib->CleanDatabaseL();
		
		// Create the random vcal file "vcal.vcs" full of "non-entries"
		RFile outfile;
		iCalTestLib->TestRegister().CreateTempFileLC(outfile, KTestVCalFile);
		test.Printf(_L("Creating random vcal.vcs\r\n"));
		
		TInt repeats = static_cast<TInt>((Math::FRand(seed) * 10) + 2);
		for(TInt j = 0; j < repeats; j++)
			{
			if(static_cast<TInt>(Math::FRand(seed) + 0.6))
				{
				outfile.Write(_L8("BEGIN:VCALENDAR"));
				outfile.Write(_L8("\r\n"));
				}
  			if(static_cast<TInt>(Math::FRand(seed) + 0.6))
				{
				outfile.Write(_L8("BEGIN:VEVENT"));
				outfile.Write(_L8("\r\n"));
				}

  			TInt entryrepeats = static_cast<TInt>(Math::FRand(seed) * 80);
			for(TInt k = 0; k < entryrepeats; k++)
				{
				PrintName(outfile, seed);
				}

  			if(static_cast<TInt>(Math::FRand(seed) + 0.6))
				{
				outfile.Write(_L8("END:VEVENT"));
				outfile.Write(_L8("\r\n"));
				}
  			if(static_cast<TInt>(Math::FRand(seed) + 0.6))
				{
				outfile.Write(_L8("END:VCALENDAR"));
				outfile.Write(_L8("\r\n"));
				}
			}

		CleanupStack::PopAndDestroy(&outfile);

		test.Next(_L("Importing All entries"));


		RFile infile;
		iCalTestLib->TestRegister().OpenTempFileLC(infile, KTestVCalFile);

		RFileReadStream readStream(infile);
		CleanupClosePushL(readStream);

		// Create array for new entries
		RPointerArray<CCalEntry> entryArray;
		CleanupResetAndDestroyPushL(entryArray);
		
		TRAPD(err, iCalDataExchange->ImportL(KUidVCalendar, readStream, entryArray));
		
		test(err == KErrNotFound || err == KErrNone); // test the import left appropriatly - not found if no matching BEGIN-END
		
		CleanupStack::PopAndDestroy(&entryArray);
		CleanupStack::PopAndDestroy(&readStream);
		CleanupStack::PopAndDestroy(&infile);
		}
		
	iCalTestLib->FileSession().Delete(KTestVCalFile);
	}
	
static void DoMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	testApp->RunTestsL();
	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-T-VCAL3-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VCAL3-0001 T_VCAL3"));

	test.Title();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,DoMainL());	
	test(ret==KErrNone);
	delete theCleanup;	
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

