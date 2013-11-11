// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>

#include "te_cntsyncbase.h"
#include "te_cntsyncsync.h"

_LIT(KSatNameInADN, "SATNotify Entry ADN");
_LIT(KSatNameInSDN, "SATNotify Entry SDN");
_LIT(KSatNameInLND, "SATNotify Entry LND");
_LIT(KSatNameInUsim, "SATNotify Entry Usim");
_LIT(KSatNameInFDN, "SATNotify Entry FDN");

 
/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncAutomaticSameICCTest object
 */
CPhbkSyncAutomaticSameICCTest* CPhbkSyncAutomaticSameICCTest::NewL()
	{
	CPhbkSyncAutomaticSameICCTest* self = new(ELeave) CPhbkSyncAutomaticSameICCTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncAutomaticSameICCTest::CPhbkSyncAutomaticSameICCTest()
	{
	SetTestStepName(_L("SyncAutomaticSameICCTest"));
	}

/** 
 * If we are unsynchronised and we set the mode to automatic same ICC, we wait until
 * the next client to connect before synchronising. 
 */
enum TVerdict CPhbkSyncAutomaticSameICCTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL(); // First do initial synchronisation

	// Set sync mode to Automatic Same ICC for all phonebooks
	SetSyncModeL(RPhoneBookSession::EAutoSameIcc);

	// Check sync mode is set to Automatic Same ICC for all phonebooks
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc);
 	delete iDb;
 	iDb=NULL;
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::LeaveIfError(iSession.Connect()); // Re-connect again 
	User::After(3000000); // 3 seconds to ensure server has chance to fully startup

	// Check sync mode is still set to Automatic Same ICC for all phonebooks
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc);
	
	// Wait until cache becomes usable
	WaitForSyncToCompleteL();

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	SetSimTsyTestNumberL(0); // This configuration has different IMSI number to previous one, so
							 // this means that Auto Sync should NOT be started
	CContactDatabase* db = NULL; // Start an instance of contacts model and do some 
								 // operation to force a sync
	TRAPD(err, db = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);
	TRAP(err, db->ICCTemplateIdL()); // Connects to the server

	TESTCHECKL(err, KErrNone); // template ID can be retrieved
	delete db;

	// Verify cache is not valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::EUnsynchronised);
	User::After(5000000); // Wait 5s and then check that cache state has not changed
	CheckCacheStateL(RPhoneBookSession::EUnsynchronised);

	SetSimTsyTestNumberL(1); // Going back to last configuration with same IMSI number as previous
							 // one, so this means that Auto Sync should be started as soon as we 
							 // connect to the server

	// Wait until cache becomes usable
	WaitForSyncToCompleteL();

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncAutomaticCurrentICCTest object
 */
