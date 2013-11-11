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

#include "TestCalInterimApiModifier.h"
#include <vtzrules.h>

CTestCalInterimApiModifier::CTestCalInterimApiModifier()
: CTestCalInterimApiSuiteStepBase()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	}


CTestCalInterimApiModifier::~CTestCalInterimApiModifier()
/**
 * Destructor
 */
	{
	delete iGuidList;
	delete iEntryIterator;
	delete iEntryView;
	delete iSession;
	}

CTestCalInterimApiModifier* CTestCalInterimApiModifier::NewL(CTestStep* aTestStep)
	{
	CTestCalInterimApiModifier* self = new(ELeave) CTestCalInterimApiModifier;
	CleanupStack::PushL(self);
	self->ConstructL(aTestStep);
	CleanupStack::Pop(self);
	return self;
	}

void CTestCalInterimApiModifier::ConstructL(CTestStep* aTestStep)
	{
	iTestStep = aTestStep;
	// Type casting to share the main thread's member variables across this thread
	iModifier = static_cast<CTestCalInterimApiMainThread*>(iTestStep);

	// A seperate session with the Calendar file is necessary to receive the notifications
	iSession = CCalSession::NewL();
	iSession->OpenL(iModifier->iCalenderFileName);
	User::LeaveIfError(iSemaphore.OpenGlobal(iModifier->iSemName));
	}

/**
* Function which performs operation like addition, update and deletion of entries based on
* the flag given in the ini file
*/
void CTestCalInterimApiModifier::RunTestsL()
	{
	iEntryView = CCalEntryView::NewL(*iSession, *this);
	CActiveScheduler::Start();

	TPtrC deleteType;
	iTestStep->GetStringFromConfig(iTestStep->ConfigSection(), KDeleteType, deleteType);
	
	TBool	enableChangeBroadCast = EFalse;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KEnableChangeBroadCast, enableChangeBroadCast);

	// if enableChangeBroadCast flag is set, then the observer starts notifying for bulk changes
	if ( enableChangeBroadCast )
		{
		EnableCalChangeBroadcast();
		}	
		
	TBool	disableChangeBroadCast = EFalse;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KDisableChangeBroadCast, disableChangeBroadCast);	
	
	// if disableChangeBroadCast flag is set, then the notifications is not expected by Modifier Thread
	if ( disableChangeBroadCast )
		{
		DisableCalChangeBroadcast();
		}
	
	AddModifyDeleteEntriesL(deleteType);
	}

/**
* Function which disables the change broadcast and the observer will not give any
* notifications if this API is called
*/
void CTestCalInterimApiModifier::DisableCalChangeBroadcast()
	{
	iSession->DisableChangeBroadcast();
	}

/**
* Function which enables the change broadcast and the observer start giving
* notifications if this API is called
*/
void CTestCalInterimApiModifier::EnableCalChangeBroadcast()
	{
	iSession->EnableChangeBroadcast();
	}

/**
* Function which performs addition, update and deletion of the entries.
@param aDeleteFlag Flag to perform different delete operations like delete by reference,
*				   delete by uid, local uid and delete by instances.
*/
void CTestCalInterimApiModifier::AddModifyDeleteEntriesL(const TPtrC& aDeleteFlag)
	{
	TBool addEntriesFlag = EFalse;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KAddEntriesFlag, addEntriesFlag);
	// If this flag is set, then add the entries in the calendar file
	if( addEntriesFlag )
		{
		AddEntriesL();
		}

	TBool updateEntriesFlag = EFalse;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KUpdateEntriesFlag, updateEntriesFlag);
	// If this flag is set, then update the entries in the calendar file
	if( updateEntriesFlag )
		{
		UpdateEntriesL();
		}

	TBool deleteEntriesFlag = EFalse;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KDeleteEntriesFlag, deleteEntriesFlag);
	// If this flag is set, then delete the entries in the calendar file
	if( deleteEntriesFlag )
		{
		if ( !aDeleteFlag.Compare(KDeleteByInstances) )
			{
			DeleteInstancesL();
			}
		if( !aDeleteFlag.Compare(KDeleteByLocalUids) )
			{
			DeleteEntriesByArrayOfLocalUidsL();
			}
		else if( !aDeleteFlag.Compare(KDeleteByReference) )
			{
			DeleteEntriesByReferenceL();
			}
		else if ( !aDeleteFlag.Compare(KDeleteByTimeRange) )
			{
			DeleteEntriesByTimeRangeL();
			}
		else // DeleteByUid
			{
			DeleteEntriesByUidListL();
			}
		}
	}

/**
* Add entries to the calendar file
*/
void CTestCalInterimApiModifier::AddEntriesL()
	{
	TInt  entriesStored(0);
	iTestStep->INFO_PRINTF1(KAddOperation);
	TRAPD(err, iEntryView->StoreL(iModifier->iEntryArray, entriesStored));
	if(err == KErrNone)
		{
		iTestStep->INFO_PRINTF1(KInfoEntryStored);
		}
	else
		{
		iTestStep->ERR_PRINTF2(KErrAddOperation, err);
		iTestStep->SetTestStepResult(EFail);
		}
	}

