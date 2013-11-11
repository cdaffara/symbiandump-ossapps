// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Tests that PhBkSync adds contacts from USIM address books to the contacts database,
// and that they appear correctly in the Contacts Model local view.
// 
//

#include <e32test.h>
#include "te_cntsyncbase.h"
#include "te_cntsyncview.h"
#include <cntview.h>

// Values for User::After()
const TInt	K1SecondWait(1000000);
const TInt	K5SecondWait(5*K1SecondWait);
const TInt	K20SecondWait(20*K1SecondWait);

// Test config numbers...
const TInt  KNormalSIMTestConfig(0);
const TInt  KLockedSIMTestConfig(8);

// Number of contacts on SIM cards
const TInt	KNumContactsNormalSIM(5);
const TInt	KNumContactsLockedSIM(25);

class TViewCleanup
	{
	public:
		TViewCleanup(CContactLocalView* aLocalView, CContactViewEventWait* aViewWaitEvent)
			{
			iLocalView = aLocalView;
			iViewWaitEvent = aViewWaitEvent;
			}
		CContactViewEventWait* iViewWaitEvent;
		CContactLocalView* iLocalView;
	};

static void ViewCleanup(TAny* aPointer)
	{
	TViewCleanup* cleanup = static_cast<TViewCleanup*> (aPointer);
	CContactLocalView* iLocalView = cleanup->iLocalView;
	CContactViewEventWait* iViewWaitEvent = cleanup->iViewWaitEvent;
	iLocalView->Close(*iViewWaitEvent);
	delete iViewWaitEvent;
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkViewICCSyncTest object
 */
CPhbkViewICCSyncTest* CPhbkViewICCSyncTest::NewL()
	{
	CPhbkViewICCSyncTest* self = new(ELeave) CPhbkViewICCSyncTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkViewICCSyncTest::CPhbkViewICCSyncTest()
	{
	SetTestStepName(_L("ViewICCSyncTest"));
	}

/**
 * Synchronisation of normal, unlocked, ICC.  Creates a Contacts View
 * with ICC entries only.
 */
enum TVerdict CPhbkViewICCSyncTest::doTestStepL()
	{
	SetSimTsyTestNumberL(0);
	DoSyncL();
	
	// all View tests use Manual Sync mode
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual)); // Set mode to manual to avoid getting KErrInUse
	CheckSyncModeL(RPhoneBookSession::EManual);
	
	// ICC unlocked
	SetSimTsyTestNumberL(KNormalSIMTestConfig);
	CleanupClosePushL(iViewSortOrder);

	// A sort order (only Family Name is present in ADN phonebooks)
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	// Helper class that gives timed wait for View events
	CContactViewEventWait *viewEventWait = CContactViewEventWait::NewL();
	CleanupStack::PushL(viewEventWait);

	// View with just ICC entries
	iIccView = CContactLocalView::NewL(*viewEventWait, *iDb, iViewSortOrder, EICCEntriesOnly);

	CleanupStack::Pop();//viewEventWait will be deleted with cleanup below
	TViewCleanup cleanup(iIccView, viewEventWait);
	CleanupStack::PushL(TCleanupItem(ViewCleanup, &cleanup));

	INFO_PRINTF1(_L("  Manual Sync"));
	// do manual synchronisation
	DoSyncL();
	WaitForSyncToCompleteL();

	// let view catch up, waits 2s
	INFO_PRINTF1(_L("  Wait for ICC view to become ready"));
	// Wait 30s for View Event: EReady or ESortOrderChanged only
	TBool eventReceived = viewEventWait->WaitForViewEvent(iIccView, 30, iEvent, EFalse);
	// check that event was received
	TESTCHECKCONDITIONL(eventReceived);
	TESTCHECKL(iEvent.iEventType, TContactViewEvent::EReady);

	INFO_PRINTF1(_L("  Check all PhoneBook entries are in view"));
	TInt count = iIccView->CountL();
	if (count != KNumContactsNormalSIM)
		{
		INFO_PRINTF3(_L("  Expecting %d entries in View but found %d:"),
			KNumContactsNormalSIM, count);
		DumpICCViewL();
		}
	TESTCHECK(count, KNumContactsNormalSIM);

	User::After(K5SecondWait);
	CleanupStack::PopAndDestroy(2);//iIccView, viewEventWait and iViewSortOrder

	return TestStepResult();
	}
	
	
