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



#ifndef  __TEST_CALINTERIMAPI_FINDINSTANCES_ITERATOR_H__
#define  __TEST_CALINTERIMAPI_FINDINSTANCES_ITERATOR_H__

#include <e32std.h>
#include <calcommon.h>
#include <calsession.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <test/testexecuteclient.h>
#include "TestCalInterimApiReadData.h"

_LIT(KAddInstances,							"addentries");
_LIT(KUpdateInstances,						"updateentries");
_LIT(KDeleteInstances,						"deleteentries");
_LIT(KAllocTest,							"alloctest");
_LIT(KUpdateUsingStoreApi,					"updateusingstoreapi");
_LIT(KSourceTime, 							"Source Time : ");
_LIT(KDestinationTime, 						"Destination Time : ");


_LIT(KNumberOfIterators,					"numberofiterators");
_LIT(KIteratorAddInstancesFlag,				"addinstances%d");
_LIT(KIteratorDelInstancesFlag,				"deleteinstances%d");
_LIT(KIteratorUpdateInstancesFlag,			"updateinstances%d");
_LIT(KUpdateEntry,							"updateEntry");
_LIT(KIteratorDataFile,						"iteratordatafile");
_LIT(KGUIDForDeletion,						"guidfordelete%d");
_LIT(KStartTimeMultipleIterators,			"starttime%d");
_LIT(KEndTimeMultipleIterators,				"endtime%d");
_LIT(KSearchTextMultipleIterators,			"searchtext%d");
_LIT(KSearchBehaviourMultipleIterators,		"searchbehaviour%d");
_LIT(KFilterMultipleIterators,				"filter%d");
_LIT(KExpectedCountMultipleIterators,		"expectedcount%d");

_LIT(KEntryCount,							"The desired Instances count is %d, while the actual Instances count is %d \n");
_LIT(KIteratorInstancesMatchFailureMessage, "The expected and the actual instances count doesnt match\n");
_LIT(KIteratorNumber,						"The current iterator being used is %d \n\n\n");
_LIT(KSourceInstancesCount, 				"The number of instances found using the regular FindInstances Api is  %d \n");
_LIT(KDestinationListCount, 				"The number of instances found using the Iterator based FindInstances Api is  %d \n");
_LIT(KInfoTotalNumberOfInstances,			"Total number of instances returned by the iterator is %d");
_LIT(KInfoEntryGUID,						"GUID: %S");
_LIT(KInfoSummary,							"Summary: %S");
_LIT(KInfoDescription,						"Description: %S");
_LIT(KInfoLocation,							"Location: %S");
_LIT(KNewLine,								"\r\n");
_LIT(KInfoType,								"Entry Type: %D");
_LIT(KInfoCount,							"Checking count");
_LIT(KInfoInstanceFound,					"The instance in section: %S was found by FindInstanceL");
_LIT(KInfoNoOfInstancesFound, 				"No. of Instances found: %D");
_LIT(KInfoErroneousArgument, 				"Error returned while getting configuration for multiple iterators: %D");

_LIT(KErrInstanceNotFound,					"The instance in section: %S was NOT found by FindInstanceL");
_LIT(KErrNotInOrder,						"The instance is not chronologically after the previous instance");
_LIT(KErrDataFileMissing,					"Data file required for adding entries missing from ini file");
_LIT(KErrGUIDMissing,						"GUID required for updating entries missing from ini file");
_LIT(KErrUpdate,							"UpdateL() failed, error reported is %d");
_LIT(KErrInStoreEntry,						"StoreL() failed, error reported is %d");
_LIT(KErrNumberofIterators,					"Number of iterators is invalid = %d");
_LIT(KErrCountMismatch,						"Expected instance count = %d, returned instance count = %d");
_LIT(KErrIteratorCountMismatch,				"Expected instance count = %d, instances returned by iterator = %d");
_LIT(KErrDuringDeleteOperation,				"Delete operation failed with err code %d \n");
_LIT(KErrDuringUpdateOperation, 			"Update operation failed with err code %d \n");
_LIT(KErrDuringStoreOperation, 				"Store operation failed with err code %d \n");
_LIT(KErrDuringFetchOperation,			 	"Fetch operation failed with err code %d \n");
_LIT(KErrDuringFindInstancesOperation,		"Find Instances operation failed with err code %d \n");
_LIT(KErrIteratorBuildFailure,				"Error Occured while building the desired iterators, The relevant error code is %d \n");
_LIT(KIteratorSettingsMisMatch, 			"The iterator settings differ from the desired settings \n");
_LIT(KInstancesFoundUsingIterator, 			"Instances Found using the iterator is :");
_LIT(KInstancesFoundUsingFindInstancesApi, 	"Instances Found using the regular find instances api is :");


/* Search Behaviour Related Literals */
_LIT(KFoldedTextSearch, 					"EFoldedTextSearch");
_LIT(KExactTextSearch, 						"EExactTextSearch");
_LIT(KSearchEntryLocation, 					"ESearchEntryLocation");
_LIT(KSearchEntryDescription, 				"ESearchEntryDescription");
_LIT(KSearchAttendeeAddress, 				"ESearchAttendeeAddress");
_LIT(KSearchAttendeeSentByAddress, 			"ESearchAttendeeSentByAddress");
_LIT(KSearchAttendeeCommonName, 			"ESearchAttendeeCommonName");
_LIT(KSearchOrganizerAddress, 				"ESearchOrganizerAddress");
_LIT(KSearchOrganizerSentByAddress, 		"ESearchOrganizerSentByAddress");
_LIT(KSearchOrganizerCommonName, 			"ESearchOrganizerCommonName");
_LIT(KSearchAllTextFields, 					"ESearchAllTextFields");


