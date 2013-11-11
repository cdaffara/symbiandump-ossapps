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
#include <es_sock.h>
#include <commdb.h>
#include <cdbpreftable.h>
#include <imsk.h>
#include <commdbconnpref.h>
#include <bacline.h>
#include <cemailaccounts.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>
using namespace CommsDat;

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif


_LIT(KSmtsTest, "T_SMTS02 - Testing SMTS Server MTM using dial out with On-line");
RTest test(KSmtsTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgEmail,
	EArgEnd
	};

_LIT(KImskOutFile, "c:\\logs\\email\\imsk25.out");
_LIT(KImskScrFile, "c:\\logs\\email\\imsk25.scr");
_LIT(KSmtsComponent,"SMTS");
_LIT(KSmtsRFCEmails,"RFC822\\");

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D TMsvId smtpService;
LOCAL_D CBaseServerMtm* serverMtm;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D TInt globalError;
LOCAL_D RConnection gConnection;
LOCAL_D RSocketServ gSocketServ;



LOCAL_C void DisplayProgress()
	{
 	TImSmtpProgress temp;
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf =serverMtm->Progress();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	

	TBuf<128> logString;
	//  State of Smtp session?
	TInt state = progress.Status();
	switch(state)
		{
		case EMsgOutboxProgressWaiting:
			testUtils->WriteComment(_L("Waiting to start\n"));
			break;
		case EMsgOutboxProgressDone:
			testUtils->WriteComment(_L("Connection Closed.\n"));
			logString.Format(_L("%d messages: Sent %d, failed to send %d, didn't attempt to send %d.\n"), 
				progress.SendTotal(),
				progress.Sent(),
				progress.FailedToSend(),
				progress.NotSent());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressConnecting:
			logString.Format(_L("Connecting to SMTP server. Sending %d messages.\n"),
				progress.SendTotal());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressSending:
			logString.Format(_L("Sending message No.%d/%d. Transmitted %d bytes of %d            \n"),     
				progress.MsgNo()+1,
				progress.SendTotal(),
				progress.iSendFileProgress.iBytesSent,
				progress.iSendFileProgress.iBytesToSend);
			testUtils->WriteComment(logString);
			break;
		}
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
	DisplayProgress();

	if (!iSmtpActive.IsActive())   //this is how you check if the MTM is finished or not!
		{
		iSmtpActive.Cancel();
		CActiveScheduler::Stop();
		return;
		}
	else
		After(4000000);
	}


LOCAL_C void TestSendingMessagesL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CMsvEntrySelection* sendSel = new(ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sendSel);

	testUtils->iServerEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId);
	User::LeaveIfError(testUtils->iServerEntry->GetChildren(*sendSel));
	TInt count=sendSel->Count();
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

	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;
	
	serverMtm->CopyFromLocalL(*sendSel,smtpService, testActive->iStatus);
	CSmtpTimer* timer=CSmtpTimer::NewL(*testActive);
	CleanupStack::PushL(timer);

	testActive->StartL();
	timer->StartL();
	CActiveScheduler::Start();
	//check progress
	DisplayProgress();
	logString.Zero();
	logString.Format(_L("\t CopyFromLocalL completed with %d"), testActive->iStatus.Int());
	testUtils->WriteComment(logString);
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf =serverMtm->Progress();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	
	globalError=progress.Error();
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

	}

