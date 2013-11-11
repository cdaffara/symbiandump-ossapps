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



#ifndef TEST_CALINTERIMAPI_DELETE_ENTRY_BYTIMERANGE_H
#define TEST_CALINTERIMAPI_DELETE_ENTRY_BYTIMERANGE_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstance.h>
#include <calentry.h>

class CCalEntryView;

/**
This test step deletes the entries specified in the time range and
in the entry type given
*/
class CTestCalInterimApiDeleteEntryByTimeRange : public CTestCalInterimApiSuiteStepBase 
	{
public:
	CTestCalInterimApiDeleteEntryByTimeRange();
	~CTestCalInterimApiDeleteEntryByTimeRange();

	virtual TVerdict	doTestStepL();

	};

_LIT(KTestCalInterimApiDeleteEntryByTimeRange, "TestCalInterimApiDeleteEntryByTimeRange");

#endif
