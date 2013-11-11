// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tcal_observer_manager.h"
#include "tcal_observer_modifier.h"
	
GLDEF_D TInt RunTestThread(TAny* /*aArgs*/);

LOCAL_D const TTimeIntervalMicroSeconds32 KWait1second(1000000); // 1 second
LOCAL_D const TInt KMaxHeapSize = 0x40000;

LOCAL_D RTest test(_L("tcal_observer"));
	
CTestAppManager* CTestAppManager::NewLC()
	{
	CTestAppManager* self = new(ELeave) CTestAppManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}
	
void CTestAppManager::ConstructL()
	{
	CTestApp::ConstructL();
	iCalTestLibrary = CCalTestLibrary::NewL();
	iCalTestLibrary->ReplaceFileL(KCalendarFile());
	iCalTestLibrary->OpenFileL(KCalendarFile());
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	User::LeaveIfError(iThread.Create(KThreadName(), RunTestThread, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess));
	iChunk.CreateGlobal(KChunkName(), sizeof(TInt) + (sizeof(TCalChangeEntry) * 6000), sizeof(TInt) + (sizeof(TCalChangeEntry) * 6000));	
	iChunk.OpenGlobal(KChunkName(), ETrue);
	iNumExpectedChanges = reinterpret_cast<TInt*>(iChunk.Base());
	iExpectedChanges = sizeof(TInt) + reinterpret_cast<TCalChangeEntry*>(iChunk.Base());
	}
	
CTestAppManager::~CTestAppManager()
	{
	delete iCalTestLibrary;
	iThread.Close();
	}
	
TInt CTestAppManager::StopWaitingForNotification(TAny* aPtr)
	{
	CTestAppManager* manager = static_cast<CTestAppManager*>(aPtr);
	if (manager)
		{
		manager->DoStopWaitingForNotification();
		}
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CTestAppManager::DoStopWaitingForNotification()
	{
	test.Printf(_L("No notification received\n"));
	iTimer->Cancel();
	}
	
// callback function for change notification
void CTestAppManager::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
	{
	test.Printf(_L("%d notifications received\n"), aChangeItems.Count());

	iNotified = ETrue;	
	iTimer->Cancel();
	CActiveScheduler::Stop();
	
	// Test that the total number of notifications so far
	// is not greater than the amount that we are expecting
	test( aChangeItems.Count() + iNumChangesFound <= *iNumExpectedChanges );
	TInt count = aChangeItems.Count();
	if(aChangeItems[0].iChangeType != MCalChangeCallBack2::EChangeUndefined)
		{
		for (TInt i(0) ; i < count ; ++i)
			{
			const TCalChangeEntry KActualChange = aChangeItems[i];
			test.Printf(_L("Change From CallBack ==> Entry LocalUid = %d; Entry type = %d; Change type = %d\n"), KActualChange.iEntryId, KActualChange.iEntryType, KActualChange.iChangeType);
			TBool changeTypeCorrect(EFalse);
			
			// for each notification we have received
			// make sure the correct details are in our expected changes  
			for (TInt j(0) ; j < *iNumExpectedChanges ; ++j)
				{
				const TCalChangeEntry KExpectedChange = iExpectedChanges[j];
				
				if (KExpectedChange.iEntryId == KActualChange.iEntryId ||
					KExpectedChange.iEntryId == 0)
					{
					if (KActualChange.iChangeType == KExpectedChange.iChangeType &&
						KActualChange.iEntryType == KExpectedChange.iEntryType)
						{
						test.Printf(_L("Change Found ==> Entry Id = %d; Entry type = %d; Change type = %d\n"), KExpectedChange.iEntryId, KExpectedChange.iEntryType, KExpectedChange.iChangeType);
						changeTypeCorrect = ETrue;
						test(KExpectedChange.iEntryType == KActualChange.iEntryType);
						
						if(KActualChange.iChangeType == MCalChangeCallBack2::EChangeAdd || 
						   KActualChange.iChangeType == MCalChangeCallBack2::EChangeModify)
							{
							TRAPD(err, TestFetchEntryByGuidL(KActualChange.iEntryId));
							test(err==KErrNone);
							}
						}
					}
				}
			test(changeTypeCorrect);
			}
		}
	// Pause the thread to simulate some extra processing that would occur on a real device
	// this will allow the modifying thread to add more than one entry between notifications
	User::After(18750);

	iNumChangesFound += aChangeItems.Count();
	}
	
void CTestAppManager::TestFetchEntryByGuidL(TCalLocalUid localId)
	{
	CCalEntry* entry = iCalTestLibrary->SynCGetEntryViewL().FetchL(localId);
	CleanupStack::PushL(entry);
	const TDesC8& uid = entry->UidL();
						
	HBufC8* guid= uid.AllocLC();
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iCalTestLibrary->SynCGetEntryViewL().FetchL(*guid, entries);
		
	CCalEntry* entryFetchen = entries[0];
	test(entries.Count()>0);
	test(entryFetchen!=NULL);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(guid);
	CleanupStack::PopAndDestroy(entry);
	}
	
void CTestAppManager::RunTestL()
	{
	// Set up notification filter (all entries in 2005)	
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(KRangeStartTime);// 1 Jan 05
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(KRangeEndTime);// 1 Jan 06
	CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, CalCommon::TCalTimeRange(calStartTime, calEndTime));
	CleanupStack::PushL(filter);
	iCalTestLibrary->GetSession().StartChangeNotification(*this, *filter);
	CleanupStack::PopAndDestroy(filter);
	
	for (*iTestCase = 0; *iTestCase < EAmount ; ++*iTestCase)
		{
		TBool notified(EFalse);
		iNumChangesFound = 0;
		
		// Allow the modifier to start preparing for the next test		
		iThread.Resume(); 
		
		NextTestL();

		// Wait until the modifier has prepared for the next test
		iSemaphore.Wait(); 			
		
		do	{
			iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
			iNotified = EFalse;
			CActiveScheduler::Start(); // wait for notification
			if (iNotified)
				{
				notified = ETrue;
				}
			}
		while (iNotified);			
		
		test(notified || !iExpectingNotification); // only test that we were notified if we are expecting to be
		test(iNumChangesFound == *iNumExpectedChanges);
		
		*iNumExpectedChanges = 0;
		}
		
	iThread.Resume(); // allow the thread to tidy up;
	iSemaphore.Wait();
	iCalTestLibrary->GetSession().StopChangeNotification();
	}


