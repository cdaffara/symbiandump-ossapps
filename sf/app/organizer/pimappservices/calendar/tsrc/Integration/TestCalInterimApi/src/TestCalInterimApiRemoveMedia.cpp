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

#include "TestCalInterimApiRemoveMedia.h"

CTestCalInterimApiRemoveMedia::CTestCalInterimApiRemoveMedia()
:CTestCalInterimApiReadData()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalInterimApiRemoveMedia);
	}

/**
* Virtual function called while parsing the entries from the data file
* A copy of the entry is made since the entry goes out of scope
@param aEntry - current entry being parsed
*/
void CTestCalInterimApiRemoveMedia::ProcessEntryL(CCalEntry* aEntry)
	{
	CCalEntry* entryCopy = NULL;
	HBufC8* guid = aEntry->UidL().AllocLC();
	TCalTime recurrenceId = aEntry->RecurrenceIdL();
	if(recurrenceId.TimeUtcL() != Time::NullTTime())
		{
		entryCopy = CCalEntry::NewL(aEntry->EntryTypeL(), guid, aEntry->MethodL(),
												 aEntry->SequenceNumberL(), aEntry->RecurrenceIdL(),
													   aEntry->RecurrenceRangeL());
		}
	else
		{
		entryCopy = CCalEntry::NewL(aEntry->EntryTypeL(), guid, aEntry->MethodL(),
												 aEntry->SequenceNumberL());
		}
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entryCopy);
	entryCopy->CopyFromL(*aEntry);
	iEntryArray.AppendL(entryCopy);
	CleanupStack::Pop(entryCopy);
	}

CTestCalInterimApiRemoveMedia::~CTestCalInterimApiRemoveMedia()
/**
 * Destructor
 */
	{
	TRAP_IGNORE(ReleaseResourcesL());
	}

TVerdict CTestCalInterimApiRemoveMedia::doTestStepL()
	{
	#ifdef _DEBUG
	BuildCalendarEntitiesL();
	ReadEntriesL();
	SetMinMaxTimeL();
	CalendarOperationsL();
	#endif
	return TestStepResult();
	}

/**
* Performs various calendar operation like add, modify and delete calendar entries, find instances, fetch entries etc.. in the Calendar file
* Simulate the removal of media during the calendar operation by setting the File System to throw KErrNotReady.
* Validate that the KErrNotReady is thrown. Further reset the file system to normal state and repeat the calendar operations
*
*/
void CTestCalInterimApiRemoveMedia::CalendarOperationsL()
	{
	/* if the attachment is present in the entry, then do the basic operations like add, update/ delete based
	on the requirement from the user */
	TBool	attachmentExists = EFalse;
	GetBoolFromConfig(ConfigSection(), KRemoveMediaAttachmentExists, attachmentExists);
	if ( attachmentExists )
		{
		BasicCalendarOperationsWithMediaRemovedL();
		}
	else
		{
		LoadEntriesL();
		PopulateEntriesGuidListL();
		FetchEntriesForUpdateL();
		BasicCalendarOperationsInRemovableMediaL();
		DeleteEntriesInRemovableMediaL();
		}
	}

/**
* Load entries into the calendar file
*/
void CTestCalInterimApiRemoveMedia::LoadEntriesL()
	{
	const TUint KNullEntryCount = 0;
	TBool 	loadEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KRemoveMediaLoadEntriesFlag, loadEntriesFlag);

	if(loadEntriesFlag)
		{
		_LIT(KLoadEntries, "load Calendar Entries \n");
		INFO_PRINTF1(KLoadEntries);
		TInt entriesStored(KNullEntryCount);
		TRAPD(err, AddEntriesL(entriesStored));
		Validate(entriesStored > KNullEntryCount);
		Validate(err, KErrNone);
		}
	}

/**
* Perfom basic calendar operations like add, update, fetch calendar entries
* Add and remove categories in removable media
*/
void CTestCalInterimApiRemoveMedia::BasicCalendarOperationsInRemovableMediaL()
	{
	TBool basicOperationsFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KRemoveMediaBasicCalendarOperations, basicOperationsFlag);
	if(basicOperationsFlag)
		{
		BasicCalendarOperationsWithMediaRemovedL();
		BasicCalendarOperationsWithMediaReInsertedL();
		}
	}

