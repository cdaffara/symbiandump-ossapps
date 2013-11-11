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

#include "TestCalInterimApiDeleteCategory.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calcategory.h>

_LIT(KGUid,								"guid");
_LIT(KCategory,							"category");
_LIT(KRecurrenceId,						"recurrenceid");
_LIT(KTimeMode,							"timemode");
_LIT(KAllocTest,						"alloctest");

_LIT(KInfoDeleteCategory,				"DeleteCategoryL() successful");
_LIT(KInfoEntryNotFetched,				"Entry Not fetched");

_LIT(KErrDeleteCategory,				"Error in DeleteCategoryL()");


CTestCalInterimApiDeleteCategory::CTestCalInterimApiDeleteCategory()
:	CTestCalInterimApiSuiteStepBase()
,	iAllocTest(EFalse)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteCategory);
	}

CTestCalInterimApiDeleteCategory::~CTestCalInterimApiDeleteCategory()
/**
 * Destructor
 */
	{
	delete iCalEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteCategory::doTestStepL()
	{
	TPtrC	guid;
	TPtrC	recurrenceId;
	TPtrC	timeMode;
	
	TESTL( GetStringFromConfig(ConfigSection(), KGUid, guid) );
	TESTL( GetStringFromConfig(ConfigSection(), KCategory, iCategory) );
	// Dont  mind if these are not given in the ini file
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceId);
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	GetBoolFromConfig(ConfigSection(), KAllocTest, iAllocTest);
	
	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	FetchEntryAndDeleteCategoryL(guid, recurrenceId, timeMode);		
	
	return TestStepResult();
	}

/** Fetch the entry from the file and delete the category of Parent / child entry
@param	aGuid Guid of the entry to be fetched
@param	aRecurrenceId Recurrence Id of the child entry
@param	aTimeMode timemode of the entry ie. Floating time/ Utc
*/		
void CTestCalInterimApiDeleteCategory::FetchEntryAndDeleteCategoryL(const TPtrC& aGuid, const TPtrC& aRecurrenceId, const TPtrC& aTimeMode)
	{
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*	entryId = HBufC8::NewLC(aGuid.Length());
	entryId->Des().Copy(aGuid);
	iCalEntryView->FetchL(entryId->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryId);

	if ( entriesFetched.Count() )
		{
		for ( TInt i = 0; i < entriesFetched.Count(); i++ )
			{
			CCalEntry*	entry = entriesFetched[i];
			if ( aRecurrenceId != KNullDesC16() )
				{
				TTime	recurrIdValue;
				recurrIdValue.Set(aRecurrenceId);
				TCalTime	recurrenceId;
				// Sets the time mode to floating or Utc based on the time mode given
				SetCalTimeL(recurrIdValue, recurrenceId, SetTimeMode(aTimeMode));
				if ( recurrenceId.TimeUtcL() == (entry->RecurrenceIdL().TimeUtcL()) )
					{
					// Deletes the category of the child entry
					DeleteCategoryL(entry);	
					// Store the entry after deleting the category
					StoreCalendarEntryL(iCalEntryView, entry);
					}		
				}
			else if ( entry->RecurrenceIdL().TimeUtcL() == Time::NullTTime() )
				{
				// Deletes the category of the parent entry
				DeleteCategoryL(entry);	
				// Store the entry after deleting the category
				StoreCalendarEntryL(iCalEntryView, entry);
				}
			}
		}
	else
		{
		ERR_PRINTF1(KInfoEntryNotFetched);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&entriesFetched);
	}
	
/** Deletes the Specified category based on the category given from the ini file and returns the CCalEntry object
@param	aEntry Pointer to CCalEntry
*/
void CTestCalInterimApiDeleteCategory::DeleteCategoryL(CCalEntry* aEntry)
	{
	const RPointerArray<CCalCategory>*	categoryList = &(aEntry->CategoryListL());
	for ( TInt i = 0; i < categoryList->Count(); i++ )
		{
		if ( (iCategory.Compare((*categoryList)[i]->ExtendedCategoryName())) == 0 )
			{
			if ( iAllocTest )
				{
				PerformOOMForDeleteCategoryL(aEntry, i);
				}
			else
				{
				TRAPD( err, aEntry->DeleteCategoryL(i) );
				if ( err == KErrNone )
					{
					INFO_PRINTF1(KInfoDeleteCategory);
					}	
				else
					{
					ERR_PRINTF1(KErrDeleteCategory);
					SetTestStepError(err);
					SetTestStepResult(EFail);
					}
				}	
			}
		}
	}

/** OOM testing for fetching the entry from the file and delete the category of Parent / child entry
@param	aEntry Pointer to CCalEntry
@param	aIndex Category count
*/		
void CTestCalInterimApiDeleteCategory::PerformOOMForDeleteCategoryL(CCalEntry* aEntry,const TInt aIndex)
	{
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAPD(err, aEntry->DeleteCategoryL(aIndex));
				
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for DeleteCategory is done"));
			break;
			}
		if ( err != KErrNoMemory )
			SetTestStepResult(EFail);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}	
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}



