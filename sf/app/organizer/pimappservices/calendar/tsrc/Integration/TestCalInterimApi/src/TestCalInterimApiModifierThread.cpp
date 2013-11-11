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

#include "TestCalInterimApiModifierThread.h"

/**
 * Constructor
 */
CTestCalInterimApiModifierThread::CTestCalInterimApiModifierThread()
:CTestCalInterimApiReadData()
, iNumExpectedChanges(0)
, iStopChangeNotification(EFalse) 
, iExecutionCompleted(EFalse)
, iNotifierOff(ETrue)
	{
	SetTestStepName(KTestCalInterimApiModifierThread);
	}

CTestCalInterimApiModifierThread::~CTestCalInterimApiModifierThread()
/**
 * Destructor
 */
	{
	iSemaphore.Close();
	iThread.Close();
	iChunk.Close();
	}

/**
* Virtual function called while parsing the entries from the data file
* Takes ownership of aEntry
@param aEntry - current entry being parsed
*/
void CTestCalInterimApiModifierThread::ProcessEntryL(CCalEntry* aEntry)
	{
	iEntryArray.AppendL(aEntry);
	}

/**
* Creates an Observer Thread
* The Observer Thread receives and validates notifications
*/

void CTestCalInterimApiModifierThread::LaunchObserverThreadL()
	{
    TRAPD(err, iThread.Create(KThreadName(), RunObserverThread, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, this, EOwnerProcess));
    if(err != KErrNone)
    	{
    	INFO_PRINTF2(KErrObserverThread, err);
    	User::Leave(err);
    	}
   	INFO_PRINTF1(KInfoObserverThread);
	}

void CTestCalInterimApiModifierThread::CreateGlobalChunkL()
	{
	const TUint chunkSize = sizeof(TCalChangeEntry) * 6000;
	iChunk.CreateGlobal(KChunkName(), chunkSize, chunkSize);
	iChunk.OpenGlobal(KChunkName(), ETrue);
	iExpectedChanges = reinterpret_cast<TCalChangeEntry*>(iChunk.Base());
	}

void CTestCalInterimApiModifierThread::CreateNotificationFilterL()
	{
	TPtrC filterEntryType;
	TESTL( GetStringFromConfig(ConfigSection(), KFilterEntryType, filterEntryType) );
	TPtrC filterStartTime;
	TESTL( GetStringFromConfig(ConfigSection(), KFilterStartTime, filterStartTime) );
	TPtrC  filterEndTime;
	TESTL( GetStringFromConfig(ConfigSection(), KFilterEndTime, filterEndTime) );
	TBool undatedTodos;
	GetBoolFromConfig(ConfigSection(), KUndatedTodos, undatedTodos);
	TPtrC timeMode;
	GetStringFromConfig(ConfigSection(), KFilterTimeMode, timeMode);

	// Convert TPtrC to TTime
	TTime	startTTime;
	startTTime.Set(filterStartTime);
	TTime	endTTime;
	endTTime.Set(filterEndTime);

	// Convert TTime to TCalTime
	TCalTime	startCalTime;
	TCalTime	endCalTime;

	if(!timeMode.Compare(KFilterTimeModeLocal))
		{
		startCalTime.SetTimeLocalL(startTTime);
		endCalTime.SetTimeLocalL(endTTime);
		}
	else if (!timeMode.Compare(KFilterTimeModeLocalFloating))
		{
		startCalTime.SetTimeLocalFloatingL(startTTime);
		endCalTime.SetTimeLocalFloatingL(endTTime);
		}
	else
		{
		startCalTime.SetTimeUtcL(startTTime);
		endCalTime.SetTimeUtcL(endTTime);
		}

	MCalChangeCallBack2::TChangeEntryType  calFilter=GetFilterEntryType(filterEntryType);
	CalCommon::TCalTimeRange calTimeRange(startCalTime,endCalTime);

	// Building the CalChangeNotification Filter to be used in the Observer
	 iFilter = CCalChangeNotificationFilter::NewL(calFilter, undatedTodos, calTimeRange);
	}

