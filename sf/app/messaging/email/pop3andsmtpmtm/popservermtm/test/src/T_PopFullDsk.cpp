// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_PopFullDsk
// Component: POPS
// Owner: NB
// Brief description of test harness:
// Tests copying messages in POPS when there is a Full Disk
// Detailed description of test harness:
// Implements the following Tests:
// 1 - Test Copy To Local
// 2 - Test Copy Within Service
// 3 - Test Copying from a Local folder to another Local Folder
// 4 - Test Offline Copy
// Input files required to run test harness:
// The following files should be copied to <drive>:\msgtest\pops\
// T_PopFullDsk_CopyAll.scr
// T_PopFullDsk_Populate.scr
// T_PopFullDsk_OfflineCopy.scr
// T_PopFullDsk_Msg.eml
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_POPSFullDisk.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\pops\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// (1) You need to create a small partitioned drive (ie less than 20GB) to 
// run the harness on (eg a Compact Flash Card).
// (2) For running on the emulator, alter the epoc32\data\epoc.ini 
// file so that it uses this drive (eg    _EPOC_DRIVE_C E:\   
// if E:\ is your small partioned drive ).
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_PopFullDsk.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_PopFullDsk.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Copy \epoc32\wins\c\msgtest\pops\*.* to
// <DRIVE>:\msgtest\pops\
// Note that this drive should be a CF card as the test files are large
// 5. Run T_PopFullDsk.exe on the other platform with the CF card
// Generic POPS Test Harness
// 
//

#include "emailtestutils.h"
#include "scripttestutils.h"
#include <miutlog.h>
#include <miutmsg.h>
#include <txtrich.h>
#include <popsmtm.h>
#include "T_mtmcmd.h"
#include <cemailaccounts.h>


// NOTE:  This is the amout of available Disk Space that we are creating to test
// the Low Disk Space condition.  It definitely has to be less than KMinimumDiskSpaceForSync
// and ideally very small, as more space is made available as the operations are performed.
const TInt KTestDiskSpace = 500;

const TInt KFillDiskTooBigFreeSize		=0x00080000;	// 1/2 Gb in kb
const TInt KFillDiskOneKByte			=0x400;
const TInt KFillDiskMaxBlockSize		=1024;

_LIT(KSubFolder,  "LocalFolder");
_LIT(KSessionPath, "c:\\");
_LIT(KTestMsgPath, "c:\\msgtest\\pops\\T_PopFullDsk_Msg.eml");  
_LIT(KMsvTestFillDiskFile,	"c:\\Dummy fill file");

_LIT(K_POPS_COMPONENT_NAME, "POPS");

_LIT(K_SCRIPT_COPY_ALL,		"T_PopFullDsk_CopyAll.scr");
_LIT(K_SCRIPT_POPULATE_ALL, "T_PopFullDsk_Populate.scr");
_LIT(K_SCRIPT_OFFLINE_COPY, "T_PopFullDsk_OfflineCopy.scr");

_LIT(KTest1Pass, "Test 1 OK, Copy To Local Failed correctly with KErrDiskFull");
_LIT(KTest2Pass, "Test 2 OK, Copy Within Service Failed correctly with KErrDiskFull");
_LIT(KTest3Pass, "Test 3 OK, Local Copy Failed correctly with KErrDiskFull");

_LIT(KTest1Failed, "Test 1 Failed, Copy To Local should NOT have completed");
_LIT(KTest2Failed, "Test 2 Failed, CopyWithin Service should NOT have completed");
_LIT(KTest3Failed, "Test 3 Failed, Local Copy should NOT have completed");

const TInt KMsvTestBackgroundDeleteInitialWait	=1000000;

_LIT(KImcmTest, "T_POPSFullDisk - Test Copy when Disk Full");
RTest test(KImcmTest);


class CDelayTimer : public CTimer
	{
public:
	CDelayTimer() : CTimer(EPriorityHigh)
		{}
	void ConstructL()
		{ CTimer::ConstructL(); CActiveScheduler::Add(this); }
	void RunL(void)
		{ CActiveScheduler::Stop(); }
	void Delay(TTimeIntervalMicroSeconds32 anInterval)
		{ CTimer::After(anInterval); CActiveScheduler::Start(); }
	};



