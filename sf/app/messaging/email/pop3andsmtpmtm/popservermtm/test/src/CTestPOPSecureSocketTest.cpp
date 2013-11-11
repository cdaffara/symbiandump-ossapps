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
// Brief description of test harness:
// Tests SSL/TLS functionality in POPS
// 
//


#include "CTestPOPSecureSocketTest.h"
#include <iapprefs.h>
#include <f32file.h>

//Literals used
_LIT(KFileName,"C:\\msgtest\\pops\\scripts\\MachineIP.txt");

#define KMaxIpvalue 100

#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT(KPopsComponent,"T_POPSECURESOCKETS");
_LIT(KPopTeststart, "T_POPSECURESOCKETS - START POPS SecureSockets Functionality Testing");
_LIT(KPopTestend, "T_POPSECURESOCKETS - END POPS SecureSockets Functionality Testing");

_LIT8(KPOPtestUserPass, "poptest");
_LIT8(KPOPInValidUserPass, "InvaliedUserPass");
_LIT8(KServerDomainname, "stalker.com");


RTest test(KPopsComponent);


CTestPOPSecureSocketTest::CTestPOPSecureSocketTest()
	{
	}

CTestPOPSecureSocketTest ::~CTestPOPSecureSocketTest()
	{
	}
/*
// Read the Ip from the MachineIP.txt  config file.
// And connect the server .
*/
TPtr16 CTestPOPSecureSocketTest::ReadServerIp()
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	RFile file;	
	User::LeaveIfError(file.Open(fs, KFileName, EFileRead));
	CleanupClosePushL(file);
	TInt  fileSize;
	User::LeaveIfError(file.Size(fileSize));
	HBufC8* iServerIp = HBufC8::NewL(fileSize);
	TPtr8 ibufferPtr = iServerIp->Des();;
	User::LeaveIfError(file.Read(ibufferPtr));
	HBufC16* a16Value = HBufC16::NewL(KMaxIpvalue);
	TPtr16 i16bufferPtr = a16Value->Des();
	i16bufferPtr.Copy(ibufferPtr);
	CleanupStack::PopAndDestroy(2);
	file.Close();
	return i16bufferPtr;
	}
	
void CTestPOPSecureSocketTest::SetupL()
	{
	_LIT(KFunction, "SetupL");
	INFO_PRINTF1(KFunction);
	INFO_PRINTF1(KPopTeststart);
		TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	
	iActiveWaiter = new(ELeave)CActiveWaiter();
	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->FileSession().SetSessionPath( _L( "C:\\" ) );
	iTestUtils->ClearEmailAccountsL();
	iTestUtils->CreateAllTestDirectories();
	iTestUtils->CleanMessageFolderL();
	}

void CTestPOPSecureSocketTest::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	delete iTestUtils;
	iTestUtils = NULL;
	INFO_PRINTF1(KPopTestend);
	}

void CTestPOPSecureSocketTest::TestRunPopSSLTLSTestsL()
	{
	_LIT(KFunction1, "TestRunNonApopTestsL");
	INFO_PRINTF1(KFunction1);

	TPtr16 i16testbufferPtr = ReadServerIp();	
	//-------------------------------
	//	Test 1 (Support SSl-TLS)
	//-------------------------------
	TypeOfTest aType1(EpopApopTLSok);
	SetSettingsL(aType1,i16testbufferPtr);
	ConnectToRemoteServerL(aType1);
	DisconnectL(); 
	
	//-------------------------------
	//	Test 2 (Invalid User)
	//-------------------------------

	TypeOfTest aType2(EpopTlsInvalidUser);
	SetSettingsL(aType2,i16testbufferPtr);
	ConnectToRemoteServerL(aType2);
	DisconnectL(); 

	}

// Setting up the POP3 Setting in SSL	
void CTestPOPSecureSocketTest::SetSettingsL(TypeOfTest aType,TPtr16 aServerIp)
	{
	iTestUtils->GoClientSideL();	

	if(!ipop3Service)
		ipop3Service = iTestUtils->CreatePopServiceL();	


	CEmailAccounts* account = CEmailAccounts::NewLC();
	isettings = new(ELeave) CImPop3Settings();

	
	CImIAPPreferences* imapIAP = CImIAPPreferences::NewLC();
	iSel = new (ELeave) CMsvEntrySelection;

	TPopAccount pop3AccountId;
	account->GetPopAccountL(ipop3Service, pop3AccountId);
  	account->LoadPopSettingsL(pop3AccountId, *isettings);
	
	switch(aType)
		{
			case EpopApopTLSok :
				 	isettings->SetServerAddressL(aServerIp );
					isettings->SetLoginNameL(KPOPtestUserPass);
					isettings->SetPasswordL(KPOPtestUserPass );
					isettings->SetSecureSockets(ETrue);
					isettings->SetSSLWrapper(ETrue);
					isettings->SetPort(995);//SSL/TLS POP3 port 995 . by default it is 110.
					isettings->SetTlsSslDomainL(KServerDomainname());

				 break;
				 
			case EpopTlsInvalidUser :
					isettings->SetServerAddressL(aServerIp);
					isettings->SetLoginNameL(KPOPInValidUserPass);
					isettings->SetPasswordL(KPOPInValidUserPass);
					isettings->SetSecureSockets(ETrue);
					isettings->SetSSLWrapper(ETrue);
					isettings->SetPort(995);//SSL/TLS POP3 port 995 . by default it is 110.
					isettings->SetTlsSslDomainL(KServerDomainname());
				 break;
		};

	account->SavePopSettingsL(pop3AccountId, *isettings);
	CleanupStack::PopAndDestroy(2, account);
	
	iTestUtils->GoServerSideL();
	iTestUtils->InstantiatePopServerMtmL();
	iPopServerMtm=iTestUtils->iPopServerMtm;
	}

//Connect to tyhe Server 
void CTestPOPSecureSocketTest::ConnectToRemoteServerL(TypeOfTest aType)
	{
	TBuf8<128> parameter;
	iSel->AppendL(ipop3Service);
	TPop3Progress temp;	
	TPckgC<TPop3Progress> paramPack(temp);
	iPopServerMtm->StartCommandL(*iSel, KPOP3MTMConnect, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	const TDesC8& progBuf = iTestUtils->iPopServerMtm->Progress();	
	paramPack.Set(progBuf);
	TPop3Progress progress=paramPack();	
		switch(aType)
		{
			case EpopApopTLSok :
				ASSERT_EQUALS( progress.iErrorCode, KErrNone|KErrAbort );
				break;
				
			case EpopTlsInvalidUser :
				{
				if(progress.iErrorCode == KErrAbort )
					{
					ASSERT_EQUALS( progress.iErrorCode, KErrAbort );
					}
				else
					{
					ASSERT_EQUALS( progress.iErrorCode, -172 );
					}
				}
				break;
		};
	}

//Disconnecting the Connection
void CTestPOPSecureSocketTest::DisconnectL()
	{
	TBuf8<128> parameter;
	iSel->AppendL(ipop3Service);
	iPopServerMtm->StartCommandL(*iSel,KPOP3MTMDisconnect , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}

	
CTestSuite* CTestPOPSecureSocketTest::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestRunPopSSLTLSTestsL);
	END_SUITE;
	}

