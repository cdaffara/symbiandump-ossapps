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

#include "cactivewaiter.h"
#include "ctestimapservermtmconnectandsync.h"

CTestImapServerMtmConnectAndSync::CTestImapServerMtmConnectAndSync( )
	{
	
	}
	
CTestImapServerMtmConnectAndSync::~CTestImapServerMtmConnectAndSync( )
	{
	
	}
	
void CTestImapServerMtmConnectAndSync::TestConnectAndSyncNoEmailL( )
	{
	INFO_PRINTF1( _L( "TestConnectAndSyncNoEmailL" ) );
	
	DoStartSpoofServerL( _L( "spoofserver_connect_and_sync_no_email_script" ) );

	DoConnectAndSyncL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoWaitForBackGroundSyncL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
/*	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EConnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::ESelecting );
	ASSERT_EQUALS( progress.iGenericProgress.iImap4SubStateProgress, TImap4GenericProgress::EIdle );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsToDo, 1 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iReturnedMsvId, ( TMsvId ) 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iTotalSize, 0 );

	ASSERT_EQUALS( progress.iSyncProgress.iType, EImap4SyncProgressType );
	ASSERT_EQUALS( progress.iSyncProgress.iState, TImap4SyncProgress::ESyncInbox );
	ASSERT_EQUALS( progress.iSyncProgress.iFoldersToDo, 1 );
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
*/
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	INFO_PRINTF1( _L( "TestConnectAndSyncNoEmailL completed" ) );
	}
	
void CTestImapServerMtmConnectAndSync::TestConnectAndSyncWithOneEmailL( )
	{
	INFO_PRINTF1( _L( "TestConnectAndSyncWithOneEmailL" ) );
	
	INFO_PRINTF1( _L( "TestConnectAndSyncWithOneEmailL completed" ) );
	}

void CTestImapServerMtmConnectAndSync::TestConnectAndSyncWithTenEmailsL( )
	{
	INFO_PRINTF1( _L( "TestConnectAndSyncWithTenEmailsL" ) );
	
	INFO_PRINTF1( _L( "TestConnectAndSyncWithTenEmailsL completed" ) );
	}
	
void CTestImapServerMtmConnectAndSync::TestConnectAndSyncWithBadLoginL( )
	{
	INFO_PRINTF1( _L( "TestConnectAndSyncWithBadLoginL" ) );
	
	DoStartSpoofServerL( _L( "spoofserver_connect_and_sync_with_bad_login_script" ) );

	DoConnectAndSyncL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrImapBadLogon );

	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EConnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::EDisconnected );
	ASSERT_EQUALS( progress.iGenericProgress.iImap4SubStateProgress, TImap4GenericProgress::EIdle );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, KErrImapBadLogon );
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
	
	INFO_PRINTF1( _L( "TestConnectAndSyncWithBadLoginL completed" ) );
	}
	
void CTestImapServerMtmConnectAndSync::TestConnectAndSyncWithBadPasswordL( )
	{
	INFO_PRINTF1( _L( "TestConnectAndSyncWithBadPasswordL" ) );
	
	DoStartSpoofServerL( _L( "spoofserver_connect_and_sync_with_bad_password_script" ) );

	DoConnectAndSyncL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrImapBadLogon );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EConnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::EDisconnected );
	ASSERT_EQUALS( progress.iGenericProgress.iImap4SubStateProgress, TImap4GenericProgress::EIdle );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, KErrImapBadLogon );
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
	
	INFO_PRINTF1( _L( "TestConnectAndSyncWithBadPasswordL completed" ) );
	}
	
CTestSuite* CTestImapServerMtmConnectAndSync::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
//	ADD_ASYNC_TEST_STEP( TestConnectAndSyncWithBadLoginL );
//	ADD_ASYNC_TEST_STEP( TestConnectAndSyncWithBadPasswordL );
	ADD_ASYNC_TEST_STEP( TestConnectAndSyncNoEmailL );
//	ADD_ASYNC_TEST_STEP( TestConnectAndSyncWithOneEmailL );
//	ADD_ASYNC_TEST_STEP( TestConnectAndSyncWithTenEmailsL );
	END_SUITE;
	}
	
void CTestImapServerMtmConnectAndSync::DoConnectAndSyncL( )
	{
	iSelection->Reset( );
	iSelection->AppendL( iImapAccount.iImapService );
	
	iImapServerMtm->StartCommandL( *iSelection, 
					    		   KIMAP4MTMConnectAndSynchronise, 
								   KNullDesC8, 
								   iActiveWaiter->iStatus );

	iActiveWaiter->WaitActive( );

	}

void CTestImapServerMtmConnectAndSync::DoWaitForBackGroundSyncL( )
	{
	
	// TODO: Replace with loop that waits for completion (checks progress)	
	iSelection->Reset( );
	iImapServerMtm->StartCommandL( *iSelection,
								   KIMAP4MTMWaitForBackground,
								   KNullDesC8,
								   iActiveWaiter->iStatus );
	
	iActiveWaiter->WaitActive( );	
	}