//-------------------------------------
//		CPopsTestHarness
//-------------------------------------
class CPopsTestHarness : public CBase
	{
public:
	static CPopsTestHarness* NewLC();
	~CPopsTestHarness();
		
	void RunTestsL();
	

private:
	CPopsTestHarness();
	void ConstructL();

	void InitL();
	void ResetL();
	void StartTestL(CMtmTestHarness& aTestCommands);
	void CopyScriptFileL(const TDesC& aScriptFile);
	void EndTestL();
	void EndTestHarnessL();

	// Fns to fill the disk
	void FillDiskL();
	TInt CheckDiskSpace(TInt64& aFreeSpace);
	void TestAvailableSpaceOnDriveL();

	// The Testing Functions
	void TestCopyToLocalL(const TDesC& aPassText, const TDesC& aFailText);
	void TestCopyWithinServiceL(const TDesC& aPassText, const TDesC& aFailText);
	void TestLocalCopyL(const TDesC& aPassText, const TDesC& aFailText);
	void TestOfflineCopyL(const TDesC& aPassText, const TDesC& aFailText);
	void DoLocalCopyTestL();
	void CheckIfTestFailedL(TInt aError, CMtmTestHarness& aTestHarness,const TDesC& aPassText, const TDesC& aFailText);
	
	void SetUpTestL(const TDesC& aScriptFile);
	void SetUpPopServiceL();
	void CreateTestHarnessesL();
	void ResetMtmL();

public:
	CMtmTestHarness*	iConnectAndCopyAll;
	CMtmTestHarness*	iConnectAndPopulateAll;
	CMtmTestHarness*	iOfflineCopy;

private:
	TBool				iRemoveMailFolder;
	TInt				iTestError;
	TInt				iTestNumber;
	TInt				iGlobalError;

	TMsvId				iPop3Service;

	CEmailTestUtils*	iTestUtils;
	CScriptTestUtils*	iScriptUtils;
	};


//

CPopsTestHarness::CPopsTestHarness() 
: iGlobalError(KErrNone)
	{
	}

CPopsTestHarness::~CPopsTestHarness()
	{
	delete iTestUtils;

	delete iScriptUtils;
	delete iConnectAndCopyAll;
	delete iConnectAndPopulateAll;
	delete iOfflineCopy;
	}

