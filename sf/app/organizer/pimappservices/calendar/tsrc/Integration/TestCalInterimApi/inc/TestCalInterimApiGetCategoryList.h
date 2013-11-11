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



#ifndef TEST_CALINTERIMAPI_GETCATEGORYLIST_H
#define TEST_CALINTERIMAPIM_GETCATEGORYLIST_H

#include "TestCalInterimApiSuiteStepBase.h"

#include <calcommon.h>
#include <calinstance.h>
#include <calentry.h>

class CCalEntryView;

/**
This test step fetches the entry from the calendar file, gets the category
list and category name of one parent or child entry at a time and compares it
with the data given in the ini file.
*/
class CTestCalInterimApiGetCategoryList : public CTestCalInterimApiSuiteStepBase 
	{
public:
	CTestCalInterimApiGetCategoryList();
	~CTestCalInterimApiGetCategoryList();

	virtual TVerdict	doTestStepL();

private:
	void	GetCategoryListAndNameL(RPointerArray<CCalEntry>& aEntriesFetched, const TPtrC& aRecurrenceId,
									RArray<TPtrC>& aCategoryArrayForParent, RArray<TPtrC>& aCategoryArrayForChild,
									const TInt& aExpectedCategoryCountForParent, const TInt& aExpectedCategoryCountForChild,
									const TPtrC& aTimeMode);
	TBool	CheckCategoryCount(const RPointerArray<CCalCategory>* aCategoryList, const TInt& aExpectedCategoryCount);									
	void	GetCategoryNameAndCompare(const RPointerArray<CCalCategory>* aCategoryList, RArray<TPtrC>& aCategoryArray);
	};

_LIT(KTestCalInterimApiGetCategoryList, "TestCalInterimApiGetCategoryList");

#endif
