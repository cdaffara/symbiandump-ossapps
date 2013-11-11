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
// This contains PhbkSync API DoSynchronisation() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestDoSynchronisationSteps.h"


/** 
 * Test synchronisation with a partially full Global/GSM ADN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step1 -- Test#10 in Unit Test Spec. Doc. //
//
CPhbkSyncDoSynchronisationTest_01::CPhbkSyncDoSynchronisationTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_01"));
	}

CPhbkSyncDoSynchronisationTest_01::~CPhbkSyncDoSynchronisationTest_01()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_01::doTestStepL( )
	{
	// 1. destroy the default .ini file if it exists
	DestroyDefaultIniFileL();

	// 2. load the partially full Global/GSM ADN phonebook from cfg file
	SetSimTsyTestNumberL(10); // see [test10] in configuration file

	// 3. set mode as EManual for the tested phonebook only.
	//    If the current is EManual, then override it anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);

	CheckCacheStateInvalidL(KUidIccGlobalAdnPhonebook); 

	// 4. do synchronisation
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);

	// 5. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a partially full USIM ADN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step2 -- Test#11 in Unit Test Spec. Doc. //
//
CPhbkSyncDoSynchronisationTest_02::CPhbkSyncDoSynchronisationTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_02"));
	}

CPhbkSyncDoSynchronisationTest_02::~CPhbkSyncDoSynchronisationTest_02()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_02::doTestStepL( )
	{
	// 1. load the partially full USIM ADN phonebook from cfg file
	SetSimTsyTestNumberL(11); // see [test11] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidUsimAppAdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidUsimAppAdnPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a partially full Global/GSM SDN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step3 -- Test#12 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_03::CPhbkSyncDoSynchronisationTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_03"));
	}

CPhbkSyncDoSynchronisationTest_03::~CPhbkSyncDoSynchronisationTest_03()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_03::doTestStepL( )
	{
	// 1. load the partially full Global/GSM SDN phonebook from cfg file
	SetSimTsyTestNumberL(12); // see [test12] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour

	return TestStepResult();
	}

/** 
 * Test synchronisation with a partially full Global/GSM LND phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step4 -- Test#13 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_04::CPhbkSyncDoSynchronisationTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_04"));
	}

CPhbkSyncDoSynchronisationTest_04::~CPhbkSyncDoSynchronisationTest_04()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_04::doTestStepL( )
	{
	// 1. load the partially full Global/GSM LND phonebook from cfg file
	SetSimTsyTestNumberL(13); // see [test13] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalLndPhonebook, KErrNone); 

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a full Global/GSM ADN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step5 -- Test#14 in Unit Test Spec. Doc. //
//
CPhbkSyncDoSynchronisationTest_05::CPhbkSyncDoSynchronisationTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_05"));
	}

CPhbkSyncDoSynchronisationTest_05::~CPhbkSyncDoSynchronisationTest_05()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_05::doTestStepL( )
	{
	// 1. load the full Global/GSM ADN phonebook from cfg file
	SetSimTsyTestNumberL(14); // see [test14] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a full USIM ADN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step6 -- Test#15 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_06::CPhbkSyncDoSynchronisationTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_06"));
	}

CPhbkSyncDoSynchronisationTest_06::~CPhbkSyncDoSynchronisationTest_06()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_06::doTestStepL( )
	{
	// 1. load the full USIM ADN phonebook from cfg file
	SetSimTsyTestNumberL(15); // see [test15] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidUsimAppAdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidUsimAppAdnPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a full SDN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step7 -- Test#16 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_07::CPhbkSyncDoSynchronisationTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_07"));
	}

CPhbkSyncDoSynchronisationTest_07::~CPhbkSyncDoSynchronisationTest_07()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_07::doTestStepL( )
	{
	// 1. load the full SDN phonebook from cfg file
	SetSimTsyTestNumberL(16); // see [test16] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with a full LND phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step8 -- Test#17 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_08::CPhbkSyncDoSynchronisationTest_08()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_08"));
	}

CPhbkSyncDoSynchronisationTest_08::~CPhbkSyncDoSynchronisationTest_08()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_08::doTestStepL( )
	{
	// 1. load the full LND phonebook from cfg file
	SetSimTsyTestNumberL(17); // see [test17] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalLndPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook); 

	return TestStepResult();
	}

/** 
 * Test synchronisation with all full supported phonebooks.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step9 -- Test#18 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_09::CPhbkSyncDoSynchronisationTest_09()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_09"));
	}

CPhbkSyncDoSynchronisationTest_09::~CPhbkSyncDoSynchronisationTest_09()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_09::doTestStepL( )
	{
	// 1. load the full LND phonebook from cfg file
	SetSimTsyTestNumberL(18); // see [test18] in configuration file

	// 2. set mode as EManual for all supported phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	// 3. do synchronisation
	for (i=0; i<KTotalNumberOfPhoneBook; i++) { 
	DoSyncL(iPhoneBook[i], KErrNone);
	}

	// 4. check the expected behaviour
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	return TestStepResult();
	}

/** 
 * Test synchronisation of all supported phonebooks, when
 * synchronisation of one of them fails.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step10 -- Test#19 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_10::CPhbkSyncDoSynchronisationTest_10()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_10"));
	}

CPhbkSyncDoSynchronisationTest_10::~CPhbkSyncDoSynchronisationTest_10()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_10::doTestStepL( )
	{
	// 1. load all (partially full) phonebooks from cfg file
	SetSimTsyTestNumberL(19); // see [test19] in configuration file

	// 2. set mode as EManual for all supported phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	// 3. do synchronisation and LND will fail (see cfg. file)
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		if (iPhoneBook[i] != KUidIccGlobalLndPhonebook) 
			{
			DoSyncL(iPhoneBook[i], KErrNone);
			}
		}

	// To simply ignore the error generated by DoSynchronisation 
	// since we cannot guarantee the phonebook synchroniser server 
	// will give us the error KErrBadHandle defined in the cfg. file.
	// However, we do know the error code != KErrNone
	DoSyncL(KUidIccGlobalLndPhonebook, KErrBadHandle, ETrue); // ignore LND fail

	// 4. check the expected behaviour
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		if (iPhoneBook[i] == KUidIccGlobalLndPhonebook) 
			{
			RPhoneBookSession::TSyncState state;
			iSession.GetPhoneBookCacheState(state, iPhoneBook[i]);
			TESTCHECK(state, RPhoneBookSession::EErrorDuringSync);
			}
		else 
			{ 
			CheckCacheStateValidL(iPhoneBook[i]);
			}
		}

	return TestStepResult();
	}

/** 
 * Test synchronisation of a Global/GSM ADN phonebook that
 * contains an entry whose size >512 characters which will
 * lead to overflow when phbksync is doing the batch reading.
 *
 * Following are the limits imposed by TSY:
 * 1.   100 -- max. number of digits/characters allowed for phonenumber
 * 2.   241 -- max. number of characters/digits allowed for name
 *
 * Following is the limit imposed by PhBkSync:
 * 1.   512 -- the max. size of the buffer used when doing batch read
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step11 -- Test#20 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_11::CPhbkSyncDoSynchronisationTest_11()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_11"));
	}

CPhbkSyncDoSynchronisationTest_11::~CPhbkSyncDoSynchronisationTest_11()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_11::doTestStepL( )
	{
	// 1. load Global/GSM ADN phonebook which has one entry > 512
	SetSimTsyTestNumberL(20); // see [test20] in configuration file

	// 2. set mode as EManual for all supported phonebooks
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);

	// 3. do synchronisation
//	DoSyncL(KUidIccGlobalAdnPhonebook, KErrOverflow); // synchronisation fails

	// there is a bug in TSY which returns KErrArgumnet rather than KErrOverflow
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrArgument); // synchronisation fails

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidIccGlobalAdnPhonebook);

	return TestStepResult();
	}

/** 
 * This test is similar to Test#20.
 * The difference is that the overflow is caused by large phone number 
 * and additional numbers rather than by names in Test#20.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step12 -- Test#21 in Unit Test Spec. Doc.//
//
CPhbkSyncDoSynchronisationTest_12::CPhbkSyncDoSynchronisationTest_12()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_12"));
	}

CPhbkSyncDoSynchronisationTest_12::~CPhbkSyncDoSynchronisationTest_12()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_12::doTestStepL( )
	{
	// 1. load USIM App ADN phonebook which has ane entry whose tel number field > 255
	SetSimTsyTestNumberL(21); // see [test21] in configuration file

	// 2. set mode as EManual for all supported phonebooks
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// 3. do synchronisation
//	DoSyncL(KUidUsimAppAdnPhonebook, KErrOverflow); // synchronisation fails

	// there is a bug in TSY which returns KErrArgumnet rather than KErrOverflow
	DoSyncL(KUidUsimAppAdnPhonebook, KErrArgument); // synchronisation fails

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidUsimAppAdnPhonebook);

	return TestStepResult();
	}

/** 
 * Test synchronisation with a corrupted USIM.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step13 -- Test#22 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_13::CPhbkSyncDoSynchronisationTest_13()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_13"));
	}

CPhbkSyncDoSynchronisationTest_13::~CPhbkSyncDoSynchronisationTest_13()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_13::doTestStepL( )
	{
	// 1. load the currupted USIM App ADN phonebook
	SetSimTsyTestNumberL(22); // see [test22] in configuration file

	// 2. set mode as EManual for all supported phonebooks
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// 3. do synchronisation
	// To simply ignore the error generated by DoSynchronisation since
	// we cannot guarantee the phonebook synchroniser server will give
	// us the error KErrBadHandle defined in the cfg. file.
	// However, we do know the error code != KErrNone
	DoSyncL(KUidUsimAppAdnPhonebook, KErrBadHandle, ETrue); // ignore the result

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidUsimAppAdnPhonebook);

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook containing entries
 * that are "read-only".
 * Note that SDN and LND phonebooks are read-only by default.
 */
