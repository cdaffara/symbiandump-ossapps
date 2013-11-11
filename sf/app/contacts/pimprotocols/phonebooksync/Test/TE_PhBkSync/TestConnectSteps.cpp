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
// This contains PhbkSync API Connect() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestConnectSteps.h"
 
//
//    Test step1 -- Test#1 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_01::CPhbkSyncConnectTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_01"));
	}

CPhbkSyncConnectTest_01::~CPhbkSyncConnectTest_01()
	{
	}

/**
 * Test connecting to the phonebook synchroniser server with the
 * .ini file prsent which specifies manual sync for all supported
 * phonebooks (i.e. Global ADN, USIM App ADN, SDN, LND, FDN).
 */
TVerdict CPhbkSyncConnectTest_01::doTestStepL( )
	{
	// 1. load the dummy ICC which contains all phonebooks.
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	SetSimTsyTestNumberL(0, EFalse); // see [test0] in configuration file

	// 2. change the mode as EManual for all phonebooks anc check it.
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; ++i) 
		{ 
		ChangeSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		CheckSyncModeL(RPhoneBookSession::EManual,  iPhoneBook[i]);    
		}

	// 3. load the same ICC, and Re-connect to the sever
	SetSimTsyTestNumberL(0, EFalse);  // No sync should happen

	// 4. check the expected behaviour
	for (i=0; i<KTotalNumberOfPhoneBook; ++i) 
		{ 
		CheckCacheStateInvalidL(iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test connecting to the phonebook synchroniser server with
 * the .ini file present which specifies "Automatic sme ICC"
 * sync option for Global ADN and USIM App ADN and FDN, and using the
 * "same" ICC. The sync option for the SDN and LND phonebooks
 * is "Manual".
 */
//
//    Test step2 -- Test#2 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_02::CPhbkSyncConnectTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_02"));
	}

CPhbkSyncConnectTest_02::~CPhbkSyncConnectTest_02()
	{
	}

TVerdict CPhbkSyncConnectTest_02::doTestStepL(void)
	{
	// 1. load the dummy ICC which contains all phonebooks.
	//    server will be re-connected after loading the ICC
	//    the default .ini.
	SetSimTsyTestNumberL(2); // see [test2] in configuration file

	// 2. do initial synchronisation for all phonebooks
	RPhoneBookSession::TPhonebookSyncMode mode;
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[i]));
		if (mode != RPhoneBookSession::EAutoCurrentIcc) 
			{
			iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]);
			DoSyncL(iPhoneBook[i], KErrNone);
			}
		}

	// 3. set the mode for each phonebook according to the test purpose 
	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);

	// 4. load the same ICC, and Re-connect to server
	SetSimTsyTestNumberL(2);	// This should automatically sync USIM App ADN phonebooks, 
								// Global/GSM ADN and FDN; SDN and LND unsynchronised.

	// 4.1 check the sync modes set previously are still there
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual,		KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual,		KUidIccGlobalLndPhonebook);

	// 5. check the expected behaviour
	// 5.1 the synchronisation should be done for USIM app, Global/GSM ADN and FDN phonebooks only 
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);
		
	// 5.2 the other phonebooks remain unsynchronised
	CheckCacheStateInvalidL(KUidIccGlobalSdnPhonebook); // In Manual mode, so auto sync not needed 
	CheckCacheStateInvalidL(KUidIccGlobalLndPhonebook); // In Manual mode, so auto sync not needed 

	return TestStepResult();
	}

/**
 * Test connecting to the phonebook synchroniser server with the 
 * .ini file present which specifies "Automatic same ICC" sync
 * option for Global ADN and USIM a=App ADN and FDN, and using a "different"
 * ICC. The sync option for the SDN, LND and FDN phonebooks is "Manual".
 */
//
//    Test step3 -- Test#3 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_03::CPhbkSyncConnectTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_03"));
	}

CPhbkSyncConnectTest_03::~CPhbkSyncConnectTest_03()
	{
	}

