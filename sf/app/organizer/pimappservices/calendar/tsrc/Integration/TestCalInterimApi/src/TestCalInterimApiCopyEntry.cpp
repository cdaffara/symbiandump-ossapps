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

#include "TestCalInterimApiCopyEntry.h"

_LIT(KDontCopyId,					"EDontCopyId");
_LIT(KCopyType,						"copytype");
_LIT(KSourceEntryGUid,				"guid");
_LIT(KDestinationEntryGUid,			"newguid");
_LIT(KSourceRecurrenceId,			"SourceRecurrId");
_LIT(KDestinationRecurrenceId,		"DestinationRecurrId");
_LIT(KDestinationRecurrenceRange,	"DestinationRecurrRange");
_LIT(KThisAndFuture,				"ThisAndFuture");
_LIT(KThisAndPrior,					"ThisAndPrior");
_LIT(KThisAndAll,					"ThisAndAll");
_LIT(KAllocTest,					"alloctest");

_LIT(KInfoCopy,						"Entry copied successfully");
_LIT(KErrInDontCopyId,				"Error:UID's and local Id of original and the copied entry matches");
_LIT(KErrInCopyAll,					"Error:UID's and local Id of original and the copied entry does not match");
_LIT(KErrStoreOperation,			"Error Occured during store operation %d \n");
_LIT(KErrFetchLOperation,			"Error Occured during FetchL operation \n");

CTestCalInterimApiCopyEntry::CTestCalInterimApiCopyEntry()
: CTestCalInterimApiSuiteStepBase(), iCopyType( CCalEntry::ECopyAll)
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiCopyEntry);
	}

CTestCalInterimApiCopyEntry::~CTestCalInterimApiCopyEntry()
/**
 * Destructor
 */
	{
	delete iDestinationEntry;
	iEntryArray.ResetAndDestroy();
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiCopyEntry::doTestStepL()
	{
	GetDataFromIniFile();
	BuildEntryViewL();
	SetCopyType();
	if ( iAllocTest ) // OOM Testing
		{
		OOMTestForCopyL();
		}
	else
		{
		SetEntryAndCopyL();	
		}
	StoreCopiedEntryL();
	CheckEntriesL();
	return TestStepResult();
	}

/** Read data from the ini file
*/
void CTestCalInterimApiCopyEntry::GetDataFromIniFile()
	{
	GetStringFromConfig(ConfigSection(), KSourceEntryGUid, iGuidSource);
	GetStringFromConfig(ConfigSection(), KDestinationEntryGUid, iGuidDestination);
	GetStringFromConfig(ConfigSection(), KSourceRecurrenceId, iSourceRecurrenceId);
	GetStringFromConfig(ConfigSection(), KDestinationRecurrenceId, iDestinationRecurrenceId);
	GetStringFromConfig(ConfigSection(), KDestinationRecurrenceRange, iDestinationRecurrenceRange);
	GetStringFromConfig(ConfigSection(), KCopyType, iCopyTypeData);
	GetBoolFromConfig(ConfigSection(), KAllocTest, iAllocTest);
	}


/** 
* Prepare the entryview
*/
void CTestCalInterimApiCopyEntry::BuildEntryViewL()
	{
	iEntryView = CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();
	}

/**
* Retrieve the entry to be copied
*/
void CTestCalInterimApiCopyEntry::SetSourceEntryL()
	{
	HBufC8*	sourceEntryGuid = HBufC8::NewLC(iGuidSource.Length());
	sourceEntryGuid->Des().Copy(iGuidSource);
	iEntryView->FetchL(sourceEntryGuid->Des(), iEntryArray);
	CleanupStack::PopAndDestroy(sourceEntryGuid);

	if( iEntryArray.Count() < 0)
		{
		ERR_PRINTF1(KErrFetchLOperation);
		SetTestStepResult(EFail);
		return;
		}

	TTime srcRecurIdInTTime;
	TCalTime srcRecurIdInCalTime;

	if (iSourceRecurrenceId == KNullDesC16()) // Parent Entry
		{
		iSourceEntry = 	iEntryArray[0];
		}
	else // Child Entry
		{
		srcRecurIdInTTime.Set(iSourceRecurrenceId);
		srcRecurIdInCalTime.SetTimeUtcL(srcRecurIdInTTime);

		for(TInt i = 1; i < iEntryArray.Count(); i++)
			{
			if(iEntryArray[i]->RecurrenceIdL().TimeUtcL() == srcRecurIdInCalTime.TimeUtcL())
				{
				iSourceEntry = iEntryArray[i];
				}
			}
		}
	}

/** 
*Create the destination entry
*/
void CTestCalInterimApiCopyEntry::SetDestinationEntryL()
	{
	TTime destRecurIdInTTime;
	TCalTime destRecurIdInCalTime;

	// Convert TPtrC to TCalTime
	if (iDestinationRecurrenceId == KNullDesC16())
		{
		destRecurIdInCalTime.SetTimeUtcL(Time::NullTTime()); // Parent Entry
		}
	else // Child Entry
		{
		destRecurIdInTTime.Set(iDestinationRecurrenceId);
		destRecurIdInCalTime.SetTimeUtcL(destRecurIdInTTime);
		}

	HBufC8*	destinationEntryGuid = HBufC8::NewLC(iGuidDestination.Length());
	destinationEntryGuid->Des().Copy(iGuidDestination);

	// Creating the destination entry
	if (iDestinationRecurrenceId == KNullDesC16()) // Parent Entry
		{
		iDestinationEntry = CCalEntry::NewL(iSourceEntry->EntryTypeL(), destinationEntryGuid,
	                                  iSourceEntry->MethodL(), iSourceEntry->SequenceNumberL());

		}
	else // Child Entry
		{
		CalCommon::TRecurrenceRange destinationRecurrenceRange = GetRecurrenceRange(iDestinationRecurrenceRange);
		iDestinationEntry = CCalEntry::NewL(iSourceEntry->EntryTypeL(), destinationEntryGuid,
	                                  iSourceEntry->MethodL(), iSourceEntry->SequenceNumberL(),
	                                       destRecurIdInCalTime, destinationRecurrenceRange);
		}

	CleanupStack::Pop(destinationEntryGuid);
	}

/** 
*Copy source entry details into destination entry based on the copy type
*/
void CTestCalInterimApiCopyEntry::CopyEntryL()
	{
 	iDestinationEntry->CopyFromL(*iSourceEntry, iCopyType);
	}

/** 
*Store the destination entry after copying relevant details from the Source Entry.
*/
void CTestCalInterimApiCopyEntry::StoreCopiedEntryL()
	{
	RPointerArray<CCalEntry> entriesToStore;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &entriesToStore));
	entriesToStore.AppendL(iDestinationEntry);
	TInt entriesStored(0);
	TRAPD(err, iEntryView->StoreL(entriesToStore, entriesStored));

	if(err != KErrNone || entriesStored <= 0)
		{
		ERR_PRINTF2(KErrStoreOperation,err);
		SetTestStepResult(EFail);
		CleanupStack::PopAndDestroy(&entriesToStore);
		return;
		}
	// Since iDestinationEntry is required, entriesToStore is just popped and the 
	// iDestinationEntry is destroyed in the destructor.
	CleanupStack::Pop(&entriesToStore); 
	}