CPopsTestHarness* CPopsTestHarness::NewLC()
	{
	CPopsTestHarness* self = new (ELeave) CPopsTestHarness();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CPopsTestHarness::ConstructL()
	{
	iTestUtils = CEmailTestUtils::NewL(test);

	InitL();
	}


void CPopsTestHarness::SetUpTestL(const TDesC& aScriptFile)
	{
	// Delete any old dummy fill files 
	iTestUtils->FileSession().Delete(KMsvTestFillDiskFile);

	// Reset & Copy the script file before filling the Disk
	ResetL();
	CopyScriptFileL(aScriptFile); 
	}

void CPopsTestHarness::RunTestsL()
	{
	iTestNumber = 0;
	iRemoveMailFolder = ETrue;
	SetUpPopServiceL();

	// 1 - Test Copy To Local
	TestCopyToLocalL(KTest1Pass, KTest1Failed);

	// 2 - Test Copy Within Service
	TestCopyWithinServiceL(KTest2Pass, KTest2Failed);

	// 3 - Test Copying from a Local folder to another Local Folder
	TestLocalCopyL(KTest3Pass, KTest3Failed);

	// 4 - Test Offline Copy
//	TestOfflineCopyL(KTest4Pass, KTest4Failed);

	// Enf of the Tests	
	EndTestHarnessL();
	}


void CPopsTestHarness::TestCopyToLocalL(const TDesC& aPassText, const TDesC& aFailText)
// ------------------------------------
// Test Copying To Local
// ------------------------------------
	{
	iTestError = 0;
	iTestNumber++;
	SetUpTestL(K_SCRIPT_COPY_ALL);
	FillDiskL();
	TRAPD(testError, StartTestL(*iConnectAndCopyAll));

	// Check if the Test Failed
	CheckIfTestFailedL(testError, *iConnectAndCopyAll, aPassText, aFailText);
	EndTestL();
	}



void CPopsTestHarness::TestCopyWithinServiceL(const TDesC& aPassText, const TDesC& aFailText)
// ------------------------------------
// Test Copy Within Service
// ------------------------------------
	{
	iTestError = 0;
	iTestNumber++;
	SetUpTestL(K_SCRIPT_POPULATE_ALL);
	FillDiskL();

	TRAPD(testError, StartTestL(*iConnectAndPopulateAll));
			
	// Check if the Test Failed
	CheckIfTestFailedL(testError, *iConnectAndPopulateAll, aPassText, aFailText);
	EndTestL();
	}


void CPopsTestHarness::TestLocalCopyL(const TDesC& aPassText, const TDesC& aFailText)
// ------------------------------------------------------------------
// Test Copying from a Local folder to another Local Folder
// ------------------------------------------------------------------
	{
	iTestError = 0;
	iTestNumber++;

	// Reset the harness & run the Test
	iTestUtils->FileSession().Delete(KMsvTestFillDiskFile);
	ResetL();
	iTestUtils->TestStart(iTestNumber);
	TRAPD(error, DoLocalCopyTestL());

		// Delete the fill file, so we can write to the log
	iTestUtils->FileSession().Delete(KMsvTestFillDiskFile);

	if (error == KErrDiskFull)
		{
		// Test OK = We could not Copy, as the disk was too Full
		iTestUtils->WriteComment(aPassText);
		}
	else if (error == KErrNone)
		{
		// ERROR - The Copy completed, even though the Disk was Full
		iTestUtils->WriteComment(aFailText);
		iTestError = KErrGeneral;
		}

	EndTestL();
	}


void CPopsTestHarness::TestOfflineCopyL(const TDesC& aPassText, const TDesC& aFailText)
// ------------------------------------------------------------------
// Test Offline Copy - Populate some Messages & then try to create 
// Offline Ops to copy them locally
// ------------------------------------------------------------------
	{
	iTestError = 0;
	iTestNumber++;

	// Connect & Populate all the messages first
	SetUpTestL(K_SCRIPT_POPULATE_ALL);
	TRAPD(test2Error, StartTestL(*iConnectAndPopulateAll));
	
	// Now reset, but DONT delete the Mail directory. 
	iRemoveMailFolder = EFalse;
	SetUpTestL(K_SCRIPT_OFFLINE_COPY);

	// Fill the Disk & attempt to do an Offline Copy
	FillDiskL();
	TRAPD(error, StartTestL(*iOfflineCopy));
			
	// Check if the Test Failed
	CheckIfTestFailedL(error, *iOfflineCopy, aPassText, aFailText);
	EndTestL();
	}


void CPopsTestHarness::DoLocalCopyTestL()
// Test copying a message locally.  ie create a message in the Drafts folder
// and copy it to a local sub folder
	{
	// Create the Message in the Drafts Folder
	iTestUtils->CreateMessageL(KTestMsgPath, KMsvDraftEntryId, KMsvDraftEntryId);
	CMsvServerEntry* serverEntry = iTestUtils->iServerEntry;
	serverEntry->SetEntry(KMsvDraftEntryId);
	CMsvEntrySelection* newMessageList = new (ELeave)CMsvEntrySelection();
	CleanupStack::PushL(newMessageList);
	serverEntry->GetChildren(*newMessageList);
	TMsvId newMessageId = (*newMessageList)[0];

	// Create  a local folder under the Drafts folder
	TMsvEntry parentEntry = serverEntry->Entry();
	TMsvEntry newFolder;
	newFolder.SetStandardFolder(ETrue);
	newFolder.iDetails.Set(KSubFolder);
	newFolder.iType.iUid=KUidMsvFolderEntryValue;
	newFolder.iMtm=parentEntry.iMtm;
	newFolder.iServiceId = parentEntry.iServiceId;
	serverEntry->CreateEntry(newFolder);

	// Fill the Disk Space and attempt to do the Copy
	FillDiskL();

	// Copy the message into the new LOCAL Sub-Folder
	CTestActive* active = new(ELeave) CTestActive;
	CleanupStack::PushL(active);
	serverEntry->CopyEntryL(newMessageId, newFolder.Id(), active->iStatus);
	active->StartL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); // newMessageList, active
	}


