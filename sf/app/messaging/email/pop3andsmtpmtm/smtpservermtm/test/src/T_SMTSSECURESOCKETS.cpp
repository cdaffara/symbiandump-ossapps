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
// Brief description of test harness:
// Tests SSL/TLS functionality in SMTS
// Input files required to run test harness:
// The following files should be copied to <drive>:\msgtest\smts\scripts
// t_smtsTlsNotSupported.scr
// t_smtsTlsTempFail.scr
// t_smtsTlsOk.scr
// t_smtsTlsSyntaxError.scr
// t_smtsTlsNoServices.scr
// t_smtsTlsRefused.scr
// t_smtsTlsStopResponding.scr
// t_smtsTlsNewSession.scr
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_SMTSSECURESOCKETS.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \messaging\msg\smts\
// bldmake bldfiles
// abld test build t_SMTSSECURESOCKETS
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 2. Build the test utilities:
// cd \messaging\msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_SMTSSECURESOCKETS.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_SMTSSECURESOCKETS.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Copy \epoc32\wins\c\msgtest\smts\scripts\*.scr to
// <DRIVE>:\msgtest\smts\scripts\
// Note that this drive should be a CF card as the test files are large
// 5. Run T_SMTSSECURESOCKETS.exe on the other platform with the CF card
// 
//

#include "emailtestutils.h"
#include "scripttestutils.h"
#include <cemailaccounts.h>

_LIT(KSmtsTest, "Testing SMTS secure connection with script files");
_LIT(KSmtsComponent,"SMTS");

// Script Files for the Tests
_LIT(KTest1ScriptFile,	"scripts\\t_smtsTlsNotSupported.scr");
_LIT(KTest1Desc,		"Test 1 connects to a server which doesn't support SSLTLS.");
_LIT(KTest1Result,		"Test 1 should finish with -246 (SMTP TLS Negotiate Failed).");

_LIT(KTest2ScriptFile,	"scripts\\t_smtsTlsTempFail.scr");
_LIT(KTest2Desc,		"Test 2 connects to a server which supports SSLTLS, but TLS is not available due to temporary reason.");
_LIT(KTest2Result,		"Test 2 should finish with -185 (SSL TLS Negotiate Failed).");

_LIT(KTest3ScriptFile,	"scripts\\t_smtsTlsOk.scr");
_LIT(KTest3Desc,		"Test 3 connects to a server which supports SSLTLS.");
_LIT(KTest3Result,		"Test 3 should complete with no error.");

_LIT(KTest4ScriptFile,	"scripts\\t_smtsTlsSyntaxError.scr");
_LIT(KTest4Desc,		"Test 4 connects to a server which supports SSLTLS, but returns an Error.");
_LIT(KTest4Result,		"Test 4 should finish with -185 (SSL TLS Negotiate Failed).");

_LIT(KTest5ScriptFile,	"scripts\\t_smtsTlsNoServices.scr");
_LIT(KTest5Desc,		"Test 5 connects to a server which supports SSLTLS, starts TLS, but the server returns no supported services.");
_LIT(KTest5Result,		"Test 5 should finish with -185 (SSL TLS Negotiate Failed).");

_LIT(KTest6ScriptFile,	"scripts\\t_smtsTlsRefused.scr");
_LIT(KTest6Desc,		"Test 6 connects to a server which supports SSLTLS, but then refuses connection, due to lack of security");
_LIT(KTest6Result,		"Test 6 should finish with -244 (SMTP Login Refused).");

_LIT(KTest8ScriptFile,	"scripts\\t_smtsTlsNewSession.scr");
_LIT(KTest8Desc,		"Test 8 - After TLS handshake, the server tries to start new session, by returning STARTTLS");
_LIT(KTest8Result,		"Test 8 should complete with no error, as MTM ignores this command.");

RTest test(KSmtsTest);

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils; 
LOCAL_D TMsvId smtpService;
LOCAL_D CBaseServerMtm* serverMtm;
LOCAL_D TInt testError;


// Class to hold the data required for a Test
class TTestData
	{
	public:
		TInt		iTestNumber;
		TBuf<50>	iTestFile;
		TBuf<200>	iTestDescription;
		TBuf<200>	iTestResult;
	};


LOCAL_C TInt TestSendingMessagesL()
	{
	testError = 0;

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
		testError = KErrNotFound;
		}
	else
		{

		serverMtm->CopyFromLocalL(*sendSel,smtpService, testActive->iStatus);

		testActive->StartL();
		CActiveScheduler::Start();
 		TImSmtpProgress temp;	
		TPckgC<TImSmtpProgress> paramPack(temp);

		const TDesC8& progBuf =serverMtm->Progress();
		paramPack.Set(progBuf);
		TImSmtpProgress progress=paramPack();	
		CleanupStack::PopAndDestroy(2); //testActive, sendsel
		testError = progress.Error();
		}

	return testError;
	}

LOCAL_C void ClosedownL()
	{
	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(2);  //testUtils, ischeduler
	}

