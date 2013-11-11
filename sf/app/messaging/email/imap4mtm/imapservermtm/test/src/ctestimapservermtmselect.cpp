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
#include "ctestimapservermtmselect.h"


CTestImapServerMtmSelect::CTestImapServerMtmSelect( )
	{
	
	}
	
CTestImapServerMtmSelect::~CTestImapServerMtmSelect( )
	{
	
	}
	
void CTestImapServerMtmSelect::TestSelectSubscribedFolderL( )
	{
	INFO_PRINTF1( _L( "TestSelectSubscribedFolderL" ) );
	
	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoSelectL( NULL );	// TODO get correct ID
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
	
	INFO_PRINTF1( _L( "TestSelectSubscribedFolderL completed" ) );
	}
	
void CTestImapServerMtmSelect::TestSelectSelectedFolderL( )
	{
	INFO_PRINTF1( _L( "TestSelectSelectedFolderL" ) );

	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoSelectL( NULL );		// TODO: Get Correct ID
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

	
	INFO_PRINTF1( _L( "TestSelectSelectedFolderL completed" ) );
	}
	
void CTestImapServerMtmSelect::TestSelectUnSubscribedFolderL( )
	{
	INFO_PRINTF1( _L( "TestSelectUnSubscribedFolderL" ) );

	DoStartSpoofServerL( _L( "spoofserver_connect_script" ) );
	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoSelectL( ( TMsvId ) 123456789 ); // folder ID that does not exist
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrServerBusy );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EConnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::EIdle );
	ASSERT_EQUALS( progress.iGenericProgress.iImap4SubStateProgress, TImap4GenericProgress::EIdle );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iMsgsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iPartsDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesToDo, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iBytesDone, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, 0 );
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
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );

	
	INFO_PRINTF1( _L( "TestSelectUnSubscribedFolderL completed" ) );
	}
	
void CTestImapServerMtmSelect::TestSelectWhenNotConnectedL( )
	{
	INFO_PRINTF1( _L( "TestSelectWhenNotConnectedL" ) );
	
	DoSelectL( NULL );		// TODO: find correct ID
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
		
	INFO_PRINTF1( _L( "TestSelectUnexistingFolderL completed" ) );
	}
	
CTestSuite* CTestImapServerMtmSelect::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
//	ADD_ASYNC_TEST_STEP( TestSelectSubscribedFolderL );
//	ADD_ASYNC_TEST_STEP( TestSelectSelectedFolderL );
	ADD_ASYNC_TEST_STEP( TestSelectUnSubscribedFolderL );
//	ADD_ASYNC_TEST_STEP( TestSelectWhenNotConnectedL );
	END_SUITE;
	
	}

void CTestImapServerMtmSelect::DoSelectL( TMsvId aId)
	{
	iSelection->Reset( );
	iSelection->AppendL( aId );

	iImapServerMtm->StartCommandL( *iSelection, 
					    		   KIMAP4MTMConnect, 
								   KNullDesC8, 
								   iActiveWaiter->iStatus );

	iActiveWaiter->WaitActive( );

	}