void CPopsTestHarness::CheckIfTestFailedL(TInt aError, CMtmTestHarness& aTestHarness,
										  const TDesC& aPassText, const TDesC& aFailText)
	{
	// Delete the fill file, so we can write to the log
	iTestUtils->FileSession().Delete(KMsvTestFillDiskFile);

	if (aError == KErrDiskFull)
		{
		// Test OK = We could not Copy, as the disk was too Full
		iTestUtils->WriteComment(aPassText);
		}
	else if (aError == KErrNone)
		{
		// The test returned ok, so check if the Mtm Test Harness returned an error
		TInt commandError = aTestHarness.CommandError();
		TInt commandStatus = aTestHarness.iStatus.Int();
		if (commandError == KErrDiskFull || commandStatus == KErrDiskFull)
			{
			// Test OK = We could not Copy as the disk was too Full
			iTestUtils->WriteComment(aPassText);
			}
		else
			{
			// ERROR - The Copy completed, even though the Disk was Full
			iTestUtils->WriteComment(aFailText);
			iTestError = KErrGeneral;
			}
		}
	}



void CPopsTestHarness::TestAvailableSpaceOnDriveL()
//
// Make sure there is enougth disk space to run the Tests
//
	{
	const TInt64 KMaximumAllowedFreeSpaceOnDrive=10*1024*1024; // 10MB

	// Get drive information.
	TVolumeInfo volumeInfo;
	User::LeaveIfError(iTestUtils->FileSession().Volume(volumeInfo, EDriveC));

	// Check that the media is present.
	test(volumeInfo.iDrive.iType!=EMediaNotPresent);

	// Check that there is not too much free space (ie 20Gb!)
	test(volumeInfo.iFree<=KMaximumAllowedFreeSpaceOnDrive);
	}



TInt CPopsTestHarness::CheckDiskSpace(TInt64& aFreeSpace)
//
// Check available disk space.
//
	{
	TVolumeInfo volumeInfo;
	TInt error = iTestUtils->FileSession().Volume(volumeInfo, EDriveC);
	aFreeSpace = error?0:volumeInfo.iFree;
	return(error);
	}