/* OOM Related Literals */
_LIT(KOOMSuccessMessage,  					"OOM testing for iterator based find instance is done \n");
_LIT(KOOMFailureMessage, 					"OOM testing for iterator based find instance failed with error code  %d \n");

_LIT(KTestCalInterimApiFindInstancesIterator,"TestCalInterimApiFindInstancesIterator");

const TInt KMinimumIteratorCount = 1;
const TInt KWaitTime = 100000;
const TInt KUnexpectedInstancesCount = -1;
const TInt KDefaultFilter = 0;


/**
This class is used to provide the get, set and store interface for find Instances iterator settings
*/
class TCalIteratorSettings
	{

public:
	TCalIteratorSettings(void);
	void  TokenizeStringL(const TDesC& aString, RArray<TPtrC>& aList, TChar aSeparator = ',');
	void  SetExpectedInstancesCount(const TInt aExpectedInstancesCount);
    void  SetFilter(const TInt aFilter);
    void  SetFilterStartdate(const TPtrC aFilterStartdate);
    void  SetFilterEndDate(const TPtrC aFilterEndDate);
    void  SetSearchText(const TPtrC aSearchText);
    void  SetSearchBehaviour(const TPtrC aSearchBehaviour);
    TInt  GetExpectedInstancesCount();
    TInt  GetFilter();
    TPtrC GetFilterStartdate();
    TPtrC GetFilterEndDate();
    TPtrC GetSearchText();
    TPtrC GetSearchBehaviour();
	TPtrC GetSearchBehaviourString();
    CalCommon::TCalTimeRange GetFilterTimeRangeL();
	void  SetFilterStartAndEndTimeL(TCalTime& aFilterStartTime, TCalTime& aFilterEndTime);
	void  ConvertToCalTimeL(TPtrC& aTimeString, TCalTime& aCalTime);
	CalCommon::TCalSearchBehaviour GetSearchBehaviourL(const TPtrC& aSearchBehaviour);
	CCalInstanceView::TCalSearchParams TCalIteratorSettings::GetSearchParamsL();


private:
	TInt    iExpectedInstancesCount;
	TInt    iFilter;
	TPtrC	iFilterStartDate;
	TPtrC	iFilterEndDate;
	TPtrC	iSearchText;
	TPtrC   iSearchBehaviour;
	};

/**
 *Test step that does a FindInstanceL search using the iterators checks the instances
 *returned against the expected instances in ini file. The iterators used can be single or multiple
 */
class CTestCalInterimApiFindInstancesIterator : public CTestCalInterimApiReadData
	{
public:

	CTestCalInterimApiFindInstancesIterator(void);
	virtual ~CTestCalInterimApiFindInstancesIterator(void);
	virtual TVerdict	doTestStepL(void);
	virtual TVerdict 	doTestStepPreambleL(void);


	// Various Get/Set Functions
	void GetConfigurationDataL(void);
	void GetGeneralConfigData(void);
	void GetIteratorConfigDataL(void);
	void GetMultipleIteratorConfig(TInt& aIndex, TCalIteratorSettings& aCalIteratorSettings);
	CalCommon::TCalSearchBehaviour GetBehaviourL(const TPtrC& aSearchBehaviour);
	void GetCalTimeListFromInstancesListL(RPointerArray<CCalInstance>& aSourceInstanceList, RArray<TCalTime>& aDestinationCaltimeList);


private:
	// Various Iterator Operations
	CCalInstanceIterator* BuildIteratorL(TCalIteratorSettings& aCalIteratorSettings);
	void PerformIteratorOperationsL(void);
	void PerformIteratorOperationsUnderOOMConditionsL(CCalFindInstanceSettings& aFindInstanceSettings);
	void PerformIteratorOperationsPostCalendarOperationsL(void);
	void PopulateIteratorsL(void);
	void PopulateCalTimeListL(RArray<TCalTime>& aCalTimeList, TCalIteratorSettings& aCalIteratorSettings);
	void PopulateValidationInstancesListL(RPointerArray<CCalInstance>& aInstanceList, TInt aIndex);
	void IteratorOperationsL(RPointerArray<CCalInstance>& aInstanceArray, TInt aIndex);



	// Various Validations Functions
	TBool CompareL(RArray<TCalTime>& aSourceList, RArray<TCalTime>& aDestinationList);
	void ValidateInstancesCountL(void);
	void ValidateIteratorsL(void);
    void ValidateInstancesL(RPointerArray<CCalInstance>& aSourceInstanceList, const TInt aIteratorIndex);
    void ValidateIteratorSettingsL(CCalFindInstanceSettings* aFindInstanceSettings, TCalIteratorSettings& aTCalIteratorSettings);


	// Print functions
	void PrintAllInstancesL(RPointerArray<CCalInstance>& aInstanceArray);

	// Various Calendar Operations
	void AddModifyDeleteInstancesL(void);
	void AddInstancesL(void);
	void DeleteAllInstancesL(void);
	void PopulateEntriesGuidListL(void);
	void UpdateAllInstancesL(void);

	// Misc Operations
	void AllocTestL(void);

private:
	TInt   									iNumberOfIterators;
	TBool  									iUpdateUsingStoreApi;
	RArray<TCalIteratorSettings> 			iIteratorSettingsList;
	RPointerArray<CCalInstanceIterator>		iIteratorList;
	CCalInstanceView*						iInstanceView;
	CCalIter*								iEntryIterator;
	CDesC8ArrayFlat*						iGuidList;
	};

#endif
