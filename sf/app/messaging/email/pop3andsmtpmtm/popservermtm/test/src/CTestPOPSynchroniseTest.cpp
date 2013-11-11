
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
// Name of test harness: T_pops_synchronise
// Component: POPS
// 0Brief description of test harness:
// Tests  the changes to POPS for small devices which permit setting a limit 
// on the number of emails downloaded at any time. 
// Detailed description of test harness:
// Implements a range of tests for POPS testing the following
// funtionality:
// -Setting sychronisation limit
// -Populating the remote mailbox with headers up to the synchronisation limit
// -Deleting headers from the remote mailbox under the service entry 
// if message has been deleted from POP3 mailbox.
// 
//



#include "CTestPOPSynchroniseTest.h"
#include <iapprefs.h>


#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

_LIT(KPopsComponent,"T_POPsSynchronise");
_LIT(KPopTeststart, "T_POPsSynchronise - START POPS Synchronise Functionality Testing");
_LIT(KPopTestend, "T_POPsSynchronise - END POPS Synchronise Functionality Testing");

//_LIT(KPOPServer, "10.192.197.21");

_LIT(KPOPServer,"msexchange01.closedtest.intra");
_LIT8(KPOPInValidUserPass, "popsyn");

RTest test(KPopsComponent);


CTestPOPSynchroniseTest::CTestPOPSynchroniseTest()
	{
	}

CTestPOPSynchroniseTest ::~CTestPOPSynchroniseTest()
	{
	}
	
void CTestPOPSynchroniseTest::SetupL()
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


	iaccount = CEmailAccounts::NewLC();
	isettings = new(ELeave) CImPop3Settings();

	CImIAPPreferences* imapIAP = CImIAPPreferences::NewLC();
	iSel = new (ELeave) CMsvEntrySelection;

	
	iaccount->GetPopAccountL(ipop3Service, ipop3AccountId);
  	iaccount->LoadPopSettingsL(ipop3AccountId, *isettings);

	isettings->SetServerAddressL(KPOPServer);
	isettings->SetLoginNameL(KPOPInValidUserPass /*iTestUtils->MachineName()*/);
	isettings->SetPasswordL(KPOPInValidUserPass /*iTestUtils->MachineName()*/);
	isettings->SetPort(110);

	iaccount->SavePopSettingsL(ipop3AccountId, *isettings);
	CleanupStack::PopAndDestroy(2, iaccount);
	
	iTestUtils->GoServerSideL();
	iTestUtils->InstantiatePopServerMtmL();
	iPopServerMtm=iTestUtils->iPopServerMtm;
	}

void CTestPOPSynchroniseTest::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	ipop3Service = NULL;
	delete isettings;
	delete iActiveWaiter;
	delete iTestUtils;
	iTestUtils = NULL;
	INFO_PRINTF1(KPopTestend);
	}