LOCAL_C void ClosedownL()
	{
	if (globalError==-245 || globalError==-183 || globalError == -36|| !globalError)
		testUtils->TestHarnessCompleted();
	else
		testUtils->TestHarnessFailed(globalError);

	gConnection.Stop();
	gConnection.Close();
	CleanupStack::PopAndDestroy(2);  //testUtils, ischeduler
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


	testUtils->WriteComment(_L(" Create 2 ISPs - pipex and demon by running T_CREATEDB.  Set demon as the default ISP."));
	testUtils->WriteComment(_L(" Run T_EDITDB and select  '1) outgoing connection pref'.  Set Ranking 1 - IAP to Demon."));
	testUtils->WriteComment(_L(" press page down and set ranking 2 IAP to pipex.")); 
	//overwrite the setings with test code one.  Don't want the default settings.

	// Handle command line arguments
	CCommandLineArguments* cmdLineArg=CCommandLineArguments::NewLC();
	TInt count = cmdLineArg->Count();
	TBool isCmdLine=FALSE;
	if (count>2)	// Command line arguments?
		{
		if ((!(cmdLineArg->Arg(EArgTestParams).Compare(KTestParams))) && count==EArgEnd)
			isCmdLine=TRUE;
		}	


	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	settings->Reset();

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		test.Printf(_L("Server address: %S\n"),&buf);
		settings->SetServerAddressL(buf);
		}
	else
		{
		_LIT(KSmtpServer, "smtp.demon.co.uk");
		settings->SetServerAddressL(KSmtpServer);
		}
	settings->SetEmailAliasL(_L("SMTP Test1"));

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgEmail));
		buf=lex.NextToken();
		test.Printf(_L("Mail: %S\n"),&buf);
		settings->SetEmailAddressL(buf);
		settings->SetReplyToAddressL(buf);
		settings->SetReceiptAddressL(buf);
		}
	else
		{
		_LIT(KSmtpEmailAddress, "<test@psion9.demon.co.uk>");
		settings->SetEmailAddressL(KSmtpEmailAddress);
		settings->SetReplyToAddressL(KSmtpEmailAddress);
		settings->SetReceiptAddressL(KSmtpEmailAddress);
		}

	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetPort(25);

	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();
	// Add IAP's to iIAPPreferences
	TImIAPChoice iap;
	
	TInt iapID = 0;
	CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(dbSession);
	CCDConnectionPrefsRecord *connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRecord);
	
	connPrefRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	connPrefRecord->iRanking = 1;
	if(!connPrefRecord->FindL(*dbSession))
		{
		User::Leave(KErrNotFound);		
		}			
	iapID = connPrefRecord->iDefaultIAP;
	
	CleanupStack::PopAndDestroy(2); //dbSession, connPrefRecord

	iap.iIAP = iapID;
	iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	prefs->AddIAPL(iap);
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService,smtpAccountId); 
	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);
	accounts->SaveSmtpIapSettingsL(smtpAccountId, *prefs);
	
	CleanupStack::PopAndDestroy(4, cmdLineArg); // accounts, prefs, settings, cmdLineArg
	
	testUtils->WriteComment(_L(" smtp server created"));

	TParse dir;
	testUtils->ResolveFile(KSmtsComponent,KSmtsRFCEmails,dir);
	testUtils->GoServerSideL();
	
	testUtils->CreateMessageFilesL(smtpService,KMsvGlobalOutBoxIndexEntryId,dir.FullName());
	testUtils->WriteComment(_L(" Messages created in the outbox"));
	testUtils->WriteComment(_L("Instantiated smtp server mtm"));
	
	testUtils->iServerEntry->SetEntry(smtpService);
	TBuf<64> logString;
	logString.Format(_L(" IAP is %d"),iapID);
	
	testUtils->WriteComment(logString);

	gSocketServ.Connect();
	User::LeaveIfError(gConnection.Open(gSocketServ));	
	testUtils->TestStart(1,_L("SocketServ startDialout called"));
	TRequestStatus status;
	gConnection.Start(status);

	TInt err=0;
	while(status==KRequestPending)
		{
		CActiveScheduler::Current()->RunIfReady(err,0);
		User::WaitForAnyRequest();
		}
	testUtils->TestFinish(1,status.Int());
	if (status.Int())
		{
		ClosedownL();
		return;
		}
	testUtils->WriteComment(_L("Genconn startDialout successful"));
	TUint32 address=0;
	TUint32 activeIAP;
	TBuf<KCommsDbSvrMaxColumnNameLength> settingName;

	settingName.Copy(TPtrC(IAP));
	settingName.Append(TChar('\\'));
	settingName.Append(TPtrC(COMMDB_ID));
	address = gConnection.GetIntSetting(settingName, activeIAP);
	
	logString.Zero();
	logString.Format(_L(" Active iap from genconn is %d"),address);
	testUtils->WriteComment(logString);

	testUtils->TestStart(2,_L("sending msgs with IAP same as active IAP"));
	TestSendingMessagesL();
	testUtils->TestFinish(2,globalError);


	if (globalError)
		{
		ClosedownL();
		return;
		}
	gConnection.Stop();
	gConnection.Close();			
	
	testUtils->TestStart(3,_L("sending msgs with IAP different to active IAP"));
	testUtils->CreateMessageFilesL(smtpService,KMsvGlobalOutBoxIndexEntryId,dir.FullName());

	logString.Zero();
	testUtils->WriteComment(logString);
	gSocketServ.Connect();
	globalError=gConnection.Open(gSocketServ);	
	if (!globalError)
		{
		CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
		CleanupStack::PushL(dbSession);
		connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
		CleanupStack::PushL(connPrefRecord);
		
		connPrefRecord->iDirection = ECommDbConnectionDirectionOutgoing;
		connPrefRecord->iRanking = 2;		
		connPrefRecord->FindL(*dbSession);				
		iapID = connPrefRecord->iDefaultIAP;
		
		CleanupStack::PopAndDestroy(2);	//dbSession, connPrefRecord	

		TCommDbMultiConnPref prefs;
		TInt rank=1;
		TCommDbConnPref pref;
		pref.SetDirection(ECommDbConnectionDirectionOutgoing);
		pref.SetBearerSet(KCommDbBearerPSD | KCommDbBearerCSD);

		pref.SetDirection(ECommDbConnectionDirectionOutgoing);
		pref.SetBearerSet(KCommDbBearerPSD | KCommDbBearerCSD);
		iap.iIAP = 	iapID;
		iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;

		pref.SetIapId(iap.iIAP);
		pref.SetDialogPreference(iap.iDialogPref);
		User::LeaveIfError(prefs.SetPreference(rank, pref));

		prefs.SetConnectionAttempts(rank);
		gConnection.Start(prefs, status);

		
		while(status==KRequestPending)
			{
			CActiveScheduler::Current()->RunIfReady(err,0);
			User::WaitForAnyRequest();
			}

		}
	if (globalError || status.Int())
		{
		if (!globalError)
			globalError=status.Int();
		ClosedownL();
		return;
		}
	address=0;
	settingName.Copy(TPtrC(KCDTypeNameIAP));
	settingName.Append(TChar('\\'));
	settingName.Append(TPtrC(KCDTypeNameRecordTag));
	err = gConnection.GetIntSetting(settingName, activeIAP);

	logString.Zero();
	logString.Format(_L(" Active IAP from genconn is %d"),address);
	testUtils->WriteComment(logString);
	TestSendingMessagesL();
	globalError=(globalError==-183)? 0 : globalError;

	testUtils->TestFinish(3,globalError);
	testUtils->WriteComment(_L("Test 3 should complete with -183"));
	ClosedownL();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTS02 Test SMTP server MTM class"));
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
