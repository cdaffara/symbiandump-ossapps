// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "TestCalInterimApiGetCategoryList.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calcategory.h>

_LIT(KGUid,							"guid");
_LIT(KParentCategory,				"parentcategory");
_LIT(KChildCategory,				"childcategory");
_LIT(KRecurrenceId,					"recurrenceid");
_LIT(KExpectedCountForParent,		"expectedcountforparent");
_LIT(KExpectedCountForChild,		"expectedcountforchild");
_LIT(KTimeMode,						"timemode");

_LIT(KInfoCategoryMatches,			"Category %S matches with entry's category List");
_LIT(KInfoParentEntryCategoryCount,	"Number of Categories for Parent Entry is %d");
_LIT(KInfoChildEntryCategoryCount,	"Number of Categories for the Child Entry is %d ");
_LIT(KInfoCategoryList,				"CategoryList is retreived correctly ");

_LIT(KErrCategoryNotEqual,			"Categories %S : %S are not equal"); 
_LIT(KErrCategoryList,				"CategoryList %d : ExpectedCategoryCount %d");

CTestCalInterimApiGetCategoryList::CTestCalInterimApiGetCategoryList()
:	CTestCalInterimApiSuiteStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiGetCategoryList);
	}

CTestCalInterimApiGetCategoryList::~CTestCalInterimApiGetCategoryList()
/**
 * Destructor
 */
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiGetCategoryList::doTestStepL()
	{
	TPtrC	gUid;
	TPtrC	parentCategories;
	TPtrC	childCategories;
	TPtrC	recurrenceId;
	TPtrC	timeMode;
	TInt	expectedCategoryCountForParent;
	TInt	expectedCategoryCountForChild;
			
	TESTL( GetStringFromConfig(ConfigSection(), KGUid, gUid) );
	// Dont mind if these are not given in the ini file
	GetStringFromConfig(ConfigSection(), KParentCategory, parentCategories);
	GetStringFromConfig(ConfigSection(), KChildCategory, childCategories);
	GetIntFromConfig(ConfigSection(), KExpectedCountForParent, expectedCategoryCountForParent); 
 	GetIntFromConfig(ConfigSection(), KExpectedCountForChild, expectedCategoryCountForChild);
 	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
				

	CCalEntryView*	calEntryView = CCalEntryView::NewL(GetSession(), *this);
	CleanupStack::PushL(calEntryView);
	CActiveScheduler::Start();
	
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	
	HBufC8*	entryGuid = HBufC8::NewLC(gUid.Length());
	entryGuid->Des().Copy(gUid);
	calEntryView->FetchL(entryGuid->Des(),	entriesFetched);
	CleanupStack::PopAndDestroy(entryGuid);
	
	RArray<TPtrC>	categoryArrayForParent;
	CleanupClosePushL(categoryArrayForParent);
	TokenizeStringL(parentCategories, categoryArrayForParent);	
	RArray<TPtrC>	categoryArrayForChild;
	CleanupClosePushL(categoryArrayForChild);
	TokenizeStringL(childCategories, categoryArrayForChild);	
	
	GetCategoryListAndNameL(entriesFetched, recurrenceId, categoryArrayForParent, categoryArrayForChild,
							expectedCategoryCountForParent, expectedCategoryCountForChild, timeMode);		
							

	CleanupStack::PopAndDestroy(2); // categoryArrayForParent, categoryArrayForChild
	CleanupStack::PopAndDestroy(&entriesFetched);
	CleanupStack::PopAndDestroy(calEntryView);
	
	return TestStepResult();
	}

