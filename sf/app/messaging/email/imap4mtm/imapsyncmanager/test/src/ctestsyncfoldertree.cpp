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

#include "ctestsyncfoldertree.h"
#include "cactivewaiter.h"
#include "moutputstream.h"

CTestSyncFolderTree::CTestSyncFolderTree()
	:iActiveWaiter(NULL){
	}

CTestSyncFolderTree::~CTestSyncFolderTree()
	{
	
	}

void CTestSyncFolderTree::TestInboxNameL()
	{
	INFO_PRINTF1(_L("TestInboxNameL - Start"));

	iActiveWaiter = new(ELeave)CActiveWaiter();
	ConnectToRemoteServerL();

	TBuf8<128> parameter;
	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );

	INFO_PRINTF1(_L("TestInboxNameL - Check Inbox exists"));
	TMsvId id = FindFolderL(_L("Inbox"), ETrue);
	ASSERT_TRUE(id != KErrNotFound);

	INFO_PRINTF1(_L("TestInboxNameL - Check INBOX does not exist"));
	id = FindFolderL(_L("INBOX"), ETrue);
	ASSERT_TRUE(id == KErrNotFound);

	DisconnectL();
	delete iActiveWaiter;

	INFO_PRINTF1(_L("TestInboxNameL - Complete"));
	}


void CTestSyncFolderTree::TestsSyncFolderL()
	{
	iActiveWaiter = new(ELeave)CActiveWaiter();
	
	ConnectToRemoteServerL();
	
	CreateFolderL(_L("Test1"),iServiceId);
	SetSubscriptionL(_L("Test1"));

	CreateFolderL(_L("Test2"),iServiceId);
	SetSubscriptionL(_L("Test2"));

	TBuf8<128> parameter;

	iSel->Reset();
	iSel->AppendL(iServiceId);

	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );

//	Check whether they have been remotely subscribed
	TMsvId id =	FindFolderL(_L("Test1"));

	iEntry->SetEntry(id);
	TBool sub = ((TMsvEmailEntry)iEntry->Entry()).Subscribed();
	
	id = FindFolderL(_L("Test2"));
	iEntry->SetEntry(id);
	sub = ((TMsvEmailEntry)iEntry->Entry()).Subscribed();
	

	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	iEntry->SetEntry(iServiceId);
	/*iSel->Reset();
	iSel->AppendL(serviceId);
	imapServerMtm->StartCommandL(*iSel, KIMAP4MTMSyncTree, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();*/
	CMsvEntrySelection* msvEntrySelection = new (ELeave) CMsvEntrySelection;
	iEntry->GetChildren(*msvEntrySelection);
	TInt count = msvEntrySelection->Count();
	delete msvEntrySelection;

	//TODO: Check whether folders have been created and synced 

	/* id =	FindFolderL(_L("Test1"));
		if(id == KErrNotFound)
		{
		CreateFolderL(_L("Test1"),serviceId);
		id = FindFolderL(_L("Test1"));
		}
	CreateFolderL(_L("Test1child"),id);*/
	DisconnectL();
	delete iActiveWaiter;
	}
 
void CTestSyncFolderTree::TestsDeleteL()
	{
	iActiveWaiter = new(ELeave)CActiveWaiter();
	ConnectToRemoteServerL();
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	CreateFolderL(_L("Test7"),iServiceId);
	TBuf8<128> parameter;
	DeleteFolderL(_L("Test7"));
	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMSyncTree , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	DisconnectL();
	ConnectToRemoteServerL();
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	 TMsvId id = FindFolderL(_L("Test7"));
	
	if(id==KErrNotFound )
		{
			// removed
		}
	DisconnectL();
	}
	
void CTestSyncFolderTree::TestsMessageSyncLimitL()
	{
	TBuf8<128> parameter;
	iActiveWaiter = new(ELeave)CActiveWaiter();

	ConnectToRemoteServerL();
	//Send some messages to remote server
	for(TInt i = 0; i <2; ++i)
		{
		SendMessageL();
		}

	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	}


void CTestSyncFolderTree::TestsMessageSyncNewLimitL()
	{
	TBuf8<128> parameter;
	iActiveWaiter = new(ELeave)CActiveWaiter();
	ChangeSyncLimitL(5);
	ConnectToRemoteServerL();
	//Send some messages to remote server
	for(TInt i = 0; i <5; ++i)
		{
		SendMessageL();
		}

	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync, parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	}

	
void CTestSyncFolderTree::TestsNewMessageSyncL()
	{
	TBuf8<128> parameter;
	iActiveWaiter = new(ELeave)CActiveWaiter();
	// connect and sync 
	ConnectToRemoteServerL();
	iSel->Reset();
	iSel->AppendL(iServiceId);
	for(TInt i = 0; i <2; ++i)
		{
		SendMessageL();
		}
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	}

void CTestSyncFolderTree::TestRenameFolderL()
	{
	TBuf8<128> parameter;
	iActiveWaiter = new(ELeave)CActiveWaiter();
	ConnectToRemoteServerL();
	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	TMsvId id = FindFolderL(_L("Test1"));
	if(id == KErrNotFound)
		{
		CreateFolderL(_L("Test1"),iServiceId);
		id = FindFolderL(_L("Test1"));
		}
	TImap4RenameFolder cmd;
	cmd.iNewName = _L("Test1Renamed");
	TPckgC<TImap4RenameFolder> package(cmd);
	iSel->Reset();
	iSel->AppendL(id);
	iImapServerMtm->StartCommandL(*iSel,KIMAP4MTMRenameFolder,package,iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iSel->Reset();
	iSel->AppendL(iServiceId);
	iImapServerMtm->StartCommandL(*iSel, KIMAP4MTMFullSync , parameter, iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	id = FindFolderL(_L("Test1Renamed"));
		if(id == KErrNotFound)
			{
				// Rename Failed
			}
	DisconnectL();	
	}
	

CTestSuite* CTestSyncFolderTree::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestInboxNameL);
	ADD_ASYNC_TEST_STEP(TestsSyncFolderL);
	ADD_ASYNC_TEST_STEP(TestsDeleteL);
	ADD_ASYNC_TEST_STEP(TestsMessageSyncLimitL);
	ADD_ASYNC_TEST_STEP(TestsMessageSyncNewLimitL);
	ADD_ASYNC_TEST_STEP(TestsNewMessageSyncL);
	ADD_ASYNC_TEST_STEP(TestRenameFolderL);
	END_SUITE;
	}