/**
* Perfom basic calendar operations like add, update, fetch calendar entries
* Add and remove categories, with drive being removed in the middle of the operation
*/
void CTestCalInterimApiRemoveMedia::BasicCalendarOperationsWithMediaRemovedL()
	{
	const TUint KNullEntryCount = 0;
	const TUint KFsAccessCount = 1;
	TInt entriesCount(KNullEntryCount); // entryCount could be number of entries added, updated, or
										// number of entries where categrories were added or removed
	SetFsErrorCondition(KErrNotReady, KFsAccessCount);
	TRAPD(err, BasicCalendarOperationsL(entriesCount));
	SetFsErrorCondition(KErrNone);
	Validate(err, KErrNotReady);
	Validate(entriesCount == KNullEntryCount);
	ValidateEntryExistsL();
	}

/**
* Restore the device and
* Perfom basic calendar operations like add, update, fetch calendar entries
* Add and remove categories,
*/
void CTestCalInterimApiRemoveMedia::BasicCalendarOperationsWithMediaReInsertedL()
	{
	const TUint KNullEntryCount = 0;
	TInt entriesCount(KNullEntryCount);
	// Reset i.e. close and reopen calendar session and repeat the add operation
	ResetConditionsL();
	TRAPD(err, BasicCalendarOperationsL(entriesCount));
	Validate(entriesCount > KNullEntryCount);
	Validate(err, KErrNone);
	ValidateEntryExistsL();
	}

/**
* Retrieves the desired Calendar Operation from the ini file
*/
CTestCalInterimApiRemoveMedia::TCalendarOperations CTestCalInterimApiRemoveMedia::GetCalendarOperation(TPtrC aSpecificOperationFlag)
	{
	CTestCalInterimApiRemoveMedia::TCalendarOperations calOperation = CTestCalInterimApiRemoveMedia::EAddEntries;
	if(!aSpecificOperationFlag.Compare(KRemoveMediaAddEntriesFlag))
		{
		calOperation = CTestCalInterimApiRemoveMedia::EAddEntries;
		}
	else if(!aSpecificOperationFlag.Compare(KRemoveMediaUpdateEntriesFlag))
		{
		calOperation = CTestCalInterimApiRemoveMedia::EUpdateEntries;
		}
	else if(!aSpecificOperationFlag.Compare(KRemoveMediaAddCategories))
		{
		calOperation = CTestCalInterimApiRemoveMedia::EAddCategories;
		}
	else if(!aSpecificOperationFlag.Compare(KRemoveMediaRemoveCategories))
		{
		calOperation = CTestCalInterimApiRemoveMedia::ERemoveCategories;
		}
	else if(!aSpecificOperationFlag.Compare(KRemoveMediaFetchEntriesFlag))
		{
		calOperation = CTestCalInterimApiRemoveMedia::EFetchEntries;
		}
	return calOperation;
	}


/**
* Perform basic calendar operations like add, update, fetch calendar entries
* Add and remove categories
*/
void CTestCalInterimApiRemoveMedia::BasicCalendarOperationsL(TInt& aEntriesCount)
	{
	TPtrC specificOperationFlag;
	GetStringFromConfig(ConfigSection(), KRemoveMediaBasicCalendarOperation, specificOperationFlag);
	CTestCalInterimApiRemoveMedia::TCalendarOperations calOperation = GetCalendarOperation(specificOperationFlag);

	switch(calOperation)
		{
		case CTestCalInterimApiRemoveMedia::EAddEntries:
			 AddEntriesL(aEntriesCount);
			 break;

		case CTestCalInterimApiRemoveMedia::EUpdateEntries:
			 UpdateEntriesL(aEntriesCount);
			 break;

		case CTestCalInterimApiRemoveMedia::EAddCategories:
		 	 AddCategoriesL(aEntriesCount);
		 	 break;

		case CTestCalInterimApiRemoveMedia::ERemoveCategories:
			 RemoveCategoriesL(aEntriesCount);
		 	 break;

		case CTestCalInterimApiRemoveMedia::EFetchEntries:
			 FetchEntriesL(aEntriesCount);
			 break;
		}
	}

