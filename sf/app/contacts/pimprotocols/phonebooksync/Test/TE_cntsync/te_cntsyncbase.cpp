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
//

#include <c32comm.h>
#include <f32file.h>

#include <e32base.h>
#include <e32test.h>
#include <e32svr.h>
#include <c32root.h>
#include <e32property.h>

#include "te_cntsyncadd.h"
#include "te_cntsyncdelete.h"
#include "te_cntsyncedit.h"
#include "te_cntsyncfind.h"
#include "te_cntsyncread.h"
#include "te_cntsyncstress.h"
#include "te_cntsyncbase.h"
#include "te_cntsyncsync.h"
#include "te_cntsyncutils.h"
#include "te_cntsyncview.h"

#include <e32property.h> 


const	TInt KTestStringSize=30;
typedef TBuf<KTestStringSize> TTestString;

#if defined (__WINS__)
#define PDD_NAME _L("ECDRV")
#define LDD_NAME _L("ECOMM")
#else
#define PDD_NAME _L("EUART1")
#define LDD_NAME _L("ECOMM")
#endif

#include "te_cntsyncbase.h"

 
/**
 	Instruct the PhBkSync server to configure to a 'Full' state
 	by using the Secure Backup interface. This will cause the
 	server to reconnect to ETel and SIMTSY.
 */
void CCntSyncTestStep::ConfigurePhbkSyncToFullL()
	{
	//
	// Ensure that the Secure Backup system does not currently think
	// it is performing a backup or restore.
	//
 	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURNormal, ENoBackup);

	//
	// Connect to the server to ensure that the server is running and
	// responding.
	//
	RPhoneBookSession  phoneBookSession;
	RPhoneBookSession::TPhonebookSyncMode  dummySyncMode;
	TInt ret;
	
	ret = phoneBookSession.Connect();
	TESTCHECKL(ret, KErrNone);

	ret = KErrNotReady;
	while (ret != KErrNone)
		{
		ret = phoneBookSession.GetSyncMode(dummySyncMode);
		if (ret != KErrNone)
			{
			User::After(1);
			}
		}

	phoneBookSession.Close();
	} // CCntSyncTestStep::ConfigurePhbkSyncToFullL


/**
 	Instruct the PhBkSync server to unconfigure to an 'Idle' state
 	by using the Secure Backup interface. This will cause the
 	server to disconnect from ETel and SIMTSY. This is effectively
 	the same as shutting down the server, but can be performed on
 	release versions.
 */
void CCntSyncTestStep::ConfigurePhbkSyncToIdleL()
	{
	//
	// Connect to the server to ensure that the server is running.
	//
	RPhoneBookSession  phoneBookSession;
	TInt ret;
	
	ret = phoneBookSession.Connect();
	TESTCHECKL(ret, KErrNone);

	//
	// Ensure that the Secure Backup system does not currently think
	// it is performing a backup or restore.
	//
 	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURBackupFull, EBackupBase);

	//
	// Check that the server is responding.
	//
	RPhoneBookSession::TPhonebookSyncMode  dummySyncMode;

	ret = KErrNone;
	while (ret == KErrNone)
		{
		ret = phoneBookSession.GetSyncMode(dummySyncMode);
		if (ret == KErrNone)
			{
			User::After(1);
			}
		}

	phoneBookSession.Close();
	} // CCntSyncTestStep::ConfigurePhbkSyncToIdleL
	

/** 
 * Set the synchronisation mode as aMode
 * @param aMode RPhoneBookSession::TPhonebookSyncMode enumeration
 */
