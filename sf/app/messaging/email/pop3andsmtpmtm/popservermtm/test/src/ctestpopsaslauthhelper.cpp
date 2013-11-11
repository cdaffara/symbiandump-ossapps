// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctestpopsaslauthhelper.h"

#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT(KPopsComponent,"T_POPAuthCRAM-MD5Support");

_LIT(KPOPServer,"msexchange01.closedtest.intra");
_LIT8(KPOPUsernamePasswd, "pop001");

RTest testPop3Auth(KPopsComponent);


CTestPopSaslAuthHelper::CTestPopSaslAuthHelper()
	{
	}

CTestPopSaslAuthHelper ::~CTestPopSaslAuthHelper()
	{
	}
	
void CTestPopSaslAuthHelper::SetupL()
	{
	_LIT(KFunction, "SetupL");
	INFO_PRINTF1(KFunction);
	
	iResponseBuffer = HBufC8::NewL(KImMailMaxBufferSize);
	}

void CTestPopSaslAuthHelper::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	
	delete iResponseBuffer;
	delete iPopSettings;
	delete iActiveWaiter;
	delete iTestUtils;
	}
	
	
void CTestPopSaslAuthHelper::CreatePopSaslAuthMechanismL()
	{
	CreatePopTestAccountL();
	ConnectToRemoteServerL();
	TestForAuthCramMd5L();
	TestForAuthPlainL();
	TestForAuthLoginL();
	DisconnectL();
	}

void CTestPopSaslAuthHelper::CreatePopTestAccountL()
	{
	_LIT(KFunction, "CreatePopTestAccountL");
	INFO_PRINTF1(KFunction);
	
	TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	
	iActiveWaiter = new(ELeave)CActiveWaiter();
	
	iTestUtils = CEmailTestUtils::NewL(testPop3Auth);
	iTestUtils->FileSession().SetSessionPath( _L( "C:\\" ) );
	iTestUtils->ClearEmailAccountsL();
	iTestUtils->CreateAllTestDirectories();
	iTestUtils->CleanMessageFolderL();
	
	iTestUtils->GoClientSideL();	

	if(!iPop3Service)
		{
		iPop3Service = iTestUtils->CreatePopServiceL();	
		}
		
	iEmailAccount = CEmailAccounts::NewLC();
	iPopSettings = new(ELeave) CImPop3Settings();

	CImIAPPreferences* imapIAP = CImIAPPreferences::NewLC();
	iEntrySelection = new (ELeave) CMsvEntrySelection;

	
	iEmailAccount->GetPopAccountL(iPop3Service, iPop3AccountId);
  	iEmailAccount->LoadPopSettingsL(iPop3AccountId, *iPopSettings);

	iPopSettings->SetServerAddressL(KPOPServer);
	iPopSettings->SetLoginNameL(KPOPUsernamePasswd /*iTestUtils->MachineName()*/);
	iPopSettings->SetPasswordL(KPOPUsernamePasswd /*iTestUtils->MachineName()*/);
	iPopSettings->SetPort(110);
	iPopSettings->SetPOP3Auth(ETrue);
	iPopSettings->SetFallBack(ETrue);

	iEmailAccount->SavePopSettingsL(iPop3AccountId, *iPopSettings);
	CleanupStack::PopAndDestroy(2, iEmailAccount);
	
	iTestUtils->GoServerSideL();
	iTestUtils->InstantiatePopServerMtmL();
	iPopServerMtm = iTestUtils->iPopServerMtm;
	}
	

