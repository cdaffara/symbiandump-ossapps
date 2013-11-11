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

#include "TestCalInterimApiImportExport.h"
#include <calalarm.h>
#include <caliterator.h>
#include <calrrule.h>
#include <fbs.h>
#include <s32file.h>
#include <e32test.h>
#include <e32std.h>
#include <caltime.h>

// Class CTestCalInterimApiImportExport - Common functions that are used between the import/export steps
CTestCalInterimApiImportExport::CTestCalInterimApiImportExport()
:	iIntvCalNumber(0)
,	iEntryView(NULL)
,	iPathVCS(KPathImportvCals)
,	iVerification(EFalse)
,	iField(0)
,	iDataExchange(NULL)
	{
	}

CTestCalInterimApiImportExport::~CTestCalInterimApiImportExport()
	{
	delete iDataExchange;
	delete iEntryView;
	}

/** Opens vcs file and imports the vcs file into entries
@param	aPath The path of the vcs file to open
@param	aFsSession A handle to a file server session
@param	avCalNumber The number of vCal to import
@param	aCalEntryArray The array which will contain the imported vCal's
@test
*/
void CTestCalInterimApiImportExport::OpenVCSAndImportL(TDes& aPath, RFs aFsSession, TInt avCalNumber, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	RFileReadStream readVCSStream;
	CleanupClosePushL(readVCSStream);

	// Format buf to hold relevant vcs file
	TFileName vCalFile;
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

	User::LeaveIfError(openError);

	TBuf<MAX_TEXT_MESSAGE> message;
	message.Format(KOpenVCSFile, &vCalFile);
	INFO_PRINTF1(message);

	// Imports the vCal
	ImportvCalL(readVCSStream, aCalEntryArray);

	CleanupStack::PopAndDestroy(); // pop readVCSStream
	}

/** Imports vCals into an array
@param	aReadStream The read stream which the vCals will be read from
@param	aCalEntryArray The array which will hold the imported vCal's
@test
*/
void CTestCalInterimApiImportExport::ImportvCalL(RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray)
	{
	INFO_PRINTF1(_L("Importing opened vcs file....."));

	if ( iDataExchange == NULL )
		{
		iDataExchange = CCalDataExchange::NewL(GetSession());
		}

	TRAPD(err, iDataExchange->ImportL(KUidVCalendar, aReadStream, aCalEntryArray));

	if (err == KErrNone)
		{
		INFO_PRINTF1(_L("Imported to Calendar Entry"));
		}
	else
		{
		INFO_PRINTF2(_L("Importing failed Error Code: %d"), err);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
	}

/** Gets vcs file number from the ini file (TestCalImportExport.ini)
@test
*/
void CTestCalInterimApiImportExport::GetvcsFileNumberFromIniFileL()
	{
	iField = KvCalNumber;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntvCalNumber)); // gets vCal number
	}

/** Stores all entries in calendar database
@param	aEntriesImported The array of entries to store
@test
*/
void CTestCalInterimApiImportExport::StoreEntryInDatabaseL(RPointerArray<CCalEntry>& aEntriesImported)
	{
	INFO_PRINTF1(KInfoAddingEntryToDatabase);
	TInt entriesStored = 0;
	TRAPD(err, iEntryView->StoreL(aEntriesImported, entriesStored));

	iField = KVerifyImStore;
	GetBoolFromConfig(ConfigSection(),iField,iVerifyImportAgainstStore);

	if (!iVerifyImportAgainstStore)
		{
		if( err == KErrNone )
			{
			TESTL(entriesStored == aEntriesImported.Count());
			INFO_PRINTF1(KInfoEntryAdded);
			}
		else if (err == KErrLocked)
			{
			_LIT(KInfofileLocked,				"File is locked");
			INFO_PRINTF1(KInfofileLocked);
			}
		else
			{
			ERR_PRINTF1(_L("Entry is not stored successfully"));
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	}

/** Fecthes all entries that are present in database
@param	aFetchEntryArray The array of entries that are fetched from database
@test
*/
void CTestCalInterimApiImportExport::FetchAllEntryFromDatabaseL(RPointerArray<CCalEntry>& aFetchEntryArray)
	{
	INFO_PRINTF1(_L("Fetching all entries from database"));
	CCalIter* iter = CCalIter::NewL(GetSession());
	CleanupStack::PushL(iter);
	TPtrC8 Uid(iter->FirstL());

	while (Uid != KNullDesC8())
		{
		TRAPD(err, iEntryView->FetchL(Uid, aFetchEntryArray));

		if(err == KErrNone)
			{
			Uid.Set(iter->NextL());
			}
		else
			{
			ERR_PRINTF1(_L("Entry is not fetched successfully"));
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}

	INFO_PRINTF1(_L("Fetched all entries from database"));
	CleanupStack::PopAndDestroy(iter);
	}

/** Export entries into a vcs file
@param	aFetchEntryArray The array of entries to export
@param	aExportVCSPath The path of vcs file to export to
@test
*/
void CTestCalInterimApiImportExport::ExportEntriesL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath)
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

	if (iDataExchange == NULL)
		{
		iDataExchange = CCalDataExchange::NewL(GetSession());
		}

	INFO_PRINTF1(message);

	TRAPD(err,iDataExchange->ExportL(KUidVCalendar, writeStream, aFetchEntryArray))

	if (err!=KErrNone)
		{
		ERR_PRINTF1(_L("Exporting Failed"));
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}

	writeStream.CommitL();

	CleanupStack::PopAndDestroy(&writeStream); // fileStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outStream.Close()
	}
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiImportvCal - Imports vcs file, stores entries, fetch entries and compares times
CTestCalInterimApiImportvCal::CTestCalInterimApiImportvCal()
:	iTimeMode(TCalTime::EFixedUtc)
	{
	SetTestStepName(KTestCalInterimApiImportvCal);
	}

CTestCalInterimApiImportvCal::~CTestCalInterimApiImportvCal()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportvCal::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("TestCalInterimApiImportvCal::doTestStepL() start"));

	iField = KVerification;
	TESTL(GetBoolFromConfig(ConfigSection(),iField,iVerification));

	iField = KNoStartEndTest;
	GetStringFromConfig(ConfigSection(),iField,iPtrTest);

	GetvcsFileNumberFromIniFileL(); // gets vcs Number, so we can import correct vcs file

	RPointerArray<CCalEntry> entriesImported;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImported));

	OpenVCSAndImportL(iPathVCS, iFsSession, iIntvCalNumber,entriesImported); // Imports vcf

	iField.Zero();
	iField = KExpectedEntryImport;
	GetIntFromConfig(ConfigSection(),iField,iExpNumImport);

	if (iPtrTest.Compare(KNoStartEnd) == 0 || (iExpNumImport!=0))
		{
		//Gets expected number of entry that should be imported
		iField.Zero();
		iField = KExpectedEntryImport;
		INFO_PRINTF2(_L("Imported Entries: %d"), entriesImported.Count());
		TESTL(iExpNumImport == entriesImported.Count());
		}

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	StoreEntryInDatabaseL(entriesImported);

	if (iVerification)
		{
		for (TInt number = 0; number < entriesImported.Count(); ++number )
			{
			INFO_PRINTF2(_L("Entry:- %d"), number);

			//Prints out UID of entry
 			HBufC* uID = HBufC::NewLC(entriesImported[number]->UidL().Length());
 			uID->Des().Copy(entriesImported[number]->UidL());
 			INFO_PRINTF2(KInfoGUID, uID);

			CheckEntriesTimesL(entriesImported, number);

			CleanupStack::PopAndDestroy(uID);
			}
		}
		
	iField = KTestInvalidGEOTest;
	GetIntFromConfig(ConfigSection(),iField,iGeoInvalidTest); // determines whether to carry out invalid GEO test
	
	if (iGeoInvalidTest)
		{
		CheckInvalidGeoValueL(entriesImported);
		}

	CleanupStack::PopAndDestroy(&entriesImported); // entriesImported.reset & entriesImported.destroy;

	INFO_PRINTF1(_L("TestCalInterimApiImportvCal::doTestStepL() finish"));
	return TestStepResult();
	}

