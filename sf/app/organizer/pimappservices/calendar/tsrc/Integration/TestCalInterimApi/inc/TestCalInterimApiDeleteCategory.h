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



#ifndef TEST_CALINTERIMAPI_DELETE_CATEGORY_H
#define TEST_CALINTERIMAPI_DELETE_CATEGORY_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calentry.h>
#include <calcategory.h>
#include <calcategorymanager.h>


/**
This test step fetches the entry from the calendar file and then deletes the specified
category from the parent or child entry. 
*/

class CCalCategory;
class CCalEntryView;
class CTestCalInterimApiDeleteCategory : public CTestCalInterimApiSuiteStepBase
	{
public:
	CTestCalInterimApiDeleteCategory();
	~CTestCalInterimApiDeleteCategory();

	virtual TVerdict	doTestStepL();

private:
	void	FetchEntryAndDeleteCategoryL(const TPtrC& aGuid, const TPtrC& aRecurrenceId, const TPtrC&	aTimeMode);	
	void	PerformOOMForDeleteCategoryL(CCalEntry* aEntry,const TInt aIndex);	
	void	DeleteCategoryL(CCalEntry* aEntry);
	
private:
	CCalEntryView*	iCalEntryView;
	TCalTime	iRecurrenceId;
	TPtrC		iCategory;
	TBool		iAllocTest;
	};

_LIT(KTestCalInterimApiDeleteCategory, "TestCalInterimApiDeleteCategory");

#endif