//
//    Test step14 -- Test#23 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_14::CPhbkSyncDoSynchronisationTest_14()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_14"));
	}

CPhbkSyncDoSynchronisationTest_14::~CPhbkSyncDoSynchronisationTest_14()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_14::doTestStepL( )
	{
	// 1. change the mode as EManual for all phonebooks anc check it.
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);    
		}

	// 2. load the dummy ICC which contains all phonebooks
	SetSimTsyTestNumberL(0); // no synchronisation should happen

	// 3. do synchronisation for read-only phonebooks (SDN and LND)
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);
	DoSyncL(KUidIccGlobalLndPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook);

	return TestStepResult();
	}

/**
 * Test synchronisation when a UICC is not present
 */
//
//    Test step15 -- Test#24 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_15::CPhbkSyncDoSynchronisationTest_15()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_15"));
	}

CPhbkSyncDoSynchronisationTest_15::~CPhbkSyncDoSynchronisationTest_15()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_15::doTestStepL( )
	{
	// 1. change the mode as EManual for all phonebooks anc check it.
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);    
		}

	// 2. load the phonebooks prepared
	SetSimTsyTestNumberL(24);

	// 3. do synchronisation and failed
	// To simply ignore the error generated by DoSynchronisation 
	// since we cannot guarantee the phonebook synchroniser server 
	// will give us the error KErrNotReady defined in the cfg. file.
	// However, we do know the error code != KErrNone
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNotReady); // ignore sync failure 
	DoSyncL(KUidUsimAppAdnPhonebook, KErrNotReady);   // ignore sync failure
	DoSyncL(KUidIccGlobalFdnPhonebook, KErrNotReady); // ignore sync failure 

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateErrorL(KUidUsimAppAdnPhonebook);
	CheckCacheStateErrorL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}

