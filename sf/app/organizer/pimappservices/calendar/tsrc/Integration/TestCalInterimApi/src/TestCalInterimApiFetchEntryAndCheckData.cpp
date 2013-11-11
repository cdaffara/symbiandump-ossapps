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

#include "TestCalInterimApiFetchEntryAndCheckData.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>
#include <calcategory.h>
#include <caluser.h>

_LIT(KGUid,							"guid");
_LIT(KExpectedCount,				"expectedcount");
_LIT(KSummary,						"summary");
_LIT(KDescription,					"description");
_LIT(KRecurrenceId,					"recurrenceid");
_LIT(KTimeMode,						"timemode");
_LIT(KCategories,					"categories");
_LIT(KSoakTest, 					"soaktest");
_LIT(KSoakTestCount,				"soaktestcount");
_LIT(KGEOLat,						"GEOLat");
_LIT(KGEOLong,						"GEOLong");
_LIT(KTrans,						"trans");
_LIT(KLocalId,						"localid");
_LIT(KPriority,						"priority");

_LIT(KInfoEntryFetched,				"Entry Fetched is %d, Expected %d");
_LIT(KInfoCategoryExists,			"Category : %S found in the entry");

_LIT(KExpectedAndFoundSummary,		"Expected Summary:%S, Retrieved Summary:%S");
_LIT(KExpectedAndFoundGEO,			"Expected GEO Lat:%f, Retrieved Lat:%f, Expected GEO Long:%f, Retrieved Long:%f");
_LIT(KExpectedAndFoundDescription,	"Expected Description:%S, Retrieved Description:%S");
_LIT(KExpectedAndFoundStatus,		"Expected Busy Status:%d, Retrieved Busy Status:%d");
_LIT(KExpectedAndFoundLocation,		"Expected Location:%S, Retrieved Location:%S");
_LIT(KExpectedAndFoundLocalId,		"Expected LocalId:%d, Retrieved LocalId:%d");
_LIT(KExpectedAndFoundPriority,		"Expected Priority:%d, Retrieved Priority:%d");


_LIT(KErrCategoryNotExists,			"Category : %S NOT found in the entry");
_LIT(KErrExpectedCount,				"EntryCount %d. Expected %d");

_LIT(KWarnNoMatchInRecurrenceId,	"Recurrence Id's are not matching");

CTestCalInterimApiFetchEntryAndCheckData::CTestCalInterimApiFetchEntryAndCheckData()
:	CTestCalInterimApiSuiteStepBase()

/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiFetchEntryAndCheckData);
	}

CTestCalInterimApiFetchEntryAndCheckData::~CTestCalInterimApiFetchEntryAndCheckData()
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
TVerdict CTestCalInterimApiFetchEntryAndCheckData::doTestStepL()
	{
	TEntryProperties	entryProperties;
	TPtrC	guid;
	TPtrC	timeMode;
	TInt	expectedCount;
	TInt	soakTestCount;
	TBool	soakTest = EFalse;

	TESTL( GetStringFromConfig(ConfigSection(), KGUid, guid) );
	TESTL( GetIntFromConfig(ConfigSection(), KExpectedCount, expectedCount) );
	// Dont mind if these are not given in the ini file.
	GetStringFromConfig(ConfigSection(), KSummary, entryProperties.iSummary);
	GetStringFromConfig(ConfigSection(), KDescription, entryProperties.iDescription);
	GetStringFromConfig(ConfigSection(), KRecurrenceId, entryProperties.iRecurrenceId);
	GetStringFromConfig(ConfigSection(), KCategories, entryProperties.iCategories);
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	GetStringFromConfig(ConfigSection(), KGEOLat, entryProperties.iGeoLat);
	GetStringFromConfig(ConfigSection(), KGEOLong, entryProperties.iGeoLong);
	entryProperties.iTrans = KValueIntNotIni;
	GetIntFromConfig(ConfigSection(), KTrans, entryProperties.iTrans);
	GetBoolFromConfig(ConfigSection(), KSoakTest, soakTest);
	GetIntFromConfig(ConfigSection(), KSoakTestCount, soakTestCount);
	entryProperties.iHasLocalId = GetIntFromConfig(ConfigSection(), KLocalId, entryProperties.iLocalId);
	entryProperties.iHasPriority = GetIntFromConfig(ConfigSection(), KPriority, entryProperties.iPriority);

	iCalEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	if ( soakTest )
		{
		SoakTestsForFetchEntryL(guid, soakTest, soakTestCount);
		}
	else
		{
		FetchAndCheckDataL(guid, expectedCount, entryProperties, timeMode);
		}

	return TestStepResult();
	}


