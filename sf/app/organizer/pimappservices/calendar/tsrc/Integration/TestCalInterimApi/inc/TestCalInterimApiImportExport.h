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

#ifndef __TEST_CALINTERIMAPI_IMPORT_EXPORT_H__
#define __TEST_CALINTERIMAPI_IMPORT_EXPORT_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calentryview.h>
#include <calattachment.h>

/** CTestCalInterimApiImportExport */
_LIT(KvCalNumber,					"vCalNumber");
_LIT(KPathImportvCals, 				"\\CITCalInterimApi\\testdata\\vCals\\ImportvCals\\vCal");
_LIT(KOpenVCSFile, 					"Opened vCal VCS file %S");
_LIT(KInfoEntryAdded,				"Entry Added");
_LIT(KInfoAddingEntryToDatabase,	"Adding Entry to Database");
_LIT(KInfoFetchedEntryFromDatabase,	"Fetched Entry from Database");
_LIT(KVerification,					"Verification");
_LIT(KVerifyImStore,				"VerifyImportAgainstStore");

const TInt KLengthDaylight = 9;
/** CTestCalInterimApiImportvCal */
_LIT(KInfoGUID,						"GUID: %S");
_LIT(KTestCalInterimApiImportvCal,	"TestCalInterimApiImportvCal");
_LIT(KExpectedStartTime,			"expectedstarttime");
_LIT(KExpectedEndTime,				"expectedendtime");
_LIT(KExpectedDTStamp,				"expecteddtstamp");
_LIT(KExpectedAlarmOffset,			"expectedAlarmOffset");
_LIT(KExpectedCompletedTime,		"expectedcompletedtime");
_LIT(KImportedTimesCorrect,			"Times of entry has been imported correctly");
_LIT(KImportedAlarmOffset,			"Alarm offset has been imported correctly");
_LIT(KTimeMode,						"timemode");
_LIT(KTimeModeFloating,				"EFloating");
_LIT(KNoStartEndTest,				"NoStartEndTest");
_LIT(KNoStart,						"NoStart");
_LIT(KNoEnd,						"NoEnd");
_LIT(KNoStartEnd,					"NoStartEnd");

/** CTestCalInterimApiImportvCalNoStart */
_LIT(KTestCalInterimApiImportvCalNoStart,	"TestCalInterimApiImportvCalNoStart");

/** CTestCalInterimApiImportvCalNoEnd */
_LIT(KTestCalInterimApiImportvCalNoEnd,		"TestCalInterimApiImportvCalNoEnd");

/** CTestCalInterimApiImportvCalNoStartEnd */
_LIT(KTestCalInterimApiImportvCalNoStartEnd,"TestCalInterimApiImportvCalNoStartEnd");
_LIT(KExpectedEntryImport,					"ExpectedEntryImport");

/** CTestCalInterimApiExport */
_LIT(KTestCalInterimApiExport,				"TestCalInterimApiExport");
_LIT(KExportVcsFile,						"ExportFile");
_LIT(KExportToVCSFile,						"Exporting to VCS file............. %S");
_LIT(KExpectedExportVcsFile,				"ExpectedExportFile");
_LIT(KNoField,								"NoField");
_LIT(KExpectedRRule,						"ExpectedRRule");
_LIT8(KRRULE,								"RRULE");
_LIT8(KSUMMARY,								"SUMMARY");
_LIT8(KDESCRIPTION,							"DESCRIPTION");
_LIT8(KCATEGORIES,							"CATEGORIES");
_LIT8(KPRIORITY,							"PRIORITY");
_LIT8(KLOCATION,							"LOCATION");
_LIT8(KMETHOD,								"X-METHOD");
_LIT8(KSEQUENCE,							"SEQUENCE");
_LIT8(KSTATUS,								"STATUS");
_LIT8(KREPLICATION,							"CLASS");
_LIT8(KTRANSP,								"TRANSP");
_LIT8(KGEO,									"GEO");
_LIT8(KPLUSSIGN,							"+");
_LIT8(KCATXPREFIX,							"X-");
_LIT8(KCOLON,								":");

/** CTestCalInterimApiImportvCalSameUid */
_LIT(KTestCalInterimApiImportvCalSameUid,	"TestCalInterimApiImportvCalSameUid");
_LIT(KTestSameUidEntriesImp,				"entriesimported");
_LIT(KTestSameUidEntriesStored,				"entriesStored");