/** 
 * Factory construction method.
 * @return Pointer to CPhbkViewICCLockedICCTest object
 */
CPhbkViewICCLockedICCTest* CPhbkViewICCLockedICCTest::NewL()
	{
	CPhbkViewICCLockedICCTest* self = new(ELeave) CPhbkViewICCLockedICCTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkViewICCLockedICCTest::CPhbkViewICCLockedICCTest()
	{
	SetTestStepName(_L("ViewICCLockedICCTest"));
	}

/**
 * Synchronisation when ICC is locked.  Creates a Contacts View of ICC entries
 * only.
 *
 * Synchroniser is initially unable to perform synchronisation because ICC is locked. The
 * Local View class recognises that the ICC is locked, and creates an empty View.
 *
 * When ICC becomes unlocked the synchronisation completes successfully. The Contacts Local 
 * View sees this, and re-sorts itself making the ICC entries appear in the View.
 */
enum TVerdict CPhbkViewICCLockedICCTest::doTestStepL()
	{
	// ICC locked, so initial Synchronisation is not carried out 
	SetSimTsyTestNumberL(KLockedSIMTestConfig);
	CleanupClosePushL(iViewSortOrder);
	// A sort order (only Family Name is present in ADN phonebooks)
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	// Helper class that gives timed wait for View events
	CContactViewEventWait *viewEventWait = CContactViewEventWait::NewL();
	CleanupStack::PushL(viewEventWait);

	// View with just ICC entries
	iIccView = CContactLocalView::NewL(*viewEventWait, *iDb, iViewSortOrder, EICCEntriesOnly);
	CleanupStack::Pop();//viewEventWait will be deleted with cleanup below
	TViewCleanup cleanup(iIccView, viewEventWait);
	CleanupStack::PushL(TCleanupItem(ViewCleanup, &cleanup));

	INFO_PRINTF1(_L("  Initial Sync Failure"));
	// check that ICC is locked
	TRAPD(err, DoSyncFailL());
	if(err == KErrGeneral)
		{
		INFO_PRINTF1(_L("The view has already been created before we attempt to sync, so sync won't fail. We should finsh test here"));
		SetTestStepResult(EPass);
		CleanupStack::PopAndDestroy(2);//iIccView, viewEventWait and iViewSortOrder
		return TestStepResult(); 
		}
	TESTCHECKL(err, KErrNone);
	User::After(K1SecondWait);

	RPhoneBookSession::TSyncState state;
	iSession.GetPhoneBookCacheState(state);

	TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync); // Cache not in valid state 

	iSession.GetLastSyncError(err);
	TESTCHECKL(err, KErrAccessDenied); // ICC locked

	// initially ready with no entries as ICC (SIM) card is not ready, waits 2s
	INFO_PRINTF1(_L("  Check View becomes ready with no entries"));
	// Wait 30seconds at max for View Event: EReady or ESortOrderChanged only
	TBool eventReceived = viewEventWait->WaitForViewEvent(iIccView, 30, iEvent, EFalse);
	// check that event was received
	TESTCHECKCONDITIONL(eventReceived);

	TESTCHECKL(iEvent.iEventType, TContactViewEvent::EReady);
	TESTCHECKL(iIccView->CountL(), 0); // view should be empty

	// wait for ICC to become unlocked
	INFO_PRINTF1(_L("  Start Sync when ICC becomes unlocked"));
	User::After(K20SecondWait); // ICC should become unlocked now 
	DoSyncL();
	WaitForSyncToCompleteL();

	// let the view catch up, waits 2s
	INFO_PRINTF1(_L("  Wait for View to Re-Sort with ICC entries"));
	// Wait 30seconds at max for View Event: EReady or ESortOrderChanged only
	eventReceived = viewEventWait->WaitForViewEvent(iIccView, 30, iEvent, EFalse);
	// check that event was received
	TESTCHECKCONDITIONL(eventReceived);

	INFO_PRINTF1(_L("  Contacts View should be resorted"));
	TESTCHECKL(iEvent.iEventType, TContactViewEvent::ESortOrderChanged);

	INFO_PRINTF1(_L("  Check all PhoneBook entries are in view"));
	TInt count = iIccView->CountL();
	if (count != KNumContactsLockedSIM)
		{
		INFO_PRINTF3(_L("  Expecting %d entries in View but found %d:"),
			KNumContactsLockedSIM, count);
		DumpICCViewL();
		}
	TESTCHECK(count, KNumContactsLockedSIM);

	User::After(K5SecondWait);
	CleanupStack::PopAndDestroy(2);//iIccView, viewEventWait and iViewSortOrder

	return TestStepResult();
	}
	
	