/** Fetch the entry and check the properties that are set.
@param	aGuid Data for fetching the entry.
@param	aEntryProperties Structure containing the entry properties
*/
void CTestCalInterimApiFetchEntryAndCheckData::FetchAndCheckDataL(const TPtrC& aGuid, const TInt& aExpectedCount,
															   const TEntryProperties& aEntryProperties, const TPtrC& aTimeMode)
	{
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*		entryId = HBufC8::NewLC(aGuid.Length());
	entryId->Des().Copy(aGuid);
	iCalEntryView->FetchL(entryId->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryId);
	TBool	matchInRecurrenceId = EFalse;
	if ( entriesFetched.Count() == aExpectedCount )
		{
		for ( TInt i = 0; i < entriesFetched.Count(); i++ )
			{
			CCalEntry*	calEntry = entriesFetched[i];
			if ( aEntryProperties.iRecurrenceId == KNullDesC16() ) // Parent Entry
				{
				if ( calEntry->RecurrenceIdL().TimeUtcL() == Time::NullTTime() )
					{
					// Checks the data for the parent entry
					CheckDataL(calEntry, aEntryProperties);
					}
				}
			else // Child Entry
				{
				TTime		recurrenceIdValue;
				recurrenceIdValue.Set(aEntryProperties.iRecurrenceId);
				TCalTime	recurrenceId;
				// Sets the time mode to floating or Utc based on the time mode given
				SetCalTimeL(recurrenceIdValue, iRecurrenceId, SetTimeMode(aTimeMode));
				if (iRecurrenceId.TimeUtcL() == (calEntry->RecurrenceIdL().TimeUtcL()) )
					{
					matchInRecurrenceId = ETrue;
					// Checks the data for the child entry
					CheckDataL(calEntry, aEntryProperties);
					}
				}
			}
		INFO_PRINTF3(KInfoEntryFetched, entriesFetched.Count(), aExpectedCount)	;
		}
	else
		{
		ERR_PRINTF3(KErrExpectedCount, entriesFetched.Count(), aExpectedCount);
		SetTestStepResult(EFail);
		}
	/* If the recurrence Id doesn't match with the recurrenceId of the child entry
	then, print a warning message */
	if (aEntryProperties.iRecurrenceId != KNullDesC16() && matchInRecurrenceId == 0)
		{
		WARN_PRINTF1(KWarnNoMatchInRecurrenceId);
		}
	CleanupStack::PopAndDestroy(&entriesFetched);
	}


/** Comparing the datas with the set values from the ini file.
@param	aCalEntry Pointer to CCalEntry.
@param	aEntryProperties Structure containing the entry properties
*/
void CTestCalInterimApiFetchEntryAndCheckData::CheckDataL(CCalEntry* aCalEntry, const TEntryProperties& aEntryProperties)
	{
	if ( aEntryProperties.iSummary != KNullDesC16() )
		{
		TPtrC	summary(aCalEntry->SummaryL());
		INFO_PRINTF3(KExpectedAndFoundSummary, &aEntryProperties.iSummary, &summary);
		TEST1( summary == aEntryProperties.iSummary, ETrue);
		}
	if ( aEntryProperties.iDescription != KNullDesC16() )
		{
		TPtrC	description(aCalEntry->DescriptionL());
		INFO_PRINTF3(KExpectedAndFoundDescription, &aEntryProperties.iDescription, &description);
		TEST1( description == aEntryProperties.iDescription, ETrue);
		}
	if ( aEntryProperties.iLocation != KNullDesC16() )
		{
		TPtrC	location(aCalEntry->LocationL());
		INFO_PRINTF3(KExpectedAndFoundLocation, &aEntryProperties.iLocation, &location);
		TEST1( location == aEntryProperties.iLocation, ETrue);	
		}
	if(aEntryProperties.iCategories != KNullDesC16() )
		{
		CheckCategoriesL(aCalEntry, aEntryProperties.iCategories);
		}
	if(aEntryProperties.iHasLocalId )
		{
		TCalLocalUid	localId(aCalEntry->LocalUidL());
		INFO_PRINTF3(KExpectedAndFoundLocalId, aEntryProperties.iLocalId, localId);
		TEST1( localId == aEntryProperties.iLocalId, ETrue );
		}
	if(aEntryProperties.iHasPriority )
		{
		TUint	priority(aCalEntry->PriorityL());
		INFO_PRINTF3(KExpectedAndFoundPriority, aEntryProperties.iPriority, priority);
		TEST1( priority == aEntryProperties.iPriority, ETrue );
		}
	if (aEntryProperties.iGeoLat != KNullDesC16() && aEntryProperties.iGeoLong != KNullDesC16())
		{
		CheckGEOPropertyL(aCalEntry, aEntryProperties.iGeoLat, aEntryProperties.iGeoLong);
		}
	if (aEntryProperties.iTrans != KValueIntNotIni)
		{
		TInt trans(aCalEntry->TimeTransparencyL());
		INFO_PRINTF3(KExpectedAndFoundStatus, aEntryProperties.iTrans, trans);
		TEST1( trans == aEntryProperties.iTrans, ETrue);
		}
		
	}