/** Gets data from ini file TestCalImportExport.ini
@param	avCalNumber This is to ensure it fecthes the correct expected times
@test
*/
void CTestCalInterimApiImportvCal::GetDataFromIniFileL(TInt avCalNumber)
	{
	iField.Zero();
	iField.Append(KExpectedStartTime);
	iField.AppendNum(avCalNumber);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedStartTime)); // gets expected start time

	iField.Zero();
	iField.Append(KExpectedEndTime);
	iField.AppendNum(avCalNumber);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedEndTime));	 // gets expected end time

	iField.Zero();
	iField.Append(KExpectedDTStamp);
	iField.AppendNum(avCalNumber);
	GetStringFromConfig(ConfigSection(),iField,iPtrExpectedDTStamp);	 // gets expected DTStamp

	iField.Zero();
	iField.Append(KExpectedAlarmOffset);
	iField.AppendNum(avCalNumber);
	GetIntFromConfig(ConfigSection(),iField,iIntAlarmOffset); // gets expected alarm time

	iField = KTimeMode;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iTimeModeEntry)); // gets expected time mode of entry
	}

/** Checks for the start time, end time, alarm offset and time mode
@param	aEntriesImported Contains the imported vCals, to check the times
@test
*/
void CTestCalInterimApiImportvCal::CheckEntriesTimesL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryArray));
	TRAPD(err, iEntryView->FetchL(aEntriesImported[aIndex]->UidL(), entryArray));

	if (iPtrTest.Compare(KNoStartEnd))
		{
		GetDataFromIniFileL(aIndex);
		}

	if (iTimeModeEntry == KTimeModeFloating)
		{
		iTimeMode = TCalTime::EFloating;
		}

	if(iPtrTest == KNullDesC)
		{
		INFO_PRINTF1(KInfoFetchedEntryFromDatabase);

		TDateTime importedTime;
		TDateTime importedEndTime;

		if (iTimeModeEntry == KTimeModeFloating)
			{
			//Here the index is 0, as there are no child entries present, parent and child entries included in separate test
			importedTime = entryArray[0]->StartTimeL().TimeLocalL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == entryArray[0]->StartTimeL().TimeLocalL());
			importedEndTime = entryArray[0]->EndTimeL().TimeLocalL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedEndTime) == entryArray[0]->EndTimeL().TimeLocalL());
			}
		else
			{
			//Here the index is 0, as there are no child entries present
			importedTime = entryArray[0]->StartTimeL().TimeUtcL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == entryArray[0]->StartTimeL().TimeUtcL());
			importedEndTime = entryArray[0]->EndTimeL().TimeUtcL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedEndTime) == entryArray[0]->EndTimeL().TimeUtcL());
			}

		TDateTime expectedTime = FormatDateTime(iPtrExpectedStartTime).DateTime();

		INFO_PRINTF7(_L("Imported Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedTime.Year(), importedTime.Month() + 1, importedTime.Day() + 1, importedTime.Hour(), importedTime.Minute(), importedTime.Second());
		INFO_PRINTF7(_L("Expected Start Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), expectedTime.Year(), expectedTime.Month() + 1, expectedTime.Day() + 1, expectedTime.Hour(), expectedTime.Minute(), expectedTime.Second());

		//Here the index is 0, as there are no child entries present
		TESTL(entryArray[0]->StartTimeL().TimeMode() == iTimeMode);
		expectedTime = FormatDateTime(iPtrExpectedEndTime).DateTime();

		INFO_PRINTF7(_L("Imported End Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedEndTime.Year(), importedEndTime.Month() + 1, importedEndTime.Day() + 1, importedEndTime.Hour(), importedEndTime.Minute(), importedEndTime.Second());
		INFO_PRINTF7(_L("Expected End Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), expectedTime.Year(), expectedTime.Month() + 1, expectedTime.Day() + 1, expectedTime.Hour(), expectedTime.Minute(), expectedTime.Second());

		TESTL(entryArray[0]->EndTimeL().TimeMode() == iTimeMode);

		if (entryArray[0]->CompletedTimeL().TimeLocalL() == Time::NullTTime())
			{
			// If completed time equals NULL, then alarm should be settable
			CCalAlarm* alarm1 = entryArray[0]->AlarmL();
			TESTL(alarm1->TimeOffset().Int() == iIntAlarmOffset);
			INFO_PRINTF1(KImportedAlarmOffset);
			}

		CheckDTStampL(aEntriesImported, aIndex);
		INFO_PRINTF1(KImportedTimesCorrect);

		if(aEntriesImported[aIndex]->EntryTypeL() == CCalEntry::ETodo)
			{
			CheckToDoCompletedTimeL(aEntriesImported, aIndex);
			}
		else
			{
			TESTL(entryArray[0]->CompletedTimeL().TimeLocalL() == Time::NullTTime());
			}
		}
	else if (iPtrTest.Compare(KNoStart) == 0)
		{
		INFO_PRINTF1(KInfoFetchedEntryFromDatabase);
		CheckNoStartEntryL(aEntriesImported, aIndex);
		}
	else if(iPtrTest.Compare(KNoEnd) == 0)
		{
		INFO_PRINTF1(KInfoFetchedEntryFromDatabase);
		CheckNoEndEntryL(aEntriesImported, aIndex);
		}
	else if(iPtrTest.Compare(KNoStartEnd) == 0)
		{
		INFO_PRINTF1(KInfoFetchedEntryFromDatabase);
		CheckNoEndStartEntryL(aEntriesImported, aIndex);
		}
	else
		{
		ERR_PRINTF1(_L("Entry is not fetched successfully"));
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}

	CleanupStack::PopAndDestroy(&entryArray);// entryArray.reset & entryArray.destroy;
	}

/** Checks that the GEO property value is not set due to invalid importing of GEO values
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
*/
void CTestCalInterimApiImportvCal::CheckInvalidGeoValueL(RPointerArray<CCalEntry>& aEntriesImported)
	{
	for (TInt index = 0; index < aEntriesImported.Count(); ++index)
			{
			RPointerArray<CCalEntry> entryArray;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryArray));
	
			INFO_PRINTF2(_L("Entry: %d"), index + 1);
			TRAPD(err, iEntryView->FetchL(aEntriesImported[index]->UidL(), entryArray));
			if (err == KErrNone)
				{
				CCalGeoValue* geoValue = CCalGeoValue::NewL();
		
				TRAP(err, geoValue = entryArray[KIndexArrayForParentEntry]->GeoValueL());
				CleanupStack::PushL(geoValue);
					
				if (err != KErrNone)
					{
					ERR_PRINTF2(_L("GeoValuesL Error Code %d"), err);
					SetTestStepResult(EFail);
					SetTestStepError(err);
					}
				
				TESTL(geoValue == NULL);
				INFO_PRINTF1(_L("GEO Property Values not set to default value invalid GEO value/format or not specified in vCal"));

				CleanupStack::PopAndDestroy(geoValue);	
				}
			else
				{
				ERR_PRINTF1(_L("Entry is not fetched successfully"));
				SetTestStepResult(EFail);
				SetTestStepError(err);
				}
				
			CleanupStack::PopAndDestroy(&entryArray); // entryArray.reset & entryArray.destroy;
			}
	}
	
/** Checks for start time, end time and time mode, for entries without start time
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckNoStartEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	TDateTime importedStartTime;
	TDateTime importedEndTime;

	CCalEntry::TType entryType = aEntriesImported[aIndex]->EntryTypeL();

	GetDataFromIniFileL(aIndex);

	if (entryType == CCalEntry::EAppt ||
		entryType == CCalEntry::EEvent ||
		entryType == CCalEntry::EAnniv) //anniv end time becomes start time
		{
		CheckApptEventAnnivTimesL(aEntriesImported, aIndex);
		}

	if (entryType == CCalEntry::EReminder)
		{
		// reminder end = null and start becomes end
		if (iTimeModeEntry == KTimeModeFloating)
			{
			TESTL(aEntriesImported[aIndex]->EndTimeL().TimeLocalL() == Time::NullTTime());
			importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeLocalL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeLocalL());
			}
		else
			{
			TESTL(aEntriesImported[aIndex]->EndTimeL().TimeUtcL() == Time::NullTTime());
			importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeUtcL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeUtcL());
			}

		INFO_PRINTF1(_L("End time of Entry is NULL"));
		TESTL(aEntriesImported[aIndex]->StartTimeL().TimeMode() == iTimeMode);
		INFO_PRINTF7(_L("Imported Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedStartTime.Year(), importedStartTime.Month() + 1, importedStartTime.Day() + 1, importedStartTime.Hour(), importedStartTime.Minute(), importedStartTime.Second());
		}

	if (entryType == CCalEntry::ETodo)
		{
		if (iTimeModeEntry == KTimeModeFloating)
			{
			TESTL(aEntriesImported[aIndex]->StartTimeL().TimeLocalL() == aEntriesImported[aIndex]->EndTimeL().TimeLocalL());
			importedEndTime = aEntriesImported[aIndex]->EndTimeL().TimeLocalL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntriesImported[aIndex]->EndTimeL().TimeLocalL());
			}
		else
			{
			TESTL(aEntriesImported[aIndex]->StartTimeL().TimeUtcL() == aEntriesImported[aIndex]->EndTimeL().TimeUtcL());
			importedEndTime = aEntriesImported[aIndex]->EndTimeL().TimeUtcL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntriesImported[aIndex]->EndTimeL().TimeUtcL());
			}

		INFO_PRINTF1(_L("Start time of Entry is NULL"));

		TESTL(aEntriesImported[aIndex]->EndTimeL().TimeMode() == iTimeMode);
		INFO_PRINTF7(_L("Imported Due Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedEndTime.Year(), importedEndTime.Month() + 1, importedEndTime.Day() + 1, importedEndTime.Hour(), importedEndTime.Minute(), importedEndTime.Second());

		CheckToDoCompletedTimeL(aEntriesImported, aIndex);
		}
	}

/** Checks for start time, end time and time mode, for entries without end time
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckNoEndEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	TDateTime importedStartTime;
	TDateTime importedEndTime;

	CCalEntry::TType entryType = aEntriesImported[aIndex]->EntryTypeL();

	GetDataFromIniFileL(aIndex);

	if (entryType == CCalEntry::EAppt ||
		entryType == CCalEntry::EEvent ||
		entryType == CCalEntry::EAnniv
		)
		{
		CheckApptEventAnnivTimesL(aEntriesImported, aIndex);
		}

	if (entryType == CCalEntry::ETodo ||
		entryType == CCalEntry::EReminder)
		{
		if (iTimeModeEntry == KTimeModeFloating)
			{
			if (entryType == CCalEntry::EReminder)
				{
				TESTL(aEntriesImported[aIndex]->EndTimeL().TimeLocalL() == Time::NullTTime());
				}
			else
				{
				TESTL(aEntriesImported[aIndex]->StartTimeL().TimeLocalL() == aEntriesImported[aIndex]->EndTimeL().TimeLocalL());
				}
			importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeLocalL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeLocalL());
			}
		else
			{
			if (entryType == CCalEntry::EReminder)
				{
				TESTL(aEntriesImported[aIndex]->EndTimeL().TimeLocalL() == Time::NullTTime());
				}
			else
				{
				TESTL(aEntriesImported[aIndex]->StartTimeL().TimeUtcL() == aEntriesImported[aIndex]->EndTimeL().TimeUtcL());
				}
			importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeUtcL().DateTime();
			TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeUtcL());
			}

		INFO_PRINTF1(_L("End time of Entry is NULL"));
		TESTL(aEntriesImported[aIndex]->StartTimeL().TimeMode() == iTimeMode);
		INFO_PRINTF7(_L("Imported Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedStartTime.Year(), importedStartTime.Month() + 1, importedStartTime.Day() + 1, importedStartTime.Hour(), importedStartTime.Minute(), importedStartTime.Second());

		CheckToDoCompletedTimeL(aEntriesImported, aIndex);
		}
	}

/** Checks for start time, end time and time mode, for entries without end  and start time
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckNoEndStartEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	TDateTime importedStartTime;
	TDateTime importedEndTime;

	if (aEntriesImported[aIndex]->EntryTypeL() == CCalEntry::ETodo)
		{
		TESTL(aEntriesImported[aIndex]->EndTimeL().TimeLocalL() == Time::NullTTime());
		INFO_PRINTF1(_L("End time of Entry is NULL"));
		TESTL(aEntriesImported[aIndex]->StartTimeL().TimeLocalL() == Time::NullTTime());
		INFO_PRINTF1(_L("Start time of Entry is NULL"));

		CheckToDoCompletedTimeL(aEntriesImported, aIndex);
		}
	}

/** Checks for start time, end time and time mode, for Appt, Event and Anniv entries
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckApptEventAnnivTimesL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	TDateTime importedStartTime;
	TDateTime importedEndTime;

	if (iTimeModeEntry == KTimeModeFloating)
		{
		importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeLocalL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeLocalL());
		importedEndTime = aEntriesImported[aIndex]->EndTimeL().TimeLocalL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntriesImported[aIndex]->EndTimeL().TimeLocalL());
		TESTL(aEntriesImported[aIndex]->StartTimeL().TimeLocalL() == aEntriesImported[aIndex]->EndTimeL().TimeLocalL());
		}
	else
		{
		importedStartTime = aEntriesImported[aIndex]->StartTimeL().TimeUtcL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntriesImported[aIndex]->StartTimeL().TimeUtcL());
		importedEndTime = aEntriesImported[aIndex]->EndTimeL().TimeUtcL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntriesImported[aIndex]->EndTimeL().TimeUtcL());
		TESTL(aEntriesImported[aIndex]->StartTimeL().TimeUtcL() == aEntriesImported[aIndex]->EndTimeL().TimeUtcL());
		}

	TESTL(aEntriesImported[aIndex]->StartTimeL().TimeMode() == iTimeMode);
	TESTL(aEntriesImported[aIndex]->EndTimeL().TimeMode() == iTimeMode);
	INFO_PRINTF7(_L("Imported Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedStartTime.Year(), importedStartTime.Month() + 1, importedStartTime.Day() + 1, importedStartTime.Hour(), importedStartTime.Minute(), importedStartTime.Second());
	INFO_PRINTF7(_L("Imported End Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedEndTime.Year(), importedEndTime.Month() + 1, importedEndTime.Day(), importedEndTime.Hour(), importedEndTime.Minute(), importedEndTime.Second());
	}

/** Checks for to do completed time
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckToDoCompletedTimeL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryArray));
	TRAPD(err, iEntryView->FetchL(aEntriesImported[aIndex]->UidL(), entryArray));

	_LIT(KNULL,"NULL");
	_LIT(KOUTSIDEYEAR,"OUTSIDEYEAR");

	TDateTime importedCompletedTime;

	iField.Zero();
	iField.Append(KExpectedCompletedTime);
	iField.AppendNum(aIndex);
	GetStringFromConfig(ConfigSection(),iField,iPtrExpectedCompletedTime); // gets expected completed time

	if (err == KErrNone)
		{
		if (iPtrExpectedCompletedTime != KNullDesC)
			{
			if (!iPtrExpectedCompletedTime.Compare(KNULL()) || !iPtrExpectedCompletedTime.Compare(KOUTSIDEYEAR()))
				{
				if (iTimeModeEntry == KTimeModeFloating)
					{
					TESTL(entryArray[0]->CompletedTimeL().TimeLocalL() == Time::NullTTime());
					}
				else
					{
					TESTL(entryArray[0]->CompletedTimeL().TimeUtcL() == Time::NullTTime());
					}

				INFO_PRINTF1(_L("Completed time of Entry is NULL"));
				}
			else
				{
				if (iTimeModeEntry == KTimeModeFloating)
					{
					importedCompletedTime = entryArray[0]->CompletedTimeL().TimeUtcL().DateTime();
					TESTL(FormatDateTime(iPtrExpectedCompletedTime) == entryArray[0]->CompletedTimeL().TimeUtcL());
					}
				else
					{
					importedCompletedTime = entryArray[0]->CompletedTimeL().TimeUtcL().DateTime();
					TESTL(FormatDateTime(iPtrExpectedCompletedTime) == entryArray[0]->CompletedTimeL().TimeUtcL());
					}

				TESTL(entryArray[0]->CompletedTimeL().TimeMode() == TCalTime::EFixedUtc);
				INFO_PRINTF1(_L("Completed time of Todo Entry has been imported correctly"));
				}
			}
		}
		else
			{
			INFO_PRINTF2(_L("FetchL failed Error Code: %d"), err);
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}

	CleanupStack::PopAndDestroy(&entryArray);// entryArray.reset & entryArray.destroy;
	}

/** Checks for DTStamp date/time
@param	aEntriesImported Contains the imported vCals, to check the times
@param	aIndex The index to be used to fecth the correct entry in aEntriesImported
@test
*/
void CTestCalInterimApiImportvCal::CheckDTStampL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryArray));
	TRAPD(err, iEntryView->FetchL(aEntriesImported[aIndex]->UidL(), entryArray));

	_LIT(KNULL,"NULL");
	_LIT(KINVALIDYEAR, "OUTSIDEYEARRANGE");

	TDateTime importedDTStamp;

	if (err == KErrNone)
		{
		if (!iPtrExpectedDTStamp.Compare(KNULL()) || !iPtrExpectedDTStamp.Compare(KINVALIDYEAR()))
			{
			if (iTimeModeEntry == KTimeModeFloating)
				{
				TESTL(entryArray[0]->DTStampL().TimeLocalL() == Time::NullTTime());
				}
			else
				{
				TESTL(entryArray[0]->DTStampL().TimeUtcL() == Time::NullTTime());
				}

			INFO_PRINTF1(_L("DTStamp time of Entry is NULL"));
			}
		else
			{
			//dont need to check floating dtstamp, as always imported as FixedUtc
			importedDTStamp = entryArray[0]->DTStampL().TimeUtcL().DateTime();
			INFO_PRINTF7(_L("Imported dtstamp Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedDTStamp.Year(), importedDTStamp.Month() + 1, importedDTStamp.Day() + 1, importedDTStamp.Hour(), importedDTStamp.Minute(), importedDTStamp.Second());
			TESTL(FormatDateTime(iPtrExpectedDTStamp) == entryArray[0]->DTStampL().TimeUtcL());

			TESTL(entryArray[0]->DTStampL().TimeMode() == TCalTime::EFixedUtc);
			INFO_PRINTF1(_L("DTStamp time of Entry has been imported correctly"));
			}
		}
	else
		{
		INFO_PRINTF2(_L("FetchL failed Error Code: %d"), err);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}

	CleanupStack::PopAndDestroy(&entryArray);// entryArray.reset & entryArray.destroy;
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiExport - Opens database (specified in ini file), then exports into
// vcs file (path specified in ini file) and checks if properties have been exported correctly
CTestCalInterimApiExport::CTestCalInterimApiExport()
	{
	SetTestStepName(KTestCalInterimApiExport);
	}

CTestCalInterimApiExport::~CTestCalInterimApiExport()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiExport::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiExport::doTestStepL() start"));

	iField = KVerification;
	TESTL(GetBoolFromConfig(ConfigSection(),iField,iVerification));

	iField = KExportVcsFile;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iExportFilePath));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	ExportEntriesL(fetchEntryArray,iExportFilePath);

	if (iVerification)
		{
		iField = KExpectedExportVcsFile;
		TESTL(GetStringFromConfig(ConfigSection(),iField,iExpectedExportFilePath));

		// Verifies if repeat rules has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KRRULE());

		// Verifies if summary has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KSUMMARY());

		// Verifies if description has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KDESCRIPTION());

		// Verifies if categories has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KCATEGORIES());

		// Verifies if priority has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KPRIORITY());

		// Verifies if location has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KLOCATION());

		// Verifies if sequence number has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KSEQUENCE());

		// Verifies if status of entry has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KSTATUS());

		// Verifies if the replication status of entry has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KREPLICATION());

		// Verifies if the transparency status of entry has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KTRANSP());

		// Verifies if the geo property value of entry has been imported and exported correctly
		CompareFileL(iExpectedExportFilePath, iExportFilePath, KGEO());
		}

	iField = KNoField;
	GetStringFromConfig(ConfigSection(),iField,iNoField);

	if (iNoField != KNullDesC)
		{
		CheckForNoFieldL(iExportFilePath);
		}

	CleanupStack::PopAndDestroy(&fetchEntryArray); // entryArray.reset & entryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiExport::doTestStepL() finish"));
	return TestStepResult();
	}

/** Compares 2 vcs fie
@param	aExpectedFile The expected vcs file
@param	aFile This is the vcs file produced, and will be compared against aExpectedFile
@test
*/
void CTestCalInterimApiExport::CompareFileL(const TDesC& aExpectedFile, const TDesC& aFile, const TDesC8& aProperty)
	{
	INFO_PRINTF3(_L("Comparing files: %S and %S"), &aExpectedFile, &aFile);

	RFile fileExpected, fileOutput;

	CleanupClosePushL(fileExpected);
	CleanupClosePushL(fileOutput);

	TInt err;
	TInt err1;
	
	TFileName expectedFileName;
#ifdef __WINS__
	expectedFileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	expectedFileName.Append(parse.Drive());
#endif
	expectedFileName.Append(aExpectedFile);
	
	err = fileExpected.Open(iFsSession, expectedFileName, EFileRead);

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aExpectedFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		User::Leave(err);
		}

	err = fileOutput.Open(iFsSession, aFile, EFileRead);
	if (err!=KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		User::Leave(err);
		}

	RFileReadStream streamExpected(fileExpected);
	RFileReadStream streamOutput(fileOutput);
	CleanupClosePushL(streamExpected);
	CleanupClosePushL(streamOutput);

	TBuf8<0x80> bufExpected,bufOutput;
	TBool isProperty = EFalse;
	TBool isPropertyExpected = EFalse;
	TBool foundBothProperty = EFalse;

	do  // This do loop iterates through both files, until the end of file is found
		{
		do // This do loop iterates thorugh both files until both properties are found and compares them
			{
			foundBothProperty = EFalse;
			if (!isProperty)
				{
				TRAP(err, streamExpected.ReadL(bufExpected, TChar(0xa)));
				}

			if (!isPropertyExpected)
				{
				TRAP(err1, streamOutput.ReadL(bufOutput, TChar(0xa)));
				}

			if (err != KErrEof || err1 != KErrEof)
				{
				isProperty = IsProperty(bufExpected, aProperty); // checks if aProperty
				isPropertyExpected = IsProperty(bufOutput, aProperty); // checks if aProperty
				}
			else
				{
				break;
				}
			if ((isProperty) && (isPropertyExpected))
				{
				foundBothProperty = ETrue;
				}
			} while (!foundBothProperty); // exit 2nd do loop when both properties found

		// Exists 2nd do loop, so both properties are found
		if (err != KErrEof && err1 != KErrEof)
			{
			CompareLinesL(bufExpected, bufOutput); // compares both properties
			}

		// After comparing, gets the next property parameters in vcs file, if we have not reached end of file
		TRAP(err, streamExpected.ReadL(bufExpected, TChar(0xa)));
		TRAP(err1, streamOutput.ReadL(bufOutput, TChar(0xa)));
		} while (err != KErrEof || err1 != KErrEof); // exits 2nd do loop when its reached the end of either file

	CleanupStack::PopAndDestroy(4, &fileExpected);
	}