void CCntSyncTestStep::SetSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode)
	{
	User::LeaveIfError(iSession.SetSyncMode(aMode));
	User::LeaveIfError(iSession.SetSyncMode(aMode, KUidIccGlobalSdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(aMode, KUidIccGlobalLndPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(aMode, KUidUsimAppAdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(aMode, KUidIccGlobalFdnPhonebook));
	}


/** 
 * Check the synchronisation mode is the same as aMode
 * @param aMode RPhoneBookSession::TPhonebookSyncMode enumeration
 */
void CCntSyncTestStep::CheckSyncModeL(RPhoneBookSession::TPhonebookSyncMode aMode)
	{
	RPhoneBookSession::TPhonebookSyncMode mode;
	User::LeaveIfError(iSession.GetSyncMode(mode));
	TESTCHECK(mode, aMode);
	User::LeaveIfError(iSession.GetSyncMode(mode, KUidIccGlobalSdnPhonebook));
	TESTCHECK(mode, aMode);
	User::LeaveIfError(iSession.GetSyncMode(mode, KUidIccGlobalLndPhonebook));
	TESTCHECK(mode, aMode);
	User::LeaveIfError(iSession.GetSyncMode(mode, KUidUsimAppAdnPhonebook));
	TESTCHECK(mode, aMode);
	User::LeaveIfError(iSession.GetSyncMode(mode, KUidIccGlobalFdnPhonebook));
	TESTCHECK(mode, aMode);
	}


/** 
 *  Check the synchronisation state is aStat
 *
 *  @param aState  An expected sync state enumeration value.
 */
void CCntSyncTestStep::CheckCacheStateL(RPhoneBookSession::TSyncState aState)
	{
	RPhoneBookSession::TSyncState state;
	TInt ret;
	
	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalAdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	TESTCHECK(state, aState);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalSdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	TESTCHECK(state, aState);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalLndPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	TESTCHECK(state, aState);

	ret = iSession.GetPhoneBookCacheState(state, KUidUsimAppAdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	TESTCHECK(state, aState);

	ret = iSession.GetPhoneBookCacheState(state, KUidIccGlobalFdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	TESTCHECK(state, aState);
	}

TVerdict CCntSyncTestStep::doTestStepPreambleL()
	{
	__UHEAP_MARK;	//mark for memory leaks!!

	iScheduler=new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iSecureBackupEngine = CSBEClient::NewL();
 	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURNormal, ENoBackup);

	TInt  ret = iSession.Connect();
	TESTCHECKL(ret, KErrNone);

	//
	// Open the existing database or create a new one.
	//
	TRAPD(err, iDb = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);

	return TestStepResult() ;
	}

TVerdict CCntSyncTestStep::doTestStepPostambleL()
	{
	delete iDb;
	iDb=NULL;

	iSession.Close();

	delete iSecureBackupEngine;
	iSecureBackupEngine = NULL;
	
	delete iScheduler;
	iScheduler = NULL;

	__UHEAP_MARKEND;

 	return TestStepResult() ;
	}


/** 
 * Set the SIM TSY test number to run
 *
 */
void CCntSyncTestStep::SetSimTsyTestNumberL(TInt aTestNumber)
	{
	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber));
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (aTestNumber != testNumberCheck)
		{
		User::Leave(KErrNotFound);		
		}

	CleanupStack::PopAndDestroy(&testNumberProperty);
	
 	delete iDb;
 	iDb=NULL;

	iSession.Close();

	//
	// Ensure that the server resets its configuration....
	//
	ConfigurePhbkSyncToIdleL();
	ConfigurePhbkSyncToFullL();

	iDb = OpenOrCreateContactDatabaseL();

	User::LeaveIfError(iSession.Connect());
		
	// Ensure that the test number has been set properly
	RProperty testNumberInUseProperty;
	User::LeaveIfError(testNumberInUseProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse));
	CleanupClosePushL(testNumberInUseProperty);

	TInt testNumberInUseCheck;
	User::LeaveIfError(testNumberInUseProperty.Get(KUidPSSimTsyCategory, KPSSimTsyTestNumberInUse,testNumberInUseCheck));
	TESTCHECKL(aTestNumber,testNumberInUseCheck);

	CleanupStack::PopAndDestroy(&testNumberInUseProperty);
	
	} // CCntSyncTestStep::SetSimTsyTestNumberL


/**
 * Publish a new value to indicate that test has finished and simtsy timer can now stop.
 *
 */
void CCntSyncTestStep::SignalSimTsyToReduceTimersL()
	{
	RProperty reduceTimersProperty;
	User::LeaveIfError(reduceTimersProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime));
	CleanupClosePushL(reduceTimersProperty);

	User::LeaveIfError(reduceTimersProperty.Set(KUidPSSimTsyCategory, KPSSimTsyTimersReduceTime,
												KReduceSimTsyTimers));

	CleanupStack::PopAndDestroy(&reduceTimersProperty);
	}


/** 
 *  Opens the Contacts Database, allowing for errors if the database exists
 *  or is not found. The function will retry where appropriate.
 *
 *  @return Pointer to opened database.
 */
CContactDatabase* CCntSyncTestStep::OpenOrCreateContactDatabaseL()
	{
	CContactDatabase*  db = NULL;
	
	//
	// 10 attempts at opening the Contacts DB...
	//
	TInt  retryCount = 10;
	
	while (retryCount > 0)
		{
		TRAPD(err, db = CContactDatabase::OpenL());
		if (err == KErrNone)
			{
			return db;
			}
		else if (err == KErrNotFound)
			{
			TRAP(err, db = CContactDatabase::CreateL());
			if (err == KErrNone)
				{
				return db;
				}
			}

		//
		// If this is the last attempt then give up...
		//
		if (retryCount == 0)
			{
			User::Leave(err);
			}

		//
		// Wait a short time and retry...
		//
		User::After(1000000);
		retryCount--;
		}

	return db;
	} // CCntSyncTestStep::OpenOrCreateContactDatabaseL


