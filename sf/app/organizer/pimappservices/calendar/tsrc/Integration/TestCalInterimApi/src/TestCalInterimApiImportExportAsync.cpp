// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32file.h>
#include "TestCalInterimApiImportExportAsync.h"

// Class CTestCalInterimApiImportExportAsync - Common functions that are used between the import/export Async Tests
CTestCalInterimApiImportExportAsync::CTestCalInterimApiImportExportAsync()
	{
	}

CTestCalInterimApiImportExportAsync::~CTestCalInterimApiImportExportAsync()
	{
	}

/** Opens vcs file and asynchronously imports the vcs file into entries
@param	aPath The path of the vcs file to open
@param	aFsSession A handle to a file server session
@param	avCalNumber The number of vCal to import
@param	aCalEntryArray The array which will contain the imported vCal's
@test
*/
void CTestCalInterimApiImportExportAsync::OpenVCSAndImportAsyncL(TDes& aPath, RFs aFsSession, TInt avCalNumber, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	RFileReadStream readVCSStream;

	// Format buf to hold relevant vcs file
	TBuf<KMaxLengthString> vCalFile;
	vCalFile.Append(_L("c:"));
	vCalFile.Append(aPath);
	vCalFile.AppendNum(avCalNumber);
	vCalFile.Append(_L(".vcs"));
	
	// Opens vcs file
	TInt openError = readVCSStream.Open(aFsSession, vCalFile, EFileRead);
	
#ifndef __WINS__
	if (openError != KErrNone)
		{
		TFileName fullName = RProcess().FileName();
		TParsePtrC parse(fullName);
		vCalFile.Replace(0, 2, parse.Drive());
		openError = readVCSStream.Open(aFsSession, vCalFile, EFileRead);
		}
#endif

	// Opens vcs file
	User::LeaveIfError(openError);

	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KOpenVCSFile, &vCalFile);
	INFO_PRINTF1(message);

	CleanupClosePushL(readVCSStream);

	// Imports the vCal
	ImportvCalAsyncL(readVCSStream, aCalEntryArray);

	CleanupStack::PopAndDestroy(); // pop readVCSStream
	}

/** Imports asynchronously vCals into an array
@param	aReadStream The read stream which the vCals will be read from
@param	aCalEntryArray The array which will hold the imported vCal's
@test
*/
void CTestCalInterimApiImportExportAsync::ImportvCalAsyncL(RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	INFO_PRINTF1(_L("Asynchronously Importing opened vcs file....."));

	if (iDataExchange == NULL)
		{
		iDataExchange = CCalDataExchange::NewL(GetSession());
		}

	TRAPD(err,iDataExchange->ImportAsyncL(KUidVCalendar, aReadStream, aCalEntryArray, *this, 0));
	CActiveScheduler::Start();

	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Asynchronously Imported to Contact Entries"));
		}
	else
		{
		INFO_PRINTF1(_L("Asynchronously Importing failed"));
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
	}

void CTestCalInterimApiImportExportAsync::Progress(TInt aPercentageCompleted)
	{
	if ( iNotifyProgress )
		{
		INFO_PRINTF2(KInfoPercentageCompleted, aPercentageCompleted);
		}
	else
		{
		ERR_PRINTF1(KErrPercentageCompleted);
		SetTestStepResult(EFail);
		}
	}

void CTestCalInterimApiImportExportAsync::Completed()
	{
	INFO_PRINTF1(_L("Operation completed"));
	CActiveScheduler::Stop();
	}

TInt CTestCalInterimApiImportExportAsync::NumberOfEntriesToHandleAtOnce()
	{
	return KNumberOfEntriesToHandle;
	}

/** Opens vcs file and asynchronously imports the vcs file into entries, and stores in caledar database
@test
*/
void CTestCalInterimApiImportExportAsync::ImportAsyncL()
	{
	iField = KvCalNumberAsync;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntvCalAsyncNumber));// gets vCal number for asynchronously

	RPointerArray<CCalEntry> entriesImportedAsync;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImportedAsync));

	OpenVCSAndImportAsyncL(iPathVCS, iFsSession, iIntvCalAsyncNumber,entriesImportedAsync); // Imports vcf

	StoreEntryInDatabaseL(entriesImportedAsync);

	CleanupStack::PopAndDestroy(&entriesImportedAsync);// entriesImportedAsync.reset & entriesImportedAsync.destroy;
	}