/**
* Populate the entry array on which UpdateL can be performed
*/
void CTestCalInterimApiRemoveMedia::FetchEntriesForUpdateL()
	{
	const TUint KGuidCount = iGuidList->Count();
	for (TInt i(0); i < KGuidCount; ++i)
		{
		TPtrC8 calEntryGuid = iGuidList->MdcaPoint(i);
		iEntryView->FetchL(calEntryGuid, iUpdateEntryArray);
		}
	}

/**
* Delete entries to the calendar file placed on Removable Media
*/
void CTestCalInterimApiRemoveMedia::DeleteEntriesInRemovableMediaL()
	{
	TBool deleteEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KRemoveMediaDeleteEntriesFlag, deleteEntriesFlag);

	if(deleteEntriesFlag)
		{
		DeleteCalendarOperationsWithMediaRemovedL();
		DeleteCalendarOperationsWithMediaReInsertedL();
		}
	}

/**
* Delete entries to the calendar file placed on Removable Media with drive being removed in the middle of operation
*/
void CTestCalInterimApiRemoveMedia::DeleteCalendarOperationsWithMediaRemovedL()
	{
	const TUint KFsAccessCount = 1;
	TInt entryCount = EntryCountL();
	SetFsErrorCondition(KErrNotReady, KFsAccessCount);
	TRAPD(err, DeleteCalendarEntriesL());
	SetFsErrorCondition(KErrNone);
	ResetConditionsL();
	Validate(err == KErrNotReady);
	Validate(EntryCountL() == entryCount);
	ValidateEntryExistsL();
	}

/**
* Restore the media and Delete entries to the calendar file placed on Removable Media
*/
void CTestCalInterimApiRemoveMedia::DeleteCalendarOperationsWithMediaReInsertedL()
	{
	const TUint KNullEntryCount = 0;
	TInt entryCount = EntryCountL();
	TRAPD(err, DeleteCalendarEntriesL());
	Validate(err == KErrNone);
	Validate(EntryCountL() == KNullEntryCount);
	ValidateEntryExistsL();
	}


/**
* Delete entries to the calendar file placed on Removable Media
*/
void CTestCalInterimApiRemoveMedia::DeleteCalendarEntriesL()
	{
	const TInt KErrEntryCount = -1;
	TInt entriesDeleted(KErrEntryCount);
	TPtrC deleteTypeFlag;
	GetStringFromConfig(ConfigSection(), KRemoveMediaDeleteTypeFlag, deleteTypeFlag);

	CTestCalInterimApiRemoveMedia::TDeleteType deleteType = GetDeleteType(deleteTypeFlag);

	switch(deleteType)
		{
		case CTestCalInterimApiRemoveMedia::EDeleteByUidList:
			 DeleteEntriesByUidListL();
			 break;

		case CTestCalInterimApiRemoveMedia::EDeleteByReference:
			 DeleteEntriesByReferenceL();
			 break;

		case CTestCalInterimApiRemoveMedia::EDeleteByLocalUids:
			 DeleteEntriesByArrayOfLocalUidsL(entriesDeleted);
			 Validate(entriesDeleted > KErrEntryCount);
			 break;

		case CTestCalInterimApiRemoveMedia::EDeleteByTimeRange:
			 DeleteEntriesByTimeRangeL();
			 break;

		case CTestCalInterimApiRemoveMedia::EDeleteByInstances:
			 DeleteInstancesL();
			 break;
		}
	}

/**
* Retrieves the desired Delete api to be used
*/
CTestCalInterimApiRemoveMedia::TDeleteType CTestCalInterimApiRemoveMedia::GetDeleteType(TPtrC aDeleteType)
	{
	CTestCalInterimApiRemoveMedia::TDeleteType deleteType = CTestCalInterimApiRemoveMedia::EDeleteByReference;

	if(aDeleteType.Compare(KRemoveMediaDeleteByReference))
		{
		deleteType = CTestCalInterimApiRemoveMedia::EDeleteByReference;
		}
	else if(aDeleteType.Compare(KRemoveMediaDeleteByUidList))
		{
		deleteType = CTestCalInterimApiRemoveMedia::EDeleteByUidList;
		}
	else if(aDeleteType.Compare(KRemoveMediaDeleteByArrayOfLocalUids))
		{
		deleteType = CTestCalInterimApiRemoveMedia::EDeleteByLocalUids;
		}
	else if(aDeleteType.Compare(KRemoveMediaDeleteByTimeRange))
		{
		deleteType = CTestCalInterimApiRemoveMedia::EDeleteByTimeRange;
		}
	else if(aDeleteType.Compare(KRemoveMediaDeleteInstances))
		{
		deleteType = CTestCalInterimApiRemoveMedia::EDeleteByInstances;
		}
	return deleteType;
	}