void CTestAppManager::NextTestL()
	{
	// CTestAppManager::RunTestL hands over execution to the function 
	// CTestAppModifier::RunTestL in the other thread for it to make 
	// changes. The manager waits until the modifier has done what it 
	// needs to and then signals the manager.
	switch (*iTestCase)
		{
		case EEntryOutOfRange:
			{
			test.Next(_L("1 Create entry that is out of range"));

			iExpectingNotification = EFalse;
			}
			break;
		case EEntryOutOfRange1:
			{
			test.Next(_L("2 Create child entry that is out of range"));

			iExpectingNotification = EFalse;
			}
			break;
		case EApptInsideRange1:
			{
			// add appt within time range

			test.Next(_L("3 Add, update and delete an appointment within the notification time range"));

			iExpectingNotification = ETrue;
			}
			break;
		case EApptInsideRange2:
			{
			test.Printf(_L("4 Update this appointment\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case EApptInsideRange3:
			{
			test.Printf(_L("5 Delete this appointment\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case EApptOutsideRange1:
			{
			// add appt outside time range

			test.Next(_L("6 Add an appointment outside the notification time range"));

			iExpectingNotification = EFalse;
			}
			break;
		case EApptOutsideRange2:
			{
			// update appt outside time range
			test.Printf(_L("7 Update this appointment\n"));
			iExpectingNotification = EFalse; // check callback notification was NOT received for adding appt
			}
			break;
		case EApptOutsideRange3:
			{
			// delete apppointment outside range
			test.Printf(_L("8 Delete this appointment\n"));
			iExpectingNotification = EFalse; // check callback notification was NOT received for delete
			}
			break;
		case ENotifyNonResponsive:
			{
			// notification is disabled by the modifying thread
			// add todo

			test.Next(_L("9 Disable change notification to check change notification from nonresponsive client (wait for 16 seconds)"));

            //The timer which enbles the broadcast added in 9.3 is not implemented here
			iExpectingNotification = EFalse;
			//iExpectingNotification = ETrue;			
            //
			// The Modifier thread disabled the notifications, but due to the
			// self startup mechanism in the Server, the notifications
			// will be dispatched when waiting for 15 seconds.
            // User::After(16 * KWait1second.Int());
			}
			break;						
		case ENotifyNonResponsive1:
			{
			test.Printf(_L("10 Delete this todo (wait for 16 seconds)\n"));
            //The timer which enbles the broadcast added in 9.3 is not implemented here
			iExpectingNotification = EFalse;
			//iExpectingNotification = ETrue;			
            //
			// The Modifier thread disabled the notifications, but due to the
			// self startup mechanism in the Server, the notifications
			// will be dispatched when waiting for 15 seconds.
            // User::After(16 * KWait1second.Int());
			}
			break;
		case ETodoInsideRange1:
			{
			// notification is disabled by the modifying thread
			// add todo within time range

			test.Next(_L("11 Disable change notification then add a todo within the notification time range"));

			iExpectingNotification = EFalse; // check callback notification was NOT received (disabled notification)
			}
			break;
		case ETodoInsideRange2:
			{
			// notification should now be re-enabled by the other thread
			test.Printf(_L("12 Re-enable change notification\n"));
			iExpectingNotification = ETrue; // check callback notification was received for todo added during disable
			}
			break;
		case ETodoInsideRange3:
			{
			test.Printf(_L("13 Update this todo\n"));
			iExpectingNotification = ETrue; // check callback notification was received for update
			}
			break;
		case ETodoInsideRange4:
			{
			test.Printf(_L("14 Delete this todo\n"));
			iExpectingNotification = ETrue; // check callback notification was received for delete
			}
			break;
		case ETodoOutsideRange1:
			{
			// add todo out of time range

			test.Next(_L("15 Add a todo entry outside the notification time range"));

			iExpectingNotification = EFalse; // check callback notification was NOT received for adding todo
			}
			break;
		case ETodoOutsideRange2:
			{
			test.Printf(_L("16 Update this todo\n"));
			iExpectingNotification = EFalse; // check callback notification was received for update
			}
			break;
		case ETodoOutsideRange3:
			{
			test.Printf(_L("17 Delete this todo\n"));
			iExpectingNotification = EFalse; // check callback notification was NOT received for delete
			}
			break;
		case EAddUndatedTodo:
			{
			test.Next(_L("18 Adding Undated Todo\n"));

			iExpectingNotification = ETrue;			
			}
			break;
		case EUpdateUndatedTodo:
			{

			test.Next(_L("19 Updating Undated Todo\n"));

			iExpectingNotification = ETrue;			
			}
			break;
		case EClearUndatedTodo:
			{
			test.Printf(_L("20 Deleting undated todo\n"));
			iExpectingNotification = ETrue; 
			}
			break;		
		case EMultipleInAndOutside1:
			{

			test.Next(_L("21 Add multiple events inside and outside the time range"));

			iExpectingNotification = ETrue; // check callback notification was received for delete
			}
			break;
		case EMultipleInAndOutside2:
			{
			test.Printf(_L("22 Delete these events\n"));
			iExpectingNotification = ETrue; // check callback notification was received for delete
			}
			break;
		case EBulkAdd:
			{

			test.Next(_L("23 Make lots of changes\n"));

			iExpectingNotification = ETrue;
			}
			break;
		case EBulkDelete:
			{
			test.Printf(_L("24 Clear the entries\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case EBulkAddToOverflowNotificationBuffer:
			{

			test.Next(_L("25 Make lots of changes and overflow the notification buffer\n"));

			test.Printf(_L("Expecting only a single 'Undefined Change' notification\n"));
			iExpectingNotification = ETrue;		
			}
			break;
		case EBulkDeleteToOverflowNotificationBuffer:
			{
			test.Printf(_L("26 Clear the entries and overflow the notification buffer\n"));
			test.Printf(_L("Expecting only a single 'Undefined Change' notification\n"));
			iExpectingNotification = ETrue;		
			}
			break;
		case ERepeatingEitherSideOfRange1:
			{

			test.Next(_L("27 Add a repeating entry across range but not in it"));

			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingEitherSideOfRange2:
			{
			test.Printf(_L("28 Update it\n"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingEitherSideOfRange3:
			{
			test.Printf(_L("29 Clear it\n"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingAcrossStartBoundary1:
			{

			test.Next(_L("30 Moving repeating entries (1)"));

			test.Printf(_L("Create repeating entry across the range start boundary\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossStartBoundary2:
			{
			test.Printf(_L("31 Move repeating entry to outside the range\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossStartBoundary3:
			{
			test.Printf(_L("32 Clear the entries"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingAcrossStartBoundary4:
			{

			test.Next(_L("33 Moving repeating entries (2)"));

			test.Printf(_L("Create repeating entry outside the range\n"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingAcrossStartBoundary5:
			{
			test.Printf(_L("34 Move repeating entry across the range start boundary\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossStartBoundary6:
			{
			test.Printf(_L("35 Clear the entries\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossEndBoundary1:
			{

			test.Next(_L("36 Moving repeating entries (3)"));

			test.Printf(_L("Create repeating entry across the range end boundary\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossEndBoundary2:
			{
			test.Printf(_L("37 Move repeating entry to outside the range\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossEndBoundary3:
			{
			test.Printf(_L("38 Clear the entries\n"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingAcrossEndBoundary4:
			{

			test.Next(_L("39 Moving repeating entries (4)"));

			test.Printf(_L("Create repeating entry outside the range\n"));
			iExpectingNotification = EFalse;
			}
			break;
		case ERepeatingAcrossEndBoundary5:
			{
			test.Printf(_L("40 Move repeating entry across the range end boundary\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case ERepeatingAcrossEndBoundary6:
			{
			test.Printf(_L("41 Clear the entries\n"));
			iExpectingNotification = ETrue;
			}
			break;
		case EFilterOnlyEventEntries1:
			{
			
			test.Next(_L("42 Filter for only event entries"));

			// change the filter so that we are only interested in event entries 
			TCalTime calStartTime;
			calStartTime.SetTimeLocalL(KRangeStartTime);// 1 Jan 05
			TCalTime calEndTime;
			calEndTime.SetTimeLocalL(KRangeEndTime);// 1 Jan 06
			iCalTestLibrary->GetSession().StopChangeNotification();
			CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryEvent, ETrue, CalCommon::TCalTimeRange(calStartTime, calEndTime));
			CleanupStack::PushL(filter);
			iCalTestLibrary->GetSession().StartChangeNotification(*this, *filter);
			CleanupStack::PopAndDestroy(filter);
			iExpectingNotification = EFalse;
			}
			break;
		case EFilterOnlyEventEntries2:
			{
			test.Printf(_L("43 Todo added"));
			iExpectingNotification = EFalse;
			}
			break;
		case EFilterOnlyEventEntries3:
			{
			test.Printf(_L("44 Event added"));
			iExpectingNotification = ETrue;
			}
			break;
		case EFilterOnlyEventEntries4:
			{
			test.Printf(_L("45 Clear the entries"));
			iExpectingNotification = ETrue;
			}
			break;
		case EFilterOnlyTodoEntries1:
			{

			test.Next(_L("46 Filter for only todo entries"));

			// change the filter so that we are only interested in todo entries 
			TCalTime calStartTime;
			calStartTime.SetTimeLocalL(KRangeStartTime);// 1 Jan 05
			TCalTime calEndTime;
			calEndTime.SetTimeLocalL(KRangeEndTime);// 1 Jan 06
			iCalTestLibrary->GetSession().StopChangeNotification();
			CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryTodo, ETrue, CalCommon::TCalTimeRange(calStartTime, calEndTime));
			CleanupStack::PushL(filter);
			iCalTestLibrary->GetSession().StartChangeNotification(*this, *filter);
			CleanupStack::PopAndDestroy(filter);
			iExpectingNotification = EFalse;
			}
			break;
		case EFilterOnlyTodoEntries2:
			{
			test.Printf(_L("47 Todo added"));
			iExpectingNotification = ETrue;
			}
			break;
		case EFilterOnlyTodoEntries3:
			{
			test.Printf(_L("48 Event added"));
			iExpectingNotification = EFalse;
			}
			break;
		case EFilterOnlyTodoEntries4:
			{
			test.Printf(_L("49 Clear the entries"));
			iExpectingNotification = ETrue;
			}
			break;
		case EFilterOnlyTodoEntries5:
			{
			test.Printf(_L("50 Restore filter to all entries"));
			// change the filter so that we are only interested in todo entries 
			TCalTime calStartTime;
			calStartTime.SetTimeLocalL(KRangeStartTime);// 1 Jan 05
			TCalTime calEndTime;
			calEndTime.SetTimeLocalL(KRangeEndTime);// 1 Jan 06
			iCalTestLibrary->GetSession().StopChangeNotification();
			CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, CalCommon::TCalTimeRange(calStartTime, calEndTime));
			CleanupStack::PushL(filter);
			iCalTestLibrary->GetSession().StartChangeNotification(*this, *filter);
			CleanupStack::PopAndDestroy(filter);
			iExpectingNotification = EFalse;
			}
			break;
		case EAddAndUpdateParentAndChild1:
			{

			test.Next(_L("51 Add and update parent and child entries"));

			test.Printf(_L("add the parent entry"));
			iExpectingNotification = ETrue;
			}
			break;
		case EAddAndUpdateParentAndChild2:
			{
			test.Printf(_L("52 Update the parent entry"));
			iExpectingNotification = ETrue;
			}
			break;
		case EAddAndUpdateParentAndChild3:
			{
			test.Printf(_L("53 add a child to the parent entry"));
			iExpectingNotification = ETrue;			
			}
			break;
		case EAddAndUpdateParentAndChild4:
			{
			test.Printf(_L("54 update the child entry"));
			iExpectingNotification = ETrue;
			}
			break;
		case EAddAndUpdateParentAndChild5:
			{
			test.Printf(_L("55 store the parent again"));
			iExpectingNotification = ETrue;		
			}
			break;
		case EAddAndUpdateParentAndChild6:
			{
			test.Printf(_L("56 add the child again"));
			iExpectingNotification = ETrue;		
			}
			break;
		case EAddAndUpdateParentAndChild7:
			{
			test.Printf(_L("57 delete the child"));
			iExpectingNotification = ETrue;
			}
			break;
		case EAddAndUpdateParentAndChild8:
			{
			test.Printf(_L("58 add the child again"));
			iExpectingNotification = ETrue;			
			}
			break;
		case EAddAndUpdateParentAndChild9:
			{
			test.Printf(_L("59 delete the parent"));
			iExpectingNotification = ETrue;
			}
			break;

		case EDeleteMultipleEntriesSetup:
			{

			test.Next(_L("60 Setup delete fail test - Create one entry"));

			iExpectingNotification = EFalse;
			}
			break;

		case EDeleteMultipleEntriesFail:
			{

			test.Next(_L("61 Test deleting multiple entries where the second delete fails"));

			iExpectingNotification = EFalse;
			}
			break;
			
		case EChangesToOtherFile:
			{

			test.Next(_L("62 Make changes to a different file and make sure we are not notified"));

			iExpectingNotification = EFalse;
			}
			break;

		default:
			{
			User::Panic(_L("63 A case statement for this test has not been added"), *iTestCase);
			}
			break;
		}
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CTestAppManager* testManager = CTestAppManager::NewLC();
	

	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	testManager->RunTestL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	

	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-OBSERVER-MANAGER-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-OBSERVER-MANAGER-0001 Calendar Interim API Observer test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
