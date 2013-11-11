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

#ifndef __TEST_CALINTERIMAPI_TZ_CHANGES_H__
#define __TEST_CALINTERIMAPI_TZ_CHANGES_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include <calinstanceview.h>
#include <calinstance.h>

/** CTestCalInterimTZChange */
_LIT(KTestCalInterimApiTZChange,"TestCalInterimApiTZChange");
_LIT(KIsFloating,				"isFloating");
_LIT(KExpectedInstanceStart,	"expectedInstanceStart");
_LIT(KExpectedInstanceEnd,		"expectedInstanceEnd");
_LIT(KExpectedEntryStart,		"expectedEntryStart");
_LIT(KExpectedEntryEnd,			"expectedEntryEnd");
_LIT(KExpectedEntryAlarmOffset,	"expectedAlarmOffset");
_LIT(KFilterStart,				"startdatefilter");
_LIT(KFilterEnd,				"enddatefilter");

_LIT(KAlarmOffsetTZ,			"Alarm offset of entry correct after TZ change");
_LIT(KInstanceTimesCorrect,		"Instance Times correct after TZ change");
_LIT(KEntryTimesCorrect,		"Entry Times correct after TZ change");

/** CTestCalInterimDSTChange */
_LIT(KTestCalInterimApiDSTChange,	"TestCalInterimApiDSTChange");

// Class CTestCalInterimApiTZChange - This class verifies the times of entry and instance after TZ Change, DST change and system time change, and
// also checks for the time mode of all times.
class CTestCalInterimApiTZChange : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiTZChange();
	~CTestCalInterimApiTZChange();
	TVerdict doTestStepL();
	
public:
	void SetTimeRangeL();
	void CheckInstancesTimesL(RPointerArray<CCalInstance>& aFetchInstanceArray, TInt aIndex);
	void GetInstanceDataFromIniFileL(TInt aIndex);
	void CheckEntryTimesL(RPointerArray<CCalInstance>& aFetchInstanceArray);
	void GetEntryDataFromIniFileL();
	
private:
	CCalInstanceView* iInstanceView;
	TBool iIsFloating;
	TBuf<KMaxLengthString> iField;
	TCalTime iStartDate;
	TCalTime iEndDate;
	TPtrC iPtrExpectedInstanceStart;
	TPtrC iPtrExpectedInstanceEnd;
	TPtrC iPtrExpectedEntryStart;
	TPtrC iPtrExpectedEntryEnd;
	TPtrC iPtrFilterStart;
	TPtrC iPtrFilterEnd;
	TInt iIntAlarmOffset;
	};
	
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimDSTChange - Waits for one second to enable DST change
class CTestCalInterimApiDSTChange : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDSTChange();
	~CTestCalInterimApiDSTChange();
	TVerdict doTestStepL();
	
private:
	};
	
#endif //__TEST_CALINTERIMAPI_TZ_CHANGES_H__