/** 
 * Factory construction method.
 * @return Pointer to CPhbkViewICCLockedMixedTest object
 */
CPhbkViewICCLockedMixedTest* CPhbkViewICCLockedMixedTest::NewL()
	{
	CPhbkViewICCLockedMixedTest* self = new(ELeave) CPhbkViewICCLockedMixedTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkViewICCLockedMixedTest::CPhbkViewICCLockedMixedTest()
	{
	SetTestStepName(_L("ViewICCLockedMixedTest"));
	}

/**
 * Synchronisation when ICC is locked.  Creates a Contacts View of Contacts
 * and ICC entries.  This is similar to the ICC entries only test, but timing
 * and behaviour of Contacts model is different.
 *
 * Synchroniser is initially unable to perform synchronisation because ICC is locked. The
 * Local View class recognises that the ICC is locked, and creates a View without ICC entries.
 *
 * When ICC becomes unlocked the synchronisation completes successfully. The Contacts Local 
 * View sees this, and re-sorts itself making the ICC entries appear in the View.
 */
enum TVerdict CPhbkViewICCLockedMixedTest::doTestStepL()
	{
	// ICC locked, so initial Synchronisation is not carried out
	SetSimTsyTestNumberL(KLockedSIMTestConfig);
	CleanupClosePushL(iViewSortOrder);

	// A sort order (only Family Name is present in ADN phonebooks)
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);

	// Helper class that gives timed wait for View events
	CContactViewEventWait *viewEventWait = CContactViewEventWait::NewL();
	CleanupStack::PushL(viewEventWait);

	// View with ICC entries and contacts
	iIccView = CContactLocalView::NewL(*viewEventWait, *iDb, iViewSortOrder, EICCEntriesAndContacts);
	
	CleanupStack::Pop();//viewEventWait will be deleted with cleanup below
	TViewCleanup cleanup(iIccView, viewEventWait);
	CleanupStack::PushL(TCleanupItem(ViewCleanup, &cleanup));

	INFO_PRINTF1(_L("  Initial Sync Failure"));
	// check that ICC is locked
	TRAPD(err, DoSyncFailL());
	if(err == KErrGeneral)
		{
		INFO_PRINTF1(_L("The view has already been created before we attempt to sync, so sync won't fail. We should finsh test here"));		
		SetTestStepResult(EPass);
		CleanupStack::PopAndDestroy(2);//iIccView, viewEventWait and iViewSortOrder
		return TestStepResult(); 
		}
	TESTCHECKL(err, KErrNone);
	User::After(K1SecondWait);

	RPhoneBookSession::TSyncState state;
	iSession.GetPhoneBookCacheState(state);

	TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync); // Cache not in valid state 

	iSession.GetLastSyncError(err);
	TESTCHECKL(err, KErrAccessDenied); // ICC locked

	// initially ready with no entries as ICC (SIM) card is not ready, waits 2s
	INFO_PRINTF1(_L("  Check View becomes ready without ICC entries"));
	// Wait 30seconds at max for View Event: EReady or ESortOrderChanged only
	TBool eventReceived = viewEventWait->WaitForViewEvent(iIccView, 30, iEvent, EFalse);
	// check that event was received
	TESTCHECKCONDITIONL(eventReceived);

	TESTCHECKL(iEvent.iEventType, TContactViewEvent::EReady);
	TESTCHECKL(iIccView->CountL(), 0); // view should be empty

	// wait for ICC to become unlocked
	INFO_PRINTF1(_L("  Start Sync when ICC becomes unlocked"));
	User::After(K20SecondWait); // ICC should become unlocked now 
	DoSyncL();
	WaitForSyncToCompleteL();

	// let the view catch up, waits 2s
	INFO_PRINTF1(_L("  Wait for View to Re-Sort with ICC entries"));
	// Wait 30seconds at max for View Event: EReady or ESortOrderChanged only
	eventReceived = viewEventWait->WaitForViewEvent(iIccView, 30, iEvent, EFalse);
	// check that event was received
	TESTCHECKCONDITIONL(eventReceived);

	INFO_PRINTF1(_L("  Contacts View should be resorted"));
	TESTCHECKL(iEvent.iEventType, TContactViewEvent::ESortOrderChanged);

	INFO_PRINTF1(_L("  Check all PhoneBook entries are in view"));
	TInt count = iIccView->CountL();
	if (count != KNumContactsLockedSIM)
		{
		INFO_PRINTF3(_L("  Expecting %d entries in View but found %d:"),
			KNumContactsLockedSIM, count);
		DumpICCViewL();
		}
	TESTCHECK(count, KNumContactsLockedSIM);

	User::After(K5SecondWait);
	CleanupStack::PopAndDestroy(2);//iIccView, viewEventWait and iViewSortOrder
	return TestStepResult();
	}