/** Checks if the line of data contains the property (aProperty) parameter
@param	aLine The line of data to check
@return	True or False indicating if property found in the line of data
@test
*/
TBool CTestCalInterimApiExport::IsProperty(const TDesC8& aLine, const TDesC8& aProperty)
	{
	TInt propertyname = aProperty.Length();
	TBufC8<15> first6char(aLine.Left(propertyname));

	if (!first6char.Compare(aProperty))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Compares two lines of data
@param	aBufExpected The line of data to compare with aBufOutput
@param	aBufOutput The second line of data to compare to aBufExpected
@test
*/
void CTestCalInterimApiExport::CompareLinesL(const TDesC8& aBufExpected, const TDesC8& aBufOutput)
	{
	TInt geo = aBufExpected.Find(KGEO);

	// if the buf expected contains the GEO property and conatins the '+' sign
	// then we need to remove the '+' sign, as this will not be exported
	// i.e.  If the import file specifies GEO:+1,0, then this will be exported as GEO:1,0
	// therefore we need to modify GEO:+1,0 to GEO:1,0 as it can be compared successfully
	if ( geo != KErrNotFound )
		{
		TInt plusSign = aBufExpected.Find(KPLUSSIGN);

		if ( plusSign != KErrNotFound )
			{
			TInt dataLength1 = aBufExpected.Length();
			TBufC8<100> modBufExpected1(aBufExpected.Right(dataLength1 - plusSign - 1));
			
			TInt butoutputLength1 = aBufOutput.Length();
			TInt colonPosOutput = aBufOutput.Find(KCOLON);
			TBufC8<100> modBufOutput1(aBufOutput.Right(butoutputLength1 - colonPosOutput - 1));
			
			TESTL(!modBufExpected1.Compare(modBufOutput1));
			}
		else
			{
			TESTL(!aBufExpected.Compare(aBufOutput));
			}
		}
	else
		{
		TInt cat = aBufExpected.Find(KCATEGORIES);
		TInt xprefix = aBufExpected.Find(KCATXPREFIX);
		
		if ( cat != KErrNotFound && xprefix != KErrNotFound)
			{
			// if buf expected contains the category property (with the X- prefix i.e. CATEGORIES:X-An
			// modify to remove the X- prefix as the prefix should not be exported, and then compare with aBufOuput
			TInt dataLength = aBufExpected.Length();
			TBufC8<100> modBufExpected(aBufExpected.Right(dataLength - xprefix - 2));
			
			TInt butoutputLength = aBufOutput.Length();
			TInt colonPos = aBufOutput.Find(KCOLON);
			TBufC8<100> modBufOutput(aBufExpected.Right(butoutputLength - colonPos -1));
			
			TESTL(!modBufExpected.Compare(modBufOutput));
			}
		else
			{
			iField = KExpectedRRule;
			GetStringFromConfig(ConfigSection(),iField,iExpectedRRule);
			
			TInt rrule = aBufOutput.Find(KRRULE);
	
			if (rrule != KErrNotFound)
				{
				if (iExpectedRRule != KNullDesC)
					{
					CheckRRule(iExpectedRRule, aBufOutput);
					return;
					}
				}

			TESTL(!aBufExpected.Compare(aBufOutput));
			}
		}
		
	INFO_PRINTF1(_L("Property have been imported and exported correctly"));
	}

/** Checks that the RRule is as expected
@param	aExpectedRRule The expected RRule to be exported
@param	aBufOutput The actual RRule that has been exported
*/
void CTestCalInterimApiExport::CheckRRule(const TDesC& aExpectedRRule, const TDesC8& aBufOutput)
	{
	TInt dataLength = aBufOutput.Length();
	TBufC8<50> bufoutput(aBufOutput.Left(dataLength - 2));
			
 	HBufC16* output = HBufC16::NewLC(bufoutput.Length());
 	output->Des().Copy(bufoutput);
 	
	TESTL( aExpectedRRule.Compare( output->Des() ) == 0);
	
 	CleanupStack::PopAndDestroy(output);
	}

/** Checks for any field that should have not been exported
@param	aFile The vcs file to open
@test
*/
void CTestCalInterimApiExport::CheckForNoFieldL(const TDesC& aFile)
	{
	INFO_PRINTF2(_L("Opening file........: %S"), &aFile);

	RFile fileOutput;

	TRAPD(err, fileOutput.Open(iFsSession, aFile, EFileRead));
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("Unable to open file: %S"), &aFile);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		User::Leave(err);
		}

	CleanupClosePushL(fileOutput);

	RFileReadStream streamOutput(fileOutput);
	CleanupClosePushL(streamOutput);

	TBuf8<0x80> bufOutput;

	do
		{
		TRAP(err, streamOutput.ReadL(bufOutput, TChar(0xa)));
		if (iNoField == _L("DTSTART"))
			{
			TESTL(!IsStartPresent(bufOutput));
			}
		else
			if(iNoField == _L("SUMMARY"))
				{
				TESTL(!IsSummaryPresent(bufOutput));
				}
			else
				if(iNoField == _L("DAYLIGHT"))
					{
					TESTL(!IsDaylightPresent(bufOutput));	
					}
				else
					{
			TESTL(!IsEndPresent(bufOutput));
			}
		} while (err!=KErrEof);

	CleanupStack::PopAndDestroy(2, &fileOutput);
	}