TVerdict CPhbkSyncConnectTest_03::doTestStepL(void)
	{
	// 1. load the dummy ICC which contains all phonebooks
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	// Same config as in previous test 
	SetSimTsyTestNumberL(2); // see [test2] in configuration file

	CheckCacheStateValidL(KUidUsimAppAdnPhonebook); // Set to AutoSameIcc in previous test 
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook); // Set to AutoSameIcc in previous test 
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook); // Set to AutoSameIcc in previous test 
	
	// 2. do initial synchronisation for all phonebooks
	RPhoneBookSession::TPhonebookSyncMode mode;
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[i]));
		if (mode != RPhoneBookSession::EAutoSameIcc) 
			{
			iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]);
			DoSyncL(iPhoneBook[i], KErrNone);
			}
		}

	// 3. set the EAutoSameIcc mode for Global ADN and Usim App ADN and FDN, and
	//    set the EManual mode for Emanual for SDN and LND phonebooks.
	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);

	// 4. load a new ICC different from the previous one, and Re-connect to server
	SetSimTsyTestNumberL(0);	// Re-connect again which should NOT
								// automatically sync any phonebooks.

	// 4.1 check the sync modes set previously are still there
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);
	
	// 4.2 check the expected behaviour 
	//    No synchronisation should be done for any of phonebook and the
	//    ICC phonebook does not appear int he contacts database.
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		CheckCacheStateInvalidL(iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test connecting to the phonebook synchroniser server with the 
 * .ini file present which specifies "Automatic current ICC" sync 
 * option for all supported phonebooks (i.e. Global ADN, USIM 
 * App ADN, SDN, LND and FDN) and using the "same" ICC as the previous one.
 */
//
//    Test step4 -- Test#4 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_04::CPhbkSyncConnectTest_04()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_04"));
	}

CPhbkSyncConnectTest_04::~CPhbkSyncConnectTest_04()
	{
	}

TVerdict CPhbkSyncConnectTest_04::doTestStepL(void)
	{
	// 1. load the dummy ICC which contains all phonebooks
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	SetSimTsyTestNumberL(0); // see [test0] in configuration file

	// 2. set the EAutoCurrentIcc mode for all phonebooks according to test purpose
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 3. load the same ICC, and re-connect it again using new mode set at start-up
	SetSimTsyTestNumberL(2); // this should automatically sync all phonebooks.

	// 4 check the sync modes set previously are still there
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, iPhoneBook[i]);
		}

	// 5. check the expected behaviour 
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		CheckCacheStateValidL(iPhoneBook[i]);
		}

	return TestStepResult();
	}
  
/**
 * Test connecting to the phonebook synchroniser server with the
 * .ini file present which specifies "Automatic current ICC" sync 
 * optin for Global ADN and USIM Ap ADN and FDN, and using a "different"
 * ICC to the previous one. The sync option for the SDN and LND
 * phonebooks is "Manual"
 */
//
//    Test step5 -- Test#5 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_05::CPhbkSyncConnectTest_05()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_05"));
	}

CPhbkSyncConnectTest_05::~CPhbkSyncConnectTest_05()
	{
	}

TVerdict CPhbkSyncConnectTest_05::doTestStepL(void)
	{
	// 1. load the dummy ICC which contains all phonebooks
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	SetSimTsyTestNumberL(0); // see [test0] in configuration file

	// 2. do initial synchronisation for all phonebooks
	RPhoneBookSession::TPhonebookSyncMode mode;
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[i]));
		if (mode != RPhoneBookSession::EAutoCurrentIcc) 
			{
			iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]);
			DoSyncL(iPhoneBook[i], KErrNone);
			}
		}

	// 3. set the EAutoCurrentIcc mode for Global ADN and Usim App ADN and FDN, and
	//    set the EManual mode for Emanual for SDN and LND phonebooks.
	iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidUsimAppAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalFdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);

	// 4. load a new ICC different from the previous one, and Re-connect to server
	SetSimTsyTestNumberL(2);	//  should automatically sync Global ADN USIM App ADN and FDN,
								// and leave other phonebooks unsynchronised.

	// 4.1 check the sync modes set previously are still there
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook);
	
	// 5. check the expected behaviour 
	//    No synchronisation should be done for any of phonebook and the
	//    ICC phonebook does not appear int he contacts database.
	// 5.1 the synchronisation should be done for the Global/GSM
	//     ADN and USIM application phonebooks
	CheckCacheStateValidL(KUidIccGlobalAdnPhonebook);
	CheckCacheStateValidL(KUidUsimAppAdnPhonebook);
	CheckCacheStateValidL(KUidIccGlobalFdnPhonebook);

	// 5.2 the other phonebooks remain unsynchronised
	CheckCacheStateInvalidL(KUidIccGlobalSdnPhonebook);
	CheckCacheStateInvalidL(KUidIccGlobalLndPhonebook);

	return TestStepResult();
	}

/**
 * Test connecting to the phonebook synchroniser without
 * .ini file
 */
