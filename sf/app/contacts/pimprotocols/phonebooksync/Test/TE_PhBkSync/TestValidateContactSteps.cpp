// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync API ValidateContact() related Unit Test Steps
// 
//

#include "TE_PhBkSyncBase.h"
#include "TestValidateContactSteps.h"

/** 
 * Test validating whether a read operation is possible on valid
 * contacts. Repeat this test case for all supported phonebooks.
 */
//
//    Test step1 -- Test#37 in Unit Test Spec. Doc. //
//
CPhbkSyncValidateContactTest_01::CPhbkSyncValidateContactTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_01"));
	}

CPhbkSyncValidateContactTest_01::~CPhbkSyncValidateContactTest_01()
	{
	}

TVerdict CPhbkSyncValidateContactTest_01::doTestStepL( )
	{
	TContactItemId id;
	TInt result;

	// destroy the default .ini file if it exists
	DestroyDefaultIniFileL();

	SetSimTsyTestNumberL(18);

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load test and all phonebooks should be synchronised automatically 
	SetSimTsyTestNumberL(18);

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);

	// Get a valid UID for each phonebook and do ValidateContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		result = iSession.ValidateContact(MContactSynchroniser::ERead, id);
		TESTCHECKL(result, KErrNone);    
		}

	return TestStepResult();
	}

/**
 * Test validating whether a read operation is possible on valid
 * contact, where reading the ccontact is not permitted due to
 * the UICC being locked/blocked/PIN protected. Repeat this test
 * case for all supported phonebooks.
 */
//
//    Test step2 -- Test#38 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_02::CPhbkSyncValidateContactTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_02"));
	}

CPhbkSyncValidateContactTest_02::~CPhbkSyncValidateContactTest_02()
	{
	}

TVerdict CPhbkSyncValidateContactTest_02::doTestStepL( )
	{
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  

	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load all phonebooks which are not locked/blocked
	SetSimTsyTestNumberL(38); 

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// Use the valid UIDs obtained to do ValidateContact
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactIds[i]);
		TESTCHECKL(result, KErrAccessDenied);
		}

	return TestStepResult();
	}

/** 
 * Test validating whether a read operation is possible on contact,
 * where the contact specified by the contact ID is blank. 
 * Repeat this test case for all supported phonebooks.
 */
//
//    Test step3 -- Test#39 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_03::CPhbkSyncValidateContactTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_03"));
	}

CPhbkSyncValidateContactTest_03::~CPhbkSyncValidateContactTest_03()
	{
	}

TVerdict CPhbkSyncValidateContactTest_03::doTestStepL( )
	{
	SetSimTsyTestNumberL(18); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// Validate a contact with blank UID for each phonebook
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::ERead, KNullContactId);
		TESTCHECKL(result, KErrNotFound);
		}

	return TestStepResult();
	}

/**
 * Test validing whether a read operation is possible on contact,
 * where the specified contact ID is invalid.  Repeat this test 
 * case for all supported phonebooks.
 */
//
//    Test step4 -- Test#40 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_04::CPhbkSyncValidateContactTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_04"));
	}

CPhbkSyncValidateContactTest_04::~CPhbkSyncValidateContactTest_04()
	{
	}

TVerdict CPhbkSyncValidateContactTest_04::doTestStepL( )
	{
	const TInt totalNumberOfInvalidUIDs(4);
	TUid  invalidContactIds[totalNumberOfInvalidUIDs];

	invalidContactIds[0].iUid = -1;
	invalidContactIds[1].iUid = 99999;
	invalidContactIds[2].iUid = -456;
	invalidContactIds[3].iUid = 8888;

	SetSimTsyTestNumberL(18); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// Validate a contact with blank UID for each phonebook
	TInt result;
	TInt j(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		for (j=0; j<totalNumberOfInvalidUIDs; j++) 
			{
			result = iSession.ValidateContact(MContactSynchroniser::ERead, invalidContactIds[j].iUid);
			TESTCHECKL(result, KErrNotFound);
			}
		}

	return TestStepResult();
	}