void CTestCalInterimApiModifierThread::RetreiveIniDataL()
	{
	// Calendar File
	iFileName = iCalenderFileName;

	// Data file
	TPtrC	dataFile;
	TESTL( GetStringFromConfig(ConfigSection(), KDataFile, dataFile) );
	TFileName dataFileName;
#ifdef __WINS__
	dataFileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	dataFileName.Append(parse.Drive());
#endif
	dataFileName.Append(dataFile);

	INFO_PRINTF2(KInfoDatafile, &dataFileName);
	OpenAgendaDataFileL(dataFileName);

	// Creating a Global Semaphore used to synchronize the Modifier & Observer thread
	TESTL( GetStringFromConfig(ConfigSection(), KSemaphoreName, iSemName) );

	TInt error = iSemaphore.CreateGlobal(iSemName, 0);

	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSemaphore.OpenGlobal(iSemName);
		}
	else
		{
		User::Leave(error);
		}
	}

void CTestCalInterimApiModifierThread::StartNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType)
	{
	TBool startNotifier = EFalse;
	GetBoolFromConfig(ConfigSection(), KStartNotifier, startNotifier);
	if(startNotifier)
		{
		StartNotifierInObserverThread(ETrue);
		/* Resume the Observer Thread and thereby enable the observer thread to register with the calendar server
		for Notifications */
		ResumeObserverThread();
		ReadAddEntriesL();
		TBool notificationExpected = ETrue;
		TBool disableChangeBroadCast = EFalse;
		AddModifyDeleteEntriesL(aInRangeFlag, notificationExpected, disableChangeBroadCast, aDeleteType);
		iEntryArray.ResetAndDestroy();
		iEntryArray.Close();
		}
	}

void CTestCalInterimApiModifierThread::StopNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType)
	{
	TBool stopNotifier = EFalse;
	GetBoolFromConfig(ConfigSection(), KStopNotifier, stopNotifier);
	if(stopNotifier)
		{
		StartNotifierInObserverThread(EFalse);
		/* Resume the Observer Thread and thereby enable the observer thread to register with the calendar server
		for Notifications */
		ResumeObserverThread();
		ReadAddEntriesL();
		TBool notificationExpected = EFalse;
		TBool disableChangeBroadCast = EFalse;
		AddModifyDeleteEntriesL(aInRangeFlag, notificationExpected, disableChangeBroadCast, aDeleteType);
		iEntryArray.ResetAndDestroy();
		iEntryArray.Close();
		}
	}

void CTestCalInterimApiModifierThread::ReEnableNotifierAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType)
	{
	TBool reEnableNotifier = EFalse;
	GetBoolFromConfig(ConfigSection(), KReEnableNotifier, reEnableNotifier);
	if(reEnableNotifier)
		{
		StartNotifierInObserverThread(ETrue);
		/* Resume the Observer Thread and thereby enable the observer thread to register with the calendar server
		for Notifications */
		ResumeObserverThread();
		ReadAddEntriesL();
		TBool notificationExpected = ETrue;
		TBool disableChangeBroadCast = EFalse;
		AddModifyDeleteEntriesL(aInRangeFlag, notificationExpected, disableChangeBroadCast, aDeleteType);
		iEntryArray.ResetAndDestroy();
		iEntryArray.Close();
		}
	}

void CTestCalInterimApiModifierThread::DisableBroadCastAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType)
	{
	TBool disableBroadCast = EFalse;
	GetBoolFromConfig(ConfigSection(), KDisableBroadCast, disableBroadCast);
	if(disableBroadCast)
		{
		/* Ensure that the Notifier is still enabled in the Observer Thread.
		   Disable all notifications from changes made within this session.
		   Perform add, modify and delete of calendar entries
		   and validate that no cal change notifications received in the observer thread */
		StartNotifierInObserverThread(ETrue);
		/* Resume the Observer Thread and thereby enable the observer thread to register with the calendar server
		for Notifications */
		ResumeObserverThread();
		AlterCalChangeBroadcast(EFalse);
		ReadAddEntriesL();
		TBool notificationExpected = EFalse;
		TBool disableChangeBroadCast = EFalse;
		AddModifyDeleteEntriesL(aInRangeFlag, notificationExpected, disableChangeBroadCast, aDeleteType);
		iEntryArray.ResetAndDestroy();
		iEntryArray.Close();
		}
	}


