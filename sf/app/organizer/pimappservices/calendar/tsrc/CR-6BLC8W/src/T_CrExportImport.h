// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if (!defined __T_CR_TEST_EXPORT_IMPORT_H__)
#define __T_CR_TEST_EXPORT_IMPORT_H__



#include "T_CrSuiteStepBase.h"



_LIT(KTestCalInterimApiExportImport,"TestCalInterimApiExportImport");

class CTestCalInterimApiExportImport : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportImport();
	~CTestCalInterimApiExportImport();
	TVerdict doTestStepL();
	};


_LIT(KTestCalInterimApiImportMixed,"TestCalInterimApiImportMixed");

class CTestCalInterimApiImportMixed : public CCrTestStepBase
	{
public:
	CTestCalInterimApiImportMixed();
	~CTestCalInterimApiImportMixed();
	TVerdict doTestStepL();
	};



_LIT(KTestCalInterimApiExportImportRpt,"TestCalInterimApiExportImportRpt");

class CTestCalInterimApiExportImportRpt : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportImportRpt();
	~CTestCalInterimApiExportImportRpt();
	TVerdict doTestStepL();
	};

_LIT(KTestCalInterimApiExportImportExc,"TestCalInterimApiExportImportExc");

class CTestCalInterimApiExportImportExc : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportImportExc();
	~CTestCalInterimApiExportImportExc();
	TVerdict doTestStepL();
	};

_LIT(KTestCalInterimApiExportImportSpor,"TestCalInterimApiExportImportSpor");

class CTestCalInterimApiExportImportSpor : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportImportSpor();
	~CTestCalInterimApiExportImportSpor();
	TVerdict doTestStepL();
	};

_LIT(KTestCalInterimApiExportRecur,"TestCalInterimApiExportRecur");	

class CTestCalInterimApiExportRecur : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportRecur();
	~CTestCalInterimApiExportRecur();
	TVerdict doTestStepL();
private:
	void setUpEntriesL(RPointerArray<CCalEntry>& aEntries);
	void setUpEntries2L(RPointerArray<CCalEntry>& aEntries);
	};
	
_LIT(KTestCalInterimApiExportImportNegOffset,"TestCalInterimApiExportImportNegOffset");	

class CTestCalInterimApiExportImportNegOffset : public CCrTestStepBase
	{
public:
	CTestCalInterimApiExportImportNegOffset();
	~CTestCalInterimApiExportImportNegOffset();
	TVerdict doTestStepL();
private:

	void setUpEntriesL(RPointerArray<CCalEntry>& aEntries);
	void setUpEntriesWithAlarmOnNextDayL(RPointerArray<CCalEntry>& aEntries);
	};

#endif //__T_CR_TEST_EXPORT_IMPORT_H__
