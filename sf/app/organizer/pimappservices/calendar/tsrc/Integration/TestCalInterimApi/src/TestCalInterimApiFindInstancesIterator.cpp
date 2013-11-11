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
#include <calinstanceiterator.h>
#include "TestCalInterimApiFindInstancesIterator.h"

/**
 * Constructor initializes the data members. Also sets the Test Step Name.
 *
 */
CTestCalInterimApiFindInstancesIterator::CTestCalInterimApiFindInstancesIterator()
:	CTestCalInterimApiReadData(),
    iNumberOfIterators(KMinimumIteratorCount),
	iUpdateUsingStoreApi(EFalse)
	{
	SetTestStepName(KTestCalInterimApiFindInstancesIterator);
	}

/**
 * Destructor
 */
CTestCalInterimApiFindInstancesIterator::~CTestCalInterimApiFindInstancesIterator()
	{
	iIteratorSettingsList.Close();
	iIteratorList.ResetAndDestroy();
	delete iInstanceView;
	delete iEntryIterator;
	delete iGuidList;
	}

/**
 * Allocates necessary entities like Instance View, An Entry Iterator etc...
 */
TVerdict CTestCalInterimApiFindInstancesIterator::doTestStepPreambleL()
	{
	CTestCalInterimApiSuiteStepBase::doTestStepPreambleL();
	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iInstanceView=CCalInstanceView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	iEntryIterator=CCalIter::NewL(GetSession());
	iGuidList = new (ELeave) CDesC8ArrayFlat(4);
	return TestStepResult();
	}


TVerdict CTestCalInterimApiFindInstancesIterator::doTestStepL()
	{
	GetConfigurationDataL(); // Retreives data from the Ini file
	AllocTestL();
	TRAPD(ret, PerformIteratorOperationsL());
	SetTestStepError(ret);
	return TestStepResult();
	}

/**
 * Generates necessary iterators, Validates their Instances count
 * Perform iterator actions like NextL() and PreviousL()
 * carry out calendar operations like Add, Modify and delete of calendar Instances
 * Validates the iterators' Instances count
 * And Repeats iterator actions like NextL() and PreviousL() again.....
 */
void CTestCalInterimApiFindInstancesIterator::PerformIteratorOperationsL()
	{
	PopulateIteratorsL();
	ValidateInstancesCountL();
	ValidateIteratorsL();
	AddModifyDeleteInstancesL();
	}


/**
 * Gets the number of iterators to be generated
 * Builds the same and appends them to Iterator List
 */
void CTestCalInterimApiFindInstancesIterator::PopulateIteratorsL()
	{
	CCalInstanceIterator*  iterator = NULL;
	for(TInt i(0); i < iNumberOfIterators; ++i)
		{
		TRAPD(err,  iterator = BuildIteratorL(iIteratorSettingsList[i]));
		if(err != KErrNone)
			{
			ERR_PRINTF2(KErrIteratorBuildFailure, err);
			SetTestStepResult(EFail);
			User::Leave(err);
			}
		CleanupStack::PushL(iterator);
		iIteratorList.AppendL(iterator);
		CleanupStack::Pop(iterator);
		}
	}

/**
 * Builds iterator objects based on desired specifications
 @param		aCalIteratorSettings	Source of iterator settings like expected instances count etc...
 @return	a pointer to an iterator object
 */
CCalInstanceIterator* CTestCalInterimApiFindInstancesIterator::BuildIteratorL(TCalIteratorSettings& aCalIteratorSettings)
	{
	CalCommon::TCalTimeRange timeRange 				= aCalIteratorSettings.GetFilterTimeRangeL();
	CalCommon::TCalViewFilter filter 				= aCalIteratorSettings.GetFilter();
	CCalInstanceView::TCalSearchParams params 		= aCalIteratorSettings.GetSearchParamsL();

	CCalFindInstanceSettings* findInstanceSettings 	= CCalFindInstanceSettings::NewL(filter, timeRange);
	CleanupStack::PushL(findInstanceSettings);


	findInstanceSettings->SetFilter(filter);
	findInstanceSettings->SetTimeRange(timeRange);
	findInstanceSettings->SetTextSearchL(params);

	CCalInstanceIterator* iterator = NULL;
	TRAPD(err,  iterator = iInstanceView->FindInstanceL(*findInstanceSettings));
	if(err != KErrNone)
		{
		ERR_PRINTF2(KErrDuringFindInstancesOperation,err);
		SetTestStepResult(EFail);
		User::Leave(err);
		}

	CleanupStack::PopAndDestroy(findInstanceSettings); // findInstanceSettings
    return iterator;
	}