/**
 * Test synchrnisation when UICC is "not ready"
 */
//
//    Test step16 -- Test#25 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_16::CPhbkSyncDoSynchronisationTest_16()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_16"));
	}

CPhbkSyncDoSynchronisationTest_16::~CPhbkSyncDoSynchronisationTest_16()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_16::doTestStepL( )
	{
	// It is not clear how to simulate that UICC is "not ready" ???

	return TestStepResult();
	}


/**
 * Test synchronisation when the UICC is "blocked"
 */
//
//    Test step17 -- Test#26 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_17::CPhbkSyncDoSynchronisationTest_17()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_17"));
	}

CPhbkSyncDoSynchronisationTest_17::~CPhbkSyncDoSynchronisationTest_17()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_17::doTestStepL( )
	{
	// 1. change the mode as EManual for all phonebooks anc check it.
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);    
		}

	// 2. load the test which contains all phonebooks, but they are locked
	SetSimTsyTestNumberL(27);

	// 3. do synchronisation and failed
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrAccessDenied);
	DoSyncL(KUidUsimAppAdnPhonebook, KErrAccessDenied);
	DoSyncL(KUidIccGlobalFdnPhonebook, KErrAccessDenied);

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateErrorL(KUidUsimAppAdnPhonebook);
	CheckCacheStateErrorL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}

