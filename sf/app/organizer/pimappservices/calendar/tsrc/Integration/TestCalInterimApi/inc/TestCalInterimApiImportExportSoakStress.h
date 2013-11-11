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

#ifndef __TEST_CALINTERIMAPI_IMPORT_EXPORT_SOAK_STRESS_H__
#define __TEST_CALINTERIMAPI_IMPORT_EXPORT_SOAK_STRESS_H__

#include "TestCalInterimApiImportExport.h"

/** CTestCalInterimApiImportExportCreateStressData */
_LIT(KTestCalInterimApiImportExportCreateStressData,"TestCalInterimApiImportExportCreateStressData");
_LIT(KNumberOfEntries,							"numberofentries");
_LIT(KEntryStartDate,							"starttime");
_LIT(KEntryEndDate,								"endtime");
_LIT(KTimesIsFloating,							"floating");

/** CTestCalInterimApiImportExportCountEntry */
_LIT(KTestCalInterimApiImportExportCountEntry,	"TestCalInterimApiImportExportCountEntry");

/** CTestCalInterimApiExportSoak */
_LIT(KTestCalInterimApiExportSoak,				"TestCalInterimApiExportSoak");
_LIT(KExportCount,								"RepeatExport");

// Class CTestCalInterimApiImportExportCreateStressData - This class prepares a calendar database, with large number of entries.
// Number of entries specified in ini file (TestCalImportExportSoakStress.ini).
class CTestCalInterimApiImportExportCreateStressData : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportExportCreateStressData();
	~CTestCalInterimApiImportExportCreateStressData();
	TVerdict doTestStepL();

private:
	TBuf<KMaxLengthString> iField;
	TInt iNumberOfEntries;
	TTime iStartdate;
	TTime iEnddate;
	TPtrC iPtrStartDate;
	TPtrC iPtrEndDate;
	TBool iBoolIsFloating;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiImportExportCountEntry - This class verifies the number of entries in database
// i.e. so we import 5000 entry and store (can be found in TestCalImportExportSoakStress.script), this class will verify that 5000
// entries are present.
class CTestCalInterimApiImportExportCountEntry : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiImportExportCountEntry();
	~CTestCalInterimApiImportExportCountEntry();
	TVerdict doTestStepL();

private:
	TBuf<KMaxLengthString> iField;
	TInt iNumberOfEntries;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiExportSoak - This class fetches entries from database, then exports multiple times into a vcs file.
// The number of calls is specified in ini file.
class CTestCalInterimApiExportSoak : public CTestCalInterimApiImportExport
	{
public:
	CTestCalInterimApiExportSoak();
	~CTestCalInterimApiExportSoak();
	TVerdict doTestStepL();

private:
	void ExportEntriesSoakL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath);

private:
	TBuf<KMaxLengthString> iField;
	TInt iExportCount;
	TPtrC iExportFilePath;
	};

#endif //__TEST_CALINTERIMAPI_IMPORT_EXPORT_SOAK_STRESS_H__
