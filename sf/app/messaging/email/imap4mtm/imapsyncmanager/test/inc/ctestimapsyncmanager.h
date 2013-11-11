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
// ctestimapsynmanager.h
// 
//

#ifndef CTESTIMAPSYNCMANAGER_H
#define CTESTIMAPSYNCMANAGER_H
#include "cimapsettings.h"
#include <cemailaccounts.h>
#include <iapprefs.h>
#include <smtpset.h>
#include "emailtestutils.h"
#include <miutmsg.h>
#include <test/tefunit.h>
#include "cimap4servermtm.h"
// Forward Declarations
class CActiveWaiter;

/**
This test fixture tests CImapSync's ability to deliver lines and literals to CImapCommand objects.
This is a white-box test, using the simple CImapLogout class to receive lines.
Set a breakpoint in CImapLogout::ParseMessageL() to ensure that the expected lines are being delivered.
TODO: Add some literal tests, when there are commands implemented that use this feature.
@internalTechnology
@prototype
*/
class CTestImapSyncManager : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapSyncManager();
	virtual ~CTestImapSyncManager();
	
	// Tests
	virtual void SetupL( );
	virtual void TearDownL( );
	void ConnectToRemoteServerL();
	void DisconnectL();
	void DeleteFolderL(const TDesC& aName);
	void CreateFolderL(const TDesC& aName,TMsvId entry1);
	TMsvId FindFolderL(const TDesC& aName, TBool aCaseSensitive = EFalse);
	void SetSubscriptionL(const TDesC& aName);

	void ChangeSyncLimitL(TInt limit);
	void SendMessageL();
private:
		CActiveWaiter* iActiveWaiter;
public:

	CImapSettings* iImapSettings;
	CImap4ServerMtm* iImapServerMtm;
	TMsvId iServiceId ;
	CMsvEntrySelection* iSel;
	CMsvServerEntry* iEntry;
	CEmailTestUtils* iTestUtils;
	TImapAccount iImapAccount;
	CImImap4Settings* iImapSettings1;
	};
	 
#endif CTESTIMAPSYNCMANAGER_H
