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

#include "TestCalInterimApiNotifier.h"

CTestCalInterimApiNotifier::CTestCalInterimApiNotifier()
/**
 * Constructor
 */
	{
	iTestStep 				 = NULL;
	iSemName  				 = NULL;
	iSession 				 = NULL;
	iVerification			 = ETrue;
	}

CTestCalInterimApiNotifier::~CTestCalInterimApiNotifier()
/**
 * Destructor
 */
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iSession;
	}

CTestCalInterimApiNotifier* CTestCalInterimApiNotifier::NewL(CTestStep* aTestStep)
	{
	CTestCalInterimApiNotifier* self = new(ELeave) CTestCalInterimApiNotifier;
	CleanupStack::PushL(self);
	self->ConstructL(aTestStep);
	CleanupStack::Pop(self);
	return self;
	}

void CTestCalInterimApiNotifier::ConstructL(CTestStep* aTestStep)
	{
	iTestStep = aTestStep;
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	
	// Type casting to share the main thread's member variables across this thread
	iMainThread = static_cast<CTestCalInterimApiMainThread*>(iTestStep);
	iSession = CCalSession::NewL();
	iSession->OpenL(iMainThread->iCalenderFileName);
	User::LeaveIfError(iSemaphore.OpenGlobal(iMainThread->iSemName));
	}

/**
* Function which registers with the notifier and suspends the current thread
@param aNotifier Flag to register the session with the notifier as per the requirement
*/
void CTestCalInterimApiNotifier::NextStep(const TBool& aNotifier)
	{
	// Start the Cal Change notification as per the requirement
	
	if(aNotifier)
		{
		iSession->StartChangeNotification(*this, *iFilter);
		}

	/* Notifier thread signals the main thread after enabling or disabling Notifier
	   and suspends the current thread. In this period the modifier thread performs some
	   operation like add, modify & delete of Calendar entries and resumes the notifier thread
	   to receive notifications, if Any */
	iSemaphore.Signal();
	RThread().Suspend();
	
    /* A timer notification request for a duration of second is Issues,
       Cal Change Notification if Any, is expected within this timeout period of 1 second.
       In the event of No Cal Change Notifications being received in the duration
       the Callback function StopWaitingForNotification is invoked */
	do	{
		iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
		iNotified = EFalse;
		CActiveScheduler::Start(); // wait for notification
		}
	while (iNotified);
	}

/**
* Function which creates the notification filter and calls the subroutine NextStep to register
* the session with the calendar server for notifications
*/
void CTestCalInterimApiNotifier::RunTestL()
	{
	CreateNotificationFilterL();
	
	iTestStep->GetIntFromConfig(iTestStep->ConfigSection(), KNumOfExpectedNotifications, iNumOfExpectedNotifications );
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KVerification, iVerification);
	
	TBool	notifier;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KNotifier, notifier );
	
	// Get the expected changetype and entrytype and populate them in the array
	TBuf<KMaxTestExecuteCommandLength>	tempStore;
	TInt	changeType;
	TInt	index = 0;
	TBool	moreData = ETrue;
	while ( moreData )
		{
		tempStore.Format(KChangeType(), ++index);
		iTestStep->INFO_PRINTF2(KInfoIndex, index);
		moreData = iTestStep->GetIntFromConfig(iTestStep->ConfigSection(), tempStore, changeType);
		iChangeTypeArray.AppendL(changeType);
		if ( moreData )
			{
			TInt	entryType;
			tempStore.Format(KEntryType(), index);
			iTestStep->GetIntFromConfig(iTestStep->ConfigSection(), tempStore, entryType);
			iEntryTypeArray.AppendL(entryType);
			}
		}
		
	// Registers with the notifier based on the requirement
	NextStep(notifier);
	
	// Stops the notifier after the notification is received
	iSession->StopChangeNotification();
	}

/**
* Function which stops the notifier
*/
TInt CTestCalInterimApiNotifier::StopWaitingForNotification(TAny* aPtr)
	{
	CTestCalInterimApiNotifier* notifier = static_cast<CTestCalInterimApiNotifier*>(aPtr);
	if (notifier)
		{
		notifier->DoStopWaitingForNotification();
		}
	CActiveScheduler::Stop();
	return KErrNone;
	}

/**
* Function which cancels the timer if the  notification is not received within the time limit
*/
void CTestCalInterimApiNotifier::DoStopWaitingForNotification()
	{
	iTimer->Cancel();
	}

