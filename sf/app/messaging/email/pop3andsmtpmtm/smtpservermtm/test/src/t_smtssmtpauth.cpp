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
// Tests SMTP AUTH functionality in SMTS
// Input files required to run test harness:
// The following files should be copied to <drive>:\msgtest\smts\scripts
// imsk25_tls_no_smtp_auth_on_server.scr
// imsk25_tls_no_smtp_auth.scr
// imsk25_tls_login.scr
// imsk25_no_tls_wrong_username.scr
// imsk25_no_tls_wrong_password.scr
// imsk25_no_tls_plain.scr
// imsk25_no_tls_login.scr
// imsk25_no_tls_cram_md5.scr
// imsk25_no_tls_no_smtp_auth_on_server.scr
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_SMTSSMTPAUTH.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \messaging\msg\smts\
// bldmake bldfiles
// abld test build t_SMTSSMTPAUTH
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 2. Build the test utilities:
// cd \messaging\msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\t_SMTSSMTPAUTH.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\t_SMTSSMTPAUTH.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Copy \epoc32\wins\c\msgtest\smts\scripts\*.scr to
// <DRIVE>:\msgtest\smts\scripts\
// Note that this drive should be a CF card as the test files are large
// 5. Run t_SMTSSMTPAUTH.exe on the other platform with the CF card
// Once the test is run, run logtoscr from the command prompt.  This will generate a number of .out 
// files in the c:\logs\email directory of the environment you are using.  To check the test have 
// completed correctly compare these files with the files of the same name in  the 
// c:\msgtest\smts\checkagainst directory.
// // Note : For dial up tests do the following:
// 1) The server names used in function ResetSettingsL() are dummy
// Need to use right server names for testing for dial up testing.
// However,they have been tested by giving the right server names. 
// 2) Uncomment the line "testUtils->DeleteScriptFilesFromLogsDirL()" 
// in function RunTestL(). 
// 
//

#include "emailtestutils.h"
#include "scripttestutils.h"
#include <cemailaccounts.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT(KSmtsTest, "Testing SMTS smtp authentication with script files");
_LIT(KSmtsComponent,"SMTS");

// Script Files for the Tests

_LIT(KTest3ScriptFile,	"scripts\\imsk25_tls_login.scr");
_LIT(KTest3Desc,		"Tls should handshake, Authentication takes place using Login Mechanism.");
_LIT(KTest3Result,		"Mail is sent.");

_LIT(KTest4ScriptFile,	"scripts\\imsk25_no_tls_wrong_username.scr");
_LIT(KTest4Desc,		"No Tls, Wrong username used ");
_LIT(KTest4Result,		"Mail is sent.");

_LIT(KTest5ScriptFile,	"scripts\\imsk25_no_tls_wrong_password.scr");
_LIT(KTest5Desc,		"No Tls, Wrong password used");
_LIT(KTest5Result,		"Mail is sent.");

_LIT(KTest6ScriptFile,	"scripts\\imsk25_no_tls_plain.scr");
_LIT(KTest6Desc,		"No Tls, authenticates using Plain Mechanism.");
_LIT(KTest6Result,		"Mail is sent.");

_LIT(KTest7ScriptFile,	"scripts\\imsk25_no_tls_login.scr");
_LIT(KTest7Desc,		"No Tls, authenticates using Login Mechanism");
_LIT(KTest7Result,		"Mail is sent.");

_LIT(KTest8ScriptFile,	"scripts\\imsk25_no_tls_cram_md5.scr");
_LIT(KTest8Desc,		"No Tls, authenticates using CRAM-MD5 Mechanism.");
_LIT(KTest8Result,		"Mail is sent.");

_LIT(KTest9ScriptFile,	"scripts\\imsk25_no_tls_no_smtp_auth_on_server.scr");
_LIT(KTest9Desc,		"No Tls, No AUTH command sent from Server.");
_LIT(KTest9Result,		"Mail is sent.");

_LIT(KTest10ScriptFile,	"scripts\\imsk25_ehlo_fail_conitnue_with_helo.scr");
_LIT(KTest10Desc,		"Smtp Auth with EHLO fail, Tls Off, Continue with HELO Command");
_LIT(KTest10Result,		"Mail is sent.");

_LIT(KTest11ScriptFile,	"scripts\\imsk25_ehlo_fail_quit_if_tls_on.scr");
_LIT(KTest11Desc,		"Smtp Auth with EHLO fail, Quit if TLS on");
_LIT(KTest11Result,		"Mail is not sent.");

