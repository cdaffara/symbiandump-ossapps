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



#ifndef TEST_CALINTERIMAPI_DELETE_ENTRYBYLOCALID_H
#define TEST_CALINTERIMAPI_DELETE_ENTRYBYLOCALID_H

#include "TestCalInterimApiSuiteStepBase.h"
#include <calentry.h>

class CCalEntryView;

/**
This test step fetches the entry from the calendar file and gets the local uid
entry. Based on the recurrence id, the parent and child entries are appended to the
array and then deletes the entry by passing the array of local Uid of entries.
*/
class CTestCalInterimApiDeleteEntryByLocalId : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteEntryByLocalId();
	~CTestCalInterimApiDeleteEntryByLocalId();

	virtual TVerdict	doTestStepL();

private:
	void	FetchEntryAndAppendToArrayL(const TPtrC& aGuid);
	void	DeleteArrayOfLocalIdsL();
	void	PerformOOMForDeleteByLocalIdL(TInt aNumOfEntriesDeleted);
	void	GetLocalIdsAndAppendToArrayL();
	
private:
	RArray<TCalTime>		iRecurrenceIdArray;
	RArray<TCalLocalUid>	iEntriesLocalIdToDelete;
	CCalEntryView*			iEntryView;
	TBool					iAllocTest;
	};

_LIT(KTestCalInterimApiDeleteEntryByLocalId, "TestCalInterimApiDeleteEntryByLocalId");

#endif