/**
 * Validates the desired instances count against actual instances count returned by the Iterator
 */
void CTestCalInterimApiFindInstancesIterator::ValidateInstancesCountL()
	{
	for(TInt i(0); i < iNumberOfIterators; ++i)
		{
		CCalInstanceIterator* iterator = iIteratorList[i];
		TInt instancesCount = iterator->Count();

		INFO_PRINTF3(KEntryCount, iIteratorSettingsList[i].GetExpectedInstancesCount(), instancesCount);

		if(iIteratorSettingsList[i].GetExpectedInstancesCount() != instancesCount)
			{
			ERR_PRINTF1(KIteratorInstancesMatchFailureMessage);
			SetTestStepResult(EFail);
			}
		}
	}

/**
 * Validates the Iterators
 * Perform iterator actions like NextL() and PreviousL()
 * Prints the Instances found using the Iterators
 * Validates the Instances found using the Iterators against the Instances found using the regular FindInstances Api
 */
void CTestCalInterimApiFindInstancesIterator::ValidateIteratorsL()
	{
	for(TInt i(0); i < iIteratorList.Count(); ++i)
		{
		INFO_PRINTF2(KIteratorNumber,i);
		RPointerArray<CCalInstance>	instanceArray;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
		IteratorOperationsL(instanceArray, i);
		ValidateInstancesL(instanceArray, i);
		CleanupStack::PopAndDestroy(); // instanceArray
		}
	}


/**
 * Performs various iterator operations NextL(), PreviousL()
 @param aInstanceArray - An array that will be populated with all the available instances using the specified iterator
 @param aIndex - Desired Iterator Index
 */
void CTestCalInterimApiFindInstancesIterator::IteratorOperationsL(RPointerArray<CCalInstance>& aInstanceArray, TInt aIndex)
	{
	RPointerArray<CCalInstance> instanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
	CCalInstanceIterator* iterator = iIteratorList[aIndex];
	CleanupStack::PushL(iterator);
	while(iterator->HasMore())
		{
		aInstanceArray.Append(iterator->NextL()); // Traversing the Iterator in the forward direction
		}

	CCalInstance* instance = NULL;
	do
		{
		instance = iterator->PreviousL(); // Traversing the Iterator in the reverse direction
		instanceArray.Append(instance);
		}while(instance != NULL);
	CleanupStack::Pop(iterator);
	CleanupStack::PopAndDestroy(); // instanceArray
	}


/**
 * Validates the Instances found using Iterator Find Instances Api against the regular Find Instances Api
 @param		aSourceInstanceList - an array containing the instances found as a result of Iterator Based find instance operation
 @param 	aIteratorIndex - Desired Iterator to be validated
 */
void CTestCalInterimApiFindInstancesIterator::ValidateInstancesL(RPointerArray<CCalInstance>& aSourceInstanceList, const TInt aIteratorIndex)
	{
	RPointerArray<CCalInstance> validationInstanceList;
	RArray<TCalTime> calTimeListUsingFindInstances;
	RArray<TCalTime> calTimeListUsingIterator;

	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &validationInstanceList));
	CleanupClosePushL(calTimeListUsingFindInstances);
	CleanupClosePushL(calTimeListUsingIterator);

    PopulateValidationInstancesListL(validationInstanceList, aIteratorIndex);
	GetCalTimeListFromInstancesListL(validationInstanceList, calTimeListUsingFindInstances);
	GetCalTimeListFromInstancesListL(aSourceInstanceList, calTimeListUsingIterator);

	if(!CompareL(calTimeListUsingFindInstances, calTimeListUsingIterator))
		{
		INFO_PRINTF1(KInstancesFoundUsingIterator);
		PrintAllInstancesL(aSourceInstanceList); // The Source and Destination Instances list is printed only in case of failure

		INFO_PRINTF1(KInstancesFoundUsingFindInstancesApi);
		PrintAllInstancesL(validationInstanceList);
		}

	CleanupStack::PopAndDestroy(3, &validationInstanceList); // calTimeListUsingIterator, calTimeListUsingFindInstances, validationInstanceList
	}