//
//    Test step6 -- Test#6 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_06::CPhbkSyncConnectTest_06()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_06"));
	}

CPhbkSyncConnectTest_06::~CPhbkSyncConnectTest_06()
	{
	}

TVerdict CPhbkSyncConnectTest_06::doTestStepL(void)
	{
	// 1. destroy the default .ini file if it exists
	iSession.Close();
	
	ConfigurePhbkSyncToIdleL();
	DestroyDefaultIniFileL();
	ConfigurePhbkSyncToFullL();

	// 2. test connecting to the phonebook synchroniser with no .ini present
	User::LeaveIfError(iSession.Connect());
	User::After(5*KOneSeconduS); // 5 seconds to ensure server has chance to set up

	// 3. check the expected behaviour 
	// 3.1 the new .ini has been created
	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	CleanupClosePushL(fs);
	TInt err(fs.Delete(KPhbkSyncIniFile)); // Delete PhBkSync.INI file
	TESTCHECKCONDITIONL(err == KErrNone  ||  err == KErrNotFound  ||  err == KErrPathNotFound);
	CleanupStack::PopAndDestroy(); // fs

	// 3.2 It contains the group ids, templates and "Manual" for all phonebooks
	TContactItemId templateId, groupId;
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		// check template id
		templateId = KNullContactId;
		User::LeaveIfError(iSession.GetPhoneBookId(templateId, 
							RPhoneBookSession::ESyncTemplateId,
							iPhoneBook[i]));
		TESTCHECKCONDITIONL(templateId != KNullContactId);

		// check group id
		groupId = KNullContactId;
		User::LeaveIfError(iSession.GetPhoneBookId(groupId, 
							RPhoneBookSession::ESyncGroupId,
							iPhoneBook[i]));
		TESTCHECKCONDITIONL(groupId != KNullContactId);

		// sync options for all phonebooks are EManual  
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test connecting to the phonebook synchroniser with a corrupt
 * .ini file present
 */
//
//    Test step7 -- Test#7 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_07::CPhbkSyncConnectTest_07()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_07"));
	}

CPhbkSyncConnectTest_07::~CPhbkSyncConnectTest_07()
	{
	}

TVerdict CPhbkSyncConnectTest_07::doTestStepL(void)
	{
	// 1. destroy the default .ini file if it exists
	DestroyDefaultIniFileL();

	// 2. test connecting to the phonebook synchroniser with no .ini present
	SetSimTsyTestNumberL(0);

	// 3. to make the created .ini file corrupted 
	iSession.Close();
	User::After(5*KOneSeconduS); // give server enough time to shut down

	// There must be a much better way to get the file corrupted
	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	CleanupClosePushL(fs);

	RFile file;
	TInt result = file.Open(fs, KPhbkSyncIniFile,EFileWrite);
	TESTCHECKCONDITIONL(result != KErrNotFound);  

	// write some garbage into the opened file to make it corrupted
	TBuf8<1024> aBuf(_L8("-=5D!£$$%^&*FD32£$£%U_0*lfjdlf, just some garbage to make sure the .ini file is no long valid any more"));
	User::LeaveIfError( file.Write(1, // position to begin with
						aBuf));
	file.Close();
	CleanupStack::PopAndDestroy();	// fs

	// 4. to Re-connect to the server using the corrupted file 
	User::LeaveIfError(iSession.Connect()); // Re-connect again which should
										   // NOT sync any phonebook due to
										   // the corrupted .ini file. But a
										   // new .ini file should be created
										   // after this.
	User::After(5*KOneSeconduS); // give server enough time to setup phonebooks

	// 5. check the expected behaviour
	// 5.1 No synchronisation is done for any phonebook
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		CheckCacheStateInvalidL(iPhoneBook[i]);
		}

	// 5.2 the newly created .ini file has Manual sync option
	//     for all phonebooks
	iSession.Close();
	User::After(5*KOneSeconduS); // give server enough time to shut down
	User::LeaveIfError(iSession.Connect());
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}

	return TestStepResult();
	}

/**
 * Test connecting to the sever when the automatic syncrhonisation
 * of the ADN phonebook fails.
 *
 * "Automatic current ICC" sync option is specified for the Global
 * ADN and LND phonebooks. The sync option for the USIM App, SDN and FDN
 * phonebooks is "Manual"
 *
 * Test closing a phonebook synchroniser server session.
 */
//
//    Test step8 -- Test#8 in Unit Test Spec. Doc.  //
//
CPhbkSyncConnectTest_08::CPhbkSyncConnectTest_08()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_08"));
	}