/**
 * Test synchronisation when the UICC is "PIN protected"
 */
//
//    Test step18 -- Test#27 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_18::CPhbkSyncDoSynchronisationTest_18()
{
  // store the name of this test step
  SetTestStepName(_L("PhbkSyncDoSynchronisationTest_18"));
}

CPhbkSyncDoSynchronisationTest_18::~CPhbkSyncDoSynchronisationTest_18()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_18::doTestStepL( )
	{
	// 1. change the mode as EManual for all phonebooks anc check it.
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);    
		}

	// 2. load the Global ADN and Usim ADN phonebooks prepared
	SetSimTsyTestNumberL(27); // no synchronisation should happen

	// 3. do synchronisation for the PIN protected phonebooks
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrAccessDenied); // should fail
	DoSyncL(KUidUsimAppAdnPhonebook, KErrAccessDenied);   // should fail
	DoSyncL(KUidIccGlobalFdnPhonebook, KErrAccessDenied); // should fail

	// 4. check the expected behaviour
	CheckCacheStateErrorL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateErrorL(KUidUsimAppAdnPhonebook);
	CheckCacheStateErrorL(KUidIccGlobalFdnPhonebook);

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook that contains entries with
 * Name field set and Number fiedl blank
 */
//
//    Test step19 -- Test#28 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_19::CPhbkSyncDoSynchronisationTest_19()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_19"));
	}

CPhbkSyncDoSynchronisationTest_19::~CPhbkSyncDoSynchronisationTest_19()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_19::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(28); // see [test28] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisatin of a phonebook that contains entries
 * with Number field set and Name field blank.
 */
//
//    Test step20 -- Test#29 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_20::CPhbkSyncDoSynchronisationTest_20()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_20"));
	}

CPhbkSyncDoSynchronisationTest_20::~CPhbkSyncDoSynchronisationTest_20()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_20::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(29); // see [test29] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook that contains entries 
 * that have letters in the phone number field.
 */ 
//
//    Test step21 -- Test#30 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_21::CPhbkSyncDoSynchronisationTest_21()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_21"));
	}

CPhbkSyncDoSynchronisationTest_21::~CPhbkSyncDoSynchronisationTest_21()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_21::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(30); // see [test30] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook that contains entries
 * with "+", "p", and "w" in th phone number field.
 */
//
//    Test step22 -- Test#31 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_22::CPhbkSyncDoSynchronisationTest_22()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_22"));
	}

CPhbkSyncDoSynchronisationTest_22::~CPhbkSyncDoSynchronisationTest_22()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_22::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(31); // see [test31] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook that has an entry
 * with the location number "0".
 */
//
//    Test step23 -- Test#32 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_23::CPhbkSyncDoSynchronisationTest_23()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_23"));
	}

