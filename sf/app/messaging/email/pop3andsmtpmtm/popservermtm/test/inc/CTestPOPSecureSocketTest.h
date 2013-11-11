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

#ifndef CTESTPOPSECURESOCKETTEST_H
#define CTESTPOPSECURESOCKETTEST_H


#include <test/tefunit.h>
#include "emailtestutils.h"
#include "scripttestutils.h"
#include "miutmsg.h"
#include <popsmtm.h>
#include <cemailaccounts.h>
#include "cactivewaiter.h"

//Different type of POP SSL-TLS test
enum TypeOfTest
	{
	EpopApopTLSok = 0, 
	EpopTlsInvalidUser,
	};


class CTestPOPSecureSocketTest : public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
	
public:
	CTestPOPSecureSocketTest(); 
	virtual ~CTestPOPSecureSocketTest();
	void SetupL();
	void TearDownL();
	TPtr16 ReadServerIp();
	void TestRunPopSSLTLSTestsL();
	void TestRunPopSSLTLSTestsL2();
	void SetSettingsL(TypeOfTest aType,TPtr16 aServerIp);
	static CTestSuite*  CreateSuiteL(const TDesC& aName);
	void ConnectToRemoteServerL(TypeOfTest aType);
	void DisconnectL();
private:
	CEmailTestUtils* iTestUtils; 
	CImPop3Settings* isettings ;
	CActiveScheduler* ischeduler;
	RFs itheFs;	
	TMsvId	ipop3Service;
	CBaseServerMtm* iPopServerMtm;
	CActiveWaiter* iActiveWaiter;
	CMsvEntrySelection* iSel;
	TPop3Cmds iCommand;
	TPopAccount iPopAccount;
	TInt itestError;
	TInt ifailed;
	//HBufC8* iServerIp;
	//TPtr8 ibufferPtr;
	
	
	};

#endif // CTestPOPSecureSocketTest.h