/**
 * Populates the Calendar Instances list supplied as parameter with instances found using the regular FindInstances Api
 * Used for validation purpose
 @param 	aInstanceList - An array that will be populated with all the available instances using the regular FindInstances Api
 @param 	aIteratorIndex - Desired Iterator to be validated
 */
void CTestCalInterimApiFindInstancesIterator::PopulateValidationInstancesListL(RPointerArray<CCalInstance>& aInstanceList,TInt aIndex)
	{
    TCalIteratorSettings iteratorSettings = iIteratorSettingsList[aIndex];
	CalCommon::TCalTimeRange timeRange = iteratorSettings.GetFilterTimeRangeL();
	CalCommon::TCalViewFilter filter = 	iteratorSettings.GetFilter();
	CCalInstanceView::TCalSearchParams params = iteratorSettings.GetSearchParamsL();

	if(iteratorSettings.GetSearchText() != KNullDesC)
		{
		iInstanceView->FindInstanceL(aInstanceList, filter, timeRange, params);
		}
	else
		{
		iInstanceView->FindInstanceL(aInstanceList, filter, timeRange);
		}
	}


/**
 * Populates an TCalTime array based on the Calendar Instances
 @param	aSourceInstanceList - Source Array which contains Calendar Instances
 @param	aDestinationCaltimeList - Destination array with start time of calendar instances in TCalTime format
 */
void CTestCalInterimApiFindInstancesIterator::GetCalTimeListFromInstancesListL(RPointerArray<CCalInstance>& aSourceInstanceList,
																					RArray<TCalTime>& aDestinationCaltimeList)
	{
	const TInt instancesCount = aSourceInstanceList.Count();
	for(TInt i(0); i < instancesCount; i++)
		{
		aDestinationCaltimeList.AppendL(aSourceInstanceList[i]->StartTimeL());
		}
	}

/**
 * Compares the calendar instances time returned by Find Instances operations against data specified in the ini file
 @param	aSourceList - Source Array which start time of Calendar Instances returned from the Iterator based FindInstances Operation
 @param	aDestinationCaltimeList - contains time values which start time of Calendar Instances returned from the regular FindInstances Operation
 */
TBool CTestCalInterimApiFindInstancesIterator::CompareL(RArray<TCalTime>& aSourceList, RArray<TCalTime>& aDestinationList)
	{
	const TInt sourceListCount = aSourceList.Count();
	const TInt destinationListCount = aDestinationList.Count();

	INFO_PRINTF2(KSourceInstancesCount, sourceListCount);

	INFO_PRINTF2(KDestinationListCount, destinationListCount);


	if(sourceListCount != destinationListCount)
		{
		SetTestStepResult(EFail);
		return EFalse;
		}
	else
		{
		for(TInt i(0); i < sourceListCount; ++i)
			{
			TTime sourceTime = aSourceList[i].TimeUtcL();
			TTime destinationTime = aDestinationList[i].TimeUtcL();
			if(sourceTime != destinationTime)
				{
				INFO_PRINTF1(KSourceTime);
				PrintTimeL(sourceTime);
				INFO_PRINTF1(KDestinationTime);
				PrintTimeL(destinationTime);
				SetTestStepResult(EFail);
				return EFalse;
				}
			}
		}
	return ETrue;
	}



/**
  *Retrieves overall iterator tests configuration info from ini file.
  */
void CTestCalInterimApiFindInstancesIterator::GetConfigurationDataL()
	{
	GetGeneralConfigData();
	GetIteratorConfigDataL();
	}

/**
  *Retrieves general iterator tests configuration info from ini file.
  */
void CTestCalInterimApiFindInstancesIterator::GetGeneralConfigData()
	{
	GetIntFromConfig(ConfigSection(),  KNumberOfIterators,   iNumberOfIterators);
	GetBoolFromConfig(ConfigSection(), KUpdateUsingStoreApi, iUpdateUsingStoreApi);
	}