void CTestCalInterimApiModifierThread::EnableBroadCastAndTestL(const TBool& aInRangeFlag, const TPtrC& aDeleteType)
	{
	TBool enableBroadCast = EFalse;
	GetBoolFromConfig(ConfigSection(), KEnableBroadCast, enableBroadCast);
	if(enableBroadCast)
		{
		/* Ensure that the Notifier is still enabled in the Observer Thread.
		   Re-enable all notifications from changes made within this session.
		   Perform add, modify and delete of calendar entries
		   and validate that the cal change notifications received in the observer thread
		   look for that extra notification which wit change type as and change entry type as
		   notified as part changes done to the calendar file during DisableCalChangeBroadcast */
		StartNotifierInObserverThread(ETrue);
		/* Resume the Observer Thread and thereby enable the observer thread to register with the calendar server
		for Notifications */
		ResumeObserverThread();
		AlterCalChangeBroadcast(ETrue);
		ReadAddEntriesL();
		TBool notificationExpected = ETrue;
		TBool disableChangeBroadCast = ETrue;
		AddModifyDeleteEntriesL(aInRangeFlag, notificationExpected, disableChangeBroadCast, aDeleteType);
		iEntryArray.ResetAndDestroy();
		iEntryArray.Close();
		}
	}


void CTestCalInterimApiModifierThread::StopExecution()
	{
	/* Set the test execution to True
	   It is used to stop the observer thread */
	iExecutionCompleted = ETrue;
	ResumeObserverThread();
	}

TVerdict CTestCalInterimApiModifierThread::doTestStepL()
	{
	// Share the current teststep logger across threads
	Logger().ShareAuto();

	// Creates a CalChangeNotification thread observer thread
	LaunchObserverThreadL();

	// Creates a global chunk i.e. it is visible to all processes and all the threads within the process
	CreateGlobalChunkL();

	// Retreiveing information from the Ini
	RetreiveIniDataL();

	// Create Notification Filter
	CreateNotificationFilterL();

	iEntryView=CCalEntryView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	/* inRangeFlag specifies the changes being done i.e. add, modify and delete of entries are in the filter range
   		or not */
	TBool inRangeFlag;
	TESTL(GetBoolFromConfig(ConfigSection(), KInRangeFlag, inRangeFlag));

	/* inRangeFlag specifies the changes being done i.e. add, modify and delete of entries are in the filter range
   		or not */
	TPtrC deleteType;
	GetStringFromConfig(ConfigSection(), KDeleteType, deleteType);

	/* Enable the notifier in the observer Thread and perform add, modify and delete of calendar entries
  	   and validate the cal change notifications received in the observer thread */
	StartNotifierAndTestL(inRangeFlag, deleteType);

	/* Disable the notifier in the observer Thread and perform add, modify and delete of calendar entries
	   and check the no cal change notifications are received in the observer thread */
	StopNotifierAndTestL(inRangeFlag, deleteType);

	/* Re-Enable the notifier in the observer Thread and perform add, modify and delete of calendar entries
	   and validate the cal change notifications received in the observer thread */
	ReEnableNotifierAndTestL(inRangeFlag, deleteType);

	/* Disable the broadCast of Cal Change Notifications & perform add modify & deletion of entries
	   Ensure that the notifier is still enabled in the observer thread. Validate that no notifications are recived in the
	   observer thread */
	DisableBroadCastAndTestL(inRangeFlag, deleteType);

	/* Enable the broadcast of Cal Change Notifications and validate the cal change notifications received in the observer thread
	   Apart from regular cal change notifications ,check out for an extra notification with Change Entry type as EChangeEntryAll
	   and Change Type as EChangeUndefined */
	EnableBroadCastAndTestL(inRangeFlag, deleteType);

	StopExecution();

	return TestStepResult();
	}


