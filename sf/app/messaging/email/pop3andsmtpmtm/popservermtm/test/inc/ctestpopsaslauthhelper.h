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

#ifndef CTESTPOPSASLAUTHHELPER_H
#define CTESTPOPSASLAUTHHELPER_H

//#include <tefunit.h>
//#include "scripttestutils.h"
#include "emailtestutils.h"
#include <cemailaccounts.h>
#include <popsmtm.h>
#include <iapprefs.h>
#include "miutmsg.h"
#include "cpopsaslauthhelper.h"
#include "cactivewaiter.h"


class CTestPopSaslAuthHelper : public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	CTestPopSaslAuthHelper(); 
	virtual ~CTestPopSaslAuthHelper();
	void SetupL();
	void TearDownL();
	
	void CreatePopSaslAuthMechanismL();
	void CreatePopTestAccountL();
	void ConnectToRemoteServerL();
	void TestForAuthCramMd5L();
	void TestForAuthLoginL();
	void TestForAuthPlainL();
	void DisconnectL();
	
	static CTestSuite*  CreateSuiteL(const TDesC& aName);

private:
	CEmailTestUtils* 	iTestUtils; 
	CEmailAccounts* 	iEmailAccount;
	CImPop3Settings* 	iPopSettings ;
	TMsvId				iPop3Service;
	TPop3Cmds 			iCommand;
	TPopAccount 		iPop3AccountId;
	CActiveWaiter* 		iActiveWaiter;
	CBaseServerMtm* 	iPopServerMtm;
	CMsvEntrySelection* iEntrySelection;
	
	CPopAuthMechanismHelper* iTestPopAuthMechanism;
	HBufC8* 			iResponseBuffer; 
	//TBuf8<KImMailMaxBufferSize>	iResponseBuffer;
	};

#endif // CTESTPOPSASLAUTHHELPER_H