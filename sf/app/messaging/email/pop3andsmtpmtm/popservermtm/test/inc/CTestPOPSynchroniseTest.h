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

#ifndef CTESTPOPSSYNCHROISETEST_H
#define CTESTPOPSSYNCHROISETEST_H


#include <test/tefunit.h>
#include "emailtestutils.h"
#include "scripttestutils.h"
#include "miutmsg.h"
#include <popsmtm.h>
#include <cemailaccounts.h>
#include "cactivewaiter.h"


#include "CTestPOPSynchroniseTest.h"
#include <iapprefs.h>

class CTestPOPSynchroniseTest : public  CActiveTestFixture
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
	
public:
	CTestPOPSynchroniseTest(); 
	virtual ~CTestPOPSynchroniseTest();
	void SetupL();
	void TearDownL();
	
	void TestPop3SynchroniseTestL();
	void ChangeSynchronisationLimitL();
	void ChangeDownloadModeL();
	
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
	TInt itestError;
	TInt ifailed;
	CMsvEntrySelection* iSelection;
	TRequestStatus*		iReportStatus;
	TInt				iError;
	HBufC*				iDetails;
	TPopAccount ipop3AccountId;
	CEmailAccounts* iaccount;
	};

#endif // CTestPOPSynchroniseTest.h