/**
 * Test validing whether a search operation is possible on a valid
 * contact. Repeat tis test case for all supported phonebooks.
 */
//
//    Test step5 -- Test#41 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_05::CPhbkSyncValidateContactTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_05"));
	}

CPhbkSyncValidateContactTest_05::~CPhbkSyncValidateContactTest_05()
	{
	}

TVerdict CPhbkSyncValidateContactTest_05::doTestStepL( )
	{
	TContactItemId id;
	TInt result;

	SetSimTsyTestNumberL(18); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load test and all phonebooks should be synchronised automatically 
	SetSimTsyTestNumberL(18); // re-use test prepared for Test#37

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// Get a valid UID for each phonebook and do ValidateContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		result = iSession.ValidateContact(MContactSynchroniser::ESearch, id);
		TESTCHECKL(result, KErrNone);    
		}

	return TestStepResult();
	}

/**
 * Test validing whether a search operation is possible on a valid 
 * contact, where searching the contact is not permitted. Repeat 
 * this test case for all supported phonebooks.
 */
//
//    Test step6 -- Test#42 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_06::CPhbkSyncValidateContactTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_06"));
	}

CPhbkSyncValidateContactTest_06::~CPhbkSyncValidateContactTest_06()
	{
	}

TVerdict CPhbkSyncValidateContactTest_06::doTestStepL( )
	{
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  

	SetSimTsyTestNumberL(18); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load all phonebooks which are not locked/blocked
	SetSimTsyTestNumberL(38);

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// Get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// Use the valid UIDs obtained to do ValidateContact
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::ESearch, validContactIds[i]);
		TESTCHECKL(result, KErrAccessDenied);
		}

	// Finally set mode as EManual for all phonebooks so that it does not affect following test cases
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test validating whether a search operation is possible on contact,
 * where the contact specified by the contact ID is blank. Repeat
 * this test case for all supported phonebooks.
 */
//
//    Test step7 -- Test#43 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_07::CPhbkSyncValidateContactTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_07"));
	}

CPhbkSyncValidateContactTest_07::~CPhbkSyncValidateContactTest_07()
	{
	}

TVerdict CPhbkSyncValidateContactTest_07::doTestStepL( )
	{
	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);
	
	// Set mode as EManual for all phonebooks
	// Do initial synchronisation for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		DoSyncL(iPhoneBook[i], KErrNone);
		}
	
	// Validate a contact with blank UID for each phonebook
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::ESearch, KNullContactId);
		TESTCHECKL(result, KErrNotFound);
		}

	return TestStepResult();
	}

/**
 * Test validating whether a search operation is possible on contact, 
 * where the specified contact ID is invalid. Repeat this test
 * case for all supported phonebooks.
 */
//
//    Test step8 -- Test#44 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_08::CPhbkSyncValidateContactTest_08()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_08"));
	}

CPhbkSyncValidateContactTest_08::~CPhbkSyncValidateContactTest_08()
	{
	}

TVerdict CPhbkSyncValidateContactTest_08::doTestStepL( )
	{
	TInt totalNumberOfInvalidUIDs = 4;
	TUid  invalidContactIds[] = 
		{
		{-1},
		{99999},
		{-456},
		{8888}
		};

	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// Set mode as EManual for all phonebooks
	// Do initial synchronisation for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		DoSyncL(iPhoneBook[i], KErrNone);
		}

	// Validate a contact with blank UID for each phonebook
	TInt result;
	TInt j(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		for (j=0; j<totalNumberOfInvalidUIDs; j++) 
			{
			result = iSession.ValidateContact(MContactSynchroniser::ESearch, invalidContactIds[j].iUid);
			TESTCHECKL(result, KErrNotFound);
			}
		}

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possible on valid
 * contact. Repeat this test case for all supported phonebooks.
 */
//
//    Test step9 -- Test#45 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_09::CPhbkSyncValidateContactTest_09()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_09"));
	}

CPhbkSyncValidateContactTest_09::~CPhbkSyncValidateContactTest_09()
	{
	}

