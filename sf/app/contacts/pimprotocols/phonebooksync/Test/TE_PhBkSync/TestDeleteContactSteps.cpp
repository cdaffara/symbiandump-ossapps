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
// This contains PhbkSync API DeleteContact() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestDeleteContactSteps.h"

/** 
 * Test deleting a contact by specifying a valid ICC slot.
 * Repeat for all supported phonebooks.
 */
//
//    Test step1 -- Test#73 in Unit Test Spec. Doc. //
//
CPhbkSyncDeleteContactTest_01::CPhbkSyncDeleteContactTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_01"));
	}

CPhbkSyncDeleteContactTest_01::~CPhbkSyncDeleteContactTest_01()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_01::doTestStepL( )
	{
	TContactItemId id;
	TInt result;
	TInt i(0);
	TRequestStatus status;

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. re-use test prepared for Test#37 which contains all phonebooks.
	//    All phonebooks should be synchronised automatically after this.
	SetSimTsyTestNumberL(37);

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. test deleting a valid ICC slot
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		iSession.DeleteContact(status, id);
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrNone);
		// check expected behaviour (entry has been deleted)
		result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
		TESTCHECKL(result, KErrNotFound);
		}

	return TestStepResult();
	}

/**
 * Test deleting a contact by specifying a valid ICC slot 
 * which contains a blank entry.
 */
//
//    Test step2 -- Test#74 in Unit Test Spec. Doc.  //
//**
CPhbkSyncDeleteContactTest_02::CPhbkSyncDeleteContactTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_02"));
	}

CPhbkSyncDeleteContactTest_02::~CPhbkSyncDeleteContactTest_02()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_02::doTestStepL( )
	{
	TContactItemId validId, invalidId;
	TInt result;
	TInt i(0);
	TRequestStatus status;

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. re-use test prepared for Test#37 which contains all phonebooks.
	//    All phonebooks should be synchronised automatically after this.
	SetSimTsyTestNumberL(37);

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. test deleting a valid ICC slot which contains a blank entry
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validId = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		invalidId = validId + 999; // make it invalid (i.e. not exist, ugly)
		iSession.DeleteContact(status, invalidId); // should fail
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrArgument);

		// check expected behaviour (entry is still there)
		result = iSession.ValidateContact(MContactSynchroniser::ERead, validId);
		TESTCHECKL(result, KErrNone);
		}

	return TestStepResult();
	}

/**
 * Test deleting a contact by specifying an invalid ICC
 * slot. Repeat for all supported phonebooks.
 */
//
//    Test step3 -- Test#75 in Unit Test Spec. Doc.  //
//
CPhbkSyncDeleteContactTest_03::CPhbkSyncDeleteContactTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_03"));
	}

CPhbkSyncDeleteContactTest_03::~CPhbkSyncDeleteContactTest_03()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_03::doTestStepL( )
	{
	TContactItemId id;
	TRequestStatus status;
	TInt result;
	TInt i(0);

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. re-use test prepared for Test#37 which contains all phonebooks.
	//    All phonebooks should be synchronised automatically after this.
	SetSimTsyTestNumberL(37);

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. test deleting a invalid ICC slot
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		// try to delete the invalid ICC slot
		id = -3;  // Note that -1 is the default UID value in lookUpTable 
		iSession.DeleteContact(status, id);
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrArgument);

		// check expected behaviour (the phonebook still exists)
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
		TESTCHECKL(result, KErrNone);
		}

	return TestStepResult();
	}

/**
 * Test deleting a contact from a read-only (SDN) phonebook.
 * By default, SDN and LND are read-only phonebooks.
 */
//
//    Test step4 -- Test#76 in Unit Test Spec. Doc.  //
//
CPhbkSyncDeleteContactTest_04::CPhbkSyncDeleteContactTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_04"));
	}

CPhbkSyncDeleteContactTest_04::~CPhbkSyncDeleteContactTest_04()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_04::doTestStepL( )
	{
	TContactItemId id;
	TInt result;
	TInt i(0);
	TRequestStatus status;

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. load the phonebooks in which SDN and LND are read-only
	SetSimTsyTestNumberL(76); // see [test76] in configuration file

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. test deleting a valid ICC slot from read-only SDN
	id = GetValidUIDFromContactsDbL(KUidIccGlobalSdnPhonebook);
	iSession.DeleteContact(status, id);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrAccessDenied);

	// 4. check expected behaviour (the entry still there)
	result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
	TESTCHECKL(result, KErrNone);

	return TestStepResult();
	}

/**
 * Test deleting a contact, where the contact is a hidden
 * entry and the hidden key has not been verified. Repeat
 * for all supported phonebooks.
 */