CPhbkSyncAutomaticCurrentICCTest* CPhbkSyncAutomaticCurrentICCTest::NewL()
	{
	CPhbkSyncAutomaticCurrentICCTest* self = new(ELeave) CPhbkSyncAutomaticCurrentICCTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncAutomaticCurrentICCTest::CPhbkSyncAutomaticCurrentICCTest()
	{
	SetTestStepName(_L("SyncAutomaticCurrentICCTest"));
	}

/** 
 * If we are unsynchronised and we set the mode to automatic current, we wait until
 * the next client to connect before synchronising.
 */
enum TVerdict CPhbkSyncAutomaticCurrentICCTest::doTestStepL()
	{
	// Set sync mode to Automatic Same ICC for all phonebooks
	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	delete iDb;
 	iDb=NULL;
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::LeaveIfError(iSession.Connect()); // Re-connect again 
	User::After(5000000); // 5 seconds to ensure server has chance to start up

	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc); // Check sync mode is still Automatic 
	WaitForSyncToCompleteL();

	// Verify cache is valid
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	// Start an instance of contacts model and do some operation to force a sync
	CContactDatabase* db = NULL;
	TRAPD(err, db = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);
	TContactItemId templateId(0);					  
	TRAP(err, templateId = db->ICCTemplateIdL()); // Connects to the server
	TESTCHECKCONDITIONL(templateId != KNullContactId);
	delete db;

	// Wait until cache becomes usable
	WaitForSyncToCompleteL();

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncManualTest object
 */
CPhbkSyncManualTest* CPhbkSyncManualTest::NewL()
	{
	CPhbkSyncManualTest* self = new(ELeave) CPhbkSyncManualTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncManualTest::CPhbkSyncManualTest()
	{
	SetTestStepName(_L("SyncManualTest"));
	}

/** Re-Synchronisation (Manual) */
enum TVerdict CPhbkSyncManualTest::doTestStepL()
	{
	DoSyncL();
	WaitForSyncToCompleteL();

	RPhoneBookSession::TSyncState state; // First check that initial Synchronisation was successful 
	iSession.GetPhoneBookCacheState(state); 
	TESTCHECKL(state, RPhoneBookSession::ECacheValid);

	// Set mode to Manual
	SetSyncModeL(RPhoneBookSession::EManual);

	CheckSyncModeL(RPhoneBookSession::EManual);
	DoSyncL();

	// Wait until cache becomes usable
	WaitForSyncToCompleteL();

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncManualNotReadyTest object
 */
CPhbkSyncManualNotReadyTest* CPhbkSyncManualNotReadyTest::NewL()
	{
	CPhbkSyncManualNotReadyTest* self = new(ELeave) CPhbkSyncManualNotReadyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncManualNotReadyTest::CPhbkSyncManualNotReadyTest()
	{
	SetTestStepName(_L("SyncManualNotReadyTest"));
	}

/** Try to open a new session and attempt a manual sync before the device has time to 
    get the SubscriberId. The Server should wait untill the SubscriberID is known before continuing */
enum TVerdict CPhbkSyncManualNotReadyTest::doTestStepL()
	{
	delete iDb;
 	iDb=NULL;
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();
	iDb = OpenOrCreateContactDatabaseL();
	User::LeaveIfError(iSession.Connect()); // Re-connect again 
	
	// Set mode to Manual
	SetSyncModeL(RPhoneBookSession::EManual);

	CheckSyncModeL(RPhoneBookSession::EManual);

	User::After(3000000); // 3 seconds to ensure server has chance to fully startup
	
	TRAPD(err, DoSyncL()); // and try sync immediately (Timing window - when debugging, do not step through)
	TESTCHECKL(err, KErrNone);	
	
	// Wait until cache becomes usable
	WaitForSyncToCompleteL();

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncManualDiffSizeSIMTest object
 */
CPhbkSyncManualDiffSizeSIMTest* CPhbkSyncManualDiffSizeSIMTest::NewL()
	{
	CPhbkSyncManualDiffSizeSIMTest* self = new(ELeave) CPhbkSyncManualDiffSizeSIMTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncManualDiffSizeSIMTest::CPhbkSyncManualDiffSizeSIMTest()
	{
	SetTestStepName(_L("SyncManualDiffSizeSIMTest"));
	}

/** 
 * Test synchronising a SIM with a different number of slots.
 */
enum TVerdict CPhbkSyncManualDiffSizeSIMTest::doTestStepL()
	{
	//
	// Sync with a SIM which has 22 entries used out of 25 for each of the 5 phonebooks...
	//
	SetSimTsyTestNumberL(24);	
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual));
	CheckSyncModeL(RPhoneBookSession::EManual);
	DoSyncL();
	WaitForSyncToCompleteL();
	
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TInt  count(sortedItems->Count());
	TESTCHECKL(count, KTest24ICCSlotsUsed);

	//
	// Sync with a SIM which has 3 entries used out of 15 for each of the 5 phonebooks...//
	//
	SetSimTsyTestNumberL(25);
	DoSyncL();
	WaitForSyncToCompleteL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	count = sortedItems->Count();
	TESTCHECKL(count, KTest25ICCSlotsUsed);

	//
	// Check that all slots can be read successfully.  This used to leave in a defect case.
	//
	for (TInt index = 0;  index < count;  index++)
		{ 
		TContactItemId  id = (*sortedItems)[index];
		CContactItemViewDef*  view = CContactItemViewDef::NewLC(CContactItemViewDef::EIncludeFields,
			                                                    CContactItemViewDef::EMaskHiddenFields);
		view->AddL(KUidContactFieldPhoneNumber); 
		TESTCHECKL(view->Count(), 1);
		CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->ReadContactL(id, *view)); 
		delete entry;
		CleanupStack::PopAndDestroy(1, view);
		} 

	//
	// Sync with a SIM which has 22 entries used out of 25 for each of the 5 phonebooks...
	//
	SetSimTsyTestNumberL(24);
	DoSyncL();
	WaitForSyncToCompleteL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	count = sortedItems->Count();
	TESTCHECKL(count, KTest24ICCSlotsUsed);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncSATUpdatesTestBase object
 */
CPhbkSyncSATUpdatesTestBase* CPhbkSyncSATUpdatesTestBase::NewL()
	{
	CPhbkSyncSATUpdatesTestBase* self = new(ELeave) CPhbkSyncSATUpdatesTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncSATUpdatesTestBase::CPhbkSyncSATUpdatesTestBase()
	{
	SetTestStepName(_L("SyncSATUpdatesTest"));
	}

/** 
 * Re-Synchronisation (SIM Toolkit initiated).
 * SIM is OK. Synchronisation mode is "Auto Sync, Current ICC".
 * Test initial synchronisation is successful. Then test that
 * a subsequent "SIM toolkit refresh" notification leads to a 
 * second synchronisation.
 */
enum TVerdict CPhbkSyncSATUpdatesTestBase::doTestStepL()
	{
	//
	// Sync to a basic SIM to allow the following sync to complete quicky...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(0);
	DoSyncL();
	WaitForSyncToCompleteL();

	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	//
	// Switch to a SIM TSY configuration which sends a "refresh" notification
	// at some point in the future and sync all phonebooks.
	//
	SetSimTsyTestNumberL(7);
	WaitForSyncToCompleteL();
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	TInt  numSlots, result;
	TContactItemId  aId0, aId1, aId2, aId3, aId4, aId5, aId6;

	result = iSession.GetNumSlots(numSlots, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNone);
	TESTCHECK(numSlots, 20);

	result = iSession.GetSlotId(0, aId0, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(1, aId1, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(2, aId2, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(3, aId3, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(4, aId4, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNone);
	result = iSession.GetSlotId(5, aId5, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNone);
	result = iSession.GetSlotId(6, aId6, KUidIccGlobalAdnPhonebook);
	TESTCHECK(result, KErrNone);

	result = iSession.GetSlotId(0, aId0, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(1, aId1, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNone);
	result = iSession.GetSlotId(2, aId2, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNone);
	result = iSession.GetSlotId(3, aId3, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(4, aId4, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(5, aId5, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNotFound);
	result = iSession.GetSlotId(6, aId6, KUidUsimAppAdnPhonebook);
	TESTCHECK(result, KErrNotFound);

	CheckNameInDBL(KSatNameInADN);
	CheckNameInDBL(KSatNameInSDN);
	CheckNameInDBL(KSatNameInLND);
	CheckNameInDBL(KSatNameInUsim);
	CheckNameInDBL(KSatNameInFDN);

	//
	// Wait for a notification that one of the phonebooks has changed state and
	// that they are all now unsynchronised...
	//
	SignalSimTsyToReduceTimersL();
	
	TRequestStatus  status1;
	iSession.NotifyPhBkCacheStateChange(status1, KUidIccGlobalAdnPhonebook);
	User::WaitForRequest(status1);
	TESTCHECK(status1.Int(), KErrNone);
	
	CheckCacheStateL(RPhoneBookSession::EUnsynchronised);//sat notification has been triggered
	
	//
	// Wait for SAT notified sync to finish and check they are all now valid...
	//
	WaitForSyncToCompleteL();
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	CheckNameInDBL(KSatNameInADN);
	CheckNameInDBL(KSatNameInSDN);
	CheckNameInDBL(KSatNameInLND);
	CheckNameInDBL(KSatNameInUsim);
	CheckNameInDBL(KSatNameInFDN);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncICCLockedTestBase object
 */
CPhbkSyncICCLockedTestBase* CPhbkSyncICCLockedTestBase::NewL()
	{
	CPhbkSyncICCLockedTestBase* self = new(ELeave) CPhbkSyncICCLockedTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncICCLockedTestBase::CPhbkSyncICCLockedTestBase()
	{
	SetTestStepName(_L("SyncICCLockedTest"));
	}

/**
 * Synchronisation when ICC is locked. Synchroniser is initially unable to perform 
 * synchronisation because ICC is locked, but when ICC becomes unlocked the synchronisation 
 * completes successfully.
 */
enum TVerdict CPhbkSyncICCLockedTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(8); // ICC locked, so Synchronisation is not carried out 
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);

	TRAPD(err, DoSyncFailL());
	TESTCHECKL(err, KErrNone);

	// Verify cache is not in valid state for all phonebooks
	CheckCacheStateL(RPhoneBookSession::EErrorDuringSync);

	// Save test time by prompting the SIMTSY...
	SignalSimTsyToReduceTimersL();

	TInt err1(KErrAccessDenied), err2(KErrAccessDenied), err3(KErrAccessDenied), 
		err4(KErrAccessDenied), err5(KErrAccessDenied);
	while (err1==KErrAccessDenied  ||  err1==KErrInUse  ||
	       err2==KErrAccessDenied  ||  err2==KErrInUse  ||
	       err3==KErrAccessDenied  ||  err3==KErrInUse  ||
	       err4==KErrAccessDenied  ||  err4==KErrInUse  ||
	       err5==KErrAccessDenied  ||  err5==KErrInUse)
		{
		// Global ADN Phonebook
		TRequestStatus status1;
		iSession.DoSynchronisation(status1);
		User::WaitForRequest(status1);
		err1 = status1.Int();

		// Global SDN Phonebook
		TRequestStatus status2;
		iSession.DoSynchronisation(status2, KUidIccGlobalSdnPhonebook);
		User::WaitForRequest(status2);
		err2 = status2.Int();

		// Global LND Phonebook
		TRequestStatus status3;
		iSession.DoSynchronisation(status3, KUidIccGlobalLndPhonebook);
		User::WaitForRequest(status3);
		err3 = status3.Int();

		// Usim App ADN Phonebook
		TRequestStatus status4;
		iSession.DoSynchronisation(status4, KUidUsimAppAdnPhonebook);
		User::WaitForRequest(status4);
		err4 = status4.Int();

		// Global FDN Phonebook
		TRequestStatus status5;
		iSession.DoSynchronisation(status5, KUidIccGlobalFdnPhonebook);
		User::WaitForRequest(status5);
		err5 = status5.Int();

		// Allow the server sometime to run, otherwise it will be staved of CPU!
		User::After(1);
		}
	TESTCHECKL(err1, KErrNone);
	TESTCHECKL(err2, KErrNone);
	TESTCHECKL(err3, KErrNone);
	TESTCHECKL(err4, KErrNone);
	TESTCHECKL(err5, KErrNone);

	// Verify cache is valid for all phonebooks
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncConsecutiveTestBase object
 */
CPhbkSyncConsecutiveTestBase* CPhbkSyncConsecutiveTestBase::NewL()
	{
	CPhbkSyncConsecutiveTestBase* self = new(ELeave) CPhbkSyncConsecutiveTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncConsecutiveTestBase::CPhbkSyncConsecutiveTestBase()
	{
	SetTestStepName(_L("SyncConsecutiveTest"));
	}

/**
 * Consecutive calls to DoSync().
 */ 
enum TVerdict CPhbkSyncConsecutiveTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(1); 

	SetSyncModeL(RPhoneBookSession::EManual); // Make sure sync mode is set to Manual 

	SetSimTsyTestNumberL(1); 
	TRAPD(err, DoSyncL());
	TRAP(err, DoSyncL());
	TRAP(err, DoSyncL());
	TRAP(err, DoSyncL());
	TRAP(err, DoSyncL());

	SetSyncModeL(RPhoneBookSession::EManual); // Set mode to manual for all phonebooks to avoid getting KErrInUse
	CheckSyncModeL(RPhoneBookSession::EManual); // it might take some time to sync entries from this config 
	SetSimTsyTestNumberL(0); 
	TRAP(err, DoSyncL());

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncFailureTestBase object
 */
CPhbkSyncFailureTestBase* CPhbkSyncFailureTestBase::NewL()
	{
	CPhbkSyncFailureTestBase* self = new(ELeave) CPhbkSyncFailureTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncFailureTestBase::CPhbkSyncFailureTestBase()
	{
	SetTestStepName(_L("SyncFailureTest"));
	}

/** 
 * Synchronisation Failure.
 * SIM is wearing out. Synchronisation mode is "Auto Sync, Current ICC".
 * Synchronisation starts but one of the reads fails halfway through.
 */
enum TVerdict CPhbkSyncFailureTestBase::doTestStepL()
	{
	RPhoneBookSession::TSyncState state;
	TRequestStatus status;

	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	SetSimTsyTestNumberL(3); // This should kick off Auto Sync, please note that this config 
							 // contains only Global/GSM ADN phonebook 
	
	User::After(10000000); // Wait 10s to ensure no Synchronisation has been done
	iSession.GetPhoneBookCacheState(state); // Cache is unavailable
	TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync);

	User::After(5000000); // Wait 5s e.g. for database to become unlocked or similar
	   				      // just in case an error occured during synchronisation
	iSession.DoSynchronisation(status); // Auto synchronisation failed so try manual sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	iSession.GetPhoneBookCacheState(state);
	while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable
		{
		iSession.GetPhoneBookCacheState(state);
		User::After(1000000);
		}
	TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state, 
													   // but only for Global/GSM ADN phonebook
	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncSATUpdatesTestBase object
 */
CPhbkSyncFailureAndCheckErrorTestBase* CPhbkSyncFailureAndCheckErrorTestBase::NewL()
	{
	CPhbkSyncFailureAndCheckErrorTestBase* self = new(ELeave) CPhbkSyncFailureAndCheckErrorTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncFailureAndCheckErrorTestBase::CPhbkSyncFailureAndCheckErrorTestBase()
	{
	SetTestStepName(_L("SyncFailureAndCheckErrorTest"));
	}

/** 
 * Synchronisation Failure.
 * SIM is wearing out. Synchronisation mode is "Auto Sync, Current ICC".
 * Synchronisation starts but one of the reads fails halfway through.
 */
enum TVerdict CPhbkSyncFailureAndCheckErrorTestBase::doTestStepL()
	{
	RPhoneBookSession::TSyncState state;
	TRequestStatus status, notifStatus;
	
	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	SetSimTsyTestNumberL(3); // This should kick off Auto Sync
	User::After(10000000); // Wait 10s to ensure no Synchronisation has been done
	iSession.GetPhoneBookCacheState(state); // Cache is unavailable
	TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync);
	TInt syncError;
	iSession.GetLastSyncError(syncError);
	TESTCHECKL(syncError, KErrBadHandle);

	User::After(5000000); // Wait 5s e.g. for database to become unlocked or similar
	   				      // just in case an error occured during synchronisation
	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.DoSynchronisation(status); // Auto synchronisation failed so try manual sync
	User::WaitForRequest(status);
	User::WaitForRequest(notifStatus);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(notifStatus.Int(), KErrNone);

	iSession.GetPhoneBookCacheState(state);
	while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable
		{
		iSession.GetPhoneBookCacheState(state);
		User::After(1000000);
		}
	TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncNotificationTestBase object
 */
CPhbkSyncNotificationTestBase* CPhbkSyncNotificationTestBase::NewL()
	{
	CPhbkSyncNotificationTestBase* self = new(ELeave) CPhbkSyncNotificationTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncNotificationTestBase::CPhbkSyncNotificationTestBase()
	{
	SetTestStepName(_L("SyncNotificationTest"));
	}

enum TVerdict CPhbkSyncNotificationTestBase::doTestStepL()
	{
	RPhoneBookSession::TSyncState state; // First check that initial Synchronisation was successful 

	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	
	SetSimTsyTestNumberL(0); // This should ensure no Auto Sync

	TRequestStatus status, notifStatus;
	TInt syncError;
	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.DoSynchronisation(status); // Auto synchronisation failed so try manual sync
	iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);
	User::WaitForRequest(notifStatus);
	//notification can't be cancelled at this stage(server reset to unsybc before syncing)

	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);
	User::WaitForRequest(status);//sync has completed cache is valid
	User::WaitForRequest(notifStatus);
	TESTCHECKL(status.Int(), KErrNone);
	TESTCHECKL(notifStatus.Int(), KErrCancel);//notification never completes
	//notification has been cancelled.
	iSession.GetPhoneBookCacheState(state); 
	TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache now in valid state 

//sync will fail
	SetSimTsyTestNumberL(3);
	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.DoSynchronisation(status); // Auto synchronisation failed so try manual sync
	User::WaitForRequest(notifStatus);

	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.GetPhoneBookCacheState(state);

	if(state==RPhoneBookSession::EErrorDuringSync)
		{
		iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);
		User::WaitForRequest(notifStatus);
		User::WaitForRequest(status);
		iSession.GetLastSyncError(syncError);
		TESTCHECKL(syncError, KErrBadHandle);//the sync has completed
		}
	if(state==RPhoneBookSession::EUnsynchronised)
		{
		User::WaitForRequest(notifStatus);
		User::WaitForRequest(status);
		iSession.GetPhoneBookCacheState(state);
		TESTCHECKL(state, RPhoneBookSession::EErrorDuringSync); // Cache now in valid state 
		iSession.GetLastSyncError(syncError);
		TESTCHECKL(syncError, KErrBadHandle);//the sync has completed
		}

//sync will be successfull
	SetSimTsyTestNumberL(1);
	iSession.NotifyPhBkCacheStateChange(notifStatus);
	iSession.DoSynchronisation(status); // Auto synchronisation failed so try manual sync
	
	User::WaitForRequest(notifStatus);

	iSession.NotifyPhBkCacheStateChange(notifStatus);	
	iSession.GetPhoneBookCacheState(state); 
	
	if(state==RPhoneBookSession::ECacheValid)
		{
		iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);
		TESTCHECKL(status.Int(), KErrNone);//the sync has completed
		User::WaitForRequest(notifStatus);
		User::WaitForRequest(status);
		}
	if(state==RPhoneBookSession::EUnsynchronised)
		{
		User::WaitForRequest(notifStatus);
		User::WaitForRequest(status);
		iSession.GetPhoneBookCacheState(state);
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache now in valid state 
		TESTCHECKL(status.Int(), KErrNone);//the sync has completed
		}

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncDeleteDBTestBase object
 */
CPhbkSyncDeleteDBTestBase* CPhbkSyncDeleteDBTestBase::NewL()
	{
	CPhbkSyncDeleteDBTestBase* self = new(ELeave) CPhbkSyncDeleteDBTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncDeleteDBTestBase::CPhbkSyncDeleteDBTestBase()
	{
	SetTestStepName(_L("SyncDeleteDBTest"));
	}

/** 
 * Test deleting Contacts DB and requesting manual synchronisation. 
 */
enum TVerdict CPhbkSyncDeleteDBTestBase::doTestStepL()
	{
	SetSyncModeL(RPhoneBookSession::EManual);

	CheckSyncModeL(RPhoneBookSession::EManual);
	// First reset all auto settings as they are not needed anymore
	SetSimTsyTestNumberL(1);
	DoSyncL();
	SetSimTsyTestNumberL(0); // This method will close session, delete Contacts DB, 
							// re-connect again and create Contacts database
	DoSyncL(); // Do manual synchronisation

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncEmptyICCTestBase object
 */
CPhbkSyncEmptyICCTestBase* CPhbkSyncEmptyICCTestBase::NewL()
	{
	CPhbkSyncEmptyICCTestBase* self = new(ELeave) CPhbkSyncEmptyICCTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncEmptyICCTestBase::CPhbkSyncEmptyICCTestBase()
	{
	SetTestStepName(_L("SyncEmptyICCTest"));
	}

/**
 * Test syncing to Empty ICC
 */
enum TVerdict CPhbkSyncEmptyICCTestBase::doTestStepL()
	{
	RPhoneBookSession::TSyncState state; // First check that initial Synchronisation was successful 
	iSession.GetPhoneBookCacheState(state); 

	SetSimTsyTestNumberL(14);

	SetSyncModeL(RPhoneBookSession::EManual);

	DoSyncL();

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncICCSlotsManipulationTestBase object
 */
CPhbkSyncICCSlotsManipulationTestBase* CPhbkSyncICCSlotsManipulationTestBase::NewL()
	{
	CPhbkSyncICCSlotsManipulationTestBase* self = new(ELeave) CPhbkSyncICCSlotsManipulationTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncICCSlotsManipulationTestBase::CPhbkSyncICCSlotsManipulationTestBase()
	{
	SetTestStepName(_L("SyncICCSlotsManipulationTest"));
	}

/**
 * Test syncing and condition of free slots
 */
enum TVerdict CPhbkSyncICCSlotsManipulationTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(21);
	DoSyncL();

	CheckSlotManipulationL(KUidIccGlobalAdnPhonebook);
	CheckSlotManipulationL(KUidIccGlobalSdnPhonebook);
	CheckSlotManipulationL(KUidIccGlobalLndPhonebook);
	CheckSlotManipulationL(KUidUsimAppAdnPhonebook);
	CheckSlotManipulationL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncICCSlotsManipulationAndWriteTestBase object
 */
CPhbkSyncICCSlotsManipulationAndWriteTestBase* CPhbkSyncICCSlotsManipulationAndWriteTestBase::NewL()
	{
	CPhbkSyncICCSlotsManipulationAndWriteTestBase* self = new(ELeave) CPhbkSyncICCSlotsManipulationAndWriteTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncICCSlotsManipulationAndWriteTestBase::CPhbkSyncICCSlotsManipulationAndWriteTestBase()
	{
	SetTestStepName(_L("SyncICCSlotsManipulationAndWriteTest"));
	}

/**
 * Test syncing and writing to free slots
 */
enum TVerdict CPhbkSyncICCSlotsManipulationAndWriteTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(21);
	DoSyncL();

	CheckSlotManipulationAndWriteL(KUidIccGlobalAdnPhonebook);
	CheckSlotManipulationAndWriteL(KUidIccGlobalSdnPhonebook);
	CheckSlotManipulationAndWriteL(KUidIccGlobalLndPhonebook);
	CheckSlotManipulationAndWriteL(KUidUsimAppAdnPhonebook);
	CheckSlotManipulationAndWriteL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}


/**
 * Check whether this name exists in the Contact Model Database.
 */
void CPhbkSyncIntegrationTestBase::CheckNameInDBL(const TDesC& aName)
	{
	CContactItemFieldDef* fieldDef=new(ELeave) CContactItemFieldDef;
	CleanupStack::PushL(fieldDef);
	fieldDef->AppendL(KUidContactFieldFamilyName); // define subset of fields to search
	CContactIdArray* array = iDb->FindLC(aName,fieldDef);
	TESTCHECKCONDITIONL(array->Count() != 0);
	CleanupStack::PopAndDestroy(2,fieldDef);
	}


/**
 * Check free slots
 */
void CPhbkSyncIntegrationTestBase::CheckSlotManipulationL(const TUid aPhonebook)
	{
	TInt err;
	TInt aSlot;
	TContactItemId aId=KNullContactId;
	TInt numSlots; 
	RArray<TInt> freeSlotArray;
	CleanupClosePushL(freeSlotArray);
	RArray<TInt> expectedFreeSlots; //create an array that contains the expected free slots
	CleanupClosePushL(expectedFreeSlots);

	expectedFreeSlots.Append(17);
	expectedFreeSlots.Append(18);
	expectedFreeSlots.Append(19);

	// Usim App phonebook is smaller than others
	if (aPhonebook != KUidUsimAppAdnPhonebook)
		{
		TInt i;
		for (i=21;i<=38;i++)
			expectedFreeSlots.Append(i);
		TInt ii;
		for (ii=42;ii<=50;ii++)
			expectedFreeSlots.Append(ii);

		err = iSession.GetNumSlots(numSlots, aPhonebook);
		TESTCHECK(numSlots, 50);
		}
	else
		{
		err = iSession.GetNumSlots(numSlots, aPhonebook);
		TESTCHECK(numSlots, 20);
		}

	iSession.GetFreeSlotsL(freeSlotArray, aPhonebook);

	TESTCHECKL(freeSlotArray.Count(), expectedFreeSlots.Count());
	TInt j;
	for (j=0;j<freeSlotArray.Count();j++)
		{
		TESTCHECKL(expectedFreeSlots[j], freeSlotArray[j]);
		}

	aSlot = freeSlotArray[0]-1;			// we know there are non empty slots before the empty ones
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	TESTCHECKCONDITIONL(aId > KNullContactId);
	
	aSlot = freeSlotArray[0];
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNotFound);

	CleanupStack::PopAndDestroy(2); // freeSlotArray & expectedFreeSlots
	}


/**
 * Check writing to free slots
 */
void CPhbkSyncIntegrationTestBase::CheckSlotManipulationAndWriteL(const TUid aPhonebook)
	{
	TInt err;
	TInt aSlot;
	TContactItemId aId=KNullContactId;
	TInt numSlots; 

	CPhbkIntegrationTestUtility* add = CPhbkIntegrationTestUtility::NewL();
	_LIT(Kslot17,"17");
	_LIT(Kslot18,"18");

	// Usim App phonebook is smaller than others
	if (aPhonebook != KUidUsimAppAdnPhonebook)
		{
		err = iSession.GetNumSlots(numSlots, aPhonebook);
		TESTCHECK(numSlots, 50);
		}
	else
		{
		err = iSession.GetNumSlots(numSlots, aPhonebook);
		TESTCHECK(numSlots, 20);
		}
	
	//test add new contact at specified slot
	TInt matchingId;
	aSlot=17;
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNotFound);

	add->AddContactL(KTestName, KTestNumber, Kslot17, aPhonebook, KTestEMail);
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	matchingId=aId;

	//edit existing contact name
	iDb->SetDbViewContactType(KUidContactICCEntry);
	CContactICCEntry* entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(aId));
	CleanupStack::PushL(entry);
	add->ChangeNameFieldL(*entry,KModifiedName);
	iDb->CommitContactL(*entry);
	CleanupStack::PopAndDestroy(2);
	
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	TESTCHECKL(aId, matchingId);

	//edit slot number
	iDb->SetDbViewContactType(KUidContactICCEntry);
	entry = static_cast<CContactICCEntry*>(iDb->OpenContactLX(aId));
	CleanupStack::PushL(entry);

	CContactItemFieldSet& fieldset = entry->CardFields();
	const TInt pos(fieldset.Find(KUidContactFieldICCSlot));
	CContactItemField& field = fieldset[pos];
	CContactTextField* textfield = field.TextStorage();
	textfield->SetTextL(Kslot18);
	iDb->CommitContactL(*entry);
	CleanupStack::PopAndDestroy(2);

	//check slot 17 is empty
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNotFound);
	
	//check slot 18 is not empty
	aSlot=18;
	err = iSession.GetSlotId(aSlot, aId, aPhonebook);
	TESTCHECKL(err, KErrNone);
	TESTCHECKL(aId, matchingId);

	delete add;
	}

/************************* STRESS TESTS ******************************/

/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncMultipleRequestsTestBase object
 */
CPhbkSyncMultipleRequestsTestBase* CPhbkSyncMultipleRequestsTestBase::NewL()
	{
	CPhbkSyncMultipleRequestsTestBase* self = new(ELeave) CPhbkSyncMultipleRequestsTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncMultipleRequestsTestBase::CPhbkSyncMultipleRequestsTestBase()
	{
	SetTestStepName(_L("SyncMultipleRequestsTest"));
	}

/**
 * Test handling of the multiple phonebook synchronisation requests.
 */
enum TVerdict CPhbkSyncMultipleRequestsTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(13);
	INFO_PRINTF1(_L("  First Sync"));

	DoSyncL();
	RPhoneBookSession session;
	User::LeaveIfError(session.Connect()); // Second connection to the Phonebook Server

	INFO_PRINTF1(_L("  Second Sync"));
	TRequestStatus status1, status2, status3;
	session.DoSynchronisation(status1); // Start first synchronisation
	session.DoSynchronisation(status2); // Start second synchronisation
	session.DoSynchronisation(status3); // Start third synchronisation
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	TESTCHECKL(status1.Int(), KErrNone);
	TESTCHECKL(status2.Int(), KErrInUse);
	TESTCHECKL(status3.Int(), KErrInUse);

	INFO_PRINTF1(_L("  Third Sync"));
	TRequestStatus status4;
	session.DoSynchronisation(status4); // Now try to do another synchronisation
	User::WaitForRequest(status4);
	TESTCHECKL(status4.Int(), KErrNone); // This one should complete successfully

	INFO_PRINTF1(_L("  Fourth Sync"));
	TRequestStatus status5, status6;
	session.DoSynchronisation(status5); // Now try multiple synchronisation requests again
	session.DoSynchronisation(status6); // Start second synchronisation
	User::WaitForRequest(status5);
	User::WaitForRequest(status6);
	TESTCHECKL(status5.Int(), KErrNone);
	TESTCHECKL(status6.Int(), KErrInUse);

	INFO_PRINTF1(_L("  Fifth Sync"));
	TRequestStatus status7;
	session.DoSynchronisation(status7); // Now try to do another synchronisation
	User::WaitForRequest(status7);
	TESTCHECKL(status7.Int(), KErrNone); // This one should complete successfully

	session.Close(); // Close second connection to the server

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncGeneralResponseTestBase object
 */
CPhbkSyncGeneralResponseTestBase* CPhbkSyncGeneralResponseTestBase::NewL()
	{
	CPhbkSyncGeneralResponseTestBase* self = new(ELeave) CPhbkSyncGeneralResponseTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncGeneralResponseTestBase::CPhbkSyncGeneralResponseTestBase()
	{
	SetTestStepName(_L("SyncGeneralResponseTest"));
	}

/**
 *  Synchronises a phonebook.  The length of time to sync and optionally the
 *  longest time taken to call GetPhoneBookCacheState() are measured.  The
 *  function will fail the test if the longest call is greater than 15% of
 *  the sync time (e.g. the server is unresponsive).
 *
 *  @param aPhonebookUid   Phonebook to synchronise.
 *  @param aTestResponse   If true, the response time of the server will be measured.
 *  @param aPhonebookName  Textual representation of the phonebook name.
 */
void CPhbkSyncGeneralResponseTestBase::DoSyncAndTestServerResponceL(TUid aPhonebookUid,
																	TBool aTestResponse,
																	const TDesC& aPhonebookName)
	{
	RPhoneBookSession::TSyncState  state(RPhoneBookSession::EUnsynchronised);
	TRequestStatus  status; 
	TTime  startOfSync, endOfSync;
	TInt64  lengthOfSync(0), longestCall(0);

	//
	// Request sync...
	//
	startOfSync.UniversalTime();
	iSession.DoSynchronisation(status, aPhonebookUid);

	//
	// Either monitor the responsiveness or just wait for the sync to
	// complete...
	//
	if (aTestResponse != EFalse)
		{
		//
		// Wait if needed, checking every 0.1 seconds...
		//
		while (state == RPhoneBookSession::EUnsynchronised) 
			{
			TTime  start, end;
			TInt  result;

			start.UniversalTime();
			result = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
			end.UniversalTime();
			if (result != KErrNone)
				{
				state = RPhoneBookSession::EUnsynchronised;
				}

			TInt64  lengthOfCall = end.MicroSecondsFrom(start).Int64();

			if (lengthOfCall > longestCall)
				{
				longestCall = lengthOfCall;
				}

			//
			// If not finished then wait a short amount of time.
			//
			if (state == RPhoneBookSession::EUnsynchronised)
				{
				User::After(1); // Will become the minimum time period on the device.
				}
			}
		}

	//
	// Complete the sync request...
	//
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);

	endOfSync.UniversalTime();
	lengthOfSync = endOfSync.MicroSecondsFrom(startOfSync).Int64();

	//
	// Check the sync was successful...
	//
	TInt  result;
	
	result = iSession.GetPhoneBookCacheState(state, aPhonebookUid);
	TESTCHECK(result, KErrNone);
	TESTCHECK(state, RPhoneBookSession::ECacheValid);

	if (aTestResponse != EFalse)
		{
		//
		// Check that the longest time taken to call GetPhoneBookCacheState() (e.g. the
		// latency in the server handling new requests) is less than 25% of the total
		// time to sync.  On average the time to delete the unconfirmed contacts will be
		// at least 75% of the sync time.
		//
		TInt64  callLimit((lengthOfSync * 25) / 100);
		
		INFO_PRINTF4(_L("  Length of %S sync was %.3fs and the longest call was %.3fs."),
					 &aPhonebookName, I64REAL(lengthOfSync) / 1000000.0,
					 I64REAL(longestCall) / 1000000.0);

		if (longestCall > callLimit)
			{
			INFO_PRINTF3(_L("    Longest call of %.3fs was not within the limit of %.3fs."),
						 I64REAL(longestCall) / 1000000.0,
						 I64REAL(callLimit) / 1000000.0);
			SetTestStepResult(EFail);		
			}
		}
	else
		{
		//
		// Just print the total sync time...
		//
		INFO_PRINTF3(_L("  Length of %S sync was %.3fs without response testing."),
					 &aPhonebookName, I64REAL(lengthOfSync) / 1000000.0);
		}
	}

/** 
 *  Test synchronising a SIM with a different number of slots such that the PBS Server uses a
 *  CIdle object to complete the operation in the background.  It should be possible for the
 *  PBS Server to handle requests frequently before the sync completes.
 */
enum TVerdict CPhbkSyncGeneralResponseTestBase::doTestStepL()
	{
	//
	// Run this test at high priority to ensure that the timings are not affected
	// by other threads
	//
	RThread().SetPriority(EPriorityAbsoluteHigh);

	//
	// Sync with a SIM which has 22 entries used out of 25 for each of the 5 phonebooks...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(24);
	DoSyncL();
	WaitForSyncToCompleteL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest24ICCSlotsUsed);

	//
	// Start sync'ing to a SIM which has 3 entries used out of 15 for each of the 5 phonebooks.
	// Wait for the synchronisation to complete, but monitor the maximum length of time that
	// it takes for the GetPhoneBookCacheState() to complete...
	//
	SetSimTsyTestNumberL(25);
	DoSyncAndTestServerResponceL(KUidIccGlobalAdnPhonebook, ETrue, _L("ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalSdnPhonebook, ETrue, _L("SDN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalLndPhonebook, ETrue, _L("LND"));
	DoSyncAndTestServerResponceL(KUidUsimAppAdnPhonebook, ETrue, _L("USIM ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalFdnPhonebook, ETrue, _L("FDN"));

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest25ICCSlotsUsed);

	//
	// Now perform the reverse case and start sync'ing to a SIM which has 22 entries used out
	// of 25 for each of the 5 phonebooks...
	//
	SetSimTsyTestNumberL(24);
	DoSyncAndTestServerResponceL(KUidIccGlobalAdnPhonebook, ETrue, _L("ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalSdnPhonebook, ETrue, _L("SDN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalLndPhonebook, ETrue, _L("LND"));
	DoSyncAndTestServerResponceL(KUidUsimAppAdnPhonebook, ETrue, _L("USIM ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalFdnPhonebook, ETrue, _L("FDN"));

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest24ICCSlotsUsed);

	//
	// Start sync'ing to a SIM which has 3 entries used out of 15 for each of the 5 phonebooks.
	// Time how long the sync takes.
	//
	SetSimTsyTestNumberL(25);
	DoSyncAndTestServerResponceL(KUidIccGlobalAdnPhonebook, EFalse, _L("ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalSdnPhonebook, EFalse, _L("SDN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalLndPhonebook, EFalse, _L("LND"));
	DoSyncAndTestServerResponceL(KUidUsimAppAdnPhonebook, EFalse, _L("USIM ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalFdnPhonebook, EFalse, _L("FDN"));

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest25ICCSlotsUsed);

	//
	// Now perform the reverse case and start sync'ing to a SIM which has 22 entries used out
	// of 25 for each of the 5 phonebooks...
	//
	SetSimTsyTestNumberL(24);
	DoSyncAndTestServerResponceL(KUidIccGlobalAdnPhonebook, EFalse, _L("ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalSdnPhonebook, EFalse, _L("SDN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalLndPhonebook, EFalse, _L("LND"));
	DoSyncAndTestServerResponceL(KUidUsimAppAdnPhonebook, EFalse, _L("USIM ADN"));
	DoSyncAndTestServerResponceL(KUidIccGlobalFdnPhonebook, EFalse, _L("FDN"));

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest24ICCSlotsUsed);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncGetAndContactFormatTestBase object
 */
CPhbkSyncGetAndContactFormatTestBase* CPhbkSyncGetAndContactFormatTestBase::NewL()
	{
	CPhbkSyncGetAndContactFormatTestBase* self = new(ELeave) CPhbkSyncGetAndContactFormatTestBase();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncGetAndContactFormatTestBase::CPhbkSyncGetAndContactFormatTestBase()
	{
	SetTestStepName(_L("SyncAndGetContactFormatTest"));
	}

/**
 *  Test GetContactFormat and GetNumSlots
 */
enum TVerdict CPhbkSyncGetAndContactFormatTestBase::doTestStepL()
	{
	SetSimTsyTestNumberL(0);

	RPhoneBookSession::TContactFieldsV1 fields;
	RPhoneBookSession::TContactFieldsV1Pckg fieldsPckg(fields);
	TInt  err, numSlots;
	
	err = iSession.GetContactFormat(fieldsPckg);
	TESTCHECK(err, KErrNone);
	err = iSession.GetNumSlots(numSlots);
	TESTCHECK(err, KErrNone);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncSetSyncModeAndAutoSyncTest object
 */
CPhbkSyncSetSyncModeAndAutoSyncTest* CPhbkSyncSetSyncModeAndAutoSyncTest::NewL()
	{
	CPhbkSyncSetSyncModeAndAutoSyncTest* self = new(ELeave) CPhbkSyncSetSyncModeAndAutoSyncTest();
	return self;
	} // CPhbkSyncSetSyncModeAndAutoSyncTest::NewL

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncSetSyncModeAndAutoSyncTest::CPhbkSyncSetSyncModeAndAutoSyncTest()
	{
	SetTestStepName(_L("SetSyncModeAndAutoSyncTest"));
	} // CPhbkSyncSetSyncModeAndAutoSyncTest::CPhbkSyncSetSyncModeAndAutoSyncTest


/**
 *  Test SetSyncMode for an auto-sync.
 */
enum TVerdict CPhbkSyncSetSyncModeAndAutoSyncTest::doTestStepL()
	{
	//
	// First make sure sync mode is manual sync...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);

	//
	// Now restart with config 0. No syncs will be performed...
	//
	SetSimTsyTestNumberL(0);

	//
	// Set all phonebooks to Auto-sync...
	//
	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	//
	// Now wait for sync to be complete on all phonebooks...
	//
	WaitForSyncToCompleteL();

	return TestStepResult();
	} // CPhbkSyncSetSyncModeAndAutoSyncTest::doTestStepL


/** 
 * Factory construction method.
 * @return Pointer to CPhbkSyncSyncUnsupportedPhonebook object
 */
CPhbkSyncSyncUnsupportedPhonebook* CPhbkSyncSyncUnsupportedPhonebook::NewL()
	{
	CPhbkSyncSyncUnsupportedPhonebook* self = new(ELeave) CPhbkSyncSyncUnsupportedPhonebook();
	return self;
	} // CPhbkSyncSyncUnsupportedPhonebook::NewL


/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkSyncSyncUnsupportedPhonebook::CPhbkSyncSyncUnsupportedPhonebook()
	{
	SetTestStepName(_L("SyncUnsupportedPhonebook"));
	} // CPhbkSyncSyncUnsupportedPhonebook::CPhbkSyncSyncUnsupportedPhonebook


/**
 *  Test sync'ing when a phonebook is not supported.
 */
enum TVerdict CPhbkSyncSyncUnsupportedPhonebook::doTestStepL()
	{
	//
	// Manual sync with a SIM which has a few entries on each phonebook...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(25);
	DoSyncL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest25ICCSlotsUsed);

	//
	// Now change to a SIM with just ADN. Total entries should be just the
	// number in the ADN phonebook (e.g. all the other phonebooks would have
	// cleared).
	//
	TRequestStatus status; 
	RPhoneBookSession::TSyncState state;

	SetSimTsyTestNumberL(26);
	
	TInt  ret = iSession.GetPhoneBookCacheState(state);
	TESTCHECK(ret, KErrNone);
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNone);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalSdnPhonebook);
	TESTCHECK(ret, KErrNotFound);
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNotFound);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalLndPhonebook);
	TESTCHECK(ret, KErrNotFound);
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNotFound);

	ret = iSession.GetPhoneBookCacheState(state, KUidUsimAppAdnPhonebook);
	TESTCHECK(ret, KErrNotFound);
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNotFound);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalFdnPhonebook);
	TESTCHECK(ret, KErrNotFound);
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNotFound);
	
	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest26ICCSlotsUsed);

	//
	// Repeat the test with auto-sync on a SIM which has a few entries on
	// each phonebook. Need to swap to this SIM twice - once to set the auto-sync
	// mode and once to test it.
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(25);
	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	SetSimTsyTestNumberL(25);
	WaitForSyncToCompleteL();

	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest25ICCSlotsUsed);

	//
	// Now change to a SIM with just ADN. Total entries should be just the
	// number in the ADN phonebook (e.g. all the other phonebooks would have
	// cleared).
	//
	SetSimTsyTestNumberL(26);
	User::After(30*1000000); // Wait for silent sync of unknown phonebooks
	
	iDb->SetDbViewContactType(KUidContactICCEntry);
	sortedItems = iDb->SortedItemsL();
	TESTCHECK(sortedItems->Count(), KTest26ICCSlotsUsed);

	return TestStepResult();
	} // CPhbkSyncSyncUnsupportedPhonebook::doTestStepL


/**
 *  Perform repeated syncs of phonebooks which have number categories assigned.
 *  A previous defect found that the UID list overflowed when frequently updated.
 */ 
enum TVerdict CPhbkSyncSyncDiffNumberTypesRepeatedly::doTestStepL()
	{
	SetSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(23); 

	for (TInt count = 0;  count < 10;  count++)
		{
		SetSimTsyTestNumberL(22); 
		DoSyncL();
		SetSimTsyTestNumberL(23); 
		DoSyncL();
		}

	return TestStepResult();
	} // CPhbkSyncSyncDiffNumberTypesRepeatedly::doTestStepL


/**
 *  Perform a sync of contacts with the plus symbol used in the international numbers.
 */ 
enum TVerdict CPhbkSyncSyncInternationalNumbers::doTestStepL()
	{
	//
	// Setup the SIMTSY and sync all phonebooks...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(29); 
	DoSyncL();

	//
	// Read all the numbers and ensure they start with a '+'...
	//
	iDb->SetDbViewContactType(KUidContactICCEntry);
	const CContactIdArray* sortedItems = iDb->SortedItemsL();
	TInt  count(sortedItems->Count());
	TESTCHECK(count, KTest29ICCSlotsUsed);

	for (TInt index = 0;  index < count;  index++)
		{ 
		CContactICCEntry*  entry = static_cast<CContactICCEntry*>(iDb->ReadContactL((*sortedItems)[index])); 

		TESTCHECKCONDITIONL(entry != NULL);
		CleanupStack::PushL(entry);

		CContactItemFieldSet&  fieldSet = entry->CardFields();
		TInt  pos;
		
		pos = fieldSet.Find(KUidContactFieldVCardMapVOICE);
		TESTCHECKCONDITION(pos >= 0);

		while (pos != KErrNotFound)
			{
			CContactTextField*  text = fieldSet[pos].TextStorage();

			TESTCHECK(text->Text()[0], '+');

			pos = fieldSet.FindNext(KUidContactFieldVCardMapVOICE, pos + 1);
			}

		CleanupStack::PopAndDestroy(entry);
		}

	return TestStepResult();
	} // CPhbkSyncSyncInternationalNumbers::doTestStepL


/**
 *  Perform a sync of contacts with the plus symbol used in the international numbers.
 */ 
enum TVerdict CPhbkSyncSyncAutomaticWithUnsupportedPhonebook::doTestStepL()
	{
	//
	// Set all settings to manual and setup the SIM to have only FDN...
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);

	SetSimTsyTestNumberL(31);

	//
	// Setup notifications for all the phonebooks...
	//
	TRequestStatus  status1, status2, status3, status4, status5;

	iSession.NotifyPhBkCacheStateChange(status1, KUidIccGlobalAdnPhonebook);
	iSession.NotifyPhBkCacheStateChange(status2, KUidIccGlobalSdnPhonebook);
	iSession.NotifyPhBkCacheStateChange(status3, KUidIccGlobalLndPhonebook);
	iSession.NotifyPhBkCacheStateChange(status4, KUidUsimAppAdnPhonebook);
	iSession.NotifyPhBkCacheStateChange(status5, KUidIccGlobalFdnPhonebook);

	//
	// Set all the phonebooks to auto-sync...
	//
	SetSyncModeL(RPhoneBookSession::EAutoCurrentIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc);

	//
	// Wait for the notifications to trigger...
	//
	User::WaitForRequest(status1);
	User::WaitForRequest(status2);
	User::WaitForRequest(status3);
	User::WaitForRequest(status4);
	User::WaitForRequest(status5);

	TESTCHECK(status1.Int(), KErrNone);
	TESTCHECK(status2.Int(), KErrNone);
	TESTCHECK(status3.Int(), KErrNone);
	TESTCHECK(status4.Int(), KErrNone);
	TESTCHECK(status5.Int(), KErrNone);

	//
	// Now check the final sync state and the Last Sync Error...
	// 
	RPhoneBookSession::TSyncState  state;
	TInt syncError;

	User::LeaveIfError(iSession.GetPhoneBookCacheState(state, KUidIccGlobalFdnPhonebook));
	TESTCHECK(state, RPhoneBookSession::ECacheValid);
	User::LeaveIfError(iSession.GetLastSyncError(syncError, KUidIccGlobalFdnPhonebook));
	TESTCHECK(syncError, KErrNone);

	return TestStepResult();
	} // CPhbkSyncSyncAutomaticWithUnsupportedPhonebook::doTestStepL