void CTestCalInterimApiModifierThread::AddModifyDeleteEntriesL(const TBool& aInRange, const TBool& aNotificationExpected, const TBool& aDisableChangeBroadCast, const TPtrC& aDeleteFlag)
	{
	ResetExpectedNotificationCount(aNotificationExpected);

	TBool addEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KAddEntriesFlag, addEntriesFlag);
	if(addEntriesFlag)
		{
		AddEntriesL(aInRange, aDisableChangeBroadCast);
	    // Enable the observer thread and validate the notifications
		ResumeObserverThread();
		}

	TBool updateEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KUpdateEntriesFlag, updateEntriesFlag);
	if(updateEntriesFlag)
		{
		UpdateEntriesL(aInRange);
	    // Enable the observer thread and validate the notifications
		ResumeObserverThread();
		}

	TBool deleteEntriesFlag = EFalse;
	GetBoolFromConfig(ConfigSection(), KDeleteEntriesFlag, deleteEntriesFlag);

	if(deleteEntriesFlag)
		{
		if (!aDeleteFlag.Compare(KDeleteByInstances))
			{
			DeleteInstancesL(aInRange);
			}
		else if(!aDeleteFlag.Compare(KDeleteByLocalUids))
			{
			DeleteEntriesByArrayOfLocalUidsL(aInRange);
			}
		else if(!aDeleteFlag.Compare(KDeleteByReference))
			{
			DeleteEntriesByReferenceL(aInRange);
			}
		else if (!aDeleteFlag.Compare(KDeleteByTimeRange))
			{
			DeleteEntriesByTimeRangeL(aInRange);
			}
		else// KDeleteByUid
			{
			DeleteEntriesByUidListL(aInRange);
			}
	    // Enable the observer thread and validate the notifications
		ResumeObserverThread();
		}
	}

/**
* Add entries to the calendar file
* And populate the expected changes notification Array
* @ param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::AddEntriesL(const TBool& aInRange, const TBool& aDisableChangeBroadCast)
	{
	TBool	bulkEntries = EFalse;
	GetBoolFromConfig(ConfigSection(), KBulkEntries, bulkEntries);
	TInt  entriesStored(0);
	iNumExpectedChanges = 0;
	ResetExpectedNotificationArray();
	INFO_PRINTF1(KAddOperation);
	TRAPD(err, iEntryView->StoreL(iEntryArray, entriesStored));
	if(err != KErrNone)
		{
		INFO_PRINTF2(KErrAddOperation,err);
		SetTestStepResult(EFail);
		}

	/* Whenever DisableChangeBroadcast is invoked, changes to the calendar file are not notified.
	   But a separate single notification with change entry type as EChangeEntryAll and Change Type as
	   EChangeUndefined and local uid as 0 will sent to the observers. This notification will be sent
	   after the EnableChangeBroadcast is invoked and some operation is performed on the calendar file.
	   The following code uploads this information into expected notification array */

	if(aDisableChangeBroadCast || bulkEntries)
		{
	   	iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeUndefined;
   		iExpectedChanges[iNumExpectedChanges].iEntryId = 0;
		iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
		++(iNumExpectedChanges);
		}

	if(aInRange)
		{
		for(TInt i = 0; i < iEntryArray.Count(); i++)
			{
			/* While adding an entry to the Calendar, a add notification is sent out.
			corresponding validation data is populated here. */
		 	iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeAdd;
			iExpectedChanges[iNumExpectedChanges].iEntryId = iEntryArray[i]->LocalUidL();
			if (iEntryArray[i]->EntryTypeL() == CCalEntry::ETodo)
				{
				iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
				}
			else
				{
				iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
				}
			++(iNumExpectedChanges);

			/* If a child entry is added apart from regular add entry notification, an update parent entry
			notification is sent. corresponding validation data is populated here.
			If recurrence id of an entry is valid, then an update change notification is added to the expected
			Notification data */

			TCalTime recurrenceId = iEntryArray[i]->RecurrenceIdL();
			TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
			TTime recurNullTime = Time::NullTTime();

			if(recurrenceIdInTTime != recurNullTime)// This loop is meant only for Child Entries
	   			{
				RPointerArray<CCalEntry> calEntries;
				CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
				TPtrC8 calEntryUid = iEntryArray[i]->UidL();
				iEntryView->FetchL(calEntryUid, calEntries);

		    	iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeModify;
	   			iExpectedChanges[iNumExpectedChanges].iEntryId = calEntries[0]->LocalUidL();

				if (calEntries[0]->EntryTypeL() == CCalEntry::ETodo)
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
					}
				else
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
					}
				CleanupStack::PopAndDestroy(); //calEntries
				++(iNumExpectedChanges);
				}
			}
		}
	}