/**
  *Retrieves iterator configuration specific info from ini file.
  */
void CTestCalInterimApiFindInstancesIterator::GetIteratorConfigDataL()
	{
	for (TInt index = 1; index <= iNumberOfIterators; ++index)
		{
		TCalIteratorSettings iteratorSettings;
		GetMultipleIteratorConfig(index, iteratorSettings);
		iIteratorSettingsList.AppendL(iteratorSettings);
		}
	}

/**
  *Retrieves iterator specific search configuration from ini file.
  @param aIndex Integer indicating the iterator number.
  @param aCalIteratorSettings - contains the desired ini settings
  */
void CTestCalInterimApiFindInstancesIterator::GetMultipleIteratorConfig(TInt& aIndex, TCalIteratorSettings& aCalIteratorSettings)
	{
	// Descriptor that contains the concatenated configuration item
	TBuf<KMaxTestExecuteCommandLength>	iteratorConfigItem;

	// Descriptor set to an initial value of KNullDesC.
	// Used to initialize the member descriptors at the beginning of each iteration.
	TPtrC	tempDescriptor(KNullDesC);

	// Populate expected count for Iterator
	iteratorConfigItem.Format(KExpectedCountMultipleIterators(), aIndex);
	TInt expectedInstancesCount = KUnexpectedInstancesCount;
	GetIntFromConfig   (ConfigSection(), iteratorConfigItem, expectedInstancesCount);
	aCalIteratorSettings.SetExpectedInstancesCount(expectedInstancesCount);

	// Populate iStartDate and iEndDate for Iterator
	tempDescriptor.Set(KNullDesC);
	iteratorConfigItem.Format(KStartTimeMultipleIterators(), aIndex);
	GetStringFromConfig(ConfigSection(), iteratorConfigItem, tempDescriptor);
	aCalIteratorSettings.SetFilterStartdate(tempDescriptor);

	tempDescriptor.Set(KNullDesC);
	iteratorConfigItem.Format(KEndTimeMultipleIterators(), aIndex);
	GetStringFromConfig(ConfigSection(), iteratorConfigItem, tempDescriptor);
	aCalIteratorSettings.SetFilterEndDate(tempDescriptor);

	// Populate filter for Iterator
	TInt filter = KDefaultFilter;
	iteratorConfigItem.Format(KFilterMultipleIterators(), aIndex);
	GetIntFromConfig(ConfigSection(), iteratorConfigItem, filter);
	aCalIteratorSettings.SetFilter(filter);

	// Populate search text for Iterator
	tempDescriptor.Set(KNullDesC);
	iteratorConfigItem.Format(KSearchTextMultipleIterators(), aIndex);
	GetStringFromConfig(ConfigSection(), iteratorConfigItem, tempDescriptor);
	aCalIteratorSettings.SetSearchText(tempDescriptor);

	// Populate search behaviour for Iterator
	tempDescriptor.Set(KNullDesC);
	iteratorConfigItem.Format(KSearchBehaviourMultipleIterators(), aIndex);
	GetStringFromConfig(ConfigSection(), iteratorConfigItem, tempDescriptor);
	aCalIteratorSettings.SetSearchBehaviour(tempDescriptor);

	}

/**
 * Performs various calendar operations like Add, modify and delete of calendar Instances
 */
void CTestCalInterimApiFindInstancesIterator::AddModifyDeleteInstancesL()
	{
	AddInstancesL();
	UpdateAllInstancesL();
	DeleteAllInstancesL();
	}

/** Adds new entries specified in the data file. These entries should generates instances
  * in the iterator filter range
  * Since the entries are added after iterator has created the list of
  * instance ids, the new entries would not be returned by the iterator.
  */
