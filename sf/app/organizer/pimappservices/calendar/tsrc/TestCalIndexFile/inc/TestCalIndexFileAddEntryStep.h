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



#ifndef __TESTCALINDEXFILE_ADDENTRY_STEP_H__
#define __TESTCALINDEXFILE_ADDENTRY_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalIndexFileStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <calalarm.h>


class CTestCalIndexFileAddEntryStep: public CTestCalIndexFileStepBase
	{
public:
	CTestCalIndexFileAddEntryStep();
	~CTestCalIndexFileAddEntryStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	

	// over-ride from CTestCalIndexFileStepBase
	void GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps);

	};

_LIT(KTestCalIndexFileAddEntryStep,"TestCalIndexFileAddEntryStep");

#endif //__TESTCALINDEXFILE_ADDENTRY_STEP_H__