/**
* Add entries to the calendar file
*/
void CTestCalInterimApiRemoveMedia::AddEntriesL(TInt& aNumEntriesStored)
	{
	iEntryView->StoreL(iEntryArray, aNumEntriesStored);
	}

/**
* Update entries in the calendar file
*/
void CTestCalInterimApiRemoveMedia::UpdateEntriesL(TInt& aNumEntriesModified)
	{
	iEntryView->UpdateL(iUpdateEntryArray, aNumEntriesModified);
	}

/**
* Delete entries in the calendar file by specifying a list of UIDs to delete.
*/
void CTestCalInterimApiRemoveMedia::DeleteEntriesByUidListL()
	{
 	iEntryView->DeleteL(*iGuidList);
	}

/**
* Delete entries in the calendar file by specifying an array of Calendar Entries LocalUids,
*/
void CTestCalInterimApiRemoveMedia::DeleteEntriesByArrayOfLocalUidsL(TInt& aNumEntriesDeleted)
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TRAPD(err, iEntryView->GetIdsModifiedSinceDateL(minTime, localUids));
	Validate(err, KErrNone);
	iEntryView->DeleteL(localUids,aNumEntriesDeleted);
	CleanupStack::PopAndDestroy(); // localUids
	}

/**
* Delete entries in the calendar file by specifying calendar entries by Reference,
*/
void CTestCalInterimApiRemoveMedia::DeleteEntriesByReferenceL()
	{
	const TUint KParentIndex = 0;
	const TUint KNullEntryCount = 0;
	const TUint KGuidCount =  iGuidList->Count();
	for (TInt j(0); j < KGuidCount; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		TRAP_IGNORE(iEntryView->FetchL(iGuidList->MdcaPoint(j), calEntries)); // This operation may leave with KErrNotReady thereby Trap Ignore is used

		const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
		if(KCalendarEntriesSpecificToGuid != KNullEntryCount)
			{
			// Deleting a Parent entry deletes all the associated child entries
			// Hence bypasing all child entries
			CCalEntry* calEntry = calEntries[KParentIndex];
			iEntryView->DeleteL(*calEntry);
			}
		CleanupStack::PopAndDestroy(); //calEntries
		}
	}


/**
* Delete entries in the calendar file by specifying time Range,
*/
void CTestCalInterimApiRemoveMedia::DeleteEntriesByTimeRangeL()
	{
	CalCommon::TCalTimeRange	calTimeRange(iCalMinTime, iCalMaxTime);
	iEntryView->DeleteL(calTimeRange, CalCommon::EIncludeAll, *this);
	CActiveScheduler::Start();
	}

/**
* Delete calendar Instances in the calendar file by specifying a time Range,
*/
void CTestCalInterimApiRemoveMedia::DeleteInstancesL()
	{
	const TUint KZeroInstances = 0;
	const TUint KZeroIndex = 0;
	RPointerArray<CCalInstance>	instanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
	CalCommon::TCalTimeRange	calTimeRange(iCalMinTime, iCalMaxTime);

	TRAPD(err, iInstanceView->FindInstanceL(instanceArray, CalCommon::EIncludeAll, calTimeRange));
	Validate(err, KErrNone);
	if(instanceArray.Count() != KZeroInstances)
		{
		iInstanceView->DeleteL(instanceArray[KZeroIndex], CalCommon::EThisAndAll);
		instanceArray.Remove(KZeroIndex);
		}

	CleanupStack::PopAndDestroy(); // instanceArray
	}