/** Opens vcs file and imports the vcs file into entries, and then stores in calendar database
@internalTechnology
@test
*/
void CTestCalInterimApiImportExportAsync::ImportL()
	{
	GetvcsFileNumberFromIniFileL(); // gets vcs Number, so we can import correct vcs file

	RPointerArray<CCalEntry> entriesImported;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImported));

	OpenVCSAndImportL(iPathVCS, iFsSession, iIntvCalNumber,entriesImported); // Imports vcf

	StoreEntryInDatabaseL(entriesImported);

	CleanupStack::PopAndDestroy(&entriesImported);// entriesImported.reset & entriesImported.destroy;
	}

/** Fecthes all entries from database and then exports entries into a vcs file
@test
*/
void CTestCalInterimApiImportExportAsync::ExportL()
	{
	iField = KExportVcsFile;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iExportFilePath));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	ExportEntriesL(fetchEntryArray,iExportFilePath);

	CleanupStack::PopAndDestroy(&fetchEntryArray);// entryArray.reset & entryArray.destroy;
	}

/** Fetches all entries from databse and then asynchronously exports entries into a vcs file
@test
*/
void CTestCalInterimApiImportExportAsync::ExportAsyncL()
	{
	iField = KExportAsyncVcsFile;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iExportAsyncFilePath));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	ExportEntriesAsyncL(fetchEntryArray,iExportAsyncFilePath);

	CleanupStack::PopAndDestroy(&fetchEntryArray);// entryArray.reset & entryArray.destroy;
	}

/** Asynchronously exports entries into a vcs file
@param aFetchEntryArray The array to export asynchronously
@param	aExportVCSPath The path of vcs file to export
@test
*/
void CTestCalInterimApiImportExportAsync::ExportEntriesAsyncL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath)
	{
	TBuf<KMaxLengthString> buf;
	buf.Zero();
	buf.Append(aExportVCSPath);
	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KExportAsyncToVCSFile, &buf);

	if (iDataExchange == NULL)
		{
		iDataExchange = CCalDataExchange::NewL(GetSession());
		}

	RFile outFile;
	CleanupClosePushL(outFile);

	iFsSession.MkDirAll(buf);

	User::LeaveIfError(outFile.Replace(iFsSession, buf, EFileWrite));

	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);

	INFO_PRINTF1(message);

	iDataExchange->ExportAsyncL(KUidVCalendar, writeStream, aFetchEntryArray, *this);
	CActiveScheduler::Start();

	writeStream.CommitL();

	CleanupStack::PopAndDestroy(&writeStream); // fileStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outStream.Close()
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportExportAsyncTest - This class imports vCals Asynchronously, and then imports vCals,
// and checks for entries in database
CTestCalInterimApiImportExportAsyncTest::CTestCalInterimApiImportExportAsyncTest()
	{
	SetTestStepName(KTestCalInterimApiImportExportAsyncTest);
	}

CTestCalInterimApiImportExportAsyncTest::~CTestCalInterimApiImportExportAsyncTest()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportExportAsyncTest::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportExportAsyncTest::doTestStepL() start"));

	iField = KEntriesInDatabase;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntEntriesInDatabase));// gets vCal number for asynchronously

	iField = KTest;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntTestNumber));// gets the test number

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	switch (iIntTestNumber)
		{
		case(ETest1):
			CActiveScheduler::Start();
			ImportAsyncL(); // Imports a vcs file asynchronously
			ImportL();      // Imports a different vcs file
			break;

		case(ETest2):
			CActiveScheduler::Start();
			ImportAsyncL(); // Imports a vcs file asynchronously
			ExportL();		// Exports entries which are in database
			ImportL();		// Imports the exported entries
			break;

		case(ETest3):
			CActiveScheduler::Start();
			ExportAsyncL();	// Exports entries from database to vcs file asynchronously
			ExportL();		// Exports same entries to different vcs file
			break;

		case(ETest4):
			CActiveScheduler::Start();
			ExportAsyncL();	// Exports entries from database to vcs file asynchronously
			ImportL();		// Imports the exported entries, and stores entries in database
			ExportL();		// Exports the stored entries
			break;

		case(ETest5):
			CActiveScheduler::Start();
			ExportAsyncL();	// Export the entries from database to vcs file asynchronously
			ImportAsyncL(); // Imports the exported entries asynchronously
			break;

		case(ETest6):
			CActiveScheduler::Start();
			ImportAsyncL(); // Imports the exported entries asynchronously
			ExportAsyncL();	// Export the entries from database to vcs file asynchronously
			break;

		default:
			User::Leave(KErrNotFound);
		}

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	TESTL(iIntEntriesInDatabase == fetchEntryArray.Count());

	CleanupStack::PopAndDestroy(&fetchEntryArray);// entryArray.reset & entryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportExportAsyncTest::doTestStepL() finish"));
	return TestStepResult();
	}
