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



#ifndef TEST_CALINTERIMAPI_DELETE_DATA_H
#define TEST_CALINTERIMAPI_DELETE_DATA_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calentry.h>
#include <badesca.h>


/**
This test step deletes the array of uid of entries and also
deletes the parent or child entry based on the recurrence id
given.
*/

class CCalInstanceView;
class CCalEntryView;
class CTestCalInterimApiDeleteData : public CTestCalInterimApiSuiteStepBase 
	{
public:
	CTestCalInterimApiDeleteData();
	~CTestCalInterimApiDeleteData();

	virtual TVerdict	doTestStepL();

private:
	void	FetchAndDeleteEntryL(const TPtrC& aGuid, const TInt& aExpectedCount,
								 const TPtrC& aRecurrenceIdValue, const TPtrC& aTimeMode, const TInt& aExpectedCountAfterDelete);
	void	DeleteArrayOfUidsL();
	void	CheckRecurrenceIdDeletedL(RPointerArray<CCalEntry>& aEntriesFetchedAfterDelete);	
	void	DeleteAndVerifyL(CCalEntry* aCalEntry, const TPtrC& aRecurrenceIdValue, const TInt& aEntryCountBeforeDelete);	
	
private:
	CDesC8ArrayFlat*	iGuidArray;
	TCalTime			iRecurrenceId;
	CCalEntryView*		iEntryView;
	};

_LIT(KTestCalInterimApiDeleteData, "TestCalInterimApiDeleteData");

#endif
