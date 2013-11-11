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



#ifndef TEST_CALINTERIMAPI_FETCH_ENTRYBYLOCALID_H
#define TEST_CALINTERIMAPI_FETCH_ENTRYBYLOCALID_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>

class CCalEntryView;

/**
This test step fetches the entry from the calendar file by passing the local id of the entry
*/
class CTestCalInterimApiFetchEntryByLocalId : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiFetchEntryByLocalId();
	~CTestCalInterimApiFetchEntryByLocalId();

	virtual TVerdict	doTestStepL();
	
private:
	void	CheckEntryL(CCalEntry* aEntry, const TInt aLocalId);
	CCalEntry*	OOMForFetchEntryByLocalIdL(const TInt aLocalId);
	
private:
	CCalEntryView*	iCalEntryView;
	};

_LIT(KTestCalInterimApiFetchEntryByLocalId, "TestCalInterimApiFetchEntryByLocalId");

#endif
