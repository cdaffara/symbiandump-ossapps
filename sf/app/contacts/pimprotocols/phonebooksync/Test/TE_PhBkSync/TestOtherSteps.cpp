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
// This contains PhbkSync combined Test Steps
// 
//

#include "TE_PhBkSyncBase.h"
#include "TestOtherSteps.h"

/**
 * Test adding an entry to the ADN phonebook, follwed 
 * by reading an entry from the LND phonebook and then
 * by deleting an entry from the ADN phonebook, using
 * the same CPhoneBookSession.
 */  
//
//    Test step1 -- Test#84 in Unit Test Spec. Doc. //
//
CPhbkSyncOtherTest_01::CPhbkSyncOtherTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_01"));
	}

CPhbkSyncOtherTest_01::~CPhbkSyncOtherTest_01()
	{
	}

TVerdict CPhbkSyncOtherTest_01::doTestStepL( )
	{
	DestroyDefaultIniFileL();
	TInt result,id;
	TRequestStatus status;

	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}


	// first, adding an entry to the ADN phonebook
	MyCSyncContactICCEntry* iccEntryInternal = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternal);
	InitialiseIccEntryInternalNewL(iccEntryInternal);  
	iccEntryInternal->iPhonebook = convertPhonebookUid2Enum(KUidIccGlobalAdnPhonebook);
	User::After(4000000);
	DoWriteContactToICCL(iccEntryInternal, iUidIccGlobalAdnPhonebookCallByRef, status);
	TESTCHECK(status.Int(), KErrNone);                           // successful
	TESTCHECKCONDITION(iccEntryInternal->iSlotNum!=KSyncIndexNotSupplied); // successful
	CleanupStack::PopAndDestroy(iccEntryInternal);

	// then, reading an entry from the LND phonebook
	id = GetValidUIDFromContactsDbL(KUidIccGlobalLndPhonebook);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
	TESTCHECKL(result, KErrNone);    

	// finally, deleting an entry from the ADN phonebook
	id = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	iSession.DeleteContact(status, id);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
	TESTCHECKL(result, KErrNotFound);

	return TestStepResult();
	}


/**
 * Test adding an entry to the ADN phonebook, follwed by adding 
 * another entry to the USIM App phonebook, using the same
 * CPhoneBookSession.
 */
//
//    Test step2 -- Test#85 in Unit Test Spec. Doc.  //
//
CPhbkSyncOtherTest_02::CPhbkSyncOtherTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_02"));
	}

CPhbkSyncOtherTest_02::~CPhbkSyncOtherTest_02()
	{
	}

TVerdict CPhbkSyncOtherTest_02::doTestStepL( )
	{
	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// construct a new entry
	MyCSyncContactICCEntry* iccEntryInternal = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternal);
	TRequestStatus status;
	InitialiseIccEntryInternalNewL(iccEntryInternal);  
	iccEntryInternal->iPhonebook = convertPhonebookUid2Enum(KUidIccGlobalAdnPhonebook);

	// first, adding an entry to the ADN phonebook
	DoWriteContactToICCL(iccEntryInternal, iUidIccGlobalAdnPhonebookCallByRef, status);
	TESTCHECK(status.Int(), KErrNone);                           // successful
	TESTCHECKCONDITION(iccEntryInternal->iSlotNum!=KSyncIndexNotSupplied); // successful

	// then, adding an entry to the USIM App phonebook
	iccEntryInternal->iSlotNum = KSyncIndexNotSupplied;  
	DoWriteContactToICCL(iccEntryInternal, iUidUsimAppAdnPhonebookCallByRef, status);
	TESTCHECK(status.Int(), KErrNone);                           // successful
	TESTCHECKCONDITION(iccEntryInternal->iSlotNum!=KSyncIndexNotSupplied); // successful
	CleanupStack::PopAndDestroy(iccEntryInternal);


	return TestStepResult();
	}

/**
 * Test deleting an entry to the ADN phonebook, followed
 * by deleting another entry to the USIM App phonebook, 
 * using the same CPhoneBookSession.
 */
//
//    Test step3 -- Test#86 in Unit Test Spec. Doc.  //
//
CPhbkSyncOtherTest_03::CPhbkSyncOtherTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_03"));
	}

CPhbkSyncOtherTest_03::~CPhbkSyncOtherTest_03()
	{
	}

TVerdict CPhbkSyncOtherTest_03::doTestStepL( )
	{
	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// first, deleting an entry from the ADN phonebook
	TRequestStatus status;
	TInt id, result;
	id = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	iSession.DeleteContact(status, id);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
	TESTCHECKL(result, KErrNotFound);

	// then, deleting an entry from the USIM App phonebook
	id = GetValidUIDFromContactsDbL(KUidUsimAppAdnPhonebook);
	iSession.DeleteContact(status, id);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
	TESTCHECKL(result, KErrNotFound);

	return TestStepResult();
	}

