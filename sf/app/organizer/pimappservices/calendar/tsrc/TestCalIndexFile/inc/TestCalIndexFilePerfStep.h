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



#ifndef __TESTCALINDEXFILE_PERF_STEP_H__
#define __TESTCALINDEXFILE_PERF_STEP_H__

#include <test/testexecutestepbase.h>
#include "TestCalIndexFileStepBase.h"

#include <calinstanceview.h>
#include <calprogresscallback.h>
#include <calentry.h>
#include <calalarm.h>


class CTestCalIndexFilePerfStep: public CTestCalIndexFileStepBase
	{
public:
	CTestCalIndexFilePerfStep();
	~CTestCalIndexFilePerfStep();
	virtual TVerdict doTestStepL();
	virtual TVerdict doTestStepPostambleL();	
	};

_LIT(KTestCalIndexFilePerfStep,"TestCalIndexFilePerfStep");

#endif //__TESTCALINDEXFILE_PERF_STEP_H__
