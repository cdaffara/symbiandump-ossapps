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

#ifndef CTESTIMAPAUTHENTICATE_H
#define CTESTIMAPAUTHENTICATE_H

#include <test/tefunit.h>
#include <cemailaccounts.h>
#include "emailtestutils.h"
#include "cimapsession.h"
#include "cimapsettings.h"


// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;
class CImapAuthMechanismHelper;
class CMsvServer;
/**
This test fixture tests CImapList.
@internalTechnology
@prototype
*/
class CTestImapAuthenticate : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapAuthenticate();
	virtual ~CTestImapAuthenticate();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestAuthwithCramMd5L();
	void TestAuthwithPlainL();
	void TestAuthwithLoginL();
	void DoCreateImapAccountL( );
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	CImapSettings*  iImapSettings;
	CMsvServerEntry* iEntry;
	CBaseServerMtm*		iImapServerMtm;
	CMsvServer*		 iMsvServer;
	CImapMailStore*  iImapMailStore;
	TMsvId iServiceId ;
	CEmailAccounts* 	iAccounts;
	TImapAccount 		iImapAccount;
	
	
	};
	
#endif //CTestImapAuthenticate_H