/**
 * Test adding an entry to the ADN phonebook, followed by adding 
 * antoher entry to the same ADN phonebook, using two different
 * CPhoneBookSession.
 */ 
//
//    Test step4 -- Test#87 in Unit Test Spec. Doc.  //
//
CPhbkSyncOtherTest_04::CPhbkSyncOtherTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_04"));
	}

CPhbkSyncOtherTest_04::~CPhbkSyncOtherTest_04()
	{
	}

TVerdict CPhbkSyncOtherTest_04::doTestStepL( )
	{
	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// wait until all phonebooks have been synchronised
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	// construct a new entry
	TRequestStatus status;
	MyCSyncContactICCEntry* iccEntryInternal = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternal);
	InitialiseIccEntryInternalNewL(iccEntryInternal);  
	iccEntryInternal->iPhonebook = convertPhonebookUid2Enum(KUidIccGlobalAdnPhonebook);

	// first, adding an entry to the ADN phonebook
	DoWriteContactToICCL(iccEntryInternal, iUidIccGlobalAdnPhonebookCallByRef, status);
	TESTCHECK(status.Int(), KErrNone);                           // successful
	TESTCHECKCONDITION(iccEntryInternal->iSlotNum!=KSyncIndexNotSupplied); // successful

	// then, adding an entry to the same ADN phonebook using a different 
	// RPhoneBookSession object/handle
	RPhoneBookSession secondSession;
	User::LeaveIfError(secondSession.Connect());

	iccEntryInternal->iSlotNum = KSyncIndexNotSupplied;  
	DoWriteContactToICCL(iccEntryInternal, iUidIccGlobalAdnPhonebookCallByRef,
						 status, &secondSession);
	TESTCHECK(status.Int(), KErrNone);                           // successful
	TESTCHECKCONDITION(iccEntryInternal->iSlotNum!=KSyncIndexNotSupplied); // successful

	secondSession.Close(); // close the second session

	CleanupStack::PopAndDestroy(iccEntryInternal);
	return TestStepResult();
	}

/**
 * Test the scenario that Phonebook Synchroniser Server thread is only
 * created by the first client. That is, it should still be alive after
 * closing the second client.
 */
//
//    Test step5 -- NewTest#01                      //
//
CPhbkSyncOtherTest_05::CPhbkSyncOtherTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_05"));
	}

CPhbkSyncOtherTest_05::~CPhbkSyncOtherTest_05()
	{
	}

TVerdict CPhbkSyncOtherTest_05::doTestStepL( )
	{
	RPhoneBookSession mySession;

	User::LeaveIfError(mySession.Connect());

	// test the services provided by server via handle mySession
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(mySession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		}
	mySession.Close();

	// it should still be possible to access the server via the iSession
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		}

	return TestStepResult();
	}

/**
 * Test the situation where phone entries are deleted and then a synchronisation
 * is performed.  Check cache state is valid after synchronisation.
 */
//
//    Test step6 -- NewTest#02                      //
//
CPhbkSyncOtherTest_06::CPhbkSyncOtherTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_06"));
	}

CPhbkSyncOtherTest_06::~CPhbkSyncOtherTest_06()
	{
	}

TVerdict CPhbkSyncOtherTest_06::doTestStepL( )
	{
	TContactItemId id;
	TRequestStatus status;

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(79);

	// set manual sync mode for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		}

	// perform test for each phonebook
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		// perform synchronisation
		DoSyncL(iPhoneBook[i], KErrNone);

		// delete an entry
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		iSession.DeleteContact(status, id);
		User::WaitForRequest(status);
		TESTCHECKL(status.Int(), KErrNone);

		// perform synchronisation again 
		DoSyncL(iPhoneBook[i], KErrNone);

		// check cache state is valid
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test the situation where phone entries are added and then a synchronisation
 * is performed.  Check cache state is valid after synchronisation.
 */
//
//    Test step7 -- NewTest#03                      //
//
CPhbkSyncOtherTest_07::CPhbkSyncOtherTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_07"));
	}

CPhbkSyncOtherTest_07::~CPhbkSyncOtherTest_07()
	{
	}

