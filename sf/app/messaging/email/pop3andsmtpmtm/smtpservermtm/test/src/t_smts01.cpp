// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "emailtestutils.h"
#include "scripttestutils.h"
#include <iapprefs.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>
using namespace CommsDat;
#include <cdbpreftable.h>
#include <securesocket.h>
#include <securesocket_internal.h>
#include <bacline.h>

#include <cemailaccounts.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif


_LIT(KSmtsTest, "T_SMTS01 - Testing SMTS Server MTM using dial out");
RTest test(KSmtsTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgEmail,
	EArgUseSecureSockets,
	EArgEnd
	};

_LIT(KImskOutFile, "c:\\logs\\email\\imsk25.out");
_LIT(KImskScrFile, "c:\\logs\\email\\imsk25.scr");
_LIT(KSmtsComponent,"SMTS");
_LIT(KSmtsRFCEmails,"RFC822\\");

// UpdateProgressTimer is the time between calls to Progress()
//  A small value ensures that progress is checked often and that small files
//  do not slip through the checking process, especially when testing on NT-RAS
const TInt UpdateProgressTimer = 50000;		// 50 milliseconds

// DisplayProgressTimer is the time between calls to DisplayProgress()
//  This value should be larger than the UpdateProgressValue to avoid the logs being
//  swamped with progress messages
const TInt DisplayProgressTimer = 400000;	// 400 milliseconds


// Class to test a mesage Send Progress
class TTestProgress 
	{
public:
	TInt iCurrentMsg;
	TInt iBytesToSend;
	TInt iBytesSent;
	};


LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D TMsvId smtpService;
LOCAL_D CBaseServerMtm* serverMtm;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D TInt globalError;
LOCAL_D TTestProgress globalProgress;
LOCAL_D TInt progressCount;


LOCAL_C TBool TimeToDisplayProgress()
	{
	// Should we display progress?
	// Yes if (progressCount mod (update_to_display_ratio)) == 0
	TInt display = (progressCount%(DisplayProgressTimer / UpdateProgressTimer))  == 0;
	return display;
	}


LOCAL_C TInt readConsoleLineBuf(const TDesC& aPrompt, TDes& aBuf)
	{
	test.Printf(_L("\n "));
	TBool quit = EFalse;
	_LIT(KDeleteChar, "\b \b");

	test.Printf(aPrompt);
	while (!quit)
		{
		TKeyCode k = test.Getch();
		TChar key = k;
		switch (key)
			{
		case EKeyBackspace:
			if (aBuf.Length() > 0)
				{
				test.Printf(KDeleteChar);
				aBuf = aBuf.Left(aBuf.Length() - 1);
				}
			break;
		case EKeyEnter:
			if (aBuf.Length())
				quit = ETrue;
			else
				test.Printf(_L("\nLogin, password and server address cannot be zero length "));
			break;
		default:
			test.Printf(_L("%c"), key);
			aBuf.Append(key);
			break;
			}
		}
	return(KErrNone);
	}


LOCAL_C void ClosedownL()
	{
	CSecureSocketLibraryLoader::Unload();
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();
		
	CleanupStack::PopAndDestroy(2);  //testUtils, ischeduler
	}


LOCAL_C void TestProgress(TImSmtpProgress& aProgress)
// Check the Progress is Valid
	{
	// Make sure that we haven't finished sending
	if (aProgress.Sent() == aProgress.SendTotal())
		return;

	// Check if we are about to send a New Message
	if (globalProgress.iCurrentMsg != aProgress.MsgNo()+1)
		{
		// Check if the previous sent message sent all of its bytes ok
		// (ie check if it had a complete Progress bar).
		if (globalProgress.iCurrentMsg > 0)
			{
			TBuf<128> logString;
			if (globalProgress.iBytesToSend != globalProgress.iBytesSent)
				// Message has completed but progress info is not correct
				{
				globalError = KErrGeneral;
				logString.Format(_L("Test failed - Incorrect Progress - Message No.%d didn't have a full progress bar"), globalProgress.iCurrentMsg);
				testUtils->WriteComment(logString);
				return;
				}
			else
				// Message has completed OK
				{
				logString.Format(_L("Completed message No.%d/%d.\n"), 
					globalProgress.iCurrentMsg, aProgress.SendTotal());
				testUtils->WriteComment(logString);			
				}
			}

		// Set up the Progress for a New Message
		globalProgress.iCurrentMsg = aProgress.MsgNo()+1;
		}
	else
		{
		// Make sure the Bytes to Send hasn't altered
		if (globalProgress.iBytesToSend >0 &&
			aProgress.iSendFileProgress.iBytesToSend != globalProgress.iBytesToSend)
			{
			globalError = KErrGeneral;
			testUtils->WriteComment(_L("Test failed - Incorrect Progress - num Bytes To Send has changed"));
			return;
			}

		// Make sure the Bytes Sent doesn't go down
		if (aProgress.iSendFileProgress.iBytesSent < globalProgress.iBytesSent)
			{
			globalError = KErrGeneral;
			testUtils->WriteComment(_L("Test failed - Incorrect Progress - num Bytes Sent has gone down"));
			return;
			}
		}

	// Progress is ok, so set the new Bytes Sent Count
	globalProgress.iBytesSent = aProgress.iSendFileProgress.iBytesSent;
	globalProgress.iBytesToSend = aProgress.iSendFileProgress.iBytesToSend;
	}


