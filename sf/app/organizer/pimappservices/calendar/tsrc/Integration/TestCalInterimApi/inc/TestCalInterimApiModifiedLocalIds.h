/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef __TEST_CALINTERIMAPI_MODIFIED_LOCAL_IDS_H__
#define __TEST_CALINTERIMAPI_MODIFIED_LOCAL_IDS_H__

#include "TestCalInterimApiSuiteStepBase.h"
#include <calentryview.h>
#include <caltime.h>

/** CTestCalInterimApiModifiedLocalIds */
_LIT(KTestCalInterimApiModifiedLocalIds,"TestCalInterimApiModifiedLocalIds");
_LIT(KLocalIds,							"localids");
_LIT(KSystemTime,						"systemTime");
_LIT(KSoak,								"soak");
_LIT(KNumberOfCalls,					"numberofcalls");

/*
* Class CTestCalInterimApiModifiedLocalIds  - This class verifies that the modified local ids returned are correct.
* The script TestCalModifiedLocalIds.script modifies parent and child entries.
*/
class CTestCalInterimApiModifiedLocalIds: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiModifiedLocalIds();
	~CTestCalInterimApiModifiedLocalIds();
	TVerdict doTestStepL();

private:
	TBuf<KMaxLengthString>	iField;
	TInt					iIntLocalIds;
	TPtrC					iPtrSystemTime;
	TPtrC					iPtrSoak;
	TInt					iRecurCount;
	CCalEntryView*			iEntryView;

public:
	};

#endif // __TEST_CALINTERIMAPI_MODIFIED_LOCAL_IDS_H__