/**
* Function which gets the filter details from the ini file and sets the notification filter
* time range
*/
void CTestCalInterimApiNotifier::CreateNotificationFilterL()
	{
	TPtrC	filterEntryType;
	iTestStep->TESTL( iTestStep->GetStringFromConfig(iTestStep->ConfigSection(), KFilterEntryType, filterEntryType) );
	TPtrC	filterStartTime;
	iTestStep->TESTL( iTestStep->GetStringFromConfig(iTestStep->ConfigSection(), KFilterStartTime, filterStartTime) );
	TPtrC	filterEndTime;
	iTestStep->TESTL( iTestStep->GetStringFromConfig(iTestStep->ConfigSection(), KFilterEndTime, filterEndTime) );
	TBool	undatedTodos;
	iTestStep->GetBoolFromConfig(iTestStep->ConfigSection(), KUndatedTodos, undatedTodos);
	TPtrC	timeMode;
	iTestStep->GetStringFromConfig(iTestStep->ConfigSection(), KFilterTimeMode, timeMode);
	
	// Convert TPtrC to TTime
	TTime	startTTime;
	startTTime.Set(filterStartTime);
	TTime	endTTime;
	endTTime.Set(filterEndTime);

	// Convert TTime to TCalTime
	TCalTime	startCalTime;
	TCalTime	endCalTime;

	if( !timeMode.Compare(KFilterTimeModeLocal) )
		{
		startCalTime.SetTimeLocalL(startTTime);
		endCalTime.SetTimeLocalL(endTTime);
		}
	else if ( !timeMode.Compare(KFilterTimeModeLocalFloating) )
		{
		startCalTime.SetTimeLocalFloatingL(startTTime);
		endCalTime.SetTimeLocalFloatingL(endTTime);
		}
	else
		{
		startCalTime.SetTimeUtcL(startTTime);
		endCalTime.SetTimeUtcL(endTTime);
		}

	MCalChangeCallBack2::TChangeEntryType  calFilter = GetFilterEntryType(filterEntryType);
	CalCommon::TCalTimeRange calTimeRange(startCalTime, endCalTime);

	// Building the CalChangeNotification Filter to be used in the Observer
	iFilter = CCalChangeNotificationFilter::NewL(calFilter, undatedTodos, calTimeRange);
	}
	
/**
* Maps the Ini input to the desired filter type
@param aFilter - String filter type input from the Ini file
@return Equivalent MCalChangeCallBack2::TChangeEntryType corresponding to the Ini data
*/
MCalChangeCallBack2::TChangeEntryType  CTestCalInterimApiNotifier::GetFilterEntryType(const TPtrC& aFilter)
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
* A call back function. It returns an array containing information about at least one change to the database.
@param aChangeItems array containing information about the type change and the
*	 identity of the item being changed
*/
void CTestCalInterimApiNotifier::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
	{
	iNotified = ETrue;
	iTimer->Cancel();
	CActiveScheduler::Stop();

	// Verify if the number of notifications received is equal to the number expected
	if ( aChangeItems.Count() == iNumOfExpectedNotifications )
		{
		iTestStep->INFO_PRINTF3(KInfoCalChangeNotification, aChangeItems.Count(), iNumOfExpectedNotifications);
		// for each notification we have recieved
		// make sure the correct details are in our expected changes
		if ( iVerification )
			{
			for (TInt j(0); j < iNumOfExpectedNotifications; ++j)
				{
				iTestStep->INFO_PRINTF3(KInfoReceivedAndExpectedEntryType, aChangeItems[j].iEntryType, iEntryTypeArray[j]);
				iTestStep->INFO_PRINTF3(KInfoReceivedAndExpectedChangeType, aChangeItems[j].iChangeType, iChangeTypeArray[j]);

				/* If the changetype and entry type does not match with the expected
				   then Set the teststepresult to fail*/
				if ( (aChangeItems[j].iChangeType != iChangeTypeArray[j])
					 || (aChangeItems[j].iEntryType != iEntryTypeArray[j]) )
					{
					iTestStep->ERR_PRINTF1(KErrInvalidNotifications);
					iTestStep->SetTestStepResult(EFail);
					}
				}
			}
		}
	else
		{
		iTestStep->ERR_PRINTF3(KErrCalChangeNotification, aChangeItems.Count(), iNumOfExpectedNotifications);
		iTestStep->SetTestStepResult(EFail);
		}
	}

/**
* Function which returns the semaphore name
*/
TPtrC& CTestCalInterimApiNotifier::SemaphoreName()
	{
	return iMainThread->iSemName;
	}

/**
* Notifier Thread Entry Point
*/
GLDEF_C TInt RunNotifierThread(TAny* aArgs)
    {
	TInt err;
	CTestCalInterimApiNotifier* notifier = NULL;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CTestStep* testStep = static_cast<CTestStep*>(aArgs);
	CActiveScheduler*	scheduler = new(ELeave) CActiveScheduler;

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
    	TRAP(err, notifier = CTestCalInterimApiNotifier::NewL(testStep));

    	if (err == KErrNone)
			{
			TRAP(err, notifier->RunTestL());
			}
			
		RSemaphore semaphoreStop;
		const TPtrC& semaphoreName = notifier->SemaphoreName();
		semaphoreStop.OpenGlobal(semaphoreName);

		delete notifier;
		notifier = NULL;
		delete scheduler;
		scheduler = NULL;
		delete trapCleanup;
		trapCleanup = NULL;
		
		semaphoreStop.Signal();
		semaphoreStop.Close();
		}
	return err;
	}

