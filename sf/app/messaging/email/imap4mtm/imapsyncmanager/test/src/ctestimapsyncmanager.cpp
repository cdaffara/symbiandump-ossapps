// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctestimapsyncmanager.h"

#include "cactivewaiter.h"
#include "moutputstream.h"
#include "cimapsettings.h"

#define KSyncLimit 10

_LIT(KImapSyncManagerTest,"test");
RTest test(KImapSyncManagerTest);

_LIT(KImapServer, "msexchange2k.closedtest.intra");

#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif


CTestImapSyncManager::CTestImapSyncManager()
: 	iActiveWaiter(NULL),
	iTestUtils( NULL )
	{}

CTestImapSyncManager::~CTestImapSyncManager()
	{
	delete iSel;
	delete iImapSettings1;
	delete iImapSettings;
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	delete iTestUtils;
	iTestUtils = NULL;
	}

void CTestImapSyncManager::SetupL()
	{
	TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);

	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->ClearEmailAccountsL( );
	iTestUtils->CreateAllTestDirectories( );
	iTestUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );
	
	iActiveWaiter = new(ELeave)CActiveWaiter();
	iTestUtils->GoServerSideL();
	iTestUtils->DeleteImapServiceL();

	iTestUtils->GoClientSideL();
	CEmailAccounts* account = CEmailAccounts::NewLC();
	iImapSettings1 = new(ELeave) CImImap4Settings();

	CImIAPPreferences* imapIAP = CImIAPPreferences::NewLC();
	account->PopulateDefaultImapSettingsL(*iImapSettings1, *imapIAP);
	
	iImapSettings1->SetServerAddressL(KImapServer);
	iImapSettings1->SetLoginNameL(iTestUtils->MachineName());
	iImapSettings1->SetPasswordL(iTestUtils->MachineName());
	iImapSettings1->SetPort(143);
	iImapSettings1->SetImapIdle(ETrue);
	iImapSettings1->SetSubscribe(EUpdateRemote);
	iImapSettings1->SetSynchronise(EUseRemote);
	iImapSettings1->SetInboxSynchronisationLimit(KSyncLimit);
	iImapSettings1->SetMailboxSynchronisationLimit(KSyncLimit);
	
	_LIT(KTxtAccountName, "testAccount");
	iImapAccount = account->CreateImapAccountL(KTxtAccountName, *iImapSettings1, *imapIAP, EFalse);
	
	CleanupStack::PopAndDestroy(2, account); // imapIAP, account

	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;

	iServiceId = iImapAccount.iImapService;
	
	iTestUtils->InstantiateImapServerMtmL();
	
	iImapServerMtm=(CImap4ServerMtm*)iTestUtils->iImapServerMtm;		
	iEntry = iTestUtils->iServerEntry;		
	
	iImapSettings = CImapSettings::NewL(*iEntry);
	iImapSettings->LoadSettingsL(iServiceId);
	iSel = new (ELeave) CMsvEntrySelection;
}
	

void CTestImapSyncManager::ChangeSyncLimitL(TInt limit)
	{
	CEmailAccounts* newaccount = CEmailAccounts::NewLC();
	iTestUtils->GoClientSideL();
	
	newaccount->GetImapAccountL(iServiceId, iImapAccount);
	newaccount->LoadImapSettingsL(iImapAccount, *iImapSettings1);
	
	iImapSettings1->SetInboxSynchronisationLimit(limit);
	iImapSettings1->SetMailboxSynchronisationLimit(limit);
	newaccount->SaveImapSettingsL(iImapAccount, *iImapSettings1);
	CleanupStack::PopAndDestroy(newaccount); 
	iTestUtils->GoServerSideL();
	
	iTestUtils->InstantiateImapServerMtmL();

	iImapServerMtm=(CImap4ServerMtm*)iTestUtils->iImapServerMtm;		

	iEntry = iTestUtils->iServerEntry;
	
	// load the new impsettings...
	iImapSettings = CImapSettings::NewL(*iEntry);
	iImapSettings->LoadSettingsL(iServiceId);
	delete iSel;
	iSel = new (ELeave) CMsvEntrySelection;
	}
	