void CTestCalInterimApiFindInstancesIterator::AddInstancesL()
	{
	TBool	addInstances = EFalse;
	GetBoolFromConfig(ConfigSection(), KAddInstances, addInstances);

	if(addInstances == EFalse)
		{
		return;
		}

	TPtrC	dataFile;
	if (GetStringFromConfig(ConfigSection(), KIteratorDataFile, dataFile))
		{
		OpenAgendaDataFileL(dataFile); // Open a Text data file specified in the ini
		ReadAddEntriesL(); // And populate the same into calendar database
		}
	else
		{
		ERR_PRINTF1(KErrDataFileMissing);
		User::Leave(KErrArgument);
		}

	ValidateInstancesCountL();
	PerformIteratorOperationsPostCalendarOperationsL();
	}


/**
* Update all calendar instances
* Any further iterator operations should result in KErrNotFound
* Reusing the existing updateAllInstances Teststep
*/
void CTestCalInterimApiFindInstancesIterator::UpdateAllInstancesL()
	{
	TBool	updateInstances = EFalse;
	GetBoolFromConfig(ConfigSection(), KUpdateInstances, updateInstances);

	if(updateInstances == EFalse)
		{
		return;
		}

	PopulateEntriesGuidListL();

	const TUint KGuidCount =  iGuidList->Count();
	for (TInt j(0); j < KGuidCount; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		iEntryView->FetchL(iGuidList->MdcaPoint(j), calEntries);

		const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
		if(KCalendarEntriesSpecificToGuid != 0)
			{
			RPointerArray<CCalEntry> updateArray;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &updateArray));
			TInt success = 0;
			TInt err = KErrNone;
			CCalEntry* calEntry = calEntries[0];
			CleanupStack::PushL(calEntry);
			updateArray.Append(calEntry);
			calEntry->SetLastModifiedDateL();

			TBool updateUsingStoreApi = EFalse;
			GetBoolFromConfig(ConfigSection(), KUpdateUsingStoreApi, updateUsingStoreApi);

			if(updateUsingStoreApi)
				{
				TRAP(err, iEntryView->StoreL(updateArray, success)); // Parent Entry update using StoreL Api
				}
			else
				{
				TRAP(err, iEntryView->UpdateL(updateArray, success)); // Parent Entry update using UpdateL Api
				}

			if(err != KErrNone)
				{
				ERR_PRINTF2(KErrDuringUpdateOperation,err);
				SetTestStepResult(EFail);
				}

			if(KCalendarEntriesSpecificToGuid > 1)
				{
				for(TInt i(1); i < (KCalendarEntriesSpecificToGuid); ++i)
					{
					updateArray.Reset();
					calEntry = calEntries[i];
					updateArray.Append(calEntry);
					TRAP(err, iEntryView->StoreL(updateArray, success)); // Child Entries update
					if(err != KErrNone)
						{
						ERR_PRINTF2(KErrDuringUpdateOperation,err);
						SetTestStepResult(EFail);
						}
					}
				}
			CleanupStack::Pop(2, &updateArray); // calEntry, updateArray
			updateArray.Reset();
			}
		CleanupStack::PopAndDestroy(); // calEntries
		}
	}


/**
* Delete all calendar Instances
*/
void CTestCalInterimApiFindInstancesIterator::DeleteAllInstancesL()
	{
	TBool	deleteInstances = EFalse;
	GetBoolFromConfig(ConfigSection(), KDeleteInstances, deleteInstances);

	if(deleteInstances == EFalse)
		{
		return;
		}

	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TRAP_IGNORE(iEntryView->GetIdsModifiedSinceDateL(minTime, localUids));
	TInt numEntriesDeleted(0);
	TRAPD(err, iEntryView->DeleteL(localUids,numEntriesDeleted));
	if(err != KErrNone)
		{
		INFO_PRINTF2(KErrDuringDeleteOperation, err);
		SetTestStepResult(EFail);
		}

	CleanupStack::PopAndDestroy(); // localUids
	ValidateInstancesCountL();
	PerformIteratorOperationsPostCalendarOperationsL();
	}

/**
 * Trying to access the iterators after add/modify/delete od calendar instances
 * Validate that the iterators doesnt panic
 */
void CTestCalInterimApiFindInstancesIterator::PerformIteratorOperationsPostCalendarOperationsL()
	{
	for(TInt i(0); i < iIteratorList.Count(); i++)
		{
		INFO_PRINTF2(KIteratorNumber,i);
		RPointerArray<CCalInstance>	instanceArray;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
		IteratorOperationsL(instanceArray, i);
		CleanupStack::PopAndDestroy(); // instanceArray
		}
	}