/** Checks is start time is present in the line of data
@param	aLine The line of data to check
@test
*/
TBool CTestCalInterimApiExport::IsStartPresent(const TDesC8& aLine)
	{
	_LIT8(KDTSTART,"DTSTART:");

	TBufC8<8> first8char(aLine.Left(8));

	if (!first8char.Compare(KDTSTART()))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Checks is end time is present in the line of data
@param	aLine The line of data to check
@test
*/
TBool CTestCalInterimApiExport::IsEndPresent(const TDesC8& aLine)
	{
	_LIT8(KDTEND,"DTEND:");

	TBufC8<6> first6char(aLine.Left(6));

	if (!first6char.Compare(KDTEND()))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/** Checks if summary is present in the line of data
@param	aLine The line of data to check
@return True if summary is present in aLine, otherwise False
*/
TBool CTestCalInterimApiExport::IsSummaryPresent(const TDesC8& aLine)
	{
	_LIT8(KSUMMARY,"SUMMARY:");

	TBufC8<8> first8char(aLine.Left(8));

	if (!first8char.Compare(KSUMMARY()))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
	
/** Checks if daylight is present in the line of data
@param	aLine The line of data to check
@return True if daylight present otherwise false
*/
TBool CTestCalInterimApiExport::IsDaylightPresent(const TDesC8& aLine)
	{
	_LIT8(KDAYLIGHT,"DAYLIGHT:");

	TBufC8<KLengthDaylight> firstchar(aLine.Left(KLengthDaylight));

	if (!firstchar.Compare(KDAYLIGHT()))
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportvCalSameUid - Imports vCal's with same uid, and checks that only 1 entry will be stored
CTestCalInterimApiImportvCalSameUid::CTestCalInterimApiImportvCalSameUid()
	{
	SetTestStepName(KTestCalInterimApiImportvCalSameUid);
	}

CTestCalInterimApiImportvCalSameUid::~CTestCalInterimApiImportvCalSameUid()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportvCalSameUid::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalSameUid::doTestStepL() start"));

	iField = KTestSameUidEntriesImp;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntEntriesImported)); // gets expected number of entries imported

	iField = KTestSameUidEntriesStored;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntEntriesStored)); // gets expected number of entries stored in calendar database

	iField = KTimeMode;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iTimeModeEntry)); // gets expected time mode of entry

	GetvcsFileNumberFromIniFileL(); // gets vcs Number, so we can import correct vcs file

	RPointerArray<CCalEntry> entriesImported;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImported));

	OpenVCSAndImportL(iPathVCS, iFsSession, iIntvCalNumber,entriesImported); // Imports vcf

	TESTL(entriesImported.Count() == iIntEntriesImported);

	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	StoreEntryInDatabaseL(entriesImported);

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));
	FetchAllEntryFromDatabaseL(fetchEntryArray);

	TESTL(fetchEntryArray.Count() == iIntEntriesStored);

	// this is to test defect, remove this line once defect fixed
	TDateTime startTime;
	TDateTime endTime;

	if (!iTimeModeEntry.Compare(KTimeModeFloating))
		{
		TESTL(fetchEntryArray[0]->EntryTypeL() == CCalEntry::EAppt);
		startTime = fetchEntryArray[0]->StartTimeL().TimeLocalL().DateTime();
		TESTL(fetchEntryArray[0]->StartTimeL().TimeMode() == TCalTime::EFloating);
		endTime = fetchEntryArray[0]->EndTimeL().TimeLocalL().DateTime();
		TESTL(fetchEntryArray[0]->EndTimeL().TimeMode() == TCalTime::EFloating);
		}
	else
		{
		TESTL(fetchEntryArray[0]->EntryTypeL() == CCalEntry::EAppt);
		startTime = fetchEntryArray[0]->StartTimeL().TimeUtcL().DateTime();
		TESTL(fetchEntryArray[0]->StartTimeL().TimeMode() == TCalTime::EFixedUtc);
		endTime = fetchEntryArray[0]->EndTimeL().TimeUtcL().DateTime();
		TESTL(fetchEntryArray[0]->EndTimeL().TimeMode() == TCalTime::EFixedUtc);
		}

	INFO_PRINTF7(_L("Start Date Year Of Stored Entry: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), startTime.Year(), startTime.Month() + 1, startTime.Day() + 1, startTime.Hour(), startTime.Minute(), startTime.Second());
	INFO_PRINTF7(_L("End Date Year Of Stored Entry: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), endTime.Year(), endTime.Month() + 1, endTime.Day() + 1, endTime.Hour(), endTime.Minute(), endTime.Second());

	CleanupStack::PopAndDestroy(&fetchEntryArray); // entryArray.reset & entryArray.destroy;
	CleanupStack::PopAndDestroy(&entriesImported); // entriesImported.reset & entriesImported.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalSameUid::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportvCalAlarm - Imports vCals with AALARM beyond DTEND and prior to DTEND
// and beyond DTSTART.  Alarm should not be set.
CTestCalInterimApiImportvCalAlarm::CTestCalInterimApiImportvCalAlarm()
	{
	SetTestStepName(KTestCalInterimApiImportvCalAlarm);
	}

CTestCalInterimApiImportvCalAlarm::~CTestCalInterimApiImportvCalAlarm()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiImportvCalAlarm::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalAlarm::doTestStepL() start"));

	GetvcsFileNumberFromIniFileL(); // gets vcs Number, so we can import correct vcs file

	RPointerArray<CCalEntry> entriesImported;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImported));

	OpenVCSAndImportL(iPathVCS, iFsSession, iIntvCalNumber,entriesImported); // Imports vcf

	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	StoreEntryInDatabaseL(entriesImported);

	RPointerArray<CCalEntry> entryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entryArray));

	for (TInt index = 0; index < entriesImported.Count(); ++index)
		{
		INFO_PRINTF2(_L("Entry: %d"), index + 1);
		TRAPD(err, iEntryView->FetchL(entriesImported[index]->UidL(), entryArray));
		if (err == KErrNone)
			{
			TESTL(entryArray[0]->AlarmL() != NULL);
			INFO_PRINTF1(_L("Alarm Present in Entry"));
			}
		else
			{
			ERR_PRINTF1(_L("Entry is not fetched successfully"));
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		}
	
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.reset & entryArray.destroy;
	CleanupStack::PopAndDestroy(&entriesImported); // entriesImported.reset & entriesImported.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalAlarm::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiVerifyChildParent  - This class verifies that there is a parent and child entries present in database
CTestCalInterimApiVerifyChildParent::CTestCalInterimApiVerifyChildParent()
:	iTimeMode(TCalTime::EFixedUtc)
	{
	SetTestStepName(KTestCalInterimApiVerifyChildParent);
	}

CTestCalInterimApiVerifyChildParent::~CTestCalInterimApiVerifyChildParent()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiVerifyChildParent::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiVerifyChildParent::doTestStepL() start"));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	if (fetchEntryArray.Count() != 0)
		{
		const TDesC8& entryUid = fetchEntryArray[0]->UidL();

		for ( TInt index = 0; index < fetchEntryArray.Count(); ++index )
			{
			const TDesC8& entryUid1 = fetchEntryArray[index]->UidL();
			TESTL(!entryUid.Compare(entryUid1));
			}

		for (TInt index = 0; index < fetchEntryArray.Count(); ++index)
			{
			if (fetchEntryArray[index]->RecurrenceIdL().TimeLocalL() != Time::NullTTime())
				{
				iField.Zero();
				iField.Append(KExpectedReccurenceTime);
				iField.AppendNum(index);
				TESTL(GetStringFromConfig(ConfigSection(), iField, iPtrExpectedReccurence)); // gets expected reccurence time for child range
				TDateTime recurrenceTime;
				if (fetchEntryArray[index]->RecurrenceIdL().TimeMode() == TCalTime::EFloating)
					{
					recurrenceTime = fetchEntryArray[index]->RecurrenceIdL().TimeLocalL().DateTime();
					INFO_PRINTF7(_L("Recurrence Date Year Of Stored Child Entry: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), recurrenceTime.Year(), recurrenceTime.Month() + 1, recurrenceTime.Day() + 1, recurrenceTime.Hour(), recurrenceTime.Minute(), recurrenceTime.Second());
					TESTL(FormatDateTime(iPtrExpectedReccurence) == fetchEntryArray[index]->RecurrenceIdL().TimeLocalL());
					TESTL(fetchEntryArray[index]->RecurrenceIdL().TimeMode() == TCalTime::EFloating);
					}
				else
					{
					recurrenceTime = fetchEntryArray[index]->RecurrenceIdL().TimeUtcL().DateTime();
					INFO_PRINTF7(_L("Recurrence Year Of Stored Child Entry: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), recurrenceTime.Year(), recurrenceTime.Month() + 1, recurrenceTime.Day() + 1, recurrenceTime.Hour(), recurrenceTime.Minute(), recurrenceTime.Second());
					TESTL(FormatDateTime(iPtrExpectedReccurence) == fetchEntryArray[index]->RecurrenceIdL().TimeUtcL());
					TESTL(fetchEntryArray[index]->RecurrenceIdL().TimeMode() == TCalTime::EFixedUtc);
					}

				CheckChildTimesL(fetchEntryArray, index);
				INFO_PRINTF1(_L("Importing of child and parent entry verification done "));
				}
			}
		}

	CleanupStack::PopAndDestroy(&fetchEntryArray); // fetchEntryArray.reset & fetchEntryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiVerifyChildParent::doTestStepL() finish"));
	return TestStepResult();
	}

/** Checks for the child's start time, end time, alarm offset and time mode and completed time
@param	aEntryArray Contains the child entries
@param	aIndex The index to be used to fecth the correct child entry in aEntryArray
@test
*/
void CTestCalInterimApiVerifyChildParent::CheckChildTimesL(RPointerArray<CCalEntry>& aEntryArray, TInt aIndex)
	{
	GetChildDataFromIniFileL(aIndex);

	if (iTimeModeEntry == KTimeModeFloating)
		{
		iTimeMode = TCalTime::EFloating;
		}

	TDateTime importedTime;
	TDateTime importedEndTime;

	if (iTimeModeEntry == KTimeModeFloating)
		{
		importedTime = aEntryArray[aIndex]->StartTimeL().TimeLocalL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntryArray[aIndex]->StartTimeL().TimeLocalL());
		importedEndTime = aEntryArray[aIndex]->EndTimeL().TimeLocalL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntryArray[aIndex]->EndTimeL().TimeLocalL());
		}
	else
		{
		importedTime = aEntryArray[aIndex]->StartTimeL().TimeUtcL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedStartTime) == aEntryArray[aIndex]->StartTimeL().TimeUtcL());
		importedEndTime = aEntryArray[aIndex]->EndTimeL().TimeUtcL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedEndTime) == aEntryArray[aIndex]->EndTimeL().TimeUtcL());
		}

	TDateTime expectedTime = FormatDateTime(iPtrExpectedStartTime).DateTime();

	INFO_PRINTF7(_L("Imported Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedTime.Year(), importedTime.Month() + 1, importedTime.Day() + 1, importedTime.Hour(), importedTime.Minute(), importedTime.Second());
	INFO_PRINTF7(_L("Expected Start Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), expectedTime.Year(), expectedTime.Month() + 1, expectedTime.Day() + 1, expectedTime.Hour(), expectedTime.Minute(), expectedTime.Second());

	TESTL(aEntryArray[aIndex]->StartTimeL().TimeMode() == iTimeMode);
	expectedTime = FormatDateTime(iPtrExpectedEndTime).DateTime();

	INFO_PRINTF7(_L("Imported End Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedEndTime.Year(), importedEndTime.Month() + 1, importedEndTime.Day() + 1, importedEndTime.Hour(), importedEndTime.Minute(), importedEndTime.Second());
	INFO_PRINTF7(_L("Expected End Date Year: %d, Month: %d, Day: %d, Expected Time Hr: %d, Min: %d, Sec: %d "), expectedTime.Year(), expectedTime.Month() + 1, expectedTime.Day() + 1, expectedTime.Hour(), expectedTime.Minute(), expectedTime.Second());

	TESTL(aEntryArray[aIndex]->EndTimeL().TimeMode() == iTimeMode);

	if (aEntryArray[aIndex]->CompletedTimeL().TimeLocalL() == Time::NullTTime())
		{
		// If completed time equals NULL, then alarm should be settable
		CCalAlarm* alarm1 = aEntryArray[aIndex]->AlarmL();
		if(alarm1 != NULL)
			{
			TESTL(alarm1->TimeOffset().Int() == iIntAlarmOffset);
			INFO_PRINTF1(KImportedAlarmOffset);			
			}
		}

	TDateTime importedDTStamp;
	//dont need to check floating dtstamp, as always imported as FixedUtc
	importedDTStamp = aEntryArray[aIndex]->DTStampL().TimeUtcL().DateTime();
	INFO_PRINTF7(_L("Imported dtstamp Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedDTStamp.Year(), importedDTStamp.Month() + 1, importedDTStamp.Day() + 1, importedDTStamp.Hour(), importedDTStamp.Minute(), importedDTStamp.Second());
	TESTL(FormatDateTime(iPtrExpectedDTStamp) == aEntryArray[aIndex]->DTStampL().TimeUtcL());

	TESTL(aEntryArray[aIndex]->DTStampL().TimeMode() == TCalTime::EFixedUtc);
	INFO_PRINTF1(_L("DTStamp time of Entry has been imported correctly"));

	if(aEntryArray[aIndex]->EntryTypeL() == CCalEntry::ETodo)
		{
		TDateTime importedCompletedTime;
		// Completed time always imported as utc
		importedCompletedTime = aEntryArray[aIndex]->CompletedTimeL().TimeUtcL().DateTime();
		TESTL(FormatDateTime(iPtrExpectedCompletedTime) == aEntryArray[aIndex]->CompletedTimeL().TimeUtcL());

		TESTL(aEntryArray[aIndex]->CompletedTimeL().TimeMode() == TCalTime::EFixedUtc);
		INFO_PRINTF1(_L("Completed time of Todo Entry has been imported correctly"));
		}
	else
		{
		TESTL(aEntryArray[aIndex]->CompletedTimeL().TimeLocalL() == Time::NullTTime());
		}

	INFO_PRINTF1(KImportedTimesCorrect);
	}

/** Gets data from ini file TestCalImportExport.ini
@param	avCalNumber This is to ensure it fecthes the correct expected times
@test
*/
void CTestCalInterimApiVerifyChildParent::GetChildDataFromIniFileL(TInt avCalNumber)
	{
	iField.Zero();
	iField.Append(KExpectedStartTime);
	iField.AppendNum(avCalNumber);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedStartTime)); // gets expected start time

	iField.Zero();
	iField.Append(KExpectedEndTime);
	iField.AppendNum(avCalNumber);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedEndTime));	 // gets expected end time

	iField.Zero();
	iField.Append(KExpectedDTStamp);
	iField.AppendNum(avCalNumber);
	GetStringFromConfig(ConfigSection(),iField,iPtrExpectedDTStamp);	 // gets expected DTStamp

	iField.Zero();
	iField.Append(KExpectedAlarmOffset);
	iField.AppendNum(avCalNumber);
	GetIntFromConfig(ConfigSection(),iField,iIntAlarmOffset); // gets expected alarm time

	iField = KTimeMode;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iTimeModeEntry)); // gets expected time mode of entry

	iField.Zero();
	iField.Append(KExpectedCompletedTime);
	iField.AppendNum(avCalNumber);
	GetStringFromConfig(ConfigSection(),iField,iPtrExpectedCompletedTime); // gets expected completed time
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiRDateExDate  - This class verifies that the exception dates and rdates have been imported correctly
CTestCalInterimApiRDateExDate::CTestCalInterimApiRDateExDate()
:	iTimeMode(TCalTime::EFixedUtc)
	{
	SetTestStepName(KTestCalInterimApiRDateExDate);
	}

CTestCalInterimApiRDateExDate::~CTestCalInterimApiRDateExDate()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiRDateExDate::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiRDateExDate::doTestStepL() start"));

	iField = KTimeMode;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iTimeModeEntry)); // gets expected time mode of entry

	if (iTimeModeEntry == KTimeModeFloating)
		{
		iTimeMode = TCalTime::EFloating;
		}

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	RArray<TCalTime> rDateList;
	CleanupClosePushL(rDateList);

	RArray<TCalTime> exDateList;
	CleanupClosePushL(exDateList);

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	TCalTime rdate;
	TCalTime exdate;

	for (TInt index = 0; index < fetchEntryArray.Count(); ++index)
		{
		INFO_PRINTF2(_L("Entry: %d"), index);

		fetchEntryArray[index]->GetRDatesL(rDateList);

		iField.Zero();
		iField.Append(KCountRDate);
		iField.AppendNum(index);
		GetIntFromConfig(ConfigSection(),iField,iIntRDate); // gets expected number of RDate

		if (iIntRDate !=0 )
			{
			TESTL(iIntRDate == rDateList.Count());
			}

		for (TInt index1 = 0; index1 < rDateList.Count(); ++index1)
			{
			iField.Zero();
			iField.Append(KExpectedRDate);
			iField.AppendNum(index);
			iField.AppendNum(index1);
			TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedRDate));	 // gets expected end time

			rdate = rDateList[index1];

			if (iTimeModeEntry == KTimeModeFloating)
				{
				TESTL(FormatDateTime(iPtrExpectedRDate) == rdate.TimeLocalL());
				}
			else
				{
				TESTL(FormatDateTime(iPtrExpectedRDate) == rdate.TimeUtcL());
				}

			TESTL(rdate.TimeMode() == iTimeMode);
			}

		rDateList.Reset();

		INFO_PRINTF1(_L("RDates have been imported correctly, as well as the time modes"));

		fetchEntryArray[index]->GetExceptionDatesL(exDateList);

		iField.Zero();
		iField.Append(KCountExDate);
		iField.AppendNum(index);
		GetIntFromConfig(ConfigSection(),iField,iIntExDate); // gets expected number of Exception Date

		if (iIntExDate !=0 )
			{
			TESTL(iIntExDate == exDateList.Count());
			}

		for (TInt index2 = 0; index2 < exDateList.Count(); ++index2)
			{
			iField.Zero();
			iField.Append(KExpectedExDate);
			iField.AppendNum(index);
			iField.AppendNum(index2);
			TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedExDate));	 // gets expected end time

			exdate = exDateList[index2];

			if (iTimeModeEntry == KTimeModeFloating)
				{
				TESTL(FormatDateTime(iPtrExpectedExDate) == exdate.TimeLocalL());
				}
			else
				{
				TESTL(FormatDateTime(iPtrExpectedExDate) == exdate.TimeUtcL());
				}

			TESTL(exdate.TimeMode() == iTimeMode);
			}

		exDateList.Reset();

		INFO_PRINTF1(_L("Exception Dates have been imported correctly, as well as the time modes"));
		}

	CleanupStack::PopAndDestroy(&exDateList); // exDateList.reset & exDateList.destroy;
	CleanupStack::PopAndDestroy(&rDateList); // rDateList.reset & rDateList.destroy;
	CleanupStack::PopAndDestroy(&fetchEntryArray); // fetchEntryArray.reset & fetchEntryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiRDateExDate::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class TestCalInterimApiBadRDateEx - This class verifies that the invalid exception dates and rdates have not been imported