TVerdict CPhbkSyncOtherTest_07::doTestStepL( )
	{
	TRequestStatus status;

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(79);

	// set manual sync mode for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		}

	// create a new entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew); 
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// create an old entry with internal (short/old) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalOld = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalOld); 
	InitialiseIccEntryInternalOldL(iccEntryInternalOld);

	// perform test for each phonebook
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		// perform synchronisation
		DoSyncL(iPhoneBook[i], KErrNone);

		// add a new entry - using appropriate format
		if (iPhoneBook[i] == KUidIccGlobalSdnPhonebook ||
			iPhoneBook[i] == KUidIccGlobalLndPhonebook) 
			{
			iccEntryInternalOld->iSlotNum = KSyncIndexNotSupplied; 
			iccEntryInternalOld->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalOld, iPhoneBook[i], status);
			}
		else 
			{
			iccEntryInternalNew->iSlotNum = KSyncIndexNotSupplied; 
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			}

		// perform synchronisation again 
		DoSyncL(iPhoneBook[i], KErrNone);

		// check cache state is valid
		CheckCacheStateValidL(iPhoneBook[i]);
		}
		CleanupStack::PopAndDestroy(2); 
	return TestStepResult();
	}

/**
 * Test the situation where phone existing entry is edited and then a synchronisation
 * is performed.  Check cache state is valid after synchronisation.
 */
//
//    Test step8 -- NewTest#04                      //
//
CPhbkSyncOtherTest_08::CPhbkSyncOtherTest_08()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_08"));
	}

CPhbkSyncOtherTest_08::~CPhbkSyncOtherTest_08()
	{
	}

TVerdict CPhbkSyncOtherTest_08::doTestStepL( )
	{
	TRequestStatus status;

	// load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(79);

	// set manual sync mode for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		}

	// create a new entry with internal (long/new) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalNew = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalNew); 
	InitialiseIccEntryInternalNewL(iccEntryInternalNew);

	// create an old entry with internal (short/old) format and initialise it
	MyCSyncContactICCEntry* iccEntryInternalOld = MyCSyncContactICCEntry::NewL();
	CleanupStack::PushL(iccEntryInternalOld); 
	InitialiseIccEntryInternalOldL(iccEntryInternalOld);
	// perform test for each phonebook
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		// perform synchronisation
		DoSyncL(iPhoneBook[i], KErrNone);

		// edit entry - using appropriate format
		if (iPhoneBook[i] == KUidIccGlobalSdnPhonebook ||
			iPhoneBook[i] == KUidIccGlobalLndPhonebook ||
			iPhoneBook[i] == KUidIccGlobalFdnPhonebook) 
			{
			iccEntryInternalOld->iSlotNum = 5; 
			iccEntryInternalOld->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalOld, iPhoneBook[i], status);
			}
		else 
			{
			iccEntryInternalNew->iSlotNum = 5; 
			iccEntryInternalNew->iPhonebook = convertPhonebookUid2Enum(iPhoneBook[i]); 
			// do the real test by writing an entry to ICC
			DoWriteContactToICCL(iccEntryInternalNew, iPhoneBook[i], status);
			}

		// perform synchronisation again 
		DoSyncL(iPhoneBook[i], KErrNone);

		// check cache state is valid
		CheckCacheStateValidL(iPhoneBook[i]);
		}	
	CleanupStack::PopAndDestroy(2); 
		return TestStepResult();
	}
/**
 * Test the situation where a synchronisation is performed. 
 * Check that cache state is notification occurs after synchronisation.
 */
//
//    Test step9 --				                    //
//
CPhbkSyncOtherTest_09::CPhbkSyncOtherTest_09()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_09"));
	}

CPhbkSyncOtherTest_09::~CPhbkSyncOtherTest_09()
	{
	}

TVerdict CPhbkSyncOtherTest_09::doTestStepL( )
	{
	SetSimTsyTestNumberL(0); // see [test0] in configuration file
	TRequestStatus notifyStatus;
	iSession.NotifyPhBkCacheStateChange(notifyStatus, KUidIccGlobalAdnPhonebook);

	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);
	DoSyncAndNotificationL(KUidIccGlobalAdnPhonebook, KErrNone,
		                   RPhoneBookSession::ECacheValid, notifyStatus);
	return TestStepResult();
	}

/**
 * Test the situation where a synchronisation is performed. 
 * Check that cache state is notification occurs after 
 * synchronisation for multiple phonebooks.
 */
//
//    Test step10 --				                    //
//
CPhbkSyncOtherTest_10::CPhbkSyncOtherTest_10()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_10"));
	}

CPhbkSyncOtherTest_10::~CPhbkSyncOtherTest_10()
	{
	}