/**
 * Prints GUID, Type and Time of all instances in the array
 @param	aInstanceArray	reference to an array of CCalInstance
 */
void CTestCalInterimApiFindInstancesIterator::PrintAllInstancesL(RPointerArray<CCalInstance>& aInstanceArray)
	{
	INFO_PRINTF2(KInfoNoOfInstancesFound, aInstanceArray.Count());
	TTime previousInstanceTime(Time::NullTTime());

	for(TInt count = 0; count < aInstanceArray.Count(); ++count)
		{
		// Print GUID
		HBufC* foundGUID = HBufC::NewLC(aInstanceArray[count]->Entry().UidL().Length());
		foundGUID->Des().Copy(aInstanceArray[count]->Entry().UidL());
		INFO_PRINTF1(KNewLine);
		INFO_PRINTF2(KInfoEntryGUID, foundGUID);
		CleanupStack::PopAndDestroy(foundGUID);

		// Print Entry Type
		INFO_PRINTF2(KInfoType, (TInt)aInstanceArray[count]->Entry().EntryTypeL());

		// Print Instance Time
		PrintTimeL(aInstanceArray[count]->Time().TimeLocalL());

		// Print Instance End Time
		PrintTimeL(aInstanceArray[count]->EndTimeL().TimeLocalL());

		// Check chronological order
		if(aInstanceArray[count]->Time().TimeUtcL() < previousInstanceTime)
			{
			ERR_PRINTF1(KErrNotInOrder);
			SetTestStepResult(EFail);
			}
		previousInstanceTime = aInstanceArray[count]->Time().TimeUtcL();

		// Print Summary
		INFO_PRINTF2(KInfoSummary, &(aInstanceArray[count]->Entry().SummaryL()));

		// Print Description
		INFO_PRINTF2(KInfoDescription, &(aInstanceArray[count]->Entry().DescriptionL()));

		// Print Location
		INFO_PRINTF2(KInfoLocation, &(aInstanceArray[count]->Entry().LocationL()));

		INFO_PRINTF1(KNewLine);
		}
	}

/**
* Run Iterator based Find Instances Api in a OOM loop
*/
void CTestCalInterimApiFindInstancesIterator::AllocTestL()
	{
	TBool	allocTest = EFalse;
	TInt	tryCount = 1;
	TInt	err = KErrNone;

	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);

	if(allocTest == EFalse)
		{
		return;
		}

	TCalIteratorSettings iteratorSettings = iIteratorSettingsList[0];

	CCalInstanceIterator* iterator = NULL;
	CalCommon::TCalTimeRange timeRange = iteratorSettings.GetFilterTimeRangeL();
	CalCommon::TCalViewFilter filter = 	iteratorSettings.GetFilter();
	CCalInstanceView::TCalSearchParams params = iteratorSettings.GetSearchParamsL();
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(filter, timeRange);
	CleanupStack::PushL(findInstanceSettings);
	findInstanceSettings->SetTextSearchL(params);

    // OOM Loop
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
		TRAP(err, PerformIteratorOperationsUnderOOMConditionsL(*findInstanceSettings));
		if ( err==KErrNone )
			{
			__UHEAP_RESET;
			INFO_PRINTF1(KOOMSuccessMessage);
			break;
			}
		if ( err != KErrNoMemory )
			{
			ERR_PRINTF2(KOOMFailureMessage, err);
			SetTestStepResult(EFail);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);
		tryCount++;
		}

	delete iterator;
	iterator = NULL;
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
    CleanupStack::PopAndDestroy();// findInstanceSettings;
   	}

