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

#include "TestCalInterimApiImportExportSoakStress.h"
#include <s32file.h>
#include <calentry.h>

// Class CTestCalInterimApiImportExportCreateStressData - This class prepares a calendar database, with large number of entries
CTestCalInterimApiImportExportCreateStressData::CTestCalInterimApiImportExportCreateStressData()
: 	iBoolIsFloating(FALSE)
	{
	SetTestStepName(KTestCalInterimApiImportExportCreateStressData);
	}

CTestCalInterimApiImportExportCreateStressData::~CTestCalInterimApiImportExportCreateStressData()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportExportCreateStressData::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportExportCreateStressData::doTestStepL() start"));

	iField = KNumberOfEntries;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iNumberOfEntries));

	RPointerArray<CCalEntry>  entriesToStore;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToStore));

	TBuf8<KMaxLengthUid> bufUid;
	bufUid.Zero();

	CCalEntry* entry = NULL;

	TCalTime startCal;
	TCalTime endCal;

	iField = KEntryStartDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrStartDate)); // gets expected start date for filter

	iField = KEntryEndDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEndDate)); // gets expected end date for filter

	iField = KTimesIsFloating;
	TESTL(GetBoolFromConfig(ConfigSection(),iField,iBoolIsFloating)); // gets expected end date for filter

	iStartdate.Set(iPtrStartDate);

	iEnddate.Set(iPtrEndDate);

	if (iBoolIsFloating)
		{
		SetCalTimeL(iStartdate, startCal, TCalTime::EFloating);
		SetCalTimeL(iEnddate, endCal, TCalTime::EFloating);
		}
	else
		{
		SetCalTimeL(iStartdate, startCal, TCalTime::EFixedUtc);
		SetCalTimeL(iEnddate, endCal, TCalTime::EFixedUtc);
		}

	for (TInt index = 0; index < iNumberOfEntries; index++)
		{
		bufUid.Append(_L8("SymbianOS"));
		bufUid.AppendNum(index);

		HBufC8* uidEntry = bufUid.AllocLC();

		if (index >= iNumberOfEntries/3 && index <= (iNumberOfEntries - (iNumberOfEntries/4)))
			{
			entry = CCalEntry::NewL(CCalEntry::EEvent, uidEntry, CCalEntry::EMethodAdd, 0);
			}
			else
			if (index >= (iNumberOfEntries - (iNumberOfEntries/4)))
				{
				entry = CCalEntry::NewL(CCalEntry::EAppt, uidEntry, CCalEntry::EMethodNone, 0);
				}
			else
				{
				entry = CCalEntry::NewL(CCalEntry::ETodo, uidEntry, CCalEntry::EMethodNone, 0);
				}

		CleanupStack::Pop(uidEntry);
		bufUid.Zero();

		entry->SetSummaryL(_L("Creating Large number of entries for stress testing"));
		entry->SetStartAndEndTimeL(startCal, endCal);

		entriesToStore.AppendL(entry);
		}

	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	StoreEntryInDatabaseL(entriesToStore);

	CleanupStack::PopAndDestroy(&entriesToStore); // entriesToStore.reset & entriesToStore.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportExportCreateStressData::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportExportCountEntry - This class verifies the number of entries in database
CTestCalInterimApiImportExportCountEntry::CTestCalInterimApiImportExportCountEntry()
	{
	SetTestStepName(KTestCalInterimApiImportExportCountEntry);
	}

CTestCalInterimApiImportExportCountEntry::~CTestCalInterimApiImportExportCountEntry()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportExportCountEntry::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportExportCountEntry::doTestStepL() start"));

	iField = KNumberOfEntries;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iNumberOfEntries));

	RPointerArray<CCalEntry> entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(entriesFetched);

	TESTL(iNumberOfEntries == entriesFetched.Count());
	INFO_PRINTF2(_L("%d entries have been imported"),iNumberOfEntries);

	CleanupStack::PopAndDestroy(&entriesFetched); // entriesToStore.reset & entriesToStore.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportExportCountEntry::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiExportSoak - This class fetches entries from database, then exports multiple times into a vcs file
CTestCalInterimApiExportSoak::CTestCalInterimApiExportSoak()
	{
	SetTestStepName(KTestCalInterimApiExportSoak);
	}

CTestCalInterimApiExportSoak::~CTestCalInterimApiExportSoak()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiExportSoak::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiExportSoak::doTestStepL() start"));

	iField = KExportCount;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iExportCount));

	iField = KExportVcsFile;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iExportFilePath));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	ExportEntriesSoakL(fetchEntryArray,iExportFilePath);

	CleanupStack::PopAndDestroy(&fetchEntryArray); // entryArray.reset & entryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiExportSoak::doTestStepL() finish"));
	return TestStepResult();
	}

/** Exports entries into a vcs file mulitple time (specified in ini file TestCalImportExportSoakStress.ini)
@param	aFetchEntryArray The array of entries to export to vcs file
@param	aExportVCSPath The path of the vcs file
@test
*/
void CTestCalInterimApiExportSoak::ExportEntriesSoakL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath)
	{
	TBuf<KMaxLengthString> buf;
	buf.Zero();
	buf.Append(aExportVCSPath);
	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KExportToVCSFile, &buf);

	RFile outFile;
	CleanupClosePushL(outFile);

	iFsSession.MkDirAll(buf);

	User::LeaveIfError(outFile.Replace(iFsSession, buf, EFileWrite));

	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);

	iDataExchange = CCalDataExchange::NewL(GetSession());

	INFO_PRINTF1(message);

	for (TInt index  = 0; index < iExportCount; index++)
		{
		INFO_PRINTF2(_L("Export Operation:%d"), index+1);
		iDataExchange->ExportL(KUidVCalendar, writeStream, aFetchEntryArray);
		}

	writeStream.CommitL();

	CleanupStack::PopAndDestroy(&writeStream); // fileStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outStream.Close()
	}