/**
* Add category information to the Calendar Entries
@param aNumOfEntries - Num of entries which have categories
*/
void CTestCalInterimApiRemoveMedia::AddCategoriesL(TInt& aNumOfEntries)
	{
	const TUint KParentIndex = 0;
	RArray<TPtrC> categoryList;
	CleanupClosePushL(categoryList);
	TPtrC categories;
	GetStringFromConfig(ConfigSection(), KRemoveMediaCategories, categories);
	TokenizeStringL(categories, categoryList);

	const TUint KCategoryCount = categoryList.Count();
	for(TInt i(0); i < KCategoryCount; ++i)
		{
		TInt err = KErrNone;
		const TUint KGuidCount = iGuidList->Count();
		for(TInt j(0); j < KGuidCount; j++)
			{
			RPointerArray<CCalEntry> calEntries;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
			TRAP_IGNORE(iEntryView->FetchL(iGuidList->MdcaPoint(i), calEntries));

			const TInt numOfEntriesSpecificToGuid = calEntries.Count();
			CCalCategory* category=CCalCategory::NewL(categoryList[i]);
			CleanupStack::PushL(category);
			calEntries[KParentIndex]->AddCategoryL(category);
			CleanupStack::Pop(category);
			RPointerArray<CCalEntry> calEntriesCategory;
			CleanupClosePushL(calEntriesCategory);
			calEntriesCategory.AppendL(calEntries[KParentIndex]);
			TInt entriesUpdated(0);
			iEntryView->UpdateL(calEntriesCategory, entriesUpdated); // Adding Category Info to parent entries
			calEntriesCategory.Reset();

			for(TInt k(1); k < numOfEntriesSpecificToGuid; k++)
				{
				calEntries[k]->AddCategoryL(category);
				calEntriesCategory.AppendL(calEntries[k]);
				TInt entriesStored(0);
				iEntryView->StoreL(calEntriesCategory, entriesStored); // Adding Category Info to child entries
				calEntriesCategory.Reset();
				++aNumOfEntries;
				}


			if(err != KErrAlreadyExists || err != KErrNone)
				{
				ERR_PRINTF2(KRemoveMediaErrAddCategoryOperation,err);
				SetTestStepResult(EFail);
				}

			CleanupStack::PopAndDestroy(&calEntriesCategory);
			CleanupStack::PopAndDestroy(&calEntries);
			}
		}
	CleanupStack::PopAndDestroy(&categoryList);
	}

/**
* Removes category information in calendar entries
@param aNumOfEntries - Num of entries with category info being removed
*/
void CTestCalInterimApiRemoveMedia::RemoveCategoriesL(TInt& aNumOfEntries)
	{
	const TUint KParentIndex = 0;
	const TUint KGuidCount = iGuidList->Count();
	for(TInt i(0); i < KGuidCount; i++)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		RPointerArray<CCalEntry> calEntriesCategory;

		TRAP_IGNORE(iEntryView->FetchL(iGuidList->MdcaPoint(i), calEntries));
		const RPointerArray<CCalCategory>* categoryList = &(calEntries[KParentIndex]->CategoryListL());
   		const TUint KCategoryCount = categoryList->Count();
		for ( TInt i(0); i < KCategoryCount; i++ )
			{
			calEntriesCategory.AppendL(calEntries[KParentIndex]);
			calEntries[KParentIndex]->DeleteCategoryL(i);
			TInt entriesStored(0);
			iEntryView->UpdateL(calEntriesCategory, entriesStored); // Removing Category Info from Parent entry
			calEntriesCategory.Reset();
			++aNumOfEntries;
			const TInt numOfEntriesSpecificToGuid = calEntries.Count();

			for(TInt k(1); k < numOfEntriesSpecificToGuid; k++)
				{
				const RPointerArray<CCalCategory>* categoryList = &(calEntries[k]->CategoryListL());
   				const TUint KCategoryCount = categoryList->Count();
				calEntriesCategory.AppendL(calEntries[k]);
				calEntries[KParentIndex]->DeleteCategoryL(i);
				TInt entriesStored(0);
				iEntryView->StoreL(calEntriesCategory, entriesStored); // Removing Category Info from Child entries
				calEntriesCategory.Reset();
				++aNumOfEntries;
				}
			}
		CleanupStack::PopAndDestroy(&calEntries);
		}
	}