/**
* Update entries and populate the expected changes notification Array
* Care is taken to ensure that updateL is not performed on the Child Entries
* @ param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::UpdateEntriesL(const TBool& aInRange)
	{
	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));
	RPointerArray<CCalEntry> updatedEntries;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &updatedEntries));
	const TInt KUidCount(iEntryArray.Count());
	iNumExpectedChanges = 0;
	ResetExpectedNotificationArray();


	for (TInt j(0) ; j < KUidCount ; ++j)
		{
		RPointerArray<CCalEntry> calEntries;

		TCalTime recurrenceId = iEntryArray[j]->RecurrenceIdL();
		TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
		TTime recurNullTime = Time::NullTTime();

		if(recurrenceIdInTTime != recurNullTime)
			{
			continue;
			}

		TPtrC8 desiredGuid(iEntryArray[j]->UidL());
		iEntryView->FetchL(desiredGuid, calEntries);

		// update of entries can be performed only on the parent entry
		calEntries[0]->SetLastModifiedDateL();
		if (aInRange)
			{
			iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[iNumExpectedChanges].iEntryId = calEntries[0]->LocalUidL();
			if (iEntryArray[j]->EntryTypeL() == CCalEntry::ETodo)
				{
				iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
				}
			else
				{
				iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
				}

			++(iNumExpectedChanges);
        	updatedEntries.Append(calEntries[0]);
			}
		for (TInt k = calEntries.Count()-1; k > -1; k--)
			{
			calEntriesArray.AppendL(calEntries[k]);
			}
		calEntries.Reset();
		}
	TInt numSuccessful(0);
	INFO_PRINTF1(KModifyOperation);
	TRAPD(err, iEntryView->UpdateL(updatedEntries, numSuccessful));
	if(err != KErrNone)
		{
		INFO_PRINTF2(KErrUpdateOperation,err);
		SetTestStepResult(EFail);
		}

	CleanupStack::Pop(); //UpdateEntriesL
	CleanupStack::PopAndDestroy(); //calEntriesArray.ResetAndDestroy();
	}

/**
* Delete entries by uid list and populate the expected changes notification Array
@param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::DeleteEntriesByUidListL(const TBool& aInRange)
	{
	PopulateNotificationOfEntriesBeingDeletedL(aInRange, EFalse);
	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));
	CDesC8ArrayFlat* deleteGuidArray = new(ELeave) CDesC8ArrayFlat(4);
	CleanupStack::PushL(deleteGuidArray);

	for (TInt j(0); j < iEntryArray.Count(); ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		TCalTime recurrenceId = iEntryArray[j]->RecurrenceIdL();
		TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
		TTime recurNullTime = Time::NullTTime();

		if(recurrenceIdInTTime == recurNullTime)
			{
			// Deleting a Parent entry deletes all the associated child entries
			// Hence bypasing all child entries
			TPtrC8 calEntryGuid = iEntryArray[j]->UidL();
			iEntryView->FetchL(calEntryGuid, calEntries);
			for(TInt k = 0; k < calEntries.Count(); k++)
				{
				calEntriesArray.Append(calEntries[k]);
				}
			TPtrC8 deleteCalEntryGuid = calEntries[0]->UidL();
			deleteGuidArray->AppendL(deleteCalEntryGuid);
			calEntries.Reset();
			}
		}

	INFO_PRINTF1(KDeleteOperation);

	TRAPD(err, iEntryView->DeleteL(*deleteGuidArray));

	CleanupStack::Pop(); //deleteGuidArray;
	deleteGuidArray->Reset();
	CleanupStack::PopAndDestroy(); //calEntriesArray.ResetAndDestroy();

	if((err != KErrNone))
		{
		INFO_PRINTF2(KErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	}

/**
* Delete entries by local uids and populate the expected changes notification Array
@param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::DeleteEntriesByArrayOfLocalUidsL(const TBool& aInRange)
	{
	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));
	RArray< TCalLocalUid > deleteLocalUidArray;
	CleanupClosePushL(deleteLocalUidArray);
	iNumExpectedChanges = 0;
	ResetExpectedNotificationArray();

	PopulateNotificationOfEntriesBeingDeletedL(aInRange, ETrue);

	const TInt KUidCount(iEntryArray.Count());
	for (TInt j(0) ; j < KUidCount ; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		TCalTime recurrenceId = iEntryArray[j]->RecurrenceIdL();
		TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
		TTime recurNullTime = Time::NullTTime();
		if(recurrenceIdInTTime != recurNullTime)
			{
			continue;
			}

		TPtrC8 calEntryGuid(iEntryArray[j]->UidL());
		iEntryView->FetchL(calEntryGuid, calEntries);

	for (TInt k = calEntries.Count()-1; k > -1; k--)
		{
		deleteLocalUidArray.AppendL(calEntries[k]->LocalUidL());
		calEntriesArray.AppendL(calEntries[k]);
		}
		calEntries.Reset();
		}
	INFO_PRINTF1(KDeleteOperation);
	TInt numSuccessfulDeleted(0);
	TRAPD(err, iEntryView->DeleteL(deleteLocalUidArray,numSuccessfulDeleted));
	if(err != KErrNone)
		{
		INFO_PRINTF2(KErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&deleteLocalUidArray);
	CleanupStack::PopAndDestroy(&calEntriesArray);
	}


/**
* Delete entries by reference and populate the expected changes notification Array
@param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::DeleteEntriesByReferenceL(const TBool& aInRange)
	{
	PopulateNotificationOfEntriesBeingDeletedL(aInRange, ETrue);
	RPointerArray<CCalEntry> calEntriesArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntriesArray));

	for (TInt j(0); j < iEntryArray.Count(); ++j)
		{
		TCalTime recurrenceId = iEntryArray[j]->RecurrenceIdL();
		TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
		TTime recurNullTime = Time::NullTTime();
		if(recurrenceIdInTTime != recurNullTime)
			{
			continue;
			}

		RPointerArray<CCalEntry> calEntries;
		TPtrC8 calEntryGuid = iEntryArray[j]->UidL();
		iEntryView->FetchL(calEntryGuid, calEntries);

		if(calEntries.Count() == 0)
			{
			continue;
			}

		for (TInt k = calEntries.Count()-1; k > -1; k--)
			{
			calEntriesArray.AppendL(calEntries[k]);
			}

		INFO_PRINTF1(KDeleteOperation);
		for (TInt k = calEntries.Count(); k > 0; k--)
			{
			TInt Index = k - 1;
			CCalEntry* calEntry = calEntries[Index];
			TRAPD(err, iEntryView->DeleteL(*calEntry));
			if(err != KErrNone)
				{
				INFO_PRINTF2(KErrDeleteOperation,err);
				SetTestStepResult(EFail);
				}
			}
			calEntries.Reset();
		}
	CleanupStack::PopAndDestroy(&calEntriesArray);
	}


/**
* Delete entries in the calendar file over a wider time Range and
* populate the expected notification array
@param aInRange - Changes being done are in the Notification Range or Not
*/
void CTestCalInterimApiModifierThread::DeleteEntriesByTimeRangeL(const TBool& aInRange)
	{
	iNumExpectedChanges = 0;
	ResetExpectedNotificationArray();
	if (aInRange)
		{
		iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeUndefined;
		iExpectedChanges[iNumExpectedChanges].iEntryId = 0;
		iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
		++(iNumExpectedChanges);
		}

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
		INFO_PRINTF2(KErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	}

/**
* Delete all Instances in the calendar file and populate the expected notification array,
@param aInRange - Changes being done are in the Notification Range or Not
*/

void CTestCalInterimApiModifierThread::DeleteInstancesL(const TBool& aInRange)
	{
 	PopulateNotificationOfEntriesBeingDeletedL(aInRange, EFalse);

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
			instanceArray.Remove(0);
			}
		else
			{
			CleanupStack::PopAndDestroy();
			break;
			}
		CleanupStack::PopAndDestroy();
		}
	if(err != KErrNone)
		{
		INFO_PRINTF2(KErrDeleteOperation,err);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy();
	}

