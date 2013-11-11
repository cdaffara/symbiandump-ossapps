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

#include "TestCalInterimApiConcurrentFileAccess.h"

CTestCalInterimApiConcurrentFileAccess::CTestCalInterimApiConcurrentFileAccess()
:CTestCalInterimApiSuiteStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiConcurrentFileAccess);
	}


CTestCalInterimApiConcurrentFileAccess::~CTestCalInterimApiConcurrentFileAccess()
/**
 * Destructor
 */
	{
	delete iGuidList;
	delete iEntryIterator;
	delete iInstanceView;
	delete iEntryView;
	}


TVerdict CTestCalInterimApiConcurrentFileAccess::doTestStepL()
	{
	iInstanceView = CCalInstanceView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iEntryIterator=CCalIter::NewL(GetSession());
	iGuidList = new (ELeave) CDesC8ArrayFlat(4);
	TPtrC	dataFile;
	TESTL( GetStringFromConfig(ConfigSection(), KConcurrentFileAccessDataFile, dataFile) );
	CalendarOperationsL();
	return TestStepResult();
	}

/**
* Performs various calendar operation lik deletion of entries, find instances, fetch entries etc.. in the Calendar file
*/
void CTestCalInterimApiConcurrentFileAccess::CalendarOperationsL()
	{
	PopulateEntriesGuidListL();

	TBool 	addCategoriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFileAccessAddCategories, addCategoriesFlag);
	if(addCategoriesFlag)
		{
		INFO_PRINTF1(_L("AddCategories Start"));
		AddCategoriesL();
		INFO_PRINTF1(_L("AddCategories End"));
		}

	TBool 	removeCategoriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFileAccessRemoveCategories, removeCategoriesFlag);
	if(removeCategoriesFlag)
		{
		RemoveCategoriesL();
		}


	TBool fetchEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFetchEntriesFlag, fetchEntriesFlag);
	if(fetchEntriesFlag)
		{
		FetchEntriesL();
		}

	TBool findInstancesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFindInstancesFlag, findInstancesFlag);
	if(findInstancesFlag)
		{
		FindInstancesL();
		}

	TBool deleteEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFileAccessDeleteEntriesFlag, deleteEntriesFlag);


	if(deleteEntriesFlag)
		{

		TPtrC deleteTypeFlag;
		GetStringFromConfig(ConfigSection(), KConcurrentFileAccessDeleteTypeFlag, deleteTypeFlag);


		if(!deleteTypeFlag.Compare(KConcurrentFileAccessDeleteByUidList))
			{
			DeleteEntriesByUidListL();
			}
		else if(!deleteTypeFlag.Compare(KConcurrentFileAccessDeleteByReference))
			{
			DeleteEntriesByReferenceL();
			}
		else if(!deleteTypeFlag.Compare(KConcurrentFileAccessDeleteByArrayOfLocalUids))
			{
			DeleteEntriesByArrayOfLocalUidsL();
			}
		else if(!deleteTypeFlag.Compare(KConcurrentFileAccessDeleteByTimeRange))
			{
			INFO_PRINTF1(_L("Delete By Time Range Start"));
			DeleteEntriesByTimeRangeL();
			INFO_PRINTF1(_L("Delete By Time Range End"));
			}
		else if(!deleteTypeFlag.Compare(KConcurrentFileAccessDeleteInstances))
			{
			DeleteInstancesL();
			}
		}
	}

