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

#ifndef __TEST_CALINTERIMAPI_IMPORT_EXPORT_ASYNC_H__
#define __TEST_CALINTERIMAPI_IMPORT_EXPORT_ASYNC_H__

#include "TestCalInterimApiImportExport.h"
#include <caldataexchange.h>

_LIT(KInfoPercentageCompleted,				"Percentage Completed %d");
_LIT(KErrPercentageCompleted,				"Error progress not required");

class CTestCalInterimApiImportExportAsync;

/** constants */
const TInt KNumberOfEntriesToHandle = 24;

/** CTestCalInterimApiImportExportAsyncTest */
enum TTestNumber
		{
		ETest1 = 1,
		ETest2 = 2,
		ETest3 = 3,
		ETest4 = 4,
		ETest5 = 5,
		ETest6 = 6,
		};

/** CTestCalInterimApiImportExportAsyncTest */
_LIT(KTestCalInterimApiImportExportAsyncTest,	"TestCalInterimApiImportExportAsyncTest");
_LIT(KvCalNumberAsync,						"importAsyncvCal");
_LIT(KEntriesInDatabase,					"entriesInDatabase");
_LIT(KTest,									"test");
_LIT(KExportAsyncVcsFile,					"ExportAsyncFile");
_LIT(KExportAsyncToVCSFile, 				"Exporting Asynchronously to VCS file............. %S");

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportExportAsync - Common functions that are used between the import/export Async Tests
// This class Imports/Exports Asynchronously and Synchronously.
class CTestCalInterimApiImportExportAsync : public CTestCalInterimApiImportExport, public MCalDataExchangeCallBack
	{
public:
	CTestCalInterimApiImportExportAsync();
	~CTestCalInterimApiImportExportAsync();

public:
	void OpenVCSAndImportAsyncL(TDes& aPath, RFs aFsSession, TInt avCalNumber, RPointerArray<CCalEntry>& aCalEntryArray);
	void ImportvCalAsyncL(RReadStream& aReadStream, RPointerArray<CCalEntry>& aCalEntryArray);
	void ImportAsyncL();
	void ImportL();
	void ExportL();
	void ExportAsyncL();
	void ExportEntriesAsyncL(RPointerArray<CCalEntry>& aFetchEntryArray, const TDesC& aExportVCSPath);

	// from MCalDataExchangeCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed();
	TInt NumberOfEntriesToHandleAtOnce();

public:

private:
	TInt iIntvCalAsyncNumber;
	TPtrC iExportFilePath;
	TPtrC iExportAsyncFilePath;
	};

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimApiImportExportAsyncTest - This class imports/exports vCals Asynchronously and Synchronously,
// and checks for number of entries in database.
class CTestCalInterimApiImportExportAsyncTest : public CTestCalInterimApiImportExportAsync
	{
public:
	CTestCalInterimApiImportExportAsyncTest();
	~CTestCalInterimApiImportExportAsyncTest();
	TVerdict doTestStepL();

private:
	TInt iIntEntriesInDatabase;
	TInt iIntTestNumber;
	};

#endif //__TEST_CAL_INTERIM_IMPORT_EXPORT_ASYNC_H__