//
//    Test step5 -- Test#77 in Unit Test Spec. Doc.  //
//
CPhbkSyncDeleteContactTest_05::CPhbkSyncDeleteContactTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_05"));
	}

CPhbkSyncDeleteContactTest_05::~CPhbkSyncDeleteContactTest_05()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_05::doTestStepL( )
	{
	TRequestStatus status;
	TContactItemId validContactId;  

	// 1. set mode as EManual for USim phonebook
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// load USim App ADN phonebook which contains hidden entries only
	SetSimTsyTestNumberL(77);  // 
	
	// 3. 10s after start up, the hidden entries become verified. 
	//    The Verified event will trigger the autosync operation of USim. 
	//    Here, wait for 10s before checking sync status.
	User::After(10*KOneSeconduS);

	// 4. check sync status
	RPhoneBookSession::TSyncState state;
	iSession.GetPhoneBookCacheState(state, KUidUsimAppAdnPhonebook); 
	while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
		{
		iSession.GetPhoneBookCacheState(state, KUidUsimAppAdnPhonebook);
		User::After(1000000);
		}
	TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
	
	// 5. now, all hidden entries have been put into contact DB, so it 
	//    should be possible to get one valid hidden entry UID for USim phonebook.
	validContactId = GetValidUIDFromContactsDbL(KUidUsimAppAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId != KNullContactId);      

	// 6. wait until the hidden entries are locked again, and then try to
	//    delete the hidden entry using the UID obtained.

	// 6.1 wait until the hidden entries become unverified again
	TInt result = KErrNone;
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId);
	while (result != KErrAccessDenied) 
		{
		result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId);
		User::After(1000000);
		}
	TESTCHECKL(result, KErrAccessDenied); 

	// 6.2 now let us try to delete the hidden entry from the locked USim
	iSession.DeleteContact(status, validContactId);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrAccessDenied);

	return TestStepResult();
	}

/**
 * Test deleting a contact after the phonebook has become
 * locked/blocked.
 */
//
//    Test step6 -- Test#78 in Unit Test Spec. Doc.  //
//
CPhbkSyncDeleteContactTest_06::CPhbkSyncDeleteContactTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_06"));
	}

CPhbkSyncDeleteContactTest_06::~CPhbkSyncDeleteContactTest_06()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_06::doTestStepL( )
	{
	TRequestStatus status;
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  
	TInt i(0);

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. load all phonebooks which are not locked/blocked at beginning,
	//    but they will be locked/blocked after 20s which should be long
	//    enough for the autosync to finish.
	SetSimTsyTestNumberL(78);

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// 4. wait for until the locked/blocked event to be trigged
	User::After(20*KOneSeconduS);  // 20s should be more than enough

	// 5. use the valid UIDs obtained to do DeleteContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.DeleteContact(status, validContactIds[i]);
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrAccessDenied);
		}

	return TestStepResult();
	}

/**
 * Test deleting a contact, but an error occurs when
 * deleting from the ICC.
 */
//
//    Test step7 -- Test#79 in Unit Test Spec. Doc.  //
//
CPhbkSyncDeleteContactTest_07::CPhbkSyncDeleteContactTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDeleteContactTest_07"));
	}

CPhbkSyncDeleteContactTest_07::~CPhbkSyncDeleteContactTest_07()
	{
	}

TVerdict CPhbkSyncDeleteContactTest_07::doTestStepL( )
	{
	// the events PhoneTerminated defined in TSY should be tested in
	// the same manner as test78. Unfortunately, the Terminated event
	// doesn't work in TSY. Therefore, this test is temporarily commented
	// out.

/*****
	TRequestStatus status;
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  
	TInt i(0);

	// 1. set mode as EAutoCurrentIcc for all phonebooks
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 2. load all phonebooks which are unlocked at bgining,
	//    but they will be locked/blocked after 20s which should be long
	//    enough for the autosync to finish.
	SetSimTsyTestNumberL(79);

	User::After(3000000); // Wait for a while for auto sync
	RPhoneBookSession::TSyncState state;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.GetPhoneBookCacheState(state, iPhoneBook[i]); 
		while (state==RPhoneBookSession::EUnsynchronised) // Wait until cache becomes usable and for Synchronisation to be carried out 
			{
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			User::After(1000000);
			}
		TESTCHECKL(state, RPhoneBookSession::ECacheValid); // Cache is now in valid state
		}

	// 3. get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// 4. wait for until the locked/blocked event to be trigged
	User::After(20*KOneSeconduS);  // 20s should be more than enough

	// 5. use the valid UIDs obtained to do DeleteContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.DeleteContact(status, validContactIds[i]);
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrAccessDenied);
		}
*****/

	return TestStepResult();
	}