void CPopsTestHarness::FillDiskL()
//
// Fill all available space on C: drive leaving aLeaveFreeByes free.
//
	{
	TInt leaveFreeBytes = KTestDiskSpace; 

	// Check that there is a C-drive and that it is less than 10MB in size.
	TestAvailableSpaceOnDriveL();

	// NOTE: This test harness should be used with a dedicated small partition or 
	//       subst'ed floppy drive as the EPOC C: drive. As a safety precaution, it
	//		 tests the free space on the partition to ensure that it does not attempt
	//       to eat 20 GB of free space and either crash Windows or at least cause a
	//       headache of some sort...

	// Delete any old dummy fill files - if there is one to delete.
	iTestUtils->FileSession().Delete(KMsvTestFillDiskFile);

	// Create the delay timer 
	CDelayTimer* delayTimer = new(ELeave)CDelayTimer;
	CleanupStack::PushL(delayTimer);
	delayTimer->ConstructL();

	// Check that available disk space does not exceed the max free space to fill. 
	// See above...
	TInt64 freeSpace;
	TInt64 freeSpaceLeft;
	TInt error = CheckDiskSpace(freeSpace);
	TInt reclaimActive = 0;
	do
		{
		delayTimer->Delay(KMsvTestBackgroundDeleteInitialWait);
		error = CheckDiskSpace(freeSpaceLeft);
		if (freeSpaceLeft == freeSpace)
			reclaimActive++;
		freeSpace = freeSpaceLeft;
		}
	while (!error && reclaimActive<3);
	User::LeaveIfError(error);

	CleanupStack::PopAndDestroy(); //delayTimer
	
	// Free space in kb.
	freeSpace/=KFillDiskOneKByte;

	// Check free space on C: drive is not too big.
	TInt freeSpaceInKb = I64INT(freeSpace);
	if (freeSpaceInKb>KFillDiskTooBigFreeSize)
		User::Leave(KErrTooBig);

	// Fill disk with large file.
	TBuf8<KFillDiskMaxBlockSize> dummy;
	dummy.SetMax();
	dummy.Fill('?');

	// Open dummy file.
	RFile fillDiskFile;
	User::LeaveIfError(fillDiskFile.Replace(iTestUtils->FileSession(), KMsvTestFillDiskFile, EFileWrite));

	// Establish required dummy file size.
	freeSpaceLeft-=leaveFreeBytes;
	if (freeSpaceLeft<0)
		// Uh oh, we have less space available than what was asked for... 
		User::Leave(KErrDiskFull);

	// Write dummy file.
	while (freeSpaceLeft>0 && error==KErrNone)
		{
		dummy.SetLength(Min(KFillDiskMaxBlockSize,I64INT(freeSpaceLeft)));
		error=fillDiskFile.Write(dummy);
		freeSpaceLeft-=KFillDiskMaxBlockSize;
		}
	fillDiskFile.Close();
	if (error==KErrDiskFull)
		error=KErrNone;
	User::LeaveIfError(error);
	}

void CPopsTestHarness::ResetMtmL()
	{
	// Get server MTM.
	iTestUtils->Reset();
	
	if (iRemoveMailFolder)
		{
		iTestUtils->CleanMessageFolderL();
		}
	else
		{
		iRemoveMailFolder = ETrue;
		}

	iTestUtils->GoServerSideL();
	SetUpPopServiceL();
	iTestUtils->InstantiatePopServerMtmL();
	}

void CPopsTestHarness::ResetL()
	{
	ResetMtmL();
	CreateTestHarnessesL();
	}

void CPopsTestHarness::StartTestL(CMtmTestHarness& aTestCommands)
	{
	iTestError = KErrNone;
	iTestUtils->TestStart(iTestNumber);

	// Run the commands.
	aTestCommands.StartL(_L("T_PopFullDsk - POP Copy Disk Full Test Harness"));
	}

void CPopsTestHarness::CopyScriptFileL(const TDesC& aScriptFile)
	{
	TParse parsedScriptFile;
	parsedScriptFile.Set(aScriptFile,0,0);
	if (!parsedScriptFile.DrivePresent())
		// This isn't a full path name so we need to resolve it
		{
		iTestUtils->ResolveFile(K_POPS_COMPONENT_NAME, aScriptFile, parsedScriptFile);
		}
	iTestUtils->CopyScriptFileL(parsedScriptFile.FullName(), _L("110"));
	}

void CPopsTestHarness::EndTestL()
	{
	if ((iTestError != KErrNone) && (iGlobalError == KErrNone))
		{
		iGlobalError = iTestError;
		}

	iTestUtils->TestFinish(iTestNumber, iTestError);
	}

void CPopsTestHarness::EndTestHarnessL()
	{
	if (iGlobalError == KErrNone)
		{
		iTestUtils->TestHarnessCompleted();
		}
	else
		{
		iTestUtils->TestHarnessFailed(iGlobalError);
		}
	}

void CPopsTestHarness::InitL()
	{
	iTestUtils->CreateAllTestDirectories();
	iTestUtils->FileSession().SetSessionPath(KSessionPath);
	iTestUtils->CleanMessageFolderL();
	iTestUtils->GoServerSideL();
	
	iScriptUtils = CScriptTestUtils::NewLC(*iTestUtils);
	CleanupStack::Pop(); // iScriptUtils
	
	iTestUtils->FileSession().SetSessionPath(KSessionPath);
	}

