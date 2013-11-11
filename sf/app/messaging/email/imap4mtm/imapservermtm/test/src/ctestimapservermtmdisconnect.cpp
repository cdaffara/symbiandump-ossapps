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
#include "ctestimapservermtmdisconnect.h"

CTestImapServerMtmDisconnect::CTestImapServerMtmDisconnect( )
	{
	
	}
	
CTestImapServerMtmDisconnect::~CTestImapServerMtmDisconnect( )
	{
	
	}
	
void CTestImapServerMtmDisconnect::TestDisconnectWhenConnectedL( )
	{
	INFO_PRINTF1( _L( "TestDisConnectWhenConnectedL" ) );
	
	DoStartSpoofServerL( _L( "spoofserver_disconnect_script" ) );

	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	// Connect progress is checked in connect test cases
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );	
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );   
	
	ASSERT_EQUALS( progress.iGenericProgress.iType, EImap4GenericProgressType );
	ASSERT_EQUALS( progress.iGenericProgress.iOperation, TImap4GenericProgress::EDisconnect );
	ASSERT_EQUALS( progress.iGenericProgress.iState, TImap4GenericProgress::EDisconnected );
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
/*
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
*/	
	INFO_PRINTF1( _L( "TestDisConnectWhenConnectedL completed" ) );
	}

void CTestImapServerMtmDisconnect::TestDisconnectWhenNotConnectedL( )
	{
	INFO_PRINTF1( _L( "TestDisConnectWhenNotConnectedL" ) );
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrDisconnected );
	
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
	ASSERT_EQUALS( progress.iGenericProgress.iErrorCode, 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iReturnedMsvId, ( TMsvId ) 0 );
	ASSERT_EQUALS( progress.iGenericProgress.iTotalSize, 0 );
/*
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
*/	
	INFO_PRINTF1( _L( "TestDisConnectWhenNotConnectedL completed" ) );
	}
	
CTestSuite* CTestImapServerMtmDisconnect::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP( TestDisconnectWhenConnectedL );
	ADD_ASYNC_TEST_STEP( TestDisconnectWhenNotConnectedL );
	END_SUITE;
	}