_LIT(KTest12ScriptFile,	"scripts\\imsk25_tls_cram.scr");
_LIT(KTest12Desc,		"Tls should handshake, Authentication takes place using CRAM-MD5 Mechanism.");
_LIT(KTest12Result,		"Mail is sent.");

_LIT(KTest13ScriptFile,	"scripts\\imsk25_ehlo_fail_conitnue_with_hostname.scr");
_LIT(KTest13Desc,		"Smtp Auth with EHLO fail, Tls Off, Continue with hostname and then HELO");
_LIT(KTest13Result,		"Mail is sent.");

#ifdef _DEBUG
_LIT(KLogFilePath,		 "c:\\logs\\email\\ImLog25.txt");
#endif

_LIT(KScriptFilePath,	 "c:\\logs\\email\\imsk25.scr\\");

// Class to hold the data required for a Test
class TTestData
	{
	public:
		TInt		iTestNumber;
		TBuf<100>	iTestFile;
		TBuf<200>	iTestDescription;
		TBuf<200>	iTestResult;
	};
	
enum TImStripLogType
	{
	EImMakeInFile,
	EImMakeOutFile,
	EImEndMakeFile
	};

RTest test(KSmtsTest);

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils; 
LOCAL_D TMsvId smtpService;
LOCAL_D CBaseServerMtm* serverMtm;
LOCAL_D TInt testError;
LOCAL_D TTestData testData;

LOCAL_C void ClearOutboxL()
	{
	testUtils->iServerEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId);
	CMsvEntrySelection* sel = new(ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);
	testUtils->iServerEntry->GetChildrenWithMtm(KUidMsgTypeSMTP, *sel);

	TInt count = sel->Count();

	while (count--)
		{
		testUtils->iServerEntry->DeleteEntry(sel->At(count));
		}

	CleanupStack::PopAndDestroy(sel);
	}

#ifdef _DEBUG
		
LOCAL_C TBool CompareFilesL(TFileName aLogFile)
	{
	RFs& fs = testUtils->FileSession();
	
	// Need to create the create filename
	TFileName name = _L("imsk");
	TInt pos = 5;	// set to skip passed the "imlog" part

	while( aLogFile[pos] != '.' )
		{
		name.Append(aLogFile[pos]);
		++pos;
		}
	name.Append(_L(".out"));		

	// Load up the two files to compare.
	fs.SetSessionPath(_L("c:\\logs\\email\\"));
	
	RFile newFile;
	if( newFile.Open(fs, name, KEntryAttReadOnly) != KErrNone )
		{
		return EFalse;
		}
	CleanupClosePushL(newFile);
	
	fs.SetSessionPath(_L("c:\\msgtest\\smts\\checkagainst\\"));
	RFile oldFile;
	if( oldFile.Open(fs, name, KEntryAttReadOnly) != KErrNone )
		{
		CleanupStack::PopAndDestroy(&newFile);
		return EFalse;
		}
	CleanupClosePushL(oldFile);
	
	// Compare the files...
	HBufC8* line1Buffer = HBufC8::NewLC(1024);
	TPtr8 line1 = line1Buffer->Des();
	TBuf8<1> aChar1;

	HBufC8* line2Buffer = HBufC8::NewLC(1024);
	TPtr8 line2 = line2Buffer->Des();
	TBuf8<1> aChar2;
	TBool finished1 = EFalse;
	TBool finished2 = EFalse;
	TBool done = EFalse;
	
	do	{
		line1.FillZ();
		line1.SetLength(0);
		// compile the lines one char at a time
		do	{
			User::LeaveIfError(newFile.Read(aChar1, 1));
			if( aChar1.Length() > 0 )
				line1.Append(aChar1);
			else
				{
				finished1 = ETrue;
				break;
				}
			// stop at the end of line or no more data
			}
		while( (aChar1[0]!=0x0A)&&(line1.Length()<1024) );

		line2.FillZ();
		line2.SetLength(0);
		do	{
			User::LeaveIfError(oldFile.Read(aChar2, 1));
			if( aChar2.Length() > 0 )
				line2.Append(aChar2);
			else
				{
				finished2 = ETrue;
				break;
				}
			// stop at the end of line or no more data
			}
		while( (aChar2[0]!=0x0A)&&(line2.Length()<1024) );
		
		if( finished1 || finished2 )
			{
			// Check that both files have been completely read.
			if( finished1 && finished2 )
				{
				// Yep - done.
				done = ETrue;
				}
			else
				{
				// Files are of different length - failed!
				CleanupStack::PopAndDestroy(4, &newFile); 
				return EFalse;
				}
			}
		else
			{
			// get rid of white space
			line1.TrimRight();
			line2.TrimRight();

			if( line1.Compare(line2) != 0 )
				{
				CleanupStack::PopAndDestroy(4, &newFile); 
				return EFalse;
				}
			}
		} 
	while(!done);
	
	CleanupStack::PopAndDestroy(4, &newFile);
	
	return ETrue;
	}
	
