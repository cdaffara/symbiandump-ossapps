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
#include "ctestimapservermtmlocalsubscribe.h"


CTestImapServerMtmLocalSubscribe::CTestImapServerMtmLocalSubscribe( )
	{
	
	}
	
CTestImapServerMtmLocalSubscribe::~CTestImapServerMtmLocalSubscribe( )
	{
	
	}
	
void CTestImapServerMtmLocalSubscribe::TestLocalSubscribeL( )
	{
	INFO_PRINTF1( _L( "TestLocalSubscribeL" ) );
	
	DoConnectL( );
	ASSERT_EQUALS(iActiveWaiter->iStatus.Int( ), KErrNone );
	
	// DoLocalSubscribe( );
	iSelection->Reset( );
	iSelection->AppendL( iImapAccount.iImapService );
		
	iImapServerMtm->StartCommandL( *iSelection, 
					    		   KIMAP4MTMConnect, 
								   KNullDesC8, 
								   iActiveWaiter->iStatus );

	iActiveWaiter->WaitActive( );
	
	DoDisconnectL( );
	ASSERT_EQUALS(iActiveWaiter->iStatus.Int( ), KErrNone );
	
	INFO_PRINTF1( _L( "TestLocalSubscribeL complete" ) );
	}
	
CTestSuite* CTestImapServerMtmLocalSubscribe::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP( TestLocalSubscribeL );
	END_SUITE;
	}