/**
* Delete entries in the calendar file by specifying a list of UIDs to delete.
*/
void CTestCalInterimApiConcurrentFileAccess::DeleteEntriesByUidListL()
	{
	INFO_PRINTF1(KConcurrentFileAccessDeleteOperation);

	TRAPD(err, iEntryView->DeleteL(*iGuidList));
	if((err != KErrNone))
		{
		ERR_PRINTF2(KConcurrentFileAccessErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	ValidateEntryCountL();
	}

/**
* Delete entries in the calendar file by specifying an array of Calendar Entries LocalUids,
*/
void CTestCalInterimApiConcurrentFileAccess::DeleteEntriesByArrayOfLocalUidsL()
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	iEntryView->GetIdsModifiedSinceDateL(minTime, localUids);
	TInt numSuccessfulDeleted(0);
	TRAPD(err, iEntryView->DeleteL(localUids,numSuccessfulDeleted));
	if(err != KErrNone)
		{
		ERR_PRINTF2(KConcurrentFileAccessErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&localUids); // Close()
	ValidateEntryCountL();
	}

/**
* Delete entries in the calendar file by specifying calendar entries by Reference,
*/
void CTestCalInterimApiConcurrentFileAccess::DeleteEntriesByReferenceL()
	{
	const TUint KGuidCount =  iGuidList->Count();
	for (TInt j(0); j < KGuidCount; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		iEntryView->FetchL(iGuidList->MdcaPoint(j), calEntries);

		const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
		if(KCalendarEntriesSpecificToGuid != 0)
			{
			INFO_PRINTF1(KConcurrentFileAccessDeleteOperation);
			// Deleting a Parent entry deletes all the associated child entries
			// Hence bypasing all child entries
			CCalEntry* calEntry = calEntries[0];
			TRAPD(err, iEntryView->DeleteL(*calEntry));
			if(err != KErrNone)
				{
				ERR_PRINTF2(KConcurrentFileAccessErrDeleteOperation,err);
				SetTestStepResult(EFail);
				}
			calEntries.ResetAndDestroy();
			}
		}
	ValidateEntryCountL();
	}



/**
* Delete entries in the calendar file by specifying time Range,
*/
void CTestCalInterimApiConcurrentFileAccess::DeleteEntriesByTimeRangeL()
	{
	TCalTime	calStartDateTime;
	calStartDateTime.SetTimeLocalL(TCalTime::MinTime());
	TCalTime	calEndDateTime;
	calEndDateTime.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange	calTimeRange(calStartDateTime, calEndDateTime);
	TRAPD(err, iEntryView->DeleteL(calTimeRange, CalCommon::EIncludeAll, *this));
	CActiveScheduler::Start();

	if(err != KErrNone)
		{
		ERR_PRINTF2(KConcurrentFileAccessErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	ValidateEntryCountL();
	}
/**
* Delete calendar Instances in the calendar file by specifying a time Range,
*/
void CTestCalInterimApiConcurrentFileAccess::DeleteInstancesL()
	{
	CCalInstanceView* instanceView = CCalInstanceView::NewL(GetSession(), *this);
	CleanupStack::PushL(instanceView);
	CActiveScheduler::Start();
	TInt err;
	RPointerArray<CCalInstance>	instanceArray;
	TCalTime	calStartDateTime;
	calStartDateTime.SetTimeLocalL(TCalTime::MinTime());
	TCalTime	calEndDateTime;
	calEndDateTime.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange	calTimeRange(calStartDateTime, calEndDateTime);

	FOREVER
		{
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
		instanceView->FindInstanceL(instanceArray, CalCommon::EIncludeAll, calTimeRange);
		if(instanceArray.Count() != 0)
			{
			TRAP(err, instanceView->DeleteL(instanceArray[0], CalCommon::EThisAndAll));
			if(err != KErrNone)
				{
				ERR_PRINTF2(KConcurrentFileAccessErrDeleteOperation,err);
				SetTestStepResult(EFail);
				break;
				}
			instanceArray.Remove(0);
			}
		else
			{
			CleanupStack::PopAndDestroy();
			break;
			}
		CleanupStack::PopAndDestroy();
		}

	CleanupStack::PopAndDestroy();
	ValidateEntryCountL();
	}

/**
* Add category information to the Calendar Entries
*/
void CTestCalInterimApiConcurrentFileAccess::AddCategoriesL()
	{
	CCalCategoryManager* categoryManager = CCalCategoryManager::NewL(GetSession());
	CleanupStack::PushL(categoryManager);

	RArray<TPtrC> categoryList;
	CleanupClosePushL(categoryList);
	TPtrC categories;
	GetStringFromConfig(ConfigSection(), KConcurrentFileAccessCategories, categories);
	TokenizeStringL(categories, categoryList);

	const TUint KCategoryCount = categoryList.Count();
	for(TInt i = 0; i < KCategoryCount; ++i)
		{
		TInt err = KErrNone;
		const TUint KGuidCount = iGuidList->Count();
		for(TInt j = 0; j < KGuidCount; j++)
			{
			RPointerArray<CCalEntry> calEntries;
			TRAP(err, iEntryView->FetchL(iGuidList->MdcaPoint(i), calEntries));

			if(err != KErrNone)
				{
				ERR_PRINTF2(KConcurrentFileAccessErrFetchOperation,err);
				SetTestStepResult(EFail);
				}
			else
				{
				const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();


				for(TInt k = (KCalendarEntriesSpecificToGuid - 1); k > -1 ; k--)
					{
					CCalCategory* category=CCalCategory::NewL(categoryList[i]);
					calEntries[k]->AddCategoryL(category);
					RPointerArray<CCalEntry> calEntriesCategory;
					calEntriesCategory.Append(calEntries[k]);
					TCalTime recurrenceId = calEntries[k]->RecurrenceIdL();
					TInt entriesStored(0);
					if(recurrenceId.TimeUtcL() != Time::NullTTime())
						{
						TRAP(err, iEntryView->StoreL(calEntriesCategory, entriesStored));
						}
					else
						{
						TRAP(err, iEntryView->UpdateL(calEntriesCategory, entriesStored));
						}
					if(err != KErrNone)
						{
						ERR_PRINTF2(KConcurrentFileAccessErrAddCategoryOperation,err);
						SetTestStepResult(EFail);
						}
					calEntriesCategory.Reset();
					}
					calEntries.ResetAndDestroy();
				}
			}
		}
	CleanupStack::Pop();
	categoryList.Close();

	CleanupStack::PopAndDestroy(); // categoryManager;			;
	}

/**
* Deletes categories information in calendar entries
*/
void CTestCalInterimApiConcurrentFileAccess::RemoveCategoriesL()
	{
	CCalCategoryManager* categoryManager = CCalCategoryManager::NewL(GetSession());
	CleanupStack::PushL(categoryManager);
	TInt countt = categoryManager->CategoryCountL();

	TInt err;
	const TUint KGuidCount = iGuidList->Count();
	for(TInt i = 0; i < KGuidCount; i++)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));

		TRAP(err, iEntryView->FetchL(iGuidList->MdcaPoint(i), calEntries));
		if(err != KErrNone)
			{
			ERR_PRINTF2(KConcurrentFileAccessErrFetchOperation,err);
			SetTestStepResult(EFail);
			}
		else
			{
			const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
			for(TInt j = (KCalendarEntriesSpecificToGuid - 1); j > -1; j--)
				{
    			const RPointerArray<CCalCategory>* categoryList = &(calEntries[j]->CategoryListL());
    			const TUint KCategoryCount = categoryList->Count();
				for ( TInt i = 0; i < KCategoryCount; i++ )
					{
					RPointerArray<CCalEntry> calEntriesCategory;
					calEntriesCategory.Append(calEntries[j]);
					TRAP( err, calEntries[j]->DeleteCategoryL(i) );
					TCalTime recurrenceId = calEntries[j]->RecurrenceIdL();
					TInt entriesStored(0);
					if(recurrenceId.TimeUtcL() != Time::NullTTime())
							{
							TRAP(err, iEntryView->StoreL(calEntriesCategory, entriesStored));
							}
					else
						{
						TRAP(err, iEntryView->UpdateL(calEntriesCategory, entriesStored));
						}
					if(err != KErrNone)
						{
						ERR_PRINTF2(KConcurrentFileAccessErrRemoveCategoryOperation,err);
						SetTestStepResult(EFail);
						}
					calEntriesCategory.Reset();
					}
				}
			}
		CleanupStack::PopAndDestroy(&calEntries);
		}
	CleanupStack::PopAndDestroy(); // categoryManager;			;
	}

/**
* Fetch entries in the calendar file
*/
void CTestCalInterimApiConcurrentFileAccess::FetchEntriesL()
	{
	TInt err = KErrNone;
	RPointerArray<CCalEntry> calEntriesArray;
	const TUint KGuidCount = iGuidList->Count();
	for (TInt i(0); i < KGuidCount; ++i)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));

		TPtrC8 calEntryGuid = iGuidList->MdcaPoint(i);

		TRAP(err, iEntryView->FetchL(calEntryGuid, calEntries));

		if(err != KErrNone)
			{
			ERR_PRINTF2(KConcurrentFileAccessErrFetchOperation,err);
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(&calEntries);
		}
	}