//Connect to Remote Server 
void CTestPopSaslAuthHelper::ConnectToRemoteServerL()
	{
	_LIT(KFunction, "ConnectToRemoteServerL");
	INFO_PRINTF1(KFunction);
	
	TBuf8<128> parameter;
	iEntrySelection->AppendL(iPop3Service);
	TPop3Progress temp;	
	TPckgC<TPop3Progress> paramPack(temp);
	
	iPopServerMtm->StartCommandL(*iEntrySelection, KPOP3MTMConnect, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	const TDesC8& progBuf = iTestUtils->iPopServerMtm->Progress();	
	paramPack.Set(progBuf);
	TPop3Progress progress=paramPack();	
	
	ASSERT_EQUALS( progress.iErrorCode, KErrNone );
	}
	
//Disconnecting from Remote Server
void CTestPopSaslAuthHelper::DisconnectL()
	{
	_LIT(KFunction, "DisconnectL");
	INFO_PRINTF1(KFunction);
	
	TBuf8<128> parameter;
	iEntrySelection->AppendL(iPop3Service);
	iPopServerMtm->StartCommandL(*iEntrySelection,KPOP3MTMDisconnect , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}

void CTestPopSaslAuthHelper::TestForAuthCramMd5L()
	{
	// For CRAM-MD5
	_LIT(KFunction, "TestForAuthCramMd5L");
	INFO_PRINTF1(KFunction);

	iTestPopAuthMechanism = CPopAuthCramMd5MechanismHelper::NewL(*iPopSettings);
	CleanupStack::PushL(iTestPopAuthMechanism);
	TPtr8 popBuffer = iResponseBuffer->Des();
	
	//popBuffer will contain "AUTH CRAM-MD5"
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	//INFO_PRINTF1(popBuffer);
			
	//set dummy server response
	_LIT(KServerResponse,"+ PDE4LjEyMjYzOTQ0MDZARDA3MDAzMS5wcm9kLmFkLnN5bWJpYW4uaW50cmE+");
	popBuffer.Append(KServerResponse);
	
	iTestPopAuthMechanism->SetLastServerMessageL(popBuffer, ETrue);
		
	//popBuffer will contain CRAM-MD5 authentication string
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	
	//check the length of response buffer 
	TInt error = (popBuffer.Length() > 0 ) ? KErrNone : KErrArgument;
	ASSERT_EQUALS(error, KErrNone);	
	
	CleanupStack::PopAndDestroy(iTestPopAuthMechanism);
	}

void CTestPopSaslAuthHelper::TestForAuthPlainL()
	{
	// For PLAIN
	_LIT(KFunction, "TestForAuthPlainL");
	INFO_PRINTF1(KFunction);
	
	iTestPopAuthMechanism = CPopAuthPlainMechanismHelper::NewL(*iPopSettings);
	CleanupStack::PushL(iTestPopAuthMechanism);
	
	TPtr8 popBuffer = iResponseBuffer->Des();
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	
	//check the length of response buffer 
	TInt error = (popBuffer.Length() > 0 ) ? KErrNone : KErrArgument;
	ASSERT_EQUALS(error, KErrNone);	
		
	CleanupStack::PopAndDestroy(iTestPopAuthMechanism);
	}

void CTestPopSaslAuthHelper::TestForAuthLoginL()
	{
	// For LOGIN
	_LIT(KFunction, "TestForAuthLoginL");
	INFO_PRINTF1(KFunction);
	
	iTestPopAuthMechanism = CPopAuthLoginMechanismHelper::NewL(*iPopSettings);
	CleanupStack::PushL(iTestPopAuthMechanism);
	
	TPtr8 popBuffer = iResponseBuffer->Des();
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	
	//set dummy server response for Username
	popBuffer.Zero();
	_LIT(KServerResponseForUsername,"+ VXNlcm5hbWU6");
	popBuffer.Append(KServerResponseForUsername);
	iTestPopAuthMechanism->SetLastServerMessageL(popBuffer, ETrue);
	
	//popBuffer will contain Username in base64 encoded format
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	
	//check the length of response buffer 
	TInt error = (popBuffer.Length() > 0 ) ? KErrNone : KErrArgument;
	ASSERT_EQUALS(error, KErrNone);
	
	//set dummy server response for Password
	popBuffer.Zero();
	_LIT(KServerResponseForPasswd,"+ UGFzc3dvcmQ6");
	popBuffer.Append(KServerResponseForPasswd);
	iTestPopAuthMechanism->SetLastServerMessageL(popBuffer, ETrue);
	
	//popBuffer will contain Password in base64 encoded format
	iTestPopAuthMechanism->GetNextClientMessageL(popBuffer);
	
	//check the length of response buffer 
	error = (popBuffer.Length() > 0 ) ? KErrNone : KErrArgument;
	ASSERT_EQUALS(error, KErrNone);
	
	CleanupStack::PopAndDestroy(iTestPopAuthMechanism);	
	}


CTestSuite* CTestPopSaslAuthHelper::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(CreatePopSaslAuthMechanismL);
	END_SUITE;
	}