CPhbkSyncDoSynchronisationTest_23::~CPhbkSyncDoSynchronisationTest_23()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_23::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(32); // see [test32] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrArgument);

	// 4. check the expected behaviour
	CheckCacheStateInvalidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisation of a phonebook that has entries 
 * with negative location numbers
 */
//
//    Test step24 -- Test#33 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_24::CPhbkSyncDoSynchronisationTest_24()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_24"));
	}

CPhbkSyncDoSynchronisationTest_24::~CPhbkSyncDoSynchronisationTest_24()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_24::doTestStepL( )
	{
	// 1. load the SDN phonebook prepared for this test
	SetSimTsyTestNumberL(33); // see [test33] in configuration file

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalSdnPhonebook, KErrArgument);

	// 4. check the expected behaviour
	CheckCacheStateInvalidL(KUidIccGlobalSdnPhonebook); 

	return TestStepResult();
	}

/**
 * Test synchronisation of multiple phonebooks by starting them 
 * sconsecutively.
 */
//
//    Test step25 -- Test#34 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_25::CPhbkSyncDoSynchronisationTest_25()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_25"));
	}

CPhbkSyncDoSynchronisationTest_25::~CPhbkSyncDoSynchronisationTest_25()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_25::doTestStepL( )
	{
	// 1. load the dummy ICC which contains all phonebooks
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	SetSimTsyTestNumberL(0); // see [test0] in configuration file

	// 2. set mode as EManual and do initial synchronisation
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		DoSyncL(iPhoneBook[i], KErrNone);
		}

	// 3. consecutively do sync according to the test purpose
	RPhoneBookSession session;  // a NEW session handle
	User::LeaveIfError(session.Connect()); // Second connection to the Server
	TRequestStatus status1, status2, status3, status4;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.DoSynchronisation(status1, iPhoneBook[i]); // Start first sync
		iSession.DoSynchronisation(status2, iPhoneBook[i]); // Start second sync
		iSession.DoSynchronisation(status3, iPhoneBook[i]); // Start third sync
		iSession.DoSynchronisation(status4, iPhoneBook[i]); // Start fourth sync
		User::WaitForRequest(status1);
		User::WaitForRequest(status2);
		User::WaitForRequest(status3);
		User::WaitForRequest(status4);
		TESTCHECK(status1.Int(), KErrNone);
		TESTCHECK(status2.Int(), KErrInUse);
		TESTCHECK(status3.Int(), KErrInUse);
		TESTCHECK(status4.Int(), KErrInUse);
		}

	// 4. do another sync and it should be successful this time
	TRequestStatus status5;
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		session.DoSynchronisation(status5, iPhoneBook[i]);
		User::WaitForRequest(status5);
		TESTCHECK(status5.Int(), KErrNone);
		}

	// 5. Close second connection to the server
	session.Close(); // Not the iSession used by every test step

	return TestStepResult();
	}

/**
 * Test synchronisation of multiple phonebooks with hidden entries.
 * 
 * === Only applies to Global/GSM ADN and USIM App ADN phonebooks ===
 */
//
//    Test step26 -- Test#35 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_26::CPhbkSyncDoSynchronisationTest_26()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_26"));
	}

CPhbkSyncDoSynchronisationTest_26::~CPhbkSyncDoSynchronisationTest_26()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_26::doTestStepL( )
	{
	// 1. load all phonebooks, where Global/GSM ADN and USim App ADN
	// phonebooks contain hidden entries. The Global/GSM ADN and USim
	// App ADN are the phonebooks which are relevant to this test.
	SetSimTsyTestNumberL(37); // re-use phonebooks prepared for test37# 

	// 2. set mode as EManual, anyway.
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);

	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);
	DoSyncL(KUidUsimAppAdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	// 4.1 Synchroniser synchronises all entries without any errors. 
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook); 
	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);

	// 4.2 Hidden entries are marked as such in the look-up table
	//     and not visible in the contacts database

	// since there is no such an API from phbksync to get a valid
	// contact UID for the hidden entry, and also the hidden is not
	// put into the contact database, it is impossible to verify 
	// the expected behaviour.

	return TestStepResult();
	}