LOCAL_C void DisplayProgress(TImSmtpProgress aProgress)
	{
	//  State of Smtp session?
	TInt state = aProgress.Status();
	TBuf<128> logString;

	switch(state)
		{
		case EMsgOutboxProgressWaiting:
			testUtils->WriteComment(_L("Waiting to start\n"));
			break;
		case EMsgOutboxProgressDone:
			testUtils->WriteComment(_L("Connection Closed.\n"));
			logString.Format(_L("%d messages: Sent %d, failed to send %d, didn't attempt to send %d.\n"), 
				aProgress.SendTotal(),
				aProgress.Sent(),
				aProgress.FailedToSend(),
				aProgress.NotSent());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressConnecting:
			logString.Format(_L("Connecting to SMTP server. Sending %d messages.\n"),
				aProgress.SendTotal());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressSending:
			logString.Format(_L("Sending message No.%d/%d. Transmitted %d bytes of %d            \n"),     
				aProgress.MsgNo()+1,
				aProgress.SendTotal(),
				aProgress.iSendFileProgress.iBytesSent,
				aProgress.iSendFileProgress.iBytesToSend);
			testUtils->WriteComment(logString);
			break;
		}
	}

LOCAL_C void UpdateProgress()
	{
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf =serverMtm->Progress();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	

	//  State of Smtp session?
	TInt state = progress.Status();

	if (state == EMsgOutboxProgressSending)
		{
		// Test the sending progress
		TestProgress(progress);
		}

	if (TimeToDisplayProgress())
		{
		DisplayProgress(progress);
		}

	progressCount++;
	}


class CSmtpTimer : public CTimer
	{
public:
	static CSmtpTimer* NewL(CTestActive& aActive);
	void StartL();
private:
	CSmtpTimer(CTestActive& aActive);
	void RunL();	
	CTestActive& iSmtpActive;
	};

CSmtpTimer::CSmtpTimer(CTestActive& aActive)
: CTimer(EPriorityLow),	iSmtpActive(aActive)

	{}


CSmtpTimer* CSmtpTimer::NewL(CTestActive& aActive)
	{
	CSmtpTimer* self = new(ELeave) CSmtpTimer(aActive);
	CleanupStack::PushL(self);
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

void CSmtpTimer::StartL()
	{
	CTimer::ConstructL();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}

void CSmtpTimer::RunL()
	{
	test.Console()->SetPos(0, 10);
	UpdateProgress();

	//this is how you check if the MTM is finished or not!
	if (globalError || !iSmtpActive.IsActive())   
		{
		iSmtpActive.Cancel();
		CActiveScheduler::Stop();
		return;
		}
	else
		{
		After(UpdateProgressTimer);
		}
	}


LOCAL_C void TestSendingMessagesL()
	{
	// About to start sending messages
	globalProgress.iCurrentMsg = 0;
	globalProgress.iBytesSent = 0;
	globalProgress.iBytesToSend = 0;

	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CMsvEntrySelection* sendSel = new(ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sendSel);
	testUtils->iServerEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId);
	User::LeaveIfError(testUtils->iServerEntry->GetChildren(*sendSel));
	TInt count = sendSel->Count();
	if (!count)
		{
		testUtils->WriteComment(_L("\t No messages to send in the outbox"));
		CleanupStack::PopAndDestroy(2); //testActive, sendsel
		return;
		}
	TBuf<100> logString;

	testUtils->WriteComment(_L("Issuing CopyFromLocal request...\n"));
	logString.Format(_L("\t messages to send in the outbox - %d"), count);
	testUtils->WriteComment(logString);
	
	serverMtm->CopyFromLocalL(*sendSel,smtpService, testActive->iStatus);
	CSmtpTimer* timer=CSmtpTimer::NewL(*testActive);
	CleanupStack::PushL(timer);

	testActive->StartL();
	timer->StartL();
	CActiveScheduler::Start();

	// If there are no errors so far, then check the progress
	if (!globalError)
		{
		UpdateProgress();
		logString.Zero();
		logString.Format(_L("\t CopyFromLocalL completed with %d"), testActive->iStatus.Int());
		testUtils->WriteComment(logString);
 		TImSmtpProgress temp;	
		TPckgC<TImSmtpProgress> paramPack(temp);
	
		const TDesC8& progBuf =serverMtm->Progress();
		paramPack.Set(progBuf);
		TImSmtpProgress progress=paramPack();
		globalError=progress.Error();
		}
	
	//For Coverage
	CMsvEntrySelection* selection1 = new(ELeave)CMsvEntrySelection();
	CleanupStack::PushL(selection1);
	TRequestStatus st;
	TMsvId s =0;
	TRAP_IGNORE(serverMtm->MoveFromLocalL(*selection1, s,st));
	TRAP_IGNORE(serverMtm->CopyToLocalL(*selection1, s,st));
	TRAP_IGNORE(serverMtm->MoveToLocalL(*selection1, s,st));
	TRAP_IGNORE(serverMtm->CopyWithinServiceL(*selection1, s,st));
	TRAP_IGNORE(serverMtm->MoveWithinServiceL(*selection1, s,st));
	
	TRAP_IGNORE(serverMtm->DeleteAllL(*selection1, st));
	TMsvEntry aEntry;
	TRAP_IGNORE(serverMtm->CreateL(aEntry,st));
	TRAP_IGNORE(serverMtm->ChangeL(aEntry,st));
	
		
	CleanupStack::PopAndDestroy(selection1); //Selection
	
	CleanupStack::PopAndDestroy(3); //testActive, sendsel, /*timer*/
	}


LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(KSmtsTest);
	
	testUtils->FileSession().Delete(KImskOutFile);
	testUtils->FileSession().Delete(KImskScrFile);

	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	
	if (!testUtils->FileSession().MkDir(KEmailDataDir))
		{
		testUtils->Printf(_L("Created c:\\logs\\email directory\n"));
		}

	testUtils->GoClientSideL();
	smtpService = testUtils->CreateSmtpServiceL();
	
	// Handle command line arguments
	CCommandLineArguments* cmdLineArg=CCommandLineArguments::NewLC();
	TInt count = cmdLineArg->Count();
	TBool isCmdLine=FALSE;

	if (count>2)	// Command line arguments?
		{
		if ((!(cmdLineArg->Arg(EArgTestParams).Compare(KTestParams))) && count==EArgEnd)
			isCmdLine=TRUE;
		}			
		TInt iapID = 0;
		CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
		CleanupStack::PushL(dbSession);
		CCDConnectionPrefsRecord *connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
		CleanupStack::PushL(connPrefRecord);
		connPrefRecord->iDirection = ECommDbConnectionDirectionOutgoing;
		if(!connPrefRecord->FindL(*dbSession))
			{
			User::Leave(KErrNotFound);		
			}		
		iapID = connPrefRecord->iDefaultIAP;
		
		CleanupStack::PopAndDestroy(2); //dbSession,connPrefRecord
	
	
	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();

	// Add IAP's to iIAPPreferences
	TImIAPChoice iap;
	iap.iIAP = iapID;
	iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	prefs->AddIAPL(iap);


	//overwrite the setings with test code one.  Don't want the default settings.

	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	settings->Reset();

	TBuf<100> buf;
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		test.Printf(_L("Server address: %S\n"),&buf);
		}
	else
		{
		readConsoleLineBuf(_L("Server address: "),buf);
		}
	settings->SetServerAddressL(buf);
	buf.Zero();
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgEmail));
		buf=lex.NextToken();
		test.Printf(_L("Mail: %S\n"),&buf);
		}
	else
		{
		readConsoleLineBuf(_L("From email address: "),buf);
		}
	settings->SetEmailAddressL(buf);
	settings->SetEmailAliasL(_L("SMTP Test01"));
	settings->SetReplyToAddressL(buf);
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetReceiptAddressL(buf);
	settings->SetSendCopyToSelf(ESendNoCopy);
	settings->SetPort(25);

	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgUseSecureSockets));
		TChar choice=lex.Get();
		test.Printf(_L("Use secure sockets: %c\n"),choice);
		if (choice=='y' || choice=='Y')
			settings->SetSecureSockets(ETrue);
		else
			settings->SetSecureSockets(EFalse);
		}
	else
		{
		test.Printf(_L("\n Use secure sockets? Enter (Y/N)\n"));
		TKeyCode k = test.Getch();
		TChar key = k;
		test.Printf(_L("%c"), key);
		switch (key)
			{
			case 'Y':
			case 'y':
				settings->SetSecureSockets(ETrue);
				break;
			default:
				settings->SetSecureSockets(EFalse);
				break;
			}
		}

	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	TSmtpAccount smtpAccountId ;
	accounts->GetSmtpAccountL(smtpService,smtpAccountId); 
	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);
	accounts->SaveSmtpIapSettingsL(smtpAccountId, *prefs);
	
	CleanupStack::PopAndDestroy(4, cmdLineArg); // accounts, settings, prefs, cmdLineArg
	testUtils->WriteComment(_L(" smtp server created"));
	
	testUtils->GoServerSideL();

	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;
	}

LOCAL_C void doMainL() 
	{
	InitL();

	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);


	TParse dir;
	testUtils->ResolveFile(KSmtsComponent,KSmtsRFCEmails,dir);

	testUtils->CreateMessageFilesL(smtpService,KMsvGlobalOutBoxIndexEntryId,dir.FullName());
	
	testUtils->WriteComment(_L(" Messages created in the outbox"));
	testUtils->WriteComment(_L("Instantiated smtp server mtm"));

	testUtils->TestStart(1);
	TestSendingMessagesL();
	testUtils->TestFinish(1,globalError);
	ClosedownL();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTS01 Test SMTP server MTM class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
