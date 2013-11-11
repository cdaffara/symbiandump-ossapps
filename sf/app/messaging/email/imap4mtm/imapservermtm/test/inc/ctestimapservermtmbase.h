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

#ifndef __CTESTIMAPSERVERMTMBASE_H__
#define __CTESTIMAPSERVERMTMBASE_H__

#include <cemailaccounts.h>
#include "cspoofserver.h"
#include "emailtestutils.h"
#include <test/tefunit.h>
#include "cimapsettings.h"

class CActiveWaiter;

// Note that all test suites must begin with "CTest"
class CTestImapServerMtmBase : public CActiveTestFixture, 
						 	   public MImapTestEventHandler
	{
public:
	virtual ~CTestImapServerMtmBase( );

	// SetUp and TearDown code (optional)
	virtual void SetupL( );
	virtual void TearDownL( );

protected:
	CTestImapServerMtmBase( );

	void DoConnectL( );
	void DoDisconnectL( );
	void DoCreateImapAccountL( );
	void DoStartSpoofServerL( const TDesC& aScriptSection );
	void DoLoadDriversL( );
private:
	// from MImapTestEventHandler
	virtual void TestComplete( TInt aErrorCode );
protected:
	CEmailTestUtils*	iTestUtils;
	CBaseServerMtm*		iImapServerMtm;		// No ownership
	CSpoofServer* 		iSpoofServer;
	CImapSettings* iImapSettings;
	CEmailAccounts* 	iAccounts;
	TImapAccount 		iImapAccount;
	
	CActiveWaiter*		iActiveWaiter;
	TMsvId iServiceId ;
	CMsvServerEntry* iEntry;
	
	CMsvEntrySelection*	iSelection;
	
	RTest iTest;
	};

#endif //__CTESTIMAPSERVERMTMBASE_H__
