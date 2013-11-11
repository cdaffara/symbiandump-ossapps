// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Telephony Fax Test server test code.
// 
//

/**
 @file
 @internalComponent
*/

#include "TE_cntSyncServer.h"
#include "te_cntsyncbase.h"

#include "te_cntsyncread.h"
#include "te_cntsyncsync.h"
#include "te_cntsyncadd.h"
#include "te_cntsyncedit.h"
#include "te_cntsyncdelete.h"
#include "te_cntsyncfind.h"
#include "te_cntsyncstress.h"
#include "te_cntsyncview.h"

_LIT(KServerName,"TE_CntSync");


CCntSyncTestServer::~CCntSyncTestServer()
	{
	//
	// Shutdown the server if possible (only works in debug mode).
	//
	RPhoneBookSession  phoneBookSession;
	TInt  ret;
	
	ret = phoneBookSession.Connect();
	if (ret == KErrNone)
		{
		phoneBookSession.ShutdownServer(EFalse);
		phoneBookSession.Close();
		}
	} // CCntSyncTestServer::~CCntSyncTestServer


/**
 *  Called inside the MainL() function to create and start the CTestServer
 *  derived server.
 *
 *  @return Instance of the test server
 */
CCntSyncTestServer* CCntSyncTestServer::NewL()
	{
	CCntSyncTestServer*  server = new(ELeave) CCntSyncTestServer();
	CleanupStack::PushL(server);
	// CServer base class call
	server->StartL(KServerName);
	CleanupStack::Pop(server);
	return server;
	} // CFaxTestIntServer::NewL


/**
 *  Uses the new Rendezvous() call to sync with the client.
 */
LOCAL_C void MainL()
	{
	//
	// Start an active scheduler...
	//
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	//
	// Ensure the SIMTSY config number is reset prior to starting...
	//
	TInt  result;
	
	result = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, 0);
	if (result != KErrNone  &&  result != KErrNotFound)
		{
		User::Leave(result);
		}

	//
	// Create the CTestServer derived server...
	//
	CCntSyncTestServer* server = NULL;

	TRAPD(err, server = CCntSyncTestServer::NewL());
	if (err == KErrNone)
		{
		//
		// Sync with the client and enter the active scheduler
		//
		RProcess::Rendezvous(KErrNone);
		scheduler->Start();
		}

	//
	// Clean up...
	//
	CleanupStack::Pop(scheduler);
	delete server;
	delete scheduler;
	} // MainL
/**
 *  @return Standard Epoc error code on exit.
 */
GLDEF_C TInt E32Main()
	{
	CTrapCleanup*  cleanup = CTrapCleanup::New();

	if (cleanup == NULL)
		{
		return KErrNoMemory;
		}

	TRAPD(err, MainL());

	delete cleanup;

	return err;
	} // E32Main


/**
 *  Implementation of CTestServer pure virtual.
 *
 *  @return A CTestStep derived instance.
 */