LOCAL_C void ResetSettingsL(TBool aSecureSocket)
	{
	_LIT(KSmtpServer, "smtp.demon.co.uk");
	_LIT(KSmtpEmailAlias, "SMTP STARTTLS");
	_LIT(KSmtpEmailAddress, "<a@b>");

	// Overwrite the setings with test code one.  Don't want the default settings.
	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService,smtpAccountId); 
	accounts->LoadSmtpSettingsL(smtpAccountId, *settings);

	settings->SetServerAddressL(KSmtpServer);
	settings->SetEmailAliasL(KSmtpEmailAlias);
	settings->SetEmailAddressL(KSmtpEmailAddress);
	settings->SetReplyToAddressL(KSmtpEmailAddress);
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetReceiptAddressL(KSmtpEmailAddress);
	settings->SetSecureSockets(aSecureSocket);
	settings->SetPort(25);
	
	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);

	CleanupStack::PopAndDestroy(2, settings); //settings, store/accounts
	}

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(KSmtsTest);

	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	
	testUtils->GoClientSideL();
 	
	smtpService = testUtils->CreateSmtpServiceL();
	ResetSettingsL(ETrue);
	CleanupStack::PopAndDestroy(testUtils);
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);
	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;
	testUtils->WriteComment(_L("Smtp server created"));
	}

LOCAL_C void CreateMsgToSendL()
// Create a message to send & place it in the outbox
	{
	TParse scriptFile;
	_LIT(KSmtsRfcFile, "RFC822\\rfc82201.txt");
	testUtils->ResolveFile(KSmtsComponent, KSmtsRfcFile, scriptFile);
	testUtils->CreateMessageL(scriptFile.FullName(), smtpService, KMsvGlobalOutBoxIndexEntryId);
	testUtils->WriteComment(_L(" Message created in the outbox"));
	testUtils->WriteComment(_L("\nPerforming Tests\n"));
	}


LOCAL_C void RunTestL(TTestData& aTestData)
	{
	// Add a blank line to the Log file before the new Test
	_LIT(KBlankLine, "");
	testUtils->WriteComment(KBlankLine);

	TParse scriptFile;
	testUtils->ResolveFile(KSmtsComponent, aTestData.iTestFile, scriptFile);
	testUtils->CopyScriptFileL(scriptFile.FullName(), _L("25"));
	testUtils->TestStart(aTestData.iTestNumber);
	testUtils->WriteComment(aTestData.iTestDescription);
	testUtils->WriteComment(aTestData.iTestResult);
	
	// Run the Test & check the error
	TestSendingMessagesL();
	}


LOCAL_C void RunAllTestsL()
	{
	
	//	Create a Message to Send
	CreateMsgToSendL();

	//-------------------------------
	//	Test 1 - TLS Not Supported
	//-------------------------------
	TTestData testData;
	testData.iTestNumber = 1;
	testData.iTestFile = KTest1ScriptFile;
	testData.iTestDescription = KTest1Desc;
	testData.iTestResult = KTest1Result;
	RunTestL(testData);
	testError=(testError==-246)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//-----------------------------------------------------------------
	//	Test 2 - TLS Not Available due to Temp Reason (ie 454 responce)
	//-----------------------------------------------------------------
	testData.iTestNumber = 2;
	testData.iTestFile = KTest2ScriptFile;
	testData.iTestDescription = KTest2Desc;
	testData.iTestResult = KTest2Result;
	RunTestL(testData);
	testError=(testError==-185)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//------------------------------------------------------
	//	Test 3 - TLS Supported & Msg Sent (ie 220 responce)
	//------------------------------------------------------
	testData.iTestNumber = 3;
	testData.iTestFile = KTest3ScriptFile;
	testData.iTestDescription = KTest3Desc;
	testData.iTestResult = KTest3Result;
	RunTestL(testData);
	testError=(testError==0)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//	Create a Message to Send, as the previous one was sent by Test 3
	CreateMsgToSendL();

	//---------------------------------------------
	//	Test 4 - TLS Syntax Error (ie 501 responce)
	//---------------------------------------------
	testData.iTestNumber = 4;
	testData.iTestFile = KTest4ScriptFile;
	testData.iTestDescription = KTest4Desc;
	testData.iTestResult = KTest4Result;
	RunTestL(testData);
	testError=(testError==-185)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//--------------------------------------------------------------------------------------
	//	Test 5 - No Supported Services (After Handshake, Server sends no Supported Services)
	//--------------------------------------------------------------------------------------
	testData.iTestNumber = 5;
	testData.iTestFile = KTest5ScriptFile;
	testData.iTestDescription = KTest5Desc;
	testData.iTestResult = KTest5Result;
	RunTestL(testData);
	testError=(testError==-185)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//----------------------------------------
	//	Test 6 - TLS Refusal (ie 554 responce)
	//----------------------------------------
	testData.iTestNumber = 6;
	testData.iTestFile = KTest6ScriptFile;
	testData.iTestDescription = KTest6Desc;
	testData.iTestResult = KTest6Result;
	RunTestL(testData);
	testError=(testError==-244)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//----------------------------------------
	//	Test 7 - Server Stops Responding
	//----------------------------------------
/*	testData.iTestNumber = 7;
	testData.iTestFile = KTest7ScriptFile;
	testData.iTestDescription = KTest7Desc;
	testData.iTestResult = KTest7Result;
	RunTestL(testData);*/

	//--------------------------------------------------------------------------
	//	Test 8 - Session Already Active (Server sends STARTTLS after handshake)
	//--------------------------------------------------------------------------
	testData.iTestNumber = 8;
	testData.iTestFile = KTest8ScriptFile;
	testData.iTestDescription = KTest8Desc;
	testData.iTestResult = KTest8Result;
	RunTestL(testData);
	testError=(testError==0)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);
	}


LOCAL_C void doMainL()
	{
	InitL();
	testUtils->iServerEntry->SetEntry(smtpService);

	// Set the Setting to use Secure Sockets & Run the Tests
	_LIT(KSecurityOn, "------Set Security On------");
	testUtils->WriteComment(KSecurityOn);
	RunAllTestsL();

	// End of the Tests
	ClosedownL();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTSSECURESOCKETS Test SMTP server MTM secure connection"));
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