/**
 *  Synchronisation and create a view when just one phonebook exists. Previously
 *  a defect existed which would fail if no FDN phonebook was defined.
 */
enum TVerdict CPhbkViewICCSinglePhonebookTest::doTestStepL()
	{
	SetSimTsyTestNumberL(30);

	//
	// Setup a sort order (only Family Name is present in ADN phonebooks)
    //
	INFO_PRINTF1(_L("Setup a sort order (only Family Name is present in ADN phonebooks)"));
	CleanupClosePushL(iViewSortOrder);
    iViewSortOrder.AppendL(KUidContactFieldFamilyName);
    iViewSortOrder.AppendL(KUidContactFieldGivenName);
    iViewSortOrder.AppendL(KUidContactFieldCompanyName);
	
	//
    // Helper class that gives timed wait for View events
	//
	CContactViewEventWait *viewEventWait = CContactViewEventWait::NewL();
	CleanupStack::PushL(viewEventWait);

	//
	// Create a view with ICC entries and contacts
	//
	INFO_PRINTF1(_L("Create a view with ICC entries and contacts"));
	iIccView = CContactLocalView::NewL(*viewEventWait, *iDb, iViewSortOrder, EICCEntriesAndContacts);
	
	CleanupStack::Pop();//viewEventWait will be deleted with cleanup below
	TViewCleanup cleanup(iIccView, viewEventWait);
	CleanupStack::PushL(TCleanupItem(ViewCleanup, &cleanup));	

	//
	// Sync ADN phonebook...
	//
	INFO_PRINTF1(_L(" Sync ADN phonebook..."));
	TRequestStatus  status; 

	iSession.DoSynchronisation(status, KUidIccGlobalAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	//
	// Close view and clean up...
	//
	INFO_PRINTF1(_L(" Close view and clean up..."));
	CleanupStack::PopAndDestroy(2);

	return TestStepResult();
	} // CPhbkViewICCSinglePhonebookTest::doTestStepL


void CPhbkSyncViewIntegrationTestBase::DumpICCViewL()
	{
	const TInt count = iIccView->CountL();

	if (count == 0)
		{
		INFO_PRINTF1(_L("  No contacts to list."));
		return;
		}

	CContactItem* contact = NULL;
	TInt  index;

	for (index = 0;  index < count;  index++)
		{
		const CViewContact&  viewContact = iIccView->ContactAtL(index);
		const TInt  id = viewContact.Id();
		
		// get actual Contact from DB
		contact = iDb->ReadMinimalContactLC(id);

		// Get & print Contact's phone number
		CContactItemFieldSet& fieldset = contact->CardFields();
		// look for Phone Number field in record
		const TInt pos = fieldset.Find(KUidContactFieldPhoneNumber);	
		User::LeaveIfError(pos);	// field must be present
		CContactItemField& field = fieldset[pos];
		CContactTextField* textfield = field.TextStorage();

		TBuf<128>  outputString;
		outputString.AppendFormat(_L("    [%04i]  Id=%04i Name=\""), index+1, id);
		outputString.Append(viewContact.Field(0));
		outputString.AppendFormat(_L("\" Number=\""));
		outputString.Append(textfield->Text());
		outputString.AppendFormat(_L("\""));
		INFO_PRINTF1(outputString);

		CleanupStack::PopAndDestroy(contact);
		contact = NULL;
		}
	}

// support class implementation, timed wait for View Event; (based on CContactViewEventQueue)

CContactViewEventWait* CContactViewEventWait::NewL()
    {
    CContactViewEventWait* self = new(ELeave) CContactViewEventWait;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


CContactViewEventWait::~CContactViewEventWait()
    {
    CTimer::Cancel();
    if (iView) iView->Close(*this);
    }


/**
 *  Waits for an event from the database.
 *
 *  @param aView        Contact view to listen for events.
 *  @param aTimeOut     Max time to wait for an event.
 *  @param aEvent       The received event, undefined if this function returns false.
 *  @param aWaitForAny  ETrue if wait is for any event, EFalse for view ready events only
 *
 *  @return true if an event was received, false if timeout expired first.
 */
TBool CContactViewEventWait::WaitForViewEvent(CContactViewBase* aView,
											  TTimeIntervalSeconds aTimeOut, 
											  TContactViewEvent& aEvent, 
											  TBool aWaitForAny)
    {
	iView = aView;
	iWaitForAny = aWaitForAny;
	iTimedOut = EFalse;
    CTimer::After(aTimeOut.Int() * 1000000);

	// wait for timer to finish or desired View event
    CActiveScheduler::Start();

    if (!iTimedOut)
        {
        aEvent = iEvent;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }


void CContactViewEventWait::RunL()
    {
    // Timer expired
	iTimedOut = ETrue;
    CActiveScheduler::Stop();
    }


void CContactViewEventWait::HandleContactViewEvent(const CContactViewBase& aView,
												   const TContactViewEvent& aEvent)
    {
	__ASSERT_ALWAYS(iView == &aView, User::Invariant());

	// either waiting for Any event, or Ready or Ready after Re-sort
	if(iWaitForAny || aEvent.iEventType == TContactViewEvent::EReady || 
			aEvent.iEventType == TContactViewEvent::ESortOrderChanged)
		{
		iEvent = aEvent;

	    CTimer::Cancel();
	    CActiveScheduler::Stop();
		}
    }


CContactViewEventWait::CContactViewEventWait()
    : CTimer(CActive::EPriorityStandard)
    {
    }


void CContactViewEventWait::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
	iView = NULL;
    }