TVerdict CPhbkSyncValidateContactTest_09::doTestStepL( )
	{
	TContactItemId id;
	TInt result;

	// Load test and all phonebooks should be synchronised automatically 
	SetSimTsyTestNumberL(37); // re-use test prepared for Test#37

	// Set mode as EManual for all phonebooks
	// Do initial synchronisation for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		DoSyncL(iPhoneBook[i], KErrNone);
		}

	// Get a valid UID for each phonebook and do ValidateContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		id = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, id);
		TESTCHECKL(result, KErrNone);    
		}

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possile on valid
 * contact, where editing the contact is not permitted. Repeat
 * this test case for all supported phonebooks.
 */
//
//    Test step10 -- Test#46 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_10::CPhbkSyncValidateContactTest_10()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_10"));
	}

CPhbkSyncValidateContactTest_10::~CPhbkSyncValidateContactTest_10()
	{
	}

TVerdict CPhbkSyncValidateContactTest_10::doTestStepL( )
	{
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  

	// Load all phonebooks which are not locked/blocked
	SetSimTsyTestNumberL(38); 

	// Set mode as EManual for all phonebooks
	// Do initial synchronisation for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		DoSyncL(iPhoneBook[i], KErrNone);
		}

	// Get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// Use the valid UIDs obtained to do ValidateContact
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactIds[i]);
		TESTCHECKL(result, KErrAccessDenied);
		}

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possible on
 * contact, where the contact specified by the contact ID is
 * blank. Repeat this test case for all supported phonebooks.
 */
//
//    Test step11 -- Test#47 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_11::CPhbkSyncValidateContactTest_11()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_11"));
	}

CPhbkSyncValidateContactTest_11::~CPhbkSyncValidateContactTest_11()
	{
	}

TVerdict CPhbkSyncValidateContactTest_11::doTestStepL( )
	{
	SetSimTsyTestNumberL(18); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// Validate a contact with blank UID for each phonebook
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, KNullContactId);
		TESTCHECKCONDITIONL(result == KErrNotFound  ||  result == KErrNotReady);

		if(result == KErrNotReady)
			{
			TInt jj = 0;
			while(jj < 5)
				{
				User::After(1000000);
				result = iSession.ValidateContact(MContactSynchroniser::EEdit, KNullContactId);
				if(result != KErrNotReady)
					{
					break;
					}
				jj++;
				}
			
			TESTCHECKL(result, KErrNotFound);
			}

		}

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possible on
 * contact, where the specified contact ID is invalid. Repeat
 * this test case for all supported phonebooks.
 */
//
//    Test step12 -- Test#48 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_12::CPhbkSyncValidateContactTest_12()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_12"));
	}

CPhbkSyncValidateContactTest_12::~CPhbkSyncValidateContactTest_12()
	{
	}

TVerdict CPhbkSyncValidateContactTest_12::doTestStepL( )
	{
	TInt totalNumberOfInvalidUIDs = 4;
	TUid  invalidContactIds[] = 
		{
		{-1},
		{99999},
		{-456},
		{8888}
		};

	SetSimTsyTestNumberL(0); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0);

	// Validate a contact with blank UID for each phonebook
	TInt result;
	TInt j(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		for (j=0; j<totalNumberOfInvalidUIDs; j++) 
			{
			result = iSession.ValidateContact(MContactSynchroniser::EEdit, invalidContactIds[j].iUid);
			TESTCHECKCONDITIONL(result == KErrNotFound  ||  result == KErrNotReady);

			if(result == KErrNotReady)
				{
				TInt kk=0;
				while(kk < 5)
					{
					User::After(1000000);
					result = iSession.ValidateContact(MContactSynchroniser::EEdit, invalidContactIds[j].iUid);
				
					if(result != KErrNotReady)
						{
						break;
						}
					kk++;
					}
			
				TESTCHECKL(result, KErrNotFound);
				}

			}
		}

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possible on contact,
 * where the specified contact is hidden.
 */
//
//    Test step13 -- Test#49 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_13::CPhbkSyncValidateContactTest_13()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_13"));
	}

CPhbkSyncValidateContactTest_13::~CPhbkSyncValidateContactTest_13()
	{
	}

