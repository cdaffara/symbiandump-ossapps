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
#include <e32property.h> 

#include "te_cntsyncbase.h"
#include "te_cntsyncstress.h"

#ifdef SYMBIAN_CNTMODEL_V2
_LIT(KContactsDBFile, "C:\\Private\\10003A73\\CONTACTS.CDB");
_LIT(KContactsDBFileBackup, "C:\\Private\\10003A73\\CONTACTS.BAK");
_LIT(KContactsSqliteDBFile, "C:\\Private\\10003A73\\SQLite__CONTACTS.CDB");
_LIT(KContactsSqliteDBFileBackup, "C:\\Private\\10003A73\\SQLite__CONTACTS.BAK");
#else
_LIT(KContactsDBFile, "C:\\Private\\100012A5\\DBS_100065FF_CONTACTS.CDB");
_LIT(KContactsDBFileBackup, "C:\\Private\\100012A5\\DBS_100065FF_CONTACTS.BAK");
#endif // SYMBIAN_CNTMODEL_V2
_LIT(KContactsModelIniFile, "C:\\Private\\10003A73\\CntModel.ini");
_LIT(KPhbkSyncIniFile,"C:\\Private\\102030A1\\Phbksync.ini");
_LIT(KPhbkSyncIniFileBackup, "C:\\Private\\102030A1\\Phbksync.bak");


/** 
 * Factory construction method.
 * @return Pointer to CPhbkRequestDoSyncSimultaneouslyTest object
 */
CPhbkRequestDoSyncSimultaneouslyTest* CPhbkRequestDoSyncSimultaneouslyTest::NewL()
	{
	CPhbkRequestDoSyncSimultaneouslyTest* self = new(ELeave) CPhbkRequestDoSyncSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkRequestDoSyncSimultaneouslyTest::CPhbkRequestDoSyncSimultaneouslyTest()
	{
	SetTestStepName(_L("RequestDoSyncSimultaneouslyTest"));
	}

/**
 * Request two simultaneous synchronisations. 
 * Two threads try to do a Synchronisation at once. 
 */
enum TVerdict CPhbkRequestDoSyncSimultaneouslyTest::doTestStepL()
	{
	const TInt KStackSize=0x8000;
	const TInt KHeapSize=0x8000;
	const TInt KMaxHeapSize=0x80000;

	_LIT(KThreadName1,"Thread1");
	_LIT(KThreadName2,"Thread2");

	RThread t1;
	RThread t2; 

	// Create the thread
	User::LeaveIfError(t1.Create(KThreadName1,DoSyncThread,KStackSize,KHeapSize,KMaxHeapSize,NULL));
	User::LeaveIfError(t2.Create(KThreadName2,DoSyncThread,KStackSize,KHeapSize,KMaxHeapSize,NULL));

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TBool req1Complete(EFalse);
	TBool req2Complete(EFalse);
	
	t1.Logon(reqStatus1); // Notify when the thread dies
	t2.Logon(reqStatus2);
	t1.Resume(); // Start the threads
	t2.Resume();
	User::WaitForRequest(reqStatus1); // Avoid using User::WaitForAnyRequest()
	User::WaitForRequest(reqStatus2); 
		
	while(req1Complete==EFalse || req2Complete==EFalse)
		{
		if (reqStatus1 != KRequestPending && req1Complete==EFalse) // Thread1 request completed
			{
			TESTCHECK(reqStatus1.Int(), KErrNone);
			req1Complete=ETrue;
			}

		if (reqStatus2 != KRequestPending && req2Complete==EFalse) // Thread2 request rejected
			{
			TESTCHECK(reqStatus2.Int(), KErrNone);
			req2Complete=ETrue;
			}
		}

	t1.Close();
	t2.Close();

	return TestStepResult();
	}

/**
 * Control passes to this function when the thread is first resumed, 
 * i.e. when the thread is initially scheduled to run.
 */
TInt CPhbkIntegrationStressTestBase::DoSyncThread(TAny* /*aPtr*/)
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAP_IGNORE(DoSyncThreadL());
	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}

/**
 * 
 */