/**
* Fetch entries in the calendar file
@param aNumOfEntriesFetched - Number of entries fetched
*/
void CTestCalInterimApiRemoveMedia::FetchEntriesL(TInt& aNumOfEntriesFetched)
	{
	const TUint KGuidCount = iGuidList->Count();
	for (TInt i(0); i < KGuidCount; ++i)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		TPtrC8 calEntryGuid = iGuidList->MdcaPoint(i);
		iEntryView->FetchL(calEntryGuid, calEntries);
		aNumOfEntriesFetched = calEntries.Count();
		CleanupStack::PopAndDestroy(&calEntries);
		}
	}


/**
* Populates a CDescArray with guids of all the entries in the Calendar file
*/
void CTestCalInterimApiRemoveMedia::PopulateEntriesGuidListL()
	{
	const TFileName&	fileName=GetCalenderFileName();
	TInt count(0);

	TPtrC8	uid(iEntryIterator->FirstL());

	while (KNullDesC8()!=uid)
		{
		iGuidList->AppendL(uid);
		uid.Set(iEntryIterator->NextL());
		count++;
		}
	INFO_PRINTF2(KRemoveMediaInfoCalendarEntryCount,count);
	}

/**
* Release all allocated memory
* Close and reopen the Calendar Session
*/
void CTestCalInterimApiRemoveMedia::ResetConditionsL()
	{
	ReleaseResourcesL();
	CloseSessionL();
	OpenSessionL();
	OpenCalendarL();
	BuildCalendarEntitiesL();
	ReadEntriesL(); // This operations just updates the memmber iEntryArray with calendar entries specified in the data file
	                // In other words it doesnt attempt to store the entries in the calendar
	PopulateEntriesGuidListL();
	FetchEntriesForUpdateL();
	}

/**
* Set the minimum and maximum permissible calendar time
*/
void CTestCalInterimApiRemoveMedia::SetMinMaxTimeL()
	{
	iCalMinTime.SetTimeLocalL(TCalTime::MinTime());
	iCalMaxTime.SetTimeLocalL(TCalTime::MaxTime());
	}


/** Set the FileSystem to throw the specified error on specified number of Disk Access
@param aErrCode - Desired File Sytem Error Code
@param aCount - Number of disk Access
*/
void CTestCalInterimApiRemoveMedia::SetFsErrorCondition(TInt aErrCode, TInt aCount)
	{
	RFs& fs = GetFsSession();
	fs.SetErrorCondition(aErrCode, aCount);
	}

/**
* Returns the count of number of calendar entries present currently in the Calendar File
* @return - Calendar Entry Count
*/
TInt CTestCalInterimApiRemoveMedia::EntryCountL()
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
* Opens the Calendar file specified in the ini file
* and establishes session with the Calendar Server
*/
void CTestCalInterimApiRemoveMedia::OpenCalendarL()
	{
	TPtrC calendarFileName;
	TESTL(GetStringFromConfig(ConfigSection(), KRemoveMediaCalenderFile, calendarFileName));
	iCalenderFileName=calendarFileName;
	TRAPD(ret, iSession->OpenL(iCalenderFileName));
	if( ret!=KErrNone )
		{
		SetTestStepResult(EFail);
		}
	}

/**
* Reads Calendar entries specified in the Data file into an Array
*/
void CTestCalInterimApiRemoveMedia::ReadEntriesL()
	{
	TPtrC	dataFile;
	TESTL( GetStringFromConfig(ConfigSection(), KRemoveMediaDataFile, dataFile) );
	OpenAgendaDataFileL(dataFile);
	ReadAddEntriesL();
	}

/**
* Allocates the necessary Calendar entities
*/
void CTestCalInterimApiRemoveMedia::BuildCalendarEntitiesL()
	{
	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iInstanceView = CCalInstanceView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iEntryIterator=CCalIter::NewL(GetSession());
	iGuidList = new (ELeave) CDesC8ArrayFlat(4);
	}


/**
* Release all calendar resource
* When a media is removed in the middle of calendar operation, then all the calendar resources
* must be released
*/
void CTestCalInterimApiRemoveMedia::ReleaseResourcesL()
	{
	iEntryArray.ResetAndDestroy();
	iEntryArray.Close();
	delete  iEntryIterator;
	iEntryIterator = NULL;
	delete iGuidList;
	iGuidList = NULL;
	delete 	iEntryView;
	iEntryView =NULL;
	delete iInstanceView;
	iInstanceView = NULL;
	iUpdateEntryArray.ResetAndDestroy();
	iUpdateEntryArray.Close();
	}