/**
* Find Instances in the calendar file by specifying a wide time Range,
*/
void CTestCalInterimApiConcurrentFileAccess::FindInstancesL()
	{
	TInt err = KErrNone;
	RPointerArray<CCalInstance>	instanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));

	TCalTime	calStartDateTime;
	calStartDateTime.SetTimeLocalL(TCalTime::MinTime());
	TCalTime	calEndDateTime;
	calEndDateTime.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange	calTimeRange(calStartDateTime, calEndDateTime);
	TRAP(err, iInstanceView->FindInstanceL(instanceArray, CalCommon::EIncludeAll, calTimeRange));

	if(err != KErrNone)
		{
		ERR_PRINTF2(KConcurrentFileAccessErrFindInstanceOperation,err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&instanceArray);
	}

/**
* Returns the count of number of calendar entries present currently in the Calendar File
* @return - Calendar Entry Count
*/
TInt CTestCalInterimApiConcurrentFileAccess::EntryCountL()
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	iEntryView->GetIdsModifiedSinceDateL(minTime, localUids);
	TInt entryCount = localUids.Count();
	CleanupStack::PopAndDestroy(&localUids); // Close()
	return(entryCount);
	}

/**
* Populates a CDescArray with guids of all the entries in the Calendar file
*/
void CTestCalInterimApiConcurrentFileAccess::PopulateEntriesGuidListL()
	{
	const TFileName&	fileName=GetCalenderFileName();
	TInt			count=0;

	TPtrC8	uid(iEntryIterator->FirstL());

	if(KNullDesC8()==uid)
		{
		return;
		}

	do
		{
		iGuidList->AppendL(uid);
		count++;
		uid.Set(iEntryIterator->NextL());
		}while (KNullDesC8()!=uid);

	INFO_PRINTF2(KConcurrentFileAccessInfoCalendarEntryCount,count);
	}

/**
* Validates the current entry count against the original calendar entry count
*/
void CTestCalInterimApiConcurrentFileAccess::ValidateEntryCountL()
	{
	TBool entryCountValidation = EFalse;
	GetBoolFromConfig(ConfigSection(), KConcurrentFileAccessEntryCountValidationFlag, entryCountValidation);
	if(entryCountValidation)
		{
		TInt expectedEntryCount = 0;
		GetIntFromConfig(ConfigSection(), KConcurrentExpectedEntryCount, expectedEntryCount);
		if(EntryCountL() != expectedEntryCount)
				{
				ERR_PRINTF3(KConcurrentFileAccessEntryCountMismatch, EntryCountL(), expectedEntryCount);
				SetTestStepResult(EFail);
				}
		}
	}