CTestCalInterimApiBadRDateEx::CTestCalInterimApiBadRDateEx()
	{
	SetTestStepName(KTestCalInterimApiBadRDateEx);
	}

CTestCalInterimApiBadRDateEx::~CTestCalInterimApiBadRDateEx()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiBadRDateEx::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiBadRDateEx::doTestStepL() start"));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	RArray<TCalTime> rDateList;
	CleanupClosePushL(rDateList);

	RArray<TCalTime> exDateList;
	CleanupClosePushL(exDateList);

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	TCalTime rdate;
	TCalTime exdate;

	for (TInt index = 0; index < fetchEntryArray.Count(); ++index)
		{
		INFO_PRINTF2(_L("Entry: %d"), index);

		fetchEntryArray[index]->GetRDatesL(rDateList);

		TESTL(rDateList.Count() == 0);

		INFO_PRINTF1(_L("Bad RDates have not been imported"));

		fetchEntryArray[index]->GetExceptionDatesL(exDateList);

		TESTL(exDateList.Count() == 0);

		INFO_PRINTF1(_L("Bad Exception Dates have not been imported"));
		}

	CleanupStack::PopAndDestroy(&exDateList); // exDateList.reset & exDateList.destroy;
	CleanupStack::PopAndDestroy(&rDateList); // rDateList.reset & rDateList.destroy;
	CleanupStack::PopAndDestroy(&fetchEntryArray); // fetchEntryArray.reset & fetchEntryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiBadRDateEx::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class TestCalInterimApiBadRptUntil  - This class verifies that the invalid repeat until date/time is equal to NULL