TVerdict CPhbkSyncValidateContactTest_13::doTestStepL( )
	{
	TInt result;

	SetSimTsyTestNumberL(54); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load Global/GSM ADN and USim App ADN phonebooks, where
	// both phonebooks contain hidden entries.
	SetSimTsyTestNumberL(38); // re-use test prepared for Test#53-1

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];

	// get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// Use the valid UIDs obtained to do ValidateContact
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactIds[i]);
		TESTCHECKL(result, KErrAccessDenied);
		}

	return TestStepResult();
	}


//
//    Test step14 -- Test#50 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_14::CPhbkSyncValidateContactTest_14()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_14"));
	}

CPhbkSyncValidateContactTest_14::~CPhbkSyncValidateContactTest_14()
	{
	}

TVerdict CPhbkSyncValidateContactTest_14::doTestStepL( )
	{
	TInt result;
	TUid  validGlobalADNUid, validUsimAppADNUid;

	SetSimTsyTestNumberL(0); 

	// set mode as EAutoCurrentIcc for Global/GSM ADN and USim App ADN
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidUsimAppAdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook));

	// load Global/GSM ADN and USim App ADN phonebooks
	SetSimTsyTestNumberL(50);	// both phonebooks should be synchronised after this

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateInvalidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateInvalidL(KUidIccGlobalLndPhonebook);
	CheckCacheStateInvalidL(KUidIccGlobalFdnPhonebook);

	// at this stage, it should be possible to access "App1" phonebooks
	validGlobalADNUid.iUid = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	result = iSession.ValidateContact(MContactSynchroniser::EEdit, validGlobalADNUid.iUid);
	TESTCHECKL(result, KErrNone);    

	validUsimAppADNUid.iUid = GetValidUIDFromContactsDbL(KUidUsimAppAdnPhonebook);
	result = iSession.ValidateContact(MContactSynchroniser::EEdit, validUsimAppADNUid.iUid);
	TESTCHECKL(result, KErrNone);

	SignalSimTsyToReduceTimersL();

	// Wait for USIM App switching to occur - App1 is closed and App2 becomes active 
	// by using the valid UIDs obtained to do ValidateContact
	while (result == KErrNotReady  ||  result == KErrNone) 
		{
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, validUsimAppADNUid.iUid);
		User::After(1000000);
		}
	TESTCHECK(result, KErrNotFound);

	// Even though USIM App1 is no longer active, access to Global/GSM ADN phonebook 
	// should NOT be affected 
	result = iSession.ValidateContact(MContactSynchroniser::EEdit, validGlobalADNUid.iUid);
	TESTCHECK(result, KErrNone);    

	return TestStepResult();
	}

/**
 * Test validating whether an edit operation is possible on USIM
 * Application phonebook contact, when the USIM Application becomes
 * PIN protected.
 */
//
//    Test step15 -- Test#51 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_15::CPhbkSyncValidateContactTest_15()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_15"));
	}

CPhbkSyncValidateContactTest_15::~CPhbkSyncValidateContactTest_15()
	{
	}

TVerdict CPhbkSyncValidateContactTest_15::doTestStepL( )
	{
	// arry used to keep a valid contactId for each phonebook
	TContactItemId validContactIds[5];  

	SetSimTsyTestNumberL(2); 

	// Set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// Load all phonebooks which are not locked/blocked
	SetSimTsyTestNumberL(38);  // re-use phonebooks prepared for Test#37

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	// Get a valid UID for each phonebook and keep them somewhere
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		validContactIds[i] = GetValidUIDFromContactsDbL(iPhoneBook[i]);
		}

	// Wait for ICC to become locked 
	WaitForIccToBecomeLockedL();

	// Use the valid UIDs obtained to do ValidateContact
	TInt result;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		if (iPhoneBook[i] == KUidIccGlobalAdnPhonebook ||	iPhoneBook[i] == KUidUsimAppAdnPhonebook) 
			{
			result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactIds[i]);
			TESTCHECKL(result, KErrAccessDenied);
			}
		}

	return TestStepResult();
	}


