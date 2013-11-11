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

#include "TestCalInterimApiDeleteEntryByLocalId.h"
#include <calsession.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calentry.h>
#include <calentryview.h>

_LIT(KGuid,							"guid");
_LIT(KRecurrenceId,					"recurrenceid%d");
_LIT(KTimeMode,						"timemode");
_LIT(KAllocTest,					"alloctest");
_LIT(KLocalId,						"localid%d");

_LIT(KInfoDeleteSuccessful,			"DeleteL() successful");

_LIT(KErrInDeleteL,					"Error in DeleteL()");

CTestCalInterimApiDeleteEntryByLocalId::CTestCalInterimApiDeleteEntryByLocalId()
:	CTestCalInterimApiSuiteStepBase()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteEntryByLocalId);
	}

CTestCalInterimApiDeleteEntryByLocalId::~CTestCalInterimApiDeleteEntryByLocalId()
/**
 * Destructor
 */
	{
	iEntriesLocalIdToDelete.Close();
	iRecurrenceIdArray.Close();
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteEntryByLocalId::doTestStepL()
	{
	TPtrC	gUid;
	TPtrC	timeMode;

	GetStringFromConfig(ConfigSection(), KGuid, gUid);
	// Don't mind if these are not given in the ini files
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	GetBoolFromConfig(ConfigSection(), KAllocTest, iAllocTest);

	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	index = 0;
	TBool	moreData = ETrue;
	TPtrC	recurrenceId;
	while ( moreData )
		{
		tempStore.Format(KRecurrenceId(), ++index);
		moreData = GetStringFromConfig(ConfigSection(), tempStore, recurrenceId);	
		if ( moreData )
			{
			TTime	recurrenceIdValue;
			recurrenceIdValue.Set(recurrenceId);
			TCalTime	recurrence;
			// Sets the time mode to floating or Utc based on the time mode given
			SetCalTimeL(recurrenceIdValue, recurrence, SetTimeMode(timeMode));
			iRecurrenceIdArray.AppendL(recurrence);	
			}
		}

	/* If guid or recurrence id is present, then fetch the entry based on the 
	   guid or recurrence id, append the local id's of the entries to the array
	   and delete the entries by local id */
	if ( (gUid != KNullDesC) || (iRecurrenceIdArray.Count() > 0) )
		{
		FetchEntryAndAppendToArrayL(gUid);	
		}
	// Get the local ids from the user and delete the entries by passing the local id's
	else 
		{
		GetLocalIdsAndAppendToArrayL();
		}
	
	// Delete the entries by passing the local id's
	DeleteArrayOfLocalIdsL();
		
	return TestStepResult();
	}


/** Fetch the entry and delete the Parent or child entry by passing the local id of the entry
@param	aGuid Guid of the entry to fetch
*/		
void CTestCalInterimApiDeleteEntryByLocalId::FetchEntryAndAppendToArrayL(const TPtrC& aGuid)
	{
	RPointerArray<CCalEntry>	entriesFetched;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesFetched));
	HBufC8*	entryGuid = HBufC8::NewLC(aGuid.Length());
	entryGuid->Des().Copy(aGuid);
	iEntryView->FetchL(entryGuid->Des(), entriesFetched);
	CleanupStack::PopAndDestroy(entryGuid);
	for ( TInt i = 0; i < entriesFetched.Count(); i++ )
		{
		if ( entriesFetched[i]->RecurrenceIdL().TimeUtcL() != Time::NullTTime() )
			{
			for ( TInt j = 0; j < iRecurrenceIdArray.Count(); j++ )
				{
				TCalTime	recurrenceId = iRecurrenceIdArray[j];	
				if ( entriesFetched[i]->RecurrenceIdL().TimeUtcL() == recurrenceId.TimeUtcL() )
					{
					// Appends the local id of the child entry based on the recurrence Id
					iEntriesLocalIdToDelete.AppendL(entriesFetched[i]->LocalUidL());
					break; // Completed operation with respect to current recurrence Id
					}
				}
			}
		if ( !iRecurrenceIdArray.Count() ) 
			{
			// if recurrence id is not present, then append the local id of the parent entry
			iEntriesLocalIdToDelete.AppendL(entriesFetched[i]->LocalUidL());
			break; // Completed operation with respect to Parent entry and hence there is no need to proceed with the child entries
			}
		}
	CleanupStack::PopAndDestroy(&entriesFetched);
	}


/** 
* delete the entries stored in the array
*/
void CTestCalInterimApiDeleteEntryByLocalId::DeleteArrayOfLocalIdsL()
	{
	TInt	numofEntriesDeleted(0);
	if ( iAllocTest )
		{
		PerformOOMForDeleteByLocalIdL(numofEntriesDeleted);
		}
	else
		{
		TRAPD( err, iEntryView->DeleteL(iEntriesLocalIdToDelete, numofEntriesDeleted) );
		if ( err == KErrNone )
			{
			INFO_PRINTF1(KInfoDeleteSuccessful);
			}
		else 
			{
			ERR_PRINTF1(KErrInDeleteL);
			SetTestStepResult(EFail);
			}
		iEntriesLocalIdToDelete.Reset();
		}
	}
	
/** 
*Get the local id of the entries from the user and append it to the array
*/	
void CTestCalInterimApiDeleteEntryByLocalId::GetLocalIdsAndAppendToArrayL()
	{
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	index = 0;
	TBool	moreData = ETrue;
	TInt	localId;
	while ( moreData )
		{
		tempStore.Format(KLocalId(), ++index);
		moreData = GetIntFromConfig(ConfigSection(), tempStore, localId);	
		if ( moreData )
			{
			iEntriesLocalIdToDelete.AppendL(localId);	
			}
		}
		iEntriesLocalIdToDelete.Reset();
	}
	
/** Perform OOM testing for Delete Entries By LocalId
@param	aNumOfEntriesDeleted Variable to store the number of deleted entries
*/	
void CTestCalInterimApiDeleteEntryByLocalId::PerformOOMForDeleteByLocalIdL(TInt aNumOfEntriesDeleted)
	{
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
	
		TRAPD( err, iEntryView->DeleteL(iEntriesLocalIdToDelete, aNumOfEntriesDeleted) );
				
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for DeleteEntryByLocalId is done"));
			break;
			}
		if ( err != KErrNoMemory )
			{
			SetTestStepResult(EFail);
			break;
			}
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}	
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}