CTestCalInterimApiBadRptUntil::CTestCalInterimApiBadRptUntil()
:	iInvalidUntilTest(EFalse)
,	iTimeMode(TCalTime::EFixedUtc)
	{
	SetTestStepName(KTestCalInterimApiBadRptUntil);
	}

CTestCalInterimApiBadRptUntil::~CTestCalInterimApiBadRptUntil()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiBadRptUntil::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiBadRptUntil::doTestStepL() start"));

	_LIT(KNULL,"NULL");

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	TDateTime importedUntilTime;
	TCalRRule rptDef;

	iField = KInvalidUntilTest;
	GetBoolFromConfig(ConfigSection(), iField, iInvalidUntilTest);

	for (TInt index = 0; index < fetchEntryArray.Count(); ++index)
		{
		TCalRRule rptDef;
		TBool fecthRule = fetchEntryArray[index]->GetRRuleL(rptDef);

		importedUntilTime = rptDef.Until().TimeLocalL().DateTime();

		if (!iInvalidUntilTest)
			{
			iField = KTimeMode;
			TESTL(GetStringFromConfig(ConfigSection(),iField,iTimeModeEntry)); // gets expected time mode of entry

			if (iTimeModeEntry == KTimeModeFloating)
				{
				iTimeMode = TCalTime::EFloating;
				}

			iField.Zero();
			iField.Append(KExpectedUntilTime);
			iField.AppendNum(index);
			TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedUntilTime));	 // gets expected repeat until time

			if (!iPtrExpectedUntilTime.Compare(KNULL()))
				{
				TESTL(fecthRule == EFalse);
				}
			else
				{
				if (iTimeModeEntry == KTimeModeFloating)
					{
					importedUntilTime = rptDef.Until().TimeLocalL().DateTime();
					TESTL(FormatDateTime(iPtrExpectedUntilTime) == rptDef.Until().TimeLocalL().DateTime());
					}
				else
					{
					importedUntilTime = rptDef.Until().TimeUtcL().DateTime();
					TESTL(FormatDateTime(iPtrExpectedUntilTime) == rptDef.Until().TimeUtcL().DateTime());
					}

				INFO_PRINTF7(_L("Imported Repeat Until Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), importedUntilTime.Year(), importedUntilTime.Month() + 1, importedUntilTime.Day() + 1, importedUntilTime.Hour(), importedUntilTime.Minute(), importedUntilTime.Second());

				TESTL(rptDef.Until().TimeMode() == iTimeMode);
				}
			}
		else
			{
			TESTL(fecthRule == EFalse);
			}
		}

	CleanupStack::PopAndDestroy(&fetchEntryArray); // fetchEntryArray.reset & fetchEntryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiBadRptUntil::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class TestCalInterimApiBadReccurence  - Verifies that the recurrence id is NULL
