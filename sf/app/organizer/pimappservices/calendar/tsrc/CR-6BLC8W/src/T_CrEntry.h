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

#if (!defined __T_CR_TEST_ENTRY_H__)
#define __T_CR_TEST_ENTRY_H__



#include "T_CrSuiteStepBase.h"

_LIT(KTestCalInterimApiStoreFetch,"TestCalInterimApiStoreFetch");

class CTestCalInterimApiStoreFetch : public CCrTestStepBase
	{
public:
	CTestCalInterimApiStoreFetch();
	~CTestCalInterimApiStoreFetch();
	TVerdict doTestStepL();
	};


_LIT(KTestCalInterimApiStoreFetchRpt,"TestCalInterimApiStoreFetchRpt");

class CTestCalInterimApiStoreFetchRpt : public CCrTestStepBase
	{
public:
	CTestCalInterimApiStoreFetchRpt();
	~CTestCalInterimApiStoreFetchRpt();
	TVerdict doTestStepL();
	void GetDatesL(TPtrC& aDataPtr, RArray<TTime>& dates);
	};


#endif //__T_CR_TEST_ENTRY_H__