/** Validate the boolean data and set the TestStep result to EFail, if required
@param aFlag - boolean data to be evaluated
*/
void CTestCalInterimApiRemoveMedia::Validate(TBool aFlag)
	{
	if(!aFlag)
		{
		_LIT(KRemoveMediaFailureMessage, "The evaluation failed");
		ERR_PRINTF1(KRemoveMediaFailureMessage);
		SetTestStepResult(EFail);
		}
	}

/** Validate and set the TestStep result to EFail, if required
@param aActualResult - actual result
@param aDesiredResult - desired result
*/
void CTestCalInterimApiRemoveMedia::Validate(TInt aActualResult, TInt aDesiredResult)
	{
	if(aActualResult != aDesiredResult)
		{
		_LIT(KRemoveMediaFailureMessage, "The actual result is %d while the desired result is %d ");
		ERR_PRINTF3(KRemoveMediaFailureMessage, aActualResult, aDesiredResult);
		SetTestStepResult(EFail);
		}
	}


/** Validate the presence of entries
*/
void CTestCalInterimApiRemoveMedia::ValidateEntryExistsL()
	{
	TBool validateEntryFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KRemoveMediaValidateEntryFlag, validateEntryFlag);

	if(!validateEntryFlag)
		{
		return;
		}

	TBool entryExpected(EFalse);
	GetBoolFromConfig(ConfigSection(), KRemoveMediaEntryExpected, entryExpected);

	TPtrC entryGuidString(NULL);
	GetStringFromConfig(ConfigSection(), KRemoveMediaEntryGuid, entryGuidString);

	TPtrC entryReccurenceIdString(NULL);
	GetStringFromConfig(ConfigSection(), KRemoveMediaEntryRecurrenceId, entryReccurenceIdString);

	CCalEntry* calEntry = NULL;
	RPointerArray<CCalEntry> calEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntryArray));

	HBufC8*	entryGuid = HBufC8::NewLC(entryGuidString.Length());
	entryGuid->Des().Copy(entryGuidString);
	iEntryView->FetchL(entryGuid->Des(), calEntryArray);
	CleanupStack::PopAndDestroy(entryGuid);


	if (entryReccurenceIdString == KNullDesC16()) // Parent Entry Check
		{
		calEntry = 	calEntryArray[0];
		ValidateEntry(calEntry, entryExpected);
		}
	else // Child Entry Check
		{
		TTime entryReccurenceIdInTTime;
		TCalTime entryReccurenceIdInCalTime;
		entryReccurenceIdInTTime.Set(entryReccurenceIdString);
		entryReccurenceIdInCalTime.SetTimeUtcL(entryReccurenceIdInTTime);

		for(TInt i = 1; i < calEntryArray.Count(); i++)
			{
			if(calEntryArray[i]->RecurrenceIdL().TimeUtcL() == entryReccurenceIdInCalTime.TimeUtcL())
				{
				ValidateEntry(calEntry, entryExpected);
				}
			}
		}
	}

/** Validate that the entry exists or not, As per expectations
@param aEntry - Entry to be checked
@param aEntryExpected - Entry expected or not
*/
void CTestCalInterimApiRemoveMedia::ValidateEntry(const CCalEntry* aEntry, TBool aEntryExpected)
	{
	if(aEntry)
		{
		if(!aEntryExpected)
			{
			ERR_PRINTF1(KRemoveMediaUnDesiredPresent);
			SetTestStepResult(EFail);
			}
		}
	else
		{
		if(aEntryExpected)
			{
			ERR_PRINTF1(KRemoveMediaDesiredEntryNotPresent);
			SetTestStepResult(EFail);
			}
		}
	}

void CTestCalInterimApiRemoveMedia::Completed(TInt aError)
  	{
	_LIT(KErrCompleted,					"Error Completed %d");
  	SetFsErrorCondition(KErrNone);
  	if ( aError != KErrNone )
  		{
  		ERR_PRINTF2(KErrCompleted, aError);
  		SetTestStepResult(EFail);
  		SetTestStepError(aError);
  		}
  	CActiveScheduler::Stop();
  	}
