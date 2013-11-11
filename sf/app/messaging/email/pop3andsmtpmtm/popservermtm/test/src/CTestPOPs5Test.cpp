
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
// Tests a wide range of POPS functionality
// Detailed description of test harness:
// Implements a range of tests for POPS testing the following
// funtionality:
// moving, copying, deleting and populating messages
// downloading mail with badly formed headers
// copying, moving and populating messages that are already populated
// online operations
// offline operations
// 
//

#include "CTestPOPs5Test.h"
#include <iapprefs.h>


#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT(KPopsComponent,"T_POPs5");
_LIT(KPopTeststart, "T_POPs5 - START POPS Functionality Testing");
_LIT(KPopTestend, "T_POPs5 - END POPS Functionality Testing");


_LIT(KPOPServer,"msexchange01.closedtest.intra");
_LIT8(KPOPInValidUserPass, "popfun");

RTest test(KPopsComponent);


CTestPOPs5Test::CTestPOPs5Test()
	{
	}

CTestPOPs5Test ::~CTestPOPs5Test()
	{
	}
	
void CTestPOPs5Test::SetupL()
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

	isettings->SetServerAddressL(KPOPServer);
	isettings->SetLoginNameL(KPOPInValidUserPass /*iTestUtils->MachineName()*/);
	isettings->SetPasswordL(KPOPInValidUserPass /*iTestUtils->MachineName()*/);
	isettings->SetPort(110);


	account->SavePopSettingsL(pop3AccountId, *isettings);
	CleanupStack::PopAndDestroy(2, account);
	
	iTestUtils->GoServerSideL();
	iTestUtils->InstantiatePopServerMtmL();
	iPopServerMtm=iTestUtils->iPopServerMtm;
	}

void CTestPOPs5Test::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	ipop3Service = NULL;
	_LIT(KFunction1, "After NULL");
	INFO_PRINTF1(KFunction1);
	delete isettings;
	_LIT(KFunction2, "After iSettings");
	INFO_PRINTF1(KFunction2);
	delete iActiveWaiter;
	_LIT(KFunction3, "After iActiveWaiter");
	INFO_PRINTF1(KFunction3);
	delete iTestUtils;
	_LIT(KFunction4, "After iTestUtils");
	INFO_PRINTF1(KFunction4);
	iTestUtils = NULL;
	_LIT(KFunction5, "After iTestUtils NULL");
	INFO_PRINTF1(KFunction5);
	INFO_PRINTF1(KPopTestend);
	}


