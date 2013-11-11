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

#include "cactivewaiter.h"
#include "ctestimapservermtmbase.h"
#include "ctestimapservermtmcreate.h"

#include <miuthdr.h>

CTestImapServerMtmCreate::CTestImapServerMtmCreate( )
	{
	
	}
	
CTestImapServerMtmCreate::~CTestImapServerMtmCreate( )
	{
	
	}
	
void CTestImapServerMtmCreate::TestCreateFolderWhenNotOnlineL( )
	{
	INFO_PRINTF1( _L( "TestCreateFolderWhenNotOnlineL" ) );
	
	// No Spoofserver needed
	DoFolderCreateL( (TMsvId) iImapAccount.iImapService, _L( "testfolder" ) );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );

	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EConnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::EDisconnected );
	ASSERT_EQUALS( progress.iGenericProgress.iImap4SubStateProgress, TImap4GenericProgress::EDisconnected );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, KErrDisconnected );
	ASSERT_EQUALS( progress.iGenericProgress.iReturnedMsvId, ( TMsvId ) 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iTotalSize, 0 );

	ASSERT_EQUALS( progress.iSyncProgress.iType, EImap4SyncProgressType );
	ASSERT_EQUALS( progress.iSyncProgress.iState, TImap4SyncProgress::EIdle );
	ASSERT_EQUALS( progress.iSyncProgress.iFoldersToDo, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iFoldersDone, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iHeadersFetched, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iOrphanedFolders, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iNewFolders, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iOrphanedMessages, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iRemoteMessagesDeleteTagged, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iMessagesFetchedOK, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iMessagePartsFetchedOK, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iMessagePartsNotFound, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iFoldersNotFound, 0 );
	ASSERT_EQUALS( progress.iSyncProgress.iErrorCode, 0 );	
	
	INFO_PRINTF1( _L( "TestCreateFolderWhenNotOnlineL completed" ) );
	}
	
void CTestImapServerMtmCreate::TestCreateFolderWhenOnlineL( )
	{
	INFO_PRINTF1( _L( "TestCreateFolderWhenOnlineL" ) );
	DoStartSpoofServerL( _L( "spoofserver_connect_when_connected_script" ) );
	
	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoFolderCreateL( (TMsvId) iImapAccount.iImapService, _L( "testfolder" ) );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	INFO_PRINTF1( _L( "TestCreateFolderWhenOnlineL completed" ) );
	}
	
void CTestImapServerMtmCreate::TestCreateMailboxWhenNotOnlineL( )
	{
	INFO_PRINTF1( _L( "TestCreateMailboxWhenNotOnlineL" ) );
	
	INFO_PRINTF1( _L( "TestCreateMailboxWhenNotOnlineL completed" ) );
	}
	
void CTestImapServerMtmCreate::TestCreateMailboxWhenOnlineL( )
	{
	INFO_PRINTF1( _L( "TestCreateMailboxWhenOnlineL" ) );
	
	INFO_PRINTF1( _L( "TestCreateMailboxWhenOnlineL completed" ) );
	}
	
CTestSuite* CTestImapServerMtmCreate::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
//	ADD_ASYNC_TEST_STEP( TestCreateFolderWhenNotOnlineL );
	ADD_ASYNC_TEST_STEP( TestCreateFolderWhenOnlineL );
//	ADD_ASYNC_TEST_STEP( TestCreateMailboxWhenNotOnlineL );
//	ADD_ASYNC_TEST_STEP( TestCreateMailboxWhenOnlineL );
	END_SUITE;
	}

void CTestImapServerMtmCreate::DoFolderCreateL( TMsvId /*aParent*/, const TDesC& /*aName*/ )
	{
	// TODO
	
/*	// create the msventry
	TMsvEmailEntry newEntry;
	newEntry.iType = KUidMsvFolderEntry;
	newEntry.iMtm = KUidMsgTypeIMAP4;
	newEntry.iServiceId = iImapAccount.iImapService;
	newEntry.iSize = 0;
	newEntry.iDetails.Set( aName );
	newEntry.SetMailbox( ETrue );
	newEntry.SetValidUID( EFalse );
	newEntry.SetComplete( ETrue );
	newEntry.SetVisible( ETrue );
//	User::LeaveIfError( entry->CreateEntry( newEntry ) );
	
	// tell the IMAP server about it
	iImapServerMtm->CreateL( newEntry, iActiveWaiter->iStatus );
	iActiveWaiter->WaitActive( );
*/
	}
