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
#include "ctestimapservermtmpopulate.h"


CTestImapServerMtmPopulate::CTestImapServerMtmPopulate( )
	{
	
	}
	
CTestImapServerMtmPopulate::~CTestImapServerMtmPopulate( )
	{
	
	}
	
void CTestImapServerMtmPopulate::TestPopulateL( )
	{
	INFO_PRINTF1( _L( "TestPopulateL" ) );
	
	DoConnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	DoPopulateL( );	// TODO get correct ID
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	// check progress
	const TDesC8& progBuf = iImapServerMtm->Progress( );
	
	TImap4CompoundProgress temp;    
	TPckgC<TImap4CompoundProgress> paramPack( temp );

	paramPack.Set( progBuf );
	TImap4CompoundProgress progress = paramPack( );
	
	DoDisconnectL( );
	ASSERT_EQUALS( iActiveWaiter->iStatus.Int( ), KErrNone );
	
	INFO_PRINTF1( _L( "TestPopulateL complete" ) );
	}
	
CTestSuite* CTestImapServerMtmPopulate::CreateSuiteL( const TDesC& aName )
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP( TestPopulateL );
	END_SUITE;
	}

void CTestImapServerMtmPopulate::DoPopulateL( )
	{
	iSelection->Reset( );
	iSelection->AppendL( iImapAccount.iImapService );
	
	iImapServerMtm->StartCommandL( *iSelection, 
					    		   KIMAP4MTMPopulate, 
								   KNullDesC8, 
								   iActiveWaiter->iStatus );

	iActiveWaiter->WaitActive( );

	}