void  CTestPOPs5Test::TestRunPops5CopyToLocalLTestsL()
	{
	//-------------------------------
	//	Test 1 void CopyToLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	
	ConnectToRemoteServerL();
	
	_LIT(KFunction1, "TestCopyToLocalL");
	INFO_PRINTF1(KFunction1);
	
	CMsvEntrySelection* aSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection);
	aSelection->AppendL(ipop3Service);
	
	iTestUtils->iServerEntry->SetEntry(ipop3Service);
	iTestUtils->iServerEntry->GetChildren(*aSelection);
	
	TRAPD(err1, iPopServerMtm->CopyToLocalL(*aSelection,ipop3Service,iActiveWaiter->iStatus));
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS( err1, KErrNone );


	//-------------------------------
	//	Test 2 void MoveToLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	
	_LIT(KFunction2, "TestMoveToLocalL");
	INFO_PRINTF1(KFunction2);

	CMsvEntrySelection* aSelection1 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection1);
	aSelection1->AppendL(ipop3Service);
	
	iTestUtils->iServerEntry->SetEntry(ipop3Service);
	iTestUtils->iServerEntry->GetChildren(*aSelection1);
	
	TRAPD(err11, iPopServerMtm->MoveToLocalL(*aSelection1,ipop3Service,iActiveWaiter->iStatus));
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS( err11, KErrNone );

	//-------------------------------
	//	Test 3 void CopyFromLocalL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	
	_LIT(KFunction3, "TestCopyFromLocalL");
	INFO_PRINTF1(KFunction3);

	CMsvEntrySelection* aSelection21 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection21);
	TRAPD(err21, iPopServerMtm->CopyFromLocalL(*aSelection21,KMsvGlobalInBoxIndexEntryId,iActiveWaiter->iStatus));
	ASSERT_EQUALS( err21, KErrNotSupported );


	//-------------------------------
	//	Test 4 void DeleteAllL(const CMsvEntrySelection& aSelection, TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------

	_LIT(KFunction4, "TestDeleteAllL");
	INFO_PRINTF1(KFunction4);

	
	CMsvEntrySelection* aSelection12 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection12);
	aSelection12->AppendL(ipop3Service);
	
	iTestUtils->iServerEntry->SetEntry(ipop3Service);
	iTestUtils->iServerEntry->GetChildren(*aSelection12);
	
	TRAPD(err12, iPopServerMtm->DeleteAllL(*aSelection12,iActiveWaiter->iStatus));
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS( err12, KErrNone );


	//-------------------------------
	//	Test 5 void CopyWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	
	_LIT(KFunction5, "TestCopyWithinServiceL");
	INFO_PRINTF1(KFunction5);

	CMsvEntrySelection* aSelection13 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection13);
	TRAPD(err13, iPopServerMtm->CopyWithinServiceL(*aSelection13,KMsvGlobalInBoxIndexEntryId,iActiveWaiter->iStatus));
	ASSERT_EQUALS( err13, KErrNotSupported );



	//-------------------------------
	//	Test 6 void void MoveFromLocalL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	
	_LIT(KFunction6, "TestMoveFromLocalL");
	INFO_PRINTF1(KFunction6);

	CMsvEntrySelection* aSelection14 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection14);
	TRAPD(err14, iPopServerMtm->MoveFromLocalL(*aSelection14,KMsvGlobalInBoxIndexEntryId,iActiveWaiter->iStatus));
	ASSERT_EQUALS( err14, KErrNotSupported );



	//-------------------------------
	//	Test 7 void MoveWithinServiceL(const CMsvEntrySelection& aSelection,TMsvId aDestination, TRequestStatus& aStatus);
	//-------------------------------
	_LIT(KFunction7, "TestMoveWithinServiceL");
	INFO_PRINTF1(KFunction7);

	CMsvEntrySelection* aSelection15 = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(aSelection15);
	TRAPD(err15, iPopServerMtm->MoveWithinServiceL(*aSelection15,KMsvGlobalInBoxIndexEntryId,iActiveWaiter->iStatus));
	ASSERT_EQUALS( err15, KErrNotSupported );

	
	//-------------------------------
	//	Test 8 void CreateL(TMsvEntry aNewEntry, TRequestStatus& aStatus);
	//-------------------------------
	
	_LIT(KFunction8, "TestCreateL");
	INFO_PRINTF1(KFunction8);

	TMsvEntry aNewEntry = iTestUtils->iServerEntry->Entry();
	
	TRAPD(err16, iPopServerMtm->CreateL(aNewEntry,iActiveWaiter->iStatus));
	ASSERT_EQUALS( err16, KErrNotSupported );

	DisconnectL(); 
	CleanupStack::PopAndDestroy(7,aSelection); // aSelection
	}
	
//Connect to tyhe Server 
void CTestPOPs5Test::ConnectToRemoteServerL()
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
	ASSERT_EQUALS( progress.iErrorCode, KErrNone );
	}
//Disconnecting the Connection
void CTestPOPs5Test::DisconnectL()
	{
	TBuf8<128> parameter;
	iSel->AppendL(ipop3Service);
	iPopServerMtm->StartCommandL(*iSel,KPOP3MTMDisconnect , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}
	
CTestSuite* CTestPOPs5Test::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestRunPops5CopyToLocalLTestsL);
	END_SUITE;
	}

	
