// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "testcalinterimapibackuprestore.h"

_LIT(KBackupRestoreFile, "C:TestFile_BackupRestore");
// The location of the calendar file used in the tests
_LIT(KBackupRestoreFileSrc, "C:\\private\\10003a5b\\TestFile_BackupRestore");
// The path where we will try to copy the calendar file
_LIT(KBackupRestoreFileTgt, "\\TestFile_BackupRestore");

TFileName BackupRestoreFileTgt()
	{
	TFileName fileName;
#ifdef __WINS__
	fileName.Append(_L("c:"));
#else
	TFileName fullName = RProcess().FileName();
	TParsePtrC parse(fullName);
	fileName.Append(parse.Drive());
#endif
	fileName.Append(KBackupRestoreFileTgt);
	
	return fileName;
	}

CBackupRestoreTestStep::CBackupRestoreTestStep()
	{
	SetTestStepName( KTestCalInterimApiBackupRestore );
	}

CBackupRestoreTestStep::~CBackupRestoreTestStep()
	{
	iPIMTestServer.Close();
	delete iCalSession;
	delete iScheduler;
	}

TVerdict CBackupRestoreTestStep::doTestStepPreambleL()
	{
	TVerdict ret = CTestStep::doTestStepPreambleL();

	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );
   
   	iCalSession = CCalSession::NewL();
   	// This file shouldn't exist yet but it could have been left over from
   	// a previous test execution so try to delete it to guarantee we are
   	// in a clean known state
   	TRAP_IGNORE(iCalSession->DeleteCalFileL(KBackupRestoreFile));
   	// Create calendar file that will be used for these tests
	iCalSession->CreateCalFileL(KBackupRestoreFile);
	
	User::LeaveIfError(iPIMTestServer.Connect());
	
	// The list of drives for the bakup engine
	const char driveC = 'C';
    iDriveList.Append(driveC);
	
	return ret;
	}	

TVerdict CBackupRestoreTestStep::doTestStepL()
    {
  	TestBackupWhenFileIsOpenL();
  	TestBackupWhenFileCloseTimerRunningL();
    TestRestoreWhenFileIsOpenL();
    TestRestoreWhenFileCloseTimerRunningL();
       
    return TestStepResult();
    }

TVerdict CBackupRestoreTestStep::doTestStepPostambleL()
	{
	TRAP_IGNORE(iPIMTestServer.DeleteFileL(BackupRestoreFileTgt()));
	
	iPIMTestServer.Close();

	iCalSession->DeleteCalFileL(KBackupRestoreFile);
	delete iCalSession;
	iCalSession = 0;

	delete iScheduler;
	iScheduler = 0;
	
	return TestStepResult();
	}

/**
Checks that when a calendar file is open it gets closed immediately when the clients closes it
and can be accessed by the backup and restore engine.

During this test a client connects to the Calendar Server and opens a calendar file. A backup start
notification is sent to the Calendar Server. The client disconnects. This causes the calendar file to
be closed. What we are checking is that in these conditions the file will be closed immediately and
not afer 5 seconds.
*/
void CBackupRestoreTestStep::TestBackupWhenFileIsOpenL()
	{
	_LIT(KTestStart, "TestBackupWhenFileIsOpenL Start");
	INFO_PRINTF1(KTestStart);
	
	TestBackupOrRestoreWhenFileIsOpenL(conn::EBURBackupPartial);
	
	_LIT(KTestEnd, "TestBackupWhenFileIsOpenL End");
	INFO_PRINTF1(KTestEnd);
	}
	
/**
Checks that when a calendar file is scheduled for close it gets closed immediately when a backup notification
is received and can be accessed by the backup and restore engine.

During this test a client connects to the Calendar Server, opens a calendar file and then closes it.
Because the file is only closed after 5 seconds we can't access it. A backup start notification is sent
to the Calendar Server. This causes the calendar file that was scheduled for close to be closed
immediately.
*/
void CBackupRestoreTestStep::TestBackupWhenFileCloseTimerRunningL()
	{
	_LIT(KTestStart, "TestBackupWhenFileCloseTimerRunningL Start");
	INFO_PRINTF1(KTestStart);
	
	TestBackupOrRestoreWhenFileCloseTimerRunningL(conn::EBURBackupPartial);
	
	_LIT(KTestEnd, "TestBackupWhenFileCloseTimerRunningL End");
	INFO_PRINTF1(KTestEnd);
	}

/**
Checks that when a calendar file is open it gets closed immediately when the clients closes it
and can be accessed by the backup and restore engine.

During this test a client connects to the Calendar Server and opens a calendar file. A restore start
notification is sent to the Calendar Server. The client disconnects. This causes the calendar file to
be closed. What we are checking is that in these conditions the file will be closed immediately and
not afer 5 seconds.
*/
void CBackupRestoreTestStep::TestRestoreWhenFileIsOpenL()
	{
	_LIT(KTestStart, "TestRestoreWhenFileIsOpenL Start");
	INFO_PRINTF1(KTestStart);
	
	TestBackupOrRestoreWhenFileIsOpenL(conn::EBURRestorePartial);
	
	_LIT(KTestEnd, "TestRestoreWhenFileIsOpenL End");
	INFO_PRINTF1(KTestEnd);
	}