/**
 * Test initial synchronisation is successful. Then test that a
 * subsequent "SIM tookit refresh" notification leads to a second 
 * synchronisation.
 */


/** 
 * Re-Synchronisation (SIM Toolkit initiated).
 * SIM is OK. Synchronisation mode is "Auto Sync, Current ICC".
 * Test initial synchronisation is successful. Then test that
 * a subsequent "SIM toolkit refresh" notification leads to a 
 * second synchronisation.
 */
//
//    Test step27 -- Test#36 in Unit Test Spec. Doc.  //
//
CPhbkSyncDoSynchronisationTest_27::CPhbkSyncDoSynchronisationTest_27()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_27"));
	}

CPhbkSyncDoSynchronisationTest_27::~CPhbkSyncDoSynchronisationTest_27()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_27::doTestStepL( )
	{
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// load the phonebook prepared for this particular test
	SetSimTsyTestNumberL(36);	// Switch to a SIM TSY configuration which sends a 
								// "refresh" notification at some point in the future

	User::After(10000000); // Wait for refresh notification to trigger
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

	return TestStepResult();
	}

/** 
 * Test synchronisation with a partially full Global/GSM FDN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step28 --								//
//
CPhbkSyncDoSynchronisationTest_28::CPhbkSyncDoSynchronisationTest_28()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_28"));
	}

CPhbkSyncDoSynchronisationTest_28::~CPhbkSyncDoSynchronisationTest_28()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_28::doTestStepL( )
	{
	// 1. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook);

	// 2. load the partially full Global/GSM FDN phonebook from cfg file
	SetSimTsyTestNumberL(39); // see [test39] in configuration file

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalFdnPhonebook, KErrNone);

	// 4. check the expected behaviour

	return TestStepResult();
	}

/** 
 * Test synchronisation with a full FDN phonebook.
 *
 * It is assumed that server been connected/started by Open step.
 */
//
//    Test step29 --								//
//
CPhbkSyncDoSynchronisationTest_29::CPhbkSyncDoSynchronisationTest_29()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncDoSynchronisationTest_29"));
	}

CPhbkSyncDoSynchronisationTest_29::~CPhbkSyncDoSynchronisationTest_29()
	{
	}

TVerdict CPhbkSyncDoSynchronisationTest_29::doTestStepL( )
	{
	// 1. load the full FDN phonebook from cfg file
	SetSimTsyTestNumberL(40); // see [test40] in configuration file

	// 2. set mode as EManual for the tested phonebook only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook)); 
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook);

	// 3. do synchronisation
	DoSyncL(KUidIccGlobalFdnPhonebook, KErrNone);

	// 4. check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook); 

	return TestStepResult();
	}


/** 
 *  Test booting the phone without PIN1 entered and then ensure that when the PIN
 *  is entered that the phonebooks are synchronised.
 */
TVerdict CPhbkSyncDoSynchronisationTest_30::doTestStepL()
	{
	//
	// Set half of the phonebooks to auto-sync and half to manual...
	//
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook)); 
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalSdnPhonebook)); 
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalLndPhonebook)); 
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook)); 
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook)); 

	//
	// Load a config were the ICC is PIN1 locked at the start...
	//
	SetSimTsyTestNumberL(101);

	//
	// Check that the auto-sync phonebooks have failed and the others are unsynchronised.
	//
	CheckCacheStateErrorL(KUidIccGlobalAdnPhonebook); 
	CheckCacheStateErrorL(KUidIccGlobalSdnPhonebook); 
	CheckCacheStateErrorL(KUidIccGlobalLndPhonebook); 
	CheckCacheStateInvalidL(KUidUsimAppAdnPhonebook); 
	CheckCacheStateInvalidL(KUidIccGlobalFdnPhonebook); 

	//
	// Move time forward and allow the PIN1 timer to trigger after being advanced,
	// so that the SIMTSY unlocks PIN1.
	//
	SignalSimTsyToReduceTimersL();
	User::After(5*1000000);
	
	//
	// Check that the auto-sync phonebooks have now sync'd and the others are unsynchronised.
	//
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook); 
	CheckCacheStateValidL(KUidIccGlobalSdnPhonebook); 
	CheckCacheStateValidL(KUidIccGlobalLndPhonebook); 
	CheckCacheStateInvalidL(KUidUsimAppAdnPhonebook); 
	CheckCacheStateInvalidL(KUidIccGlobalFdnPhonebook); 

	return TestStepResult();
	} // CPhbkSyncDoSynchronisationTest_30::doTestStepL