/** 
*Compare the source and destination entry details
*/
void CTestCalInterimApiCopyEntry::CheckEntriesL()
	{
	// Check the UID details only for the parent entry
	if ( iDestinationEntry->RecurrenceIdL().TimeUtcL() == Time::NullTTime() )
		{
		CheckUidDetailsL();	
		}
	CompareEntriesL(iSourceEntry, iDestinationEntry);
	}

/** 
* Set the copytype to be used, By Default it is set to CCalEntry::ECopyAll
*/
void CTestCalInterimApiCopyEntry::SetCopyType()
	{
	if ( iCopyTypeData.Compare(KDontCopyId) == 0 )
		{
		iCopyType = CCalEntry::EDontCopyId;
		}
	}


/** Get the Recurrence Range of the entry from ini file and convert it of TRecurrenceRange.
@param	aRecurrenceRange Data for Setting the Recurrence Range of the entry
@return An object of type CalCommon::TRecurrenceRange with desired Recurrence range
*/
CalCommon::TRecurrenceRange CTestCalInterimApiCopyEntry::GetRecurrenceRange(const TPtrC& aRecurrenceRange)
	{
	CalCommon::TRecurrenceRange	retRecurrenceRange = CalCommon::EThisOnly;

	if ( aRecurrenceRange.Compare(KThisAndFuture) == 0 )
		{
		retRecurrenceRange = CalCommon::EThisAndFuture;
		}
	else if ( aRecurrenceRange.Compare(KThisAndPrior) == 0 )
		{
		retRecurrenceRange = CalCommon::EThisAndPrior;
		}
	else if ( aRecurrenceRange.Compare(KThisAndAll) == 0 )
		{
		retRecurrenceRange = CalCommon::EThisAndAll;
		}

	return retRecurrenceRange;
	}


/** 
* Compare Global Uids and Local Uids of the Source and the Destination entry
*/
void CTestCalInterimApiCopyEntry::CheckUidDetailsL()
	{
	const TDesC8&	originalUid = iSourceEntry->UidL();
	TCalLocalUid	originalLocalId = iSourceEntry->LocalUidL();
	const TDesC8&	Uid = iDestinationEntry->UidL();
	TCalLocalUid	localId = iDestinationEntry->LocalUidL();

	if ( iCopyType == CCalEntry::EDontCopyId )
		{
		if ( (originalUid == Uid) || (originalLocalId == localId) )
			{
			ERR_PRINTF1(KErrInDontCopyId);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(KInfoCopy);
			}
		}
	else // ECopyAll
		{
		if ( (originalUid != Uid) || (originalLocalId != localId) )
			{
			ERR_PRINTF1(KErrInCopyAll);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(KInfoCopy);
			}
		}
	}

/**
* Set the destination entry and copy the entry to another
*/	
void CTestCalInterimApiCopyEntry::SetEntryAndCopyL()
	{
	delete iDestinationEntry;
	iDestinationEntry = NULL;
	iEntryArray.ResetAndDestroy();
	
	SetSourceEntryL();
  	SetDestinationEntryL();
  	CopyEntryL();
	}

/** 
* Perform OOM testing for copy entry
*/		
void CTestCalInterimApiCopyEntry::OOMTestForCopyL()
	{
	TInt tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAPD(err, SetEntryAndCopyL());
				
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for CopyEntryL is done"));
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