/**
* Update entries 
* Care is taken to ensure that updateL is not performed on the Child Entries
*/
void CTestCalInterimApiModifier::UpdateEntriesL()
	{
	RPointerArray<CCalEntry> updatedEntries;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &updatedEntries));
	const TInt KUidCount(iModifier->iEntryArray.Count());

	for (TInt j(0) ; j < KUidCount ; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
		TCalTime recurrenceId = iModifier->iEntryArray[j]->RecurrenceIdL();
		TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
		TTime recurNullTime = Time::NullTTime();

		if(recurrenceIdInTTime != recurNullTime)
			{
			continue;
			}

		TPtrC8 desiredGuid(iModifier->iEntryArray[j]->UidL());
		iEntryView->FetchL(desiredGuid, calEntries);

		// update of entries can be performed only on the parent entry
		calEntries[0]->SetLastModifiedDateL();
		updatedEntries.Append(calEntries[0]);
		calEntries.Reset();
		CleanupStack::Pop(&calEntries);
		}
		
	TInt numSuccessful(0);
	iTestStep->INFO_PRINTF1(KModifyOperation);
	TRAPD(err, iEntryView->UpdateL(updatedEntries, numSuccessful));
	TESTL( numSuccessful == updatedEntries.Count() );
	if(err != KErrNone)
		{
		iTestStep->ERR_PRINTF2(KErrUpdateOperation,err);
		iTestStep->SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(); //UpdateEntries
	}

/**
* Delete entries by uid list 
*/
void CTestCalInterimApiModifier::DeleteEntriesByUidListL()
	{
	CDesC8ArrayFlat* deleteGuidArray = new(ELeave) CDesC8ArrayFlat(4);
	CleanupStack::PushL(deleteGuidArray);
	const TInt KUidCount(iModifier->iEntryArray.Count());
	for (TInt j(0); j < KUidCount; ++j)
		{
		if(iModifier->iEntryArray[j]->RecurrenceIdL().TimeUtcL() == Time::NullTTime())
			{
			// Deleting a Parent entry deletes all the associated child entries
			// Hence bypasing all child entries
			deleteGuidArray->AppendL(iModifier->iEntryArray[j]->UidL());
			}
		}
	TRAPD(err, iEntryView->DeleteL(*deleteGuidArray));
	CleanupStack::PopAndDestroy(deleteGuidArray);

	if((err != KErrNone))
		{
		iTestStep->ERR_PRINTF2(KErrDeleteOperation,err);
		iTestStep->SetTestStepResult(EFail);
		}
	}
/**
* Delete entries by local uids
*/
void CTestCalInterimApiModifier::DeleteEntriesByArrayOfLocalUidsL()
	{
	RArray<TCalLocalUid> localUids;
	CleanupClosePushL(localUids);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	iEntryView->GetIdsModifiedSinceDateL(minTime, localUids);
	TInt numSuccessfulDeleted(0);
	TRAPD(err, iEntryView->DeleteL(localUids, numSuccessfulDeleted));
	if(err != KErrNone)
		{
		iTestStep->ERR_PRINTF2(KErrDeleteOperation,err);
		iTestStep->SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&localUids); // Close()
	}

/**
* Delete entries by reference 
*/
void CTestCalInterimApiModifier::DeleteEntriesByReferenceL()
	{
	const TUint KGuidCount =  iGuidList->Count();
	for (TInt j(0); j < KGuidCount; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupClosePushL(calEntries);
		iEntryView->FetchL(iGuidList->MdcaPoint(j), calEntries);

		const TUint KCalendarEntriesSpecificToGuid 	= calEntries.Count();
		if(KCalendarEntriesSpecificToGuid != 0)
			{
			iTestStep->INFO_PRINTF1(KDeleteOperation);
			// Deleting a Parent entry deletes all the associated child entries
			// Hence bypasing all child entries
			CCalEntry* calEntry = calEntries[0];
			TRAPD(err, iEntryView->DeleteL(*calEntry));
			if(err != KErrNone)
				{
				iTestStep->ERR_PRINTF2(KErrDeleteOperation,err);
				iTestStep->SetTestStepResult(EFail);
				}
			}
		CleanupStack::PopAndDestroy(&calEntries);
		}
	}