/**
* Performs Various iterator operations under OOM conditions
* Every time it should leave with KErrNoMemory, in case of OOM situation
*/
void CTestCalInterimApiFindInstancesIterator::PerformIteratorOperationsUnderOOMConditionsL(CCalFindInstanceSettings& aFindInstanceSettings)
	{
	CCalInstanceIterator* iterator = NULL;
	iterator = iInstanceView->FindInstanceL(aFindInstanceSettings);
	CleanupStack::PushL(iterator);
		
	TInt count = 0;
	while( iterator->HasMore() )
		{
		CCalInstance* nextInstance = iterator->NextL(); // Traversing through forward direction
		CleanupStack::PushL(nextInstance);
		
		if( count > 0 )
			{
			CCalInstance* preInstance = iterator->PreviousL(); // Traversing through backward direction
			CleanupStack::PushL(preInstance);
			TTime preInstanceDateTime = preInstance->Time().TimeLocalL();
			PrintTimeL(preInstanceDateTime);	
			CleanupStack::PopAndDestroy(preInstance);
			
			CleanupStack::PopAndDestroy(nextInstance);
			nextInstance = iterator->NextL();
			CleanupStack::PushL(nextInstance);
			}
			
		TTime nextInstanceDateTime = nextInstance->Time().TimeLocalL();
		PrintTimeL(nextInstanceDateTime);
		CleanupStack::PopAndDestroy(nextInstance);
		count++;
		}
		
	CleanupStack::PopAndDestroy();	// iterator
	}

/**
* Populates a CDescArray with guids of all the entries in the Calendar file
*/
void CTestCalInterimApiFindInstancesIterator::PopulateEntriesGuidListL()
	{
	TPtrC8	uid(iEntryIterator->FirstL());

	while(KNullDesC8()!=uid)
		{
		iGuidList->AppendL(uid);
		uid.Set(iEntryIterator->NextL());
		}
	}

/**
 * Constructor initializes the TCalIteratorSettings class data members.
 *
 */
TCalIteratorSettings::TCalIteratorSettings()
:	iExpectedInstancesCount(KUnexpectedInstancesCount),
	iFilter(KDefaultFilter),
	iFilterStartDate(KNullDesC),
	iFilterEndDate(KNullDesC),
	iSearchText(KNullDesC),
	iSearchBehaviour(KNullDesC)
	{
	}

void  TCalIteratorSettings::SetExpectedInstancesCount(const TInt aExpectedInstancesCount)
	{
	iExpectedInstancesCount = 	aExpectedInstancesCount;
	}

void  TCalIteratorSettings::SetFilter(const TInt aFilter)
	{
	iFilter = aFilter;
	}

void  TCalIteratorSettings::SetFilterStartdate(const TPtrC aFilterStartdate)
	{
	iFilterStartDate.Set(aFilterStartdate);
	}

void  TCalIteratorSettings::SetFilterEndDate(const TPtrC aFilterEndDate)
	{
	iFilterEndDate.Set(aFilterEndDate);
	}

void  TCalIteratorSettings::SetSearchText(const TPtrC aSearchText)
	{
	iSearchText.Set(aSearchText);
	}

void  TCalIteratorSettings::SetSearchBehaviour(const TPtrC aSearchBehaviour)
	{
	iSearchBehaviour.Set(aSearchBehaviour);
	}

TInt  TCalIteratorSettings::GetExpectedInstancesCount()
	{
	return	iExpectedInstancesCount;
	}

TInt  TCalIteratorSettings::GetFilter()
	{
	return iFilter;
	}

TPtrC TCalIteratorSettings::GetFilterStartdate()
	{
	return iFilterStartDate;
	}

TPtrC TCalIteratorSettings::GetFilterEndDate()
	{
	return	iFilterEndDate;
	}

TPtrC TCalIteratorSettings::GetSearchText()
	{
	return	iSearchText;
	}

TPtrC TCalIteratorSettings::GetSearchBehaviourString()
	{
	return	iSearchBehaviour;
	}


/**
 * Wrapper function to get search specific data like search string and behaviour
 @param		aCalIteratorSettings	source Iterator settings like expected instances count etc...
 @return	an iterator search parameter
 */

CCalInstanceView::TCalSearchParams TCalIteratorSettings::GetSearchParamsL()
	{
	CalCommon::TCalSearchBehaviour behaviour = GetSearchBehaviourL(GetSearchBehaviourString());
	CCalInstanceView::TCalSearchParams params(GetSearchText(), behaviour);
	return params;
	}