/** Check the updated categories extended name
@param	aCalEntry Pointer to CCalEntry
@param	aCategories Expected category name
*/
void CTestCalInterimApiFetchEntryAndCheckData::CheckCategoriesL(CCalEntry* aCalEntry, const TDesC& aCategories)
	{
	RArray<TPtrC> categoryList;
	TokenizeStringL(aCategories, categoryList);
	RPointerArray<CCalCategory> entryCatList;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyCategoryArray, &entryCatList));
	entryCatList = aCalEntry->CategoryListL();

	for(TInt ii = 0; ii < categoryList.Count(); ++ii)
		{
		TInt jj = 0;
		for(; jj < entryCatList.Count(); ++jj)
			{
			if(entryCatList[jj]->ExtendedCategoryName() == categoryList[ii])
				{
				INFO_PRINTF2(KInfoCategoryExists, &(categoryList[ii]));
				break; // break jj
				}
			} // jj

		if(jj >= entryCatList.Count())
			{
			ERR_PRINTF2(KErrCategoryNotExists, &(categoryList[ii]));
			SetTestStepResult(EFail);
			}
		}// ii

	CleanupStack::PopAndDestroy(&entryCatList);
	}

/** Check the GEO Property value stored in entry, with expected GEO Property value
@param	aCalEntry Pointer to CCalEntry
@param	aGeoLat Expected GEO latitude value
@param	aGeoLong Expected GEO longitude value
*/
void CTestCalInterimApiFetchEntryAndCheckData::CheckGEOPropertyL(CCalEntry* aCalEntry, const TDesC& aGeoLat, const TDesC& aGeoLong)
	{
	CCalGeoValue* geoValue = NULL;
	
	TRAPD(err, geoValue = aCalEntry->GeoValueL());

	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("GeoValuesL Error Code %d"), err);
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
	else
		{
		CleanupStack::PushL(geoValue);
		}
	
	TReal lat = 0.0;
	TReal longitude = 0.0;
	TBool hasGEO = geoValue->GetLatLong(lat,longitude);
	
	TESTL(hasGEO);

	TLex lex(aGeoLat);
	TReal expectLat = 0.0;
	lex.Val(expectLat);

	TLex lex1(aGeoLong);
	TReal expectLong = 0.0;
	lex1.Val(expectLong);

	INFO_PRINTF5(KExpectedAndFoundGEO, expectLat, lat, expectLong, longitude);

	TEST1(lat == expectLat, ETrue);
	TEST1(longitude == expectLong, ETrue);
	
	if (err == KErrNone)
		{
		CleanupStack::PopAndDestroy(geoValue);	
		}
	}

/** Performing soak test for fetch entry by Guid
@param	aGuid guid of the entry
@param	aSoakTest Boolean value to perform soak test
@param	aSoakTestCount Count of the soak test
*/
void CTestCalInterimApiFetchEntryAndCheckData::SoakTestsForFetchEntryL(const TPtrC& aGuid, const TBool& aSoakTest,
																	const TInt& aSoakTestCount)
	{
	if ( aSoakTest )
		{
		for ( TInt count = 1; count <= aSoakTestCount; count++ )
			{
			RPointerArray<CCalEntry>	entriesFetched;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
			HBufC8*		entryId = HBufC8::NewLC(aGuid.Length());
			entryId->Des().Copy(aGuid);
			TRAPD( err, iCalEntryView->FetchL(entryId->Des(), entriesFetched) );
			CleanupStack::PopAndDestroy(entryId);
			TESTL ( err == KErrNone );
			CleanupStack::PopAndDestroy(&entriesFetched);
			}
		}
	}