void CTestCalInterimApiModifierThread::StartNotifierInObserverThread(TBool aStart)
	{	
	if(aStart)
		{
		if(iNotifierOff)
			{
			iStopChangeNotification = 0;
			}		
		}
	else
		{
		iStopChangeNotification = 1;
		iNotifierOff = ETrue;	
		}
	}

void CTestCalInterimApiModifierThread::AlterCalChangeBroadcast(TBool aEnableBroadCast)
	{
	if(aEnableBroadCast == 1)
		{
		GetSession().EnableChangeBroadcast();		
		}
	else		
		{
		GetSession().DisableChangeBroadcast();	
		}
	}


/**
* Reset the expected number of notifications used during StopNotifierInObserverThread
* and DisableCalChangeBroadcast operations
@param aNotificationExpected - whether notifications are expected or not
*/
void CTestCalInterimApiModifierThread::ResetExpectedNotificationCount(const TBool& aNotificationExpected)
	{
	if(aNotificationExpected == EFalse )
		{
		iNumExpectedChanges = 0;
		}
	}

/**
* Maps the Ini input to the desired filter type
@param aFilter - String filter type input from the Ini file
@return Equivalent MCalChangeCallBack2::TChangeEntryType corresponding to the Ini data
*/
MCalChangeCallBack2::TChangeEntryType  CTestCalInterimApiModifierThread::GetFilterEntryType(const TPtrC& aFilter)
	{
	MCalChangeCallBack2::TChangeEntryType	retEntryType = MCalChangeCallBack2::EChangeEntryAll;

	if ( aFilter.Compare(KTypeToDo) == 0 )
		{
		retEntryType=MCalChangeCallBack2::EChangeEntryTodo;
		}
	else if ( aFilter.Compare(KTypeEvent) == 0 )
		{
		retEntryType=MCalChangeCallBack2::EChangeEntryEvent;
		}

	return retEntryType;
	}