LOCAL_C void ParseLogFileL(TFileName aLogFile)
	{
	RFs& fs = testUtils->FileSession();
	fs.SetSessionPath(_L("c:\\logs\\email\\"));

	CScriptTestUtils* scriptTestUtils = CScriptTestUtils::NewLC(*testUtils);

	TInt portStringCounter;
	TBufC<KMaxFileName> logFileName;
	TBuf<KMaxFileName> scriptFileName;

	// First create the script file, then create the output test file.

	TImStripLogType lineType = EImMakeInFile;
	while ((lineType == EImMakeInFile)
			|| (lineType == EImMakeOutFile))
		{
		logFileName = aLogFile;

		// Get the name of the destination script file.
		scriptFileName = _L("imsk");
		portStringCounter = 5;

		while (logFileName[portStringCounter] != L'.')
			{
			scriptFileName.Append(logFileName[portStringCounter]);
			portStringCounter++;
			}

		if (EImMakeInFile == lineType)
			{
			scriptFileName.Append(_L(".scr"));
			}
		else if (EImMakeOutFile == lineType)
			{
			scriptFileName.Append(_L(".out"));
			}

		if (lineType == EImMakeInFile)
			scriptTestUtils->LogToInputFileL(logFileName, scriptFileName, EFalse);
		else
			scriptTestUtils->LogToOutputFileL(logFileName, scriptFileName, ETrue);

		// If we have just created the script file the create the output test file.
		if (EImMakeInFile == lineType)
			{
			lineType = EImMakeOutFile;
			}
		else
			{
			lineType = EImEndMakeFile;
			}
		}

	CleanupStack::PopAndDestroy(); // scriptTestUtils
	}

LOCAL_C void renameLogL(TTestData& aTestData, TFileName& aLogFile)
	{
	TFileName destPath = aTestData.iTestFile;

	// Make the destination file name.
	destPath.Delete(0,15);
	destPath.Delete(destPath.Length()-4,4);
	TFileName destPath1 = (_L("c:\\logs\\email\\Imlog25_"));
	destPath1.Append(destPath);
	destPath1.Append(_L(".txt"));
	
	aLogFile.Zero();
	aLogFile.Append(_L("Imlog25_"));
	aLogFile.Append(destPath);
	aLogFile.Append(_L(".txt"));

	RFs aFs;
	User::LeaveIfError(aFs.Connect());
	
	TInt err = aFs.Delete(destPath1);

	__ASSERT_ALWAYS((err == KErrNone) || (err == KErrNotFound), User::Panic(_L("smts_auth_test"),KErrGeneral));

	// Copy the file.
	CFileMan* fileMan = CFileMan::NewL(aFs);
	CleanupStack::PushL(fileMan);
	fileMan->Rename(KLogFilePath, destPath1);
	aFs.Close();
	CleanupStack::PopAndDestroy(fileMan);
	}

#endif
	
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
	CleanupStack::PopAndDestroy(testUtils);
	
	//explicitly close the message server so that a new instance of CEmailTestUtils can be created (waits for the server to close) 
	TMsvDummyObserver* dummyObserver = new (ELeave) TMsvDummyObserver;
	CMsvSession* msvSession = CMsvSession::OpenSyncL(*dummyObserver);
	msvSession->CloseMessageServer();
	delete msvSession;
	delete dummyObserver;
	
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);
	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;

	testUtils->WriteComment(_L("Smtp server created"));
	}