/** Do a manual synchronisation */
void CCntSyncTestStep::DoSyncL()
	{
	// Global ADN Phonebook
	TRequestStatus status; 
	iSession.DoSynchronisation(status);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Global SDN Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Global LND Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Usim App ADN Phonebook
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Global FDN Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);
	}

/** Do a manual synchronisation, but without checking for a succeess. This mehtod can 
 *	be used to test DoSync failures.
 */
void CCntSyncTestStep::DoSyncFailL()
	{
	TRequestStatus status; 
	iSession.DoSynchronisation(status, KUidIccGlobalAdnPhonebook);
	User::WaitForRequest(status);
	//If status is KErrNone then view has already been created so, sync 
	//will not fail with access denied so no point in continuing with the test
	if(status.Int() == KErrNone)
		{
		User::Leave(KErrGeneral);
		}
	TESTCHECKL(status.Int(), KErrAccessDenied);

	// Global SDN Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	//If status is KErrNone then view has already been created so, sync 
	//will not fail with access denied so no point in continuing with the test	
	if(status.Int() == KErrNone)
		{
		User::Leave(KErrGeneral);
		}	
	TESTCHECKL(status.Int(), KErrAccessDenied);

	// Global LND Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	//If status is KErrNone then view has already been created so, sync 
	//will not fail with access denied so no point in continuing with the test	
	if(status.Int() == KErrNone)
		{
		User::Leave(KErrGeneral);
		}	
	TESTCHECKL(status.Int(), KErrAccessDenied);

	// Usim App ADN Phonebook
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	//If status is KErrNone then view has already been created so, sync 
	//will not fail with access denied so no point in continuing with the test	
	if(status.Int() == KErrNone)
		{
		User::Leave(KErrGeneral);
		}	
	TESTCHECKL(status.Int(), KErrAccessDenied);

	// Global FDN Phonebook
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	//If status is KErrNone then view has already been created so, sync 
	//will not fail with access denied so no point in continuing with the test	
	if(status.Int() == KErrNone)
		{
		User::Leave(KErrGeneral);
		}	
	TESTCHECKL(status.Int(), KErrAccessDenied);
	}

void CCntSyncTestStep::WaitForSyncToCompleteL()
/**
 * Wait for all phonebooks to become syncronised.  The phonebooks checked are ADN, SDN,
 * LND, USIM ADN and FDN.
 */	{
	RPhoneBookSession::TSyncState  iccAdnState, iccSdnState, iccLndState;
	RPhoneBookSession::TSyncState  usimAdnState, iccFdnState;

	//
	// Initial check and setup of variables...
	//
	TInt  ret;
	
	ret = iSession.GetPhoneBookCacheState(iccAdnState, KUidIccGlobalAdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	ret = iSession.GetPhoneBookCacheState(iccSdnState, KUidIccGlobalSdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	ret = iSession.GetPhoneBookCacheState(iccLndState, KUidIccGlobalLndPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	ret = iSession.GetPhoneBookCacheState(usimAdnState, KUidUsimAppAdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}
	ret = iSession.GetPhoneBookCacheState(iccFdnState, KUidIccGlobalFdnPhonebook);
	if (ret != KErrNone  &&  ret != KErrNotReady)
		{
		User::Leave(ret);
		}

	//
	// Wait if needed, checking every 1 second...
	//
	while (iccAdnState  != RPhoneBookSession::ECacheValid  ||
		   iccSdnState  != RPhoneBookSession::ECacheValid  ||
		   iccLndState  != RPhoneBookSession::ECacheValid  ||
		   usimAdnState != RPhoneBookSession::ECacheValid  ||
		   iccFdnState  != RPhoneBookSession::ECacheValid) 
		{
		User::After(1000000);
		ret = iSession.GetPhoneBookCacheState(iccAdnState, KUidIccGlobalAdnPhonebook);
		if (ret != KErrNone  &&  ret != KErrNotReady)
			{
			User::Leave(ret);
			}
		ret = iSession.GetPhoneBookCacheState(iccSdnState, KUidIccGlobalSdnPhonebook);
		if (ret != KErrNone  &&  ret != KErrNotReady)
			{
			User::Leave(ret);
			}
		ret = iSession.GetPhoneBookCacheState(iccLndState, KUidIccGlobalLndPhonebook);
		if (ret != KErrNone  &&  ret != KErrNotReady)
			{
			User::Leave(ret);
			}
		ret = iSession.GetPhoneBookCacheState(usimAdnState, KUidUsimAppAdnPhonebook);
		if (ret != KErrNone  &&  ret != KErrNotReady)
			{
			User::Leave(ret);
			}
		ret = iSession.GetPhoneBookCacheState(iccFdnState, KUidIccGlobalFdnPhonebook);
		if (ret != KErrNone  &&  ret != KErrNotReady)
			{
			User::Leave(ret);
			}
		}
	}