/** It gets the category list and category name of one parent/ child entry at a time and 
compares it with the set values.
@param	aEntriesFetched	 Array containing the fetched entries
@param	aRecurrenceId Recurrence id of the child entry fetched from the ini file
@param	aCategoryArrayForParent	Array containing the parent categories fetched from the ini file
@param	aCategoryArrayForChild	Array containing the child categories fetched from the ini file
@param	aExpectedCategoryCountForParent	expected Parent entry's caategory count
@param	aExpectedCategoryCountForChild	Expected Child entry's category count
@param	aTimeMode	Timemode of the entry ie. Floating/UTC
*/
void CTestCalInterimApiGetCategoryList::GetCategoryListAndNameL(RPointerArray<CCalEntry>& aEntriesFetched, const TPtrC& aRecurrenceId,
															 RArray<TPtrC>& aCategoryArrayForParent, RArray<TPtrC>& aCategoryArrayForChild,
															 const TInt& aExpectedCategoryCountForParent, const TInt& aExpectedCategoryCountForChild,
															 const TPtrC& aTimeMode)
	{
	for ( TInt i = 0; i < aEntriesFetched.Count(); i++ )
		{
		// If recurrenceId is present, then it is child entry
		if ( aRecurrenceId != KNullDesC16() )
			{
			TTime	recurrence;
			recurrence.Set(aRecurrenceId);
			TCalTime	recurrenceId;
			// Sets the time mode to floating or Utc based on the time mode given
			SetCalTimeL(recurrence, recurrenceId, SetTimeMode(aTimeMode));
			
			// Verifies if the recurrence id matches in the entry's list
			if ( aEntriesFetched[i]->RecurrenceIdL().TimeUtcL() == recurrenceId.TimeUtcL() )
				{
				const RPointerArray<CCalCategory>* categoryList = &(aEntriesFetched[i]->CategoryListL());
				// If category count matches with the ini data, then get the category name
				if ( CheckCategoryCount(categoryList, aExpectedCategoryCountForChild) ) 
					{
					TInt	childEntryCategoryCount = categoryList->Count();
					INFO_PRINTF3(KInfoChildEntryCategoryCount, i, childEntryCategoryCount);
					GetCategoryNameAndCompare(categoryList, aCategoryArrayForChild);	
					}
				break; // Completed operation with respect to the current recurrence Id
				}		
			}
		else  // if recurrenceId is not present, then it is parent entry
			{
			const RPointerArray<CCalCategory>* categoryList = &(aEntriesFetched[i]->CategoryListL());	
			if ( CheckCategoryCount(categoryList, aExpectedCategoryCountForParent) )
				{	
				TInt	parentEntryCategoryCount(categoryList->Count());
				INFO_PRINTF2(KInfoParentEntryCategoryCount, parentEntryCategoryCount);
				GetCategoryNameAndCompare(categoryList, aCategoryArrayForParent);
				}
				break;	// Completed operation with respect to the parent entry
			}
		}
	}


/** Gets the category Name of the entry and compare it with the set value
@param	aCategoryList Array containing the list of categories present in the entry
@param	aCategoryArray Array containing the categories fetched from ini file
*/
void CTestCalInterimApiGetCategoryList::GetCategoryNameAndCompare(const RPointerArray<CCalCategory>* aCategoryList, RArray<TPtrC>& aCategoryArray)
	{
	for ( TInt j = 0; j < aCategoryList->Count() ; j++)
		{
		const TPtrC&	category = aCategoryArray[j];
		const TDesC&	categoryName((*aCategoryList)[j]->ExtendedCategoryName());
		if ( category.Compare(categoryName) == 0 )
			{
			INFO_PRINTF2(KInfoCategoryMatches, &category);
			}
		else
			{
			ERR_PRINTF3(KErrCategoryNotEqual, &category, &categoryName);
			SetTestStepResult(EFail);
			}
		}
	}

/** Gets the category count from the entry and compares it with the expected count
@param	aCategoryList Array containing the list of categories present in the entry
@param	aExpectedCategoryCount Expected category count fetched from ini file
*/
TBool CTestCalInterimApiGetCategoryList::CheckCategoryCount(const RPointerArray<CCalCategory>* aCategoryList, const TInt& aExpectedCategoryCount)
	{
	TBool	result = EFalse;
	if ( aCategoryList->Count() == aExpectedCategoryCount)
		{
		INFO_PRINTF1(KInfoCategoryList);
		result = ETrue;
		}
	else
		{
		ERR_PRINTF3(KErrCategoryList, aCategoryList->Count(), aExpectedCategoryCount);
		SetTestStepResult(EFail);
		}
	return result;
	}
	