void CPopsTestHarness::SetUpPopServiceL()
	{
	// Create the POP3 service and set the settings for this test.
	iPop3Service = iTestUtils->CreatePopServiceL();

	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);

	settings->Reset();
	_LIT(KPopServer, "pop3.demon.co.uk");
	settings->SetServerAddressL(KPopServer);
	settings->SetLoginNameL(iTestUtils->MachineName());
	settings->SetPasswordL(iTestUtils->MachineName());
	settings->SetPort(110);
	settings->SetDisconnectedUserMode(ETrue); // eh xxxx, must test this when set to EFalse

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
 	TPopAccount popAccount;
 	accounts->GetPopAccountL(iPop3Service, popAccount);						
 	accounts->SavePopSettingsL(popAccount, *settings);
	CleanupStack::PopAndDestroy(2, settings); // accounts/store, settings

	test.Printf(_L("\nPerforming Tests\n"));
	}


void CPopsTestHarness::CreateTestHarnessesL()
	{
	CMsvEntrySelection* serviceSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(serviceSelection);
	serviceSelection->AppendL(iPop3Service);
	TBuf8<128> parameter;

	//--------------------------------------------
	// Copy To Local
	//--------------------------------------------
	delete iConnectAndCopyAll;
	iConnectAndCopyAll = NULL;
	iConnectAndCopyAll = CMtmTestHarness::NewL(*(iTestUtils->iPopServerMtm), *iTestUtils);
	// Connect
	iConnectAndCopyAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Copy all messages under the service to the local inbox.
	CMtmTestCommand* copyToLocal = CMtmTestCopyToLocal::NewL(*(iTestUtils->iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iConnectAndCopyAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils->iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils->iServerEntry)));
	// Quit
	iConnectAndCopyAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	//--------------------------------------------
	// Copy Within Service
	//--------------------------------------------
	delete iConnectAndPopulateAll;
	iConnectAndPopulateAll = NULL;
	iConnectAndPopulateAll = CMtmTestHarness::NewL(*(iTestUtils->iPopServerMtm), *iTestUtils);
	// Connect
	iConnectAndPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Copy all messages under the service to the local service.
	copyToLocal = CMtmTestCopyWithinService::NewL(*(iTestUtils->iPopServerMtm), 0, iPop3Service);
	iConnectAndPopulateAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils->iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils->iServerEntry)));
	// Quit
	iConnectAndPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));


	//--------------------------------------------
	// Offline Copy
	//--------------------------------------------
	CMsvServerEntry* serverEntry = iTestUtils->iServerEntry;
	serverEntry->SetEntry(iPop3Service);
	CMsvEntrySelection* newMessageList = new (ELeave)CMsvEntrySelection();
	CleanupStack::PushL(newMessageList);
	serverEntry->GetChildren(*newMessageList);
	delete iOfflineCopy;
	iOfflineCopy = NULL;
	iOfflineCopy = CMtmTestHarness::NewL(*(iTestUtils->iPopServerMtm), *iTestUtils);
	// Copy all messages under the service to the local inbox.
	copyToLocal = CMtmTestCopyToLocal::NewL(*(iTestUtils->iPopServerMtm), newMessageList, KMsvGlobalInBoxIndexEntryId);
	iOfflineCopy->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils->iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils->iServerEntry)));
	// Connect
	iOfflineCopy->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Quit
	iOfflineCopy->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils->iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));


	CleanupStack::PopAndDestroy(2); // newMessageList, serviceSelection
	}


//



LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler;
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create the Test Harness & run the Tests
	CPopsTestHarness* popsTestHarness = CPopsTestHarness::NewLC();
	popsTestHarness->RunTestsL();

	CleanupStack::PopAndDestroy(2); // popsTestHarness, scheduler
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_PopFullDsk, POP Copy Disk Full Tests"));
	CTrapCleanup* theCleanup = CTrapCleanup::New();

	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}