void CTestImapSyncManager::ConnectToRemoteServerL()
	{
	TBuf8<128> parameter;
	iSel->AppendL(iServiceId);
	
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMConnect, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}

void CTestImapSyncManager::DisconnectL()
	{
	TBuf8<128> parameter;
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMDisconnect, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();	
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}
	
	
void CTestImapSyncManager::CreateFolderL( const TDesC& aName,TMsvId iEntry1)
	{
	iEntry->SetEntry(iEntry1);
	TMsvEmailEntry newEntry;
	newEntry.iType=KUidMsvFolderEntry;
	newEntry.iMtm=iEntry->Entry().iMtm;
	newEntry.iServiceId=iServiceId;
	newEntry.iSize=0;
	newEntry.iDetails.Set(aName);
	newEntry.SetMailbox(ETrue);
	newEntry.SetValidUID(EFalse);
	newEntry.SetComplete(ETrue);
	newEntry.SetVisible(ETrue);
	User::LeaveIfError(iEntry->CreateEntry(newEntry));
	iImapServerMtm->CreateL(newEntry,iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	}
	
void CTestImapSyncManager::DeleteFolderL(const TDesC& aName)
	{
	TBuf8<128> parameter;
	TMsvId id = FindFolderL(aName);
	// delete
	CMsvEntrySelection* msvEntrySelection = new (ELeave) CMsvEntrySelection;
	msvEntrySelection->AppendL(id);
	iImapServerMtm->DeleteAllL(*msvEntrySelection,iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	delete msvEntrySelection;
	}



TMsvId CTestImapSyncManager::FindFolderL(const TDesC& aName, TBool aCaseSensitive)
	{
	TMsvId remoteId = KErrNotFound;
	iEntry->SetEntry(iServiceId);
	CMsvEntrySelection* msvEntrySelection = new (ELeave) CMsvEntrySelection;
	iEntry->GetChildren(*msvEntrySelection);
	TPtrC iEntryName = aName;
	TInt count = msvEntrySelection->Count();
	for (TInt i=0; i<count; i++) 
		{	
		iEntry->SetEntry((*msvEntrySelection)[i]);
		
		if (aCaseSensitive)
			{
			if (iEntryName.Compare(iEntry->Entry().iDetails ) == 0)
				{
				remoteId = (*msvEntrySelection)[i];
				break;
				}
			}
		else if ((iEntryName.CompareF(iEntry->Entry().iDetails) == 0))
			{
			remoteId = (*msvEntrySelection)[i];
			break;
			}
		}
		delete msvEntrySelection;
	return remoteId;	
	}

void CTestImapSyncManager::SetSubscriptionL(const TDesC& aName)
	{
	TBuf8<128> parameter;
	TMsvId id = FindFolderL(aName);
	CMsvEntrySelection*	msvEntrySelection = new (ELeave) CMsvEntrySelection;
	msvEntrySelection->Reset();
	msvEntrySelection->AppendL(id);
	iImapServerMtm->StartCommandL(*msvEntrySelection, KIMAP4MTMLocalSubscribe, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	TBool sub = ((TMsvEmailEntry)iEntry->Entry()).LocalSubscription();
	delete msvEntrySelection;
	}

void CTestImapSyncManager::SendMessageL()
	{
	//Create a message in the local OUTBOX which can be send to server.
	iTestUtils->CreateMessageL(_L("c:\\msgtest\\testdata\\t_unit_imapsyncmanager_test_9.txt"),iServiceId,KMsvGlobalOutBoxIndexEntryId );
	// Send a Message to remote INBOX
	CMsvEntrySelection* sendSel = new(ELeave) CMsvEntrySelection();
	iEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId);
	iEntry->GetChildren(*sendSel);
	TMsvId	id0 = FindFolderL(_L("INBOX"));
	// copy message to remote server inbox
	iImapServerMtm->MoveFromLocalL(*sendSel, id0, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	delete sendSel;
	}

void CTestImapSyncManager::TearDownL()
	{
	delete iSel;
	delete iImapSettings1;
	delete iImapSettings;
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	delete iTestUtils;
	iTestUtils = NULL;
	}
	
