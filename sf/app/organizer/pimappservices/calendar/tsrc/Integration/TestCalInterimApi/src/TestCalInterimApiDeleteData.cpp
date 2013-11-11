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

#include "TestCalInterimApiDeleteData.h"
#include <calsession.h>
#include <calentry.h>
#include <calentryview.h>

_LIT(KGUid,						"guid%d");
_LIT(KDeleteEntry,				"deleteentries");
_LIT(KDeleteArrayOfUids,		"deletearrayofuids");
_LIT(KExpectedCount,			"expectedcount%d");
_LIT(KRecurrenceId,				"recurrenceid");
_LIT(KExpectedCountAfterDelete,	"expectedcountafterdelete");
_LIT(KTimeMode,					"timemode");

_LIT(KInfoIndex,				"Index %d");
_LIT(KInfoDeletion,				"Entry Deleted succesfully");
_LIT(KInfoMatchInRecurrenceId,	"Recurrence Id for entryindex%d matches");
_LIT(KInfoNoMatchInRecurrenceId,"Recurrence Id for entryindex%d does not matches");

_LIT(KErrEntryNotDeleted,		"Error:Entry Not deleted");
_LIT(KErrExpectedCount,			"EntryCount %d. Expected %d");
_LIT(KErrEntriesNotFound,		"Error:Entries Not Found");
_LIT(KErrInDeleteArrayOfUids,	"Error:Entries Still Found after deleting");
_LIT(KErrInDeleteEntry,			"Error in DeleteL() function");
_LIT(KErrNoMatchInEntryCount,	"Error:Entry count after fetch and expected count does not matches");
_LIT(KErrRecurrenceIdExists,	"Error:Recurrence Id still exists after deleting the child entry");

_LIT(KWarnMessage,				"Either one of them should be true to perform some operation");


CTestCalInterimApiDeleteData::CTestCalInterimApiDeleteData()
:	CTestCalInterimApiSuiteStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteData);
	}

CTestCalInterimApiDeleteData::~CTestCalInterimApiDeleteData()
/**
 * Destructor
 */
	{
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteData::doTestStepL()
	{
	TBool	deleteEntry;
	TBool	deleteArrayOfUids;
	TPtrC	recurrenceIdValue;
	TPtrC	timeMode;
	TInt	expectedCountAfterDelete;
	
	TESTL( GetBoolFromConfig(ConfigSection(), KDeleteEntry, deleteEntry) );
	TESTL( GetBoolFromConfig(ConfigSection(), KDeleteArrayOfUids, deleteArrayOfUids) );
	// Don't mind if these are not present in ini files
	GetStringFromConfig(ConfigSection(), KRecurrenceId, recurrenceIdValue);
	// expectedCountAfterDelete field is mandatory for delete entry by reference
	GetIntFromConfig(ConfigSection(), KExpectedCountAfterDelete, expectedCountAfterDelete);
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	
	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TPtrC	gUid;
	TInt	index = 0;
	TBool	moreData = ETrue;
	iGuidArray = new(ELeave)CDesC8ArrayFlat(1);
	while ( moreData )
		{
		tempStore.Format(KGUid(), ++index);
		INFO_PRINTF2(KInfoIndex, index);
		moreData = GetStringFromConfig(ConfigSection(), tempStore, gUid);			
		if ( moreData )
			{
			TInt	expectedCount = 1;
			tempStore.Format(KExpectedCount(), index);
			GetIntFromConfig(ConfigSection(), tempStore, expectedCount);
			HBufC8*		entryGuid = HBufC8::NewLC(gUid.Length());
			entryGuid->Des().Copy(gUid);
			if ( deleteEntry ) // Fetch the entry and delete the parent or child entry
				{ 
				FetchAndDeleteEntryL(gUid, expectedCount, recurrenceIdValue, timeMode, expectedCountAfterDelete);
				}
			if ( deleteArrayOfUids ) // Delete the array of Uid of entries
				{
				iGuidArray->AppendL(entryGuid->Des());
				}
			CleanupStack::PopAndDestroy(entryGuid);		
			}
		}
		
	if ( deleteArrayOfUids )
		{
		DeleteArrayOfUidsL();
		}
	if ( !deleteEntry && !deleteArrayOfUids ) // either one should be true
		{
		WARN_PRINTF1(KWarnMessage);
		}

	return TestStepResult();
	}


/** Deletes the array of UID of entries.
It leaves with error code KErrNotFound if the entries are not present.
*/
void CTestCalInterimApiDeleteData::DeleteArrayOfUidsL()
	{
	TRAPD(err, iEntryView->DeleteL(*iGuidArray));
	if ( err == KErrNotFound )
		{
		ERR_PRINTF1(KErrEntriesNotFound);
		SetTestStepResult(EFail);
		iGuidArray->Reset();
		}
	else if ( err == KErrNone )
		{
		for (TInt i = 0; i < iGuidArray->MdcaCount(); i++ )
			{
			RPointerArray<CCalEntry>	entriesFetchedAfterDelete;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetchedAfterDelete));
			HBufC8*	gUid = (*iGuidArray)[i].AllocLC();
			iEntryView->FetchL(gUid->Des(), entriesFetchedAfterDelete);
			CleanupStack::PopAndDestroy(gUid);
			// Verify if the entry is present or not
			if ( entriesFetchedAfterDelete.Count() > 0 )
				{
				ERR_PRINTF1(KErrInDeleteArrayOfUids);
				SetTestStepResult(EFail);
				}
			else if ( entriesFetchedAfterDelete.Count() == 0 )
				{
				INFO_PRINTF1(KInfoDeletion);
				}
			CleanupStack::PopAndDestroy(&entriesFetchedAfterDelete);
			iGuidArray->Reset();
			}	
		}
	else
		{
		ERR_PRINTF1(KErrInDeleteEntry);
		SetTestStepResult(EFail);
		}
	}