//
//    OOM DoSynchronisation step1
//
CPhbkSyncOOMDoSynchronisationTest_01::CPhbkSyncOOMDoSynchronisationTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOOMDoSynchronisationTest_01"));
	}

CPhbkSyncOOMDoSynchronisationTest_01::~CPhbkSyncOOMDoSynchronisationTest_01()
	{
	}

TVerdict CPhbkSyncOOMDoSynchronisationTest_01::doTestStepL( )
	{
	// Note that only after session is connected, can Heap be marked
	DestroyDefaultIniFileL();

	// load all phonebooks
	SetSimTsyTestNumberL(80);

	// set mode as EManual for all supported phonebooks
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]));
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);

	// do OOM test against synchronisation API
	RPhoneBookSession newSession;
	TRequestStatus status;
	TInt failure(1);
	iSession.__DbgMarkHeap();
	TInt err = newSession.Connect();

	// connected OK
	INFO_PRINTF1(_L("This OOM test takes a bit longer due session closes/connects"));

	do 
		{
		iSession.__DbgFailNext(failure++);
		newSession.DoSynchronisation(status, KUidIccGlobalAdnPhonebook);
		User::WaitForRequest(status);
		if ((err = status.Int()) != KErrNone)
			{
			newSession.Close();
			User::After(2000000); // 2 seconds
			iSession.__DbgCheckHeap(0);
			User::LeaveIfError(newSession.Connect());
			}
		} while (err != KErrNone);

	User::After(5000000); // 5 seconds
	newSession.Close();
	iSession.__DbgCheckHeap(0);
	iSession.__DbgMarkEnd(0);

	// OOM test OK
	INFO_PRINTF2(_L("DoSynchronisation becomes successful after %d FailNext calls"),failure);

	// check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);

	// to ask the phbksync server to check the heap 
	iSession.Close();

	// to ensure server has enough time to shut down
	User::After(3000000); 
	TInt ret(iSession.Connect());
	
	TESTCHECK(ret, KErrNone)

	return TestStepResult();
	}

//
//    SubscriberId test step1
//
CPhbkSyncSubscriberIdTest_01::CPhbkSyncSubscriberIdTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncSubscriberIdTest_01"));
	}

CPhbkSyncSubscriberIdTest_01::~CPhbkSyncSubscriberIdTest_01()
	{
	}

TVerdict CPhbkSyncSubscriberIdTest_01::doTestStepL( )
	{
	// the purpose of this test is to test that DoSync request
	// is issued and is executed in server before the method
	// ::CompleteGetPhoneStoreInfo is called via the AO.
	// By doing that, we can trigger the condition "subscriberId"
	// is not retrived in ::DoSynchronisationL. 

	DestroyDefaultIniFileL();

	// load phonebooks and don't wait after connect
	//
	
	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);
	
	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,81));
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (81 != testNumberCheck)
	{
		User::Leave(KErrNotFound);		
	}

	CleanupStack::PopAndDestroy(&testNumberProperty);
	
	iSession.Close();
	
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();

	TInt ret = iSession.Connect();
	User::LeaveIfError(ret);
	
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, 
						 KUidIccGlobalAdnPhonebook));

	// Note that these is no delay after connect is done that will trigger
	// the execution sequence: ::DoSynchronisation followed by 
	// ::CompleteGetPhoneStoreInfo in phbksync.
	//
	DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);

	// check the expected behaviour
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);

	return TestStepResult();
	}

