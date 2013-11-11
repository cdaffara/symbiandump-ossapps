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



#ifndef __TESTCALINDEXFILE_MODIFY_ENTRY_STEP_H__
#define __TESTCALINDEXFILE_MODIFY_ENTRY_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalIndexFileStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <calalarm.h>


class CTestCalIndexFileModifyEntryStep: public CTestCalIndexFileStepBase
	{
public:
	CTestCalIndexFileModifyEntryStep();
	~CTestCalIndexFileModifyEntryStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	

	void ModifyEntryL();

	// over-ride from CTestCalIndexFileStepBase
	void GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps);

private:
	TInt iEntryNumToMod;

	};

_LIT(KTestCalIndexFileModifyEntryStep,"TestCalIndexFileModifyEntryStep");

#endif //__TESTCALINDEXFILE_MODIFY_ENTRY_STEP_H__
