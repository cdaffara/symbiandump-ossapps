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

#ifndef CTESTIMAPLIST_H
#define CTESTIMAPLIST_H

#include <test/tefunit.h>
#include "tmessageflaginfo.h"
#include "cimaplistfolderinfo.h"

// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;
class CImapFolderInfo;

/**
This test fixture tests CImapList.
@internalTechnology
@prototype
*/
class CTestImapList : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapList();
	virtual ~CTestImapList();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestListWithFlagsL();
	void TestListWithNoFlagsL();
	void TestListWithEmptyDataL();
	void TestListWithSlashL();
	void TestListWithDotL();
	void TestListWithInverterL();

//	void ParseLine(/*CImapListFolderInfo* aFolderInfo*/);	
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	CImapFolderInfo* folderInfo;
	};
	
#endif //CTESTIMAPLIST_H