void CPhbkIntegrationStressTestBase::DoSyncThreadL()
	{
	RPhoneBookSession session;
	User::LeaveIfError(session.Connect()); // Connect to the Phonebook Server
	TRequestStatus status;
	
	// Attempt synchronisation for each phonebook
	// Global ADN
	session.DoSynchronisation(status);
	User::WaitForRequest(status);
	__ASSERT_ALWAYS((status.Int()==KErrNone) || (status.Int()==KErrInUse), User::Invariant());
	

	// Global SDN
	session.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::WaitForRequest(status);
	__ASSERT_ALWAYS((status.Int()==KErrNone) || (status.Int()==KErrInUse), User::Invariant());
	
	// Global LND
	session.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::WaitForRequest(status);
	__ASSERT_ALWAYS((status.Int()==KErrNone) || (status.Int()==KErrInUse), User::Invariant());
	
	// USIM App
	session.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::WaitForRequest(status);
	__ASSERT_ALWAYS((status.Int()==KErrNone) || (status.Int()==KErrInUse), User::Invariant());
	

	// Global FDN
	session.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::WaitForRequest(status);
	__ASSERT_ALWAYS((status.Int()==KErrNone) || (status.Int()==KErrInUse), User::Invariant());	
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkLaunchServerSimultaneouslyTest object
 */
CPhbkLaunchServerSimultaneouslyTest* CPhbkLaunchServerSimultaneouslyTest::NewL()
	{
	CPhbkLaunchServerSimultaneouslyTest* self = new(ELeave) CPhbkLaunchServerSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkLaunchServerSimultaneouslyTest::CPhbkLaunchServerSimultaneouslyTest()
	{
	SetTestStepName(_L("LaunchServerSimultaneouslyTest"));
	}

/** 
 * Create two threads which simultaneously attempt to connect to the Phonebook Server.
 */
enum TVerdict CPhbkLaunchServerSimultaneouslyTest::doTestStepL()
	{
	SetSimTsyTestNumberL(9);

	RThread thread1;
	RThread thread2;
	// Create the threads
	User::LeaveIfError(thread1.Create(_L("Thread1"), ServerLaunchThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread2.Create(_L("Thread2"), ServerLaunchThread, KDefaultStackSize, 0x2000, 0x20000, NULL));	
	thread1.Resume(); // Start the threads
	thread2.Resume();
	User::After(5000000);
	thread1.Close();
	thread2.Close();

	return TestStepResult();
	}


/** 
 * Phonebook Server Launch thread function. 
 */
TInt CPhbkIntegrationStressTestBase::ServerLaunchThread(TAny* /*aAny*/)
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		return KErrNoMemory;

	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	CContactDatabase* db =NULL;
	TRAPD(err, db = CContactDatabase::OpenL());
	__ASSERT_DEBUG(err==KErrNone,User::Invariant());
	TRAP(err, CheckServerThreadCountL(1));
	__ASSERT_DEBUG(err==KErrNone,User::Invariant());
	delete db;
	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}

/**
 * Checks the server thread count.
 */
void CPhbkIntegrationStressTestBase::CheckServerThreadCountL(TInt aExpectedCount)
	{
	TFullName name;
	TInt count=0;
	_LIT(KThreadToFind,"*PhBkSyncServer*"); // PHBKSYNC_SVR_NAME
	TFindThread find(KThreadToFind);
	
	RThread candidate;
	while(find.Next(name)==KErrNone)
		{
		// If the thread still lives, increase count.
		if(candidate.Open(name)==KErrNone)
			{
			const TExitCategoryName& category = candidate.ExitCategory();

			RDebug::Print(_L("[%x] CheckServerThreadCount thread: %x -----"), (TUint)RThread().Id(), (TUint)candidate.Id());
			RDebug::Print(_L("[%x] Thread %x ExitCategory %S"), (TUint)RThread().Id(), (TUint)candidate.Id(), &category);
			RDebug::Print(_L("[%x] Thread %x ExitReason   %d"), (TUint)RThread().Id(), (TUint)candidate.Id(), candidate.ExitReason());
			RDebug::Print(_L("[%x] Thread %x ExitType     %d"), (TUint)RThread().Id(), (TUint)candidate.Id(), candidate.ExitType());

			// If the thread has an ExitCategory, it is dead and not worthy of counting
			if(category.Length()==0)
			    {
				count++;
                }

			candidate.Close();
			}
		}

	__ASSERT_ALWAYS(count==aExpectedCount, User::Invariant()); 
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkRequestReadSimultaneouslyTest object
 */
CPhbkRequestReadSimultaneouslyTest* CPhbkRequestReadSimultaneouslyTest::NewL()
	{
	CPhbkRequestReadSimultaneouslyTest* self = new(ELeave) CPhbkRequestReadSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkRequestReadSimultaneouslyTest::CPhbkRequestReadSimultaneouslyTest()
	{
	SetTestStepName(_L("RequestReadSimultaneouslyTest"));
	}

/** 
 * Create three threads which simultaneously attempt to request Read from
 * the Phonebook Server.
 */
enum TVerdict CPhbkRequestReadSimultaneouslyTest::doTestStepL()
	{
	RThread thread1;
	RThread thread2;
	RThread thread3;
	// Create the threads
	User::LeaveIfError(thread1.Create(_L("ThreadA"), MultipleReadThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread2.Create(_L("ThreadB"), MultipleReadThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread3.Create(_L("ThreadC"), MultipleReadThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	
	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TRequestStatus reqStatus3;

	thread1.Logon(reqStatus1); // Notify when the thread dies
	thread2.Logon(reqStatus2);
	thread3.Logon(reqStatus3);
	thread1.Resume(); // Start the thread
	thread2.Resume();
	thread3.Resume();
	User::WaitForRequest(reqStatus1); // Avoid using User::WaitForAnyRequest()
	User::WaitForRequest(reqStatus2); 
	User::WaitForRequest(reqStatus3);

	TBool req1Complete(EFalse);
	TBool req2Complete(EFalse);
	TBool req3Complete(EFalse);

	while(req1Complete==EFalse || req2Complete==EFalse || req3Complete==EFalse)
		{
		if (reqStatus1 != KRequestPending && req1Complete==EFalse) // Thread1 request completed
			{
			TESTCHECK(reqStatus1.Int(), KErrNone);
			req1Complete=ETrue;
			}

		if (reqStatus2 != KRequestPending && req2Complete==EFalse) // Thread2 request rejected
			{
			TESTCHECK(reqStatus2.Int(), KErrNone);
			req2Complete=ETrue;
			}

		if (reqStatus3 != KRequestPending && req3Complete==EFalse) // Thread3 request rejected
			{
			TESTCHECK(reqStatus3.Int(), KErrNone);
			req3Complete=ETrue;
			}
		}
	
//	User::After(5000000);
	thread1.Close();
	thread2.Close();
	thread3.Close();

	return TestStepResult();
	}

/** 
 * Send Read request to the Phonebook server.
 */
TInt CPhbkIntegrationStressTestBase::MultipleReadThread(TAny* /*aAny*/)
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAP_IGNORE(MultipleReadThreadL()); 
	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}

/** 
 * 
 */
void CPhbkIntegrationStressTestBase::MultipleReadThreadL()
	{
	CPhbkIntegrationTestUtility* access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidIccGlobalAdnPhonebook, KTest9ICCSlotsADN);
	access->ReadContactL(KUidIccGlobalSdnPhonebook, KTest9ICCSlotsSDN);
	access->ReadContactL(KUidIccGlobalLndPhonebook, KTest9ICCSlotsLND);
	access->ReadContactL(KUidUsimAppAdnPhonebook, KTest9ICCSlotsUsim);

	CleanupStack::PopAndDestroy(access);
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkRequestWriteSimultaneouslyTest object
 */
CPhbkRequestWriteSimultaneouslyTest* CPhbkRequestWriteSimultaneouslyTest::NewL()
	{
	CPhbkRequestWriteSimultaneouslyTest* self = new(ELeave) CPhbkRequestWriteSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkRequestWriteSimultaneouslyTest::CPhbkRequestWriteSimultaneouslyTest()
	{
	SetTestStepName(_L("RequestWriteSimultaneouslyTest"));
	}

/** 
 * Create three threads which simultaneously attempt to request Write from
 * the Phonebook Server.
 */
enum TVerdict CPhbkRequestWriteSimultaneouslyTest::doTestStepL()
	{
	SetSimTsyTestNumberL(1);
	DoSyncL();

	RThread thread1;
	RThread thread2;
	RThread thread3;
	// Create the threads
	User::LeaveIfError(thread1.Create(_L("ThreadX"), MultipleWriteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread2.Create(_L("ThreadY"), MultipleWriteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread3.Create(_L("ThreadZ"), MultipleWriteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TRequestStatus reqStatus3;

	thread1.Logon(reqStatus1); // Notify when the thread dies 
	thread2.Logon(reqStatus2);
	thread3.Logon(reqStatus3);
	thread1.Resume(); // Start the threads
	thread2.Resume();
	thread3.Resume();
	User::WaitForRequest(reqStatus1); // Avoid using User::WaitForAnyRequest()
	User::WaitForRequest(reqStatus2); 
	User::WaitForRequest(reqStatus3); 

	TBool req1Complete(EFalse);
	TBool req2Complete(EFalse);
	TBool req3Complete(EFalse);

	while(req1Complete==EFalse || req2Complete==EFalse || req3Complete==EFalse)
		{
		if (reqStatus1 != KRequestPending && req1Complete==EFalse) // Thread1 request completed
			{
			TESTCHECK(reqStatus1.Int(), KErrNone);
			req1Complete=ETrue;
			}

		if (reqStatus2 != KRequestPending && req2Complete==EFalse) // Thread2 request rejected
			{
			TESTCHECK(reqStatus2.Int(), KErrNone);
			req2Complete=ETrue;
			}

		if (reqStatus3 != KRequestPending && req3Complete==EFalse) // Thread3 request rejected
			{
			TESTCHECK(reqStatus3.Int(), KErrNone);
			req3Complete=ETrue;
			}

		}

	thread1.Close();
	thread2.Close();
	thread3.Close();

	return TestStepResult();
	}


/** 
 * Send Write request to the Phonebook server.
 */
TInt CPhbkIntegrationStressTestBase::MultipleWriteThread(TAny* /*aAny*/)
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAP_IGNORE(MultipleWriteThreadL());
	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}

/** 
 *
 */
void CPhbkIntegrationStressTestBase::MultipleWriteThreadL()
	{
	CPhbkIntegrationTestUtility* access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->AddContactL(KTestName, KTestNumber, KUidIccGlobalAdnPhonebook);
	access->AddContactL(KTestName, KTestNumber, KUidIccGlobalSdnPhonebook);
	access->AddContactL(KTestName, KTestNumber, KUidIccGlobalLndPhonebook);
	access->AddContactL(KTestName, KTestNumber, KUidUsimAppAdnPhonebook, KTestEMail);
	access->AddContactL(KTestName, KTestNumber, KUidIccGlobalFdnPhonebook);

	CleanupStack::PopAndDestroy(access);
	}

/** 
 * Factory construction method.
 * @return Pointer to CPhbkRequestDeleteSimultaneouslyTest object
 */
CPhbkRequestDeleteSimultaneouslyTest* CPhbkRequestDeleteSimultaneouslyTest::NewL()
	{
	CPhbkRequestDeleteSimultaneouslyTest* self = new(ELeave) CPhbkRequestDeleteSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkRequestDeleteSimultaneouslyTest::CPhbkRequestDeleteSimultaneouslyTest()
	{
	SetTestStepName(_L("RequestDeleteSimultaneouslyTest"));
	}

/** 
 * Create three threads which simultaneously attempt to request Delete from
 * the Phonebook Server.
 */
enum TVerdict CPhbkRequestDeleteSimultaneouslyTest::doTestStepL()
	{
	RThread thread1;
	RThread thread2;
	RThread thread3;
	// Create the threads
	User::LeaveIfError(thread1.Create(_L("Thread10"), MultipleDeleteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread2.Create(_L("Thread11"), MultipleDeleteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));
	User::LeaveIfError(thread3.Create(_L("Thread12"), MultipleDeleteThread, KDefaultStackSize, 0x2000, 0x20000, NULL));

	TRequestStatus reqStatus1;
	TRequestStatus reqStatus2;
	TRequestStatus reqStatus3;

	thread1.Logon(reqStatus1); // Notify when the thread dies
	thread2.Logon(reqStatus2);
	thread3.Logon(reqStatus3);
	thread1.Resume(); // Start the thread
	thread2.Resume();
	thread3.Resume();
	User::WaitForRequest(reqStatus1); // Avoid using User::WaitForAnyRequest()
	User::WaitForRequest(reqStatus2); 
	User::WaitForRequest(reqStatus3);
		
	TBool req1Complete(EFalse);
	TBool req2Complete(EFalse);
	TBool req3Complete(EFalse);

	while(req1Complete==EFalse || req2Complete==EFalse || req3Complete==EFalse)
		{
		if (reqStatus1 != KRequestPending && req1Complete==EFalse) // Thread1 request completed
			{
			TESTCHECK(reqStatus1.Int(), KErrNone);
			req1Complete=ETrue;
			}

		if (reqStatus2 != KRequestPending && req2Complete==EFalse) // Thread2 request rejected
			{
			TESTCHECK(reqStatus2.Int(), KErrNone);
			req2Complete=ETrue;
			}

		if (reqStatus3 != KRequestPending && req3Complete==EFalse) // Thread3 request rejected
			{
			TESTCHECK(reqStatus3.Int(), KErrNone);
			req3Complete=ETrue;
			}
		}

	//User::After(5000000);
	thread1.Close();
	thread2.Close();
	thread3.Close();

	return TestStepResult();
	}

/** 
 * Send Delete request to the Phonebook server.
 */
TInt CPhbkIntegrationStressTestBase::MultipleDeleteThread(TAny* /*aAny*/)
	{
	CActiveScheduler* scheduler = new CActiveScheduler;
	if(!scheduler)
		return KErrNoMemory;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAP_IGNORE(MultipleDeleteThreadL());
	delete cleanup;	
	delete CActiveScheduler::Current();
	return KErrNone;
	}

/** 
 * Multiple delete thread
 */
void CPhbkIntegrationStressTestBase::MultipleDeleteThreadL()
	{
	CPhbkIntegrationTestUtility* access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->DeleteContactL(KUidIccGlobalAdnPhonebook, 2);
	access->DeleteContactL(KUidIccGlobalSdnPhonebook, 2);
	access->DeleteContactL(KUidIccGlobalLndPhonebook, 2);
	access->DeleteContactL(KUidUsimAppAdnPhonebook, 2);
	access->DeleteContactL(KUidIccGlobalFdnPhonebook, 2);

	CleanupStack::PopAndDestroy(access);
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkRequestSyncAndReadSimultaneouslyTest object
 */
CPhbkRequestSyncAndReadSimultaneouslyTest* CPhbkRequestSyncAndReadSimultaneouslyTest::NewL()
	{
	CPhbkRequestSyncAndReadSimultaneouslyTest* self = new(ELeave) CPhbkRequestSyncAndReadSimultaneouslyTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkRequestSyncAndReadSimultaneouslyTest::CPhbkRequestSyncAndReadSimultaneouslyTest()
	{
	SetTestStepName(_L("RequestSyncAndReadSimultaneouslyTest"));
	}

/** 
 * Request a synch and read simultaneously
 */
enum TVerdict CPhbkRequestSyncAndReadSimultaneouslyTest::doTestStepL()
	{
	SetSimTsyTestNumberL(9);

	// Ensure manual sync only
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalSdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalLndPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidUsimAppAdnPhonebook));
	User::LeaveIfError(iSession.SetSyncMode(RPhoneBookSession::EManual, KUidIccGlobalFdnPhonebook));

	TRequestStatus status;
	// For each phonebook start the sync and attempt to read

	// Synchronise Global ADN
	iSession.DoSynchronisation(status);
	User::After(6000000);
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// const CContactIdArray* sortedItems = iDb->SortedItemsL();
	
	//Read Global ADN
	CPhbkIntegrationTestUtility* access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidIccGlobalAdnPhonebook, KTest9ICCSlotsADN);
	CleanupStack::PopAndDestroy(access);
	
	//wait for end of sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Synchronise Global SDN
	iSession.DoSynchronisation(status, KUidIccGlobalSdnPhonebook);
	User::After(6000000);
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// sortedItems = iDb->SortedItemsL();
	
	//Read Global SDN
	access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidIccGlobalSdnPhonebook, KTest9ICCSlotsSDN);
	CleanupStack::PopAndDestroy(access);
	
	//wait for end of sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Synchronise Global LND
	iSession.DoSynchronisation(status, KUidIccGlobalLndPhonebook);
	User::After(6000000);
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// sortedItems = iDb->SortedItemsL();
	
	//Read Global LND
	access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidIccGlobalLndPhonebook, KTest9ICCSlotsLND);
	CleanupStack::PopAndDestroy(access);
	
	//wait for end of sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Synchronise Usim App
	iSession.DoSynchronisation(status, KUidUsimAppAdnPhonebook);
	User::After(6000000);
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// sortedItems = iDb->SortedItemsL();
	
	//Read Usim App
	access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidUsimAppAdnPhonebook, KTest9ICCSlotsUsim);
	CleanupStack::PopAndDestroy(access);
	
	//wait for end of sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	// Synchronise Global FDN
	iSession.DoSynchronisation(status, KUidIccGlobalFdnPhonebook);
	User::After(6000000);
	iDb->SetDbViewContactType(KUidContactICCEntry);
	// sortedItems = iDb->SortedItemsL();
	
	//Read Global FDN
	access = CPhbkIntegrationTestUtility::NewL();
	CleanupStack::PushL(access);
	access->ReadContactL(KUidIccGlobalFdnPhonebook, KTest9ICCSlotsFDN);
	CleanupStack::PopAndDestroy(access);
	
	//wait for end of sync
	User::WaitForRequest(status);
	TESTCHECKL(status.Int(), KErrNone);

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkCheckServerThreadPriorityTest object
 */
CPhbkCheckServerThreadPriorityTest* CPhbkCheckServerThreadPriorityTest::NewL()
	{
	CPhbkCheckServerThreadPriorityTest* self = new(ELeave) CPhbkCheckServerThreadPriorityTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkCheckServerThreadPriorityTest::CPhbkCheckServerThreadPriorityTest()
	{
	SetTestStepName(_L("CheckServerThreadPriorityTest"));
	}

/** 
 * Confirm server thread priority
 */
enum TVerdict CPhbkCheckServerThreadPriorityTest::doTestStepL()
	{
	// Connect to the Phonebook Server
	RPhoneBookSession session;
	User::LeaveIfError(session.Connect()); 

	RThread phoneBookServerThread;
	_LIT(KThreadToFind,"*PhBkSyncServer*"); // PHBKSYNC_SVR_NAME
	TFindThread find(KThreadToFind);
	TFullName findName;
	TInt findErr = find.Next(findName);
	TESTCHECKL(findErr, KErrNone);
	TInt ret = phoneBookServerThread.Open(findName);

	// Check the server thread priority level
	TESTCHECKL(ret, KErrNone);
	TThreadPriority serverPriority(phoneBookServerThread.Priority());
	TESTCHECK(serverPriority, EPriorityNormal); 

	phoneBookServerThread.Close();
	session.Close();

	return TestStepResult();
	}


/** 
 * Factory construction method.
 * @return Pointer to CPhbkServerLostFileRecoveryTest object
 */
CPhbkServerLostFileRecoveryTest* CPhbkServerLostFileRecoveryTest::NewL()
	{
	CPhbkServerLostFileRecoveryTest* self = new(ELeave) CPhbkServerLostFileRecoveryTest();
	return self;
	}

/**
 *  Default constructor.  Each test step initialises it's own name.
 */
CPhbkServerLostFileRecoveryTest::CPhbkServerLostFileRecoveryTest()
	{
	SetTestStepName(_L("ServerLostFileRecoveryTest"));
	}

/** 
 * Test server recovery after losing config files
 */
enum TVerdict CPhbkServerLostFileRecoveryTest::doTestStepL()
	{
	delete iDb;
	iDb=NULL;
	User::After(1000000); //1 second wait for contact to shut down
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	
	// ***********************************************
	// First, test situation where there are no config
	// files at all.
	// ***********************************************

	// Delete all INI files and the contact database
	TRAPD(ignore,CContactDatabase::DeleteDefaultFileL());	
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);

	fsSession.Delete(KContactsModelIniFile); //ignore return value
	fsSession.Delete(KPhbkSyncIniFile); //ignore return value

	// Verify that server starts okay
	ConfigurePhbkSyncToFullL();
	TInt err = iSession.Connect();
	TESTCHECKL(err, KErrNone);

	// Perform synchronisation
	TRAP(err, DoSyncL());
	TESTCHECKL(err, KErrNone);

	// Verify that contact database is available
	TRAP(err, iDb = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);
	User::After(1000000); //1 second wait

	// Get the groupId and template ID for each phonebook
	TContactItemId groupIdADN(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdADN, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKCONDITIONL(groupIdADN != KNullContactId);

	TContactItemId groupIdSDN(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdSDN, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKCONDITIONL(groupIdSDN != KNullContactId);

	TContactItemId groupIdLND(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdLND, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKCONDITIONL(groupIdLND != KNullContactId);

	TContactItemId groupIdUsim(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdUsim, 
						RPhoneBookSession::ESyncGroupId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKCONDITIONL(groupIdUsim != KNullContactId);

	TContactItemId tempIdADN(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdADN, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKCONDITIONL(tempIdADN != KNullContactId);

	TContactItemId tempIdSDN(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdSDN, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKCONDITIONL(tempIdSDN != KNullContactId);

	TContactItemId tempIdLND(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdLND, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKCONDITIONL(tempIdLND != KNullContactId);

	TContactItemId tempIdUsim(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdUsim, 
						RPhoneBookSession::ESyncTemplateId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKCONDITIONL(tempIdUsim != KNullContactId);

	// shutdown existing server session and close database
	delete iDb;
	iDb=NULL;
	User::After(1000000);
	iSession.Close();
	ConfigurePhbkSyncToIdleL();
	
	// ************************************************
	// Now, remove phbksync INI file only and test that
	// existing group and template ID's in the contact
	// database are adopted as required.
	// ************************************************

	// Now remove phbksync INI file only
	fsSession.Delete(KPhbkSyncIniFile);

	// Verify that server starts okay
	ConfigurePhbkSyncToFullL();
	err = iSession.Connect();
	TESTCHECKL(err, KErrNone);

	// Perform synchronisation
	TRAP(err, DoSyncL());
	TESTCHECKL(err, KErrNone);

	// Verify that contact database is available
	TRAP(err, iDb = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);
	User::After(1000000); //1 second wait

	// Check the groupId and template ID for each phonebook is the same as before
	TContactItemId groupIdADN2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdADN2, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKL(groupIdADN2, groupIdADN);

	TContactItemId groupIdSDN2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdSDN2, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKL(groupIdSDN2, groupIdSDN);

	TContactItemId groupIdLND2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdLND2, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKL(groupIdLND2, groupIdLND);

	TContactItemId groupIdUsim2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdUsim2, 
						RPhoneBookSession::ESyncGroupId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKL(groupIdUsim2, groupIdUsim);

	TContactItemId tempIdADN2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdADN2, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKL(tempIdADN2, tempIdADN);

	TContactItemId tempIdSDN2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdSDN2, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKL(tempIdSDN2, tempIdSDN);

	TContactItemId tempIdLND2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdLND2, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKL(tempIdLND2, tempIdLND);

	TContactItemId tempIdUsim2(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdUsim2, 
						RPhoneBookSession::ESyncTemplateId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKL(tempIdUsim2, tempIdUsim);

	// shutdown existing server session and close database
	delete iDb;
	iDb=NULL;
	User::After(1000000);
	iSession.Close();
	ConfigurePhbkSyncToIdleL();

	// ************************************************
	// Now, remove contact database only and test that
	// group and template ID's in the phbksync INI file
	// are re-created.  This should result in same IDs
	// as were created when we had no config files.
	// ************************************************

	// Now remove contact database only
	TRAP(ignore,CContactDatabase::DeleteDefaultFileL());	

	// Verify that server starts okay
	ConfigurePhbkSyncToFullL();
	err = iSession.Connect();
	TESTCHECKL(err, KErrNone);

	// Perform synchronisation
	TRAP(err, DoSyncL());
	TESTCHECKL(err, KErrNone);

	// Verify that contact database is available
	TRAP(err, iDb = OpenOrCreateContactDatabaseL());
	TESTCHECKL(err, KErrNone);
	User::After(1000000); //1 second wait

	// Check the groupId and template ID for each phonebook is the same as before
	TContactItemId groupIdADN3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdADN3, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKL(groupIdADN3, groupIdADN);

	TContactItemId groupIdSDN3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdSDN3, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKL(groupIdSDN3, groupIdSDN);

	TContactItemId groupIdLND3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdLND3, 
						RPhoneBookSession::ESyncGroupId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKL(groupIdLND3, groupIdLND);

	TContactItemId groupIdUsim3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(groupIdUsim3, 
						RPhoneBookSession::ESyncGroupId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKL(groupIdUsim3, groupIdUsim);

	TContactItemId tempIdADN3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdADN3, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalAdnPhonebook));
	TESTCHECKL(tempIdADN3, tempIdADN);

	TContactItemId tempIdSDN3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdSDN3, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalSdnPhonebook));
	TESTCHECKL(tempIdSDN3, tempIdSDN);

	TContactItemId tempIdLND3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdLND3, 
						RPhoneBookSession::ESyncTemplateId,
						KUidIccGlobalLndPhonebook));
	TESTCHECKL(tempIdLND3, tempIdLND);

	TContactItemId tempIdUsim3(KNullContactId);
	TRAP(err, iSession.GetPhoneBookId(tempIdUsim3, 
						RPhoneBookSession::ESyncTemplateId,
						KUidUsimAppAdnPhonebook));
	TESTCHECKL(tempIdUsim3, tempIdUsim);
	
	CleanupStack::PopAndDestroy(&fsSession); 
	return TestStepResult();
	}


/** 
 *  Factory construction method.
 *  @return Pointer to CPhbkIntegrationBackupTest object.
 */
CPhbkIntegrationBackupTest* CPhbkIntegrationBackupTest::NewL()
	{
	CPhbkIntegrationBackupTest* self = new(ELeave) CPhbkIntegrationBackupTest();
	return self;
	}

/**
 *  Default constructor.
 */
CPhbkIntegrationBackupTest::CPhbkIntegrationBackupTest()
	{
	SetTestStepName(_L("IntegrationBackupTest"));
	}

/** 
 *  Destructor.
 */
CPhbkIntegrationBackupTest::~CPhbkIntegrationBackupTest()
	{
	// NOP
	}

/** 
 *  Simulate a backup and restore of the Contacts DB.  This will require
 *  the PhBkSync server to unconfigure and close its connection to the
 *  Contacts database.
 */
enum TVerdict CPhbkIntegrationBackupTest::doTestStepL()
	{
	//
	// Setup the SIM.TSY configuration and then close TE_CntSync's handle
	// to the Contacts DB as it'll interfer with this test.  Future tests
	// will re-open it automatically.
	//
	SetSyncModeL(RPhoneBookSession::EManual);
	CheckSyncModeL(RPhoneBookSession::EManual);
	SetSimTsyTestNumberL(0);
	DoSyncL();
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	delete iDb;
	iDb = NULL;

	//
	// Open the file server...
	//
	RFs fsSession;
	User::LeaveIfError(fsSession.Connect());
	CleanupClosePushL(fsSession);
	CFileMan*  fileManager = CFileMan::NewL(fsSession);
	CleanupStack::PushL(fileManager);

	//
	// Try and copy the Contacts DB file while it is in use...
	//
	TInt  result, resultSqlite;
  
	result = fileManager->Copy(KContactsDBFile, KContactsDBFileBackup);
	resultSqlite = fileManager->Copy(KContactsSqliteDBFile, KContactsSqliteDBFileBackup);
	TESTCHECKCONDITION((result == KErrInUse) || (resultSqlite == KErrInUse));
 
	result = fileManager->Copy(KPhbkSyncIniFile, KPhbkSyncIniFileBackup);
	TESTCHECK(result, KErrNone);

	//
	// SQLite uses a different filename. This is stored here if in use.
	//
    const TDesC&  databaseName = (resultSqlite == KErrInUse) ? KContactsSqliteDBFile() : KContactsDBFile(); 
    const TDesC&  databaseBackupName = (resultSqlite == KErrInUse) ? KContactsSqliteDBFileBackup() : KContactsDBFileBackup(); 
    	
	//
	// Notify the PhBkSync server that a backup is about to take place and
	// that the server should release the Contacts DB...
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURBackupFull, EBackupBase);

	RPhoneBookSession::TPhonebookSyncMode  dummySyncMode;

	result = KErrNone;
	while (result == KErrNone)
		{
		result = iSession.GetSyncMode(dummySyncMode);
		if (result == KErrNone)
			{
			User::After(1);
			}
		}
	TESTCHECK(result, KErrNotReady);

	//
	// Now copy the Contacts DB and PhBkSync INI files...
	//
	result = fileManager->Copy(databaseName, databaseBackupName);
	TESTCHECK(result, KErrNone);
	result = fileManager->Copy(KPhbkSyncIniFile, KPhbkSyncIniFileBackup);
	TESTCHECK(result, KErrNone);
	
	//
	// Check that a sync notification cannot be posted in this state...
	//
	TRequestStatus  status;
	
	iSession.NotifyPhBkCacheStateChange(status, KUidIccGlobalAdnPhonebook);
	User::WaitForRequest(status);
	TESTCHECK(status.Int(), KErrNotReady);

	//
	// Notify the PhBkSync server that the backup is over.  PhBkSync will
	// restart and continue as before...
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURNormal, ENoBackup);

	result = KErrNotReady;
	while (result == KErrNotReady)
		{
		result = iSession.GetSyncMode(dummySyncMode);
		if (result == KErrNotReady)
			{
			User::After(1);
			}
		}
	TESTCHECK(result, KErrNone);

	//
	// Check PhBkSync server is still running okay...
	//
	CheckCacheStateL(RPhoneBookSession::EUnsynchronised);
	DoSyncL();
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	//
	// Change the settings in the INI file to test that the restore works...
	//
	SetSyncModeL(RPhoneBookSession::EAutoSameIcc);
	CheckSyncModeL(RPhoneBookSession::EAutoSameIcc);

	//
	// Notify the PhBkSync server that a restore is about to take place and
	// that the server should release the Contacts DB...
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURRestorePartial, EBackupIncrement);

	result = KErrNone;
	while (result == KErrNone)
		{
		result = iSession.GetSyncMode(dummySyncMode);
		if (result == KErrNone)
			{
			User::After(1);
			}
		}
	TESTCHECK(result, KErrNotReady);

	//
	// Delete the Contacts DB and PhBkSync INI files and rename the backup files
	// to the original names.
	//
	result = fsSession.Delete(databaseName);
	TESTCHECK(result, KErrNone);
	result = fsSession.Delete(KPhbkSyncIniFile);
	TESTCHECK(result, KErrNone);
	result = fsSession.Rename(databaseBackupName, databaseName);
	TESTCHECK(result, KErrNone);
	result = fsSession.Rename(KPhbkSyncIniFileBackup, KPhbkSyncIniFile);
	TESTCHECK(result, KErrNone);

	//
	// Notify the PhBkSync server that the restore is over.  PhBkSync will
	// restart and continue as it did before the backup...
	//
	iSecureBackupEngine->SetBURModeL(TDriveList(_L8("C")),
 									 EBURNormal, ENoBackup);

	result = KErrNotReady;
	while (result == KErrNotReady)
		{
		result = iSession.GetSyncMode(dummySyncMode);
		if (result == KErrNotReady)
			{
			User::After(1);
			}
		}
	TESTCHECK(result, KErrNone);

	//
	// Check PhBkSync server is running as before the backup...
	//
	CheckSyncModeL(RPhoneBookSession::EManual);

	CheckCacheStateL(RPhoneBookSession::EUnsynchronised);
	DoSyncL();
	CheckCacheStateL(RPhoneBookSession::ECacheValid);

	//
	// Clean up and finish the test...
	//
	CleanupStack::PopAndDestroy(fileManager); 
	CleanupStack::PopAndDestroy(&fsSession); 

	return TestStepResult();
	} // CPhbkIntegrationBackupTest::doTestStepL