/** CTestCalInterimApiImportvCalAlarm */
_LIT(KTestCalInterimApiImportvCalAlarm,		"TestCalInterimApiImportvCalAlarm");
_LIT(KTestInvalidGEOTest,					"invalidGEOVerification");
/** constants */
const TInt KIndexArrayForParentEntry = 0;

/** CTestCalInterimApiVerifyChildParent */
_LIT(KTestCalInterimApiVerifyChildParent,	"TestCalInterimApiVerifyChildParent");
_LIT(KExpectedUid,							"expecteduid");
_LIT(KExpectedReccurenceTime,				"expectedchild");

/** CTestCalInterimApiRDateExDate */
_LIT(KTestCalInterimApiRDateExDate,			"TestCalInterimApiRDateExDate");
_LIT(KExpectedRDate,						"expectedrdate");
_LIT(KExpectedExDate,						"expectedexdate");
_LIT(KCountRDate,							"verifycountrdate");
_LIT(KCountExDate,							"verifycountexdate");

/** CTestCalInterimApiBadRDateEx */
_LIT(KTestCalInterimApiBadRDateEx,			"TestCalInterimApiBadRDateEx");

/** CTestCalInterimApiBadRptUntil */
_LIT(KTestCalInterimApiBadRptUntil,			"TestCalInterimApiBadRptUntil");
_LIT(KInvalidUntilTest,						"invalidUntilTest");
_LIT(KExpectedUntilTime,					"expectedUntilTime");

/** TestCalInterimApiBadReccurence */
_LIT(KTestCalInterimApiBadReccurence,		"TestCalInterimApiBadReccurence");

/** CTestCalInterimApiImportvCalAttachment */
_LIT(KTestCalInterimApiImportvCalAttachment,"TestCalInterimApiImportvCalAttachment");
_LIT(KCIDAttachFile,						"CIDattachment");
_LIT(KDriveForStoringAttachment,			"attachdrive");
_LIT(KAttributes,							"attribute");
_LIT(KLabel,								"label");
_LIT(KImportedEntriesCount,					"importedentriescount");
_LIT(KAttachmentCount,						"attachmentcount");

// Class CTestCalInterimApiImportExport - Common functions that are used between the import/export steps
class CTestCalInterimApiImportExport : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiImportExport();
	~CTestCalInterimApiImportExport();

public:
	void OpenVCSAndImportL(TDes& aPath, RFs aFsSession, TInt avCalNumber, RPointerArray<CCalEntry>& aCalEntryArray);
	void ImportvCalL(RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray);
	void GetvcsFileNumberFromIniFileL();
	void StoreEntryInDatabaseL(RPointerArray<CCalEntry>& aEntriesImported);
	void FetchAllEntryFromDatabaseL(RPointerArray<CCalEntry>& aFetchEntryArray);
	void ExportEntriesL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath);

public:
	TInt iIntvCalNumber;
	CCalEntryView *iEntryView;
	TBuf<KMaxLengthString> iPathVCS;
	TBool	iVerification;
	TBuf<KMaxLengthString> iField;
	CCalDataExchange* iDataExchange;
	TBool iVerifyImportAgainstStore;

private:
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiImportvCal - Imports vcs file, stores entries, fetch entries and compares times
class CTestCalInterimApiImportvCal : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportvCal();
	~CTestCalInterimApiImportvCal();
	TVerdict doTestStepL();

private:
	void GetDataFromIniFileL(TInt avCalNumber);
	void CheckEntriesTimesL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckInvalidGeoValueL(RPointerArray<CCalEntry>& aEntriesImported);
	void CheckNoStartEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckNoEndEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckNoEndStartEntryL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckApptEventAnnivTimesL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckToDoCompletedTimeL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);
	void CheckDTStampL(RPointerArray<CCalEntry>& aEntriesImported, TInt aIndex);

private:
	TPtrC iPtrExpectedStartTime;
	TPtrC iPtrExpectedEndTime;
	TPtrC iPtrExpectedDTStamp;
	TPtrC iPtrExpectedCompletedTime;
	TPtrC iTimeModeEntry;
	TInt iIntAlarmOffset;
	TCalTime::TTimeMode	iTimeMode;
	TPtrC iPtrTest;
	TInt iExpNumImport;
	TBool iGeoInvalidTest;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiExport - Opens database (specified in ini file), then exports into vcs file (path specified in ini file).
class CTestCalInterimApiExport : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiExport();
	~CTestCalInterimApiExport();
	TVerdict doTestStepL();