CalCommon::TCalTimeRange TCalIteratorSettings::GetFilterTimeRangeL()
	{
	TCalTime filterStartDate;
	TCalTime filterEndDate;
	SetFilterStartAndEndTimeL(filterStartDate, filterEndDate);
	CalCommon::TCalTimeRange timeRange(filterStartDate, filterEndDate);
	return timeRange;
	}

void TCalIteratorSettings::SetFilterStartAndEndTimeL(TCalTime& aFilterStartTime, TCalTime& aFilterEndTime)
	{
	aFilterStartTime.SetTimeLocalL(TCalTime::MinTime());
	aFilterEndTime.SetTimeLocalL(TCalTime::MaxTime());

	TPtrC  filterStartDateString(GetFilterStartdate());
	TPtrC  filterEndDateString(GetFilterEndDate());

    ConvertToCalTimeL(filterStartDateString, aFilterStartTime);
    ConvertToCalTimeL(filterEndDateString, aFilterEndTime);
	}

/**
 * Converts search behaviour string to CalCommon::TCalSearchBehaviour type object.
 @return	TCalSearchBehaviour object that represents the search behaviour
 @param		aSearchBehaviour	string describing the search behaviour
 */
CalCommon::TCalSearchBehaviour TCalIteratorSettings::GetSearchBehaviourL(const TPtrC& aSearchBehaviour)
	{
	TUint behaviour = 0x0;

	RArray<TPtrC> list;
	CleanupClosePushL(list);
	TokenizeStringL(aSearchBehaviour, list);

	for(TInt i = 0; i < list.Count(); ++i)
		{
		if(list[i] == KFoldedTextSearch)
			{
			behaviour 	|= CalCommon::EFoldedTextSearch;
			}
		else if(list[i] == KExactTextSearch)
			{
			behaviour 	|= CalCommon::EExactTextSearch;
			}
		else if(list[i] == KSearchEntryLocation)
			{
			behaviour 	|= CalCommon::ESearchEntryLocation;
			}
		else if(list[i] == KSearchEntryDescription)
			{
			behaviour 	|= CalCommon::ESearchEntryDescription;
			}
		else if(list[i] == KSearchAttendeeAddress)
			{
			behaviour 	|= CalCommon::ESearchAttendeeAddress;
			}
		else if(list[i] == KSearchAttendeeSentByAddress)
			{
			behaviour 	|= CalCommon::ESearchAttendeeSentByAddress;
			}
		else if(list[i] == KSearchAttendeeCommonName)
			{
			behaviour 	|= CalCommon::ESearchAttendeeCommonName;
			}
		else if(list[i] == KSearchOrganizerAddress)
			{
			behaviour 	|= CalCommon::ESearchOrganizerAddress;
			}
		else if(list[i] == KSearchOrganizerSentByAddress)
			{
			behaviour 	|= CalCommon::ESearchOrganizerSentByAddress;
			}
		else if(list[i] == KSearchOrganizerCommonName)
			{
			behaviour 	|= CalCommon::ESearchOrganizerCommonName;
			}
		else if(list[i] == KSearchAllTextFields)
			{
			behaviour 	|= CalCommon::ESearchAllTextFields;
			}
		}

	CleanupStack::PopAndDestroy(); // list
	return (CalCommon::TCalSearchBehaviour)behaviour;
	}


/**
  * Parses a comma separated string and constructs a list out of the values
  */
void TCalIteratorSettings::TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator)
	{
	TLex lexer(aString);

	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();

		while(!lexer.Eos() && lexer.Get() != aSeparator)
			{
			}

		if(!lexer.Eos())
			{
			lexer.UnGet(); // Do not include trailing ','
			}
		aList.AppendL(lexer.MarkedToken());
		lexer.Inc();
		}
	}

/**
 * Converts search behaviour string to CalCommon::TCalSearchBehaviour type object.
 @param		aTimeString	source time string
 @param		aCalTime	destination tcaltime object to be updated
 */
void TCalIteratorSettings::ConvertToCalTimeL(TPtrC& aTimeString, TCalTime& aCalTime)
	{
	if(aTimeString != KNullDesC)
		{
		TTime time(aTimeString);
		aCalTime.SetTimeUtcL(time);
		}
	}
