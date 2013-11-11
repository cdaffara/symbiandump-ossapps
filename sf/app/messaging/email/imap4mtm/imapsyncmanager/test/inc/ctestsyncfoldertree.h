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

#ifndef CTESTSYNCFOLDERTREE_H
#define CTESTSYNCFOLDERTREE_H

#include "ctestimapsyncmanager.h"

// Forward Declarations
class CActiveWaiter;

/**
@internalTechnology
@prototype
*/
class CTestSyncFolderTree : public CTestImapSyncManager
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestSyncFolderTree(/*MImapTestEventHandler* aOwner*/);
	virtual ~CTestSyncFolderTree();
	void TestInboxNameL();
	void TestsSyncFolderL();
	void TestsDeleteL();
	void TestsMessageSyncLimitL();
	void TestsNewMessageSyncL();
	void TestsMessageSyncNewLimitL();
	void TestRenameFolderL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
private:
	CActiveWaiter* iActiveWaiter;
	};
	 
#endif // CTESTSYNCFOLDERTREE_H