private:
	void CompareFileL(const TDesC& aInputFile, const TDesC& aOutputFile, const TDesC8& aProperty);
	TBool IsProperty(const TDesC8& aLine, const TDesC8& aProperty);
	void CompareLinesL(const TDesC8& aBufExpected, const TDesC8& aBufOutput);
	void CheckRRule(const TDesC& aExpectedRRule, const TDesC8& aBufOutput);
	void CheckForNoFieldL(const TDesC& aFile);
	TBool IsStartPresent(const TDesC8& aLine);
	TBool IsEndPresent(const TDesC8& aLine);
	TBool IsSummaryPresent(const TDesC8& aLine);
	TBool IsDaylightPresent(const TDesC8& aLine);

private:
	TPtrC iExportFilePath;
	TPtrC iExpectedExportFilePath;
	TPtrC iNoField;
	TPtrC iExpectedRRule;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiImportvCalSameUid - Imports vCal's with same uid, and checks that only 1 entry will be stored
class CTestCalInterimApiImportvCalSameUid : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportvCalSameUid();
	~CTestCalInterimApiImportvCalSameUid();
	TVerdict doTestStepL();

private:
	TInt iIntEntriesImported;
	TInt iIntEntriesStored;
	TPtrC iTimeModeEntry;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiImportvCalAlarm - Imports vCals with AALARM beyond DTEND and prior to DTEND
// and beyond DTSTART.  Alarm should not be set.
class CTestCalInterimApiImportvCalAlarm : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportvCalAlarm();
	~CTestCalInterimApiImportvCalAlarm();
	TVerdict doTestStepL();
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiVerifyChildParent  - This class verifies that there is a parent and child entries present in database
class CTestCalInterimApiVerifyChildParent : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiVerifyChildParent();
	~CTestCalInterimApiVerifyChildParent();
	TVerdict doTestStepL();

private:
	void CheckChildTimesL(RPointerArray<CCalEntry>& aEntryArray, TInt aIndex);
	void GetChildDataFromIniFileL(TInt avCalNumber);

private:
	TCalTime::TTimeMode	iTimeMode;
	TPtrC iPtrExpectedReccurence;
	TPtrC iPtrExpectedStartTime;
	TPtrC iPtrExpectedEndTime;
	TPtrC iPtrExpectedDTStamp;
	TPtrC iPtrExpectedCompletedTime;
	TPtrC iTimeModeEntry;
	TInt iIntAlarmOffset;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiRDateExDate  - This class verifies the Rdates and exception dates have been imported correctly
class CTestCalInterimApiRDateExDate : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiRDateExDate();
	~CTestCalInterimApiRDateExDate();
	TVerdict doTestStepL();

private:
	TPtrC iTimeModeEntry;
	TCalTime::TTimeMode	iTimeMode;
	TInt iIntRDate;
	TInt iIntExDate;
	TPtrC iPtrExpectedRDate;
	TPtrC iPtrExpectedExDate;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiBadRDateEx  - This class verifies the  bad Rdates and exception dates have not been imported
class CTestCalInterimApiBadRDateEx : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiBadRDateEx();
	~CTestCalInterimApiBadRDateEx();
	TVerdict doTestStepL();

private:
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiBadRptUntil  - This class verifies that the invalid repeat until time is equal to NULL
class CTestCalInterimApiBadRptUntil : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiBadRptUntil();
	~CTestCalInterimApiBadRptUntil();
	TVerdict doTestStepL();

private:
	TBool iInvalidUntilTest;
	TPtrC iTimeModeEntry;
	TCalTime::TTimeMode	iTimeMode;
	TPtrC iPtrExpectedUntilTime;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiBadReccurence  - Verifies that the recurrence id is NULL
class CTestCalInterimApiBadReccurence : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiBadReccurence();
	~CTestCalInterimApiBadReccurence();
	TVerdict doTestStepL();

private:
	};
	
// Class CTestCalInterimApiImportvCalAttachment - Imports vCals with attachment
class CTestCalInterimApiImportvCalAttachment : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportvCalAttachment();
	~CTestCalInterimApiImportvCalAttachment();
	TVerdict doTestStepL();

private:
	TPtrC iCIDAttachFile;
	TInt iAttributes;
	TPtrC iDrive;
	TBool iSetAttribute;
	TInt iImportedEntriesCount;
	TInt iExpectedAttachmentCount;
	TPtrC iLabel;
	};

#endif //__TEST_CAL_INTERIM_IMPORT_EXPORT_H__