LOCAL_C void ResetSettingsL(TBool aSecureSocket, TBool aSmtpAuth)
	{
	_LIT(KSmtpEmailAlias, "SMTP AUTH");
	_LIT(KSmtpServer, "lon-msgtest06.intra");
	_LIT(KSmtpEmailAddress, "<smtp@lon-msgtest06.intra>");
	_LIT8(KSmtpUserName,	"smtp-bad");
	_LIT8(KSmtpPassword,	"password-bad"); 

	TPtrC8 userName(KSmtpUserName);
	TPtrC8 password(KSmtpPassword);

	CleanupStack::PopAndDestroy(testUtils);

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
	settings->SetSMTPAuth(aSmtpAuth);
	settings->SetLoginNameL(userName);
	settings->SetPasswordL(password);

	settings->SetPort(25);
	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);
	
	CleanupStack::PopAndDestroy(2, settings); //settings, store/accounts
	
	//explicitly close the message server so that a new instance of CEmailTestUtils can be created (waits for the server to close) 
	TMsvDummyObserver* dummyObserver = new (ELeave) TMsvDummyObserver;
	CMsvSession* msvSession = CMsvSession::OpenSyncL(*dummyObserver);
	msvSession->CloseMessageServer();
	delete msvSession;
	delete dummyObserver;
	
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);
	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;
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
	testUtils->FileSession().RmDir(KScriptFilePath);
	// Add a blank line to the Log file before the new Test
	_LIT(KBlankLine, "");
	testUtils->WriteComment(KBlankLine);

	TParse scriptFile;
	testUtils->ResolveFile(KSmtsComponent, aTestData.iTestFile, scriptFile);
	testUtils->CopyScriptFileL(scriptFile.FullName(), _L("25"));

//  For dial up tests, ucomment the below line
//	testUtils->DeleteScriptFilesFromLogsDirL();

	testUtils->TestStart(aTestData.iTestNumber);
	testUtils->WriteComment(aTestData.iTestDescription);
	testUtils->WriteComment(aTestData.iTestResult);
	
	// Run the Test & check the error
	if( TestSendingMessagesL() == KErrNone )
		{
#ifdef _DEBUG
		// Test was executed ok - check to see if result was correct
		TFileName logFile;
		renameLogL(testData, logFile);
		ParseLogFileL(logFile);
		
		if( !CompareFilesL(logFile) )
			testError = KErrNotFound;
#else
		// Cannot compare logs on REL builds, since the log file is not generated
		testUtils->WriteComment(_L("!!!! Logs comparison skipped on REL builds !!!!"));
		testUtils->WriteComment(_L("!!!! Run on DEB build for complete test    !!!!"));
#endif
		}
	}