CTestCalInterimApiBadReccurence::CTestCalInterimApiBadReccurence()
	{
	SetTestStepName(KTestCalInterimApiBadReccurence);
	}

CTestCalInterimApiBadReccurence::~CTestCalInterimApiBadReccurence()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiBadReccurence::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiBadReccurence::doTestStepL() start"));

	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	FetchAllEntryFromDatabaseL(fetchEntryArray);

	for (TInt index = 0; index < fetchEntryArray.Count(); ++index)
		{
		TESTL(fetchEntryArray[index]->RecurrenceIdL().TimeLocalL() == Time::NullTTime());
		}

	CleanupStack::PopAndDestroy(&fetchEntryArray); // fetchEntryArray.reset & fetchEntryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiBadReccurence::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportvCalAttachment - Imports vCals with attachments
CTestCalInterimApiImportvCalAttachment::CTestCalInterimApiImportvCalAttachment()
:	iExpectedAttachmentCount(0)
	{
	SetTestStepName(KTestCalInterimApiImportvCalAttachment);
	}

CTestCalInterimApiImportvCalAttachment::~CTestCalInterimApiImportvCalAttachment()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiImportvCalAttachment::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalAttachment Start:"));

	GetvcsFileNumberFromIniFileL(); // gets vcs Number, so we can import correct vcs file

	RPointerArray<CCalEntry> entriesImported;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesImported));

	OpenVCSAndImportL(iPathVCS, iFsSession, iIntvCalNumber,entriesImported); // Imports vcs

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	iField = KImportedEntriesCount;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iImportedEntriesCount)); 
	TESTL( entriesImported.Count() == iImportedEntriesCount );
	
	for ( TInt i = 0; i < entriesImported.Count(); i++ )
		{
		CCalEntry*	entry = entriesImported[i];
		TBool	checkAttachmentCount;
		iField.Zero();
		iField.Append(KAttachmentCount);
		iField.AppendNum(i);
		checkAttachmentCount = GetIntFromConfig(ConfigSection(), iField, iExpectedAttachmentCount);
		if ( checkAttachmentCount )
			{
			TESTL( entry->AttachmentCountL() == iExpectedAttachmentCount );	
			}
		
		for ( TInt j = 0; j < entry->AttachmentCountL(); j++ )
			{
			CCalAttachment*	attachment = entry->AttachmentL(j);	
			iField.Zero();
			iField.Append(KCIDAttachFile);
			iField.AppendNum(i);
			iField.AppendNum(j);
			GetStringFromConfig(ConfigSection(), iField, iCIDAttachFile);
			if ( iCIDAttachFile != KNullDesC() && attachment->FileAttachment()->ContentId().Length() > 0 )
				{ 
				RFile file;
				User::LeaveIfError(iFsSession.ShareProtected());
				User::LeaveIfError(file.Open(iFsSession, iCIDAttachFile, EFileRead | EFileWrite | EFileShareExclusive));
				attachment->FileAttachment()->SetResourceL(file);
				iCIDAttachFile.Set(KNullDesC()); // reset the variable
				}
			// Set the drive for storing the attachment if required
			iField.Zero();
			iField.Append(KDriveForStoringAttachment);
			iField.AppendNum(i);
			iField.AppendNum(j);
			GetStringFromConfig(ConfigSection(), iField, iDrive);
			if ( iDrive != KNullDesC() && attachment->FileAttachment() )
				{
				TDriveName driveName;
				driveName.Copy(iDrive);
				driveName.UpperCase();
				// Set the drive for storing the attachment
				attachment->FileAttachment()->SetDriveL(driveName);
				iDrive.Set( KNullDesC());
				}
			// Set the attributes for the attachment if required
			iField.Zero();
			iField.Append(KAttributes);
			iField.AppendNum(i);
			iField.AppendNum(j);
			iSetAttribute = GetIntFromConfig(ConfigSection(), iField, iAttributes);
			if ( iSetAttribute )
				{
				attachment->SetAttribute((CCalAttachment::TAttributes)iAttributes);
				iSetAttribute = EFalse;
				}
			// Set the content id of the attachment
			iField.Zero();
			iField.Append(KLabel);
			iField.AppendNum(i);
			iField.AppendNum(j);
			GetStringFromConfig(ConfigSection(), iField, iLabel);
			if ( iLabel != KNullDesC() )
				{
				HBufC*	label = iLabel.AllocLC();
				attachment->SetLabelL(*label);
				CleanupStack::PopAndDestroy(label);
				}
			}
		}
	
	StoreEntryInDatabaseL(entriesImported);

	CleanupStack::PopAndDestroy(&entriesImported); // entriesImported.reset & entriesImported.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiImportvCalAttachment::doTestStepL() finish"));
	return TestStepResult();
	}

