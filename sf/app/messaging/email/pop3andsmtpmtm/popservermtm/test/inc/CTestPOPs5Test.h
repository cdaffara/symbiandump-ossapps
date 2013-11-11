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

#ifndef CTESTPOPS5TEST_H
#define CTESTPOPS5TEST_H


#include <test/tefunit.h>
#include "emailtestutils.h"
#include <cemailaccounts.h>
#include "cactivewaiter.h"


class CTestPOPs5Test : public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
	
public:
	CTestPOPs5Test(); 
	virtual ~CTestPOPs5Test();
	void SetupL();
	void TearDownL();
	void TestRunPops5CopyToLocalLTestsL();
	static CTestSuite*  CreateSuiteL(const TDesC& aName);
	void ConnectToRemoteServerL();
	void DisconnectL();
private:
	CEmailTestUtils* iTestUtils; 
	CImPop3Settings* isettings ;
	TMsvId	ipop3Service;
	CBaseServerMtm* iPopServerMtm;
	CActiveWaiter* iActiveWaiter;
	CMsvEntrySelection* iSel;
	TPop3Cmds iCommand;
	TPopAccount iPopAccount;
	CMsvEntrySelection* iSelection;
	
	};

#endif // CTestPOPs5Test.h