/**
Checks that when a calendar file is scheduled for close it gets closed immediately when a restore notification
is received and can be accessed by the backup and restore engine.

During this test a client connects to the Calendar Server, opens a calendar file and then closes it.
Because the file is only closed after 5 seconds we can't access it. A restore start notification is sent
to the Calendar Server. This causes the calendar file that was scheduled for close to be closed
immediately.
*/
void CBackupRestoreTestStep::TestRestoreWhenFileCloseTimerRunningL()
	{
	_LIT(KTestStart, "TestRestoreWhenFileCloseTimerRunningL Start");
	INFO_PRINTF1(KTestStart);
	
	TestBackupOrRestoreWhenFileCloseTimerRunningL(conn::EBURRestorePartial);
	
	_LIT(KTestEnd, "TestRestoreWhenFileCloseTimerRunningL End");
	INFO_PRINTF1(KTestEnd);
	}
	
void CBackupRestoreTestStep::TestBackupOrRestoreWhenFileIsOpenL(conn::TBURPartType aType)
	{
	TRAP_IGNORE(iPIMTestServer.DeleteFileL(BackupRestoreFileTgt()));
	
	// Open the file
	iCalSession->OpenL(KBackupRestoreFile);
	
	// Try to access the file. This won't work because the client is still connected
	// This is to make sure that we are in the expected state before we proceed
	// with the actual test.
	TRAPD(err, iPIMTestServer.CopyFileL(KBackupRestoreFileSrc, BackupRestoreFileTgt()));
	TEST(err == KErrInUse);
	
	// Start backup or restore
    conn::CSBEClient* backupClient = conn::CSBEClient::NewL();
    CleanupStack::PushL(backupClient);
    backupClient->SetBURModeL(iDriveList, aType, conn::EBackupBase);
    
    SimulateClientDisconnections();
    
    // Try to access the file, should be fine now since the client has disconnected
	TRAP(err, iPIMTestServer.CopyFileL(KBackupRestoreFileSrc, BackupRestoreFileTgt()));
	TEST(err == KErrNone);
	
	backupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
    CleanupStack::PopAndDestroy(backupClient);
	
	TRAP_IGNORE(iPIMTestServer.DeleteFileL(BackupRestoreFileTgt()));
	
	// Re-open the calendar session
	iCalSession = CCalSession::NewL();
	}
	
void CBackupRestoreTestStep::TestBackupOrRestoreWhenFileCloseTimerRunningL(conn::TBURPartType aType)
	{
	TRAP_IGNORE(iPIMTestServer.DeleteFileL(BackupRestoreFileTgt()));
	
	// Open the file
	iCalSession->OpenL(KBackupRestoreFile);
	
	// Initialize Backup. This takes a bit of time so do it before
	// we close the session so that it doesn't add a delay.
    conn::CSBEClient* backupClient = conn::CSBEClient::NewL();
    CleanupStack::PushL(backupClient);
       	
	SimulateClientDisconnections();
	
	// Try to access the file. This won't work because we have disconnected the clients
	// but the file is only closed after 5 seconds so it is still open.
	TRAPD(err, iPIMTestServer.CopyFileL(KBackupRestoreFileSrc, BackupRestoreFileTgt()));
	TEST(err == KErrInUse);
	
	// Start backup or restore, on reception of the notification the server
	// will close the files scheduled for close and so we should be able 
	// to access the file
	backupClient->SetBURModeL(iDriveList, aType, conn::EBackupBase);
	
	// Wait a tiny bit for the server to process the notification
	const TInt KDelayForNotificationProcessing = 100000;
 	User::After(KDelayForNotificationProcessing);
 	
 	// Try to access files, should be fine now
	TRAP(err, iPIMTestServer.CopyFileL(KBackupRestoreFileSrc, BackupRestoreFileTgt()));
	TEST(err == KErrNone);
   
	backupClient->SetBURModeL(iDriveList, conn::EBURNormal, conn::ENoBackup);
    CleanupStack::PopAndDestroy(backupClient);
	
	TRAP_IGNORE(iPIMTestServer.DeleteFileL(BackupRestoreFileTgt()));
	
	// Re-open the calendar session
	iCalSession = CCalSession::NewL();
	}
		
void CBackupRestoreTestStep::SimulateClientDisconnections()
	{
	// Simulate clients disconnecting by manually closing iCalSession
    delete iCalSession;
    iCalSession = NULL;
    
    // Wait a tiny bit for the server to process the disconnection and schedule 
    // the file close
    const TInt KServerCloseDelay = 100000;
 	User::After(KServerCloseDelay);
	}