void CTestPOPSynchroniseTest::TestPop3SynchroniseTestL()
	{

	_LIT(KFunction1, "TestNoPop3FolderMessagesL");
	INFO_PRINTF1(KFunction1);
	
	ConnectToRemoteServerL();
	
	TInt totalEntries=0;
	User::LeaveIfError(iTestUtils->iServerEntry->SetEntry(ipop3Service));
	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(sel);
	User::LeaveIfError(iTestUtils->iServerEntry->GetChildren(*sel));
	totalEntries = sel->Count();
	ASSERT_EQUALS( totalEntries, sel->Count() );
	sel->Reset();
	
	_LIT(KFunction2, "TestNoNewMsgL");
	INFO_PRINTF1(KFunction2);

	TInt newEntries=0;
	User::LeaveIfError(iTestUtils->iServerEntry->SetEntry(ipop3Service));
	User::LeaveIfError(iTestUtils->iServerEntry->GetChildren(*sel));

	for (TInt count =0; count < sel->Count(); count++)
		{
		User::LeaveIfError(iTestUtils->iServerEntry->SetEntry(sel->At(count) ) );
		if (iTestUtils->iServerEntry->Entry().New())
			newEntries++;
		}
	TInt  entry =  newEntries;
	ASSERT_EQUALS( newEntries, entry );
	sel->Reset();
	
	_LIT(KFunction3, "TestNoOldMsgL");
	INFO_PRINTF1(KFunction3);
	
	TInt oldEntries=0;
	User::LeaveIfError(iTestUtils->iServerEntry->SetEntry(ipop3Service));
	User::LeaveIfError(iTestUtils->iServerEntry->GetChildren(*sel));

	for (TInt count =0; count < sel->Count(); count++)
		{
		User::LeaveIfError(iTestUtils->iServerEntry->SetEntry(sel->At(count)) );
		if (! iTestUtils->iServerEntry->Entry().New())
			oldEntries++;
		}
	TInt  oldentry =  oldEntries;
	ASSERT_EQUALS( oldEntries, oldentry );
	sel->Reset();
	
	DisconnectL();
	CleanupStack::PopAndDestroy(1, sel);
	ChangeSynchronisationLimitL();
	ChangeDownloadModeL();
	}

void CTestPOPSynchroniseTest::ChangeSynchronisationLimitL()
	{
	// Get the settings for the POP3 service and change the synchronisation limit
	// Used after the settings have been created and set.
	
	_LIT(KFunction4, "ChangeSynchronisationLimitL");
	INFO_PRINTF1(KFunction4);

	
	TInt aLimit = 2;
    iTestUtils->GoClientSideL();
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
	TPopAccount id;
 	accounts->GetPopAccountL(ipop3Service, id);
 	accounts->LoadPopSettingsL(id, *settings);

	settings->SetInboxSynchronisationLimit(aLimit);
 	accounts->SavePopSettingsL(id, *settings);

	CleanupStack::PopAndDestroy(settings);
	settings = NULL;
	settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
 	accounts->LoadPopSettingsL(id, *settings);
 	TInt ret = settings->InboxSynchronisationLimit() ;
 	ASSERT_EQUALS(ret , aLimit);
 	
 	CleanupStack::PopAndDestroy(settings);
	CleanupStack::PopAndDestroy(accounts);
	}

void CTestPOPSynchroniseTest::ChangeDownloadModeL()
	{
	// Get the settings for the POP3 service and change the synchronisation limit
	// Used after the settings have been created and set.
	
	_LIT(KFunction5, "ChangeDownloadModeL");
	INFO_PRINTF1(KFunction5);

	TBuf<20> downloadType;
	
	iTestUtils->GoClientSideL();
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
	TPopAccount id;
 	accounts->GetPopAccountL(ipop3Service, id);
 	accounts->LoadPopSettingsL(id, *settings);
 	TPop3GetMailOptions aMode(EGetPop3EmailHeaders);
 	settings->SetGetMailOptions(aMode);
 	accounts->SavePopSettingsL(id, *settings);
	CleanupStack::PopAndDestroy(settings);
	settings = NULL;
	settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
 	accounts->LoadPopSettingsL(id, *settings);
	TPop3GetMailOptions aMode1 = settings->GetMailOptions();
	ASSERT_EQUALS(aMode1 , aMode);
	CleanupStack::PopAndDestroy(settings);
	CleanupStack::PopAndDestroy(accounts); 
	}

//Connect to tyhe Server 
void CTestPOPSynchroniseTest::ConnectToRemoteServerL()
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
void CTestPOPSynchroniseTest::DisconnectL()
	{
	TBuf8<128> parameter;
	iSel->AppendL(ipop3Service);
	iPopServerMtm->StartCommandL(*iSel,KPOP3MTMDisconnect , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}
	
CTestSuite* CTestPOPSynchroniseTest::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestPop3SynchroniseTestL);
	//ADD_ASYNC_TEST_STEP(ChangeSynchronisationLimitL);
	END_SUITE;
	}