CTestStep* CCntSyncTestServer::CreateTestStep(const TDesC& aStepName)
	{
    //
	// Create the required test step...
	//
	CTestStep*  testStep = NULL;

	if (aStepName == _L("ReadICCContactTest"))
		{
		testStep = new CPhbkSyncReadICCContactTest();
		}
	else if (aStepName == _L("ReadAdditionalNumTagsTest"))
		{
		testStep = new CPhbkSyncReadAdditionalNumTagsTest();
		}
	else if (aStepName == _L("ReadSequenceOfICCEntriesTest"))
		{
		testStep = new CPhbkSyncReadSequenceOfICCEntriesTest();
		}
	else if (aStepName == _L("ReadContactInvalidUIDTest"))
		{
		testStep = new CPhbkSyncReadContactInvalidUIDTest();
		}
	else if (aStepName == _L("ReadContactsEmptyFieldTest"))
		{
		testStep = new CPhbkSyncReadContactsEmptyFieldTest();
		}
	else if (aStepName == _L("ReadContactFullViewICCLockedTest"))
		{
		testStep = new CPhbkSyncReadContactFullViewICCLockedTest();
		}
	else if (aStepName == _L("ReadContactDiffViewsTest"))
		{
		testStep = new CPhbkSyncReadContactDiffViewsTest();
		}
	else if (aStepName == _L("ReadICCMinimalContactTest"))
		{
		testStep = new CPhbkSyncReadICCMinimalContactTest();
		}
	else if (aStepName == _L("ReadMinimalContactInvalidUIDTest"))
		{
		testStep = new CPhbkSyncReadMinimalContactInvalidUIDTest();
		}
	else if (aStepName == _L("ReadMinimalContactEmptyFieldTest"))
		{
		testStep = new CPhbkSyncReadMinimalContactEmptyFieldTest();
		}
	else if (aStepName == _L("ReadMinimalContactICCLockedTest"))
		{
		testStep = new CPhbkSyncReadMinimalContactICCLockedTest();
		}
	else if (aStepName == _L("ReadContactTextDefFullViewTest"))
		{
		testStep = new CPhbkSyncReadContactTextDefFullViewTest();
		}
	else if (aStepName == _L("ReadContactNameTextDefTest"))
		{
		testStep = new CPhbkSyncReadContactNameTextDefTest();
		}
	else if (aStepName == _L("ReadContactNumberTextDefTest"))
		{
		testStep = new CPhbkSyncReadContactNumberTextDefTest();
		}
	else if (aStepName == _L("ReadContactInvalidTextDefTest"))
		{
		testStep = new CPhbkSyncReadContactInvalidTextDefTest();
		}
	else if (aStepName == _L("ReadContactICCLockedTest"))
		{
		testStep = new CPhbkSyncReadContactICCLockedTest();
		}
	else if (aStepName == _L("ReadFullICCNameViewTest"))
		{
		testStep = new CPhbkSyncReadFullICCNameViewTest();
		}
	else if (aStepName == _L("ReadFullICCNumberViewTest"))
		{
		testStep = new CPhbkSyncReadFullICCNumberViewTest();
		}
	else if (aStepName == _L("ReadFullICCFullViewTest"))
		{
		testStep = new CPhbkSyncReadFullICCFullViewTest();
		}
	else if (aStepName == _L("ReadFullICCNameTextDefTest"))
		{
		testStep = new CPhbkSyncReadFullICCNameTextDefTest();
		}
	else if (aStepName == _L("ReadFullICCNumberTextDefTest"))
		{
		testStep = new CPhbkSyncReadFullICCNumberTextDefTest();
		}
	else if (aStepName == _L("ReadFullICCFullViewTextDefTest"))
		{
		testStep = new CPhbkSyncReadFullICCFullViewTextDefTest();
		}
	else if (aStepName == _L("ReadInternationalNumberTest"))
		{
		testStep = new CPhbkSyncReadInternationalNumberTest();
		}
	else if (aStepName == _L("ReadWithOutOpeningPhBkSyncFirstTest"))
		{
		testStep = new CPhbkSyncReadWithOutOpeningPhBkSyncFirstTest();
		}
	else if (aStepName == _L("SyncAutomaticSameICCTest"))
		{
		testStep = new CPhbkSyncAutomaticSameICCTest();
		}
	else if (aStepName == _L("SyncAutomaticCurrentICCTest"))
		{
		testStep = new CPhbkSyncAutomaticCurrentICCTest();
		}
	else if (aStepName == _L("SyncManualTest"))
		{
		testStep = new CPhbkSyncManualTest();
		}
	else if (aStepName == _L("SyncManualNotReadyTest"))
		{
		testStep = new CPhbkSyncManualNotReadyTest();
		}
	else if (aStepName == _L("SyncManualDiffSizeSIMTest"))
		{
		testStep = new CPhbkSyncManualDiffSizeSIMTest();
		}
	else if (aStepName == _L("SyncSATUpdatesTest"))
		{
		testStep = new CPhbkSyncSATUpdatesTestBase();
		}
	else if (aStepName == _L("SyncConsecutiveTest"))
		{
		testStep = new CPhbkSyncConsecutiveTestBase();
		}
	else if (aStepName == _L("SyncMultipleRequestsTest"))
		{
		testStep = new CPhbkSyncMultipleRequestsTestBase();
		}
	else if (aStepName == _L("SyncGeneralResponseTest"))
		{
		testStep = new CPhbkSyncGeneralResponseTestBase();
		}
	else if (aStepName == _L("SyncICCLockedTest"))
		{
		testStep = new CPhbkSyncICCLockedTestBase();
		}
	else if (aStepName == _L("SyncFailureTest"))
		{
		testStep = new CPhbkSyncFailureTestBase();
		}
	else if (aStepName == _L("SyncFailureAndCheckErrorTest"))
		{
		testStep = new CPhbkSyncFailureAndCheckErrorTestBase();
		}
	else if (aStepName == _L("SyncNotificationTest"))
		{
		testStep = new CPhbkSyncNotificationTestBase();
		}
	else if (aStepName == _L("SyncDeleteDBTest"))
		{
		testStep = new CPhbkSyncDeleteDBTestBase();
		}
	else if (aStepName == _L("SyncEmptyICCTest"))
		{
		testStep = new CPhbkSyncEmptyICCTestBase();
		}
	else if (aStepName == _L("SyncICCSlotsManipulationTest"))
		{
		testStep = new CPhbkSyncICCSlotsManipulationTestBase();
		}
	else if (aStepName == _L("SyncICCSlotsManipulationAndWriteTest"))
		{
		testStep = new CPhbkSyncICCSlotsManipulationAndWriteTestBase();
		}
	else if (aStepName == _L("SyncAndGetContactFormatTest"))
		{
		testStep = new CPhbkSyncGetAndContactFormatTestBase();
		}
	else if (aStepName == _L("SetSyncModeAndAutoSyncTest"))
		{
		testStep = new CPhbkSyncSetSyncModeAndAutoSyncTest();
		}
	else if (aStepName == _L("SyncUnsupportedPhonebook"))
		{
		testStep = new CPhbkSyncSyncUnsupportedPhonebook();
		}
	else if (aStepName == _L("SyncDiffNumberTypesRepeatedly"))
		{
		testStep = new CPhbkSyncSyncDiffNumberTypesRepeatedly();
		}
	else if (aStepName == _L("SyncInternationalNumbers"))
		{
		testStep = new CPhbkSyncSyncInternationalNumbers();
		}
	else if (aStepName == _L("SyncAutomaticWithUnsupportedPhonebook"))
		{
		testStep = new CPhbkSyncSyncAutomaticWithUnsupportedPhonebook();
		}
	else if (aStepName == _L("AddICCContactTest"))
		{
		testStep = new CPhbkAddICCContactTest();
		}
	else if (aStepName == _L("AddEntryEmptyNameTest"))
		{
		testStep = new CPhbkAddEntryEmptyNameTest();
		}
	else if (aStepName == _L("AddEntryEmptyNumberTest"))
		{
		testStep = new CPhbkAddEntryEmptyNumberTest();
		}
	else if (aStepName == _L("AddEntryEmptyEmailTest"))
		{
		testStep = new CPhbkAddEntryEmptyEmailTest();
		}
	else if (aStepName == _L("AddAdditionalNumWithTagTest"))
		{
		testStep = new CPhbkAddAdditionalNumWithTagTest();
		}
	else if (aStepName == _L("AddPhonebookFullTest"))
		{
		testStep = new CPhbkAddPhonebookFullTest();
		}
	else if (aStepName == _L("AddBoundaryConditionsTest"))
		{
		testStep = new CPhbkAddBoundaryConditionsTest();
		}
	else if (aStepName == _L("AddNameBoundaryExceededTest"))
		{
		testStep = new CPhbkAddNameBoundaryExceededTest();
		}
	else if (aStepName == _L("AddNumberBoundaryExceededTest"))
		{
		testStep = new CPhbkAddNumberBoundaryExceededTest();
		}
	else if (aStepName == _L("AddSequenceOfEntriesTest"))
		{
		testStep = new CPhbkAddSequenceOfEntriesTest();
		}
	else if (aStepName == _L("AddEntryICCLockedTest"))
		{
		testStep = new CPhbkAddEntryICCLockedTest();
		}
	else if (aStepName == _L("AddEntryICCWriteFailsTest"))
		{
		testStep = new CPhbkAddEntryICCWriteFailsTest();
		}
	else if (aStepName == _L("AddEntryInvalidFieldTest"))
		{
		testStep = new CPhbkAddEntryInvalidFieldTest();
		}
	else if (aStepName == _L("AddEntryNumberWithDTMFTest"))
		{
		testStep = new CPhbkAddEntryNumberWithDTMFTest();
		}
	else if (aStepName == _L("AddEntryReadOnlyAccessTest"))
		{
		testStep = new CPhbkAddEntryReadOnlyAccessTest();
		}
	else if (aStepName == _L("OpenICCContactTest"))
		{
		testStep = new CPhbkOpenICCContactTest();
		}
	else if (aStepName == _L("OpenContactInvalidUIDTest"))
		{
		testStep = new CPhbkOpenContactInvalidUIDTest();
		}
	else if (aStepName == _L("EditICCContactTest"))
		{
		testStep = new CPhbkEditICCContactTest();
		}
	else if (aStepName == _L("EditICCContactTwiceTest"))
		{
		testStep = new CPhbkEditICCContactTwiceTest();
		}
	else if (aStepName == _L("EditContactEmptyNameTest"))
		{
		testStep = new CPhbkEditContactEmptyNameTest();
		}
	else if (aStepName == _L("EditContactEmptyNumberTest"))
		{
		testStep = new CPhbkEditContactEmptyNumberTest();
		}
	else if (aStepName == _L("EditContactNameExceededTest"))
		{
		testStep = new CPhbkEditContactNameExceededTest();
		}
	else if (aStepName == _L("EditContactNumberExceededTest"))
		{
		testStep = new CPhbkEditContactNumberExceededTest();
		}
	else if (aStepName == _L("EditContactICCLockedTest"))
		{
		testStep = new CPhbkEditContactICCLockedTest();
		}
	else if (aStepName == _L("EditContactICCWriteFailsTest"))
		{
		testStep = new CPhbkEditContactICCWriteFailsTest();
		}
	else if (aStepName == _L("EditContactEmptyFieldsTest"))
		{
		testStep = new CPhbkEditContactEmptyFieldsTest();
		}
	else if (aStepName == _L("EditSlotWriteTest"))
		{
		testStep = new CPhbkEditSlotWriteTest();
		}
	else if (aStepName == _L("OpenContactICCLockedTest"))
		{
		testStep = new CPhbkOpenContactICCLockedTest();
		}
	else if (aStepName == _L("OpenEntryReadOnlyAccessTest"))
		{
		testStep = new CPhbkOpenEntryReadOnlyAccessTest();
		}
	else if (aStepName == _L("OpenFullICCTest"))
		{
		testStep = new CPhbkOpenFullICCTest();
		}
	else if (aStepName == _L("DeleteICCContactTest"))
		{
		testStep = new CPhbkDeleteICCContactTest();
		}
	else if (aStepName == _L("DeleteEntryInvalidUIDTest"))
		{
		testStep = new CPhbkDeleteEntryInvalidUIDTest();
		}
	else if (aStepName == _L("DeleteEntryICCLockedTest"))
		{
		testStep = new CPhbkDeleteEntryICCLockedTest();
		}
	else if (aStepName == _L("DeleteEntryICCDeleteFailsTest"))
		{
		testStep = new CPhbkDeleteEntryICCDeleteFailsTest();
		}
	else if (aStepName == _L("DeleteEntryReadOnlyAccessTest"))
		{
		testStep = new CPhbkDeleteEntryReadOnlyAccessTest();
		}
	else if (aStepName == _L("DeleteArrayOneEntryTest"))
		{
		testStep = new CPhbkDeleteArrayOneEntryTest();
		}
	else if (aStepName == _L("DeleteArrayInvalidIdTest"))
		{
		testStep = new CPhbkDeleteArrayInvalidIdTest();
		}
	else if (aStepName == _L("DeleteArrayMultipleEntriesTest"))
		{
		testStep = new CPhbkDeleteArrayMultipleEntriesTest();
		}
	else if (aStepName == _L("DeleteArrayICCLockedTest"))
		{
		testStep = new CPhbkDeleteArrayICCLockedTest();
		}
	else if (aStepName == _L("DeleteArrayReadOnlyAccessTest"))
		{
		testStep = new CPhbkDeleteArrayReadOnlyAccessTest();
		}
	else if (aStepName == _L("DeleteArrayICCDeleteFailsTest"))
		{
		testStep = new CPhbkDeleteArrayICCDeleteFailsTest();
		}
	else if (aStepName == _L("DeleteArrayInvalidUIDTest"))
		{
		testStep = new CPhbkDeleteArrayInvalidUIDTest();
		}
	else if (aStepName == _L("DeleteArrayFullPhonebookTest"))
		{
		testStep = new CPhbkDeleteArrayFullPhonebookTest();
		}
	else if (aStepName == _L("DeleteAllEntriesTest"))
		{
		testStep = new CPhbkDeleteAllEntriesTest();
		}
	else if (aStepName == _L("DeleteSlotEntryTwiceTest"))
		{
		testStep = new CPhbkDeleteSlotEntryTwiceTest();
		}
	else if (aStepName == _L("FindByNameTest"))
		{
		testStep = new CPhbkFindByNameTest();
		}
	else if (aStepName == _L("FindByNumberTest"))
		{
		testStep = new CPhbkFindByNumberTest();
		}
	else if (aStepName == _L("FindByNonExistingNameTest"))
		{
		testStep = new CPhbkFindByNonExistingNameTest();
		}
	else if (aStepName == _L("FindByNonExistingNumberTest"))
		{
		testStep = new CPhbkFindByNonExistingNumberTest();
		}
	else if (aStepName == _L("FindByNumberAsyncSearchTest"))
		{
		testStep = new CPhbkFindByNumberAsyncSearchTest();
		}
	else if (aStepName == _L("FindByNameAsyncSearchTest"))
		{
		testStep = new CPhbkFindByNameAsyncSearchTest();
		}
	else if (aStepName == _L("FindByNonExistNumAsyncSearchTest"))
		{
		testStep = new CPhbkFindByNonExistNumAsyncSearchTest();
		}
	else if (aStepName == _L("FindByNameICCLockedTest"))
		{
		testStep = new CPhbkFindByNameICCLockedTest();
		}
	else if (aStepName == _L("FindByNumberICCLockedTest"))
		{
		testStep = new CPhbkFindByNumberICCLockedTest();
		}
	else if (aStepName == _L("FindByNumICCLockedAsyncSearchTest"))
		{
		testStep = new CPhbkFindByNumICCLockedAsyncSearchTest();
		}
	else if (aStepName == _L("FindByInternationalPrefixTest"))
		{
		testStep = new CPhbkFindByInternationalPrefixTest();
		}
	else if (aStepName == _L("FindByInterPrefixAsyncSearchTest"))
		{
		testStep = new CPhbkFindByInterPrefixAsyncSearchTest();
		}
	else if (aStepName == _L("LaunchServerSimultaneouslyTest"))
		{
		testStep = new CPhbkLaunchServerSimultaneouslyTest();
		}
	else if (aStepName == _L("RequestDoSyncSimultaneouslyTest"))
		{
		testStep = new CPhbkRequestDoSyncSimultaneouslyTest();
		}
	else if (aStepName == _L("RequestReadSimultaneouslyTest"))
		{
		testStep = new CPhbkRequestReadSimultaneouslyTest();
		}
	else if (aStepName == _L("RequestWriteSimultaneouslyTest"))
		{
		testStep = new CPhbkRequestWriteSimultaneouslyTest();
		}
	else if (aStepName == _L("RequestDeleteSimultaneouslyTest"))
		{
		testStep = new CPhbkRequestDeleteSimultaneouslyTest();
		}
	else if (aStepName == _L("RequestSyncAndReadSimultaneouslyTest"))
		{
		testStep = new CPhbkRequestSyncAndReadSimultaneouslyTest();
		}
	else if (aStepName == _L("CheckServerThreadPriorityTest"))
		{
		testStep = new CPhbkCheckServerThreadPriorityTest();
		}
	else if (aStepName == _L("ServerLostFileRecoveryTest"))
		{
		testStep = new CPhbkServerLostFileRecoveryTest();
		}
	else if (aStepName == _L("IntegrationBackupTest"))
		{
		testStep = new CPhbkIntegrationBackupTest();
		}
	else if (aStepName == _L("ViewICCSyncTest"))
		{
		testStep = new CPhbkViewICCSyncTest();
		}
	else if (aStepName == _L("ViewICCLockedICCTest"))
		{
		testStep = new CPhbkViewICCLockedICCTest();
		}
	else if (aStepName == _L("ViewICCLockedMixedTest"))
		{
		testStep = new CPhbkViewICCLockedMixedTest();
		}
	else if (aStepName == _L("ViewICCSinglePhonebookTest"))
		{
		testStep = new CPhbkViewICCSinglePhonebookTest();
		}
	else if (aStepName == _L("AddEntryWithNoTemplate"))
		{
		testStep = new CPhbkAddEntryWithNoTemplateTest();
		}
	
   	//
	// Set the test step name here to save code!!!
	//
	if (testStep != NULL)
		{
		testStep->SetTestStepName(aStepName);
		}

	return testStep;
	} // CCntSyncTestServer::CreateTestStep
