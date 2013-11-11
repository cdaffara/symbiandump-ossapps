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



#ifndef __TESTCALINDEXFILE_DELETE_ENTRY_STEP_H__
#define __TESTCALINDEXFILE_DELETE_ENTRY_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalIndexFileStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <calalarm.h>


class CTestCalIndexFileDeleteEntryStep: public CTestCalIndexFileStepBase
	{
public:
	CTestCalIndexFileDeleteEntryStep();
	~CTestCalIndexFileDeleteEntryStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	

	void DeleteEntryL();

	//over-ride from CTestCalIndexFileStepBase
	//TBool ValidateDbContentsL();
	void GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps);

private:
	TInt	iEntryNumToDelete;
	};

_LIT(KTestCalIndexFileDeleteEntryStep,"TestCalIndexFileDeleteEntryStep");

#endif //__TESTCALINDEXFILE_DELETE_ENTRY_STEP_H__