LOCAL_C void RunAllTestsL()
	{
	
/*	//---------------------------------------------------
	//	Test 1 - TLS On, Smtp Auth On, No Smtp on Server
	//---------------------------------------------------
	ResetSettingsL(ETrue, ETrue);
	testData.iTestNumber = 1;
	testData.iTestFile = KTest1ScriptFile;
	testData.iTestDescription = KTest1Desc;
	testData.iTestResult = KTest1Result;
	RunTestL(testData);
	testError=(testError==-1)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);

	//CreateMsgToSendL(); 
	renameLogL(testData);

	//-----------------------------------------------------------------
	//	Test 2 - TLS on, SMTP Auth Off, Smtp on Server
	//-----------------------------------------------------------------
	ResetSettingsL(ETrue, EFalse);
	testData.iTestNumber = 2;
	testData.iTestFile = KTest2ScriptFile;
	testData.iTestDescription = KTest2Desc;
	testData.iTestResult = KTest2Result;
	RunTestL(testData);
	testError=(testError==-1)? 0 : testError;
	testUtils->TestFinish(testData.iTestNumber, testError);
	//CreateMsgToSendL(); 
	renameLogL(testData);
*/

	//------------------------------------------------------
	//	Test 3 - TLS on, SMTP Auth On, Login Mechanism used
	//------------------------------------------------------
	ResetSettingsL(ETrue, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 3;
	testData.iTestFile = KTest3ScriptFile;
	testData.iTestDescription = KTest3Desc;
	testData.iTestResult = KTest3Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);


	//---------------------------------------------
	//	Test 4 - TLS off, SMTP Auth On, Wrong UserName
	//---------------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 4;
	testData.iTestFile = KTest4ScriptFile;
	testData.iTestDescription = KTest4Desc;
	testData.iTestResult = KTest4Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);


	//--------------------------------------------------------------------------------------
	//	Test 5 - TLS off, SMTP Auth On, Wrong Password
	//--------------------------------------------------------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 5;
	testData.iTestFile = KTest5ScriptFile;
	testData.iTestDescription = KTest5Desc;
	testData.iTestResult = KTest5Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);


	//----------------------------------------
	//	Test 6 - TLS off, SMTP Auth On, Plain Mechanism used
	//----------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 6;
	testData.iTestFile = KTest6ScriptFile;
	testData.iTestDescription = KTest6Desc;
	testData.iTestResult = KTest6Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);
	
	//--------------------------------------------------------------------------
	//	Test 7 - TLS off, SMTP Auth On, Login Mechanism used
	//--------------------------------------------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();	
	testData.iTestNumber = 7;
	testData.iTestFile = KTest7ScriptFile;
	testData.iTestDescription = KTest7Desc;
	testData.iTestResult = KTest7Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);


	//--------------------------------------------------------------------------
	//	Test 8 - TLS off, SMTP Auth On, CRAM-MD5 Mechanism used
	//--------------------------------------------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 8;
	testData.iTestFile = KTest8ScriptFile;
	testData.iTestDescription = KTest8Desc;
	testData.iTestResult = KTest8Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);

	//----------------------------------------
	//	Test 9 - TLS off, SMTP Auth On, No SMTP on Server
	//----------------------------------------
	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 9;
	testData.iTestFile = KTest9ScriptFile;
	testData.iTestDescription = KTest9Desc;
	testData.iTestResult = KTest9Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);

	//---------------------------------------------------------------------
	//	Test 10 - SMTP Auth with Ehlo is not supported, Conitnue using Helo Command
	//---------------------------------------------------------------------

	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();

	testData.iTestNumber = 10;
	testData.iTestFile = KTest10ScriptFile;
	testData.iTestDescription = KTest10Desc;
	testData.iTestResult = KTest10Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);

	//---------------------------------------------------------------------
	//	Test 11 - SMTP Auth with Ehlo is not supported, Quit if Tls On
	//---------------------------------------------------------------------

	ResetSettingsL(ETrue, ETrue);
	CreateMsgToSendL();

	testData.iTestNumber = 11;
	testData.iTestFile = KTest11ScriptFile;
	testData.iTestDescription = KTest11Desc;
	testData.iTestResult = KTest11Result;
	RunTestL(testData);
	// This test is slightly different - an error code is expected.
	if( testError == KErrSmtpTLSNegotiateFailed )
		{
#ifdef _DEBUG
		testError = KErrNone;
		// Test was executed ok - check to see if result was correct
		TFileName logFile;
		renameLogL(testData, logFile);
		ParseLogFileL(logFile);
		
		if( !CompareFilesL(logFile) )
			testError = KErrNotFound;
#else
		// Cannot compare logs on REL builds, since the log file is not generated
		testUtils->WriteComment(_L("!!!! Logs comparison skipped on REL builds !!!!"));
		testUtils->WriteComment(_L("!!!! Run on DEB build for complete test    !!!!"));
#endif
		}
	else
		{
		if( testError == KErrNone )
			testError = KErrNotFound;	// it should have been an error! (Although unlikely to get here as log comparison will fail!)
		}		
	testUtils->TestFinish(testData.iTestNumber, testError);
	
	ClearOutboxL();
	
	//------------------------------------------------------
	//	Test 12 - TLS on, SMTP Auth On, CRAM-MD5 Mechanism used
	//------------------------------------------------------
	ResetSettingsL(ETrue, ETrue);
	CreateMsgToSendL();
	testData.iTestNumber = 12;
	testData.iTestFile = KTest12ScriptFile;
	testData.iTestDescription = KTest12Desc;
	testData.iTestResult = KTest12Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);

	//---------------------------------------------------------------------
	//	Test 13 - SMTP Auth with Ehlo is not supported, Conitnue with hostname and then HELO
	//---------------------------------------------------------------------

	ResetSettingsL(EFalse, ETrue);
	CreateMsgToSendL();

	testData.iTestNumber = 13;
	testData.iTestFile = KTest13ScriptFile;
	testData.iTestDescription = KTest13Desc;
	testData.iTestResult = KTest13Result;
	RunTestL(testData);
	testUtils->TestFinish(testData.iTestNumber, testError);

	// Delete the script file generated in C:\logs\email directory
	testUtils->DeleteScriptFilesFromLogsDirL();
	}

LOCAL_C void doMainL()
	{
	InitL();
	
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);

	testUtils->iServerEntry->SetEntry(smtpService);

	ResetSettingsL(ETrue,ETrue);
	testUtils->DeleteScriptFilesFromLogsDirL();
	RunAllTestsL();

	// End of the Tests
	ClosedownL();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTSSMTPAUTH Test SMTP server MTM - SMTP AUTH"));
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
