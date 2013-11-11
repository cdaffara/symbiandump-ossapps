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



#ifndef TEST_CALINTERIMAPI_COMPARE_H
#define TEST_CALINTERIMAPI_COMPARE_H

#include "TestCalInterimApiSuiteStepBase.h"
#include <calentryview.h>

_LIT(KEntry1,						"entry1");
_LIT(KEntry2,						"entry2");
_LIT(KParentOfChildEntry1,			"parentOfChildEntry1");
_LIT(KParentOfChildEntry2,			"parentOfChildEntry2");
_LIT(KTrans,						"trans");
_LIT(KLat,							"GEOLat");
_LIT(KLong,							"GEOLong");
_LIT(KCompareValue,					"comparison");

// Class CTestCalInterimApiCompare - Compares two entries with another
class CTestCalInterimApiCompare: public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiCompare();
	~CTestCalInterimApiCompare();
	
	TVerdict 		doTestStepL();
	
private:
	CCalEntry* GetEntryL(const TPtrC& aUid, const TPtrC& aRecuurenceId, TBool aGetChild); 
	void SetGeoValueEntryL(const TDesC& aDataLat, const TDesC& aDataLong);
	
private:
	CCalEntryView *iEntryView;
	CCalEntry* iCopyEntry;
	};

_LIT(KTestCalInterimApiCompare,"TestCalInterimApiCompare");

#endif // TEST_CALINTERIMAPI_COMPARE_H