/** Fetch the entry by passing the UID and delete the Parent entry or child entry based 
	on the recurrence id given
@param	aGuid Guid of the entry
@param	aExpectedCount expected count of the fetched entries
@param	aRecurrenceId recurrence Id of the child entry
@param	aTimeMode timemode of the entry i.e floating/ UTC
@param	aExpectedCountAfterDelete expected count after delete
*/
void CTestCalInterimApiDeleteData::FetchAndDeleteEntryL(const TPtrC& aGuid, const TInt& aExpectedCount, const TPtrC& aRecurrenceIdValue,
													 const TPtrC& aTimeMode, const TInt& aExpectedCountAfterDelete)
	{
	RPointerArray<CCalEntry>	entriesFetchedBeforeDelete;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetchedBeforeDelete));
	HBufC8*	entryGuid = HBufC8::NewLC(aGuid.Length());
	entryGuid->Des().Copy(aGuid);
	iEntryView->FetchL(entryGuid->Des(), entriesFetchedBeforeDelete);
	CleanupStack::PopAndDestroy(entryGuid);
	TInt	entryCountBeforeDelete(entriesFetchedBeforeDelete.Count());
	if ( entryCountBeforeDelete == aExpectedCount )
		{
		for ( TInt j = 0; j < entryCountBeforeDelete; j++ )
			{
			CCalEntry*	calEntry = entriesFetchedBeforeDelete[j];
			// Deleting the child entry based on the recurrence Id
			if ( aRecurrenceIdValue != KNullDesC16() )
				{
				TTime	recurrIdValue;
				recurrIdValue.Set(aRecurrenceIdValue);
				// Sets the time mode to floating or Utc based on the time mode given
				SetCalTimeL(recurrIdValue, iRecurrenceId, SetTimeMode(aTimeMode));
				if ( iRecurrenceId.TimeUtcL() == (calEntry->RecurrenceIdL().TimeUtcL()) )
					{
					INFO_PRINTF2(KInfoMatchInRecurrenceId, j);	
					DeleteAndVerifyL(calEntry, aRecurrenceIdValue, aExpectedCountAfterDelete);
					}
				else
					{
					INFO_PRINTF2(KInfoNoMatchInRecurrenceId, j);
					}
				}
			// Deletes the parent entry
			else if ( calEntry->RecurrenceIdL().TimeUtcL() == Time::NullTTime() )
				{
				DeleteAndVerifyL(calEntry, aRecurrenceIdValue, aExpectedCountAfterDelete);
				break;
				}	
			}
		}
		else
		{
		ERR_PRINTF3(KErrExpectedCount, entryCountBeforeDelete, aExpectedCount);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&entriesFetchedBeforeDelete);
	}
	
		
/** Delete the Parent/Child entry and verify if it has been deleted
@param	aCalEntry Pointer to the CCalEntry
@param	aRecurrenceId RecurrenceId of the entry
@param	aExpectedCountAfterDelete Count of the entry after delete
*/
void CTestCalInterimApiDeleteData::DeleteAndVerifyL(CCalEntry* aCalEntry, const TPtrC& aRecurrenceIdValue, 
												 const TInt& aExpectedCountAfterDelete)	
	{
	const TPtrC8	gUid = aCalEntry->UidL();
	iEntryView->DeleteL(*aCalEntry);
	RPointerArray<CCalEntry>	entriesFetchedAfterDelete;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetchedAfterDelete));
	iEntryView->FetchL(gUid, entriesFetchedAfterDelete);
				
	if ( (aRecurrenceIdValue != KNullDesC16()) && (entriesFetchedAfterDelete.Count() == (aExpectedCountAfterDelete)) )
		{
		INFO_PRINTF1(KInfoDeletion);
		CheckRecurrenceIdDeletedL(entriesFetchedAfterDelete);	
		}
	else if ( aRecurrenceIdValue == KNullDesC16() )
		{
		if ( entriesFetchedAfterDelete.Count() )
			{
			ERR_PRINTF1(KErrEntryNotDeleted);
			SetTestStepResult(EFail);
			}	
		else
			{
			INFO_PRINTF1(KInfoDeletion);
			}
		}
	else
		{
		ERR_PRINTF1(KErrNoMatchInEntryCount);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&entriesFetchedAfterDelete);
	}

/** Compare and check the recurrence id of the deleted entry if it still exists.
@param	aEntriesFetchedAfterDelete Array containing the fetched entries
*/
void CTestCalInterimApiDeleteData::CheckRecurrenceIdDeletedL(RPointerArray<CCalEntry>& aEntriesFetchedAfterDelete)
	{
	for ( TInt i = 0; i < aEntriesFetchedAfterDelete.Count(); i++ )
		{
		CCalEntry*	entry = aEntriesFetchedAfterDelete[i];
		if ( iRecurrenceId.TimeUtcL() == (entry->RecurrenceIdL().TimeUtcL()) )
			{
			ERR_PRINTF1(KErrRecurrenceIdExists);
			SetTestStepResult(EFail);
			}
		}
	}
	