/**
* Delete entries in the calendar file over a wider time Range
*/
void CTestCalInterimApiModifier::DeleteEntriesByTimeRangeL()
	{
	TPtrC timeMode;
	GetStringFromConfig(ConfigSection(), KFilterTimeMode, timeMode);

	TPtrC deleteFilterStartTime;
	GetStringFromConfig(ConfigSection(), KDeleteFilterStartTime, deleteFilterStartTime);
	TCalTime	calStartDateTime;

	if(deleteFilterStartTime != KNullDesC())
		{
		// Convert TPtrC to TTime
		TTime	startTTime;
		startTTime.Set(deleteFilterStartTime);

		if(!timeMode.Compare(KFilterTimeModeLocal))
			{
			calStartDateTime.SetTimeLocalL(startTTime);
			}
		else if (!timeMode.Compare(KFilterTimeModeLocalFloating))
			{
			calStartDateTime.SetTimeLocalFloatingL(startTTime);
			}
		else
			{
			calStartDateTime.SetTimeUtcL(startTTime);
			}
		}
	else
		{
		calStartDateTime.SetTimeLocalL(TCalTime::MinTime());
		}


	TCalTime	calEndDateTime;
	TPtrC  deleteFilterEndTime;
	GetStringFromConfig(ConfigSection(), KDeleteFilterEndTime, deleteFilterEndTime);

	if(deleteFilterEndTime != KNullDesC())
		{
		// Convert TPtrC to TTime
		TTime	endTTime;
		endTTime.Set(deleteFilterEndTime);

		if(!timeMode.Compare(KFilterTimeModeLocal))
			{
			calEndDateTime.SetTimeLocalL(endTTime);
			}
		else if (!timeMode.Compare(KFilterTimeModeLocalFloating))
			{
			calEndDateTime.SetTimeLocalFloatingL(endTTime);
			}
		else
			{
			calEndDateTime.SetTimeUtcL(endTTime);
			}
		}
	else
		{
		calEndDateTime.SetTimeLocalL(TCalTime::MaxTime());
		}

	CalCommon::TCalTimeRange	calTimeRange(calStartDateTime, calEndDateTime);
	TRAPD(err, iEntryView->DeleteL(calTimeRange, CalCommon::EIncludeAll, *this));
	CActiveScheduler::Start();

	if(err != KErrNone)
		{
		iTestStep->ERR_PRINTF2(KErrDeleteOperation,err);
		iTestStep->SetTestStepResult(EFail);
		}
	}

/**
* Delete all Instances in the calendar file 
*/
void CTestCalInterimApiModifier::DeleteInstancesL()
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

	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
	instanceView->FindInstanceL(instanceArray, CalCommon::EIncludeAll, calTimeRange);
	if(instanceArray.Count() != 0)
		{
		TRAP(err, instanceView->DeleteL(instanceArray[0], CalCommon::EThisAndAll));
		instanceArray.Remove(0);
		if(err != KErrNone)
			{
			iTestStep->ERR_PRINTF2(KErrDeleteOperation,err);
			iTestStep->SetTestStepResult(EFail);
			}
		}
	else
		{
		iTestStep->WARN_PRINTF1(KWarnInstancesNotFound);
		}
		
	CleanupStack::PopAndDestroy();// instanceArray
	CleanupStack::PopAndDestroy(); // instanceView
	}
	
/**
* Populates a CDescArray with guids of all the entries in the Calendar file
*/
void CTestCalInterimApiModifier::PopulateEntriesGuidListL()
	{
	TInt	count = 0;
	iEntryIterator=CCalIter::NewL(GetSession());
	iGuidList = new (ELeave) CDesC8ArrayFlat(4);
	TPtrC8	uid(iEntryIterator->FirstL());

	if(KNullDesC8()!=uid)
		{
		iGuidList->AppendL(uid);
		count++;
		}

	while (KNullDesC8()!=uid)
		{
		iGuidList->AppendL(uid);
		uid.Set(iEntryIterator->NextL());
		count++;
		}
	}
	
TVerdict CTestCalInterimApiModifier::doTestStepL()
	{
	return TestStepResult();	
	}
	
/**
* Function which returns the semaphore name
*/
TPtrC& CTestCalInterimApiModifier::ModifierSemaphoreName()
	{
	return (iModifier->iSemName);
	}
	
/**
* Modifier Thread entry point
*/
GLDEF_C TInt RunModifierThread(TAny* aArgs)
    {
	TInt err;
	CTestCalInterimApiModifier* modifierThread = NULL;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CTestStep* iTestStep = static_cast<CTestStep*>(aArgs);
	CActiveScheduler*	scheduler = new(ELeave) CActiveScheduler;

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
    	TRAP(err, modifierThread = CTestCalInterimApiModifier::NewL(iTestStep));

    	if (err == KErrNone)
			{
			TRAP(err, modifierThread->RunTestsL());
			}
 		RSemaphore semaphoreStop;
		const TPtrC& semaphoreName = modifierThread->ModifierSemaphoreName();
		semaphoreStop.OpenGlobal(semaphoreName);

		delete modifierThread;
		modifierThread = NULL;
		delete scheduler;
		scheduler = NULL;
		delete trapCleanup;
		trapCleanup = NULL;
		
		semaphoreStop.Signal();
		semaphoreStop.Close();
		}
	
	return err;
	}