CPhbkSyncConnectTest_08::~CPhbkSyncConnectTest_08()
	{
	}

TVerdict CPhbkSyncConnectTest_08::doTestStepL(void)
	{
	// 1. load the dummy ICC which contains all phonebooks
	//    server will be re-connected after loading the ICC
	//    using the default .ini.
	SetSimTsyTestNumberL(0); // see [test0] in configuration file

	// 2. do initial synchronisation for all phonebooks
	RPhoneBookSession::TPhonebookSyncMode mode;
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[i]));
		if (mode != RPhoneBookSession::EAutoCurrentIcc) 
			{
			iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]);
			DoSyncL(iPhoneBook[i], KErrNone);
			}
		}

	// 3. set the mode for each phonebook according to the test purpose 
	iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook);

	iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalLndPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalLndPhonebook);

	// 4. load the new ICC prepared for this test, and Re-connect to server
	// Since unsynchronisation of Global ADN is simulated by PhBkError in
	// cfg. file, we will force SetSimTsyTestNumberL to ignore the failure.
	SetSimTsyTestNumberL(8, EFalse); // this should only sync LND phonebook and
								  // leave other phonebooks unsynchronised.

	// 4.1 check the sync modes set previously are still there
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EAutoCurrentIcc, KUidIccGlobalLndPhonebook);

	// 5. check the expected behaviour
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{ 
		User::LeaveIfError(iSession.GetSyncMode(mode, iPhoneBook[i]));
		if (iPhoneBook[i] == KUidIccGlobalAdnPhonebook)
			{
			CheckCacheStateErrorL(iPhoneBook[i]);
			}
		else if(iPhoneBook[i] == KUidIccGlobalLndPhonebook)
			{
			CheckCacheStateValidL(iPhoneBook[i]);
			}
		else 
			{
			CheckCacheStateInvalidL(iPhoneBook[i]);
			}
		}

	return TestStepResult();
	}


CPhbkSyncConnectTest_09::CPhbkSyncConnectTest_09()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncConnectTest_09"));
	} // CPhbkSyncConnectTest_09::CPhbkSyncConnectTest_09


CPhbkSyncConnectTest_09::~CPhbkSyncConnectTest_09()
	{
	// NOP
	} // CPhbkSyncConnectTest_09::~CPhbkSyncConnectTest_09


TVerdict CPhbkSyncConnectTest_09::doTestStepL(void)
/**
 *  Test connecting to the server several times when the IMSI number is
 *  returned empty each time. This used to cause an invalid 
 */
	{
	//
	// This test will require the ADN phonebook to be sync'd manually...
	//
	iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);
	CheckSyncModeL(RPhoneBookSession::EManual, KUidIccGlobalAdnPhonebook);

	//
	// Load SIMTSY config 90...
	//
	SetSimTsyTestNumberL(90);

	//
	// Stop & start PhBkSync three times, syncing each time. This will force
	// the server to request the IMSI number, write it to the INI file and
	// read it again (three times).
	//
	TInt  counter;

	for (counter = 0;  counter < 3; counter++)
		{
		iSession.Close();
		
		ConfigurePhbkSyncToIdleL();
		ConfigurePhbkSyncToFullL();

		User::LeaveIfError(iSession.Connect());

		DoSyncL(KUidIccGlobalAdnPhonebook, KErrNone);
		}

	return TestStepResult();
	} // CPhbkSyncConnectTest_09::doTestStepL


//
//    OOM Connect step1
//
CPhbkSyncOOMConnectTest_01::CPhbkSyncOOMConnectTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOOMConnectTest_01"));
	}

CPhbkSyncOOMConnectTest_01::~CPhbkSyncOOMConnectTest_01()
	{
	}

TVerdict CPhbkSyncOOMConnectTest_01::doTestStepL( )
	{
	User::After(3000000); 
	RPhoneBookSession newSession;
	TInt ret(-1), failure(1);
	iSession.__DbgMarkHeap();

	while (ret != KErrNone)
	  {
	    iSession.__DbgFailNext(failure);
	    ret = newSession.Connect();
	    if (ret != KErrNone)
	      iSession.__DbgCheckHeap(0);
	    failure++;
	  }

	// connected OK
	INFO_PRINTF2(_L("Connect becomes successful after %d FailNext calls"), failure);
	newSession.Close();
	
	// flush any FailNext there might be hanging around
	iSession.__DbgCheckHeap(0);
	iSession.__DbgMarkEnd(0);
	return TestStepResult();
	}

