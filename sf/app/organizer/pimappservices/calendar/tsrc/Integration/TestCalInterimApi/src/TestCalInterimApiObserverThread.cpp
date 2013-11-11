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


#include "TestCalInterimApiObserverThread.h"

CTestCalInterimApiObserverThread::CTestCalInterimApiObserverThread()
/**
 * Constructor
 */
	{
	}

CTestCalInterimApiObserverThread::~CTestCalInterimApiObserverThread()
/**
 * Destructor
 */
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	iChunk.Close();
	iSemaphore.Close();
	delete iSession;
	}

CTestCalInterimApiObserverThread* CTestCalInterimApiObserverThread::NewL(CTestStep* aTestStep)
	{
	CTestCalInterimApiObserverThread* self = new(ELeave) CTestCalInterimApiObserverThread;
	CleanupStack::PushL(self);
	self->ConstructL(aTestStep);
	CleanupStack::Pop(self);
	return self;
	}

void CTestCalInterimApiObserverThread::ConstructL(CTestStep* aTestStep)
	{
	//	aTestStep - pointer to current Test Step, used for printing and validation purpose
	iTestStep = 	static_cast<CTestCalInterimApiModifierThread*>(aTestStep);
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	iChunk.OpenGlobal(KChunkName(), EFalse);
	RefreshData();
	
	//	Setting the pointer member variable to appropriate location in the Global chunk
	iExpectedChanges	 	 = reinterpret_cast<TCalChangeEntry*>(iChunk.Base());

	// A seperate session with the Calendar file is necessary to receive the notifications
	iSession = CCalSession::NewL();
	iSession->OpenL(iFileName);
	iSemaphore.OpenGlobal(iSemName);
	}


void CTestCalInterimApiObserverThread::RefreshData()
	{
	iNumExpectedChanges = iTestStep->iNumExpectedChanges; 
	iFilter = iTestStep->iFilter;
	iFileName = iTestStep->iFileName;
	iSemName.Set(iTestStep->iSemName); 
	iStopChangeNotification = iTestStep->iStopChangeNotification;
	iExecutionCompleted = iTestStep->iExecutionCompleted;
	iNotifierOff = iTestStep->iNotifierOff;
	}
	
void CTestCalInterimApiObserverThread::NextStepL()
	{

	// Start or Stop the Cal Change Notifier as per the requirement
	if(iStopChangeNotification == 1)
		{
		iSession->StopChangeNotification();
		}
	else
		{
		if(iNotifierOff)
			{
			iSession->StartChangeNotification(*this, *iFilter);
			iNotifierOff = EFalse;
			}
		}
	/* Observer thread signals the modifier thread after enabling or disabling Notifier
	   and suspends the current thread. In this period the modifier thread performs some
	   operation like add, modify & delete of Calendar entries and resumes the observer thread
	   to receive notifications, if Any */
	iSemaphore.Signal();
	RThread().Suspend();
	RefreshData();
	
	
	if (iExecutionCompleted)
		{
		return;
		}


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

void CTestCalInterimApiObserverThread::RunTestL()
	{
	while (!(iExecutionCompleted))
		{
		NextStepL();
		}
	iSession->StopChangeNotification();
	}

TInt CTestCalInterimApiObserverThread::StopWaitingForNotification(TAny* aPtr)
	{
	CTestCalInterimApiObserverThread* observer = static_cast<CTestCalInterimApiObserverThread*>(aPtr);
	if (observer)
		{
		observer->DoStopWaitingForNotification();
		}
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CTestCalInterimApiObserverThread::DoStopWaitingForNotification()
	{
	iTimer->Cancel();
	}

/**
* A call back function. It returns an array containing information about at least one change to the database.
@param aChangeItems array containing information about the type change and the
*		 identity of the item being changed
*/

void CTestCalInterimApiObserverThread::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
	{
	iTestStep->INFO_PRINTF2(KInfoCalChangeNotification, aChangeItems.Count());
	iTestStep->INFO_PRINTF2(KNumberOfNotificationsExpected, iNumExpectedChanges);

	iNotified = ETrue;
	iTimer->Cancel();
	CActiveScheduler::Stop();



	for (TInt i(0); i < aChangeItems.Count(); ++i)
		{
		iTestStep->INFO_PRINTF4(KInfoCalChangeNotificationCallbackData, aChangeItems[i].iEntryId, aChangeItems[i].iEntryType, aChangeItems[i].iChangeType);
		}

	for (TInt i(0); i < aChangeItems.Count(); ++i)
		{
		iTestStep->INFO_PRINTF4(KInfoDesiredCalChangeNotificationData, iExpectedChanges[i].iEntryId, iExpectedChanges[i].iEntryType, iExpectedChanges[i].iChangeType);
		}


	// Test that the total number of notifications so far
	// is not greater than the amount that we are expecting
	if(aChangeItems.Count() > iNumExpectedChanges )
		{
		iTestStep->INFO_PRINTF1(KErrExcessCalChangeNotificationsReceived);
		iTestStep->SetTestStepResult(EFail);
		}

	for (TInt i(0); i < aChangeItems.Count(); ++i)
		{
		iTestStep->INFO_PRINTF4(KInfoCalChangeNotificationCallbackData, aChangeItems[i].iEntryId, aChangeItems[i].iEntryType, aChangeItems[i].iChangeType);
		TBool entryIdFound(EFalse);
		TBool changeTypeCorrect(EFalse);

		// for each notification we have recieved
		// make sure the correct details are in our expected changes
		for (TInt j(0); j < iNumExpectedChanges; ++j)
			{
			if (iExpectedChanges[j].iEntryId == aChangeItems[i].iEntryId)
				{
				entryIdFound = ETrue;
				if	(aChangeItems[i].iChangeType == iExpectedChanges[j].iChangeType)
					{
					iTestStep->INFO_PRINTF4(KInfoDesiredCalChangeNotificationData, iExpectedChanges[j].iEntryId, iExpectedChanges[j].iEntryType, iExpectedChanges[j].iChangeType);
					changeTypeCorrect = ETrue;
					break;
					}
				}
			}
		if(!entryIdFound)
			{
			iTestStep->INFO_PRINTF1(KErrEntryIdNotFound);
			iTestStep->SetTestStepResult(EFail);
			}
		if(!changeTypeCorrect)
			{
			iTestStep->INFO_PRINTF1(KErrInvalidChangeTypeFound);
			iTestStep->SetTestStepResult(EFail);
			}
		}

	}

TPtrC& CTestCalInterimApiObserverThread::SemaphoreName()
	{
	return (iSemName);
	}


// Observer thread entry point
GLDEF_C TInt RunObserverThread(TAny* aArgs)
    {
	TInt err;
	CTestCalInterimApiObserverThread* observer = NULL;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CTestStep* iTestStep = static_cast<CTestStep*>(aArgs);
	CActiveScheduler*	scheduler = new(ELeave) CActiveScheduler;

	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);
    	TRAP(err, observer = CTestCalInterimApiObserverThread::NewL(iTestStep));

    	if (err == KErrNone)
			{
			TRAP(err, observer->RunTestL());
			}

		RSemaphore semaphoreStop;
		const TPtrC& semaphoreName = observer->SemaphoreName();
		semaphoreStop.OpenGlobal(semaphoreName);

		delete observer;
		delete scheduler;
		delete trapCleanup;

		semaphoreStop.Signal();
		semaphoreStop.Close();
		}

	return err;
	}