//
//    Test step16 -- Test#52 in Unit Test Spec. Doc.  //
//
CPhbkSyncValidateContactTest_16::CPhbkSyncValidateContactTest_16()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_16"));
	}

CPhbkSyncValidateContactTest_16::~CPhbkSyncValidateContactTest_16()
	{
	}

TVerdict CPhbkSyncValidateContactTest_16::doTestStepL( )
	{
	// =============================================================
	// *** This test is covered by Test#51 since the phonebooks ***
	// *** in [test38] are both USIM Application PIN protected  ***
	// *** and ICC locked/blocked/PIN protected.
	// =============================================================

	return TestStepResult();
	}

/**
 * Test validating whether a read operation is possible on valid
 * contact, where reading the ccontact is not permitted due to
 * the FDN entry being PIN2 protected. Check that other phonebook
 * can still be accessed for reading. Once PIN2 is verified, editing
 * FDN contact should be permitted. Again check reading from other
 * phonebook.
 */
//
//    Test step17 --								//
//
CPhbkSyncValidateContactTest_17::CPhbkSyncValidateContactTest_17()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncValidateContactTest_17"));
	}

CPhbkSyncValidateContactTest_17::~CPhbkSyncValidateContactTest_17()
	{
	}

TVerdict CPhbkSyncValidateContactTest_17::doTestStepL( )
	{
	// set mode as EAutoCurrentIcc for all phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load all phonebooks which are not locked/blocked
	SetSimTsyTestNumberL(41); 

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);

	TContactItemId validContactId = GetValidUIDFromContactsDbL(KUidIccGlobalFdnPhonebook);
	TESTCHECKCONDITIONL(validContactId != KNullContactId);
	
	// Wait for PIN2 to become protected for editing FDN phonebook 
	TInt result = KErrNone;
	result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactId);
	while (result != KErrAccessDenied) 
		{
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactId);
		User::After(1000000);
		}
	TESTCHECKL(result, KErrAccessDenied);

	// check that other phonebooks can still be read
	TContactItemId validContactId2 = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId2 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId2);
	TESTCHECKL(result, KErrNone);

	TContactItemId validContactId3 = GetValidUIDFromContactsDbL(KUidIccGlobalSdnPhonebook);
	TESTCHECKCONDITIONL(validContactId3 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId3);
	TESTCHECKL(result, KErrNone);

	TContactItemId validContactId4 = GetValidUIDFromContactsDbL(KUidIccGlobalLndPhonebook);
	TESTCHECKCONDITIONL(validContactId4 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId4);
	TESTCHECKL(result, KErrNone);

	TContactItemId validContactId5 = GetValidUIDFromContactsDbL(KUidUsimAppAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId5 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId5);
	TESTCHECKL(result, KErrNone);

	// Wait for PIN2 to become validated for editing FDN phonebook 
	result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactId);
	while (result != KErrNone) 
		{
		result = iSession.ValidateContact(MContactSynchroniser::EEdit, validContactId);
		User::After(1000000);
		}
	TESTCHECKL(result, KErrNone);

	// check that other phonebooks can still be read
	validContactId2 = GetValidUIDFromContactsDbL(KUidIccGlobalAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId2 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId2);
	TESTCHECKL(result, KErrNone);

	validContactId3 = GetValidUIDFromContactsDbL(KUidIccGlobalSdnPhonebook);
	TESTCHECKCONDITIONL(validContactId3 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId3);
	TESTCHECKL(result, KErrNone);

	validContactId4 = GetValidUIDFromContactsDbL(KUidIccGlobalLndPhonebook);
	TESTCHECKCONDITIONL(validContactId4 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId4);
	TESTCHECKL(result, KErrNone);

	validContactId5 = GetValidUIDFromContactsDbL(KUidUsimAppAdnPhonebook);
	TESTCHECKCONDITIONL(validContactId5 != KNullContactId);
	result = iSession.ValidateContact(MContactSynchroniser::ERead, validContactId5);
	TESTCHECKL(result, KErrNone);
	
	return TestStepResult();
	}