/**
* Resumes the Observer Thread
*/
void CTestCalInterimApiModifierThread::ResumeObserverThread()
	{
	iThread.Resume();
	iSemaphore.Wait();
	}

void CTestCalInterimApiModifierThread::ResetExpectedNotificationArray()
	{
	for (TInt j(0); j < iNumExpectedChanges; ++j)
		{
		iExpectedChanges[j].iChangeType = MCalChangeCallBack2::EChangeUndefined;
		iExpectedChanges[j].iEntryId = 0;
		iExpectedChanges[j].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
		}
	}

/**
* Populates the expected notification array out of  delete operation
@param aInRange - Changes being done are in the Notification Range or Not
@param aParentUpdate - Whether a parent modify notification must be added to the
* 		expected notification array or not
@return Calendar Entry Count
*/
void CTestCalInterimApiModifierThread::PopulateNotificationOfEntriesBeingDeletedL(const TBool& aInRange, TBool aParentUpdate)
	{
	iNumExpectedChanges = 0;
	if(aInRange)
		{
		for (TInt j(0) ; j < iEntryArray.Count() ; ++j)
			{
			RPointerArray<CCalEntry> calEntries;
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &calEntries));
			TCalTime recurrenceId = iEntryArray[j]->RecurrenceIdL();
			TTime recurrenceIdInTTime = recurrenceId.TimeUtcL();
			TPtrC8 calEntryGuid = iEntryArray[j]->UidL();
			iEntryView->FetchL(calEntryGuid, calEntries);

			if ((recurrenceId.TimeUtcL() != Time::NullTTime()) || (calEntries.Count() < 1))
				{
				CleanupStack::PopAndDestroy();
				continue;
				}

			for(TInt k = 0; k < calEntries.Count(); k++)
				{
				CCalEntry*	entry = calEntries[k];
				iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeDelete;
				iExpectedChanges[iNumExpectedChanges].iEntryId = entry->LocalUidL();
				if (entry->EntryTypeL() == CCalEntry::ETodo)
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
					}
				else
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
					}
				++(iNumExpectedChanges);
				}

			if((aParentUpdate) && (calEntries.Count() > 1))
				{
				CCalEntry*	entry = calEntries[0];
				iExpectedChanges[iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeModify;
				iExpectedChanges[iNumExpectedChanges].iEntryId = entry->LocalUidL();
				if (entry->EntryTypeL() == CCalEntry::ETodo)
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
					}
				else
					{
					iExpectedChanges[iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
					}
				++(iNumExpectedChanges);
				}
			CleanupStack::PopAndDestroy();
			}
		}
	}
