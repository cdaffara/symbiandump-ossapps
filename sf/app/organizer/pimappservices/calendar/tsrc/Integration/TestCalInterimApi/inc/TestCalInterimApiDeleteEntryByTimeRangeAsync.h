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



#ifndef TEST_CALINTERIMAPI_DELETE_ENTRY_BY_TIMERANGE_ASYNC_H
#define TEST_CALINTERIMAPI_DELETE_ENTRY_BY_TIMERANGE_ASYNC_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstance.h>
#include <calentry.h>

class CCalEntryView;

/**
This test step deletes the entries specified in the time range and in the entry
type given. This test is specific to the async test. If the file to delete is 
currently being used by the server, it will wait 500ms until it attempts to 
access the file again until. It will continue to do this until it gains access.
*/
class CTestCalInterimApiDeleteEntryByTimeRangeAsync : public CTestCalInterimApiSuiteStepBase 
	{
public:
	CTestCalInterimApiDeleteEntryByTimeRangeAsync();
	~CTestCalInterimApiDeleteEntryByTimeRangeAsync();

	virtual TVerdict	doTestStepL();
	};

_LIT(KTestCalInterimApiDeleteEntryByTimeRangeAsync, "TestCalInterimApiDeleteEntryByTimeRangeAsync");

#endif