TVerdict CPhbkSyncOtherTest_10::doTestStepL( )
	{
	SetSimTsyTestNumberL(2); // see [test2] in configuration file

	//
	// Ensure that the phone books use manual sync...
	//
	TInt phoneBook(0);

	for (phoneBook = 0;  phoneBook < KTotalNumberOfPhoneBook;  phoneBook++) 
		{
		RPhoneBookSession::TPhonebookSyncMode mode;

		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[phoneBook]));
		if (mode != RPhoneBookSession::EAutoCurrentIcc) 
			{
			iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[phoneBook]);
			}
		}

	//
	// Post notifications for all phone books and then sync and wait for notifications...
	//
	TRequestStatus notifyStatusList[KTotalNumberOfPhoneBook];

	for (phoneBook = 0;  phoneBook < KTotalNumberOfPhoneBook;  phoneBook++) 
		{
		iSession.NotifyPhBkCacheStateChange(notifyStatusList[phoneBook],
											 iPhoneBook[phoneBook]);
		}

	for (phoneBook = 0;  phoneBook < KTotalNumberOfPhoneBook;  phoneBook++) 
		{ 
		DoSyncAndNotificationL(iPhoneBook[phoneBook], KErrNone,
			                   RPhoneBookSession::ECacheValid,
			                   notifyStatusList[phoneBook]);
		}

	return TestStepResult();
	}

/**
 * Test the situation where a synchronisation is performed. 
 * Check that cache state is notification cancel occuers before synchronisation.
 */
//
//    Test step11 --				                    //
//
CPhbkSyncOtherTest_11::CPhbkSyncOtherTest_11()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_11"));
	}

CPhbkSyncOtherTest_11::~CPhbkSyncOtherTest_11()
	{
	}

TVerdict CPhbkSyncOtherTest_11::doTestStepL( )
	{
	SetSimTsyTestNumberL(0); // see [test0] in configuration file
	TRequestStatus notifyStatus;
	iSession.NotifyPhBkCacheStateChange(notifyStatus, KUidIccGlobalAdnPhonebook);
	iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient);

	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);
	DoSyncAndNotificationL(KUidIccGlobalAdnPhonebook, KErrNone,
		                   RPhoneBookSession::ECacheValid, notifyStatus);
	return TestStepResult();
	}

/**
 * Test the situation where a synchronisation is performed. 
 * Check that cache state is notification occurs after synchronisation.
 * and that the status is EErrorDuringSync and the state is KErrBadHandle
 */
//
//    Test step12 --				                //
//
CPhbkSyncOtherTest_12::CPhbkSyncOtherTest_12()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_12"));
	}

CPhbkSyncOtherTest_12::~CPhbkSyncOtherTest_12()
	{
	}

TVerdict CPhbkSyncOtherTest_12::doTestStepL( )
	{
	SetSimTsyTestNumberL(22); // this config should cause a sync error whilst reading Usim PhBk
	TRequestStatus notifyStatus;
	iSession.NotifyPhBkCacheStateChange(notifyStatus, KUidUsimAppAdnPhonebook);

	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);
	DoSyncAndNotificationL(KUidUsimAppAdnPhonebook, KErrBadHandle,
		                   RPhoneBookSession::EErrorDuringSync, notifyStatus);
	return TestStepResult();
	}

/**
 * Test the situation where a synchronisation is performed. 
 * Check the last sync error is KErrNone
 */
//
//    Test step13--				                //
//
CPhbkSyncOtherTest_13::CPhbkSyncOtherTest_13()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_13"));
	}

CPhbkSyncOtherTest_13::~CPhbkSyncOtherTest_13()
	{
	}

TVerdict CPhbkSyncOtherTest_13::doTestStepL( )
	{
	SetSimTsyTestNumberL(0); // see [test0] in configuration file
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);
	TInt syncError;
	iSession.GetLastSyncError(syncError);
	TESTCHECK(syncError, KErrNone);
	return TestStepResult();
	}

/**
 * Test the situation where multiple requests to open/close sessions are made close to each other 
 * which leads to the same server side session address being used. Any notifications
 * for a dropped session should be removed.
 */
//
//    Test step14--				                    //
//
CPhbkSyncOtherTest_14::CPhbkSyncOtherTest_14()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOtherTest_14"));
	}

CPhbkSyncOtherTest_14::~CPhbkSyncOtherTest_14()
	{
	}

TVerdict CPhbkSyncOtherTest_14::doTestStepL( )
	{
	SetSimTsyTestNumberL(0);
	TRequestStatus notifyStatus;
	
	TInt ret;
	iSession.Close(); // Close original session
	ret = iSession.Connect(); // Connect to new session
	TESTCHECK(ret, KErrNone);
	iSession.NotifyPhBkCacheStateChange(notifyStatus, KUidIccGlobalAdnPhonebook); // Post notification

	iSession.Close(); // Close new session, retaining an outstanding notification
	ret = iSession.Connect(); // Connect to new session, server side using coincidently the same session address
	TESTCHECK(ret, KErrNone);
	iSession.CancelRequest(ESyncNotifyCacheStateChangeCancelClient); // Attempt to cancel notification from previous session, should have been removed when old session dropped.

	/* No WaitForRequest() here since the session has been dropped and it will never complete */

	iSession.Close();
	ret = iSession.Connect(); // this is the connect we're interested in as would never reach here if cancel paniced.

	TESTCHECK(ret, KErrNone);

	return TestStepResult();
	}
